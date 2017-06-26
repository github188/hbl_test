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
#include <syslog.h>

#include "SysHandle.h"

#include "Record.h"
#include "rtc_util.h"

extern "C"
{
    #include "dsp.h"
}

#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"

int CHandleXcsPtl::netInfoQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_NET_INFO_QUERY pstrNetInfo = (PSTR_NET_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    NET_CONF_INFO strNetInfo;
    int ret = SysConfGet(e_NET_CONF, &strNetInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo get net Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    XCSPTL_INFO("org netget ip:0x%x netmask:0x%x,gateway:0x%x.\n", \
    strNetInfo.uiIPAddr,\
    strNetInfo.uiNetMask,\
    strNetInfo.uiGateWay);

    
    pstrNetInfo->uiIPAddr = htonl(strNetInfo.uiIPAddr);
    pstrNetInfo->uiNetMask = htonl(strNetInfo.uiNetMask);
    pstrNetInfo->uiGateWay = htonl(strNetInfo.uiGateWay);

    XCSPTL_INFO("change netget ip:0x%x netmask:0x%x,gateway:0x%x.\n", \
    pstrNetInfo->uiIPAddr,\
    pstrNetInfo->uiNetMask,\
    pstrNetInfo->uiGateWay);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_NET_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_NET_INFO, CPE_NET_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::netInfoSet(XCS_CLIENT *pClintSocket)
{

    STR_NET_INFO_SET *pstrNetInfo = (STR_NET_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    NET_CONF_INFO strNetInfo;

    strNetInfo.uiIPAddr = ntohl(pstrNetInfo->uiIPAddr);
    strNetInfo.uiNetMask = ntohl(pstrNetInfo->uiNetMask);
    strNetInfo.uiGateWay = ntohl(pstrNetInfo->uiGateWay);
    XCSPTL_INFO("netset ip:0x%x netmask:0x%x,gateway:0x%x.\n", \
    strNetInfo.uiIPAddr,\
    strNetInfo.uiNetMask,\
    strNetInfo.uiGateWay);

    int ret = SysConfSet(e_NET_CONF, &strNetInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo set net Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::netInfoOfPicStoreQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_PIC_NET_SAVE_INFO_QUERY pstrPicInfoGet = (PSTR_PIC_NET_SAVE_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    NET_HDD_CONF_INFO strNetPicStoreInfo;
    int ret = SysConfGet(e_PIC_NET_HDD_CONF, &strNetPicStoreInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo get pic store net Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    pstrPicInfoGet->uiEnable = htonl(strNetPicStoreInfo.uiEnable);
    pstrPicInfoGet->uiIpAddr = htonl(strNetPicStoreInfo.uiIpAddr);
    pstrPicInfoGet->uiStorageType = htonl(strNetPicStoreInfo.uiStorageType);

    strNetPicStoreInfo.szFolderName[31] =0;
    strNetPicStoreInfo.szUserName[31] =0;
    strNetPicStoreInfo.szPassWord[31] =0;
    strcpy(pstrPicInfoGet->szFolderName, strNetPicStoreInfo.szFolderName);
    strcpy(pstrPicInfoGet->szUserName, strNetPicStoreInfo.szUserName);
    strcpy(pstrPicInfoGet->szPassWord, strNetPicStoreInfo.szPassWord);

    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_PIC_NET_SAVE_INFO_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_NET_INFO, CPE_NET_PIC_NET_SAVE_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

/******************************************************************************
 * CHandleXcsPtl.netInfoOfPicStoreSet() :
 * 
 * DESCRIPTION:  由于cifs的挂载目录需要放在monitor启动过程中实现，所以在这里只是
 * 简单地保存设置，需要重新启动板子
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.12, wangbin Create
 * --------------------
 ******************************************************************************/
int CHandleXcsPtl::netInfoOfPicStoreSet(XCS_CLIENT *pClintSocket)
{
    int ret = 0;
    STR_PIC_NET_SAVE_INFO_SET *pstrPicInfoSet = (STR_PIC_NET_SAVE_INFO_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    NET_HDD_CONF_INFO strNetPicStoreInfo;
    ret = SysConfGet(e_PIC_NET_HDD_CONF, &strNetPicStoreInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo Get pic net staort Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    
    strNetPicStoreInfo.uiEnable = ntohl(pstrPicInfoSet->uiEnable);
    strNetPicStoreInfo.uiIpAddr = ntohl(pstrPicInfoSet->uiIpAddr);
    strNetPicStoreInfo.uiStorageType = ntohl(pstrPicInfoSet->uiStorageType);

    pstrPicInfoSet->szFolderName[31] =0;
    pstrPicInfoSet->szUserName[31] =0;
    pstrPicInfoSet->szPassWord[31] =0;
    strcpy(strNetPicStoreInfo.szFolderName, pstrPicInfoSet->szFolderName);
    strcpy(strNetPicStoreInfo.szUserName, pstrPicInfoSet->szUserName);
    strcpy(strNetPicStoreInfo.szPassWord, pstrPicInfoSet->szPassWord);

    ret = SysConfSet(e_PIC_NET_HDD_CONF, &strNetPicStoreInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo set pic net staort Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    else
    {
        shell_print("pic store changed,reboot xcs\n");
        syslog(LOG_ERR|LOG_USER, "pic store changed,reboot xcs\n");
        STR_SYS_APP_MSG strTmpSysMsg;
        memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
        strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
        strTmpSysMsg.uiMsgType = MSG_REMOTE_REBOOT;
        ret = sendMsgToMainApp(&strTmpSysMsg);
        if(-1 == ret)
        {
            XCSPTL_ERROR("send pic store reboot msg failed.\n");
        }
        else
        {
            XCSPTL_INFO("send pic store reboot msg ok.\n");
        }
    }

    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::netInfoOfRecordStoreQuery(XCS_CLIENT *pClintSocket)
{
    STR_RECORD_NET_SAVE_QUERY *pstrRecInfoGet = (STR_RECORD_NET_SAVE_QUERY *)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    NET_HDD_CONF_INFO strNetPicStoreInfo;
    int ret = SysConfGet(e_REC_NET_HDD_CONF, &strNetPicStoreInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo get rec store net Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    pstrRecInfoGet->uiEnable = htonl(strNetPicStoreInfo.uiEnable);
    pstrRecInfoGet->uiIpAddr = htonl(strNetPicStoreInfo.uiIpAddr);
    pstrRecInfoGet->uiStorageType = htonl(strNetPicStoreInfo.uiStorageType);

    strNetPicStoreInfo.szFolderName[31] =0;
    strNetPicStoreInfo.szUserName[31] =0;
    strNetPicStoreInfo.szPassWord[31] =0;
    strcpy(pstrRecInfoGet->szFolderName, strNetPicStoreInfo.szFolderName);
    strcpy(pstrRecInfoGet->szUserName, strNetPicStoreInfo.szUserName);
    strcpy(pstrRecInfoGet->szPassWord, strNetPicStoreInfo.szPassWord);

    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_RECORD_NET_SAVE_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_NET_INFO, CPE_NET_RECORD_NET_SAVE_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::netInfoOfRecordStoreSet(XCS_CLIENT *pClintSocket)
{
    STR_RECORD_NET_SAVE_SET *pstrRecInfoSet = (STR_RECORD_NET_SAVE_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    NET_HDD_CONF_INFO strNetPicStoreInfo;

    int ret= 0;
    
    ret = SysConfGet(e_REC_NET_HDD_CONF, &strNetPicStoreInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo get rec net staort Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    strNetPicStoreInfo.uiEnable = ntohl(pstrRecInfoSet->uiEnable);
    strNetPicStoreInfo.uiIpAddr = ntohl(pstrRecInfoSet->uiIpAddr);
    strNetPicStoreInfo.uiStorageType = ntohl(pstrRecInfoSet->uiStorageType);

    pstrRecInfoSet->szFolderName[31] =0;
    pstrRecInfoSet->szUserName[31] =0;
    pstrRecInfoSet->szPassWord[31] =0;
    strcpy(strNetPicStoreInfo.szFolderName, pstrRecInfoSet->szFolderName);
    strcpy(strNetPicStoreInfo.szUserName, pstrRecInfoSet->szUserName);
    strcpy(strNetPicStoreInfo.szPassWord, pstrRecInfoSet->szPassWord);

    ret = SysConfSet(e_REC_NET_HDD_CONF, &strNetPicStoreInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo set rec net staort Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    else
    {
        shell_print("rec store changed,reboot xcs\n");
        syslog(LOG_ERR|LOG_USER, "rec store changed,reboot xcs\n");
        STR_SYS_APP_MSG strTmpSysMsg;
        memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
        strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
        strTmpSysMsg.uiMsgType = MSG_REMOTE_REBOOT;
        ret = sendMsgToMainApp(&strTmpSysMsg);
        if(-1 == ret)
        {
            XCSPTL_ERROR("send rec store reboot msg failed.\n");
        }
        else
        {
            XCSPTL_INFO("send rec store reboot msg ok.\n");
        }
    }
    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}


int CHandleXcsPtl::netInfoOfNRMQuery(XCS_CLIENT *pClintSocket)
{
    STR_NET_REC_MANAGE_QUERY *pstrNRMInfoGet = (STR_NET_REC_MANAGE_QUERY *)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    NET_REC_MANAGE_INFO strNRMInfo;
    int ret = SysConfGet(e_NET_REC_MANAGE_CONF, &strNRMInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo get nrm Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    pstrNRMInfoGet->cEnable = strNRMInfo.cEnable;
    pstrNRMInfoGet->uiMainNRMIp = htonl(strNRMInfo.uiMainNRMIp);
    pstrNRMInfoGet->uiBakNRMIp = htonl(strNRMInfo.uiBakNRMIp);
    
    pstrNRMInfoGet->usMainNRMPort = htons(strNRMInfo.usMainNRMPort);
    pstrNRMInfoGet->usBakNRMPort = htons(strNRMInfo.usBakNRMPort);

    strncpy(pstrNRMInfoGet->szPuID, strNRMInfo.szPuID, sizeof(strNRMInfo.szPuID));

    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_NET_REC_MANAGE_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_NET_INFO, CPE_NET_RECORD_MANAGE_INFO_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

    return 0;
}

int CHandleXcsPtl::netInfoOfNRMSet(XCS_CLIENT *pClintSocket)
{
    STR_NET_REC_MANAGE_SET *pstrNRMInfoSet = (STR_NET_REC_MANAGE_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    NET_REC_MANAGE_INFO strNRMInfo;

    int ret= 0;
    
    ret = SysConfGet(e_NET_REC_MANAGE_CONF, &strNRMInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo get nrm Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    strNRMInfo.cEnable = pstrNRMInfoSet->cEnable;
    strNRMInfo.uiMainNRMIp = ntohl(pstrNRMInfoSet->uiMainNRMIp);
    strNRMInfo.uiBakNRMIp = ntohl(pstrNRMInfoSet->uiBakNRMIp);
    
    strNRMInfo.usMainNRMPort= ntohs(pstrNRMInfoSet->usMainNRMPort);
    strNRMInfo.usBakNRMPort = ntohs(pstrNRMInfoSet->usBakNRMPort);
    
    strncpy(strNRMInfo.szPuID, pstrNRMInfoSet->szPuID, sizeof(strNRMInfo.szPuID));

    ret = SysConfSet(e_NET_REC_MANAGE_CONF, &strNRMInfo, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("netinfo set nrm Info Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    else
    {
        STR_SYS_APP_MSG strTmpSysMsg;
        memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
        strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
        strTmpSysMsg.uiMsgType = MSG_NRM_INFO_SET;
        ret = sendMsgToMainApp(&strTmpSysMsg);
        if(-1 == ret)
        {
            XCSPTL_ERROR("send nrm change msg failed.\n");
        }
        else
        {
            XCSPTL_INFO("send nrm change msg ok.\n");
        }
    }
    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

