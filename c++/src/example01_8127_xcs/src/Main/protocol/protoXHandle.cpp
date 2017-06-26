#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "SysConf.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"
#include <time.h>
#include <sys/time.h>
#include <map>
#include "Record.h"
#include "rtc_util.h"

#include "dsp.h"
#include "vernum.h"

#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"
#include "MmsSend.h"
#include "SysHandle.h"
#include "TcpServer.h"

#define TSTR_HEAD   "INFO:XCS Protocl :[0x%x] "
#define TSTR_ENDL   "(%d)\n"
int gXcsProtocolDebug = 0x1;

#define USE_MAP  1

#if 0
static unsigned int			g_nSectionId = 0;
std::map<unsigned int, MMS_INFO>    g_MmsInfo;

#else
static MMS_INFO  g_MmsInfo[MAX_SECTION_NUM] __attribute__((aligned(32)));
#endif



extern const char * REC_DISK_MAP[];

int kjXcsDebugSwitch(int nLevel)
{
	gXcsProtocolDebug = nLevel;

	return 0;
}

int KJ_XCS_CMD_TRACE(int nCmdType,int nSubCmdType, int nRecvLen)
{
    /*  */
    if(CPH_HEART_BEAT == nCmdType)
        return 0;
    XCSPTL_INFO("xcsh protocol:funType:0x%x,subFunType:0x%x\n", nCmdType, nSubCmdType);
	return 0;
}

int CHandleXcsPtl::DataPump(XCS_CLIENT * pCmdPack)
{
	if(NULL==pCmdPack)
	{
	    XCSPTL_INFO("in param null,error\n");
		return (-1);
    }
	int nRet = 0;

	if( pCmdPack->nRecvBufLen >= NT_PACK_HEAD_LENGTH)
	{
		PSTR_PACKET_HEADER pPacketHead =  (PSTR_PACKET_HEADER)(pCmdPack->pRecvBuf);
		
		if( (pPacketHead->SyncWord == (unsigned long)htons(XCS_SYNC_WORD))
		 || (pPacketHead->SyncWord == (unsigned long)XCS_SYNC_WORD ))
		{
			pCmdPack->nPackLen = NT_PACK_HEAD_LENGTH + ntohl(pPacketHead->uiPayloadLength);
			if(pCmdPack->nRecvLen < pCmdPack->nPackLen)
			{
				nRet = 0;
			}
			else if(pCmdPack->nRecvLen == pCmdPack->nPackLen) /* 判断包是否已经收齐 */
			{
				nRet = DispatchCmd(pCmdPack);

				pCmdPack->nRecvLen = 0;
				pCmdPack->nPackLen = 0;
			}
			else
			{
				XCSPTL_INFO("%s:%d error kejun xcs protocol package recvlen=%d packlen=%d.\n",\
                    __FUNCTION__,__LINE__,pCmdPack->nRecvLen,pCmdPack->nPackLen);
                
				pCmdPack->nRecvLen = 0;
				pCmdPack->nPackLen = 0;
			}
		}
		else
		{
			pCmdPack->nRecvLen = 0;
			pCmdPack->nPackLen = 0;
			XCSPTL_INFO("%s:%d error kejun xcs protocol package.\n",__FUNCTION__,__LINE__);
			nRet = -1;
		}
	}

	return nRet;
}

int CHandleXcsPtl::SetNakPack(PSTR_PACKET_HEADER pPackHead, char cErrorCode)
{
	if(NULL==pPackHead)
		return (-1);
	
	memset(pPackHead, 0, sizeof(STR_PACKET_HEADER));
	pPackHead->SyncWord = htons(XCS_SYNC_WORD);
	pPackHead->Version = XCS_PROTOCOL_VER;
	pPackHead->IsAckOrNot = 1;
	pPackHead->DataType = CPH_NAK;
	pPackHead->SubDataType = 0x00;
	pPackHead->uiPayloadLength = 0;
	pPackHead->ucErrorCode = cErrorCode;
	return 0;
}

int CHandleXcsPtl::SetAckPack(PSTR_PACKET_HEADER pPackHead)
{
	if(NULL==pPackHead)
		return (-1);
	
	memset(pPackHead, 0, sizeof(STR_PACKET_HEADER));
	pPackHead->SyncWord = htons(XCS_SYNC_WORD);
	pPackHead->Version = XCS_PROTOCOL_VER;
	pPackHead->IsAckOrNot = 1;
	pPackHead->DataType = CPH_ACK;
	pPackHead->SubDataType = 0x00;
	pPackHead->uiPayloadLength = 0;
	return 0;
}

