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

#include "SysConf.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"
#include <time.h>
#include <sys/time.h>
#include "Record.h"
#include "rtc_util.h"

#include "SysHandle.h"


extern "C"
{
    #include "dsp.h"
}

#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"

int CHandleXcsPtl::dCtlCapParamQuery(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int ret =0;
    PSTR_DEV_CTL_CAP_QUERY pstrDevInfo = (PSTR_DEV_CTL_CAP_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    DCTL_CONF_INFO strDctlConfInfo;
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    for(i =0; i< MAX_COM_DEV; i++)
    {
        ret = SysConfGet(e_DCTL_CONF, &strDctlConfInfo, i);
        if( 0 > ret)
        {
            XCSPTL_ERROR("dctl get bayonet Ftp Info Conf failed.\n");
            SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
            return -1;
        }
        pstrDevInfo->cComId = strDctlConfInfo.cComId+1;
        pstrDevInfo->cDataBits = strDctlConfInfo.cDatabits;
        pstrDevInfo->cEnable = strDctlConfInfo.cEnable;
        pstrDevInfo->cParity = strDctlConfInfo.cParity;
        pstrDevInfo->cStopBits = strDctlConfInfo.cStopbits;
        pstrDevInfo->uiVehicleType = htonl(strDctlConfInfo.uiVehicleType);
        pstrDevInfo->uiBaudrate = htonl(strDctlConfInfo.uiBaudrate);
        pstrDevInfo->usVehHighSpeed = htons(strDctlConfInfo.usMaxMeasureSpeed);
        pstrDevInfo++;
    }
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + MAX_COM_DEV*sizeof(STR_DEV_CTL_CAP_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_DEV_CTL, CPE_DEV_CTL_CAP_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::dCtlCapParamSet(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int ret =0;
    char cComNum =0;
    STR_DEV_CTL_CAP_SET *pstrDevInfo = (STR_DEV_CTL_CAP_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    cComNum = (pClintSocket->nPackLen - NT_PACK_HEAD_LENGTH)/ sizeof(STR_DEV_CTL_CAP_SET);
    for(i =0; i< MAX_COM_DEV; i++)
    {
        DCTL_CONF_INFO strDctlConfInfo;
        if(pstrDevInfo->cComId > 0)
            strDctlConfInfo.cComId = pstrDevInfo->cComId -1;
        strDctlConfInfo.cDatabits = pstrDevInfo->cDataBits;
        strDctlConfInfo.cEnable = pstrDevInfo->cEnable;
        strDctlConfInfo.cParity = pstrDevInfo->cParity;
        strDctlConfInfo.cStopbits = pstrDevInfo->cStopBits;
        strDctlConfInfo.uiVehicleType = ntohl(pstrDevInfo->uiVehicleType);
        strDctlConfInfo.uiBaudrate = ntohl(pstrDevInfo->uiBaudrate);
        strDctlConfInfo.usMaxMeasureSpeed = ntohs(pstrDevInfo->usVehHighSpeed);
        XCSPTL_INFO("cComNum:%d, dctl comid:%d,vehicle:%d.\n",cComNum, strDctlConfInfo.cComId, strDctlConfInfo.uiVehicleType);
        ret = SysConfSet(e_DCTL_CONF, &strDctlConfInfo, i);
        if( 0 > ret)
        {
            XCSPTL_ERROR("dctl Set param Conf failed.\n");
            SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
            return -1;
        }
        pstrDevInfo++;
    }
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_DEV_CTL_CAP;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send dctl param Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send dctl param Conf ok.\n");
    }
    
    /* 由于外设控制模块的车检器属性是和车检器类型绑定的，所以该模块没有提供接口，需要要再加入 */
    
    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}


