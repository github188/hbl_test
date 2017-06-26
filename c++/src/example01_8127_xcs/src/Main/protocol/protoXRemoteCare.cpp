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
#include <syslog.h>
#include <stdlib.h>

#include "vernum.h"
#include "Record.h"
#include "rtc_util.h"
#include "dsp.h"
#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"
#include "SysHandle.h"

#define NM_TMP_FILE "/tmp/nmPost.tmp"

extern unsigned int g_uiHardWVer;

int CHandleXcsPtl::rCareSysTimeQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_SYS_TIME_QUERY pstrTimeInfo = (PSTR_SYS_TIME_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    pstrTimeInfo->uiSysTime = htonl(time(NULL));//返回本地时间,而不是格林尼治时间

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_SYS_TIME_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_SYS_TIME_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::rCareSysTimeSet(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    STR_SYS_TIME_SET *pstrTimeInfo = (STR_SYS_TIME_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    unsigned int uiSysTime = ntohl(pstrTimeInfo->uiSysTime);
    unsigned int uiUtc = uiSysTime; //uiSysTime是本地时间,而不是格林尼治时间

    if(m_uiThreadFlag == DVS_PROTO_TYPE)
    {
        ret = ntpEnFlagGet();
        if(ret > 0)
        {
            XCSPTL_INFO("ntp is enable,egnore timer set!\n");
            SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
            return 0;
        }
    }

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_SYS_TIME_SET;
    memcpy(strTmpSysMsg.aucData,(void *)&uiUtc, sizeof(uiUtc));
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send sys set time msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("remote client send sys set time msg ok.\n");
    }
    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));

    return 0;
}

int CHandleXcsPtl::rCareSysVerQuery(XCS_CLIENT *pClintSocket)
{
    int iFLen =0;
    FILE *file = NULL;
    
    PSTR_SYS_RELEASE_QUERY pstrVerInfo = (PSTR_SYS_RELEASE_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);
    strcpy(pstrVerInfo->cSerialNum, GetProductSerialNo());
    pstrVerInfo->uiHardwareVer  = htonl(g_uiHardWVer);
    pstrVerInfo->uiProcType     = htonl(device_code);
    pstrVerInfo->uiProtocolVer  = htonl(1);
    pstrVerInfo->uiSoftwareVer  = htonl(version_int);


    system("rm /tmp/macTmp.txt");
    system("ifconfig eth0 | grep HWaddr | awk '{print $5}' >> /tmp/macTmp.txt");

    /* 读取暂存文件到缓存中 */
    file = fopen("/tmp/macTmp.txt","rb");
    if ( NULL == file )
    {
        return -1;
    }
    fseek(file, 0, SEEK_END);
    iFLen = ftell(file);
    fseek(file, 0, SEEK_SET);
    fread(pstrVerInfo->szcMac, 17, 1, file);
    fflush(file);
    fclose(file);
        

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_SYS_RELEASE_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_SYS_RELEASE_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;

}

int CHandleXcsPtl::rCareLogQuery(XCS_CLIENT *pClintSocket)
{
    
    //PSTR_SYS_LOG_PC_QUERY pstrLogPcQ = (PSTR_SYS_LOG_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);

    //PSTR_SYS_LOG_QUERY_ACK pstrLogAck = (PSTR_SYS_LOG_QUERY_ACK)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    #if 0
    int iCnt = 0;

    iCnt = mcLogQuery((ELogType)(ntohl(pstrLogPcQ->uiLogType), \
        ntohl(pstrLogPcQ->uiBeginTime), \
        ntohl(pstrLogPcQ->uiEndTime), \
        pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH+sizeof(TLogQueryIndexHead), \
        DVS_PACKET_LEN-(NT_PACK_HEAD_LENGTH+sizeof(TLogQueryIndexHead)));//UTC 时间
    #endif
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_SYS_LOG_QUERY_ACK);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_SYS_LOG_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::rCareLogDownLoad(XCS_CLIENT *pClintSocket)
{
    //PSTR_SYS_LOG_PC_DOWN pstrLogPcQ = (PSTR_SYS_LOG_PC_DOWN)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);

    //PSTR_SYS_LOG_DOWN_ACK pstrLogAck = (PSTR_SYS_LOG_DOWN_ACK)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);


    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_SYS_LOG_DOWN_ACK);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_SYS_LOG_DOWN, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;

}


int CHandleXcsPtl::rCareReboot(XCS_CLIENT *pClintSocket)
{
    int ret =0;    
    //PSTR_REMOTE_UPDATE pstrPacket = (PSTR_REMOTE_UPDATE )(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    shell_print("remote client reboot xcs\n");
    syslog(LOG_ERR|LOG_USER, "remote client reboot xcs\n");

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_REMOTE_REBOOT;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send reboot msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("remote client send reboot msg ok.\n");
    }


    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return ret;
}



int CHandleXcsPtl::rCareUpgrade(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    PSTR_REMOTE_UPDATE pstrPacket = (PSTR_REMOTE_UPDATE )(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_REMOTE_UPDATE;
    memcpy(strTmpSysMsg.aucData, pstrPacket->ucURL, sizeof(strTmpSysMsg.aucData));
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send upgrade msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("send upgrade msg ok.\n");
    }

    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return 0;
}

int CHandleXcsPtl::rCareUpdateProgress(XCS_CLIENT *pClintSocket)
{
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pClinetContent->sRoadName, pClinetContent->sDirName);

    //PSTR_UPDATE_PROGRESS pstrUpInfo = (PSTR_UPDATE_PROGRESS)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);


    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_SYS_LOG_DOWN_ACK);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_UPDATE_PROGRESS, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;

}

