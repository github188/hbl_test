/******************************************************************************
 * electPoliceProHandle.cpp:
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

extern LANE_SWITCH_STA_INFO  g_EPLaneSwitchInfo[MAX_VEHICLE_NUM]; /* 电警车道切换状态 */

int CHandleXcsPtl::electPoliceGInfoQuery(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int j =0;
    /* 1.获取配置信息 */
    ELEC_POLICE_OVERALL_CONF_INFO strCfgInfoGet;
    int nRet = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strCfgInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("electPolice get gInfo Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
    }
    /* 2.给客户端赋值 */
    PSTR_ELECT_POLICE_GLOBAL_INFO_QUERY pClinetContent = (PSTR_ELECT_POLICE_GLOBAL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    pClinetContent->cFlashComId = strCfgInfoGet.cComId;
    for(i=0; i< 4; i++)
    {
        pClinetContent->ausFlashAddr[i] = htons(strCfgInfoGet.ausFlashAddr[i]);
        XCSPTL_INFO("electPolice flash 485addr:%d\n", strCfgInfoGet.ausFlashAddr[i]);
        
        pClinetContent->aucFreqFlashEn[i] = strCfgInfoGet.aucFreqFlashEn[i];
        XCSPTL_INFO("electPolice freq flash en:%d\n", strCfgInfoGet.aucFreqFlashEn[i]);

        pClinetContent->aucFlashTrigMode[i] = strCfgInfoGet.aucFlashTrigMode[i];
        XCSPTL_INFO("electPolice flash trig mode:%d\n", strCfgInfoGet.aucFlashTrigMode[i]);

        pClinetContent->aucStrobeFlashFre[i] = strCfgInfoGet.aucStrobeFlashFre[i];
        XCSPTL_INFO("electPolice strobe flash freq:%d\n", strCfgInfoGet.aucStrobeFlashFre[i]);
    }


    pClinetContent->cFlashDevType = strCfgInfoGet.cFlashDevType;
    pClinetContent->cLightCtlMode = strCfgInfoGet.cFlashCtlMode;

    strCfgInfoGet.szRoadId[7] = 0;
    strCfgInfoGet.szDirId[7] =0;
    strcpy(pClinetContent->szRoadId, strCfgInfoGet.szRoadId);
    strcpy(pClinetContent->szDirId, strCfgInfoGet.szDirId);


    for(j =0; j< MAX_AREA_GROUP_NUM; j++)
    {
        for(i =0; i< MAX_AREA_NUM; i++)
        {
            pClinetContent->strRedArea[j][i].strArea.usAreaHeight = htons(strCfgInfoGet.strRedArea[j][i].strArea.usAreaHeight);
            pClinetContent->strRedArea[j][i].strArea.usAreaPosX = htons(strCfgInfoGet.strRedArea[j][i].strArea.usAreaPosX);
            pClinetContent->strRedArea[j][i].strArea.usAreaPosY = htons(strCfgInfoGet.strRedArea[j][i].strArea.usAreaPosY);
            pClinetContent->strRedArea[j][i].strArea.usAreaWidth = htons(strCfgInfoGet.strRedArea[j][i].strArea.usAreaWidth);        
        }
        pClinetContent->strRedLightMode[j].ucLightStrobeEn = strCfgInfoGet.strRedLightMode[j].ucLightStrobeEn;
        pClinetContent->strRedLightMode[j].ucLightStrobeFrame = strCfgInfoGet.strRedLightMode[j].ucLightStrobeFrame;
        pClinetContent->strRedLightMode[j].ucSingalColLight = strCfgInfoGet.strRedLightMode[j].ucSingalColLight;
        pClinetContent->strRedLightMode[j].ucMapRDNo = strCfgInfoGet.strRedLightMode[j].ucMapRDNo;
    }

    memcpy(&(pClinetContent->strRedLow), &strCfgInfoGet.strRedLow, sizeof(strCfgInfoGet.strRedLow));
    memcpy(&(pClinetContent->strRedHigh), &strCfgInfoGet.strRedHigh, sizeof(strCfgInfoGet.strRedHigh));    

    memcpy(&(pClinetContent->strGreenLow), &strCfgInfoGet.strGreenLow, sizeof(strCfgInfoGet.strGreenLow));
    memcpy(&(pClinetContent->strGreenHigh), &strCfgInfoGet.strGreenHigh, sizeof(strCfgInfoGet.strGreenHigh));    

    memcpy(&(pClinetContent->strYellowLow), &strCfgInfoGet.strYellowLow, sizeof(strCfgInfoGet.strYellowLow));
    memcpy(&(pClinetContent->strYellowHigh), &strCfgInfoGet.strYellowHigh, sizeof(strCfgInfoGet.strYellowHigh));   

    strCfgInfoGet.szCarRoadName[31] =0;
    strCfgInfoGet.szDirection[31] =0;
    strCfgInfoGet.szRemarks[31] =0;
    strcpy(pClinetContent->szCarRoadName, strCfgInfoGet.szCarRoadName);
    strcpy(pClinetContent->szDirection, strCfgInfoGet.szDirection);
    strcpy(pClinetContent->szRoadRemarks, strCfgInfoGet.szRemarks);
    
    pClinetContent->cCarDistance =  strCfgInfoGet.cCarDistance;
    pClinetContent->cDetectType =  strCfgInfoGet.cDetectType;
    pClinetContent->cFrameInterval =  strCfgInfoGet.cFrameInterval;
    pClinetContent->cLightAmendDisFlag =  strCfgInfoGet.cLightAmendDisFlag;    
    
    pClinetContent->cValidChanSum =  strCfgInfoGet.cValidChanSum;
    if(pClinetContent->cValidChanSum > g_uiCamLaneSum)
    {
       pClinetContent->cValidChanSum = g_uiCamLaneSum; 
    }

    pClinetContent->usEPGProVer = htons(XCS_PRO_EPG_VER_2);
    
    pClinetContent->usFlashCtlType = htons(strCfgInfoGet.usFlashLightCtlType);
    pClinetContent->usFlashLightType = htons(strCfgInfoGet.usFlashLightType);
    pClinetContent->usPicMixStyle = htons(strCfgInfoGet.usPicStyle);

    pClinetContent->cRLAmednEn = strCfgInfoGet.cLightAmendEn;
    pClinetContent->cRLDecType = strCfgInfoGet.cRLDectType;

    pClinetContent->cOSDColour = strCfgInfoGet.cOSDColour;
    pClinetContent->cRLAmendLevel = strCfgInfoGet.cRLAmendLevel;

    pClinetContent->cPPSendEn = strCfgInfoGet.cPPSendEn;

    pClinetContent->cPPFlashEn = strCfgInfoGet.cPPFlashEn;
    //XCSPTL_INFO("ep g query rlAmendFlag:%d,rlDecType:%d .\n",pClinetContent->cRLAmednEn, pClinetContent->cRLDecType);

    strcpy(pClinetContent->szEpDevCode, strCfgInfoGet.szEpDevCode);
    pClinetContent->szEpDevCode[MAX_DEV_CODE_LEN-1] =0;
    XCSPTL_INFO("query ep dev code:%s\n", pClinetContent->szEpDevCode);


    strcpy(pClinetContent->szExEpDevCode, strCfgInfoGet.szExEpDevCode);
    pClinetContent->szExEpDevCode[EXT_DEV_CODE_LEN] =0;
    XCSPTL_INFO("query ep exten dev code:%s\n", pClinetContent->szExEpDevCode);

    pClinetContent->cRedModifyMode = strCfgInfoGet.cRedModifyMode;

    pClinetContent->cACSyncSwitchEn = strCfgInfoGet.cACSyncSwitchEn;
    pClinetContent->ucACSwitchThreshold = strCfgInfoGet.ucACSwitchThreshold;
    pClinetContent->ucACDecCycle = strCfgInfoGet.ucACDecCycle;

    pClinetContent->usDayACPhase = htons(strCfgInfoGet.usDayACPhase);
    pClinetContent->usNightACPhase = htons(strCfgInfoGet.usNightACPhase);

    pClinetContent->ucSmartSwitchEn = strCfgInfoGet.ucSmartSwitchEn;
    pClinetContent->ucTimerSwitchEn = strCfgInfoGet.ucTimerSwitchEn;
    pClinetContent->uiFillInLightBeginTime = htonl(strCfgInfoGet.uiFillInLightBeginTime);
    pClinetContent->uiFillInLightLastTime = htonl(strCfgInfoGet.uiFillInLightLastTime);    
    
    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_ELECT_POLICE_GLOBAL_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_GLOBAL_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceGInfoSet(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int j =0;
    int ret =0;
    unsigned short usTmpVer =0;
    ELEC_POLICE_OVERALL_CONF_INFO strCfgInfoSet;
    
    /* 2.设置客户端内容 */
    STR_ELECT_POLICE_GLOBAL_INFO_SET *pClinetInfo = (STR_ELECT_POLICE_GLOBAL_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    strCfgInfoSet.cComId = pClinetInfo->cFlashComId;
    for(i=0; i< 4; i++)
    {
        strCfgInfoSet.ausFlashAddr[i] = ntohs(pClinetInfo->ausFlashAddr[i]);
        XCSPTL_INFO("EP set flash addr:%d.\n", strCfgInfoSet.ausFlashAddr[i]);

        strCfgInfoSet.aucFreqFlashEn[i] = pClinetInfo->aucFreqFlashEn[i];
        XCSPTL_INFO("EP set freq flash en:%d.\n", strCfgInfoSet.aucFreqFlashEn[i]);

        //strCfgInfoSet.aucFlashTrigMode[i] = pClinetInfo->aucFlashTrigMode[i];
        strCfgInfoSet.aucFlashTrigMode[i] = 1;		/* 固定为下降沿 */
        XCSPTL_INFO("EP set flash trig mode:%d.\n", strCfgInfoSet.aucFlashTrigMode[i]);

        strCfgInfoSet.aucStrobeFlashFre[i] = pClinetInfo->aucStrobeFlashFre[i];
        XCSPTL_INFO("EP set strobe flash freq:%d.\n", strCfgInfoSet.aucStrobeFlashFre[i]);
    }

    strCfgInfoSet.cFlashDevType = pClinetInfo->cFlashDevType;
    strCfgInfoSet.cFlashCtlMode = pClinetInfo->cLightCtlMode;

    pClinetInfo->szRoadId[7] =0;
    pClinetInfo->szDirId[7] =0;
    strcpy(strCfgInfoSet.szRoadId, pClinetInfo->szRoadId);
    strcpy(strCfgInfoSet.szDirId, pClinetInfo->szDirId);
    
    for(j =0; j< MAX_AREA_GROUP_NUM; j++)
    {
        for(i =0; i< MAX_AREA_NUM; i++)
        {
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaHeight = ntohs(pClinetInfo->strRedArea[j][i].strArea.usAreaHeight);
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaPosX = ntohs(pClinetInfo->strRedArea[j][i].strArea.usAreaPosX);
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaPosY = ntohs(pClinetInfo->strRedArea[j][i].strArea.usAreaPosY);
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaWidth = ntohs(pClinetInfo->strRedArea[j][i].strArea.usAreaWidth); 
            XCSPTL_INFO("EP set red area group:%d,num:%d,height:%d,width:%d,posX:%d,posY:%d.\n", \
            j,i,strCfgInfoSet.strRedArea[j][i].strArea.usAreaHeight,\
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaWidth,\
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaPosX,\
            strCfgInfoSet.strRedArea[j][i].strArea.usAreaPosY);
        }
        strCfgInfoSet.strRedLightMode[j].ucLightStrobeEn = pClinetInfo->strRedLightMode[j].ucLightStrobeEn;
        strCfgInfoSet.strRedLightMode[j].ucLightStrobeFrame = pClinetInfo->strRedLightMode[j].ucLightStrobeFrame;
        strCfgInfoSet.strRedLightMode[j].ucSingalColLight = pClinetInfo->strRedLightMode[j].ucSingalColLight;
        strCfgInfoSet.strRedLightMode[j].ucMapRDNo = pClinetInfo->strRedLightMode[j].ucMapRDNo;
    }
    memcpy(&strCfgInfoSet.strRedLow, &(pClinetInfo->strRedLow), sizeof(strCfgInfoSet.strRedLow));
    memcpy(&strCfgInfoSet.strRedHigh, &(pClinetInfo->strRedHigh), sizeof(strCfgInfoSet.strRedHigh));    

    memcpy(&strCfgInfoSet.strGreenLow, &(pClinetInfo->strGreenLow), sizeof(strCfgInfoSet.strGreenLow));
    memcpy(&strCfgInfoSet.strGreenHigh, &(pClinetInfo->strGreenHigh), sizeof(strCfgInfoSet.strGreenHigh));    

    memcpy(&strCfgInfoSet.strYellowLow, &(pClinetInfo->strYellowLow), sizeof(strCfgInfoSet.strYellowLow));
    memcpy(&strCfgInfoSet.strYellowHigh, &(pClinetInfo->strYellowHigh), sizeof(strCfgInfoSet.strYellowHigh));    


    pClinetInfo->szCarRoadName[31] =0;
    pClinetInfo->szDirection[31] =0;
    pClinetInfo->szRoadRemarks[31] =0;
    strcpy(strCfgInfoSet.szCarRoadName, pClinetInfo->szCarRoadName);
    strcpy(strCfgInfoSet.szDirection, pClinetInfo->szDirection);
    strcpy(strCfgInfoSet.szRemarks,  pClinetInfo->szRoadRemarks);

    //strCfgInfoSet.usFlashLightCtlType = ntohs(pClinetInfo->usFlashCtlType);
    strCfgInfoSet.usFlashLightCtlType = 2;/* 固定为ttl模式 */
    strCfgInfoSet.usFlashLightType = ntohs(pClinetInfo->usFlashLightType);
    strCfgInfoSet.usPicStyle = ntohs(pClinetInfo->usPicMixStyle);
    usTmpVer = ntohs(pClinetInfo->usEPGProVer);
    strCfgInfoSet.usXcsProVer = usTmpVer;
    g_usEpXcsProVer = usTmpVer;
    if(( XCS_PRO_INVALID_VER0 == usTmpVer ) || ( XCS_PRO_INVALID_VER1 == usTmpVer))
    {
        strCfgInfoSet.cDetectType = GROUD_LOOP;
        strCfgInfoSet.cValidChanSum= 3;
        strCfgInfoSet.cCarDistance = 40;
        strCfgInfoSet.cFrameInterval = 2;
        strCfgInfoSet.cLightAmendDisFlag = 1;
        XCSPTL_INFO("invalid set cLightAmendDisFlag is %d .\n", strCfgInfoSet.cLightAmendDisFlag);
    }
    else if(( XCS_PRO_EPG_VER == usTmpVer) || ( XCS_PRO_EPG_VER_2 == usTmpVer ))
    {
        strCfgInfoSet.cDetectType = pClinetInfo->cDetectType;
        strCfgInfoSet.cValidChanSum= pClinetInfo->cValidChanSum;
        strCfgInfoSet.cCarDistance = pClinetInfo->cCarDistance;
        /* 对车距做一个范围设置 */
        if( strCfgInfoSet.cCarDistance < 20 )
            strCfgInfoSet.cCarDistance =20;
        strCfgInfoSet.cFrameInterval = pClinetInfo->cFrameInterval;
        strCfgInfoSet.cLightAmendDisFlag = pClinetInfo->cLightAmendDisFlag;
        XCSPTL_INFO("normal set cLightAmendDisFlag is %d .\n", strCfgInfoSet.cLightAmendDisFlag);
    }
    else
    {
        strCfgInfoSet.cDetectType = pClinetInfo->cDetectType;
        strCfgInfoSet.cValidChanSum= pClinetInfo->cValidChanSum;
        strCfgInfoSet.cCarDistance = pClinetInfo->cCarDistance;
        /* 对车距做一个范围设置 */
        if( strCfgInfoSet.cCarDistance < 20 )
            strCfgInfoSet.cCarDistance =20;

        strCfgInfoSet.cFrameInterval = pClinetInfo->cFrameInterval;
        strCfgInfoSet.cLightAmendDisFlag = pClinetInfo->cLightAmendDisFlag;
        XCSPTL_INFO("other normal set cLightAmendDisFlag is %d. ver:0x%x\n", strCfgInfoSet.cLightAmendDisFlag, usTmpVer);
    }

    if(strCfgInfoSet.cValidChanSum > g_uiCamLaneSum)
    {
        strCfgInfoSet.cValidChanSum = g_uiCamLaneSum;
    }

    strCfgInfoSet.cLightAmendEn = pClinetInfo->cRLAmednEn;
    strCfgInfoSet.cRLDectType = pClinetInfo->cRLDecType;

    strCfgInfoSet.cOSDColour = pClinetInfo->cOSDColour;
    strCfgInfoSet.cRLAmendLevel = pClinetInfo->cRLAmendLevel;

    strCfgInfoSet.cPPSendEn = pClinetInfo->cPPSendEn;

    strCfgInfoSet.cPPFlashEn = pClinetInfo->cPPFlashEn;

    //XCSPTL_INFO("ep g set lightAmendFlag:%d,rlDecType:%d.\n", pClinetInfo->cRLAmednEn, pClinetInfo->cRLDecType);

    strcpy(strCfgInfoSet.szEpDevCode, pClinetInfo->szEpDevCode);
    strCfgInfoSet.szEpDevCode[MAX_DEV_CODE_LEN-1] =0;
    
    strcpy(strCfgInfoSet.szExEpDevCode, pClinetInfo->szExEpDevCode);
    strCfgInfoSet.szExEpDevCode[EXT_DEV_CODE_LEN-1] =0;
    
    strCfgInfoSet.cRedModifyMode = pClinetInfo->cRedModifyMode;

    strCfgInfoSet.cACSyncSwitchEn = pClinetInfo->cACSyncSwitchEn;
    strCfgInfoSet.ucACSwitchThreshold = pClinetInfo->ucACSwitchThreshold;
    strCfgInfoSet.ucACDecCycle = pClinetInfo->ucACDecCycle;
    strCfgInfoSet.usDayACPhase = ntohs(pClinetInfo->usDayACPhase);
    strCfgInfoSet.usNightACPhase = ntohs(pClinetInfo->usNightACPhase);

    strCfgInfoSet.ucSmartSwitchEn = pClinetInfo->ucSmartSwitchEn;
    strCfgInfoSet.ucTimerSwitchEn = pClinetInfo->ucTimerSwitchEn;

    strCfgInfoSet.uiFillInLightBeginTime = ntohl(pClinetInfo->uiFillInLightBeginTime);
    strCfgInfoSet.uiFillInLightLastTime = ntohl(pClinetInfo->uiFillInLightLastTime);

    ret = SysConfSet(e_ELEC_POLICE_OVERALL_CONF, &strCfgInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set gInfo Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
		return -1;
    }
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_GLOBAL_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police overall info failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police overall info msg ok.\n");
    }
    XCSPTL_INFO("ep g set go here ok\n");
    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
	return 0;
}

int CHandleXcsPtl::electPoliceChannelInfoQuery(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int ret =0;
    unsigned int uiCarRoadId =0;
    ELEC_POLICE_VEHICLE_CONF_INFO strChannelInfoGet;
    PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY pPcQueryInfo = (PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    memset(&strChannelInfoGet, 0, sizeof(strChannelInfoGet));
    uiCarRoadId = ntohl(pPcQueryInfo->uiCarRoadId);
    if(uiCarRoadId > MAX_VEHICLE_NUM)
    {
        XCSPTL_ERROR("electPolice query channel Info lane id is error:%d.\n", uiCarRoadId);
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    if(uiCarRoadId >0)
        uiCarRoadId--;
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strChannelInfoGet, uiCarRoadId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("ep get channel Info Conf failed,lane:%d.\n", uiCarRoadId);
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
    }
    PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY pStrSend = (PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    strChannelInfoGet.szOSDContent[31] =0;
    strcpy(pStrSend->szOSDContent, strChannelInfoGet.szOSDContent);

    pStrSend->strCoilDesp.strComCoilDesp.cBehindCoilId = strChannelInfoGet.strCoilDesp.strComCoilDesp.cBehindCoilId;
    pStrSend->strCoilDesp.strComCoilDesp.cBoardId = strChannelInfoGet.strCoilDesp.strComCoilDesp.cBoardId;
    pStrSend->strCoilDesp.strComCoilDesp.cComId = strChannelInfoGet.strCoilDesp.strComCoilDesp.cComId;
    pStrSend->strCoilDesp.strComCoilDesp.cFrontCoilId = strChannelInfoGet.strCoilDesp.strComCoilDesp.cFrontCoilId;
    pStrSend->strCoilDesp.strComCoilDesp.cGroupId = strChannelInfoGet.strCoilDesp.strComCoilDesp.cGroupId;

    pStrSend->strCoilDesp.strGpioCoilDesp.cBehindCoilId = strChannelInfoGet.strCoilDesp.strGpioCoilDesp.cBehindCoilId;
    pStrSend->strCoilDesp.strGpioCoilDesp.cFrontCoilId = strChannelInfoGet.strCoilDesp.strGpioCoilDesp.cFrontCoilId;

    //memcpy(&(pStrSend->strCoilDesp), &(strChannelInfoGet.strCoilDesp), sizeof(strChannelInfoGet.strCoilDesp));
    pStrSend->strRedLDesp.strEPComRedLightDesp.cCarRoadId = strChannelInfoGet.strRedLDesp.strEPComRedLightDesp.cCarRoadId;
    pStrSend->strRedLDesp.strEPComRedLightDesp.cComId = strChannelInfoGet.strRedLDesp.strEPComRedLightDesp.cComId;
    pStrSend->strRedLDesp.strEPGpioRedLightDesp.cGpioId = strChannelInfoGet.strRedLDesp.strEPGpioRedLightDesp.cGpioId;
    //memcpy(&(pStrSend->strRedLDesp), &(strChannelInfoGet.strRedLDesp), sizeof(strChannelInfoGet.strRedLDesp));

    pStrSend->strRoadArea.usAreaHeight = htons(strChannelInfoGet.strRoadArea.usAreaHeight);
    pStrSend->strRoadArea.usAreaPosX = htons(strChannelInfoGet.strRoadArea.usAreaPosX);
    pStrSend->strRoadArea.usAreaPosY = htons(strChannelInfoGet.strRoadArea.usAreaPosY);
    pStrSend->strRoadArea.usAreaWidth = htons(strChannelInfoGet.strRoadArea.usAreaWidth);

    pStrSend->uiCarRoadId = htonl(strChannelInfoGet.uiCarRoadId+1);
    pStrSend->usOSDPosX = htons(strChannelInfoGet.usOSDPosX);
    pStrSend->usOSDPosY = htons(strChannelInfoGet.usOSDPosY);
    pStrSend->usRedLConnectMode = htons(strChannelInfoGet.usRedLConnectMode);
    pStrSend->usRedLDetectType = htons(strChannelInfoGet.usRedLDetectType);
    pStrSend->usVehicleLinkMode = htons(strChannelInfoGet.usVehicleLinkMode);
    pStrSend->ucSimSnapFlag = strChannelInfoGet.uiForcedRedLight;

    pStrSend->ucMinPlatePixelSize = strChannelInfoGet.ucMinPlatePixelSize;
    pStrSend->ucMaxPlatePixelSize = strChannelInfoGet.ucMaxPlatePixelSize;
    pStrSend->ucCorrEveningThresh = strChannelInfoGet.ucCorrEveningThresh;

    XCSPTL_INFO("ep query minPlatePix:%d.\n", pStrSend->ucMinPlatePixelSize);
    XCSPTL_INFO("ep query maxPlatePix:%d.\n", pStrSend->ucMaxPlatePixelSize);
    XCSPTL_INFO("ep query EventThresh:%d.\n", pStrSend->ucCorrEveningThresh);
    
    /*Start of ITS on 2011-4-2 14:4 2.0.0.1*/
    pStrSend->cDetectType = strChannelInfoGet.cDetectType;
    pStrSend->cCoilSwitchFlag = strChannelInfoGet.cCoilSwitchFlag;
    if( strChannelInfoGet.cCoilSwitchSta == SWITCH_DEFAULT_VAL )
    {
        pStrSend->cCoilSwitchSta = strChannelInfoGet.cDetectType;
    }
    else
    {
        pStrSend->cCoilSwitchSta = strChannelInfoGet.cCoilSwitchSta;
    }

    XCSPTL_INFO("ep query coil dec switch flag:%d.\n",  pStrSend->cCoilSwitchFlag);
    XCSPTL_INFO("ep query coil dec type:%d.\n",         pStrSend->cDetectType);
    XCSPTL_INFO("ep query coil dec sta:%d.\n",          pStrSend->cCoilSwitchSta);    

    pStrSend->usFlashLightType = htons(strChannelInfoGet.usFlashLightType);
    if( strChannelInfoGet.cInLightSta == SWITCH_DEFAULT_VAL )
    {
        pStrSend->cInLightSta = strChannelInfoGet.usFlashLightType;
    }
    else
    {
        pStrSend->cInLightSta = strChannelInfoGet.cInLightSta;
    }
    pStrSend->cVLDecEn = strChannelInfoGet.cVLDecEn;
    /*End of ITS on 2011-4-2 14:4 2.0.0.1*/

    XCSPTL_INFO("ep query inLight type:%d.\n", strChannelInfoGet.usFlashLightType);
    XCSPTL_INFO("ep query inLight sta:%d.\n",  strChannelInfoGet.cInLightSta);

    pStrSend->cLaneDir = strChannelInfoGet.cLaneDir;

    for(i =0; i< MAX_LIGHT_TYPE; i++)
    {
        pStrSend->strTrafficLight[i].cEnable = strChannelInfoGet.strTrafficLight[i].cEnable;
        pStrSend->strTrafficLight[i].cLightType = strChannelInfoGet.strTrafficLight[i].cLightType;
        pStrSend->strTrafficLight[i].cVehicleNum = strChannelInfoGet.strTrafficLight[i].cVehicleNum;
        XCSPTL_INFO("query %d_light,enable:%d,type:%d,num:%d.\n", i,strChannelInfoGet.strTrafficLight[i].cEnable,\
        strChannelInfoGet.strTrafficLight[i].cLightType,strChannelInfoGet.strTrafficLight[i].cVehicleNum);
    }
    
    pStrSend->strTrafficLigntArea.usAreaPosX = htons(strChannelInfoGet.strTrafficLArea.usAreaPosX);
    pStrSend->strTrafficLigntArea.usAreaPosY = htons(strChannelInfoGet.strTrafficLArea.usAreaPosY);
    pStrSend->strTrafficLigntArea.usAreaWidth = htons(strChannelInfoGet.strTrafficLArea.usAreaWidth);
    pStrSend->strTrafficLigntArea.usAreaHeight = htons(strChannelInfoGet.strTrafficLArea.usAreaHeight);   

    for(i =0; i< 4; i++)
    {
        pStrSend->strVLPoint[i].usVLX1 = htons(strChannelInfoGet.strVLPoint[i].usVLX1);
        pStrSend->strVLPoint[i].usVLY1 = htons(strChannelInfoGet.strVLPoint[i].usVLY1);
    }

    pStrSend->cLaneLoopDispFlag = strChannelInfoGet.cLaneLoopDispFlag;
    pStrSend->cVLDispFlag = strChannelInfoGet.cVLDispFlag;

    strChannelInfoGet.szLaneId[7] =0;
    strcpy(pStrSend->szLaneId, strChannelInfoGet.szLaneId);

    pStrSend->usEPProVer = htons(XCS_PRO_EP_VER_2);

    pStrSend->cCarDistance = strChannelInfoGet.cCarDistance;
    pStrSend->cFrameInterval = strChannelInfoGet.cFrameInterval;

    pStrSend->cCarNoDir = strChannelInfoGet.cCarNoDir;
    pStrSend->cCarTrackEn = strChannelInfoGet.cCarTrackEn;
    pStrSend->cCarSnapDir = strChannelInfoGet.cCarSnapDir;

    XCSPTL_INFO("query carNoDir:%d,carTrackEn:%d,carSnapDir:%d\n", pStrSend->cCarNoDir, pStrSend->cCarTrackEn, pStrSend->cCarSnapDir);

    for(i =0; i< 2; i++)
    {
        pStrSend->strNoLaneVLPoint[i].usVLX1 = htons(strChannelInfoGet.strNoLaneVLPoint[i].usVLX1);
        pStrSend->strNoLaneVLPoint[i].usVLY1 = htons(strChannelInfoGet.strNoLaneVLPoint[i].usVLY1);
    }

    pStrSend->usCoilDist = htons(strChannelInfoGet.usCoilDist);

    pStrSend->ucPPDelaySnapFrames = strChannelInfoGet.ucPPDelaySnapFrames;

	pStrSend->cSwitchLaneEn      = strChannelInfoGet.cSwitchLaneEn;
    pStrSend->cSwitchLaneLeftNo  = strChannelInfoGet.cSwitchLaneLeftNo;
    pStrSend->cSwitchLaneStraightNo = strChannelInfoGet.cSwitchLaneStraightNo;


    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceChannelInfoSet(XCS_CLIENT *pClintSocket)
{
    int i =0;
    int ret =0;
    unsigned short usTmpVer =0;
    STR_ELECT_POLICE_CAR_CHANNEL_INFO_SET *pClientInfo = (STR_ELECT_POLICE_CAR_CHANNEL_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    ELEC_POLICE_VEHICLE_CONF_INFO strChannelInfoSet;

    memset(&strChannelInfoSet, 0, sizeof(ELEC_POLICE_VEHICLE_CONF_INFO));
    memset(&strChannelInfoSet.szOSDContent, 0, sizeof(strChannelInfoSet.szOSDContent));

    pClientInfo->szOSDContent[31] =0;
    strcpy(strChannelInfoSet.szOSDContent, pClientInfo->szOSDContent);
    XCSPTL_INFO("set osdcontent:%s,sys osd:%s\n",pClientInfo->szOSDContent, strChannelInfoSet.szOSDContent);

    strChannelInfoSet.uiCarRoadId = ntohl(pClientInfo->uiCarRoadId);
    if(strChannelInfoSet.uiCarRoadId > MAX_VEHICLE_NUM)
    {
        XCSPTL_ERROR("electPolice Set channel Info lane id is error:%d.\n", strChannelInfoSet.uiCarRoadId);
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    if(strChannelInfoSet.uiCarRoadId > 0)
        strChannelInfoSet.uiCarRoadId -= 1;

    strChannelInfoSet.strCoilDesp.strComCoilDesp.cBehindCoilId = pClientInfo->strCoilDesp.strComCoilDesp.cBehindCoilId;
    strChannelInfoSet.strCoilDesp.strComCoilDesp.cBoardId = pClientInfo->strCoilDesp.strComCoilDesp.cBoardId;
    strChannelInfoSet.strCoilDesp.strComCoilDesp.cComId = pClientInfo->strCoilDesp.strComCoilDesp.cComId;
    strChannelInfoSet.strCoilDesp.strComCoilDesp.cFrontCoilId = pClientInfo->strCoilDesp.strComCoilDesp.cFrontCoilId;
    strChannelInfoSet.strCoilDesp.strComCoilDesp.cGroupId = pClientInfo->strCoilDesp.strComCoilDesp.cGroupId;

    strChannelInfoSet.strCoilDesp.strGpioCoilDesp.cBehindCoilId = pClientInfo->strCoilDesp.strGpioCoilDesp.cBehindCoilId;
    strChannelInfoSet.strCoilDesp.strGpioCoilDesp.cFrontCoilId = pClientInfo->strCoilDesp.strGpioCoilDesp.cFrontCoilId;
    //memcpy(&(strChannelInfoSet.strCoilDesp), &(pClientInfo->strCoilDesp), sizeof(strChannelInfoSet.strCoilDesp));

    strChannelInfoSet.strRedLDesp.strEPComRedLightDesp.cCarRoadId = pClientInfo->strRedLDesp.strEPComRedLightDesp.cCarRoadId;
    strChannelInfoSet.strRedLDesp.strEPComRedLightDesp.cComId = pClientInfo->strRedLDesp.strEPComRedLightDesp.cComId;
    strChannelInfoSet.strRedLDesp.strEPGpioRedLightDesp.cGpioId = pClientInfo->strRedLDesp.strEPGpioRedLightDesp.cGpioId;
    //memcpy(&(strChannelInfoSet.strRedLDesp), &(pClientInfo->strRedLDesp), sizeof(strChannelInfoSet.strRedLDesp));

    //XCSPTL_INFO("\n -----ep channel red light cCarRoadId:%d-----\n", strChannelInfoSet.strRedLDesp.strEPComRedLightDesp.cCarRoadId);

    strChannelInfoSet.strRoadArea.usAreaHeight = ntohs(pClientInfo->strRoadArea.usAreaHeight);
    strChannelInfoSet.strRoadArea.usAreaPosX = ntohs(pClientInfo->strRoadArea.usAreaPosX);
    strChannelInfoSet.strRoadArea.usAreaPosY = ntohs(pClientInfo->strRoadArea.usAreaPosY);
    strChannelInfoSet.strRoadArea.usAreaWidth = ntohs(pClientInfo->strRoadArea.usAreaWidth);

    
    strChannelInfoSet.usOSDPosX = ntohs(pClientInfo->usOSDPosX);
    strChannelInfoSet.usOSDPosY = ntohs(pClientInfo->usOSDPosY);
    strChannelInfoSet.usRedLConnectMode = ntohs(pClientInfo->usRedLConnectMode);
    strChannelInfoSet.usRedLDetectType = ntohs(pClientInfo->usRedLDetectType);
    strChannelInfoSet.usVehicleLinkMode = ntohs(pClientInfo->usVehicleLinkMode);
    strChannelInfoSet.uiForcedRedLight = pClientInfo->ucSimSnapFlag;

    strChannelInfoSet.ucMinPlatePixelSize = pClientInfo->ucMinPlatePixelSize;
    strChannelInfoSet.ucMaxPlatePixelSize = pClientInfo->ucMaxPlatePixelSize;
    strChannelInfoSet.ucCorrEveningThresh = pClientInfo->ucCorrEveningThresh;

    XCSPTL_INFO("ep set minPlatePix:%d.\n", pClientInfo->ucMinPlatePixelSize);
    XCSPTL_INFO("ep set maxPlatePix:%d.\n", pClientInfo->ucMaxPlatePixelSize);
    XCSPTL_INFO("ep set EventThresh:%d.\n", pClientInfo->ucCorrEveningThresh);    


    /*Start of ITS on 2011-4-2 14:8 2.0.0.1,只要配置客户端有配置，则主动把切换状态切回到初始状态*/
    strChannelInfoSet.cDetectType       = pClientInfo->cDetectType;
    strChannelInfoSet.cCoilSwitchFlag   = pClientInfo->cCoilSwitchFlag;
    strChannelInfoSet.cCoilSwitchSta    = SWITCH_DEFAULT_VAL;
    strChannelInfoSet.cRedLightDectSta  = SWITCH_DEFAULT_VAL;
    strChannelInfoSet.usFlashLightType  = ntohs(pClientInfo->usFlashLightType);
    strChannelInfoSet.cInLightSta       = (char)(strChannelInfoSet.usFlashLightType);
    strChannelInfoSet.cVLDecEn          = pClientInfo->cVLDecEn;
    /*End of ITS on 2011-4-2 14:8 2.0.0.1*/

    XCSPTL_INFO("ep set coil dec switch flag:%d.\n", pClientInfo->cCoilSwitchFlag);
    XCSPTL_INFO("ep set coil dec type:%d.\n", pClientInfo->cDetectType);
    XCSPTL_INFO("ep set coil dec sta:%d.\n", pClientInfo->cCoilSwitchSta);    


    XCSPTL_INFO("ep set inlight type:%d.\n", strChannelInfoSet.usFlashLightType);
    XCSPTL_INFO("ep set inlight sta:%d.\n", strChannelInfoSet.cInLightSta);

    strChannelInfoSet.cCarNoDir = pClientInfo->cCarNoDir;
    strChannelInfoSet.cCarTrackEn = pClientInfo->cCarTrackEn;
    strChannelInfoSet.cCarSnapDir = pClientInfo->cCarSnapDir;


    usTmpVer = ntohs(pClientInfo->usEPProVer);
    if(( XCS_PRO_INVALID_VER0 == usTmpVer ) || ( XCS_PRO_INVALID_VER1 == usTmpVer))
    {
        strChannelInfoSet.cLaneLoopDispFlag = 1;
        strChannelInfoSet.cVLDispFlag = 1;
        
        strChannelInfoSet.strTrafficLight[0].cEnable = 1;
        strChannelInfoSet.strTrafficLight[0].cLightType = 1;
        strChannelInfoSet.strTrafficLight[0].cVehicleNum = strChannelInfoSet.strRedLDesp.strEPComRedLightDesp.cCarRoadId;
        XCSPTL_INFO("invalid ep channel set vlDispFlag:%d,laneLoopDispFlag:%d\n", strChannelInfoSet.cVLDispFlag, strChannelInfoSet.cLaneLoopDispFlag);
    }
    else if(( XCS_PRO_EP_VER == usTmpVer) || (XCS_PRO_EP_VER_2 == usTmpVer))
    {
        
        strChannelInfoSet.cLaneDir = pClientInfo->cLaneDir;

        for(i =0; i< MAX_LIGHT_TYPE; i++)
        {
            strChannelInfoSet.strTrafficLight[i].cEnable = pClientInfo->strTrafficLight[i].cEnable;
            strChannelInfoSet.strTrafficLight[i].cLightType = pClientInfo->strTrafficLight[i].cLightType;
            strChannelInfoSet.strTrafficLight[i].cVehicleNum = pClientInfo->strTrafficLight[i].cVehicleNum;
            XCSPTL_INFO("set %d_light,enable:%d,type:%d,num:%d.\n", i, strChannelInfoSet.strTrafficLight[i].cEnable,\
            strChannelInfoSet.strTrafficLight[i].cLightType,strChannelInfoSet.strTrafficLight[i].cVehicleNum);
        }

        strChannelInfoSet.strTrafficLArea.usAreaPosX = ntohs(pClientInfo->strTrafficLigntArea.usAreaPosX);
        strChannelInfoSet.strTrafficLArea.usAreaPosY = ntohs(pClientInfo->strTrafficLigntArea.usAreaPosY);
        strChannelInfoSet.strTrafficLArea.usAreaWidth = ntohs(pClientInfo->strTrafficLigntArea.usAreaWidth);
        strChannelInfoSet.strTrafficLArea.usAreaHeight = ntohs(pClientInfo->strTrafficLigntArea.usAreaHeight);

        for(i =0; i< 4; i++)
        {
            strChannelInfoSet.strVLPoint[i].usVLX1 = ntohs(pClientInfo->strVLPoint[i].usVLX1);
            strChannelInfoSet.strVLPoint[i].usVLY1 = ntohs(pClientInfo->strVLPoint[i].usVLY1);
        }
        //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

        strChannelInfoSet.cLaneLoopDispFlag = pClientInfo->cLaneLoopDispFlag;
        strChannelInfoSet.cVLDispFlag = pClientInfo->cVLDispFlag;


        strChannelInfoSet.cCarDistance = pClientInfo->cCarDistance;
        strChannelInfoSet.cFrameInterval = pClientInfo->cFrameInterval;
        XCSPTL_INFO("ep channel set vlDispFlag:%d,laneLoopDispFlag:%d\n", strChannelInfoSet.cVLDispFlag, strChannelInfoSet.cLaneLoopDispFlag);

        for(i =0; i< 2; i++)
        {
            strChannelInfoSet.strNoLaneVLPoint[i].usVLX1 = ntohs(pClientInfo->strNoLaneVLPoint[i].usVLX1);
            strChannelInfoSet.strNoLaneVLPoint[i].usVLY1 = ntohs(pClientInfo->strNoLaneVLPoint[i].usVLY1);
        }
       
    }
    else
    {
        strChannelInfoSet.cLaneDir = pClientInfo->cLaneDir;

        for(i =0; i< MAX_LIGHT_TYPE; i++)
        {
            strChannelInfoSet.strTrafficLight[i].cEnable = pClientInfo->strTrafficLight[i].cEnable;
            strChannelInfoSet.strTrafficLight[i].cLightType = pClientInfo->strTrafficLight[i].cLightType;
            strChannelInfoSet.strTrafficLight[i].cVehicleNum = pClientInfo->strTrafficLight[i].cVehicleNum;
            XCSPTL_INFO("set %d_light,enable:%d,type:%d,num:%d.\n", i, strChannelInfoSet.strTrafficLight[i].cEnable,\
            strChannelInfoSet.strTrafficLight[i].cLightType,strChannelInfoSet.strTrafficLight[i].cVehicleNum);
        }

        strChannelInfoSet.strTrafficLArea.usAreaPosX = ntohs(pClientInfo->strTrafficLigntArea.usAreaPosX);
        strChannelInfoSet.strTrafficLArea.usAreaPosY = ntohs(pClientInfo->strTrafficLigntArea.usAreaPosY);
        strChannelInfoSet.strTrafficLArea.usAreaWidth = ntohs(pClientInfo->strTrafficLigntArea.usAreaWidth);
        strChannelInfoSet.strTrafficLArea.usAreaHeight = ntohs(pClientInfo->strTrafficLigntArea.usAreaHeight);

        for(i =0; i< 4; i++)
        {
            strChannelInfoSet.strVLPoint[i].usVLX1 = ntohs(pClientInfo->strVLPoint[i].usVLX1);
            strChannelInfoSet.strVLPoint[i].usVLY1 = ntohs(pClientInfo->strVLPoint[i].usVLY1);
        }
        //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

        strChannelInfoSet.cLaneLoopDispFlag = pClientInfo->cLaneLoopDispFlag;
        strChannelInfoSet.cVLDispFlag = pClientInfo->cVLDispFlag;

        for(i =0; i< 2; i++)
        {
            strChannelInfoSet.strNoLaneVLPoint[i].usVLX1 = ntohs(pClientInfo->strNoLaneVLPoint[i].usVLX1);
            strChannelInfoSet.strNoLaneVLPoint[i].usVLY1 = ntohs(pClientInfo->strNoLaneVLPoint[i].usVLY1);
        }
        XCSPTL_INFO("other ep channel set vlDispFlag:%d,laneLoopDispFlag:%d,ver:0x%x\n", strChannelInfoSet.cVLDispFlag, strChannelInfoSet.cLaneLoopDispFlag, usTmpVer);
       
    }

    pClientInfo->szLaneId[7] =0;
    strcpy(strChannelInfoSet.szLaneId, pClientInfo->szLaneId);
    strChannelInfoSet.usCoilDist = ntohs(pClientInfo->usCoilDist);

    strChannelInfoSet.ucPPDelaySnapFrames = pClientInfo->ucPPDelaySnapFrames;

    strChannelInfoSet.cSwitchLaneEn 		= pClientInfo->cSwitchLaneEn;
    strChannelInfoSet.cSwitchLaneLeftNo  	= pClientInfo->cSwitchLaneLeftNo;
    strChannelInfoSet.cSwitchLaneStraightNo = pClientInfo->cSwitchLaneStraightNo;

	XCSPTL_INFO("lane:%d,switchlaneEn:%d,leftNo:%d,straightNo:%d.\n", \
	strChannelInfoSet.uiCarRoadId,\
	pClientInfo->cSwitchLaneEn,\
	pClientInfo->cSwitchLaneLeftNo,\
	pClientInfo->cSwitchLaneStraightNo);
 
    ret = SysConfSet(e_ELEC_POLICE_VEHICLE_CONF, &strChannelInfoSet, strChannelInfoSet.uiCarRoadId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set channel Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_CAR_CHANNEL_INFO_SET;
    strTmpSysMsg.ucCarRoadId = strChannelInfoSet.uiCarRoadId;
    strTmpSysMsg.aucData[0] = pClientInfo->ucSimSnapFlag;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police channel info failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police channel info msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::electPoliceRedLightStaQuery(XCS_CLIENT * pClintSocket)
{
    unsigned int uiCarRoadId =0;
    PSTR_ELECT_POLICE_RED_L_STA_PC_QUERY pPcQueryInfo = (PSTR_ELECT_POLICE_RED_L_STA_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    PSTR_ELEC_POLICE_RED_L_STA_QUERY pStrRedStaSend = (PSTR_ELEC_POLICE_RED_L_STA_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    uiCarRoadId = ntohl(pPcQueryInfo->uiCarRoadId);
    if(uiCarRoadId > 0)
        uiCarRoadId--;
    XCSPTL_INFO("query carRoadId:%d, valid LaneId:%d.\n", pPcQueryInfo->uiCarRoadId, uiCarRoadId);
    pStrRedStaSend->uiCarRoadId = pPcQueryInfo->uiCarRoadId;
    pStrRedStaSend->ucRedSta = redLightNormalStaQuery(uiCarRoadId);
    memcpy(pStrRedStaSend->strTrafficLight, g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL, sizeof( pStrRedStaSend->strTrafficLight ));

	pStrRedStaSend->cSwitchLaneLeftNo = g_EPLaneSwitchInfo[uiCarRoadId].cSwitchLaneLeftNo;
	pStrRedStaSend->cSwitchLaneLeftNoSta = g_EPLaneSwitchInfo[uiCarRoadId].cSwitchLaneLeftNoSta;

	pStrRedStaSend->cSwitchLaneStraightNo = g_EPLaneSwitchInfo[uiCarRoadId].cSwitchLaneStraightNo;
	pStrRedStaSend->cSwitchLaneStraightNoSta = g_EPLaneSwitchInfo[uiCarRoadId].cSwitchLaneStraightNoSta;
	
    
    XCSPTL_INFO("query carRoadId:%d, red lignt sta:%d.\n", uiCarRoadId, pStrRedStaSend->ucRedSta);
    
    XCSPTL_INFO("query switchlaneLeftNo:%d, switchlaneLeftNo sta:%d.\n", \
    pStrRedStaSend->cSwitchLaneLeftNo,\
    pStrRedStaSend->cSwitchLaneLeftNoSta);

    XCSPTL_ERROR("query switchlaneStaighttNo:%d, switchlaneStaighttNo sta:%d.\n", \
    pStrRedStaSend->cSwitchLaneStraightNo,\
    pStrRedStaSend->cSwitchLaneStraightNoSta);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_ELEC_POLICE_RED_L_STA_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_RED_LIGHT_STA_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}


int CHandleXcsPtl::electPoliceSimuHandSnap(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    unsigned int uiCarRoadId =0;
    PSTR_ELECT_POLICE_SIMU_RUN_SNAP pPcSetInfo = (PSTR_ELECT_POLICE_SIMU_RUN_SNAP)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    uiCarRoadId = ntohl(pPcSetInfo->uiCarRoadId);
    if(uiCarRoadId > 0)
        uiCarRoadId--;
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_ELECT_POLICE_SIMU_RUN_SNAP;
    strTmpSysMsg.ucCarRoadId = uiCarRoadId;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police force red light failed.\n");
    }
    else
    {
        XCSPTL_INFO("send simu elect police force red light msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

#if 0
int CHandleXcsPtl::electPoliceForceRedLQuery(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    unsigned int uiCarRoadId =0;
    PSTR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY pPcQueryInfo = (PSTR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    PSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY pStrRedStaSend = (PSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    uiCarRoadId = ntohl(pPcQueryInfo->uiCarRoadId);
    if(uiCarRoadId > 0)
        uiCarRoadId--;
    pStrRedStaSend->uiCarRoadId = pPcQueryInfo->uiCarRoadId;

    ELEC_POLICE_VEHICLE_CONF_INFO strChannelInfoGet;
    memset(&strChannelInfoGet, 0, sizeof(strChannelInfoGet));
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strChannelInfoGet, uiCarRoadId);
    if(0 > ret)
    {
        XCSPTL_INFO("get sys conf red light sta failed\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    pStrRedStaSend->ucForceRedSta = strChannelInfoGet.uiForcedRedLight;

    XCSPTL_INFO("force red light sta:%d\n", pStrRedStaSend->ucForceRedSta);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_ELEC_POLICE_RED_L_STA_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceForceRedLSet(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    unsigned int uiCarRoadId =0;
    STR_ELECT_POLICE_RED_LIGHT_ENFORCE_SET *pPcSetInfo = (STR_ELECT_POLICE_RED_LIGHT_ENFORCE_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    uiCarRoadId = ntohl(pPcSetInfo->uiCarRoadId);
    if(uiCarRoadId >0)
        uiCarRoadId--;

    ELEC_POLICE_VEHICLE_CONF_INFO strChannelInfoGet;
    memset(&strChannelInfoGet, 0, sizeof(strChannelInfoGet));
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strChannelInfoGet, uiCarRoadId);
    strChannelInfoGet.uiForcedRedLight = pPcSetInfo->ucForceRedSta;
    ret = SysConfSet(e_ELEC_POLICE_VEHICLE_CONF, &strChannelInfoGet, uiCarRoadId);

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_RED_LIGHT_ENFORCE_SET;
    strTmpSysMsg.ucCarRoadId = uiCarRoadId;
    strTmpSysMsg.aucData[0] = pPcSetInfo->ucForceRedSta;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send ep force red light msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("send ep force red light msg ok.\n");
    }
    XCSPTL_INFO("force red light sta:%d\n", pPcSetInfo->ucForceRedSta);

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}
#endif

int CHandleXcsPtl::electPoliceFtpInfoQuery(XCS_CLIENT * pClintSocket)
{
    FTP_CONF_INFO strFtpInfoGet;
    
    int nRet = SysConfGet(e_ELEC_POLICE_FTP_CONF, &strFtpInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("electPolice get electPolice Ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);
    PSTR_ELECT_POLICE_FTP_INFO_QUERY pPcQueryInfo = (PSTR_ELECT_POLICE_FTP_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    pPcQueryInfo->uiEnable = htonl(strFtpInfoGet.uiEnable);
    pPcQueryInfo->uiFtpIP = htonl(strFtpInfoGet.uiFtpIP);
    pPcQueryInfo->usFtpPort = htons(strFtpInfoGet.usFtpPort);

    strFtpInfoGet.szUserName[127] =0;
    strFtpInfoGet.szUserPasswd[127] =0;
    strcpy(pPcQueryInfo->szUserName, strFtpInfoGet.szUserName);
    strcpy(pPcQueryInfo->szUserPasswd, strFtpInfoGet.szUserPasswd);


    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_ELECT_POLICE_FTP_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_FTP_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceFtpInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    FTP_CONF_INFO strFtpInfoSet;
    STR_ELECT_POLICE_FTP_INFO_SET *pClientInfo = (STR_ELECT_POLICE_FTP_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    strFtpInfoSet.uiEnable = ntohl(pClientInfo->uiEnable);
    strFtpInfoSet.uiFtpIP = ntohl(pClientInfo->uiFtpIP);
    strFtpInfoSet.usFtpPort = ntohs(pClientInfo->usFtpPort);

    pClientInfo->szUserName[127] =0;
    pClientInfo->szUserPasswd[127] =0;
    strcpy(strFtpInfoSet.szUserName, pClientInfo->szUserName);
    strcpy(strFtpInfoSet.szUserPasswd, pClientInfo->szUserPasswd);


    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    ret = SysConfSet(e_ELEC_POLICE_FTP_CONF, &strFtpInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set electPolice ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_FTP_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police ftp Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police ftp Conf msg ok.\n");
    }


    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::epBackupsFtpInfoQuery(XCS_CLIENT * pClintSocket)
{
    FTP_CONF_INFO strFtpInfoGet;
    
    int nRet = SysConfGet(e_EP_BACKUPS_FTP_CONF, &strFtpInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("ep get bayonet backups Ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    XCSPTL_INFO("ep query backups ftp enable:%d,ip:0x%x,port %d name %s passwd %s .\n",
        strFtpInfoGet.uiEnable,strFtpInfoGet.uiFtpIP,strFtpInfoGet.usFtpPort,strFtpInfoGet.szUserName,strFtpInfoGet.szUserPasswd);
    
    PSTR_EP_BACKUPS_FTP_INFO_QUERY pPcQueryInfo = (PSTR_EP_BACKUPS_FTP_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    pPcQueryInfo->uiEnable = htonl(strFtpInfoGet.uiEnable);
    pPcQueryInfo->uiFtpIP = htonl(strFtpInfoGet.uiFtpIP);
    pPcQueryInfo->usFtpPort = htons(strFtpInfoGet.usFtpPort);

    strFtpInfoGet.szUserName[31] =0;
    strFtpInfoGet.szUserPasswd[31] =0;
    strncpy(pPcQueryInfo->szUserName, strFtpInfoGet.szUserName, 31);
    strncpy(pPcQueryInfo->szUserPasswd, strFtpInfoGet.szUserPasswd, 31);
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_BAYONET_FTP_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_EP_BACKUPS_FTP_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::epBackupsFtpInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    FTP_CONF_INFO strFtpInfoSet;
    STR_EP_BACKUPS_FTP_INFO_SET *pClientInfo = (STR_EP_BACKUPS_FTP_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    strFtpInfoSet.uiEnable = ntohl(pClientInfo->uiEnable);
    strFtpInfoSet.uiFtpIP = ntohl(pClientInfo->uiFtpIP);
    strFtpInfoSet.usFtpPort = ntohs(pClientInfo->usFtpPort);

    pClientInfo->szUserName[31] =0;
    pClientInfo->szUserPasswd[31] =0;
    strncpy(strFtpInfoSet.szUserName, pClientInfo->szUserName, 31);
    strncpy(strFtpInfoSet.szUserPasswd, pClientInfo->szUserPasswd, 31);

    XCSPTL_INFO("set ep backups ftp enable:%d,ip:0x%x,port %d name %s passwd %s .\n",
        strFtpInfoSet.uiEnable,strFtpInfoSet.uiFtpIP,strFtpInfoSet.usFtpPort,strFtpInfoSet.szUserName,strFtpInfoSet.szUserPasswd);

    ret = SysConfSet(e_EP_BACKUPS_FTP_CONF, &strFtpInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("ep Set ep backups ftp Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_FTP_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send ep backups ftp Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send ep backups ftp Conf msg ok.\n");
    }

    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::epTimeRQuery(XCS_CLIENT *pClintSocket)
{
    int i=0;
    int ret=0;
    char cLane =0;

    EP_TIME_RANGE_INFO strSysEpTimeRange[EP_FORBID_MAX_DIR];
    
    PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY pPcQueryInfo = (PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    cLane = pPcQueryInfo->ucLaneId;
    if(cLane > 0)
    {
        cLane -= 1;
    }

    XCSPTL_INFO("q ep time range,lane:%d\n", cLane);
    PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY pStrSend = (PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    ret = SysConfGet(e_EP_RANGE_TIME_CONF, strSysEpTimeRange, cLane);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get ep time range Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    pStrSend->cLaneId = pPcQueryInfo->ucLaneId;
    for(i =0; i< EP_FORBID_MAX_DIR; i++)
    {
        pStrSend->strForbidDirInfo[i].cForbidDirType = strSysEpTimeRange[i].cForbidType;
        pStrSend->strForbidDirInfo[i].cRange1Enable = strSysEpTimeRange[i].cRange1Enable;
        pStrSend->strForbidDirInfo[i].cRange2Enable = strSysEpTimeRange[i].cRange2Enable;
        pStrSend->strForbidDirInfo[i].iBeginTime1 = htonl(strSysEpTimeRange[i].iBeginTime1);
        pStrSend->strForbidDirInfo[i].iLastTime1 = htonl(strSysEpTimeRange[i].iLastTime1);
        pStrSend->strForbidDirInfo[i].iBeginTime2 = htonl(strSysEpTimeRange[i].iBeginTime2);
        pStrSend->strForbidDirInfo[i].iLastTime2 = htonl(strSysEpTimeRange[i].iLastTime2);
    }
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_TIME_RANGE_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::epTimeRSet(XCS_CLIENT *pClintSocket)
{
    int i=0;
    int ret=0;
    char cLaneId =0;
    STR_ELECT_POLICE_FORBIT_TIME_RANGE_SET *pClientInfo = (STR_ELECT_POLICE_FORBIT_TIME_RANGE_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    EP_TIME_RANGE_INFO strSysEpTimeRange[EP_FORBID_MAX_DIR];

    memset(strSysEpTimeRange, 0, sizeof(strSysEpTimeRange));
    
    cLaneId = pClientInfo->cLaneId;
    if(cLaneId > 0)
    {
        cLaneId -= 1;
    }
    
    ret = SysConfGet(e_EP_RANGE_TIME_CONF, strSysEpTimeRange, cLaneId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set get ep time range Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    for(i =0; i< EP_FORBID_MAX_DIR; i++)
    {
        strSysEpTimeRange[i].cForbidType = pClientInfo->strForbidDirInfo[i].cForbidDirType;
        strSysEpTimeRange[i].cRange1Enable = pClientInfo->strForbidDirInfo[i].cRange1Enable;
        strSysEpTimeRange[i].cRange2Enable = pClientInfo->strForbidDirInfo[i].cRange2Enable;
        if(strSysEpTimeRange[i].cRange1Enable > 0)
        {
            strSysEpTimeRange[i].iBeginTime1 = ntohl(pClientInfo->strForbidDirInfo[i].iBeginTime1);
            strSysEpTimeRange[i].iLastTime1 = ntohl(pClientInfo->strForbidDirInfo[i].iLastTime1);
        }
        if(strSysEpTimeRange[i].cRange2Enable > 0)
        {
            strSysEpTimeRange[i].iBeginTime2 = ntohl(pClientInfo->strForbidDirInfo[i].iBeginTime2);
            strSysEpTimeRange[i].iLastTime2 = ntohl(pClientInfo->strForbidDirInfo[i].iLastTime2);
        }
    }
    ret = SysConfSet(e_EP_RANGE_TIME_CONF, strSysEpTimeRange, cLaneId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set ep time range Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.ucCarRoadId = cLaneId;
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_TIME_RANGE_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send EP time range Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send EP time range info msg ok.\n");
    }
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}


int CHandleXcsPtl::epAssistRedLightQuery(XCS_CLIENT *pClintSocket)
{
    int i=0;
    int ret=0;
    char cLane =0;

    EP_ASSIST_RED_LIGHT_INFO strSysEpAssistRD[EP_ASSIST_RED_LIGHT_NUM];
    
    PSTR_EP_ASSIST_RD_PC_QUERY pPcQueryInfo = (PSTR_EP_ASSIST_RD_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    cLane = pPcQueryInfo->ucLaneId;
    if(cLane > 0)
    {
        cLane -= 1;
    }

    XCSPTL_INFO("q ep assist rd,lane:%d\n", cLane);
    PSTR_EP_ASSIST_RD_QUERY pStrSend = (PSTR_EP_ASSIST_RD_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    ret = SysConfGet(e_EP_ASSIST_REDLIGHT_CONF, strSysEpAssistRD, cLane);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get ep assist red light failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    pStrSend->cLaneId = pPcQueryInfo->ucLaneId;
    
    memset(pStrSend->strEpAssistRDInfo, 0, sizeof( pStrSend->strEpAssistRDInfo ));
    
    for(i =0; i< EP_ASSIST_RED_LIGHT_NUM; i++)
    {
        pStrSend->strEpAssistRDInfo[i].cEnableFlag = strSysEpAssistRD[i].cEnableFlag;
        if( strSysEpAssistRD[i].cEnableFlag > 0 )
        {
            pStrSend->strEpAssistRDInfo[i].cRedLightNum = strSysEpAssistRD[i].cRedLightNum +1;
            pStrSend->strEpAssistRDInfo[i].iBeginTime1 = htonl(strSysEpAssistRD[i].iBeginTime1);
            pStrSend->strEpAssistRDInfo[i].iLastTime1 = htonl(strSysEpAssistRD[i].iLastTime1);
            pStrSend->strEpAssistRDInfo[i].iBeginTime2 = htonl(strSysEpAssistRD[i].iBeginTime2);
            pStrSend->strEpAssistRDInfo[i].iLastTime2 = htonl(strSysEpAssistRD[i].iLastTime2);
            pStrSend->strEpAssistRDInfo[i].iBeginTime3 = htonl(strSysEpAssistRD[i].iBeginTime3);
            pStrSend->strEpAssistRDInfo[i].iLastTime3 = htonl(strSysEpAssistRD[i].iLastTime3);
        }
        XCSPTL_INFO("1assist red light get info.\n");
        XCSPTL_INFO("seq:%d,rlNum:%d,En:%d,bt1:0x%x,et1:0x%x,bt2:0x%x,et2:0x%x,bt3:0x%x,et3:0x%x.\n",\
        i,\
        strSysEpAssistRD[i].cRedLightNum,\
        strSysEpAssistRD[i].cEnableFlag,\
        strSysEpAssistRD[i].iBeginTime1,\
        strSysEpAssistRD[i].iLastTime1,\
        strSysEpAssistRD[i].iBeginTime2,\
        strSysEpAssistRD[i].iLastTime2,\
        strSysEpAssistRD[i].iBeginTime3,\
        strSysEpAssistRD[i].iLastTime3);
    }
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_EP_ASSIST_RD_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_EP_ASSIST_RED_LIGHT_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::epAssistRedLightSet(XCS_CLIENT *pClintSocket)
{
    int i=0;
    int ret=0;
    char cLaneId =0;
    STR_EP_ASSIST_RD_SET *pClientInfo = (STR_EP_ASSIST_RD_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    EP_ASSIST_RED_LIGHT_INFO strSysEpAssistRD[EP_ASSIST_RED_LIGHT_NUM];

    cLaneId = pClientInfo->cLaneId;
    if(cLaneId > 0)
    {
        cLaneId -= 1;
    }
    
    ret = SysConfGet(e_EP_ASSIST_REDLIGHT_CONF, strSysEpAssistRD, cLaneId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("sys set, get ep assist red light Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    memset(strSysEpAssistRD, 0, sizeof(strSysEpAssistRD));

    for(i =0; i< EP_ASSIST_RED_LIGHT_NUM; i++)
    {
        strSysEpAssistRD[i].cEnableFlag = pClientInfo->strEpAssistRDInfo[i].cEnableFlag;
        if(strSysEpAssistRD[i].cEnableFlag > 0)
        {
            strSysEpAssistRD[i].cRedLightNum = pClientInfo->strEpAssistRDInfo[i].cRedLightNum -1;
            strSysEpAssistRD[i].iBeginTime1 = ntohl(pClientInfo->strEpAssistRDInfo[i].iBeginTime1);
            strSysEpAssistRD[i].iLastTime1 = ntohl(pClientInfo->strEpAssistRDInfo[i].iLastTime1);
            strSysEpAssistRD[i].iBeginTime2 = ntohl(pClientInfo->strEpAssistRDInfo[i].iBeginTime2);
            strSysEpAssistRD[i].iLastTime2 = ntohl(pClientInfo->strEpAssistRDInfo[i].iLastTime2);
            strSysEpAssistRD[i].iBeginTime3 = ntohl(pClientInfo->strEpAssistRDInfo[i].iBeginTime3);
            strSysEpAssistRD[i].iLastTime3 = ntohl(pClientInfo->strEpAssistRDInfo[i].iLastTime3);
        }
        XCSPTL_INFO("1assist red light set info.\n");
        XCSPTL_INFO("seq:%d,rlNum:%d,En:%d,bt1:0x%x,et1:0x%x,bt2:0x%x,et2:0x%x,bt3:0x%x,et3:0x%x.\n",\
        i,\
        strSysEpAssistRD[i].cRedLightNum,\
        strSysEpAssistRD[i].cEnableFlag,\
        strSysEpAssistRD[i].iBeginTime1,\
        strSysEpAssistRD[i].iLastTime1,\
        strSysEpAssistRD[i].iBeginTime2,\
        strSysEpAssistRD[i].iLastTime2,\
        strSysEpAssistRD[i].iBeginTime3,\
        strSysEpAssistRD[i].iLastTime3);
    }
    ret = SysConfSet(e_EP_ASSIST_REDLIGHT_CONF, strSysEpAssistRD, cLaneId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set ep assist red light Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.ucCarRoadId = cLaneId;
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_ASSIST_RED_LIGHT_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send EP assist red light Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send EP assist red light info msg ok.\n");
    }
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::electPoliceLprInfoQuery(XCS_CLIENT * pClintSocket)
{
    EP_LPR_CONF_INFO strEpLprInfoGet;
    
    int nRet = SysConfGet(e_EP_LPR_INFO_CONF, &strEpLprInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("electPolice get electPolice Lpr Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    PEP_LPR_CONF_INFO_QUERY pPcQueryInfo = (PEP_LPR_CONF_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    pPcQueryInfo->ucLPREn = strEpLprInfoGet.ucLPREn;
    pPcQueryInfo->ucLPRCConfidence = strEpLprInfoGet.ucLPRCConfidence;
    pPcQueryInfo->usLPRWidth = htons(strEpLprInfoGet.usLPRWidth);
    pPcQueryInfo->usLPRHeight = htons(strEpLprInfoGet.usLPRHeight);

    strEpLprInfoGet.szLPRFirstChar[5] =0;
    strcpy(pPcQueryInfo->szLPRFirstChar, strEpLprInfoGet.szLPRFirstChar);

    XCSPTL_ERROR("ep lpr firstCode:%s.\n", pPcQueryInfo->szLPRFirstChar);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(EP_LPR_CONF_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_LPR_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceLprInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    EP_LPR_CONF_INFO strEpLprInfoSet;
    EP_LPR_CONF_INFO_SET *pClientInfo = (EP_LPR_CONF_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    strEpLprInfoSet.ucLPREn= pClientInfo->ucLPREn;
    strEpLprInfoSet.ucLPRCConfidence = pClientInfo->ucLPRCConfidence;
    strEpLprInfoSet.usLPRWidth = ntohs(pClientInfo->usLPRWidth);
    strEpLprInfoSet.usLPRHeight = ntohs(pClientInfo->usLPRHeight);

    pClientInfo->szLPRFirstChar[5] =0;
    strcpy(strEpLprInfoSet.szLPRFirstChar, pClientInfo->szLPRFirstChar);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    ret = SysConfSet(e_EP_LPR_INFO_CONF, &strEpLprInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set electPolice lpr Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_LPR_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police lpr Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police lpr Conf msg ok.\n");
    }


    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::electPoliceIrregInfoQuery(XCS_CLIENT * pClintSocket)
{
    EP_IRREG_CONF_INFO strEpIrregInfoGet;
    
    int nRet = SysConfGet(e_EP_IRREG_INFO_CONF, &strEpIrregInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("electPolice get electPolice irreg Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    PEP_IRREG_CONF_INFO_QUERY pPcQueryInfo = (PEP_IRREG_CONF_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

	pPcQueryInfo->cDispFlag = strEpIrregInfoGet.cDispFlag;
	
    memcpy((char *)pPcQueryInfo+1,(char *)&strEpIrregInfoGet+4, sizeof(strEpIrregInfoGet)-4);
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(EP_IRREG_CONF_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_IRREG_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceIrregInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    EP_IRREG_CONF_INFO strEpIrregInfoSet;
    EP_IRREG_CONF_INFO_SET *pClientInfo = (EP_IRREG_CONF_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    strEpIrregInfoSet.cDispFlag = pClientInfo->cDispFlag;
    
    memcpy((char *)&strEpIrregInfoSet+4, (char *)pClientInfo+1, sizeof(strEpIrregInfoSet)-4);

    ret = SysConfSet(e_EP_IRREG_INFO_CONF, &strEpIrregInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set electPolice irreg Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_IRREG_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police irreg Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police irreg Conf msg ok.\n");
    }


    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::electPoliceNewAddIrregInfoQuery(XCS_CLIENT * pClintSocket)
{
    EP_NEW_ADD_IRREG_CONF_INFO strEpNewAddIrregInfoGet;
    
    int nRet = SysConfGet(e_EP_NEWADD_IRREG_INFO_CONF, &strEpNewAddIrregInfoGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("electPolice get electPolice new add irreg Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    PEP_NEWADD_IRREG_CONF_INFO_QUERY pPcQueryInfo = (PEP_NEWADD_IRREG_CONF_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    memcpy((char *)pPcQueryInfo,(char *)&strEpNewAddIrregInfoGet, sizeof(strEpNewAddIrregInfoGet));
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(EP_NEWADD_IRREG_CONF_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_ELECT_POLICE_NEWADD_IRREG_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::electPoliceNewAddIrregInfoSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    EP_NEW_ADD_IRREG_CONF_INFO strEpIrregInfoSet;
    EP_NEWADD_IRREG_CONF_INFO_SET *pClientInfo = (EP_NEWADD_IRREG_CONF_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    memcpy((char *)&strEpIrregInfoSet, (char *)pClientInfo, sizeof(strEpIrregInfoSet));

    ret = SysConfSet(e_EP_NEWADD_IRREG_INFO_CONF, &strEpIrregInfoSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set electPolice new add irreg Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_IRREG_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police newadd irreg Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police newadd irreg Conf msg ok.\n");
    }


    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}


int CHandleXcsPtl::epSOTNoEntryQuery(XCS_CLIENT *pClintSocket)
{
    int ret=0;
    char cLane =0;

    EP_STOP_ON_TRAFFIC_NO_ENTRY strSysEpSOTNoEntry;
    
    PSTR_EP_SOT_NO_ENTRY_PC_QUERY pPcQueryInfo = (PSTR_EP_SOT_NO_ENTRY_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    cLane = pPcQueryInfo->ucLaneId;
    if(cLane > 0)
    {
        cLane -= 1;
    }

    XCSPTL_INFO("q ep sot no entry,lane:%d\n", cLane);
    PSTR_EP_SOT_NOENTRY_QUERY pStrSend = (PSTR_EP_SOT_NOENTRY_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    ret = SysConfGet(e_EP_SOT_NOENTRY_CONF, &strSysEpSOTNoEntry, cLane);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get ep sot no entry sys conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    pStrSend->cLaneId = pPcQueryInfo->ucLaneId;
    
    memset(&(pStrSend->strEpSOTNoEntryInfo), 0, sizeof( pStrSend->strEpSOTNoEntryInfo ));
    
    pStrSend->strEpSOTNoEntryInfo.cEnableFlag = strSysEpSOTNoEntry.cEnableFlag;
    if( strSysEpSOTNoEntry.cEnableFlag > 0 )
    {
        pStrSend->strEpSOTNoEntryInfo.iBeginTime1 = htonl(strSysEpSOTNoEntry.iBeginTime1);
        pStrSend->strEpSOTNoEntryInfo.iLastTime1 = htonl(strSysEpSOTNoEntry.iLastTime1);
        pStrSend->strEpSOTNoEntryInfo.iBeginTime2 = htonl(strSysEpSOTNoEntry.iBeginTime2);
        pStrSend->strEpSOTNoEntryInfo.iLastTime2 = htonl(strSysEpSOTNoEntry.iLastTime2);
        pStrSend->strEpSOTNoEntryInfo.iBeginTime3 = htonl(strSysEpSOTNoEntry.iBeginTime3);
        pStrSend->strEpSOTNoEntryInfo.iLastTime3 = htonl(strSysEpSOTNoEntry.iLastTime3);
    }
    XCSPTL_INFO("sot no entry get info.\n");
    XCSPTL_INFO("En:%d,bt1:0x%x,et1:0x%x,bt2:0x%x,et2:0x%x,bt3:0x%x,et3:0x%x.\n",\
    strSysEpSOTNoEntry.cEnableFlag,\
    strSysEpSOTNoEntry.iBeginTime1,\
    strSysEpSOTNoEntry.iLastTime1,\
    strSysEpSOTNoEntry.iBeginTime2,\
    strSysEpSOTNoEntry.iLastTime2,\
    strSysEpSOTNoEntry.iBeginTime3,\
    strSysEpSOTNoEntry.iLastTime3);
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_EP_ASSIST_RD_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_EP_SOT_NO_ENTRY_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::epSOTNoEntrySet(XCS_CLIENT *pClintSocket)
{
    int ret=0;
    char cLaneId =0;
    STR_EP_SOT_NOENTRY_SET *pClientInfo = (STR_EP_SOT_NOENTRY_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    EP_STOP_ON_TRAFFIC_NO_ENTRY strSysEpSOTNoEntry;

    cLaneId = pClientInfo->cLaneId;
    if(cLaneId > 0)
    {
        cLaneId -= 1;
    }
    XCSPTL_INFO("set ep sot no entry,lane:%d\n", cLaneId);
    ret = SysConfGet(e_EP_SOT_NOENTRY_CONF, &strSysEpSOTNoEntry, cLaneId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("sys set, get ep sot no entry Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    memset(&strSysEpSOTNoEntry, 0, sizeof(strSysEpSOTNoEntry));

    strSysEpSOTNoEntry.cEnableFlag = pClientInfo->strEpSOTNoEntryInfo.cEnableFlag;
    if(strSysEpSOTNoEntry.cEnableFlag > 0)
    {
        strSysEpSOTNoEntry.iBeginTime1 = ntohl(pClientInfo->strEpSOTNoEntryInfo.iBeginTime1);
        strSysEpSOTNoEntry.iLastTime1 = ntohl(pClientInfo->strEpSOTNoEntryInfo.iLastTime1);
        strSysEpSOTNoEntry.iBeginTime2 = ntohl(pClientInfo->strEpSOTNoEntryInfo.iBeginTime2);
        strSysEpSOTNoEntry.iLastTime2 = ntohl(pClientInfo->strEpSOTNoEntryInfo.iLastTime2);
        strSysEpSOTNoEntry.iBeginTime3 = ntohl(pClientInfo->strEpSOTNoEntryInfo.iBeginTime3);
        strSysEpSOTNoEntry.iLastTime3 = ntohl(pClientInfo->strEpSOTNoEntryInfo.iLastTime3);
    }
    XCSPTL_INFO("sot no entry set info.\n");
    XCSPTL_INFO("En:%d,bt1:0x%x,et1:0x%x,bt2:0x%x,et2:0x%x,bt3:0x%x,et3:0x%x.\n",\
    strSysEpSOTNoEntry.cEnableFlag,\
    strSysEpSOTNoEntry.iBeginTime1,\
    strSysEpSOTNoEntry.iLastTime1,\
    strSysEpSOTNoEntry.iBeginTime2,\
    strSysEpSOTNoEntry.iLastTime2,\
    strSysEpSOTNoEntry.iBeginTime3,\
    strSysEpSOTNoEntry.iLastTime3);

    ret = SysConfSet(e_EP_SOT_NOENTRY_CONF, &strSysEpSOTNoEntry, cLaneId);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set ep sot no entry Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.ucCarRoadId = cLaneId;
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_SOT_NO_ENTRY_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send EP SOT NO ENTRY Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send EP SOT NO ENTRY info msg ok.\n");
    }
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}

int CHandleXcsPtl::epJpgOsdConfQuery(XCS_CLIENT * pClintSocket)
{
    EP_JPG_OSD_CONF strSysEpJpgOsdConfGet;
    
    int nRet = SysConfGet(e_EP_JPG_OSD_CONF, &strSysEpJpgOsdConfGet, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("electPolice get jpg osd Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    PSTR_EP_JPG_OSD_CONF_QUERY pPcQueryInfo = (PSTR_EP_JPG_OSD_CONF_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

	memcpy((void *)pPcQueryInfo, (void *)&strSysEpJpgOsdConfGet, sizeof(strSysEpJpgOsdConfGet));
    
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_EP_JPG_OSD_CONF_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_ELECT_POLICE, CPE_EP_JPG_OSD_CONF_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::epJpgOsdConfSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    EP_JPG_OSD_CONF strSysEpJpgOsdConfSet;
    STR_EP_JPG_OSD_CONF_SET *pClientInfo = (STR_EP_JPG_OSD_CONF_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

	memcpy((void *)&strSysEpJpgOsdConfSet, (void *)pClientInfo, sizeof(strSysEpJpgOsdConfSet));
	
    ret = SysConfSet(e_EP_JPG_OSD_CONF, &strSysEpJpgOsdConfSet, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("electPolice Set electPolice jpg osd Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_JPG_OSD_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send elect police jpg osd Conf failed.\n");
    }
    else
    {
        XCSPTL_INFO("send elect police jpg osd Conf msg ok.\n");
    }


    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return 0;
}
