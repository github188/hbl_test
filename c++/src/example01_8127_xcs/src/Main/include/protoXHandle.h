/******************************************************************************
 * protoXHandle.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.09, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _PROTO_XCS_HANDLE_H__
#define _PROTO_XCS_HANDLE_H__

#include "msgQLib.h"
#include "protoXcs.h"
#include "SysHandle.h"

int kjXcsDebugSwitch(int nLevel);

typedef struct tagClientThreadParam
{
	int nIndex;
	void *pArg;
} CTP;

typedef struct tagDvsClientInfo
{
	pthread_t		threadTcpClinet;     // thread id
	CTP				param;
	MSG_Q_ID		msgQEvent;					
	
	int				sockClient;    // socket description
	unsigned int	sin_addr;      // source ip
	unsigned short	sin_port;      // tcp port
	char			cRsv[2];
	char			*pRecvBuf;     // recv buf
	int				nRecvBufLen;
	int				nRecvLen;      // receive length
	int				nPackLen;      // package complete length
	char			*pSendBuf;     // send buf
	int				nSendBufLen;   
	int				nSendLen;      // send length
	int				nTTL;
    sem_t           sem;           // 
	int             bUserValid;       // 0-无用户，1-正常用户
	MSG_Q_ID        msgQOutput;       // 向外传数据的消息队列
    unsigned int    uiSessionId;
    unsigned int    uiHeartBit;     //记录该线程连接心跳数
    unsigned int    uiThreadTime;   //记录该线程时间数
    unsigned int	uiHBFlag;		//心跳线程标志
}XCS_CLIENT;

typedef struct tagMmsInfo
{
	unsigned int 	nIp;
	unsigned short 	usPort;
	char 			cSplitMode;
	char			cConnectMode;
	char			cChannel;
    char            cActive;
    char            cUsed;
	char			cRsv[1];
}MMS_INFO,*PMMS_INFO;

class CHandleXcsPtl  
{
	public:
		int DataPump(XCS_CLIENT * pCmdPack);
		int InitClient(XCS_CLIENT * pCmdPack);
		int UninitClient(XCS_CLIENT * pCmdPack);
		
        unsigned int    uiDataUploadIp;
        unsigned short  usDataUploadPort;

		/*Start of ITS on 2011-6-2 13:37 2.0.0.1*/
		unsigned int    m_uiThreadFlag;
		/*End of ITS on 2011-6-2 13:37 2.0.0.1*/
        
	private:

		int DispatchCmd(XCS_CLIENT * pCmdPack);

        /* 登录协议 */
        int CmdUserLogin(XCS_CLIENT *pCmdPack);
        int CmdUserLogout(XCS_CLIENT *pCmdPack);

        /* 实时监视 */  
        int realMVideoQuery(XCS_CLIENT *pCmdPack);
        int realMVideoCut(XCS_CLIENT *pCmdPack);
        int realMPicParamQuery(XCS_CLIENT *pCmdPack);
        int realMPicParamSet(XCS_CLIENT *pCmdPack);
        int realMVideoParamQuery(XCS_CLIENT *pCmdPack);
        int realMVideoParamSet(XCS_CLIENT *pCmdPack);
        int realMDataReportParamSet(XCS_CLIENT * pClintSocket);
        
        /* 外设属性设置 */
        int dCtlCapParamQuery(XCS_CLIENT *pCmdPack);
        int dCtlCapParamSet(XCS_CLIENT *pCmdPack);

        /* 远程维护 */
        int rCareSysTimeQuery(XCS_CLIENT *pCmdPack);
        int rCareSysTimeSet(XCS_CLIENT *pCmdPack);
        int rCareSysVerQuery(XCS_CLIENT *pCmdPack);
        int rCareLogQuery(XCS_CLIENT *pCmdPack);
        int rCareLogDownLoad(XCS_CLIENT *pCmdPack);
        int rCareUpgrade(XCS_CLIENT *pCmdPack);
        int rCareUpdateProgress(XCS_CLIENT *pCmdPack);
        int rCareReboot(XCS_CLIENT *pCmdPack);
        int rCareNtpInfoQuery(XCS_CLIENT *pCmdPack);
        int rCareNtpInfoSet(XCS_CLIENT *pCmdPack);
		int rCareRestoreDevConf(XCS_CLIENT *pClintSocket);

		int rCareRestartTimeQuery(XCS_CLIENT *pClintSocket);
		int rCareRestartTimeSet(XCS_CLIENT *pClintSocket);

        /* 卡口相关 */
        int bayonetGInfoQuery(XCS_CLIENT *pClintSocket);
        int bayonetGInfoSet(XCS_CLIENT *pClintSocket);
        int bayonetChannelInfoQuery(XCS_CLIENT *pClintSocket);
        int bayonetChannelInfoSet(XCS_CLIENT *pClintSocket);
        int bayonetVLInfoQuery(XCS_CLIENT *pClintSocket);
        int bayonetHighVLInfoSet(XCS_CLIENT *pClintSocket);
        int bayonetHandSnap(XCS_CLIENT *pClintSocket);
        int bayonetFtpInfoQuery(XCS_CLIENT *pClintSocket);
        int bayonetFtpInfoSet(XCS_CLIENT *pClintSocket);

        int bayonetJpgOsdConfQuery(XCS_CLIENT *pClintSocket);
        int bayonetJpgOsdConfSet(XCS_CLIENT *pClintSocket);

		int bayonetIrregInfoQuery(XCS_CLIENT *pClintSocket);
		int bayonetIrregInfoSet(XCS_CLIENT *pClintSocket);


		int	bayonetBackupsFtpInfoQuery(XCS_CLIENT *pClintSocket);
		int	bayonetBackupsFtpInfoSet(XCS_CLIENT *pClintSocket);

        /* 电子警察相关 */
        int electPoliceGInfoQuery(XCS_CLIENT *pClintSocket);
        int electPoliceGInfoSet(XCS_CLIENT *pClintSocket);
        int electPoliceChannelInfoQuery(XCS_CLIENT *pClintSocket);
        int electPoliceChannelInfoSet(XCS_CLIENT *pClintSocket);
        int electPoliceRedLightStaQuery(XCS_CLIENT *pClintSocket);
        int electPoliceSimuHandSnap(XCS_CLIENT *pClintSocket);
        int electPoliceForceRedLQuery(XCS_CLIENT *pClintSocket);
        int electPoliceForceRedLSet(XCS_CLIENT *pClintSocket);
        int electPoliceFtpInfoQuery(XCS_CLIENT *pClintSocket);
        int electPoliceFtpInfoSet(XCS_CLIENT *pClintSocket);
        int epTimeRQuery(XCS_CLIENT *pClintSocket);
        int epTimeRSet(XCS_CLIENT *pClintSocket);

        int epAssistRedLightQuery(XCS_CLIENT *pClintSocket);
        int epAssistRedLightSet(XCS_CLIENT *pClintSocket);

        int electPoliceLprInfoQuery(XCS_CLIENT *pClintSocket);
        int electPoliceLprInfoSet(XCS_CLIENT *pClintSocket);
        int electPoliceIrregInfoQuery(XCS_CLIENT *pClintSocket);
        int electPoliceIrregInfoSet(XCS_CLIENT *pClintSocket);

        int electPoliceNewAddIrregInfoQuery(XCS_CLIENT *pClintSocket);
        int electPoliceNewAddIrregInfoSet(XCS_CLIENT *pClintSocket);

        int epSOTNoEntryQuery(XCS_CLIENT *pClintSocket);
        int epSOTNoEntrySet(XCS_CLIENT *pClintSocket);

		int epJpgOsdConfQuery(XCS_CLIENT *pClintSocket);
		int epJpgOsdConfSet(XCS_CLIENT *pClintSocket);
		
		int	epBackupsFtpInfoQuery(XCS_CLIENT *pClintSocket);
		int	epBackupsFtpInfoSet(XCS_CLIENT *pClintSocket);
        
        /* 高清摄像机信息 */
        int hdCamaraAttribQuery(XCS_CLIENT *pClintSocket);
        int hdCamaraStaInfoQuery(XCS_CLIENT *pClintSocket);
        int hdCamaraParamQuery(XCS_CLIENT *pClintSocket);
        int hdCamaraCtlInfoQuery(XCS_CLIENT *pClintSocket);
        int hdCamaraCtlInfoSet(XCS_CLIENT *pClintSocket);
        int hdCamaraTimeRQuery(XCS_CLIENT *pClintSocket);
        int hdCamaraTimeRSet(XCS_CLIENT *pClintSocket);
		int hdCamaraDefaultParamQuery(XCS_CLIENT *pClintSocket);

        /* 网络信息 */
        int netInfoQuery(XCS_CLIENT *pClintSocket);
        int netInfoSet(XCS_CLIENT *pClintSocket);
        int netInfoOfPicStoreQuery(XCS_CLIENT *pClintSocket);
        int netInfoOfPicStoreSet(XCS_CLIENT *pClintSocket);
        int netInfoOfRecordStoreQuery(XCS_CLIENT *pClintSocket);
        int netInfoOfRecordStoreSet(XCS_CLIENT *pClintSocket);
        
		int netInfoOfNRMQuery(XCS_CLIENT *pClintSocket);
		int netInfoOfNRMSet(XCS_CLIENT *pClintSocket);		