int CHandleXcsPtl::SetSimpleAckHead(PSTR_PACKET_HEADER pPackHead,unsigned char cDataType,unsigned char cSubDataType,int nPayloadLen)
{
	if(0==pPackHead )
		return (-1);
	
	memset(pPackHead, 0, sizeof(STR_PACKET_HEADER));
	pPackHead->SyncWord = htons(XCS_SYNC_WORD);
	pPackHead->Version = XCS_PROTOCOL_VER;
	pPackHead->IsAckOrNot = 1;
	pPackHead->DataType = cDataType;
	pPackHead->SubDataType = cSubDataType;
	pPackHead->uiPayloadLength = htonl(nPayloadLen);
	
	return 0;
}

int CHandleXcsPtl::CmdUserLogin(XCS_CLIENT *pCmdPack)
{
    int ret =0;
	int nEvent = 0;
    unsigned int uiDevConfVer =0;
	struct in_addr in;
	
	if(NULL==pCmdPack)
		return (-1);
	
	PSTR_USER_LOGIN pUserLogin = (PSTR_USER_LOGIN)(pCmdPack->pRecvBuf + NT_PACK_HEAD_LENGTH);
    PSTR_USER_LOGIN_REP pUserLoginRsp = (PSTR_USER_LOGIN_REP)(pCmdPack->pSendBuf + NT_PACK_HEAD_LENGTH);

	in.s_addr = pCmdPack->sin_addr;

    uiDevConfVer = ntohl(pUserLogin->uiDecConfVer);

	unsigned char uc4Ver = uiDevConfVer >> 24;
	unsigned char uc3Ver = (uiDevConfVer >> 16) & 0xff;
	unsigned char uc2Ver = (uiDevConfVer >> 8) & 0xff;
	unsigned char uc1Ver = uiDevConfVer & 0xff;

    shell_print("dec conf is 0x%x,dev fix is 0x%x.",uiDevConfVer, FIX_DEC_CONF_VER);
    if ( (strcmp(pUserLogin->cUserName, "hzkj")==0) && (strcmp(pUserLogin->cUserPasswd,"hzkj")==0) )
    {
        if( uiDevConfVer >= FIX_DEC_CONF_VER )
        {
    		char szLog[256] = {0};
    		sprintf(szLog,"Dev Conf Ver:(%d.%d.%d.%d) User( %s ) Login From Ip[%s] Succeed.\n",\
    		uc4Ver, uc3Ver, uc2Ver, uc1Ver, pUserLogin->cUserName, inet_ntoa(in));

    		mcLog(LMT_DVSPH,LT_INFO, szLog);
    		XCSPTL_ERROR("INFO:%s", szLog);

            pCmdPack->bUserValid = 1;
            pCmdPack->uiThreadTime =0;
            pCmdPack->uiHeartBit =1;
        	SetAckPack( (PSTR_PACKET_HEADER)(pCmdPack->pSendBuf) );
        	return 0;
        }
    }

	char szLog[256] = {0};
	sprintf(szLog,"Dev Conf Ver:(%d.%d.%d.%d) User( %s ) Login From Ip[%s] failed.\n",\
	uc4Ver, uc3Ver, uc2Ver, uc1Ver, pUserLogin->cUserName, inet_ntoa(in));

	mcLog(LMT_DVSPH,LT_INFO,szLog);
	XCSPTL_ERROR("INFO:%s",szLog);

    pCmdPack->bUserValid = 0;
    pUserLoginRsp->uiDecConfVer = htonl( FIX_DEC_CONF_VER );

    pCmdPack->uiHeartBit= 1;
    pCmdPack->uiThreadTime = 0;

    nEvent = EVENT_DISCONN;
    ret = msgQSend(pCmdPack->msgQEvent, (char *)&nEvent, sizeof(nEvent), 0);
    if( ret != sizeof(nEvent) )
        XCSPTL_ERROR("login error dicconn msgQSend() failed.\n");

    /* 3.组织应答包 */
    pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_USER_LOGIN_REP);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pCmdPack->pSendBuf), CPH_LOG, CPE_LOG_USER_LOGIN, \
    pCmdPack->nSendLen - NT_PACK_HEAD_LENGTH);
	
	return 0;
}

