/******************************************************************************
 * FtpEpSend.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.28, xcsworker Create
 * --------------------
 ******************************************************************************/
#ifndef _EP_SEND_H
#define  _EP_SEND_H


class FtpEpObj;

class FtpEpSend
{
public:
	FtpEpSend();
	~FtpEpSend();
	bool  	FtpInit(FtpServerInfo *pStrFtpServer,
			const char *pcNasPath,
			const char *pcPortName,
			const CameraInfo *strCameraInfo);
	bool  	FtpPutBuf(char *pcBuf,unsigned int uiLen,int index);
	bool  	FtpSendPicture(JpegSendInfo  *pStrSendInfo);	
	bool  	FtpSwitchDbgLevel(int siLevel);
	bool  	FtpPrintStaticsInfo(void);
	bool  	FtpSetNasPath(const char *pcNasPath);
	bool  	FtpSetPortName(const char *pcPortName);
	bool  	FtpSetServer(FtpServerInfo *pStrFtpServer);
	bool  	FtpSetBackupsServer(FtpServerInfo *pStrFtpServer);
	bool  	FtpSetPlatformInfo(StrPlatformInfo *pStrPlatformInfo);
	bool  	FtpSetEPRoadRemarks(const char *pcRoadRemarks);/* add by its 2010-7-5 */
	char* 	FtpGetSoftWareVer(void);
	bool	FtpSetIrregInfo(FTP_EP_IRREG_INFO* strEpIrregInfo);
private:
	FtpEpObj    *m_pCEpHandle;

};

#endif
