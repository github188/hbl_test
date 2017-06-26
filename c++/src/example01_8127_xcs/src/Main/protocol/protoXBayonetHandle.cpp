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
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "SysConf.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"
#include <time.h>
#include <sys/time.h>
#include "Record.h"
#include "rtc_util.h"

extern "C"
{
    #include "dsp.h"
}
     
#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"

#include "SysHandle.h"

int CHandleXcsPtl::bayonetGInfoQuery(XCS_CLIENT *pClintSocket)
{
    /* 1.获取配置信息 */
    PASSPORT_OVERALL_CONF_INFO strCfgInfoGet;
    int nRet = SysConfGet(e_PASSPORT_OVERALL_CONF, &strCfgInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("bayonet get gInfo Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
    }
    /* 2.给客户端赋值 */
    PSTR_BAYONET_GLOBAL_INFO_QUERY pClinetContent = (PSTR_BAYONET_GLOBAL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    strCfgInfoGet.szPassPortName[31] =0;
    strCfgInfoGet.szDirection[31] =0;
    strcpy(pClinetContent->szRoadName, strCfgInfoGet.szPassPortName);
    strcpy(pClinetContent->szDirName, strCfgInfoGet.szDirection);

    XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->szRoadName, pClinetContent->szDirName);

    strCfgInfoGet.szPassPortId[7] =0;
    strCfgInfoGet.szDirId[7] =0;
    strcpy(pClinetContent->szPassPortId, strCfgInfoGet.szPassPortId);
    strcpy(pClinetContent->szDirId, strCfgInfoGet.szDirId);

    XCSPTL_INFO("query ppId:%s,dirId:%s .\n",pClinetContent->szPassPortId, pClinetContent->szDirId);

    pClinetContent->cOSDColor    = strCfgInfoGet.cOSDColor;
    pClinetContent->cValidLanes  = strCfgInfoGet.cValidLanes;
    if(pClinetContent->cValidLanes > g_uiCamLaneSum)
    {
       pClinetContent->cValidLanes = g_uiCamLaneSum; 
    }

    pClinetContent->cVLWorkMode  = strCfgInfoGet.cVLWorkMode;

    pClinetContent->ucLPREn = strCfgInfoGet.ucLPREn;
    
    strCfgInfoGet.szLPRFirstChar[31] =0;
    strcpy(pClinetContent->szLPRFirstChar, strCfgInfoGet.szLPRFirstChar);
    pClinetContent->ucLPRCConfidence = strCfgInfoGet.ucLPRCConfidence;
    pClinetContent->ucVSpeedMeaFlag  = strCfgInfoGet.ucVSpeedMeaFlag;
    pClinetContent->usVSpeedMeaLen   = htons(strCfgInfoGet.usVSpeedMeaLen);

    for(int i =0; i<4; i++ )
    {
        pClinetContent->aucFreqFlashEn[i] = strCfgInfoGet.aucFreqFlashEn[i];
        XCSPTL_INFO("query pp freq flash en:%d\n", strCfgInfoGet.aucFreqFlashEn[i]);

        pClinetContent->aucFlashTrigMode[i] = strCfgInfoGet.aucFlashTrigMode[i];
        XCSPTL_INFO("query pp flash trig mode:%d\n", strCfgInfoGet.aucFlashTrigMode[i]);

        pClinetContent->aucStrobeFlashFre[i] = strCfgInfoGet.aucStrobeFlashFre[i];
        XCSPTL_INFO("query pp strobe flash freq:%d\n", strCfgInfoGet.aucStrobeFlashFre[i]);
    }

    strCfgInfoGet.szPpDevCode[MAX_DEV_CODE_LEN-1] =0;
    strcpy(pClinetContent->szPpDevCode, strCfgInfoGet.szPpDevCode);
    XCSPTL_INFO("query pp dev code:%s\n", pClinetContent->szPpDevCode);

    pClinetContent->ucFillInLSwitchEn = strCfgInfoGet.ucFillInLSwitchEn;
    pClinetContent->ucGrayThresholdVal = strCfgInfoGet.ucGrayThresholdVal;
    pClinetContent->ucCheckSwitchCycle = strCfgInfoGet.ucCheckSwitchCycle;

    strCfgInfoGet.szExPpDevCode[EXT_DEV_CODE_LEN-1] =0;
    strcpy(pClinetContent->szExPpDevCode, strCfgInfoGet.szExPpDevCode);
    XCSPTL_INFO("query pp ext dev code:%s\n", pClinetContent->szExPpDevCode);

    pClinetContent->ucSmartSwitchEn = strCfgInfoGet.ucSmartSwitchEn;
    pClinetContent->ucTimerSwitchEn = strCfgInfoGet.ucTimerSwitchEn;
    pClinetContent->uiFillInLightBeginTime = htonl(strCfgInfoGet.uiFillInLightBeginTime);
    pClinetContent->uiFillInLightLastTime = htonl(strCfgInfoGet.uiFillInLightLastTime);

    
    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_GLOBAL_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_GLOBAL_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::bayonetGInfoSet(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int ret =0;
    PASSPORT_OVERALL_CONF_INFO strCfgInfoSet;

    memset(&strCfgInfoSet, 0, sizeof(strCfgInfoSet));
    /* 2.设置客户端内容 */
    STR_BAYONET_GLOBAL_INFO_SET *pClinetInfo = (STR_BAYONET_GLOBAL_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    pClinetInfo->szRoadName[31] =0;
    pClinetInfo->szDirName[31] =0;
    strcpy(strCfgInfoSet.szPassPortName, pClinetInfo->szRoadName);
    strcpy(strCfgInfoSet.szDirection, pClinetInfo->szDirName);

    pClinetInfo->szPassPortId[7] =0;
    pClinetInfo->szDirId[7] =0;
    strcpy(strCfgInfoSet.szPassPortId, pClinetInfo->szPassPortId);
    strcpy(strCfgInfoSet.szDirId, pClinetInfo->szDirId);

    strCfgInfoSet.cOSDColor   = pClinetInfo->cOSDColor;
    strCfgInfoSet.cValidLanes = pClinetInfo->cValidLanes;
    if(strCfgInfoSet.cValidLanes > g_uiCamLaneSum)
    {
        strCfgInfoSet.cValidLanes = g_uiCamLaneSum;
    }
    
    strCfgInfoSet.cVLWorkMode = pClinetInfo->cVLWorkMode;

    XCSPTL_INFO("set roadName:%s,dirName:%s .\n", pClinetInfo->szRoadName, pClinetInfo->szDirName);
    XCSPTL_INFO("set ppId:%s,dirId:%s .\n", pClinetInfo->szPassPortId, pClinetInfo->szDirId);

    strCfgInfoSet.ucLPREn = pClinetInfo->ucLPREn;
    
    pClinetInfo->szLPRFirstChar[31] =0;
    strcpy(strCfgInfoSet.szLPRFirstChar , pClinetInfo->szLPRFirstChar);
    strCfgInfoSet.ucLPRCConfidence = pClinetInfo->ucLPRCConfidence;
    strCfgInfoSet.ucVSpeedMeaFlag  = pClinetInfo->ucVSpeedMeaFlag;
    strCfgInfoSet.usVSpeedMeaLen   = ntohs(pClinetInfo->usVSpeedMeaLen);

    for(i =0; i<4; i++ )
    {
        strCfgInfoSet.aucFreqFlashEn[i] = pClinetInfo->aucFreqFlashEn[i];
        XCSPTL_INFO("set pp freq flash en:%d\n", strCfgInfoSet.aucFreqFlashEn[i]);
        
        strCfgInfoSet.aucFlashTrigMode[i] = pClinetInfo->aucFlashTrigMode[i];
        XCSPTL_INFO("set pp flash trig mode:%d\n", strCfgInfoSet.aucFlashTrigMode[i]);

        strCfgInfoSet.aucStrobeFlashFre[i] = pClinetInfo->aucStrobeFlashFre[i];
        XCSPTL_INFO("set pp strobe flash freq:%d\n", strCfgInfoSet.aucStrobeFlashFre[i]);
    }

    strcpy(strCfgInfoSet.szPpDevCode, pClinetInfo->szPpDevCode);

    pClinetInfo->szPpDevCode[MAX_DEV_CODE_LEN-1] =0;

    strCfgInfoSet.ucFillInLSwitchEn = pClinetInfo->ucFillInLSwitchEn;
    strCfgInfoSet.ucGrayThresholdVal = pClinetInfo->ucGrayThresholdVal;
    strCfgInfoSet.ucCheckSwitchCycle = pClinetInfo->ucCheckSwitchCycle;

    strcpy(strCfgInfoSet.szExPpDevCode, pClinetInfo->szExPpDevCode);
    pClinetInfo->szExPpDevCode[EXT_DEV_CODE_LEN-1] =0;

	strCfgInfoSet.ucSmartSwitchEn = pClinetInfo->ucSmartSwitchEn;
    strCfgInfoSet.ucTimerSwitchEn = pClinetInfo->ucTimerSwitchEn;
    strCfgInfoSet.uiFillInLightBeginTime = ntohl(pClinetInfo->uiFillInLightBeginTime);
    strCfgInfoSet.uiFillInLightLastTime = ntohl(pClinetInfo->uiFillInLightLastTime);

    ret = SysConfSet(e_PASSPORT_OVERALL_CONF, &strCfgInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("bayonet Set gInfo Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
		return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_BAYONET_GLOBAL_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet global info Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet global channel info msg ok.\n");
    }

    
    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
	return 0;
}

int CHandleXcsPtl::bayonetChannelInfoQuery(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    unsigned int uiCarRoadId =0;
    PORT_VEHICLE_CONF_INFO strChannelInfoGet;
    PSTR_BAYONET_CAR_CHANNEL_INFO_PC_QUERY pPcQueryInfo = (PSTR_BAYONET_CAR_CHANNEL_INFO_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    uiCarRoadId = ntohl(pPcQueryInfo->uiCarRoadId);
    if(uiCarRoadId > 0)
        uiCarRoadId--;
    ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strChannelInfoGet, uiCarRoadId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("bayonet get channel Info Conf failed.lane id is %d\n", uiCarRoadId);
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
    }
    PSTR_BAYONET_CAR_CHANNEL_INFO_QUERY pStrSend = (PSTR_BAYONET_CAR_CHANNEL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    for(int i =0; i<4; i++ )
    {
        pStrSend->ausFlashAddr[i] = htons(strChannelInfoGet.ausFlashAddr[i]);
        XCSPTL_INFO("query pp flash addr is %d\n", strChannelInfoGet.ausFlashAddr[i]);
    }
    pStrSend->cFlashComId = strChannelInfoGet.cComId;
    pStrSend->cLightDevType = strChannelInfoGet.cFlashDevType;
    pStrSend->cLightCtlMode = strChannelInfoGet.cFlashCtlMode;

    strChannelInfoGet.szLaneId[7] =0;
    strChannelInfoGet.szOSDContent[31] =0;
    strcpy(pStrSend->szLaneId, strChannelInfoGet.szLaneId);
    strcpy(pStrSend->szOSDContent, strChannelInfoGet.szOSDContent);

    pStrSend->strRoadArea.usAreaHeight = htons(strChannelInfoGet.strRoadArea.usAreaHeight);
    pStrSend->strRoadArea.usAreaPosX = htons(strChannelInfoGet.strRoadArea.usAreaPosX);
    pStrSend->strRoadArea.usAreaPosY= htons(strChannelInfoGet.strRoadArea.usAreaPosY);    
    pStrSend->strRoadArea.usAreaWidth = htons(strChannelInfoGet.strRoadArea.usAreaWidth);

    pStrSend->strVLArea.usAreaHeight = htons(strChannelInfoGet.strVLArea.usAreaHeight);
    pStrSend->strVLArea.usAreaPosX = htons(strChannelInfoGet.strVLArea.usAreaPosX);
    pStrSend->strVLArea.usAreaPosY= htons(strChannelInfoGet.strVLArea.usAreaPosY);    
    pStrSend->strVLArea.usAreaWidth = htons(strChannelInfoGet.strVLArea.usAreaWidth);

    pStrSend->uiCarRoadId = htonl(uiCarRoadId +1);
    /* 协议版本号还没确定,下一版本完成 */

    pStrSend->usDetectType = htons(strChannelInfoGet.usDetectType);
    pStrSend->usIrreSpeed = htons(strChannelInfoGet.usIrreSpeed);
    pStrSend->usFlashLightCtlType = htons(strChannelInfoGet.usFlashLightCtlType);
    pStrSend->usFlashLightType = htons(strChannelInfoGet.usFlashLightType);
    pStrSend->usMaxSpeed = htons(strChannelInfoGet.usMaxSpeed);
    pStrSend->usMinSpeed = htons(strChannelInfoGet.usMinSpeed);
    pStrSend->usOSDPosX = htons(strChannelInfoGet.usOSDPosX);
    pStrSend->usOSDPosY = htons(strChannelInfoGet.usOSDPosY);
    pStrSend->usVehicleLinkMode = htons(strChannelInfoGet.usVehicleLinkMode);
    pStrSend->usCoilDistance = htons((unsigned short)(strChannelInfoGet.uiCoilDist));

    memset(&(pStrSend->strCoilDesp), 0, sizeof(STR_COIL_DESP));
    
    if (strChannelInfoGet.usVehicleLinkMode == CONNECT_TYPE_COM )
    {
        pStrSend->strCoilDesp.strComCoilDesp.cBehindCoilId  = strChannelInfoGet.strCoilDesp.strComCoilDesp.cBehindCoilId;
        pStrSend->strCoilDesp.strComCoilDesp.cBoardId       = strChannelInfoGet.strCoilDesp.strComCoilDesp.cBoardId;
        pStrSend->strCoilDesp.strComCoilDesp.cComId         = strChannelInfoGet.strCoilDesp.strComCoilDesp.cComId;
        pStrSend->strCoilDesp.strComCoilDesp.cFrontCoilId   = strChannelInfoGet.strCoilDesp.strComCoilDesp.cFrontCoilId;
        pStrSend->strCoilDesp.strComCoilDesp.cGroupId       = strChannelInfoGet.strCoilDesp.strComCoilDesp.cGroupId;

    }
    else if (strChannelInfoGet.usVehicleLinkMode == CONNECT_TYPE_GPIO )
    {
        pStrSend->strCoilDesp.strGpioCoilDesp.cBehindCoilId  = strChannelInfoGet.strCoilDesp.strGpioCoilDesp.cBehindCoilId;
        pStrSend->strCoilDesp.strGpioCoilDesp.cFrontCoilId   = strChannelInfoGet.strCoilDesp.strGpioCoilDesp.cFrontCoilId;

    }
    
    pStrSend->cVLDispFlag = strChannelInfoGet.cVLDispFlag;
    pStrSend->cLaneDispFlag = strChannelInfoGet.cLaneDispFlag;

    pStrSend->cSingalCoilPos = strChannelInfoGet.cSingalCoilPos;
    pStrSend->cGroudCoilDecType = strChannelInfoGet.cGroudCoilDecType;
    pStrSend->cSingalCoilSnapType = strChannelInfoGet.cSingalCoilSnapType;
    pStrSend->cCoilSwitchFlag = strChannelInfoGet.cCoilSwitchFlag;

    if( strChannelInfoGet.cCoilSwitchSta == SWITCH_DEFAULT_VAL )
    {
        pStrSend->cCoilSwitchSta = (char)(strChannelInfoGet.usDetectType);
    }
    else
    {
        pStrSend->cCoilSwitchSta = strChannelInfoGet.cCoilSwitchSta;
    }

    if( strChannelInfoGet.cInLightSta == SWITCH_DEFAULT_VAL )
    {
        pStrSend->cInLightSta = (char)(strChannelInfoGet.usFlashLightType);
    }
    else
    {
        pStrSend->cInLightSta = strChannelInfoGet.cInLightSta;
    }

    XCSPTL_INFO("pp query coil dec switch flag:%d.\n",  pStrSend->cCoilSwitchFlag);
    XCSPTL_INFO("pp query coil dec type:%d.\n",         strChannelInfoGet.usDetectType);
    XCSPTL_INFO("pp query coil dec sta:%d.\n",          pStrSend->cCoilSwitchSta);

    XCSPTL_INFO("pp query inlight type:%d.\n",         strChannelInfoGet.usFlashLightType);
    XCSPTL_INFO("pp query inlight sta:%d.\n",          pStrSend->cInLightSta);

    
    pStrSend->usPPProtoVer = htons(XCS_PRO_PP_VER_2);
    XCSPTL_INFO("query cLaneDispFlag is %d .\n", strChannelInfoGet.cLaneDispFlag);
    
    // memcpy(&(pStrSend->strCoilDesp), &(strChannelInfoGet.strCoilDesp), sizeof(strChannelInfoGet.strCoilDesp));
   
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    pStrSend->cVLSnapDir = strChannelInfoGet.cVLSnapDir;
    pStrSend->cVLDecEn   = strChannelInfoGet.cVLDecEn;

    pStrSend->usSingalCoilHigh = htons(strChannelInfoGet.usSingalCoilHigh);

    pStrSend->ucDaySnapObj = strChannelInfoGet.ucDaySnapObj;
    pStrSend->ucNightSnapObj = strChannelInfoGet.ucNightSnapObj;

    pStrSend->ucSnapPicNum   = strChannelInfoGet.ucSnapPicNum;
    pStrSend->usSnapPicInter = htons(strChannelInfoGet.usSnapPicInter);
    XCSPTL_INFO("pp query snap pic num:%d,inter:%d\n", strChannelInfoGet.ucSnapPicNum, strChannelInfoGet.usSnapPicInter);

    pStrSend->ucInLightEn = strChannelInfoGet.ucInLightEn;
    pStrSend->strTimeSlotSnapObj.ucSnapObj = strChannelInfoGet.strTimeSlotSnapObj.ucSnapObj;
    pStrSend->strTimeSlotSnapObj.uiBeginTimeSecs = htonl(strChannelInfoGet.strTimeSlotSnapObj.uiBeginTimeSecs);
    pStrSend->strTimeSlotSnapObj.uiLastTimeSecs = htonl(strChannelInfoGet.strTimeSlotSnapObj.uiLastTimeSecs);

    pStrSend->cSnapDir = strChannelInfoGet.cSnapDir;
    pStrSend->cSDEnFlag = strChannelInfoGet.cSDEnFlag;
    pStrSend->cTruckEnFlag = strChannelInfoGet.cTruckEnFlag;
    pStrSend->iSDBeginTime1 = htonl(strChannelInfoGet.iSDBeginTime1);
    pStrSend->iSDLastTime1 = htonl(strChannelInfoGet.iSDLastTime1);
    pStrSend->iSDBeginTime2 = htonl(strChannelInfoGet.iSDBeginTime2);
    pStrSend->iSDLastTime2 = htonl(strChannelInfoGet.iSDLastTime2);
    pStrSend->iSDBeginTime3 = htonl(strChannelInfoGet.iSDBeginTime3);
    pStrSend->iSDLastTime3 = htonl(strChannelInfoGet.iSDLastTime3);
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_CAR_CHANNEL_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_CAR_CHANNEL_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}



int CHandleXcsPtl::bayonetChannelInfoSet(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    unsigned short usTmpVer =0;
    STR_BAYONET_CAR_CHANNEL_INFO_SET *pClientInfo = (STR_BAYONET_CAR_CHANNEL_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    
    PORT_VEHICLE_CONF_INFO strChannelInfoSet;
    
    strChannelInfoSet.uiCarRoadId = ntohl(pClientInfo->uiCarRoadId);
    if(strChannelInfoSet.uiCarRoadId > 0)
        strChannelInfoSet.uiCarRoadId--;
    for(int i =0; i<4; i++ )
    {
        strChannelInfoSet.ausFlashAddr[i] = ntohs(pClientInfo->ausFlashAddr[i]);
        XCSPTL_INFO("set pp flash addr is %d\n", strChannelInfoSet.ausFlashAddr[i]);
    }
    strChannelInfoSet.cComId = pClientInfo->cFlashComId;
    strChannelInfoSet.cFlashDevType = pClientInfo->cLightDevType;
    strChannelInfoSet.cFlashCtlMode = pClientInfo->cLightCtlMode;

    pClientInfo->szLaneId[7] =0;
    pClientInfo->szOSDContent[31] =0;
    strcpy(strChannelInfoSet.szLaneId, pClientInfo->szLaneId);
    strncpy(strChannelInfoSet.szOSDContent, pClientInfo->szOSDContent,32);
    
    strChannelInfoSet.strRoadArea.usAreaHeight  = ntohs(pClientInfo->strRoadArea.usAreaHeight);
    strChannelInfoSet.strRoadArea.usAreaPosX    = ntohs(pClientInfo->strRoadArea.usAreaPosX);
    strChannelInfoSet.strRoadArea.usAreaPosY    = ntohs(pClientInfo->strRoadArea.usAreaPosY);    
    strChannelInfoSet.strRoadArea.usAreaWidth   = ntohs(pClientInfo->strRoadArea.usAreaWidth);

    strChannelInfoSet.strVLArea.usAreaHeight = ntohs(pClientInfo->strVLArea.usAreaHeight);
    strChannelInfoSet.strVLArea.usAreaPosX = ntohs(pClientInfo->strVLArea.usAreaPosX);
    strChannelInfoSet.strVLArea.usAreaPosY= ntohs(pClientInfo->strVLArea.usAreaPosY);    
    strChannelInfoSet.strVLArea.usAreaWidth = ntohs(pClientInfo->strVLArea.usAreaWidth);

    strChannelInfoSet.usDetectType = ntohs(pClientInfo->usDetectType);
    strChannelInfoSet.usIrreSpeed = ntohs(pClientInfo->usIrreSpeed);
    strChannelInfoSet.usFlashLightCtlType = ntohs(pClientInfo->usFlashLightCtlType);
    strChannelInfoSet.usFlashLightType = ntohs(pClientInfo->usFlashLightType);
    strChannelInfoSet.usMaxSpeed = ntohs(pClientInfo->usMaxSpeed);
    strChannelInfoSet.usMinSpeed = ntohs(pClientInfo->usMinSpeed);
    strChannelInfoSet.usOSDPosX = ntohs(pClientInfo->usOSDPosX);
    strChannelInfoSet.usOSDPosY = ntohs(pClientInfo->usOSDPosY);
    strChannelInfoSet.usVehicleLinkMode = ntohs(pClientInfo->usVehicleLinkMode);
    strChannelInfoSet.uiCoilDist = ntohs(pClientInfo->usCoilDistance);


    strChannelInfoSet.cSingalCoilPos        = pClientInfo->cSingalCoilPos;
    strChannelInfoSet.cGroudCoilDecType     = pClientInfo->cGroudCoilDecType;
    strChannelInfoSet.cSingalCoilSnapType   = pClientInfo->cSingalCoilSnapType;
    strChannelInfoSet.cCoilSwitchFlag       = pClientInfo->cCoilSwitchFlag;

    strChannelInfoSet.cInLightSta           = (char)(strChannelInfoSet.usFlashLightType);
    strChannelInfoSet.cCoilSwitchSta        = SWITCH_DEFAULT_VAL;

    if (strChannelInfoSet.usVehicleLinkMode == CONNECT_TYPE_COM)
    {
        strChannelInfoSet.strCoilDesp.strComCoilDesp.cBehindCoilId  = pClientInfo->strCoilDesp.strComCoilDesp.cBehindCoilId;
        strChannelInfoSet.strCoilDesp.strComCoilDesp.cBoardId       = pClientInfo->strCoilDesp.strComCoilDesp.cBoardId;
        strChannelInfoSet.strCoilDesp.strComCoilDesp.cComId         = pClientInfo->strCoilDesp.strComCoilDesp.cComId;
        strChannelInfoSet.strCoilDesp.strComCoilDesp.cFrontCoilId   = pClientInfo->strCoilDesp.strComCoilDesp.cFrontCoilId;
        strChannelInfoSet.strCoilDesp.strComCoilDesp.cGroupId       = pClientInfo->strCoilDesp.strComCoilDesp.cGroupId;        
    }
    else if (strChannelInfoSet.usVehicleLinkMode == CONNECT_TYPE_GPIO )
    {
        strChannelInfoSet.strCoilDesp.strGpioCoilDesp.cBehindCoilId  = pClientInfo->strCoilDesp.strGpioCoilDesp.cBehindCoilId;
        strChannelInfoSet.strCoilDesp.strGpioCoilDesp.cFrontCoilId   = pClientInfo->strCoilDesp.strGpioCoilDesp.cFrontCoilId;        
    }

    usTmpVer = ntohs(pClientInfo->usPPProtoVer);
    if(( XCS_PRO_INVALID_VER0 == usTmpVer ) || ( XCS_PRO_INVALID_VER1 == usTmpVer))
    {
        strChannelInfoSet.cVLDispFlag = 1;
        strChannelInfoSet.cLaneDispFlag = 1;
        XCSPTL_INFO("invalid set cLaneDispFlag is %d .\n", pClientInfo->cLaneDispFlag);
    }
    else if( XCS_PRO_PP_VER_2 == usTmpVer)
    {
        strChannelInfoSet.cVLDispFlag = pClientInfo->cVLDispFlag;
        strChannelInfoSet.cLaneDispFlag = pClientInfo->cLaneDispFlag;
        XCSPTL_INFO("set cLaneDispFlag is %d .\n", pClientInfo->cLaneDispFlag);
    }
    else
    {
        strChannelInfoSet.cVLDispFlag = pClientInfo->cVLDispFlag;
        strChannelInfoSet.cLaneDispFlag = pClientInfo->cLaneDispFlag;
        XCSPTL_INFO("set cLaneDispFlag is %d. ver:0x%x\n", pClientInfo->cLaneDispFlag, usTmpVer);
    }

    strChannelInfoSet.cVLSnapDir  = pClientInfo->cVLSnapDir;
    strChannelInfoSet.cVLDecEn    = pClientInfo->cVLDecEn;

    strChannelInfoSet.usSingalCoilHigh = ntohs(pClientInfo->usSingalCoilHigh);
    strChannelInfoSet.ucDaySnapObj  = pClientInfo->ucDaySnapObj;
    strChannelInfoSet.ucNightSnapObj= pClientInfo->ucNightSnapObj;

    strChannelInfoSet.ucSnapPicNum = pClientInfo->ucSnapPicNum;
    strChannelInfoSet.usSnapPicInter = ntohs(pClientInfo->usSnapPicInter);
    XCSPTL_INFO("lane:%d,set pp snapPicNum is %d. inter is %d\n", strChannelInfoSet.uiCarRoadId,\
    strChannelInfoSet.ucSnapPicNum, strChannelInfoSet.usSnapPicInter);

    strChannelInfoSet.ucInLightEn = pClientInfo->ucInLightEn;

    strChannelInfoSet.strTimeSlotSnapObj.ucSnapObj = pClientInfo->strTimeSlotSnapObj.ucSnapObj;
    strChannelInfoSet.strTimeSlotSnapObj.uiBeginTimeSecs = ntohl(pClientInfo->strTimeSlotSnapObj.uiBeginTimeSecs);
    strChannelInfoSet.strTimeSlotSnapObj.uiLastTimeSecs = ntohl(pClientInfo->strTimeSlotSnapObj.uiLastTimeSecs);

    strChannelInfoSet.cSnapDir = pClientInfo->cSnapDir;
    strChannelInfoSet.cSDEnFlag = pClientInfo->cSDEnFlag;
    strChannelInfoSet.cTruckEnFlag = pClientInfo->cTruckEnFlag;
    strChannelInfoSet.iSDBeginTime1 = ntohl(pClientInfo->iSDBeginTime1);
    strChannelInfoSet.iSDLastTime1 = ntohl(pClientInfo->iSDLastTime1);
    strChannelInfoSet.iSDBeginTime2 = ntohl(pClientInfo->iSDBeginTime2);
    strChannelInfoSet.iSDLastTime2 = ntohl(pClientInfo->iSDLastTime2);
    strChannelInfoSet.iSDBeginTime3 = ntohl(pClientInfo->iSDBeginTime3);
    strChannelInfoSet.iSDLastTime3 = ntohl(pClientInfo->iSDLastTime3);
    
    ret = SysConfSet(e_PASSPORT_VEHICLE_CONF, &strChannelInfoSet, strChannelInfoSet.uiCarRoadId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("bayonet Set channel Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_BAYONET_CAR_CHANNEL_INFO_SET;
	strTmpSysMsg.ucCarRoadId = strChannelInfoSet.uiCarRoadId;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet channel info Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet channel info msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;  
}

int CHandleXcsPtl::bayonetVLInfoQuery(XCS_CLIENT * pClintSocket)
{
    unsigned int uiCarRoadId =0;
    PSTR_BAYONET_VL_INFO_PC_QUERY pPcQueryInfo = (PSTR_BAYONET_VL_INFO_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    VL_CONF_INFO strVlInfoGet;
    uiCarRoadId = ntohl(pPcQueryInfo->uiCarRoadId);
    if(uiCarRoadId > 0)
        uiCarRoadId--;
    int nRet = SysConfGet(e_VL_CONF, &strVlInfoGet, uiCarRoadId);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("bayonet get VL Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    PSTR_BAYONET_VL_INFO pStrVLSend = (PSTR_BAYONET_VL_INFO)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    
    pStrVLSend->uiCarRoadId = htonl(strVlInfoGet.uiCarRoadId +1);
    pStrVLSend->uiCorrDayThresh = htonl(strVlInfoGet.uiCorrDayThresh);
    pStrVLSend->uiCorrEveningThresh = htonl(strVlInfoGet.uiCorrEveningThresh);
    pStrVLSend->uiCorrMotorThresh = htonl(strVlInfoGet.uiCorrMotorThresh);
    pStrVLSend->uiCorrNightThresh = htonl(strVlInfoGet.uiCorrNightThresh);
    pStrVLSend->uiDayMotorWidth = htonl(strVlInfoGet.uiDayMotorWidth);
    pStrVLSend->uiNightMotorWidth = htonl(strVlInfoGet.uiNightMotorWidth);
    pStrVLSend->usThrDayToNight   = htons(strVlInfoGet.usThrDayToNight);
    pStrVLSend->usThrNightToDay   = htons(strVlInfoGet.usThrNightToDay);
    pStrVLSend->ucCapDelayInterval = strVlInfoGet.ucCapDelayInterval;

    pStrVLSend->cCapDelayNum      = strVlInfoGet.cCapDelayNum;
    pStrVLSend->cMinPlatePixelSize   = strVlInfoGet.cMinPlatePixelSize;
    pStrVLSend->cMaxPlatePixelSize   = strVlInfoGet.cMaxPlatePixelSize;
    //pStrVLSend->uiVersion = htonl(strVlInfoGet.uiNightMotorWidth);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    //XCSPTL_ERROR("query minPixelSize:%d,maxPixelSize:%d .\n",strVlInfoGet.cMinPlatePixelSize, strVlInfoGet.cMaxPlatePixelSize);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_VL_INFO);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_VIRTUAL_LOOP_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::bayonetHighVLInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
 
    VL_CONF_INFO strVlInfoSet;
    PSTR_BAYONET_VL_INFO pClientInfo = (PSTR_BAYONET_VL_INFO)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    strVlInfoSet.uiCarRoadId = ntohl(pClientInfo->uiCarRoadId);
    if(strVlInfoSet.uiCarRoadId > 0)
       strVlInfoSet.uiCarRoadId--; 
    strVlInfoSet.uiCorrDayThresh = ntohl(pClientInfo->uiCorrDayThresh);
    strVlInfoSet.uiCorrEveningThresh = ntohl(pClientInfo->uiCorrEveningThresh);
    strVlInfoSet.uiCorrMotorThresh = ntohl(pClientInfo->uiCorrMotorThresh);
    strVlInfoSet.uiCorrNightThresh = ntohl(pClientInfo->uiCorrNightThresh);
    strVlInfoSet.uiDayMotorWidth = ntohl(pClientInfo->uiDayMotorWidth);
    strVlInfoSet.uiNightMotorWidth = ntohl(pClientInfo->uiNightMotorWidth);
    strVlInfoSet.usThrDayToNight   = ntohs(pClientInfo->usThrDayToNight);
    strVlInfoSet.usThrNightToDay   = ntohs(pClientInfo->usThrNightToDay);
    strVlInfoSet.ucCapDelayInterval = pClientInfo->ucCapDelayInterval;

    strVlInfoSet.cCapDelayNum      = pClientInfo->cCapDelayNum;
    strVlInfoSet.cMinPlatePixelSize   = pClientInfo->cMinPlatePixelSize;
    strVlInfoSet.cMaxPlatePixelSize   = pClientInfo->cMaxPlatePixelSize;

    //XCSPTL_INFO("Set road:%d,DayThresh:%d .\n",strVlInfoSet.uiCarRoadId, strVlInfoSet.uiCorrDayThresh);
    //XCSPTL_ERROR("set minPixelSize:%d,maxPixelSize:%d .\n",strVlInfoSet.cMinPlatePixelSize, strVlInfoSet.cMaxPlatePixelSize);
    
    int nRet = SysConfSet(e_VL_CONF, &strVlInfoSet, strVlInfoSet.uiCarRoadId);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("bayonet Set VL Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_BAYONET_HIGH_VL_INFO_SET;
    strTmpSysMsg.ucCarRoadId = strVlInfoSet.uiCarRoadId;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet vl Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet vl Conf msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::bayonetHandSnap(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    PSTR_BAYONET_HAND_SNAP pPcSetInfo = (PSTR_BAYONET_HAND_SNAP)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    STR_BAYONET_HAND_SNAP  strHandSnap;

    strHandSnap.uiCarRoadId = ntohl(pPcSetInfo->uiCarRoadId);
    if(strHandSnap.uiCarRoadId > 0)
        strHandSnap.uiCarRoadId--;
    strHandSnap.uiCarSpeed = ntohl(pPcSetInfo->uiCarSpeed);

    XCSPTL_INFO("hand snap carId:%d, carSpeed:%d .\n", strHandSnap.uiCarRoadId, strHandSnap.uiCarSpeed);
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_BAYONET_MANUAL_SNAP;
    PSTR_BAYONET_HAND_SNAP pstrHandSnapInfo = (PSTR_BAYONET_HAND_SNAP)(strTmpSysMsg.aucData);
    pstrHandSnapInfo->uiCarRoadId = strHandSnap.uiCarRoadId;
    pstrHandSnapInfo->uiCarSpeed = strHandSnap.uiCarSpeed;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet hand snap failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet hand snap msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::bayonetFtpInfoQuery(XCS_CLIENT * pClintSocket)
{
    FTP_CONF_INFO strFtpInfoGet;
    
    int nRet = SysConfGet(e_PASSPORT_FTP_CONF, &strFtpInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("bayonet get bayonet Ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    XCSPTL_INFO("query enable:%d,ip:0x%x,port %d name %s passwd %s .\n",
        strFtpInfoGet.uiEnable,strFtpInfoGet.uiFtpIP,strFtpInfoGet.usFtpPort,strFtpInfoGet.szUserName,strFtpInfoGet.szUserPasswd);
    
    PSTR_BAYONET_FTP_INFO_QUERY pPcQueryInfo = (PSTR_BAYONET_FTP_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    pPcQueryInfo->uiEnable = htonl(strFtpInfoGet.uiEnable);
    pPcQueryInfo->uiFtpIP = htonl(strFtpInfoGet.uiFtpIP);
    pPcQueryInfo->usFtpPort = htons(strFtpInfoGet.usFtpPort);

    strFtpInfoGet.szUserName[31] =0;
    strFtpInfoGet.szUserPasswd[31] =0;
    strncpy(pPcQueryInfo->szUserName, strFtpInfoGet.szUserName, 31);
    strncpy(pPcQueryInfo->szUserPasswd, strFtpInfoGet.szUserPasswd, 31);
    
    pPcQueryInfo->cTRecEnable = strFtpInfoGet.cTRecEnable;
    pPcQueryInfo->ucTRecLast = strFtpInfoGet.ucTRecLast;
    pPcQueryInfo->usTRecInterval = htons(strFtpInfoGet.usTRecInterval);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_FTP_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_FTP_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::bayonetFtpInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    FTP_CONF_INFO strFtpInfoSet;
    STR_BAYONET_FTP_INFO_SET *pClientInfo = (STR_BAYONET_FTP_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    strFtpInfoSet.uiEnable = ntohl(pClientInfo->uiEnable);
    strFtpInfoSet.uiFtpIP = ntohl(pClientInfo->uiFtpIP);
    strFtpInfoSet.usFtpPort = ntohs(pClientInfo->usFtpPort);

    pClientInfo->szUserName[31] =0;
    pClientInfo->szUserPasswd[31] =0;
    strncpy(strFtpInfoSet.szUserName, pClientInfo->szUserName, 31);
    strncpy(strFtpInfoSet.szUserPasswd, pClientInfo->szUserPasswd, 31);

    strFtpInfoSet.cTRecEnable = pClientInfo->cTRecEnable;
    strFtpInfoSet.ucTRecLast = pClientInfo->ucTRecLast;
    strFtpInfoSet.usTRecInterval = ntohs(pClientInfo->usTRecInterval);

    XCSPTL_INFO("set enable:%d,ip:0x%x,port %d name %s passwd %s .\n",
        strFtpInfoSet.uiEnable,strFtpInfoSet.uiFtpIP,strFtpInfoSet.usFtpPort,strFtpInfoSet.szUserName,strFtpInfoSet.szUserPasswd);

    ret = SysConfSet(e_PASSPORT_FTP_CONF, &strFtpInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("bayonet Set bayonet ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_BAYONET_FTP_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet ftp Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet ftp Conf msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::bayonetBackupsFtpInfoQuery(XCS_CLIENT * pClintSocket)
{
    FTP_CONF_INFO strFtpInfoGet;
    
    int nRet = SysConfGet(e_PASSPORT_BACKUPS_FTP_CONF, &strFtpInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("bayonet get bayonet backups Ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    XCSPTL_INFO("query backups ftp enable:%d,ip:0x%x,port %d name %s passwd %s .\n",
        strFtpInfoGet.uiEnable,strFtpInfoGet.uiFtpIP,strFtpInfoGet.usFtpPort,strFtpInfoGet.szUserName,strFtpInfoGet.szUserPasswd);
    
    PSTR_BAYONET_BACKUPS_FTP_INFO_QUERY pPcQueryInfo = (PSTR_BAYONET_BACKUPS_FTP_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    pPcQueryInfo->uiEnable = htonl(strFtpInfoGet.uiEnable);
    pPcQueryInfo->uiFtpIP = htonl(strFtpInfoGet.uiFtpIP);
    pPcQueryInfo->usFtpPort = htons(strFtpInfoGet.usFtpPort);

    strFtpInfoGet.szUserName[31] =0;
    strFtpInfoGet.szUserPasswd[31] =0;
    strncpy(pPcQueryInfo->szUserName, strFtpInfoGet.szUserName, 31);
    strncpy(pPcQueryInfo->szUserPasswd, strFtpInfoGet.szUserPasswd, 31);
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_FTP_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_BACKUPS_FTP_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::bayonetBackupsFtpInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    FTP_CONF_INFO strFtpInfoSet;
    STR_BAYONET_BACKUPS_FTP_INFO_SET *pClientInfo = (STR_BAYONET_BACKUPS_FTP_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    strFtpInfoSet.uiEnable = ntohl(pClientInfo->uiEnable);
    strFtpInfoSet.uiFtpIP = ntohl(pClientInfo->uiFtpIP);
    strFtpInfoSet.usFtpPort = ntohs(pClientInfo->usFtpPort);

    pClientInfo->szUserName[31] =0;
    pClientInfo->szUserPasswd[31] =0;
    strncpy(strFtpInfoSet.szUserName, pClientInfo->szUserName, 31);
    strncpy(strFtpInfoSet.szUserPasswd, pClientInfo->szUserPasswd, 31);

    XCSPTL_INFO("set backups ftp enable:%d,ip:0x%x,port %d name %s passwd %s .\n",
        strFtpInfoSet.uiEnable,strFtpInfoSet.uiFtpIP,strFtpInfoSet.usFtpPort,strFtpInfoSet.szUserName,strFtpInfoSet.szUserPasswd);

    ret = SysConfSet(e_PASSPORT_BACKUPS_FTP_CONF, &strFtpInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("bayonet Set bayonet backups ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_BAYONET_FTP_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet backups ftp Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet backups ftp Conf msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::bayonetJpgOsdConfQuery(XCS_CLIENT * pClintSocket)
{
    BAYONET_JPG_OSD_CONF strPpJpgOsdInfoGet;
    
    int nRet = SysConfGet(e_PP_JPG_OSD_CONF, &strPpJpgOsdInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("bayonet get bayonet jpg osd Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    PSTR_BAYONET_JPG_OSD_CONF_QUERY pPcQueryInfo = (PSTR_BAYONET_JPG_OSD_CONF_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

	memcpy((void *)pPcQueryInfo, (void *)&strPpJpgOsdInfoGet, sizeof(strPpJpgOsdInfoGet));

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_JPG_OSD_CONF_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_JPG_OSD_CONF_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::bayonetJpgOsdConfSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    BAYONET_JPG_OSD_CONF strPpJpgOsdInfoSet;
    
    STR_BAYONET_JPG_OSD_CONF_SET *pClientInfo = (STR_BAYONET_JPG_OSD_CONF_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

	memcpy((void *)&strPpJpgOsdInfoSet, (void *)pClientInfo, sizeof(strPpJpgOsdInfoSet));

    ret = SysConfSet(e_PP_JPG_OSD_CONF, &strPpJpgOsdInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("bayonet Set jpg osd Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType  = MSG_PP_JPG_OSD_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send bayonet jpg osd Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send bayonet jpg osd Conf msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::bayonetIrregInfoQuery(XCS_CLIENT * pClintSocket)
{
    PP_IRREG_CONF_INFO strPpIrregInfoGet;
    
    int nRet = SysConfGet(e_PP_IRREG_INFO_CONF, &strPpIrregInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("pp get pp irreg Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    PPP_IRREG_CONF_INFO_QUERY pPcQueryInfo = (PPP_IRREG_CONF_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

	memcpy((void *)pPcQueryInfo, (void *)(&strPpIrregInfoGet), sizeof(strPpIrregInfoGet));

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(PP_IRREG_CONF_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_BAYONET, CPE_BAYONET_IRREG_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::bayonetIrregInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    PP_IRREG_CONF_INFO strPpIrregInfoSet;
    PP_IRREG_CONF_INFO_SET *pClientInfo = (PP_IRREG_CONF_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

	memset((void *)&strPpIrregInfoSet, 0, sizeof(strPpIrregInfoSet));
	memcpy((void *)&strPpIrregInfoSet, (void *)pClientInfo, sizeof(strPpIrregInfoSet));

    ret = SysConfSet(e_PP_IRREG_INFO_CONF, &strPpIrregInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("pp Set pp irreg Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_PP_IRREG_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send pp irreg Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send pp irreg Conf msg ok.\n");
    }


    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}