int CHandleXcsPtl::CmdUserLogout(XCS_CLIENT *pCmdPack)
{
    int ret =0;
	int nEvent = 0;

	if(NULL==pCmdPack)
		return (-1);
    struct in_addr in;
	in.s_addr = pCmdPack->sin_addr;
	
	char szLog[256] = {0};
	sprintf(szLog,"Xcs User Logout From Ip[%s] .\n", inet_ntoa(in));
	mcLog(LMT_DVSPH,LT_INFO,szLog);
	XCSPTL_INFO("INFO:%s",szLog);

    
	if(pCmdPack->bUserValid)
	{
        STR_EXCEPT_DISCON_CLIENT strClientInfo;
		STR_SYS_APP_MSG strTmpSysMsg;


        strClientInfo.uiIp              = pCmdPack->sin_addr;
        strClientInfo.usPort            = pCmdPack->sin_port;
        strClientInfo.uiSessionId       = pCmdPack->uiSessionId;
        
        strClientInfo.uiDataUploadIp    = uiDataUploadIp;
        strClientInfo.usDataUploadPort  = usDataUploadPort;
        
        memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
        strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
        strTmpSysMsg.uiMsgType = MSG_XCS_EXCEPT_DISCONNECT;
 
        memcpy(strTmpSysMsg.aucData, &strClientInfo, sizeof(strClientInfo));
       
        ret = sendMsgToMainApp(&strTmpSysMsg);
        if(-1 == ret)
        {
            XCSPTL_ERROR("send client disconnect msg failed.\n");
        }
        else
        {
            XCSPTL_INFO("send client disconnect msg ok.\n");
        }
	}
    
    pCmdPack->bUserValid = 0;
    pCmdPack->uiHeartBit= 1;
    pCmdPack->uiThreadTime = 0;

    nEvent = EVENT_DISCONN;
    ret = msgQSend(pCmdPack->msgQEvent, (char *)&nEvent, sizeof(nEvent), 0);
    if( ret != sizeof(nEvent) )
    {
        XCSPTL_ERROR("logout dicconn msgQSend() failed.\n");
    }
    else
    {
        XCSPTL_ERROR("normal logout EVENT_DISCONN msgQSend() ok.\n");
    }

	SetAckPack( (PSTR_PACKET_HEADER)(pCmdPack->pSendBuf) );
	return 0;
}

int CHandleXcsPtl::CmdHeartBeat(XCS_CLIENT *pCmdPack)
{
	if(NULL==pCmdPack)
		return (-1);
	
  	SetAckPack( (PSTR_PACKET_HEADER)(pCmdPack->pSendBuf) );
	
	return 0;
}



/******************************************************************************
 * CHandleXcsPtl.DispatchCmd() :
 * 
 * DESCRIPTION:  对xcs协议进行解析
 *  
 * Input  :xcs clinet 对象
 * Output : 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.03, kj Create
 * --------------------
 ******************************************************************************/
