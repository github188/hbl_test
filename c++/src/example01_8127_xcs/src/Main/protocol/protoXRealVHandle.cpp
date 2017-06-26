/******************************************************************************
 * bayonetProHandle.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.04, kj Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include "SysConfCheck.h"
#include "SysConf.h"
#include "SysHandle.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"
#include <time.h>
#include <sys/time.h>
#include "Record.h"
#include "rtc_util.h"
     extern "C" {
#include "dsp.h"
     }
#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"
#include "MmsSendDefine.h"
#include "MmsSend.h"

#include "Ieee1394.h"
#include "nMCtlApi.h"

#include "dsp_service.h"

extern int g_SectionNum;

extern int dspEncParamSet(int iChanId);


int CHandleXcsPtl::realMVideoQuery(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    MMS_INFO strMMSInfo;
    MMS_INFO strTmpMMSInfo;    
    memset(&strMMSInfo, 0, sizeof(MMS_INFO));
    memset(&strTmpMMSInfo, 0, sizeof(MMS_INFO));
    /* 需要调用实时图像模块接口,开始图像连接 */
    PSTR_REAL_VIDEO_QUERY pstrRVideoInfo = (PSTR_REAL_VIDEO_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    PSTR_REAL_VIDEO_RESULT pstrRVideoResult = (PSTR_REAL_VIDEO_RESULT)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    /* 由于返回的数据结构和请求的一模一样，所以直接赋值 */
    *pstrRVideoResult = *pstrRVideoInfo;

    strMMSInfo.cChannel = pstrRVideoInfo->ucChannel - 1;
	/* 若要支持多通道视频，这里需要进行修改 */
    if(0 != strMMSInfo.cChannel )
    {
        XCSPTL_ERROR("mms section channel is not 0\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf),0x1); 
        return 0;
    }
    strMMSInfo.nIp = ntohl(pstrRVideoInfo->ReceiveIpAddr);
    strMMSInfo.usPort = ntohs(pstrRVideoInfo->ReceivePort);
    strMMSInfo.cConnectMode = pstrRVideoInfo->cVideoConnMode;
    strMMSInfo.cActive = pstrRVideoInfo->bActive;
    strMMSInfo.cSplitMode = pstrRVideoInfo->bStream;

    TActivePack activePack;
    activePack.nPackLen = ntohl(pstrRVideoInfo->nPackLen);
    if(activePack.nPackLen <= sizeof(activePack.ucPackBuf))
    {
        memcpy(activePack.ucPackBuf, (unsigned char *)pstrRVideoInfo + sizeof(STR_REAL_VIDEO_QUERY), activePack.nPackLen);
    }
    else
    {
        activePack.nPackLen = 0;
        memset(activePack.ucPackBuf, 0, sizeof(activePack.ucPackBuf));
    }

    XCSPTL_INFO("XCS:channel:%d,ip:0x%x,port:%d,connect:%d,active:%d,packlen:%d,SplitMode:%d\n", strMMSInfo.cChannel, strMMSInfo.nIp,\
    strMMSInfo.usPort, strMMSInfo.cConnectMode, strMMSInfo.cActive, activePack.nPackLen, strMMSInfo.cSplitMode);

    ret = mmsSectionCmp(&strMMSInfo);
    if(ret >=0 )
    {
        syslog(LOG_ERR|LOG_USER,"sec is exist,direct return!\n");
        
        pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_REAL_VIDEO_RESULT);
        syslog(LOG_ERR|LOG_USER, "repeat request mms video ip:0x%x ok!\n", strMMSInfo.nIp);
        SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_VCHANN, CPE_VCHANN_REQ_RTVIDEO, sizeof(STR_REAL_VIDEO_RESULT));
		return 0;
    }

    ret = mmsSectionNum();
    if(MAX_SECTION_NUM == ret)
    {
        /* 由于第一路基本为平台录像，所以修改为最后一路 */
        mmsSectionIdGet(&strTmpMMSInfo, MAX_SECTION_NUM-1);
        ret = MmsStop(strTmpMMSInfo.cChannel, strTmpMMSInfo.nIp, strTmpMMSInfo.usPort,\
        strTmpMMSInfo.cSplitMode, strTmpMMSInfo.cConnectMode);
        if( 0 != ret )
        {
            XCSPTL_ERROR("full video mmsStop failed error:%d\n", ret);
            XCSPTL_ERROR("mmsStart stop full mms error:%d\n", ret);
            
            syslog(LOG_ERR|LOG_USER, "XCS:mmsStart stop full mms failed,ret:%d.reInit mms mode.\n", ret);
            
            SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf),0x1);
            return 0;
        }
        else
        {
            syslog(LOG_ERR|LOG_USER,"sec is reach to max,need shutdown first!\n");
            mmsSectionDel(&strTmpMMSInfo);
            ret = mmsSectionNum();
            if(0 == ret)
            {
                dspEncStaCtl(strTmpMMSInfo.cChannel, 0);
            }
            XCSPTL_INFO("XCS:mmsStart stop full mms ok:%d\n", ret);
        }
        syslog(LOG_ERR|LOG_USER, "XCS:mmsStart stop full mms ok:%d\n", ret);
    }

    ret = mmsSectionAdd(&strMMSInfo);
    if(ret >=0 )
    {
        ret = MmsStart(strMMSInfo.cChannel, strMMSInfo.cActive, strMMSInfo.nIp, strMMSInfo.usPort, \
        strMMSInfo.cSplitMode, strMMSInfo.cConnectMode, &activePack);
        if( 0 != ret )
        {
            pstrRVideoResult->ucConnectOK =0;
            pstrRVideoResult->ucConReason =0x80;
            syslog(LOG_ERR|LOG_USER,"MmsStart failed,need shutdown first!\n");
            mmsSectionDel(&strMMSInfo);
            ret = mmsSectionNum();
            if(0 == ret)
            {
                dspEncStaCtl(strMMSInfo.cChannel, 0);
            }
            XCSPTL_ERROR("mmsStart failed error:%d\n", ret);
            syslog(LOG_ERR|LOG_USER, "mmsStart Ip:0x%x failed ret:%d\n", strMMSInfo.nIp, ret);
            SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf),0x1);
        }
        else
        {
            pstrRVideoResult->ucConnectOK =1;
            pstrRVideoResult->ucConReason =0;
            ret = mmsSectionNum();
            if(1 == ret)
            {
                dspEncStaCtl(strMMSInfo.cChannel, 1);
            }

			CALL_CYC_CMD(NET_DEV_SET_FORCE_KEY_FRAME, strMMSInfo.cChannel);
            pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_REAL_VIDEO_RESULT);
            syslog(LOG_ERR|LOG_USER, "mmsStart ip:0x%x ok!\n", strMMSInfo.nIp);
            SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_VCHANN, CPE_VCHANN_REQ_RTVIDEO, sizeof(STR_REAL_VIDEO_RESULT));
        }
    }
    else
    {
        pstrRVideoResult->ucConnectOK =0;
        pstrRVideoResult->ucConReason =0x84;
        XCSPTL_ERROR("add mms section failed\n");
        syslog(LOG_ERR|LOG_USER, "add mms section ip:0x%x failed\n", strMMSInfo.nIp);
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf),0x1); 
    }
    return 0;
}