int CHandleXcsPtl::rCareNtpInfoQuery(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    PSTR_NTP_SERVER_QUERY pstrNtpInfo = (PSTR_NTP_SERVER_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
    NTP_CONF_INFO ntpConf;
    ret = SysConfGet(e_NTP_CONF, &ntpConf, 0);
    if (ret == 0 )
    {
        pstrNtpInfo->uiEnable   = htonl(ntpConf.uiEnable);
        pstrNtpInfo->uiServerIp = htonl(ntpConf.uiServerIP);
        pstrNtpInfo->usJudgeCycle = htons(ntpConf.usJudgeCycle);
        pstrNtpInfo->ucJudegPrecision = ntpConf.ucJudegPrecision;
        
        pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_NTP_SERVER_QUERY);
        SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_NTP_SERVER_QUERY, \
        pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
       
    }
    else
    {
        SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
    }
    return 0;
}

int CHandleXcsPtl::rCareNtpInfoSet(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    STR_NTP_SERVER_SET *pstrNtpInfo = (STR_NTP_SERVER_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    NTP_CONF_INFO ntpConf;

    ntpConf.uiEnable = ntohl(pstrNtpInfo->uiEnable);
    ntpConf.uiServerIP = ntohl(pstrNtpInfo->uiServerIp);
    ntpConf.usJudgeCycle = ntohs(pstrNtpInfo->usJudgeCycle);
    ntpConf.ucJudegPrecision = pstrNtpInfo->ucJudegPrecision;

    XCSPTL_INFO("set ntp info,ucJudegPrecision is %d. cycle is %d\n", pstrNtpInfo->ucJudegPrecision,\
    ntpConf.usJudgeCycle);
    
    ret = SysConfSet(e_NTP_CONF, &ntpConf, 0);
    if (0 != ret )
    {
        SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
    }
    else
    {
        /* 3.组织应答包 */
        SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    }
    
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_NTP_SERVER_SET;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send ntp ip msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("send ntp ip msg ok.\n");
    }
    return 0;
}



int CHandleXcsPtl::rCareNetManageQuery(XCS_CLIENT *pClintSocket)
{
    int ret =0;
    FILE *file = NULL;
    int file_size = 0;

    PSTR_NET_MANAGE_QUERY pstrNMInfo = (PSTR_NET_MANAGE_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

/* 读取XML文件到缓存中 */
    file = fopen(NM_TMP_FILE,"r+");
    if ( NULL == file )
    {
        XCSPTL_ERROR("open %s failed\n", NM_TMP_FILE);
	    syslog(LOG_ERR|LOG_USER, "proto line:%d open /tmp/nmPost.tmp failed!\n", __LINE__);
        return -1;
    }
    fseek(file,0L,SEEK_END);
	file_size=ftell(file);
	
    fseek(file,0L,SEEK_SET);

    pstrNMInfo->uiDataLen = ntohl(file_size);

    //XCSPTL_ERROR("open %s ok, fileSize is %d\n", NM_TMP_FILE, file_size);
    ret = fread(((char *)pstrNMInfo+4), 1, file_size, file);
    if(ret != file_size)
    {
        fclose(file);
        XCSPTL_ERROR("failed proto read xml file len is %d\n", ret);
        SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
    }
    else
    {
        fclose(file);
        //XCSPTL_ERROR("ok proto read xml file len is %d\n", ret);
        pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + file_size +4;
        SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_NET_MANAGE_PRO, CPE_NET_MANAGE_QUERY, \
        pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    }
    return 0;
}

int CHandleXcsPtl::rCareRestoreDevConf(XCS_CLIENT *pClintSocket)
{
    int ret =0;    
    //PSTR_REMOTE_UPDATE pstrPacket = (PSTR_REMOTE_UPDATE )(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    
    //XCSPTL_INFO("query roadName:%s,dirName:%s .\n",pInfo->sRoadName, pInfo->sDirName);

    shell_print("remote client restore dev conf.\n");
    syslog(LOG_ERR|LOG_USER, "remote client restore dev conf.\n");

    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_REMOTE_RESTORE_PARAM;
    ret = sendMsgToMainApp(&strTmpSysMsg);
    if(-1 == ret)
    {
        XCSPTL_ERROR("send restore dev param msg failed.\n");
    }
    else
    {
        XCSPTL_INFO("send restore dev param msg ok.\n");
    }


    /* 3.组织应答包 */
	SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
    return ret;
}


int CHandleXcsPtl::rCareRestartTimeQuery(XCS_CLIENT *pClintSocket)
{
	int ret = 0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    PSTR_RESTART_TIME_QUERY pstrTimeInfo = (PSTR_RESTART_TIME_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);

    pstrTimeInfo->uiRestartTime = htonl(strSysFuncCfg.uiRestartTime);
    pstrTimeInfo->cEnFlag = strSysFuncCfg.cRestartTimeEn;
    XCSPTL_INFO("query EnFlag:%d,time:%d.\n", pstrTimeInfo->cEnFlag, pstrTimeInfo->uiRestartTime);

    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_RESTART_TIME_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_QUERY, CPE_RESTART_TIME_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::rCareRestartTimeSet(XCS_CLIENT *pClintSocket)
{
	int ret = 0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    STR_RESTART_TIME_SET *pstrTimeInfo = (STR_RESTART_TIME_SET *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

    XCSPTL_INFO("Set EnFlag:%d,time:%d.\n", pstrTimeInfo->cEnFlag, pstrTimeInfo->uiRestartTime);
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
    
    strSysFuncCfg.cRestartTimeEn = pstrTimeInfo->cEnFlag;
    strSysFuncCfg.uiRestartTime = ntohl(pstrTimeInfo->uiRestartTime);

    ret = SysConfSet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);

    /* 3.组织应答包 */
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));

    return 0;
}