#ifdef MODULE_REC
		/* 录像信息 */
		int RecordRtspQuery(XCS_CLIENT *pClintSocket);
		int RecordRtspSet(XCS_CLIENT *pClintSocket);
		int RecordFileQuery(XCS_CLIENT *pClintSocket);
		int RecordFilePlayQuery(XCS_CLIENT *pClintSocket);
		int RecordFileDownLoadQuery(XCS_CLIENT *pClintSocket);
#endif
		/* 330接入信息 */
        int devCon330InfoQuery(XCS_CLIENT *pClintSocket);
        int devCon330InfoSet(XCS_CLIENT *pClintSocket);        

        /* V3接入信息 */
        int devConV3InfoQuery(XCS_CLIENT *pClintSocket);
        int devConV3InfoSet(XCS_CLIENT *pClintSocket);

		/* 电警330接入信息 */
        int devConEp330InfoQuery(XCS_CLIENT *pClintSocket);
        int devConEp330InfoSet(XCS_CLIENT *pClintSocket);

		/* V2接入信息 */
        int devConV2InfoQuery(XCS_CLIENT *pClintSocket);
        int devConV2InfoSet(XCS_CLIENT *pClintSocket);

		/* GB28181平台接入 */
		int devConGb28181InfoQuery(XCS_CLIENT *pClintSocket);
		int devConGb28181InfoSet(XCS_CLIENT *pClintSocket);

        /* 心跳 */
        int CmdHeartBeat(XCS_CLIENT *pCmdPack);

		int rCareNetManageQuery(XCS_CLIENT *pCmdPack);
        
        /* 应答 */
        int SetSimpleAckHead(PSTR_PACKET_HEADER pPackHead,unsigned char cDataType,unsigned char cSubDataType,int nPayloadLen);
        int SetAckPack(PSTR_PACKET_HEADER pPackHead);
        int SetNakPack(PSTR_PACKET_HEADER pPackHead, char cErrorCode);
	
};

#if 0
unsigned int AddMmsSection(MMS_INFO &mmsInfo);
int DelMmsSection(unsigned int nSectionId);
PMMS_INFO GetMmsSection(unsigned int nSectionId);
#else
int mmsSectionManageUninit(void);
int mmsSectionCmp(PMMS_INFO pstrArray);
int mmsSectionAdd(PMMS_INFO pstrMmsInfo);
int mmsSectionDel(PMMS_INFO pstrArray);
int mmsSectionGet(PMMS_INFO pstrArray, unsigned int uiSecIp);
int mmsSectionNum(void);
int mmsSectionIdGet(PMMS_INFO pstrArray,unsigned char cId);
#endif



#endif /* _PROTO_XCS_HANDLE_H__ */

