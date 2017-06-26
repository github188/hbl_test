/******************************************************************************
 * protoXDevCon330Handle.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.02.23, xcs team Create
 * --------------------
 ******************************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include "SysConf.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"

#include "SysHandle.h"

extern "C"
{
    #include "dsp.h"
}

int CHandleXcsPtl::devCon330InfoQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_330_CON_QUERY pstr330Info = (PSTR_330_CON_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    PLATFORM_330_CONF_INFO str330PlantInfo;
    int ret = SysConfGet(e_PLATFORM_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get 330 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    pstr330Info->cVer =1;
    pstr330Info->cEnable = str330PlantInfo.cEnable;
    pstr330Info->cLaneSeq1 = str330PlantInfo.sz330DriveWayId[0];
    pstr330Info->cLaneSeq2 = str330PlantInfo.sz330DriveWayId[1];
    pstr330Info->cLaneSeq3 = str330PlantInfo.sz330DriveWayId[2];
    pstr330Info->cLaneSeq4 = str330PlantInfo.sz330DriveWayId[3];

    str330PlantInfo.sz330DevId[127] =0;
    str330PlantInfo.sz330DirId[15] =0;
    str330PlantInfo.sz330Key[255] =0;
    strcpy(pstr330Info->sz330DevId, str330PlantInfo.sz330DevId);
    strcpy(pstr330Info->sz330DirId, str330PlantInfo.sz330DirId);
    strcpy(pstr330Info->sz330Key, str330PlantInfo.sz330Key);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_330_CON_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_DEV_CONNECT_PRO, CPE_330_PLATFORM_CON_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::devCon330InfoSet(XCS_CLIENT *pClintSocket)
{
    int ret = -1;
    STR_330_CON_SET *pstr330Info = (STR_330_CON_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    PLATFORM_330_CONF_INFO str330PlantInfo;
    memset(&str330PlantInfo, 0, sizeof(str330PlantInfo));
    ret = SysConfGet(e_PLATFORM_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get 330 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    str330PlantInfo.cEnable = pstr330Info->cEnable;
    str330PlantInfo.sz330DriveWayId[0]= pstr330Info->cLaneSeq1;
    str330PlantInfo.sz330DriveWayId[1]= pstr330Info->cLaneSeq2;
    str330PlantInfo.sz330DriveWayId[2]= pstr330Info->cLaneSeq3;
    str330PlantInfo.sz330DriveWayId[3]= pstr330Info->cLaneSeq4;

    pstr330Info->sz330DevId[127] =0;
    pstr330Info->sz330DirId[15] =0;
    pstr330Info->sz330Key[255] =0;
    strcpy(str330PlantInfo.sz330DevId, pstr330Info->sz330DevId);
    strcpy(str330PlantInfo.sz330DirId, pstr330Info->sz330DirId);
    strcpy(str330PlantInfo.sz330Key, pstr330Info->sz330Key);

    ret = SysConfSet(e_PLATFORM_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set 330 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_330_DEV_CONNECT_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send 330 dev connect Conf msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("send 330 dev connect Conf msg ok.\n");
    }
    
    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::devConV3InfoQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_V3_CON_QUERY pstrV3Info = (PSTR_V3_CON_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    PLATFORM_V3_CONF_INFO strV3PlantInfo;
    int ret = SysConfGet(e_V3_CONF, &strV3PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get v3 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    pstrV3Info->cVersion =1;
    pstrV3Info->cV3Enable = strV3PlantInfo.cV3Enable;
    pstrV3Info->cV3ServerSeq = strV3PlantInfo.cV3ServerSeq;
    pstrV3Info->cV3TimeSynFlag = strV3PlantInfo.cV3TimeSynFlag;
    pstrV3Info->uiV3SerIp = htonl(strV3PlantInfo.uiV3SerIp);
    pstrV3Info->usV3Port = htons(strV3PlantInfo.usV3Port);

    strV3PlantInfo.szDevNo[19] =0;
    strV3PlantInfo.szDevPwd[15] =0;
    strcpy(pstrV3Info->szDevNo, strV3PlantInfo.szDevNo);
    strcpy(pstrV3Info->szDevPwd, strV3PlantInfo.szDevPwd);


    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_V3_CON_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_DEV_CONNECT_PRO, CPE_V3_PLATFORM_CON_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::devConV3InfoSet(XCS_CLIENT *pClintSocket)
{
    int ret =-1;
    STR_V3_CON_SET *pstrV3Info = (STR_V3_CON_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    PLATFORM_V3_CONF_INFO strV3PlantInfo;
    memset(&strV3PlantInfo, 0, sizeof(strV3PlantInfo));
    ret = SysConfGet(e_V3_CONF, &strV3PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get v3 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    strV3PlantInfo.cV3Enable = pstrV3Info->cV3Enable;
    strV3PlantInfo.cV3ServerSeq = pstrV3Info->cV3ServerSeq;
    strV3PlantInfo.cV3TimeSynFlag = pstrV3Info->cV3TimeSynFlag;
    strV3PlantInfo.uiV3SerIp = ntohl(pstrV3Info->uiV3SerIp);
    strV3PlantInfo.usV3Port = ntohs(pstrV3Info->usV3Port);

    pstrV3Info->szDevNo[19] =0;
    pstrV3Info->szDevPwd[15] =0;
    strcpy(strV3PlantInfo.szDevNo, pstrV3Info->szDevNo);
    strcpy(strV3PlantInfo.szDevPwd, pstrV3Info->szDevPwd);

    ret = SysConfSet(e_V3_CONF, &strV3PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set v3 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    /* 若设置为使能则重启platform_v3 程序，以重新登录或登出平台 */
    system("killall -q platform_v3");

    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}