int CHandleXcsPtl::DispatchCmd(XCS_CLIENT * pCmdPack)
{
	if(NULL==pCmdPack)
	{
	    XCSPTL_INFO("in param is null,error\n");
		return -1;
	}
	PSTR_PACKET_HEADER pPackHead = (PSTR_PACKET_HEADER)(pCmdPack->pRecvBuf);

	if( ( NULL==pCmdPack->pRecvBuf ) || ( NULL==pCmdPack->pSendBuf ))
		return (-1);
	
	//if(CPH_STATUS != pPackHead->DataType)
	{
		KJ_XCS_CMD_TRACE(pPackHead->DataType, pPackHead->SubDataType, pCmdPack->nPackLen);
	}

    pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH;
	switch(pPackHead->DataType)
	{
	case CPH_LOG:
		switch(pPackHead->SubDataType)
		{
		case CPE_LOG_USER_LOGIN:
			CmdUserLogin(pCmdPack);
			break;
		case CPE_LOG_USER_LOGOUT:
			CmdUserLogout(pCmdPack);
			break;
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			break;
		}
		break;
	case CPH_VCHANN:
		switch(pPackHead->SubDataType)
		{
        case CPE_VCHANN_REQ_RTVIDEO:
			realMVideoQuery(pCmdPack);
		    break;
		case CPE_VCHANN_CUT_RTVIDEO:
            realMVideoCut(pCmdPack);
		    break;
		case CPE_VCHANN_SNAP_PARAM_QUERY:
            realMPicParamQuery(pCmdPack);
		    break;
		case CPE_VCHANN_SNAP_PARAM_SET:
            realMPicParamSet(pCmdPack);
		    break;
		case CPE_VCHANN_VIDEO_PARAM_QUERY:
			realMVideoParamQuery(pCmdPack);
		    break;
		case CPE_VCHANN_VIDEO_PARAM_CONFIG:
    		realMVideoParamSet(pCmdPack);
		    break;
		case CPE_VCHANN_DATA_REPORT:
		    pCmdPack->nSendLen = 0;
		    break;
        case CPE_VCHAN_DATA_REPORT_SET:
            realMDataReportParamSet(pCmdPack);
            break;
		default:
			pCmdPack->nSendLen = 0;
		    XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",\
		                DVS_DEVICE_TYPE_NAME,\
						(int)(pPackHead->DataType),\
						(int)(pPackHead->SubDataType) );
		    break;
		}
		break;
	case CPH_DEV_CTL:
		switch(pPackHead->SubDataType)
		{
        case CPE_DEV_CTL_CAP_QUERY:
            dCtlCapParamQuery(pCmdPack);
            break;
        case CPE_DEV_CTL_CAP_SET:
            dCtlCapParamSet(pCmdPack);
            break;
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			break;
		}
		break;
	case CPH_XCS_QUERY:
		switch(pPackHead->SubDataType)
		{
		case CPE_SYS_TIME_QUERY:
		    rCareSysTimeQuery(pCmdPack);
			break;
		case CPE_SYS_TIME_SET:
		    rCareSysTimeSet(pCmdPack);
			break;
		case CPE_SYS_RELEASE_QUERY:
		    rCareSysVerQuery(pCmdPack);
			break;
		case CPE_SYS_LOG_QUERY:
		    rCareLogQuery(pCmdPack);
			break;
	    case CPE_SYS_LOG_DOWN:
	        rCareLogDownLoad(pCmdPack);
	        break;
	    case CPE_REMOTE_UPDATE:
	        rCareUpgrade(pCmdPack);
	        break;
	    case CPE_UPDATE_PROGRESS:
	        rCareUpdateProgress(pCmdPack);
	        break;
	    case CPE_REMOTE_REBOOT:
	        rCareReboot(pCmdPack);
	        break;
	    case CPE_NTP_SERVER_QUERY:
	        rCareNtpInfoQuery(pCmdPack);
	        break;
	    case CPE_NTP_SERVER_SET:
	        rCareNtpInfoSet(pCmdPack);
	        break;
	    case CPE_REMOTE_RESTORE_DEV_PARAM:
	    	rCareRestoreDevConf(pCmdPack);
	    	break;
	    case CPE_RESTART_TIME_QUERY:
	    	rCareRestartTimeQuery(pCmdPack);
	    	break;
	    case CPE_RESTART_TIME_SET:
	    	rCareRestartTimeSet(pCmdPack);
	    	break;

		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			break;
		}
		break;
	case CPH_BAYONET:
		switch(pPackHead->SubDataType)
		{
		case CPE_BAYONET_GLOBAL_INFO_QUERY:
		    bayonetGInfoQuery(pCmdPack);
			break;
		case CPE_BAYONET_GLOBAL_INFO_SET:
		    bayonetGInfoSet(pCmdPack);
			break;
		case CPE_BAYONET_CAR_CHANNEL_INFO_QUERY:
            bayonetChannelInfoQuery(pCmdPack);
			break;
		case CPE_BAYONET_CAR_CHANNEL_INFO_SET:
		    bayonetChannelInfoSet(pCmdPack);
			break;
	    case CPE_BAYONET_VIRTUAL_LOOP_INFO_QUERY:
	        bayonetVLInfoQuery(pCmdPack);
	        break;
	    case CPE_BAYONET_VIRTUAL_LOOP_INFO_SET:
	        bayonetHighVLInfoSet(pCmdPack);
	        break;
	    case CPE_BAYONET_HAND_SNAP:
            bayonetHandSnap(pCmdPack);
	        break;
	    case CPE_BAYONET_FTP_INFO_QUERY:
            bayonetFtpInfoQuery(pCmdPack);
	        break;
	    case CPE_BAYONET_FTP_INFO_SET:
            bayonetFtpInfoSet(pCmdPack);
	        break;
	    case CPE_BAYONET_JPG_OSD_CONF_QUERY:
            bayonetJpgOsdConfQuery(pCmdPack);
	        break;
	    case CPE_BAYONET_JPG_OSD_CONF_SET:
            bayonetJpgOsdConfSet(pCmdPack);
	        break;
	    case CPE_BAYONET_IRREG_INFO_QUERY:
	    	bayonetIrregInfoQuery(pCmdPack);
	    	break;
	    case CPE_BAYONET_IRREG_INFO_SET:
	    	bayonetIrregInfoSet(pCmdPack);
	    	break;
	    case CPE_BAYONET_BACKUPS_FTP_INFO_QUERY:
            bayonetBackupsFtpInfoQuery(pCmdPack);
	        break;
	    case CPE_BAYONET_BACKUPS_FTP_INFO_SET:
            bayonetBackupsFtpInfoSet(pCmdPack);
	        break;
			    	
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			break;
		}
		break;
	case CPH_ELECT_POLICE:
		switch(pPackHead->SubDataType)
		{
		case CPE_ELECT_POLICE_GLOBAL_INFO_QUERY:
		    electPoliceGInfoQuery(pCmdPack);
			break;
		case CPE_ELECT_POLICE_GLOBAL_INFO_SET:
		    electPoliceGInfoSet(pCmdPack);
			break;
		case CPE_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY:
            electPoliceChannelInfoQuery(pCmdPack);
			break;
		case CPE_ELECT_POLICE_CAR_CHANNEL_INFO_SET:
            electPoliceChannelInfoSet(pCmdPack);
			break;
		case CPE_ELECT_POLICE_RED_LIGHT_STA_QUERY:
            electPoliceRedLightStaQuery(pCmdPack);
		    break;
		case CPE_ELECT_POLICE_SIMU_RUN_SNAP:
            electPoliceSimuHandSnap(pCmdPack);
			break;
	    #if 0
		case CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY:
		    electPoliceForceRedLQuery(pCmdPack);
			break;
	    case CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_SET:
	        electPoliceForceRedLSet(pCmdPack);
	        break;
	    #endif
	    case CPE_ELECT_POLICE_FTP_INFO_QUERY:
	        electPoliceFtpInfoQuery(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_FTP_INFO_SET:
	        electPoliceFtpInfoSet(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_TIME_RANGE_QUERY:
	        epTimeRQuery(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_TIME_RANGE_SET:
	        epTimeRSet(pCmdPack);
	        break;
	    case CPE_EP_ASSIST_RED_LIGHT_QUERY:
	        epAssistRedLightQuery(pCmdPack);
	        break;
	    case CPE_EP_ASSIST_RED_LIGHT_SET:
	        epAssistRedLightSet(pCmdPack);
			break;
	    case CPE_ELECT_POLICE_LPR_QUERY:
	        electPoliceLprInfoQuery(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_LPR_SET:
	        electPoliceLprInfoSet(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_IRREG_INFO_QUERY:
	        electPoliceIrregInfoQuery(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_IRREG_INFO_SET:
	        electPoliceIrregInfoSet(pCmdPack);
	        break;
	    case CPE_EP_SOT_NO_ENTRY_QUERY:
	        epSOTNoEntryQuery(pCmdPack);
	        break;
	    case CPE_EP_SOT_NO_ENTRY_SET:
	        epSOTNoEntrySet(pCmdPack);
			break;
		case CPE_EP_JPG_OSD_CONF_QUERY:
	        epJpgOsdConfQuery(pCmdPack);
	        break;
	    case CPE_EP_JPG_OSD_CONF_SET:
	        epJpgOsdConfSet(pCmdPack);
	        break;
	    case CPE_EP_BACKUPS_FTP_INFO_QUERY:
            epBackupsFtpInfoQuery(pCmdPack);
	        break;
	    case CPE_EP_BACKUPS_FTP_INFO_SET:
            epBackupsFtpInfoSet(pCmdPack);
	        break;	
	    case CPE_ELECT_POLICE_NEWADD_IRREG_INFO_QUERY:
	        electPoliceNewAddIrregInfoQuery(pCmdPack);
	        break;
	    case CPE_ELECT_POLICE_NEWADD_IRREG_INFO_SET:
	        electPoliceNewAddIrregInfoSet(pCmdPack);
	        break;	        
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			break;
		}
		break;
#ifdef MODULE_REC
	case CPH_XCS_RECORD:
		switch(pPackHead->SubDataType)
		{
		case CPE_RECORD_DISK_INFO_QUERY:
            pCmdPack->nSendLen = 0;
			break;
		case CPE_RECORD_DOWNLOAD_PLAYTYPE_QUERY:
		    XCSPTL_INFO("-----------------------record query is here!\n");
            RecordRtspQuery(pCmdPack);
			break;
		case CPE_RECORD_DOWNLOAD_PLAYTYPE_SET:
            RecordRtspSet(pCmdPack);
			break;
		case CPE_RECORD_PERIOD_PLAN_QUERY:
            pCmdPack->nSendLen = 0;
			break;
		case CPE_RECORD_PERIOD_PLAN_ADD:
            pCmdPack->nSendLen = 0;
			break; 
		case CPE_RECORD_PERIOD_PLAN_DEL:
            pCmdPack->nSendLen = 0;
			break;
		case CPE_RECORD_PERIOD_PLAN_UPDATE:
            pCmdPack->nSendLen = 0;
			break;
		case CPE_RECORD_CHANNEL_PARAM_QUERY:
            pCmdPack->nSendLen = 0;
			break;
		case CPE_RECORD_CHANNEL_PARAM_SET:
            pCmdPack->nSendLen = 0;
			break;
	    case CPE_XCS_RECORD_SEARCH:
			RecordFileQuery(pCmdPack);
	        break;
	    case CPE_XCS_RECORD_PLAYBACK:
	        RecordFilePlayQuery(pCmdPack);
	        break;
	    case CPE_RECORD_FILE_DOWNLOAD:
	        RecordFileDownLoadQuery(pCmdPack);
	        break;
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			break;
		}
		break;
#endif
	case CPH_CAMARA:
		switch(pPackHead->SubDataType)
		{
		case CPE_CAMARA_ATTRIB_QUERY:
		    hdCamaraAttribQuery(pCmdPack);
		    break;
		case CPE_CAMARA_PARAM_QUERY:
		    hdCamaraParamQuery(pCmdPack);
		    break;
		case CPE_CAMARA_STA_INFO_QUERY:
            hdCamaraStaInfoQuery(pCmdPack);
		    break;
		case CPE_CAMARA_CTL_INFO_QUERY:
            hdCamaraCtlInfoQuery(pCmdPack);
		    break;
		case CPE_CAMARA_CTL_INFO_SET:
            hdCamaraCtlInfoSet(pCmdPack);
		    break;
    	case CPE_CAMARA_TIME_RANGE_QUERY:
            hdCamaraTimeRQuery(pCmdPack);
		    break;
		case CPE_CAMARA_TIME_RANGE_SET:
            hdCamaraTimeRSet(pCmdPack);
		    break;
		case CPE_CAMARA_DEFAULT_CONF_QUERY:
		    hdCamaraDefaultParamQuery(pCmdPack);
		    break;
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			
			break;			
		}
		break;
	case CPH_NET_INFO:
		switch(pPackHead->SubDataType)
		{
		case CPE_NET_INFO_QUERY:
            netInfoQuery(pCmdPack);
			break;
		case CPE_NET_INFO_SET:
            netInfoSet(pCmdPack);
			break;
	    case CPE_NET_PIC_NET_SAVE_INFO_QUERY:
	        netInfoOfPicStoreQuery(pCmdPack);
	        break;
	    case CPE_NET_PIC_NET_SAVE_INFO_SET:
	        netInfoOfPicStoreSet(pCmdPack);
	        break;
        case CPE_NET_RECORD_NET_SAVE_INFO_QUERY:
            netInfoOfRecordStoreQuery(pCmdPack);
            break;
        case CPE_NET_RECORD_NET_SAVE_INFO_SET:
            netInfoOfRecordStoreSet(pCmdPack);
            break;
        case CPE_NET_RECORD_MANAGE_INFO_QUERY:
            netInfoOfNRMQuery(pCmdPack);
            break;
        case CPE_NET_RECORD_MANAGE_INFO_SET:
            netInfoOfNRMSet(pCmdPack);
            break;

        	        
		default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			
			break;
		}
		break;
	case CPH_DEV_CONNECT_PRO:
		switch(pPackHead->SubDataType)
		{
		case CPE_330_PLATFORM_CON_QUERY:
            devCon330InfoQuery(pCmdPack);
			break;
		case CPE_330_PLATFORM_CON_SET:
            devCon330InfoSet(pCmdPack);
			break;
		case CPE_V3_PLATFORM_CON_QUERY:
            devConV3InfoQuery(pCmdPack);
			break;
		case CPE_V3_PLATFORM_CON_SET:
            devConV3InfoSet(pCmdPack);
			break;
		case CPE_EP_330_PLATFORM_CON_QUERY:
	        devConEp330InfoQuery(pCmdPack);
	        break;	
	    case CPE_EP_330_PLATFORM_CON_SET:
	        devConEp330InfoSet(pCmdPack);
	        break;
	    case CPE_V2_PLATFORM_CON_QUERY:
            devConV2InfoQuery(pCmdPack);
			break;
		case CPE_V2_PLATFORM_CON_SET:
            devConV2InfoSet(pCmdPack);
			break;
		case CPE_GB28181_PLATFORM_CON_QUERY:	
			devConGb28181InfoQuery(pCmdPack);
			break;
		case CPE_GB28181_PLATFORM_CON_SET:	
			devConGb28181InfoSet(pCmdPack);
			break;
	    default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			
			break;
		}
		break;
		/* 客户端每隔15秒发送一次心跳 */
    case CPH_HEART_BEAT:
        pCmdPack->uiHeartBit++;
        pCmdPack->uiHBFlag =1;
        //pCmdPack->nSendLen = 0;
        XCSPTL_INFO("HBFlag:%d web client send heartBit is %d!\n", pCmdPack->uiHBFlag, pCmdPack->uiHeartBit);
        CmdHeartBeat(pCmdPack);
        return 0;
        break;
    case CPH_NET_MANAGE_PRO:
		switch(pPackHead->SubDataType)
		{
		case CPE_NET_MANAGE_QUERY:
            rCareNetManageQuery(pCmdPack);
			break;
	    default:
			pCmdPack->nSendLen = 0;
			XCSPTL_WARNING("WARNING:%s Nonsupport kejun xcs Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
			
			break;
		}
		break;
	default:
		pCmdPack->nSendLen = 0;
		XCSPTL_WARNING("WARNING:%s Nonsupport Its Protocol Command[0x%x:0x%x].\n",DVS_DEVICE_TYPE_NAME,\
						(int)pPackHead->DataType,\
						(int)pPackHead->SubDataType);
		break;
	}

	if(pCmdPack->nSendLen == 0)
	{
		XCSPTL_WARNING("WARNING: NAK kejun xcs Protocol Command[0x%x:0x%x].\n", (int)pPackHead->DataType, (int)pPackHead->SubDataType);
		SetNakPack((PSTR_PACKET_HEADER)pCmdPack->pSendBuf, ERROR_FUNC_CODE);
		pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH;
	}
	return 0;
}


int CHandleXcsPtl::InitClient(XCS_CLIENT * pCmdPack)
{
	pCmdPack->bUserValid = 0;
	return 0;
}

int CHandleXcsPtl::UninitClient(XCS_CLIENT *pCmdPack)
{
#if 0
	int ret = 0;

	if(pCmdPack->bUserValid)
	{
		char szLog[256] = {0};
        STR_EXCEPT_DISCON_CLIENT strClientInfo;
		STR_SYS_APP_MSG strTmpSysMsg;

		sprintf(szLog,"kejun xcs User() Disconnect Exceptional.\n");
		mcLog(LMT_DVSPH,LT_INFO,szLog);
		XCSPTL_INFO("INFO:%s",szLog);

        strClientInfo.uiIp = pCmdPack->sin_addr;
        strClientInfo.usPort = pCmdPack->sin_port;
        strClientInfo.uiSessionId = pCmdPack->uiSessionId;
        strClientInfo.uiDataUploadIp    = pHKjXcs->uiDataUploadIp;
        strClientInfo.usDataUploadPort  = pHKjXcs->usDataUploadPort;
        memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
        strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
        strTmpSysMsg.uiMsgType = MSG_XCS_EXCEPT_DISCONNECT;
        memcpy(strTmpSysMsg.aucData, &strClientInfo, sizeof(strClientInfo));
        ret = sendMsgToMainApp(&strTmpSysMsg);
        if(-1 == ret)
        {
            XCSPTL_ERROR("send client disconnect msg failed.\n");
        }
        else
        {
            XCSPTL_INFO("send client disconnect msg ok.\n");
        }
	}
	else
	{
	    shell_print("uninit clinet is here\n");
	}

	pCmdPack->bUserValid = 0;
#endif

	return 0;
}

#if 0
unsigned int AddMmsSection(MMS_INFO &mmsInfo)
{
#if 0
    int nSectionId = 0;
    g_nSectionId ++;
    nSectionId = g_nSectionId;
    shell_print("g_nSection id is %d \n",g_nSectionId);
    
    g_MmsInfo.insert(std::make_pair(nSectionId, mmsInfo));
    return nSectionId;
#else
    if (g_SectionNum< MAX_SECTION_NUM  )
    {
        if ( 0 ==g_MmsUsed[g_SectionNum] )
        {
            memcpy(&g_MmsInfo[g_SectionNum],&mmsInfo,sizeof(MMS_INFO));
            g_MmsUsed[g_SectionNum] = 1;
            g_SectionNum += 1;
        }
    }
    return g_SectionNum - 1;
#endif
    
}

int DelMmsSection(unsigned int nSectionId)
{
#if 0
	std::map<unsigned int, MMS_INFO>::iterator  posStreamHandle;
	posStreamHandle = g_MmsInfo.find(nSectionId);
	g_MmsInfo.erase(posStreamHandle);
	return 0;
#else
    //int id = nSectionId % MAX_SECTION_NUM ;
    
    if ( 1 == g_MmsUsed[nSectionId])
    {
        g_MmsUsed [nSectionId] = 0;
        memset(&g_MmsInfo[nSectionId],0x00,sizeof(MMS_INFO));
        g_SectionNum -= 1;
    }
    else
    {
        return -1;
    }
    return 0;
 #endif
}

PMMS_INFO GetMmsSection(unsigned int nSectionId)
{
#if 0
    std::map<unsigned int, MMS_INFO>::iterator  posStreamHandle;
    posStreamHandle = g_MmsInfo.find(nSectionId);

    if (posStreamHandle != g_MmsInfo.end())
        return &posStreamHandle->second;

    return NULL;
#else

    //int id = nSectionId % MAX_SECTION_NUM ;

    if ( 1 == g_MmsUsed[nSectionId])
    {
        return &g_MmsInfo[nSectionId];
    }
    else
    {
        return NULL;
    }
 #endif
 
}

#else

static char s_cUsedSun =0;


/******************************************************************************
 * mmsSectionCmp() :
 * 
 * DESCRIPTION:  比较数据结构成员是否一样，若一样则返回数组下标，或不一样返回-1
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.03.26, wangbin Create
 * --------------------
 ******************************************************************************/
int mmsSectionCmp(PMMS_INFO pstrArray)
{
    unsigned char i =0;
    for(i =0; i< MAX_SECTION_NUM; i++)
    {
        if( (pstrArray->nIp == g_MmsInfo[i].nIp) \
        && (pstrArray->usPort == g_MmsInfo[i].usPort) \
        && (pstrArray->cChannel == g_MmsInfo[i].cChannel) )
        {
            return i;
        }
    }
    return -1;
}

int mmsSectionManageUninit(void)
{
    memset(g_MmsInfo, 0, sizeof(g_MmsInfo));
    s_cUsedSun =0;
    return 0;
}

int mmsSectionAdd(PMMS_INFO pstrMmsInfo)
{
    int ret =0;
    unsigned char i =0;
    if(s_cUsedSun >= MAX_SECTION_NUM)
    {
        return -1;
    }
    ret = mmsSectionCmp(pstrMmsInfo);
    if(ret >= 0)
    {
        XCSPTL_INFO("error,mms link exist,please stop first\n");
        return -2;
    }
    else
    {
        for(i =0; i< MAX_SECTION_NUM; i++)
        {
            if(0 == g_MmsInfo[i].cUsed)
            {
                g_MmsInfo[i] = *pstrMmsInfo;
                g_MmsInfo[i].cUsed =1;
                s_cUsedSun++;

                syslog(LOG_ERR|LOG_USER,"mms sec add:ip:%x,port:%d,splitMode:%d,connectMode:%d,channel:%d,Active:%d,cUsed:%d.",\
                g_MmsInfo[i].nIp,g_MmsInfo[i].usPort,g_MmsInfo[i].cSplitMode,\
                g_MmsInfo[i].cConnectMode,\
                g_MmsInfo[i].cChannel,g_MmsInfo[i].cActive, g_MmsInfo[i].cUsed);
                return 0;
            }
        }
    }
    return -1;
}


int mmsSectionDel(PMMS_INFO pstrArray)
{
    unsigned int ret =0;
    ret = mmsSectionCmp(pstrArray);
    if(ret >= 0)
    {
        if(ret < MAX_SECTION_NUM)
        {
            syslog(LOG_ERR|LOG_USER,"mms sec del:ip:%x,port:%d,splitMode:%d,connectMode:%d,channel:%d,Active:%d,cUsed:%d.",\
            g_MmsInfo[ret].nIp,g_MmsInfo[ret].usPort,g_MmsInfo[ret].cSplitMode,g_MmsInfo[ret].cConnectMode,\
            g_MmsInfo[ret].cChannel,g_MmsInfo[ret].cActive, g_MmsInfo[ret].cUsed);
            
            memset(&g_MmsInfo[ret], 0, sizeof(MMS_INFO));
            s_cUsedSun--;
            return 0;
        }
        else
        {
            XCSPTL_INFO("error,array index is too big\n");
            return -2;
        }
    }
    else
    {
        XCSPTL_INFO("error,array have no this info\n");
        return -1;
    }
	return 0;
}

int mmsSectionNum(void)
{
    return s_cUsedSun;
}

int mmsSectionGet(PMMS_INFO pstrArray, unsigned int uiSecIp)
{
    unsigned char i =0;
    for(i =0; i< MAX_SECTION_NUM; i++)
    {
        if( (g_MmsInfo[i].cUsed)&& ( g_MmsInfo[i].nIp == uiSecIp )&&( 0 == g_MmsInfo[i].cSplitMode ))
        {
            *pstrArray = g_MmsInfo[i];
            return i;
        }
        /*shell_print("mmsSecGet:i:%d,used:%d,ip:0x%x,secIp:0x%x,cSplitMode:%d!\n",\
        i,g_MmsInfo[i].cUsed,g_MmsInfo[i].nIp,uiSecIp, g_MmsInfo[i].cSplitMode);*/
    }
    return -1;
}

int mmsSectionIdGet(PMMS_INFO pstrArray, unsigned char cId)
{
    if(cId < MAX_SECTION_NUM)
    {
        if(g_MmsInfo[cId].cUsed)
        {
            *pstrArray = g_MmsInfo[cId];
            return cId;
        }
    }
    else
    {
        XCSPTL_ERROR("error,array have no this id\n");
        return -1;
    }
    return 0;    
}


#endif



