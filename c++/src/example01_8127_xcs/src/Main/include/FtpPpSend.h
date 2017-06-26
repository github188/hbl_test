/******************************************************************************
 * FtpPpSend.h:
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
#ifndef _PP_SEND_H
#define  _PP_SEND_H


class FtpPassPortObj;

class FtpPpSend
{
public:
    	FtpPpSend();
    	~FtpPpSend();
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
	char* 	FtpGetSoftWareVer(void);
	bool	FtpSetIrregInfo(FTP_PP_IRREG_INFO* strPpIrregInfo);

	bool	FtpSetDirLaneName(const char *pcDirName,const char *pcLaneName);
	
private:
	FtpPassPortObj    *m_pCPpHandle;
	
};

#endif