int CHandleXcsPtl::realMVideoCut(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    MMS_INFO strMMSInfo;
    memset(&strMMSInfo, 0, sizeof(MMS_INFO));
    /* 需要调用实时图像模块接口，断开图像连接 */
    PSTR_REAL_VIDEO_STOP pstrRVideoStop = (PSTR_REAL_VIDEO_STOP)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    strMMSInfo.cChannel = pstrRVideoStop->ucChannel - 1;
    strMMSInfo.nIp = ntohl(pstrRVideoStop->ReceiveIpAddr);
    strMMSInfo.usPort = ntohs(pstrRVideoStop->ReceivePort);
    strMMSInfo.cConnectMode = pstrRVideoStop->cVideoConnMode;
    strMMSInfo.cActive = pstrRVideoStop->bActive;
    strMMSInfo.cSplitMode = pstrRVideoStop->bStream;

    ret = mmsSectionCmp(&strMMSInfo);
    if(ret >=0 )
    {
        ret = mmsSectionIdGet(&strMMSInfo, ret);
        if(ret >= 0)
        {
            ret = MmsStop(strMMSInfo.cChannel, strMMSInfo.nIp, strMMSInfo.usPort, strMMSInfo.cSplitMode, strMMSInfo.cConnectMode);
            if( 0 != ret )
            {
            	syslog(LOG_ERR|LOG_USER,"XCS:normal mmsStop failed ret:%d,channel:0x%x,splitMode:0x%x,ip:0x%x,port:0x%x\n", ret, strMMSInfo.cChannel,strMMSInfo.cSplitMode,strMMSInfo.nIp, strMMSInfo.usPort);
                XCSPTL_ERROR("XCS:normal mmsStop failed ret:%d,channel:0x%x,splitMode:0x%x,ip:0x%x,port:0x%x\n", ret, strMMSInfo.cChannel,strMMSInfo.cSplitMode,strMMSInfo.nIp, strMMSInfo.usPort);
            	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
            }
            else
            {
                XCSPTL_INFO("normal mmsStop ok ret:%d\n", ret);
                syslog(LOG_ERR|LOG_USER,"normal mmsStop ok!\n");
                mmsSectionDel(&strMMSInfo);
                ret = mmsSectionNum();
                if(0 == ret)
                {
                    dspEncStaCtl(strMMSInfo.cChannel, 0);
                }
            	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
            }
        }
        else
        {
            XCSPTL_ERROR("have no exist mmsinfo id,this info is error\n");
       	    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
        }
    }
    else
    {
        XCSPTL_ERROR("have no exist mmsinfo,this info is error\n");
       	//SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf),0x1);     
       	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    }
    return 0;
}