int CHandleXcsPtl::devConEp330InfoQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_330_CON_QUERY pstr330Info = (PSTR_330_CON_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    PLATFORM_330_CONF_INFO str330PlantInfo;
    int ret = SysConfGet(e_EP_330_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get EP 330 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    pstr330Info->cVer =1;
    pstr330Info->cEnable = str330PlantInfo.cEnable;
    pstr330Info->cLaneSeq1 = str330PlantInfo.sz330DriveWayId[0];
    pstr330Info->cLaneSeq2 = str330PlantInfo.sz330DriveWayId[1];
    pstr330Info->cLaneSeq3 = str330PlantInfo.sz330DriveWayId[2];
    pstr330Info->cLaneSeq4 = str330PlantInfo.sz330DriveWayId[3];

    str330PlantInfo.sz330DevId[127] =0;
    str330PlantInfo.sz330DirId[15] =0;
    str330PlantInfo.sz330Key[255] =0;
    strcpy(pstr330Info->sz330DevId, str330PlantInfo.sz330DevId);
    strcpy(pstr330Info->sz330DirId, str330PlantInfo.sz330DirId);
    strcpy(pstr330Info->sz330Key, str330PlantInfo.sz330Key);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_330_CON_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_DEV_CONNECT_PRO, CPE_EP_330_PLATFORM_CON_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::devConEp330InfoSet(XCS_CLIENT *pClintSocket)
{
    int ret = -1;
    STR_330_CON_SET *pstr330Info = (STR_330_CON_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    PLATFORM_330_CONF_INFO str330PlantInfo;
    memset(&str330PlantInfo, 0, sizeof(str330PlantInfo));
    ret = SysConfGet(e_EP_330_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get EP 330 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    str330PlantInfo.cEnable = pstr330Info->cEnable;
    str330PlantInfo.sz330DriveWayId[0]= pstr330Info->cLaneSeq1;
    str330PlantInfo.sz330DriveWayId[1]= pstr330Info->cLaneSeq2;
    str330PlantInfo.sz330DriveWayId[2]= pstr330Info->cLaneSeq3;
    str330PlantInfo.sz330DriveWayId[3]= pstr330Info->cLaneSeq4;

    pstr330Info->sz330DevId[127] =0;
    pstr330Info->sz330DirId[15] =0;
    pstr330Info->sz330Key[255] =0;
    strcpy(str330PlantInfo.sz330DevId, pstr330Info->sz330DevId);
    strcpy(str330PlantInfo.sz330DirId, pstr330Info->sz330DirId);
    strcpy(str330PlantInfo.sz330Key, pstr330Info->sz330Key);

    ret = SysConfSet(e_EP_330_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set EP 330 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_EP_330_DEV_CONNECT_INFO_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send EP 330 dev connect Conf msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("send EP 330 dev connect Conf msg ok.\n");
    }
    
    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::devConV2InfoQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_V3_CON_QUERY pstrV2Info = (PSTR_V3_CON_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    PLATFORM_V3_CONF_INFO strV2PlantInfo;
    int ret = SysConfGet(e_V2_CONF, &strV2PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get v2 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    pstrV2Info->cVersion =1;
    pstrV2Info->cV3Enable = strV2PlantInfo.cV3Enable;
    pstrV2Info->cV3ServerSeq = strV2PlantInfo.cV3ServerSeq;
    pstrV2Info->cV3TimeSynFlag = strV2PlantInfo.cV3TimeSynFlag;
    pstrV2Info->uiV3SerIp = htonl(strV2PlantInfo.uiV3SerIp);
    pstrV2Info->usV3Port = htons(strV2PlantInfo.usV3Port);

    strV2PlantInfo.szDevNo[19] =0;
    strV2PlantInfo.szDevPwd[15] =0;
    strcpy(pstrV2Info->szDevNo, strV2PlantInfo.szDevNo);
    strcpy(pstrV2Info->szDevPwd, strV2PlantInfo.szDevPwd);


    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_V3_CON_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_DEV_CONNECT_PRO, CPE_V2_PLATFORM_CON_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::devConV2InfoSet(XCS_CLIENT *pClintSocket)
{
    int ret =-1;
    STR_V3_CON_SET *pstrV2Info = (STR_V3_CON_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    PLATFORM_V3_CONF_INFO strV2PlantInfo;
    memset(&strV2PlantInfo, 0, sizeof(strV2PlantInfo));
    ret = SysConfGet(e_V2_CONF, &strV2PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get v2 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

    strV2PlantInfo.cV3Enable = pstrV2Info->cV3Enable;
    strV2PlantInfo.cV3ServerSeq = pstrV2Info->cV3ServerSeq;
    strV2PlantInfo.cV3TimeSynFlag = pstrV2Info->cV3TimeSynFlag;
    strV2PlantInfo.uiV3SerIp = ntohl(pstrV2Info->uiV3SerIp);
    strV2PlantInfo.usV3Port = ntohs(pstrV2Info->usV3Port);

    pstrV2Info->szDevNo[19] =0;
    pstrV2Info->szDevPwd[15] =0;
    strcpy(strV2PlantInfo.szDevNo, pstrV2Info->szDevNo);
    strcpy(strV2PlantInfo.szDevPwd, pstrV2Info->szDevPwd);

    ret = SysConfSet(e_V2_CONF, &strV2PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set v2 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    /* 若设置为使能则重启platform 程序，以重新登录或登出平台 */
    system("killall -q platform");

    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::devConGb28181InfoQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_GB28181_CON_QUERY pstrGb28181Info = (PSTR_GB28181_CON_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    GB28181_CONF_INFO strGb28181PlantInfo;
    int ret = SysConfGet(e_GB28181_CONF, &strGb28181PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get Gb28181 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

	strcpy(pstrGb28181Info->szServerId, strGb28181PlantInfo.szServerId);
	strcpy(pstrGb28181Info->szServerDomain, strGb28181PlantInfo.szServerDomain);
	strcpy(pstrGb28181Info->szServerIp, strGb28181PlantInfo.szServerIp);
	
	pstrGb28181Info->uiServerPort = htonl(strGb28181PlantInfo.uiServerPort);
	pstrGb28181Info->uiRegisterExpiry = htonl(strGb28181PlantInfo.uiRegisterExpiry);
	pstrGb28181Info->uiKeepLiveTime = htonl(strGb28181PlantInfo.uiKeepLiveTime);
	pstrGb28181Info->uiConMode = htonl(strGb28181PlantInfo.uiConMode);
	
	strcpy(pstrGb28181Info->szClientId, strGb28181PlantInfo.szClientId);
	strcpy(pstrGb28181Info->szClientUserName, strGb28181PlantInfo.szClientUserName);
	strcpy(pstrGb28181Info->szClientPasswd, strGb28181PlantInfo.szClientPasswd);

	pstrGb28181Info->cEnFlag = strGb28181PlantInfo.cEnFlag;

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_GB28181_CON_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_DEV_CONNECT_PRO, CPE_GB28181_PLATFORM_CON_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::devConGb28181InfoSet(XCS_CLIENT *pClintSocket)
{
    int ret =-1;
    STR_GB28181_CON_SET *pstrGb28181Info = (STR_GB28181_CON_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    GB28181_CONF_INFO strGb28181PlantInfo;
    memset(&strGb28181PlantInfo, 0, sizeof(strGb28181PlantInfo));
    ret = SysConfGet(e_GB28181_CONF, &strGb28181PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("get gb28181 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }

	strcpy(strGb28181PlantInfo.szServerId,pstrGb28181Info->szServerId);
	strcpy(strGb28181PlantInfo.szServerDomain, pstrGb28181Info->szServerDomain);
	strcpy(strGb28181PlantInfo.szServerIp, pstrGb28181Info->szServerIp);
	
	strGb28181PlantInfo.uiServerPort = ntohl(pstrGb28181Info->uiServerPort);
	strGb28181PlantInfo.uiRegisterExpiry = ntohl(pstrGb28181Info->uiRegisterExpiry);
	strGb28181PlantInfo.uiKeepLiveTime = ntohl(pstrGb28181Info->uiKeepLiveTime);
	strGb28181PlantInfo.uiConMode = ntohl(pstrGb28181Info->uiConMode);
	
	strcpy(strGb28181PlantInfo.szClientId, pstrGb28181Info->szClientId);
	strcpy(strGb28181PlantInfo.szClientUserName, pstrGb28181Info->szClientUserName);
	strcpy(strGb28181PlantInfo.szClientPasswd, pstrGb28181Info->szClientPasswd);

	strGb28181PlantInfo.cEnFlag = pstrGb28181Info->cEnFlag;

    ret = SysConfSet(e_GB28181_CONF, &strGb28181PlantInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("set Gb28181 platform Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    /* 若设置为使能则重启sipua 程序，以重新登录或登出平台 */
    system("killall -q sipua");

    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