int CHandleXcsPtl::realMPicParamQuery(XCS_CLIENT *pClintSocket)
{
    
    //PSTR_VCHANN_SNAP_PARAM_QUERY pStrSnapParam = (PSTR_VCHANN_SNAP_PARAM_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_VCHANN_SNAP_PARAM_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_VCHANN, CPE_VCHANN_SNAP_PARAM_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
   
}

int CHandleXcsPtl::realMPicParamSet(XCS_CLIENT * pClintSocket)
{
    //STR_VCHANN_SNAP_PARAM_SET *pStrSnapParam = (STR_VCHANN_SNAP_PARAM_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::realMVideoParamQuery(XCS_CLIENT * pClintSocket)
{
    int i =0;
    VIDEO_CONF_INFO strVideoConfGet;

    /* 图像参数信息查询 */
    PSTR_VCHANN_VIDEO_PARAM_QUERY pStrVideoParam = (PSTR_VCHANN_VIDEO_PARAM_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    for(i =0; i< TOTAL_CHANNEL_NUM; i++)
    {
        int nRet = SysConfGet(e_VIDEO_CONF, &strVideoConfGet, i);
        if( 0 > nRet)
        {
            XCSPTL_ERROR("electPolice get video Conf failed.\n");
            SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
            return -1;
        }
        else
        {
        	pStrVideoParam->usChannel = htons(strVideoConfGet.usChannel+1);
            pStrVideoParam->cEncodeType = strVideoConfGet.cEncodeType;
            pStrVideoParam->cFrameRate = strVideoConfGet.cFrameRate;
            pStrVideoParam->cImageResol = strVideoConfGet.cImageResol;
            pStrVideoParam->cKeyFInterval = strVideoConfGet.cKeyFInterval;

            strVideoConfGet.szContent[63] =0;
            strcpy(pStrVideoParam->szContent, strVideoConfGet.szContent);
            pStrVideoParam->cVBR = strVideoConfGet.cVBR;
            pStrVideoParam->cVQuality = strVideoConfGet.cVQuality;
            pStrVideoParam->uiBitRate = htonl(strVideoConfGet.uiBitRate);
            pStrVideoParam->usChannel = htons(strVideoConfGet.usChannel+1);
            pStrVideoParam->usXPos = htons(strVideoConfGet.usXPos);
            pStrVideoParam->usYPos = htons(strVideoConfGet.usYPos);

            pStrVideoParam->usDebugFrameX  = htons(strVideoConfGet.usDebugFrameX);
            pStrVideoParam->usDebugFrameY  = htons(strVideoConfGet.usDebugFrameY);
            pStrVideoParam->ucDebugFrameEn = strVideoConfGet.ucDebugFrameEn;

            pStrVideoParam->ucTimeDisMode = strVideoConfGet.ucTimeDisMode;
            pStrVideoParam->usTimeXPos = htons(strVideoConfGet.usTimeXPos);
            pStrVideoParam->usTimeYPos  = htons(strVideoConfGet.usTimeYPos);

            
            pStrVideoParam++;
        }
    }
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + TOTAL_CHANNEL_NUM*sizeof(STR_VCHANN_VIDEO_PARAM_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_VCHANN, CPE_VCHANN_VIDEO_PARAM_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::realMVideoParamSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    int iChannelNum =0;
    VIDEO_CONF_INFO strVideoConfSet;
    STR_VCHANN_VIDEO_PARAM_SET *pStrVideoParam = (STR_VCHANN_VIDEO_PARAM_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);


    iChannelNum = (pClintSocket->nPackLen - NT_PACK_HEAD_LENGTH) / sizeof(STR_VCHANN_VIDEO_PARAM_SET);
    iChannelNum = (iChannelNum<=TOTAL_CHANNEL_NUM) ? iChannelNum:TOTAL_CHANNEL_NUM;
    for(int i=0; i< iChannelNum; i++,pStrVideoParam++)
    {
        int iChaSeq = ntohs(pStrVideoParam->usChannel);
        if(iChaSeq > 0)
            iChaSeq--;

        if (!IS_P_CHANNEL_VALID(iChaSeq))
        {
            XCSPTL_INFO("%s:%d Invalid Channel Num [%d] 0x%x.\n",__FUNCTION__,__LINE__,iChaSeq,pStrVideoParam->usChannel);
            continue;
        }
		memset(&strVideoConfSet,0,sizeof(VIDEO_CONF_INFO));
		
        strVideoConfSet.cEncodeType = pStrVideoParam->cEncodeType;
        strVideoConfSet.cFrameRate = pStrVideoParam->cFrameRate;
        strVideoConfSet.cImageResol = pStrVideoParam->cImageResol;
        strVideoConfSet.cKeyFInterval = pStrVideoParam->cKeyFInterval;

        pStrVideoParam->szContent[63] =0;
        strcpy(strVideoConfSet.szContent, pStrVideoParam->szContent);
        strVideoConfSet.cVBR = pStrVideoParam->cVBR;
        strVideoConfSet.cVQuality = pStrVideoParam->cVQuality;
        strVideoConfSet.uiBitRate = ntohl(pStrVideoParam->uiBitRate);
        strVideoConfSet.usChannel = iChaSeq;
        strVideoConfSet.usXPos = ntohs(pStrVideoParam->usXPos);
        strVideoConfSet.usYPos = ntohs(pStrVideoParam->usYPos);
        
        strVideoConfSet.usDebugFrameX = ntohs(pStrVideoParam->usDebugFrameX);
        strVideoConfSet.usDebugFrameY = ntohs(pStrVideoParam->usDebugFrameY);
        strVideoConfSet.ucDebugFrameEn= pStrVideoParam->ucDebugFrameEn;

        strVideoConfSet.ucTimeDisMode = pStrVideoParam->ucTimeDisMode;
        strVideoConfSet.usTimeXPos = ntohs(pStrVideoParam->usTimeXPos);
        strVideoConfSet.usTimeYPos = ntohs(pStrVideoParam->usTimeYPos);
        
        ret = SysConfSet(e_VIDEO_CONF, &strVideoConfSet, iChaSeq);
        if( 0 > ret)
        {
            XCSPTL_ERROR("electPolice set video Conf failed.\n");
            SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
            return -1;
        }
        STR_SYS_APP_MSG strTmpSysMsg;
        memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
        strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
        strTmpSysMsg.uiMsgType = MSG_VIDEO_PARAM_SET;
        strTmpSysMsg.ucChannelId = iChaSeq;
        ret = sendMsgToMainApp(&strTmpSysMsg);
        if(-1 == ret)
        {
            XCSPTL_ERROR("set video Conf failed.\n");
        }
        else
        {
            XCSPTL_INFO("send xcs msg ok.\n");
        }
    }

    
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}
int CHandleXcsPtl::realMDataReportParamSet(XCS_CLIENT * pClintSocket)
{
    int ret = 0;
    STR_DATA_REPORT_SET  *pStrDataReport = (STR_DATA_REPORT_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    STR_SYS_APP_MSG strTmpSysMsg;
    STR_DATA_REPORT_SET   strTmpDataReport;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));

    strTmpDataReport.uiRecvIp   = ntohl(pStrDataReport->uiRecvIp);
    strTmpDataReport.usRecvPort = ntohs(pStrDataReport->usRecvPort);
    strTmpDataReport.uiRecvType = ntohl(pStrDataReport->uiRecvType);
    strTmpDataReport.usConnectType = ntohs(pStrDataReport->usConnectType);
    memcpy(strTmpSysMsg.aucData,&strTmpDataReport,sizeof(STR_DATA_REPORT_SET));

    uiDataUploadIp      = strTmpDataReport.uiRecvIp;
    usDataUploadPort    = strTmpDataReport.usRecvPort;
    
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_DATA_REPORT_PARAM_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("set dataReportParam failed.\n");
    }
    else
    {
        XCSPTL_ERROR("send dataReportParam xcs msg ok.\n");
    }

    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

