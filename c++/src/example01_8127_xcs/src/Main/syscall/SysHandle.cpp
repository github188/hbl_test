/******************************************************************************
 * SysHandle.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.18, wangbin Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/sem.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "xcsSysMsg.h"
#include "sysHandle_define.h"
#include "rtc_util.h"


#include "McLog.h"
#include "SysHandle.h"

#include "JpegSnap.h"
#include "FtpEpSend.h"
#include "FtpPpSend.h"

#include "xcsCommDef.h"
#include "vehicleReport.h"
#include "Ieee1394.h"

#include "Record.h"
#include "nfsMmc.h"

#include "DataUpLoad.h"

#include "NtpConf.h"
#include "Ntp.h"

#include "dspParamManage.h"

#include "xcsGlobalDef.h"

#include "nMCtlApi.h"
#include "epRecApi.h"

#include "dsp.h"
#include "dsp_define.h"
#include "dsp_service.h"

#if defined(CYC500XZD)||defined(CYC200XZD)
#include "zebraCtl.h"
#endif

char g_cOnvifVideoSta =0;   /* onvif��Ƶ����״̬ */
char g_cNetRecVideoSta =0;  /* �ƴ洢��Ƶ����״̬ */

char g_cDspInitSta =0;      /* DSPģ������״̬1:�ɹ� ����ʧ�� */
char g_cRecInitSta =0;      /* ¼��ģ������״̬1:�ɹ� ����ʧ�� */
char g_cMmsInitSta =0;      /* MMSģ������״̬1:�ɹ� ����ʧ�� */
char g_cFtpInitSta =0;      /* FTPģ������״̬1:�ɹ� ����ʧ�� */
char g_cXcsInitSta =0;      /* XCSģ������״̬1:�ɹ� ����ʧ�� */
char g_cCCDCtlSta  =0;      /* CCD����ģ������״̬1:�ɹ� ����ʧ�� */
char g_cNMModuleSta=0;      /* ����ģ������״̬1:�ɹ� ����ʧ�� */
char g_cAlarmRecSta=0;      /* ����¼��ģ������״̬1:�ɹ� ����ʧ�� */

/* ���������������ȷ���ĳ����� */
unsigned int g_uiCamLaneSum =1;

/* ���ÿͻ���ʵ�����õĳ�������Ҳ����ʵ��֧�ֵĳ����� */
unsigned int g_uiClientLaneSum =1;

int g_SysHandle_Debug   = 0xffff;
int g_Recv_Debug        = 0x1;

/* ���������� */
int g_iVehicleHeartSum =0;

/* ��ʱ��ѯ���������� */
int g_iMonitorVehicleSum =0;


/* ¼�񻺴�ȫ��ָ�� */
void* g_pRecData=NULL;

FtpPpSend *g_CPPFtp = NULL;

#if defined(CYC500XKW4C)||defined(CYC500XAW)
FtpEpSend *g_CEPFtp = NULL;
#endif

/* ץ��ͼƬ·��ͳ����Ϣ�ṹ */
#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)
extern STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[1];
#elif defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
extern STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[MAX_VEHICLE_NUM];
#else 
extern STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[MAX_VEHICLE_NUM];
#endif
char    g_bForceRed[MAX_VEHICLE_NUM]= {RED_LIGHT_OFF};      /* ģ��ͨ��Ϊ��ƣ����ȼ���ߣ�ֻҪΪ�漴��ʾΪ��� */

LANE_SWITCH_STA_INFO    g_EPLaneSwitchInfo[MAX_VEHICLE_NUM]   = {0};    /* �羯�������״̬���л�״̬ */
PP_LANE_SWITCH_STA_INFO g_PPLaneSwitchInfo[MAX_VEHICLE_NUM]   = {0};    /* ���ڳ����л�״̬ */

CARROAD_DECT_CONF g_aEPConfInfo[MAX_VEHICLE_NUM];
CARROAD_DECT_CONF g_aPassPortChanInfo[MAX_VEHICLE_NUM];

CAMERA_TIME_RANGE_INFO g_strCamTimeR[MAX_CAM_GROUP_NUM];

/* Ϊ�˷�ֹÿ���жϺ�����в������ã������¼�ϴ����õ�����ţ����ϵ���û�����ʱ��ǿ������һ�� */
int    g_iCamParamGroupId =0;

/* ��¼����������״̬��ͬʱҲ��Ϊ���ܷ��ͱ�־ */
int g_iErrorTimes =203;

int g_aiCoilSta[MAX_VEHICLE_NUM] = {MODE_SHUTDOWN_STA};
int g_aiVirtualSta[MAX_VEHICLE_NUM] = {MODE_SHUTDOWN_STA};
int g_aiLampSta[MAX_VEHICLE_NUM] = {MODE_SHUTDOWN_STA};

int g_aiVehicleType[MAX_COM_DEV] = {0};

/* ��Ƽ�ⷽʽȫ�ֱ�������Ҫ�����л��ж� */
int g_iRLDecType =0;

extern MSG_Q_ID pSysManageQueue;

extern CDataUpLoad  *g_DataReport;
extern int          g_bTcpDataOk;
extern CNtp         *g_NtpCheck;

extern unsigned long g_ulOrgTime;

/* ���⹤��״̬ģʽȫ�ֱ�������ʼ��Ϊ����� */
extern int g_iWorkFlashMode[MAX_VEHICLE_NUM];

/* ����·�ڳ�����Ϣ�����ֶγ��� */
#define ROAD_REMARKS_LEN 64

extern sem_t sem_dsp;

/* Ƶ���������л�״̬ */
extern int g_iPreSta;
extern int g_iEpPreSta;

extern void secSwitchToDateString(unsigned long ulSecs, char *pcBuf);

extern int olviaSendInit(void);
extern int olviaSendUnIn(void);

extern strCXProInfo g_strCXProInfo;

int ppLaneSwitch(char cLane, char cSta);

int DataReportInit()
{
    bool ret = true;
    
    if ( NULL == g_DataReport)
    {
        g_DataReport = new CDataUpLoad;
        if (NULL == g_DataReport )
            return -1;
    }

    ret = g_DataReport->UpLoadInit();

    if ( !ret )
    {
        shell_print("data report module init failed\n");
        return -1;
    }
    return 0;
}
int DataReportUninit(void)
{
    bool ret = false;

    if ( NULL != g_DataReport)
    {
        ret = g_DataReport->UpLoadUninit();
        if ( !ret )
        {
            shell_print("data report module uninit failed\n");
            return -1;
        }
        delete g_DataReport;
        g_DataReport = NULL;
    }
    return 0;
}

int SetDataReportParam(STR_DATA_REPORT_SET *pStrDataReportParam)
{
    if ( (NULL == pStrDataReportParam) ||(NULL == g_DataReport))
    {
        shell_print("DataReport Param pointer is Null\n");
        return -1;
    }

    unsigned int    uiRecvIp;
    unsigned short  usRecvPort;
    unsigned int    uiRecvType;

    bool bRet = false;
    

    uiRecvIp      = pStrDataReportParam->uiRecvIp;
    usRecvPort    = pStrDataReportParam->usRecvPort;
    uiRecvType    = pStrDataReportParam->uiRecvType;
  
   // uiRecvIp      = 0xac10491f;//pStrDataReportParam->uiRecvIp;
  //  usRecvPort    = 0x47dc;//pStrDataReportParam->usRecvPort;  18396
  //  uiRecvType    = 0x01;//pStrDataReportParam->uiRecvType;
	

    SYSHANDLE_INFO("data zzzffff report,ip:0x%x,port:0x%x,recType:0x%x\n",uiRecvIp,usRecvPort,uiRecvType);


    /*���������ϱ�,
            bit 0 ͼƬ�ϱ���
            bit1 ���ڲɼ������ϱ�,
            bit2 �����·������ϱ�

        */   
        
    if ( uiRecvType &0x07)
    {
        bRet = g_DataReport->AddServer(uiRecvIp, usRecvPort);

        if ( bRet )
        {
            g_bTcpDataOk = 1;
        }
        else
        {
            g_bTcpDataOk = 0;
        }
    }
    else
    {
        g_bTcpDataOk = 0;
        g_DataReport->DelServer(uiRecvIp, usRecvPort);
    }
    return 0;
    
}

/* ���ص羯��������ʻ���� */
char ePCarDistanceGet()
{
    int ret =0;
    ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if ( 0== ret )
	{
	    return strEPGlobalConf.cCarDistance;
	}
	else
	{
	    return ret;
	}
}

/* ���ص羯ָ��������������ʻ���� */
char ePLaneCarDistanceGet(int iLane)
{
    int ret =0;
    ELEC_POLICE_VEHICLE_CONF_INFO strEPLaneConf;

    memset(&strEPLaneConf, 0, sizeof(strEPLaneConf));
	ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneConf, iLane);
	if ( 0== ret )
	{
	    return strEPLaneConf.cCarDistance;
	}
	else
	{
	    return ret;
	}
}

int GetLocalMac(char *pszMac)
{  
    int sock_mac;  
      
    struct ifreq ifr_mac;  

	if(NULL == pszMac)
	{
		return -1;
	}
	
    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );  
    if( sock_mac == -1)  
    {  
        SYSHANDLE_ERROR("create socket falise...mac \n");  
        return -1;  
    }  
      
    memset(&ifr_mac, 0, sizeof(ifr_mac));     
    strncpy(ifr_mac.ifr_name, "eth0", sizeof(ifr_mac.ifr_name)-1);     
  
    if( (ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)  
    {  
        SYSHANDLE_ERROR("mac ioctl error \n");  
        return -1;  
    }  
      
    sprintf(pszMac,"%02x%02x%02x%02x%02x%02x",  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],  
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);  
  
    SYSHANDLE_ERROR("local mac:%s \n",pszMac);      
      
    close( sock_mac );  
    return 0;
}  

int GetLocalIp(char *pcIpaddr)  
{  
  
    int sock_get_ip;  
  
    struct   sockaddr_in *sin;  
    struct   ifreq ifr_ip;     

  	if( NULL == pcIpaddr)
  	{
  		return 0;
  	}
  	
    if ((sock_get_ip=socket(AF_INET, SOCK_STREAM, 0)) == -1)  
    {  
         SYSHANDLE_ERROR("socket create failse...GetLocalIp!\n");
         return -1;
    }

    memset(&ifr_ip, 0, sizeof(ifr_ip));     
    strncpy(ifr_ip.ifr_name, "eth0", sizeof(ifr_ip.ifr_name) - 1);     
   
    if( ioctl( sock_get_ip, SIOCGIFADDR, &ifr_ip) < 0 )     
    {
    	 SYSHANDLE_ERROR("socket ioctl failse...GetLocalIp!\n");
         return -1;
    }       
    sin = (struct sockaddr_in *)&ifr_ip.ifr_addr;     
    strcpy(pcIpaddr, inet_ntoa(sin->sin_addr));         
      
    SYSHANDLE_ERROR("local ip:%s,0x%x \n", pcIpaddr, sin->sin_addr);
    close( sock_get_ip );  
      
    return (int)(sin->sin_addr.s_addr);
}  

int ftpSendInit(void)
{
    int ret =0;
    char szIpAddr[20] = {0};

    g_CPPFtp = new  FtpPpSend;
	if(NULL == g_CPPFtp)
	{
	    SYSHANDLE_ERROR("creat pp ftp global class error\n");
	    return -1;
	}

#if defined(CYC500XKW4C)||defined(CYC500XAW)
	g_CEPFtp = new FtpEpSend;
	if(NULL == g_CEPFtp)
	{
	    SYSHANDLE_ERROR("creat ep ftp global class error\n");
	    return -1;
	}
#endif
	FtpServerInfo strFtpServerInfo;
	
    FTP_CONF_INFO strFtpConf;
	CameraInfo strDevCameraInfo;
	
    memset(&strFtpConf, 0, sizeof(strFtpConf));
    memset(&strFtpServerInfo,0,sizeof(strFtpServerInfo));

	
	memset(&strDevCameraInfo, 0, sizeof(strDevCameraInfo));

	strDevCameraInfo.uiCameraIp = GetLocalIp(szIpAddr);
	//GetLocalMac(szIpAddr);
	
    strFtpServerInfo.m_bNasEnable = false;
    g_CPPFtp->FtpSwitchDbgLevel(DBG_ALL);	// tangzl 0725
    g_CPPFtp->FtpInit(&strFtpServerInfo, CIFS_MOUNT_POINT, "hangzhou", &strDevCameraInfo);	// tangzl 0725
    ret = ftpPP330PlatFormInfoSet();

#if defined(CYC500XKW4C)||defined(CYC500XAW)
    g_CEPFtp->FtpSwitchDbgLevel(DBG_ALL);	// tangzl 0725
    g_CEPFtp->FtpInit(&strFtpServerInfo, CIFS_MOUNT_POINT, "hangzhou", &strDevCameraInfo);	// tangzl 0725
    ret = ftpPP330PlatFormInfoSet();
    ret += ftpEP330PlatFormInfoSet();
    ret += ftpEPRoadRemarksSet();
#endif
    ftpServerChange();

    ret = ftpPPChannelNameSet();
    ret = ftpPPIrregNameInfoSet();
    
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    ret = ftpEPChannelNameSet();
    ret = ftpEPIrregNameInfoSet();
#endif
    
    return 0;
}

int ftpServerChange(void)
{
    int ret =0;
    struct in_addr in;
    char szTemp[128];
    FTP_CONF_INFO strPPFtpConf;
    
    FTP_CONF_INFO strPPBackupsFtpConf;
    
    FTP_CONF_INFO strEPFtpConf;

	FTP_CONF_INFO strEpBackupsFtpConf;
    
    FtpServerInfo strFtpServerInfo;
    NET_HDD_CONF_INFO strPicStorageInfo;
    int iCifsSta =0;

    memset(&strFtpServerInfo, 0, sizeof(strFtpServerInfo));

    iCifsSta = checkmount(e_cifs);
    
    ret = SysConfGet(e_PASSPORT_FTP_CONF, &strPPFtpConf, 0);
    ret += SysConfGet(e_PIC_NET_HDD_CONF, &strPicStorageInfo, 0);
    if ( 0== ret )
    {
        in.s_addr = strPPFtpConf.uiFtpIP;
        in.s_addr = htonl(in.s_addr);
        sprintf(szTemp,"%s", inet_ntoa(in));
        strcpy(strFtpServerInfo.m_host, szTemp);
        strFtpServerInfo.m_port = strPPFtpConf.usFtpPort;
        strcpy(strFtpServerInfo.m_user, strPPFtpConf.szUserName);
        strcpy(strFtpServerInfo.m_passwd, strPPFtpConf.szUserPasswd);
        strFtpServerInfo.m_bFtpEnable = strPPFtpConf.uiEnable;
        
        if ( (1== strPicStorageInfo.uiEnable ) && (1== strPicStorageInfo.uiAvaiable) && ( 0== iCifsSta ))
        {
            strFtpServerInfo.m_bNasEnable = true;
            g_CPPFtp->FtpSetNasPath(CIFS_MOUNT_POINT);	// tangzl 0725
        }
        else
        {
            strFtpServerInfo.m_bNasEnable = false;
            ret = checkmount(e_udisk);
            if(0 == ret)
            {
                SYSHANDLE_ERROR("pp u disk is ok,ftp nas path is %s\n", U_DIST_MOUNT_POINT);
                strFtpServerInfo.m_bNasEnable = true;
                g_CPPFtp->FtpSetNasPath(U_DIST_MOUNT_POINT);	// tangzl 0725
            }
            else
            {
                SYSHANDLE_ERROR("pp check u disk failed!try sd.\n");

                ret = checkmount(e_sd);
	            if(0 == ret)
	            {
	                SYSHANDLE_ERROR("pp sd is ok,ftp nas path is %s\n", SD_MOUNT_POINT);
	                strFtpServerInfo.m_bNasEnable = true;
	                g_CPPFtp->FtpSetNasPath(SD_MOUNT_POINT);	// tangzl 0725
	            }
	            else
	            {
	                SYSHANDLE_ERROR("pp check sd failed!\n");
	            }

            }
        }
    }

    ret = ftpXmlFileSendFlagGet();
    strFtpServerInfo.m_iSendFlag = ret;
    ret = g_CPPFtp->FtpSetServer(&strFtpServerInfo);	// tangzl 0725
    
	/* �������ñ���ftp��Ϣ */
	ret = SysConfGet(e_PASSPORT_BACKUPS_FTP_CONF, &strPPBackupsFtpConf, 0);
    if ( 0== ret )
    {

        memset(&strFtpServerInfo, 0, sizeof(strFtpServerInfo));
    	
        in.s_addr = strPPBackupsFtpConf.uiFtpIP;
        in.s_addr = htonl(in.s_addr);
        sprintf(szTemp,"%s", inet_ntoa(in));
        strcpy(strFtpServerInfo.m_host, szTemp);
        strFtpServerInfo.m_port = strPPBackupsFtpConf.usFtpPort;
        strcpy(strFtpServerInfo.m_user, strPPBackupsFtpConf.szUserName);
        strcpy(strFtpServerInfo.m_passwd, strPPBackupsFtpConf.szUserPasswd);
        strFtpServerInfo.m_bFtpEnable = strPPBackupsFtpConf.uiEnable;


        ret = g_CPPFtp->FtpSetBackupsServer(&strFtpServerInfo);	// tangzl 0725
	}

#if defined(CYC500XKW4C)||defined(CYC500XAW)
    ret = SysConfGet(e_ELEC_POLICE_FTP_CONF, &strEPFtpConf, 0);
    ret += SysConfGet(e_PIC_NET_HDD_CONF, &strPicStorageInfo, 0);
    if ( 0== ret )
    {
        in.s_addr = strEPFtpConf.uiFtpIP;
        in.s_addr = htonl(in.s_addr);
        sprintf(szTemp,"%s", inet_ntoa(in));
        strcpy(strFtpServerInfo.m_host, szTemp);
        strFtpServerInfo.m_port = strEPFtpConf.usFtpPort;
        strcpy(strFtpServerInfo.m_user, strEPFtpConf.szUserName);
        strcpy(strFtpServerInfo.m_passwd, strEPFtpConf.szUserPasswd);
        strFtpServerInfo.m_bFtpEnable = strEPFtpConf.uiEnable;
        
        if ( (1== strPicStorageInfo.uiEnable )&&(1== strPicStorageInfo.uiAvaiable) && ( 0 == iCifsSta ) )
        {
            strFtpServerInfo.m_bNasEnable = true;
            g_CEPFtp->FtpSetNasPath(CIFS_MOUNT_POINT);	// tangzl 0725
        }
        else
        {
            strFtpServerInfo.m_bNasEnable = false;
            ret = checkmount(e_udisk);
            if(0 == ret)
            {
                SYSHANDLE_ERROR("ep u disk is ok,ftp nas path is %s\n", U_DIST_MOUNT_POINT);
                strFtpServerInfo.m_bNasEnable = true;
                g_CEPFtp->FtpSetNasPath(U_DIST_MOUNT_POINT);	// tangzl 0725
            }
            else
            {
                SYSHANDLE_ERROR("ep check u disk failed,try sd!\n");
                ret = checkmount(e_sd);
	            if(0 == ret)
	            {
	                SYSHANDLE_ERROR("ep sd is ok,ftp nas path is %s\n", SD_MOUNT_POINT);
	                strFtpServerInfo.m_bNasEnable = true;
	                g_CEPFtp->FtpSetNasPath(SD_MOUNT_POINT);	// tangzl 0725
	            }
	            else
	            {
	                SYSHANDLE_ERROR("ep check sd failed!\n");
	            }
            }
        }
    }
    ret = ftpXmlFileSendFlagGet();
    strFtpServerInfo.m_iSendFlag = ret;
    ret = g_CEPFtp->FtpSetServer(&strFtpServerInfo);	// tangzl 0725
    
	/* �������ñ���ftp��Ϣ */
	ret = SysConfGet(e_EP_BACKUPS_FTP_CONF, &strEpBackupsFtpConf, 0);
    if ( 0== ret )
    {

        memset(&strFtpServerInfo, 0, sizeof(strFtpServerInfo));
    	
        in.s_addr = strEpBackupsFtpConf.uiFtpIP;
        in.s_addr = htonl(in.s_addr);
        sprintf(szTemp,"%s", inet_ntoa(in));
        strcpy(strFtpServerInfo.m_host, szTemp);
        strFtpServerInfo.m_port = strEpBackupsFtpConf.usFtpPort;
        strcpy(strFtpServerInfo.m_user, strEpBackupsFtpConf.szUserName);
        strcpy(strFtpServerInfo.m_passwd, strEpBackupsFtpConf.szUserPasswd);
        strFtpServerInfo.m_bFtpEnable = strEpBackupsFtpConf.uiEnable;


        ret = g_CEPFtp->FtpSetBackupsServer(&strFtpServerInfo);	// tangzl 0725
	}
#endif
    if(TRUE == ret)
    {
        SYSHANDLE_INFO("reset ftp server ok\n");
    }
    else
    {
        SYSHANDLE_INFO("reset ftp server failed\n");
    }
    return ret;
}

int epPPPicSendFlag(void)
{
    int ret =0;
    ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
    memset(&strEPOverAllConf, 0, sizeof(strEPOverAllConf));
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read ep overall conf failed\n");
	    return 0;
    }
    else
    {
        if(strEPOverAllConf.cPPSendEn == 1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}

int ftpPPChannelNameSet(void)
{
    int ret =0;
    PASSPORT_OVERALL_CONF_INFO strPPOverAllConf;
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp overall conf failed\n");
	    return -1;
    }
    ret = g_CPPFtp->FtpSetPortName(strPPOverAllConf.szPassPortName);	// tangzl 0725
    return ret;
}

int ftpPPIrregNameInfoSet(void)
{
    int ret =0;
    PP_IRREG_CONF_INFO strPPIrregConfGet;
    PASSPORT_OVERALL_CONF_INFO strPPOverAllConf;
    FTP_PP_IRREG_INFO strFtpPpIrregConf;

    memset(&strFtpPpIrregConf, 0, sizeof(strFtpPpIrregConf));
    ret = SysConfGet(e_PP_IRREG_INFO_CONF, &strPPIrregConfGet, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp overall conf failed\n");
	    return -1;
    }
    
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp overall conf failed\n");
	    return -1;
    }

    memcpy(&strFtpPpIrregConf, &strPPIrregConfGet, sizeof(strPPIrregConfGet));

    memset(strFtpPpIrregConf.szPpDevCode, 0, sizeof(strFtpPpIrregConf.szPpDevCode));
    strcat(strFtpPpIrregConf.szPpDevCode, strPPOverAllConf.szPpDevCode);
    strcat(strFtpPpIrregConf.szPpDevCode, strPPOverAllConf.szExPpDevCode);
    strFtpPpIrregConf.szPpDevCode[FTP_MAX_DEV_CODE_LEN-1] =0;

	strcpy(strFtpPpIrregConf.szPpSnCode, GetProductSerialNo());
	
    ret = g_CPPFtp->FtpSetIrregInfo( &strFtpPpIrregConf );	// tangzl 0725

    return ret;
}

int ftpEPChannelNameSet(void)
{
    int ret =0;
    ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read ep overall conf failed\n");
	    return -1;
    }
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    ret = g_CEPFtp->FtpSetPortName(strEPOverAllConf.szCarRoadName);	// tangzl 0725
#endif
    return ret;
}

int ftpEPIrregNameInfoSet(void)
{
    int ret =0;
    EP_IRREG_CONF_INFO strEPIrregConfGet;
    ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
    FTP_EP_IRREG_INFO strFtpIrregConf;

    memset(&strFtpIrregConf, 0, sizeof(strFtpIrregConf));
    ret = SysConfGet(e_EP_IRREG_INFO_CONF, &strEPIrregConfGet, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read ep overall conf failed\n");
	    return -1;
    }
    
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read ep overall conf failed\n");
	    return -1;
    }

    memcpy(&strFtpIrregConf, &strEPIrregConfGet, sizeof(strEPIrregConfGet));

    memset(strFtpIrregConf.szEpDevCode, 0, sizeof(strFtpIrregConf.szEpDevCode));
    strcat(strFtpIrregConf.szEpDevCode, strEPOverAllConf.szEpDevCode);
    strcat(strFtpIrregConf.szEpDevCode, strEPOverAllConf.szExEpDevCode);
    strFtpIrregConf.szEpDevCode[FTP_MAX_DEV_CODE_LEN-1] =0;

	strcpy(strFtpIrregConf.szEpSnCode, GetProductSerialNo());

#if defined(CYC500XKW4C)||defined(CYC500XAW)
    ret = g_CEPFtp->FtpSetIrregInfo( &strFtpIrregConf );	// tangzl 0725
#endif
    return ret;
}

int ftpEPRoadRemarksSet(void)
{
    int ret =0;
    char i =0;
    char szRoadRemarks[ROAD_REMARKS_LEN]={0};
    char *pcTmp =NULL;
    char cStep =0;
    char cSum =0;
    ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
    ELEC_POLICE_VEHICLE_CONF_INFO strVehicleConfInfo;
    memset(&strEPOverAllConf, 0, sizeof(strEPOverAllConf));
    memset(&strVehicleConfInfo, 0, sizeof(strVehicleConfInfo));
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read ep overall conf failed\n");
	    return -1;
    }
    pcTmp = szRoadRemarks;
    cStep = sprintf(pcTmp, "%s", "����:");
    pcTmp += cStep;
    cSum += cStep;
    cStep = strlen(strEPOverAllConf.szRemarks);
    if((cSum + cStep) >= ROAD_REMARKS_LEN)
    {
        strncpy(pcTmp, strEPOverAllConf.szRemarks, (ROAD_REMARKS_LEN-cSum));
        pcTmp += (ROAD_REMARKS_LEN-cSum);
        cSum += (ROAD_REMARKS_LEN-cSum);
        goto flagSend;
    }
    strncpy(pcTmp, strEPOverAllConf.szRemarks, cStep);
    cSum += cStep;
    pcTmp += cStep;
    cStep = strlen(" ץ��:");
    if((cSum + cStep) >= ROAD_REMARKS_LEN)
    {
        strncpy(pcTmp, " ץ��:", (ROAD_REMARKS_LEN-cSum));
        pcTmp += (ROAD_REMARKS_LEN-cSum);
        cSum += (ROAD_REMARKS_LEN-cSum);
        goto flagSend;
    }
    cStep = sprintf(pcTmp, "%s", " ץ��:");
    pcTmp += cStep;
    cSum += cStep;
    for(i =0; i< g_uiClientLaneSum; i++)
    {
        ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strVehicleConfInfo, i);
        if( 0 != ret)
        {
            SYSHANDLE_ERROR("read ep vehicle conf failed\n");
    	    return -1;
        }
        cStep = strlen(strVehicleConfInfo.szOSDContent);
        if((cSum + cStep) >= ROAD_REMARKS_LEN)
        {
            strncpy(pcTmp, strVehicleConfInfo.szOSDContent, (ROAD_REMARKS_LEN-cSum));
            pcTmp += (ROAD_REMARKS_LEN-cSum);
            cSum += (ROAD_REMARKS_LEN-cSum);
            goto flagSend;
        }
        strncpy(pcTmp, strVehicleConfInfo.szOSDContent, cStep);
        pcTmp += cStep;
        cSum += cStep;
    }
flagSend:
    if(((unsigned int)pcTmp - (unsigned int)szRoadRemarks) >=63)
    {
        szRoadRemarks[63] = 0;
        SYSHANDLE_INFO("set1 ep xml roadremarks:%s\n", szRoadRemarks);
    }
    else
    {
        *pcTmp = 0;
        szRoadRemarks[63] = 0;
        SYSHANDLE_INFO("set2 ep xml roadremarks:%s\n", szRoadRemarks);
    }
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    SYSHANDLE_INFO("set ep xml roadremarks:%s,sumlen:%d\n", szRoadRemarks, cSum);
    ret = g_CEPFtp->FtpSetEPRoadRemarks(szRoadRemarks);	// tangzl 0725
#endif
    return ret;
}


int ftpPP330PlatFormInfoSet(void)
{
    int ret =0;
    StrPlatformInfo strFtp330Info;
    PLATFORM_330_CONF_INFO str330PlantInfo;
    memset(&strFtp330Info, 0, sizeof(strFtp330Info));
    ret = SysConfGet(e_PLATFORM_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        SYSHANDLE_ERROR("read pp 330 platform conf failed\n");
	    return -1;
    }
    strFtp330Info.i330Connect = str330PlantInfo.cEnable;
    strFtp330Info.szDriveWayId[0]= str330PlantInfo.sz330DriveWayId[0];
    strFtp330Info.szDriveWayId[1]= str330PlantInfo.sz330DriveWayId[1];
    strFtp330Info.szDriveWayId[2]= str330PlantInfo.sz330DriveWayId[2];
    strFtp330Info.szDriveWayId[3]= str330PlantInfo.sz330DriveWayId[3];
    strcpy(strFtp330Info.szDevId, str330PlantInfo.sz330DevId);
    strcpy(strFtp330Info.szDirId, str330PlantInfo.sz330DirId);
    strcpy(strFtp330Info.szKey, str330PlantInfo.sz330Key);
    
    ret = g_CPPFtp->FtpSetPlatformInfo(&strFtp330Info);	// tangzl 0725
    if(g_usEpXcsProVer != XCS_PRO_EPG_VER_2)
    {
	#if defined(CYC500XKW4C)||defined(CYC500XAW)
        ret = g_CEPFtp->FtpSetPlatformInfo(&strFtp330Info);	// tangzl 0725
    #endif
    }
    return ret;
}

#if defined(CYC500XKW4C)||defined(CYC500XAW)
int ftpEP330PlatFormInfoSet(void)
{
    int ret =0;
    StrPlatformInfo strFtp330Info;
    PLATFORM_330_CONF_INFO str330PlantInfo;
    memset(&strFtp330Info, 0, sizeof(strFtp330Info));
    ret = SysConfGet(e_EP_330_CONF, &str330PlantInfo, 0);
    if( 0 > ret)
    {
        SYSHANDLE_ERROR("read EP 330 platform conf failed\n");
	    return -1;
    }
    strFtp330Info.i330Connect = str330PlantInfo.cEnable;
    strFtp330Info.szDriveWayId[0]= str330PlantInfo.sz330DriveWayId[0];
    strFtp330Info.szDriveWayId[1]= str330PlantInfo.sz330DriveWayId[1];
    strFtp330Info.szDriveWayId[2]= str330PlantInfo.sz330DriveWayId[2];
    strFtp330Info.szDriveWayId[3]= str330PlantInfo.sz330DriveWayId[3];
    strcpy(strFtp330Info.szDevId, str330PlantInfo.sz330DevId);
    strcpy(strFtp330Info.szDirId, str330PlantInfo.sz330DirId);
    strcpy(strFtp330Info.szKey, str330PlantInfo.sz330Key);
    
    ret = g_CEPFtp->FtpSetPlatformInfo(&strFtp330Info);	// tangzl 0725
    return ret;
}
#endif


int ftpPPLaneNameSet(JpegSendInfo *pstrJpgInfo, char cLaneId)
{
    int ret =0;
    PASSPORT_OVERALL_CONF_INFO strPPOverAllConf;
    memset(&strPPOverAllConf, 0, sizeof(strPPOverAllConf));
    
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp overall conf failed\n");
	    return -1;
    }
    PORT_VEHICLE_CONF_INFO strPPVehicleConf;
    
    ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPVehicleConf, cLaneId);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp vehicle conf failed\n");
        return -1;
    }
#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
    strcpy(pstrJpgInfo->szDirection, strPPOverAllConf.szDirection);
    strcpy(pstrJpgInfo->szLaneName, strPPVehicleConf.szOSDContent);
#endif
/* 500t���ڹ��ܳ�����Ϣ��ӵ羯�Ǳ߻�ȡ */
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    ELEC_POLICE_VEHICLE_CONF_INFO strEPVehicleConf;
    memset(&strEPVehicleConf, 0, sizeof(strEPVehicleConf));
    
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPVehicleConf, cLaneId);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp vehicle conf failed\n");
	    return -1;
    }
    strcpy(pstrJpgInfo->szDirection, strPPOverAllConf.szDirection);
    strcpy(pstrJpgInfo->szLaneName, strEPVehicleConf.szOSDContent);
#endif
    return ret;
}

int epGXcsProVerGet(void)
{
    int ret =0;
    ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
    memset(&strEPOverAllConf, 0, sizeof(strEPOverAllConf));
    
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp overall conf failed\n");
	    return -1;
    }
    else
    {
        g_usEpXcsProVer = strEPOverAllConf.usXcsProVer;
        return 0;
    }
}

int ftpEPLaneNameSet(JpegSendInfo *pstrJpgInfo, char cLaneId)
{
    int ret =0;
    if(cLaneId > MAX_VEHICLE_NUM)
    {
        SYSHANDLE_ERROR("input cLaneId:%d error\n", cLaneId);
        return -1;
    }
    ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
    memset(&strEPOverAllConf, 0, sizeof(strEPOverAllConf));
    
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp overall conf failed\n");
	    return -1;
    }
    
    ELEC_POLICE_VEHICLE_CONF_INFO strEPVehicleConf;
    memset(&strEPVehicleConf, 0, sizeof(strEPVehicleConf));
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPVehicleConf, cLaneId);
    if( 0 != ret)
    {
        SYSHANDLE_ERROR("read pp vehicle conf failed\n");
	    return -1;
    }
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    strcpy(pstrJpgInfo->szDirection, strEPOverAllConf.szDirection);
    SYSHANDLE_INFO("read ep lanName :%s\n", strEPVehicleConf.szOSDContent);
    strcpy(pstrJpgInfo->szLaneName, strEPVehicleConf.szOSDContent);
#endif
    return ret;
}


/* �������ģ���ʼ�� */

/* �ϵ��ȡ�������ϴι���״̬������һ����Ĭ��ΪOK״̬ */
int dctlVehilceStaGet(void)
{
    int ret =0;
    SYS_DEF_CONF_INFO strSysDefCfg;
    memset(&strSysDefCfg, 0, sizeof(strSysDefCfg));
    ret = SysConfGet(e_SYS_DEF_CONF, &strSysDefCfg, 0);
	if(0==ret)
	{
	    return strSysDefCfg.iVehiSta;
	}
	else
	{
	    SYSHANDLE_INFO("get default vehicle sta failed\n");
	    return -1;
	}
}

/* ��ȡ�Ƿ�ʹ��U�̹��� */
unsigned char uDiskEnFlagGet(void)
{
    int ret = 0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.ucUdiskEn;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of U disk En flag failed!\n");
	    return 0;
	}
}

/* ��ȡ�Ƿ�ʹ��SD������ */
unsigned char sdEnFlagGet(void)
{
    int ret = 0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.ucSDEn;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of sd En flag failed!\n");
	    return 0;
	}
}

/* ��ȡ���ڽ�ͨ�Ƽ�����˿ں� */
unsigned short netVehiclePortGet(void)
{
    int ret = 5004; /* Ĭ�϶˿ں� */
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.usNetVehiclePort;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of net vehicle port failed!\n");
	    return ret;
	}
}

/* �ӱ��������ļ��л�ȡ��������� */

/* ��ȡ�����gammaֵ */
float kodarGammaValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EGAMMA + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara gamma conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ��ȡ�����contrastֵ */
float kodarContrastValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EBRIGHTNESS + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara contrast conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ��ȡ���������ֵ */
float kodarGreyValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EEXPOSURE + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara contrast conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ��ȡ�����Saturationֵ */
float kodarSaturationValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ESATURATION + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara SATURATION conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}



/* ��ȡ�����sharpֵ */
float kodarSharpValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ESHARPNESS + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara sharp conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ��ȡ�����Contrastֵ */
float kodarAWBValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ECONTRAST + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara Contrast conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ��ȡ�����Saturationֵ */
float kodarIsoValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EISO + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara Iso conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ��ȡ�����Denoiseֵ */
float kodarDeNoiseValGet(char cGroup)
{
    int ret = 0;
    CAMERA_CTRL_CONF_INFO strCamInfo;

    memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EAWB + cGroup*MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara denoise conf error\n");
        return -1;
	}
    else
    {
        return strCamInfo.fFixVal;
    }
}

/* ���ó�������ǰ����״̬���Ա����豸�����Ҳ�ܱ����ϴ�״̬��1��ʾ������0��ʾ�쳣 */
int dctlVehilceStaSet(int iSta)
{
    int ret =0;
    SYS_DEF_CONF_INFO strSysDefCfg;
    strSysDefCfg.iVehiSta = iSta;
    ret = SysConfSet(e_SYS_DEF_CONF, &strSysDefCfg, 0);
	if(0==ret)
	{
        return 0;
	}
	else
	{
	    SYSHANDLE_INFO("set default vehicle sta failed\n");
	    return -1;
	}
}

/* �ϵ��ȡ�Ƿ�ʹ������ͳ�ƹ��� */
int epFlowRateFlagGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.cFlowRateFlag;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of flow rate func enable flag failed\n");
	    return ret;
	}
}

/* �ϵ��ȡ�����ϱ����ʱ�䣬��λΪ���� */
int epFlowRateInterGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.cFlowRateInter;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of flow rate inter failed\n");
	    return ret;
	}
}

/* �ϵ��ȡ�Ƿ�ʹ�ܱ���¼���� */
int epAlarmRecFlagGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.cAlarmRecFlag;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of alarm rec enable flag failed\n");
	    return -1;
	}
}

/* ��ȡ�Ƿ���xml�ļ���־ */
int ftpXmlFileSendFlagGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.cXmlFileSendFlag;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of xml file send flag failed\n");
	    return ret;
	}
}

/* �ϵ��ȡ�����ʱ���� */
int redLightDelaySecGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    ret = trafficLDelaySecSet(strSysFuncCfg.ucRedLightDelay);
	    return ret;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of red light delay secs failed\n");
	    return ret;
	}
}



/* �ϵ��ȡ��Ƶ����Ƿ���̵Ʊ�־ */
int greenLightDectFlagGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.ucVDLGreenMode;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of vdl green light dect flag failed\n");
	    return ret;
	}
}


/* ��ȡ�羯����ͼƬ�Ƿ������־ */
char epPpFalshEnFlagGet(void)
{
    int ret =0;
    ELEC_POLICE_OVERALL_CONF_INFO strEPGInfo;

	memset(&strEPGInfo, 0, sizeof(strEPGInfo));
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGInfo, 0);
    if(0 == ret)
    {
        return strEPGInfo.cPPFlashEn;
    }
    else
    {
        SYSHANDLE_INFO("get sys conf of ep pp flash flag failed\n");
	    return ret;
    }
}

/* �ϵ��ȡ�羯������Ȧ����ͼƬ�ǲ��õ�0��ͼƬ���ǵ�1��ͼƬ */
int eptPpStepNumGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.ucEpPpSnap0Or1;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of ep pp step num failed\n");
	    return ret;
	}
}

/* �ϵ��ȡ�羯�ظ���Ȧ���ڲ��õڼ�����ץ��ͼƬ */
unsigned char epPpSnapPositionGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.ucEpPpSnapPosition;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of ep pp snap position failed\n");
	    return ret;
	}
}

/* ��ȡ�羯�����Ƿ�ץ�ı�־ */
unsigned char ePReservSnapFlagGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));

    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
        return strSysFuncCfg.ucBackWordsSanpFlag;
	}
	else
	{
	    return 1;
	}
}

/******************************************************************************
 * dctlLoadPPCoil() :
 * 
 * DESCRIPTION:  ��ȡ������Ȧ������ȫ�ֱ����У��Ա���ץ��
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.24, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlLoadPPCoil()
{
	int i=0;
	int ret=-1;

    PORT_VEHICLE_CONF_INFO strPortConfInfo;
    memset(&strPortConfInfo, 0, sizeof(strPortConfInfo));
    memset(g_aPassPortChanInfo, -1, sizeof(g_aPassPortChanInfo));

    nMVehicleSumSet(1);
    for(i =0; i< MAX_VEHICLE_NUM; i++)
    {
        ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPortConfInfo, i);
    	if(0==ret)
    	{
    	    //SYSHANDLE_INFO("loadDetectInfo vehicleConnectType:0x%x\n", strPortConfInfo.usVehicleLinkMode);
    	    if(CONNECT_TYPE_COM == strPortConfInfo.usVehicleLinkMode)
            {
    	        g_aPassPortChanInfo[i].uiCarRoadId = strPortConfInfo.uiCarRoadId;
        	    g_aPassPortChanInfo[i].cComId = strPortConfInfo.strCoilDesp.strComCoilDesp.cComId;
        	    g_aPassPortChanInfo[i].cBoardId = strPortConfInfo.strCoilDesp.strComCoilDesp.cBoardId;
        	    g_aPassPortChanInfo[i].cModuleId = strPortConfInfo.strCoilDesp.strComCoilDesp.cGroupId;
        	    g_aPassPortChanInfo[i].cFrontCoilId = strPortConfInfo.strCoilDesp.strComCoilDesp.cFrontCoilId;
        	    g_aPassPortChanInfo[i].cBehindCoilId = strPortConfInfo.strCoilDesp.strComCoilDesp.cBehindCoilId;
        	}
        	else if(CONNECT_TYPE_GPIO == strPortConfInfo.usVehicleLinkMode)
            {
                g_aPassPortChanInfo[i].uiCarRoadId = strPortConfInfo.uiCarRoadId;
                g_aPassPortChanInfo[i].cFrontCoilId = strPortConfInfo.strCoilDesp.strGpioCoilDesp.cFrontCoilId;
                g_aPassPortChanInfo[i].cBehindCoilId = strPortConfInfo.strCoilDesp.strGpioCoilDesp.cBehindCoilId;
            }
            g_aPassPortChanInfo[i].usCoilDistance = strPortConfInfo.uiCoilDist;
            g_aPassPortChanInfo[i].cDecType = strPortConfInfo.usDetectType;
            g_aPassPortChanInfo[i].usSingalCoilHigh = strPortConfInfo.usSingalCoilHigh;
            
    	}
    	else
    	{
    	    SYSHANDLE_INFO("get chan:%d port conf failed\n", i);
    	    return -1;
    	}
    }
    return 0;
}

/******************************************************************************
 * dctlPPSaveSwitchInfo() :
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.18, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlPPSaveSwitchInfo(unsigned char cLane)
{
	int ret =0;
	PORT_VEHICLE_CONF_INFO strPPoliceConf;

    memset(&strPPoliceConf, 0, sizeof(strPPoliceConf));
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPoliceConf, cLane);
	if ( 0== ret )
	{
		strPPoliceConf.cCoilSwitchSta   = g_PPLaneSwitchInfo[cLane].cCoilSwitchSta;
		strPPoliceConf.cInLightSta      = g_PPLaneSwitchInfo[cLane].cInLightSta;
		SYSHANDLE_INFO("pp lane:%d coil switch coilSta:%d inLihgtSta:%d\n", cLane, strPPoliceConf.cCoilSwitchSta, strPPoliceConf.cInLightSta);
        SysConfSet(e_PASSPORT_VEHICLE_CONF, &strPPoliceConf, cLane);
	}

	return 0;
}


/******************************************************************************
 * dctlPPLoadSwitchInfo() :
 * 
 * DESCRIPTION:
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.18, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlPPLoadSwitchInfo(void)
{
    unsigned int i =0;
	int ret =0;
	PORT_VEHICLE_CONF_INFO strPPVehConf;

	for (i = 0; i<g_uiClientLaneSum; i++)
	{
	    memset(&strPPVehConf, 0, sizeof(strPPVehConf));
    	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPVehConf, i);
    	if ( 0== ret )
    	{
			g_PPLaneSwitchInfo[i].cCoilDecType      = strPPVehConf.usDetectType;
			g_PPLaneSwitchInfo[i].cCoilSwitchFlag   = strPPVehConf.cCoilSwitchFlag;
			g_PPLaneSwitchInfo[i].cCoilSwitchSta    = strPPVehConf.cCoilSwitchSta;
			g_PPLaneSwitchInfo[i].cInLightSta       = strPPVehConf.cInLightSta;
			g_PPLaneSwitchInfo[i].cInLightType      = strPPVehConf.usFlashLightType;
            g_PPLaneSwitchInfo[i].cSingalCoilPos    = strPPVehConf.cSingalCoilPos;
            g_PPLaneSwitchInfo[i].cSingalCoilSnapType = strPPVehConf.cSingalCoilSnapType;
            g_PPLaneSwitchInfo[i].cGroudCoilDecType = strPPVehConf.cGroudCoilDecType;
            SYSHANDLE_INFO("pp lane id:%d;\n \
                            coil dec type:%d;\n \
                            coil switch flag:%d;\n \
                            coil switch sta:%d;\n \
                            inlight sta:%d; \n \
                            inlight type:%d;\n \
                            singal coil pos:%d;\n \
                            singal coil snap type:%d;\n \
                            groud coil dec type:%d;\n ", \
                            i,\
                            g_PPLaneSwitchInfo[i].cCoilDecType,\
                            g_PPLaneSwitchInfo[i].cCoilSwitchFlag,\
                            g_PPLaneSwitchInfo[i].cCoilSwitchSta,\
							g_PPLaneSwitchInfo[i].cInLightSta,\
							g_PPLaneSwitchInfo[i].cInLightType,\
							g_PPLaneSwitchInfo[i].cSingalCoilPos,\
							g_PPLaneSwitchInfo[i].cSingalCoilSnapType,\
							g_PPLaneSwitchInfo[i].cGroudCoilDecType);

			RADAR_DETECT_PARAM rdp;
			rdp.chan_id = 0;
			rdp.lane_id = i;
			rdp.detect_enable = strPPVehConf.cGroudCoilDecType == CS_RADAR_SPEED;
			ret = CALL_CYC_CMD(NET_DEV_SET_RADAR_DETECT_ENABLE, rdp);
		}
	}
	return 0;
}

int ppGetUsedLaneSum(void)
{
    int ret =-1;
    PASSPORT_OVERALL_CONF_INFO strPPGConfInfo;

	memset(&strPPGConfInfo, 0, sizeof(strPPGConfInfo));
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPGConfInfo, 0);
    if(0 == ret)
    {
	    g_uiClientLaneSum = strPPGConfInfo.cValidLanes;
    }
    return ret;
}

int dCtlLoadEPCoil()
{
	unsigned int i =0;
	int ret =-1;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGConfInfo;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPConfInfo;
	
	memset(&strEPGConfInfo, 0, sizeof(strEPGConfInfo));
	memset(&strEPConfInfo, 0, sizeof(strEPConfInfo));
    memset(g_aEPConfInfo, -1, sizeof(g_aEPConfInfo));
    
	for ( i = 0; i<g_uiClientLaneSum; i++)
	{
        ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPConfInfo, i);
        if(0 == ret)
        {
            if(CONNECT_TYPE_COM == strEPConfInfo.usVehicleLinkMode)
            {
                g_aEPConfInfo[i].uiCarRoadId = strEPConfInfo.uiCarRoadId;
                g_aEPConfInfo[i].cComId = strEPConfInfo.strCoilDesp.strComCoilDesp.cComId;
                g_aEPConfInfo[i].cBoardId = strEPConfInfo.strCoilDesp.strComCoilDesp.cBoardId;
                g_aEPConfInfo[i].cModuleId = strEPConfInfo.strCoilDesp.strComCoilDesp.cGroupId;
                g_aEPConfInfo[i].cFrontCoilId = strEPConfInfo.strCoilDesp.strComCoilDesp.cFrontCoilId;
               
                g_aEPConfInfo[i].cBehindCoilId = strEPConfInfo.strCoilDesp.strComCoilDesp.cBehindCoilId;
            }
            else if(CONNECT_TYPE_GPIO == strEPConfInfo.usVehicleLinkMode)
            {
                g_aEPConfInfo[i].cComId = 0xff;
                g_aEPConfInfo[i].uiCarRoadId = strEPConfInfo.uiCarRoadId;
                g_aEPConfInfo[i].cBoardId = strEPConfInfo.strCoilDesp.strGpioCoilDesp.cBoardId;
                g_aEPConfInfo[i].cModuleId = 0xff;
                g_aEPConfInfo[i].cFrontCoilId = strEPConfInfo.strCoilDesp.strGpioCoilDesp.cFrontCoilId;
                g_aEPConfInfo[i].cBehindCoilId = strEPConfInfo.strCoilDesp.strGpioCoilDesp.cBehindCoilId;
            }
            g_aEPConfInfo[i].usCoilDistance = strEPConfInfo.usCoilDist;
        }
	}
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGConfInfo, 0);
    if(0 == ret)
    {
        for ( i = 0; i<g_uiClientLaneSum; i++)
	    {
	        g_aEPConfInfo[i].cDecType = strEPGConfInfo.cDetectType;
	        g_aEPConfInfo[i].iTotalLane = strEPGConfInfo.cValidChanSum;
	    }
	    nMVehicleSumSet(strEPGConfInfo.cValidChanSum);
    }
	return 0;
}

int epGetUsedLaneSum(void)
{
    int ret =-1;
    ELEC_POLICE_OVERALL_CONF_INFO strEPGConfInfo;

	memset(&strEPGConfInfo, 0, sizeof(strEPGConfInfo));
    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGConfInfo, 0);
    if(0 == ret)
    {
	    g_uiClientLaneSum = strEPGConfInfo.cValidChanSum;
    }
    return ret;
}


/******************************************************************************
 * dctlEPLoadRedInfo() :
 * 
 * DESCRIPTION:  �����ⲿIO����������״̬�ĳ�ʼֵ�����������ر�ʱΪ0�����Զ���0Ϊ���
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.18, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlEPLoadRedInfo(char cMode, unsigned int uiLane)
{
    unsigned int i =0;
    unsigned int j =0;
	int ret =0;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;

	for (i = 0; i<g_uiClientLaneSum; i++)
	{
	    if(SINGAL_LANE_SET_MODE == cMode)
	    {
	        i = uiLane;
	    }
	    
	    memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
    	ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, i);
    	if ( 0== ret )
    	{
			g_bForceRed[i] = strEPoliceConf.uiForcedRedLight;

            g_EPLaneSwitchInfo[i].cRoadDir = strEPoliceConf.cLaneDir;

			g_EPLaneSwitchInfo[i].cSwitchLaneEn = strEPoliceConf.cSwitchLaneEn;
			g_EPLaneSwitchInfo[i].cSwitchLaneLeftNo = strEPoliceConf.cSwitchLaneLeftNo;
			g_EPLaneSwitchInfo[i].cSwitchLaneStraightNo = strEPoliceConf.cSwitchLaneStraightNo;
			g_EPLaneSwitchInfo[i].cSwitchLaneStraightNoSta = 0;
			g_EPLaneSwitchInfo[i].cSwitchLaneLeftNoSta = 0;
            
			for(j =0; j< MAX_LIGHT_TYPE; j++)
			{
			    g_EPLaneSwitchInfo[i].strTrafficL[j].cEnable = strEPoliceConf.strTrafficLight[j].cEnable;
			    g_EPLaneSwitchInfo[i].strTrafficL[j].cLightType = strEPoliceConf.strTrafficLight[j].cLightType;
			    g_EPLaneSwitchInfo[i].strTrafficL[j].cVehicleNum = strEPoliceConf.strTrafficLight[j].cVehicleNum -1;
			}
            /*Start of ITS on 2011-4-6 15:38 2.0.0.1*/
			g_EPLaneSwitchInfo[i].cCoilSwitchSta = strEPoliceConf.cCoilSwitchSta;
			g_EPLaneSwitchInfo[i].cCoilDecType = strEPoliceConf.cDetectType;
			g_EPLaneSwitchInfo[i].cCoilSwitchFlag = strEPoliceConf.cCoilSwitchFlag;
			g_EPLaneSwitchInfo[i].cInLightSta = strEPoliceConf.cInLightSta;
			g_EPLaneSwitchInfo[i].cInLightType = strEPoliceConf.usFlashLightType;
			g_EPLaneSwitchInfo[i].cVLDecEn = strEPoliceConf.cVLDecEn;
            
			SYSHANDLE_INFO("%s red light info:\n \
                            lane id:%d,red sta:%d;\n \
                            type0 sta:%d,enable:%d;\n \
                            type1 Sta:%d,enable:%d;\n \
                            type2 Sta:%d,enable:%d;\n \
                            type3 Sta:%d,enable:%d;\n\
                            dsp red light sta:%d;\n",\
                            __FUNCTION__, i, g_EPLaneSwitchInfo[i].cRedLSta,\
                            g_EPLaneSwitchInfo[i].strTrafficL[0].cLSta,\
                            g_EPLaneSwitchInfo[i].strTrafficL[0].cEnable,\
                            g_EPLaneSwitchInfo[i].strTrafficL[1].cLSta,\
                            g_EPLaneSwitchInfo[i].strTrafficL[1].cEnable,\
                            g_EPLaneSwitchInfo[i].strTrafficL[2].cLSta,\
                            g_EPLaneSwitchInfo[i].strTrafficL[2].cEnable,\
                            g_EPLaneSwitchInfo[i].strTrafficL[3].cLSta,\
                            g_EPLaneSwitchInfo[i].strTrafficL[3].cEnable);

            SYSHANDLE_INFO("lane id:%d;\n \
                            coil dec type:%d;\n \
                            coil switch flag:%d;\n \
                            coil switch sta:%d;\n \
                            inlight sta:%d; \n \
                            inlight type:%d;\n \
                            vlEnable flag:%d;\n \
                            road dir:%d;\n", \
                            i,\
                            g_EPLaneSwitchInfo[i].cCoilDecType,\
                            g_EPLaneSwitchInfo[i].cCoilSwitchFlag,\
                            g_EPLaneSwitchInfo[i].cCoilSwitchSta,\
                            g_EPLaneSwitchInfo[i].cInLightSta,\
                            g_EPLaneSwitchInfo[i].cInLightType,\
                            g_EPLaneSwitchInfo[i].cVLDecEn,\
                            g_EPLaneSwitchInfo[i].cRoadDir);
            /*End of ITS on 2011-4-6 15:38 2.0.0.1*/
            if(OTHER_SET_MODE != cMode)
            {
                redLightChangeInformDsp(i, cMode);
            }
            
            if(SINGAL_LANE_SET_MODE == cMode)
            {
                break;
            }
		}
	}
	return 0;
}

/******************************************************************************
 * dctlEPSaveSwitchInfo() :
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.18, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlEPSaveSwitchInfo(unsigned char cLane)
{
	int ret =0;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;

    memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
	ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, cLane);
	if ( 0== ret )
	{
        /*Start of ITS on 2011-4-6 15:38 2.0.0.1*/
		strEPoliceConf.cCoilSwitchSta   = g_EPLaneSwitchInfo[cLane].cCoilSwitchSta;
		strEPoliceConf.cInLightSta      = g_EPLaneSwitchInfo[cLane].cInLightSta;
		SYSHANDLE_INFO("lane:%d coil switch coilSta:%d inLihgtSta:%d\n", cLane, strEPoliceConf.cCoilSwitchSta, strEPoliceConf.cInLightSta);
        /*End of ITS on 2011-4-6 15:38 2.0.0.1*/
        SysConfSet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, cLane);
	}

	return 0;
}

/******************************************************************************
 * dctlRedLightReport() :
 * 
 * DESCRIPTION:  ��ͨ�Ƽ������ͨ��״̬�ϱ�,�����ڲ��ӱ����豸ʱ���ϱ�������ΪȫF
 * �������б�������ʱ����λ��Ӧ��״̬��Ϊ�㣬����Ϊ��ʱΪ��ƣ���������������һ��
 * �����������Թ涨Ϊ1ʱ��ʾ���
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.24, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlRedLightReport(int iChannel, int iSta)
{
    int ret =0;
    PORT_VEHICLE_CONF_INFO strPPortConf;
    static int s_iReportTimes =0;
    static int s_iSnapTimes =0;

    memset(&strPPortConf, 0, sizeof(strPPortConf));
    ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, 0);
    
    s_iReportTimes++;
    ret = dspPPCoilTypeGet(0, 0);
    if(( GROUD_LOOP == ret ) && ( iSta == (g_aPassPortChanInfo[0].cModuleId -1) )\
    ||(g_aPassPortChanInfo[0].cModuleId == 3))
    {
        s_iSnapTimes++;
        if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[0] )
        {
            if( strPPortConf.ucSnapPicNum <= 1)
            {
                dctlPPTTLNormalSnap(0, 1, SNAP_TYPE_SINGAL_COIL);
            }
            else
            {
                dctlPPTTLNormalSnap(0, 1, SNAP_TYPE_SINGAL_COIL);
                usleep(strPPortConf.usSnapPicInter*1000);
                dctlPPTTLNormalSnap(0, 1, SNAP_TYPE_SINGAL_COIL);
            }
        }
        else
        {
            if( strPPortConf.ucSnapPicNum <= 1)
            {
                dctlPPTTLForceSnap(0, 1, SNAP_TYPE_SINGAL_COIL);
            }
            else
            {
                dctlPPTTLForceSnap(0, 1, SNAP_TYPE_SINGAL_COIL);
                usleep(strPPortConf.usSnapPicInter*1000);
                dctlPPTTLForceSnap(0, 1, SNAP_TYPE_SINGAL_COIL);
            }
        }
    }
    SYSHANDLE_INFO("ttl snap lane:0x%x sta:0x%x, report times:%d,snap times:%d,delaySnap ms:%d,coil group Id:%d\n", \
        iChannel, iSta, s_iReportTimes, s_iSnapTimes, strPPortConf.usSnapPicInter, g_aPassPortChanInfo[0].cModuleId);
	return 0;
}

void hagdDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    static char s_cCarDir =0;
    SYSHANDLE_INFO("hgd channel:%d :\n", iNum);
    if (pstVehicle->strDevProtocolSet.strHagdProtoCol.cCommuId == HAGDA_REAL_STA_REPORT)
    {
    #if 0
        //���Ӿ���ץ�Ŀ���
        if ( HAGDA_OCCUPY_STA_REPORT == pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cModuleType )
        {
            hgdEPSnapCtl(pstVehicle->iComNum, \
            pstVehicle->strDevProtocolSet.strHagdProtoCol.cBoardId,\
            pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cModuleId,\
            pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cLineCycleSta);		
        }
        else
    #endif
        SYSHANDLE_INFO("hgd first rcv,comNum:%d,devType:%d,boardId:%d,commuId:%d,moduleId:%d, moduleType:%d\n",\
        pstVehicle->iComNum,\
        pstVehicle->iDevType, \
        pstVehicle->strDevProtocolSet.strHagdProtoCol.cBoardId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.cCommuId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cModuleId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cModuleType
        );
        //���ڲ���ʱ��һ��ͼƬ����
        {
            if( pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cCarDir == 1 )
            {
                s_cCarDir = CAR_RUN_CORRECT_DIR;
            }
            else if ( pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cCarDir == 2 )
            {
                s_cCarDir = CAR_RUN_REVERSE_DIR;
            }
            else
            {   
                SYSHANDLE_ERROR("unknow car dir,val:%d\n", pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cCarDir);
                s_cCarDir = CAR_RUN_CORRECT_DIR;
            }
            hgdPassPortSnapBegin(pstVehicle->iComNum,\
            pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealSta.cModuleId,\
            s_cCarDir);
        }
    }
    // ���ڲ���ʱ�ڶ���ͼƬ����
    else if (pstVehicle->strDevProtocolSet.strHagdProtoCol.cCommuId == HAGDA_REAL_DATA_REPORT)
    {
        SYSHANDLE_INFO("hgd sec rcv,comNum:%d,devType:%d,boardId:%d,commuId:%d, moduleId:%d, speed:%d,dir:%d\n",\
        pstVehicle->iComNum,\
        pstVehicle->iDevType, \
        pstVehicle->strDevProtocolSet.strHagdProtoCol.cBoardId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.cCommuId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealData.realDataSpeedStr.cSpeedModeId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealData.realDataSpeedStr.cCarSpeed,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealData.realDataSpeedStr.cCarChannelDirSta
        );
        hgdPassPortSnapEnd(pstVehicle->iComNum,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealData.realDataSpeedStr.cSpeedModeId,\
        pstVehicle->strDevProtocolSet.strHagdProtoCol.strProtoData.strHagdRealData.realDataSpeedStr.cCarSpeed,\
        s_cCarDir);
    }
    else
    {
        SYSHANDLE_ERROR("Un dealed Commtype %x \n", pstVehicle->strDevProtocolSet.strHagdProtoCol.cCommuId);
    }
}

int vehicleNMStaProc(void)
{
    g_iErrorTimes--;
    if(g_iErrorTimes < 0)
    {
        g_iErrorTimes = 0;
    }
    if( g_iErrorTimes == 200 )
    {
        nMMsgSend(NM_COIL_LOOP_STA, 0, 0, g_aiCoilSta);
        nMMsgSend(NM_SIGNAL_LAMP_STA, 0, 0, g_aiLampSta);
        nMMsgSend(NM_VIR_LOOP_STA, 0, 0, g_aiVirtualSta);
    }
    return 0;
}


int  sjPPCoilStaSwitch(unsigned char *pucSta, unsigned char ucMonitroSta)
{
    unsigned int iLaneId =0;
    char cSta =0;
    unsigned char cTmpLane =0;

    unsigned char ucCoilSta[4] ={0};

    /* ��ʱͳ�Ƴ�����������Ϣ */
    if(g_iErrorTimes < 202)
    {
        g_iErrorTimes =202;
    }
    g_iErrorTimes++;

    /* ��Ϊ������Ȧ����Ҫ�����أ���Ȼ�����ָ����� */
    if( BREAKDOWN_STA_STOP == ucMonitroSta )/* ������û�ӻ�����ֹͣ */
    {
        SYSHANDLE_INFO("error:pp sj vehilce stop!\n");
        for(iLaneId=0; iLaneId < g_uiClientLaneSum; iLaneId++)
        {
            g_aiLampSta[iLaneId] = MODE_ERROR_STA;
            ppLaneSwitch(iLaneId, VEHICLE_BUG_ERROR);
        }
        return 0;
    }
    else if( BREAKDOWN_STA_WORK == ucMonitroSta )/* �������ϵ紦�ڹ���״̬ */
    {
        SYSHANDLE_INFO("ok:pp kj vehilce working!\n");
        return 0;
        //syslog(LOG_ERR|LOG_USER, "kj vehilce coil except,num:%d\n", ucDevNum);
    }


    g_iVehicleHeartSum++;

    memcpy(ucCoilSta, pucSta, sizeof(ucCoilSta));

    for( cTmpLane =0; cTmpLane < 2; cTmpLane++)
    {
        if(ucCoilSta[cTmpLane*2] == ucCoilSta[cTmpLane*2 +1])
        {
            /* Ϊ0��ʾ����������,1Ϊ�쳣 */
            if(ucCoilSta[cTmpLane*2] == 0)
            {
                cSta = BREAKDOWN_STA_OK;
            }
            else
            {
                cSta = BREAKDOWN_STA_FAILED;
            }
        }
        else
        {
            cSta = BREAKDOWN_STA_FAILED;
        }
        
        if(BREAKDOWN_STA_OK == cSta)
        {
            /* �������ָ������������л����ظ���Ȧģʽ */
            if(cTmpLane == (g_aPassPortChanInfo[cTmpLane].cModuleId -1))
            {
                ppLaneSwitch(cTmpLane, VEHICLE_BUG_OK);
                SYSHANDLE_INFO("sj vehilce switch bad to ok,lane:%d,sta:%d\n", cTmpLane, cSta);
            }
        }
        else /* ���������� */
        {
            /* ����ȣ�����Ҫ����Ӧ����������Ȧ�����л� */
            if(cTmpLane == (g_aPassPortChanInfo[cTmpLane].cModuleId -1))
            {
                ppLaneSwitch(cTmpLane, VEHICLE_BUG_ERROR);
                SYSHANDLE_INFO("sj vehilce coil switch ok to bad,lane:%d,sta:%d\n", cTmpLane, cSta);
                //syslog(LOG_ERR|LOG_USER, "kj vehilce coil except,num:%d\n", ucDevNum);
            }
        }
    }
    return 0;
}


/*
 * Function:       
 * Description:    ���ڳ����л�
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          ����������������״̬:1����ʾ�쳣��0����ʾ����
 * Output:         
 * Return:         
 * Others:         
 */
int ppLaneSwitch(char cLane, char cSta)
{
    unsigned char j = cLane;
    
    /* �жϸó����Ƿ�ʹ���л�����������Ϊ�ظ���Ȧģʽ */
    if(( g_PPLaneSwitchInfo[j].cCoilSwitchFlag > 0 ) \
        && (g_PPLaneSwitchInfo[j].cCoilDecType == GROUD_LOOP))
    {
        if(cSta == VEHICLE_BUG_ERROR)
        {
            if( g_PPLaneSwitchInfo[j].cCoilSwitchSta != VIRTUAL_LOOP )
            {
                /* ��Ҫ�л���������Ȧץ��ģʽ */
                g_PPLaneSwitchInfo[j].cCoilSwitchSta = VIRTUAL_LOOP;
				VEHICLE_DETECT_TYPE vdt;
				memset(&vdt, 0, sizeof(vdt));
				vdt.chan_id = 0;
				vdt.lane_id = j;
				vdt.detect_type = e_VIRTUAL_LOOP;

                //sem_wait(&sem_dsp);
                CALL_CYC_CMD(NET_DEV_SET_VEHICLE_DETECT_TYPE, vdt);
                //sem_post(&sem_dsp);
                
                dspPPVLParamSet(0, j);
                
                SYSHANDLE_INFO("vehicle error,lane:%d coil switch coilSta:%d.\n", j, g_PPLaneSwitchInfo[j].cCoilSwitchSta);

                g_aiCoilSta[j] = MODE_ERROR_STA;
                g_aiVirtualSta[j] = MODE_OK_STA;
            }
        }
        else
        {
            if(  g_PPLaneSwitchInfo[j].cCoilSwitchSta != GROUD_LOOP )
            {
                /* ��Ҫ�л����ظ���Ȧץ��ģʽ */
                g_PPLaneSwitchInfo[j].cCoilSwitchSta = GROUD_LOOP;
				VEHICLE_DETECT_TYPE vdt;
				memset(&vdt, 0, sizeof(vdt));
				vdt.chan_id = 0;
				vdt.lane_id = j;
				vdt.detect_type = e_INNERVATION_LOOP;

                //sem_wait(&sem_dsp);
                CALL_CYC_CMD(NET_DEV_SET_VEHICLE_DETECT_TYPE, vdt);
                //sem_post(&sem_dsp);
                
                dspPPVLParamSet(0, j);                

                SYSHANDLE_INFO("vehicle ok_2,lane:%d coil switch coilSta:%d.\n", j, g_PPLaneSwitchInfo[j].cCoilSwitchSta);

                g_aiCoilSta[j] = MODE_OK_STA;
                g_aiVirtualSta[j] = MODE_SHUTDOWN_STA;
            }
            else
            {
                g_aiCoilSta[j] = MODE_OK_STA;
                g_aiVirtualSta[j] = MODE_SHUTDOWN_STA;
            }
        }
        /* �����л�״̬ */
        dctlPPSaveSwitchInfo(cLane);
    }
    else if(( g_PPLaneSwitchInfo[j].cCoilSwitchFlag == 0 ) \
        && (g_PPLaneSwitchInfo[j].cCoilDecType == GROUD_LOOP))
    {
        g_aiCoilSta[j] = MODE_OK_STA;
        g_aiVirtualSta[j] = MODE_SHUTDOWN_STA;
    }
    else if(( g_PPLaneSwitchInfo[j].cCoilSwitchFlag == 0 ) \
        && (g_PPLaneSwitchInfo[j].cCoilDecType == VIRTUAL_LOOP))
    {
        g_aiCoilSta[j] = MODE_SHUTDOWN_STA;
        g_aiVirtualSta[j] = MODE_OK_STA;
    }
    else
    {
        SYSHANDLE_DEBUG("pp do not need switch,lane:%d,switchFlag:%d,coilType:%d!\n",\
        j, g_PPLaneSwitchInfo[j].cCoilSwitchFlag, g_PPLaneSwitchInfo[j].cCoilDecType);
    }
    return 0;
}

int  kjPPCoilStaSwitch(unsigned char ucVehicleSta, unsigned char ucLaneSta)
{
    unsigned int j =0;
    unsigned int iLaneId =0;
    char cTmpLane =0;
    char ucTmpLaneSta =0;

    /* ��ʱͳ�Ƴ�����������Ϣ */
    if(g_iErrorTimes < 202)
    {
        g_iErrorTimes =202;
    }
    g_iErrorTimes++;

    if( BREAKDOWN_ALL_STA == ucVehicleSta )/* ���������� */
    {
        for(cTmpLane =0; cTmpLane< (MAX_VEHICLE_NUM -1); cTmpLane++)
        {
            ucTmpLaneSta = (ucLaneSta >> cTmpLane) & 0x01;
            /* ���Ҷ�Ӧʵ�ʳ��� */
            for(j =0; j< g_uiClientLaneSum; j++)
            {
                /* ����ȣ�����Ҫ����Ӧ����������Ȧ�����л� */
                if(cTmpLane == (g_aPassPortChanInfo[j].cModuleId -1))
                {
                    SYSHANDLE_INFO("kj vehilce coil except,vehicle num:%d, lane:%d\n", cTmpLane, j);
                    /* ע��:��Ȧ״̬ͨ�������ϱ�����Ҫ�����Ƿ񰴳�����ʵ�ʳ����ϱ����� */
                    if(ucTmpLaneSta == 0x01)
                    {
                        ppLaneSwitch(j, VEHICLE_BUG_ERROR);
                    }
                    else
                    {
                        ppLaneSwitch(j, VEHICLE_BUG_OK);
                    }
                }
                else
                {
                    SYSHANDLE_INFO("not find kj vehilce coil except,vehicle num:%d, lane:%d\n", cTmpLane, j);
                }
            }
        }
    }
    else if( BREAKDOWN_STA_STOP == ucVehicleSta )/* ������û�ӻ�����ֹͣ */
    {
        SYSHANDLE_INFO("error:pp kj vehilce stop!\n");
        for(iLaneId=0; iLaneId < g_uiClientLaneSum; iLaneId++)
        {
            g_aiLampSta[iLaneId] = MODE_ERROR_STA;
            ppLaneSwitch(iLaneId, VEHICLE_BUG_ERROR);
        }
    }
    else if( BREAKDOWN_STA_WORK == ucVehicleSta )/* �������ϵ紦�ڹ���״̬ */
    {
        SYSHANDLE_INFO("ok:pp kj vehilce working!\n");
        //syslog(LOG_ERR|LOG_USER, "kj vehilce coil except,num:%d\n", ucDevNum);
    }
    return 0;
}


/*
 * Function:       
 * Description:    �羯�����л�
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          ����������������״̬:1����ʾ�쳣��0����ʾ����
 * Output:         
 * Return:         
 * Others:         
 */
int epLaneSwitch(char cLane, char cSta)
{
    unsigned char j = cLane;
    
    /* �жϸó����Ƿ�ʹ���л�����������Ϊ�ظ���Ȧģʽ */
    if(( g_EPLaneSwitchInfo[j].cCoilSwitchFlag > 0 ) \
        && (g_EPLaneSwitchInfo[j].cCoilDecType == GROUD_LOOP))
    {
        if(cSta == VEHICLE_BUG_ERROR)
        {
            if( g_EPLaneSwitchInfo[j].cCoilSwitchSta != VIRTUAL_LOOP )
            {
                /* �л���������Ȧץ��ģʽ */
                g_EPLaneSwitchInfo[j].cCoilSwitchSta = VIRTUAL_LOOP;
				EP_VEHICLE_DETECT_TYPE evdt;
				memset(&evdt, 0, sizeof(evdt));
				evdt.lane_id = j;
				evdt.detect_type = e_VIRTUAL_LOOP;
				evdt.bayonetSnap = epPPPicSendFlag();
                //sem_wait(&sem_dsp);
				CALL_CYC_CMD(NET_DEV_SET_EP_VI_DETECT_TYPE, evdt);
                //sem_post(&sem_dsp);
                
                SYSHANDLE_INFO("vehicle error,lane:%d coil switch coilSta:%d inLihgtSta:%d\n", j, \
                g_EPLaneSwitchInfo[j].cCoilSwitchSta, g_EPLaneSwitchInfo[j].cInLightSta);

                g_aiCoilSta[j] = MODE_ERROR_STA;
                g_aiVirtualSta[j] = MODE_OK_STA;

            }
        }
        else
        {
            if(  g_EPLaneSwitchInfo[j].cCoilSwitchSta != GROUD_LOOP )
            {
                /* �л����ظ���Ȧץ��ģʽ */
				g_EPLaneSwitchInfo[j].cCoilSwitchSta = GROUD_LOOP;

				EP_VEHICLE_DETECT_TYPE evdt;
				memset(&evdt, 0, sizeof(evdt));
				evdt.lane_id = j;
				evdt.detect_type = e_INNERVATION_LOOP;
				evdt.bayonetSnap = epPPPicSendFlag();
                //sem_wait(&sem_dsp);
				CALL_CYC_CMD(NET_DEV_SET_EP_VI_DETECT_TYPE, evdt);
                //sem_post(&sem_dsp);
                
                SYSHANDLE_INFO("vehicle ok_2,lane:%d coil switch coilSta:%d inLihgtSta:%d,inLightType:%d\n", j, \
                g_EPLaneSwitchInfo[j].cCoilSwitchSta, g_EPLaneSwitchInfo[j].cInLightSta, g_EPLaneSwitchInfo[j].cInLightType);

                g_aiCoilSta[j] = MODE_OK_STA;
                g_aiVirtualSta[j] = MODE_SHUTDOWN_STA;
            }
            else
            {
                g_aiCoilSta[j] = MODE_OK_STA;
                g_aiVirtualSta[j] = MODE_SHUTDOWN_STA;
            }
        }
        /* �����л�״̬ */
        dctlEPSaveSwitchInfo(cLane);
    }
    else if(( g_EPLaneSwitchInfo[j].cCoilSwitchFlag == 0 ) \
        && (g_EPLaneSwitchInfo[j].cCoilDecType == GROUD_LOOP))
    {
        g_aiCoilSta[j] = MODE_OK_STA;
        g_aiVirtualSta[j] = MODE_SHUTDOWN_STA;
    }
    else if(( g_EPLaneSwitchInfo[j].cCoilSwitchFlag == 0 ) \
        && (g_EPLaneSwitchInfo[j].cCoilDecType == VIRTUAL_LOOP))
    {
        g_aiCoilSta[j] = MODE_SHUTDOWN_STA;
        g_aiVirtualSta[j] = MODE_OK_STA;
    }
    else
    {
        SYSHANDLE_DEBUG("do not need switch,lane:%d,switchFlag:%d,coilType:%d!\n",\
        j, g_EPLaneSwitchInfo[j].cCoilSwitchFlag, g_EPLaneSwitchInfo[j].cCoilDecType);
    }
    return 0;
}



int  kjEPCoilStaSwitch(unsigned char ucVehicleSta, unsigned char ucLaneSta, unsigned char ucRLSta)
{
    int i =0;
    unsigned int j =0;
    unsigned int iLanId =0;
    char cTmpLane =0;
    unsigned char ucTmpLaneSta =0;
    unsigned char ucTmpRLSta =0;

    if( BREAKDOWN_ALL_STA == ucVehicleSta )/* �������ϵ磬���й��� */
    {
        /* �����жϳ�����Ȧ���� */
        for(cTmpLane =0; cTmpLane< (MAX_VEHICLE_NUM -1); cTmpLane++)
        {
            ucTmpLaneSta = (ucLaneSta >> cTmpLane) & 0x01;
            /* ���Ҷ�Ӧʵ�ʳ��� */
            for(j =0; j< g_uiClientLaneSum; j++)
            {
                if( cTmpLane == (g_aEPConfInfo[j].cModuleId -1) )
                {
                    SYSHANDLE_INFO("kj vehilce coil except,vehicle num:%d, lane:%d\n", cTmpLane, j);
                    /* ע��:��Ȧ״̬ͨ�������ϱ�����Ҫ�����Ƿ񰴳�����ʵ�ʳ����ϱ����� */
                    if(ucTmpLaneSta == 0x01)
                    {
                        epLaneSwitch(j, VEHICLE_BUG_ERROR);
                    }
                    else
                    {
                        epLaneSwitch(j, VEHICLE_BUG_OK);
                    }
                }
                else
                {
                    SYSHANDLE_INFO("not find kj vehilce coil except,vehicle num:%d, lane:%d\n", cTmpLane, j);
                }
            }
        }
        /* �����жϺ�ƹ��ϣ�ֻ�ϱ����ܣ������л� */
        for(cTmpLane =0; cTmpLane < (MAX_VEHICLE_NUM -1); cTmpLane++)
        {
            ucTmpRLSta = (ucRLSta >> cTmpLane) & 0x01;
            for(iLanId=0; iLanId < g_uiClientLaneSum; iLanId++)
            {
                for(i =0; i< MAX_LIGHT_TYPE; i++)
                {
                    if(g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cVehicleNum == cTmpLane)
                    {
                        if(ucTmpRLSta == 0x01)
                        {
                            /* ��ƹ���ͨ�������ϱ� */
                            g_aiLampSta[iLanId] = MODE_ERROR_STA;
                        }
                        else
                        {
                            g_aiLampSta[iLanId] = MODE_OK_STA;
                        }
                     }
                }
            }
            //syslog(LOG_ERR|LOG_USER, "kj vehilce red light is except,num:%d\n", ((ucDevNum - 0x10)+1) );
        }
    }
    else if( BREAKDOWN_STA_STOP == ucVehicleSta )/* ������û�ӻ�����ֹͣ */
    {
        /* �Ѻ�Ƽ�ⷽʽ�л�����Ƶ���ģʽ */
        dspEPRedLightAreaSet(RL_DYNA_SET_MODE, RED_DETECT_VIDEO);

        SYSHANDLE_INFO("error:kj vehilce stop!\n");
        for(iLanId=0; iLanId < g_uiClientLaneSum; iLanId++)
        {
            g_aiLampSta[iLanId] =MODE_ERROR_STA;
            epLaneSwitch(iLanId, VEHICLE_BUG_ERROR);
        }
        syslog(LOG_ERR|LOG_USER, "kj vehilce stop work.\n");
    }
    else if( BREAKDOWN_STA_WORK == ucVehicleSta )/* �������ϵ紦�ڹ���״̬ */
    {
        /* �Ѻ�Ƽ�ⷽʽ�л�������ָ�����ģʽ */
        dspEPRedLightAreaSet(RL_NORMAL_SET_MODE, 0);
        SYSHANDLE_INFO("ok:kj vehilce restart working!\n");
        syslog(LOG_ERR|LOG_USER, "kj vehilce restart work.\n");
    }
    
    /* ��ʱͳ�Ƴ�����������Ϣ */
    g_iErrorTimes =201;
    vehicleNMStaProc();
    return 0;
}

/*
 * Function:       
 * Description:    ��ǿ�ƺ�����ü������������û��߳�����ƺŷ����仯�ˣ���Ҫ��ʱ֪ͨ��dsp
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
int redLightChangeInformDsp(unsigned int iLanId, char cMode)
{
    int i =0;
    int ret =0;
    int iDirSum =0;
    
    lane_nd_red_light_flag strDspRedLightInfoSet;

    memset(&strDspRedLightInfoSet, 0, sizeof(strDspRedLightInfoSet));

    strDspRedLightInfoSet.uiLane = iLanId;
    /* ����dsp��ͨ��״̬�µĳ�����ʻ���� */
    for(iDirSum =0; iDirSum<MAX_LANE_DIR_TYPE; iDirSum++)
    {
        if( g_EPLaneSwitchInfo[iLanId].cRoadDir & (0x01 << iDirSum) )
        {
            strDspRedLightInfoSet.strNDRedLight[iDirSum].ucNormalDirection = (0x01 << iDirSum);
        }
    }

    for(i =0; i< MAX_LIGHT_TYPE; i++)
	{
	    if( 0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable )
	    {
	        if(SINGAL_LANE_SET_MODE == cMode)
	        {
    	        /* ��ǿ�ƺ��Ϊ�棬��ǿ�����ø÷���Ľ�ͨ��״̬Ϊ��� */
    	        if( RED_LIGHT_ON == g_bForceRed[iLanId] )
                {
                    /* ǿ���޸�ȫ�ֺ��״̬���Ա����´����ü�web�����ѯ��ʾ */
                    g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta = RED_LIGHT_ON;
                    g_EPLaneSwitchInfo[iLanId].cRedLSta = RED_LIGHT_ON;
                }
                else
                {
                    /* ǿ���޸�ȫ�ֺ��״̬���Ա����´����ü�web�����ѯ��ʾ */
                    g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta = RED_LIGHT_OFF;
                    g_EPLaneSwitchInfo[iLanId].cRedLSta = RED_LIGHT_OFF;
                }
            }
            /* ����dsp��ͨ��״̬ */
	        strDspRedLightInfoSet.strNDRedLight[i].ucRedLightFlag \
	        = g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta;
        }
        else
        {
            /* ����dsp��ͨ��״̬ */
            strDspRedLightInfoSet.strNDRedLight[i].ucRedLightFlag = RED_LIGHT_OFF;
        }
	}

    /* ��dsp���͵�ǰ������ͨ�Ƽ�������ʻ����״̬ */
	//sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_EPVL_LANE_ND_RED_LIGHT, strDspRedLightInfoSet);
	//sem_post(&sem_dsp);

    if(ret != 0)
    {
        SYSHANDLE_ERROR("call NET_DEV_SET_EPVL_LANE_ND_RED_LIGHT failed, error=%d\n", NET_DVE_GetLastError());
        return -1;
    }
    else
    {
        SYSHANDLE_INFO("force set lane:%d nd red light ok,(%d,%d,%d),(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)\n",\
        strDspRedLightInfoSet.uiLane,\
        strDspRedLightInfoSet.strNDRedLight[0].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[0].ucRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[0].ucAidRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[1].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[1].ucRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[1].ucAidRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[2].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[2].ucRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[2].ucAidRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[3].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[3].ucRedLightFlag,\
        strDspRedLightInfoSet.strNDRedLight[3].ucAidRedLightFlag);
    }
    return 0;
}

unsigned char g_aucRLSta[MAX_TL_NUM] = {0xff};
unsigned char g_aucVideoRLSta[MAX_TL_NUM] = {0xff};

int kjRLStaChangeJudge(unsigned char ucId, unsigned char ucSta, char cRLDecType)
{
	if( RED_DETECT_VIDEO == cRLDecType )
	{
		if( g_aucVideoRLSta[ucId] == ucSta )
		{
			return 0;
		}
		else
		{
			g_aucVideoRLSta[ucId] = ucSta;
			return 1;
		}
	}
	else if(RED_DETECT_SINGAL == cRLDecType )
	{
		if( g_aucRLSta[ucId] == ucSta )
		{
			return 0;
		}
		else
		{
			g_aucRLSta[ucId] = ucSta;
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

int kjRedLightInform(unsigned char ucVehId, unsigned char ucSta, char cRLDecType)
{
    int ret =0;
  
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));

	ret = kjRLStaChangeJudge(ucVehId, ucSta, cRLDecType);
    if(ret <= 0)
    {
    	return ret;
	}
   
    strTmpSysMsg.uiModuleId = TRAFFIC_LIGHT_MANA_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_TRAFFIC_LIGHT_INFO_SET;
    strTmpSysMsg.aucData[0] = ucVehId;
    strTmpSysMsg.aucData[1] = ucSta;
    strTmpSysMsg.aucData[2] = cRLDecType;
    ret = sendMsgToTrafficLM(&strTmpSysMsg);
    if(-1 == ret)
    {
        SYSHANDLE_ERROR("send vehicel red light msg failed.\n");
        ret = getTrafficMsgQSize();
        syslog(LOG_ERR|LOG_USER, "send rl num:%d,sta:%d msg failed.Query size:%d.\n",ucVehId+1,ucSta,ret);
    }
    else
    {
        unsigned long ulPlanStart = time(NULL) + 8*3600;
        char szDefault[256];

        memset(szDefault,0x00,sizeof(szDefault));
        secSwitchToDateString(ulPlanStart, szDefault);
        SYSHANDLE_INFO("send msg ok,rl num:%d,Sta:%d,decType:%d,cur date:%s\n", ucVehId+1, ucSta, cRLDecType,szDefault);

        //ret = getTrafficMsgQSize();
        //syslog(LOG_ERR|LOG_USER,"send msg ok,rl num:%d,Sta:%d,decType:%d,Query size:%d\n", ucVehId+1, ucSta, cRLDecType, ret);
    }
    return ret;
}

int redLightDecTypeChange(unsigned char ucVehId, unsigned char ucSta, char cRLDecType)
{
    int ret =0;
    
    STR_SYS_APP_MSG strTmpSysMsg;
    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
    strTmpSysMsg.uiModuleId = TRAFFIC_LIGHT_MANA_MODULE_ID;
    strTmpSysMsg.uiMsgType = MSG_TRAFFIC_LIGHT_MODE_SET;
    strTmpSysMsg.aucData[0] = ucVehId;
    strTmpSysMsg.aucData[1] = ucSta;
    strTmpSysMsg.aucData[2] = cRLDecType;
    ret = sendMsgToTrafficLM(&strTmpSysMsg);
    if(-1 == ret)
    {
        SYSHANDLE_ERROR("send vehicel red light dec type msg failed.\n");
    }
    else
    {
        SYSHANDLE_INFO("send vehicel red light dec type msg ok.\n");
    }
    return ret;
}

/******************************************************************************
 * AssistRDTimeRangeJudge() :
 * 
 * DESCRIPTION:  �жϸ�����ƺ��Ƿ�ʹ�ܼ�ʱ����Ƿ�ʹ��
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2012.05.05, wangbin Create
 * --------------------
 ******************************************************************************/
int AssistRDTimeRangeJudge(char cLaneId, unsigned char ucVehId)
{
    int i =0;
    int ret =0;
    unsigned int iSeconds =0;
    unsigned long ulPlanStart = time(NULL);
    struct tm tmPlanStart;
    int aiAssistRDTimeSta[EP_ASSIST_RED_LIGHT_NUM];
    
    ulPlanStart += 8*3600;
    gmtime_r((time_t *)&ulPlanStart, &tmPlanStart);
    iSeconds = tmPlanStart.tm_hour*3600 + tmPlanStart.tm_min*60;

    EP_ASSIST_RED_LIGHT_INFO strEpAssistRD[EP_ASSIST_RED_LIGHT_NUM];

    ret = SysConfGet(e_EP_ASSIST_REDLIGHT_CONF, strEpAssistRD, cLaneId);
    if( 0 == ret)
    {
        for(i =0; i< EP_ASSIST_RED_LIGHT_NUM; i++)
        {
            aiAssistRDTimeSta[i] = ASSIST_RD_OUT_RANGE;
            if((strEpAssistRD[i].iBeginTime1 + strEpAssistRD[i].iLastTime1) > DATE_24_HOUR)
            {
                if( (iSeconds >= strEpAssistRD[i].iBeginTime1) && \
                    (iSeconds <= DATE_24_HOUR ) )
                {
                    SYSHANDLE_INFO("1OK_1,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else if ( (iSeconds >= 0) && \
                          (iSeconds <= ((strEpAssistRD[i].iBeginTime1 + strEpAssistRD[i].iLastTime1) - DATE_24_HOUR)) )
                {
                    SYSHANDLE_INFO("1OK_2,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else
                {
                    /*SYSHANDLE_INFO("1failed1,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
                    strEpAssistRD[i].iBeginTime1,strEpAssistRD[i].iLastTime1);*/
                }
            }
            else
            {
                if( (iSeconds >= strEpAssistRD[i].iBeginTime1) && \
                    (iSeconds <= (strEpAssistRD[i].iBeginTime1 + strEpAssistRD[i].iLastTime1) ) )
                {
                    SYSHANDLE_INFO("1_OK3,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else
                {
                    /*SYSHANDLE_INFO("1_failed2,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
                    strEpAssistRD[i].iBeginTime1,strEpAssistRD[i].iLastTime1);*/
                }
            }

            if((strEpAssistRD[i].iBeginTime2 + strEpAssistRD[i].iLastTime2) > DATE_24_HOUR)
            {
                if( (iSeconds >= strEpAssistRD[i].iBeginTime2) && \
                    (iSeconds <= DATE_24_HOUR ) )
                {
                    SYSHANDLE_INFO("2OK_1,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else if ( (iSeconds >= 0) && \
                          (iSeconds <= ((strEpAssistRD[i].iBeginTime2 + strEpAssistRD[i].iLastTime2) - DATE_24_HOUR)) )
                {
                    SYSHANDLE_INFO("2OK_2,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else
                {
                    /*SYSHANDLE_INFO("2failed1,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
                    strEpAssistRD[i].iBeginTime2,strEpAssistRD[i].iLastTime2);*/
                }
            }
            else
            {
                if( (iSeconds >= strEpAssistRD[i].iBeginTime2) && \
                    (iSeconds <= (strEpAssistRD[i].iBeginTime2 + strEpAssistRD[i].iLastTime2) ) )
                {
                    SYSHANDLE_INFO("2_OK3,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else
                {
                    /*SYSHANDLE_INFO("2_failed2,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
                    strEpAssistRD[i].iBeginTime2,strEpAssistRD[i].iLastTime2);*/
                }
            }

            if((strEpAssistRD[i].iBeginTime3 + strEpAssistRD[i].iLastTime3) > DATE_24_HOUR)
            {
                if( (iSeconds >= strEpAssistRD[i].iBeginTime3) && \
                    (iSeconds <= DATE_24_HOUR ) )
                {
                    SYSHANDLE_INFO("3OK_1,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else if ( (iSeconds >= 0) && \
                          (iSeconds <= ((strEpAssistRD[i].iBeginTime3 + strEpAssistRD[i].iLastTime3) - DATE_24_HOUR)) )
                {
                    SYSHANDLE_INFO("3OK_2,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else
                {
                    /*SYSHANDLE_INFO("3failed1,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
                    strEpAssistRD[i].iBeginTime3,strEpAssistRD[i].iLastTime3);*/
                }
            }
            else
            {
                if( (iSeconds >= strEpAssistRD[i].iBeginTime3) && \
                    (iSeconds <= (strEpAssistRD[i].iBeginTime3 + strEpAssistRD[i].iLastTime3) ) )
                {
                    SYSHANDLE_INFO("3_OK3,assist red light include this time:%d Sec\n", iSeconds);
                    aiAssistRDTimeSta[i] = ASSIST_RD_IN_RANGE;
                }
                else
                {
                    /*SYSHANDLE_INFO("3_failed2,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
                    strEpAssistRD[i].iBeginTime3,strEpAssistRD[i].iLastTime3);*/
                }
            }
        }

        for(i =0; i< EP_ASSIST_RED_LIGHT_NUM; i++)
        {
            if( (strEpAssistRD[i].cEnableFlag > 0) && ( strEpAssistRD[i].cRedLightNum == ucVehId)\
            && (aiAssistRDTimeSta[i] == ASSIST_RD_IN_RANGE ) )
            {
                return ASSIST_RD_IN_RANGE;
            }
        }

        for(i =0; i< EP_ASSIST_RED_LIGHT_NUM; i++)
        {
            if( (strEpAssistRD[i].cEnableFlag > 0) && ( strEpAssistRD[i].cRedLightNum != ucVehId)\
            && (aiAssistRDTimeSta[i] == ASSIST_RD_IN_RANGE ) )
            {
                return ASSIST_RD_INVALID_NUM;
            }
        }

        for(i =0; i< EP_ASSIST_RED_LIGHT_NUM; i++)
        {
            if( (strEpAssistRD[i].cEnableFlag > 0) && ( strEpAssistRD[i].cRedLightNum == ucVehId)\
            && (aiAssistRDTimeSta[i] == ASSIST_RD_OUT_RANGE ) )
            {
                return ASSIST_RD_OUT_RANGE;
            }
        }
        
        return ASSIST_RD_DISABLE;
    }
    else
    {
        SYSHANDLE_INFO("sys get assist red light conf failed.\n");
        return ASSIST_RD_DISABLE;
    }
    return ASSIST_RD_DISABLE;
}

int kjAssistRedLightHandle(char cLane, unsigned char ucVehId, unsigned char ucSta, EP_ASSIST_RED_LIGHT_STA *pStrAssistRDSta)
{
    int ret =0;
    ret = AssistRDTimeRangeJudge(cLane, ucVehId);
    SYSHANDLE_INFO("assist red light range time ret is %d!\n", ret);
    if(ret == ASSIST_RD_IN_RANGE)
    {
        pStrAssistRDSta->iLaneNum = cLane;
        pStrAssistRDSta->iRedLightNum = ucVehId;
        pStrAssistRDSta->iSta = ucSta;
        pStrAssistRDSta->iValidFlag = 1;
        return 1;
    }
    else if (ret == ASSIST_RD_INVALID_NUM)
    {
        pStrAssistRDSta->iLaneNum = cLane;
        pStrAssistRDSta->iRedLightNum = ucVehId;
        pStrAssistRDSta->iSta = ucSta;
        pStrAssistRDSta->iValidFlag = 0;
        return 1;
    }
    else if (ret == ASSIST_RD_OUT_RANGE)
    {
        pStrAssistRDSta->iLaneNum = cLane;
        pStrAssistRDSta->iRedLightNum = ucVehId;
        pStrAssistRDSta->iSta = ucSta;
        pStrAssistRDSta->iValidFlag = 0;
        return 0;
    }
    else
    {
        pStrAssistRDSta->iLaneNum = cLane;
        pStrAssistRDSta->iRedLightNum = ucVehId;
        pStrAssistRDSta->iSta = ucSta;
        pStrAssistRDSta->iValidFlag = 0;
        return 0;
    }
}

int cxProtoRLInfoGet(void)
{
	int i =0;
	int cLightType =0;
	struct tm tmPlanStart;
	memset(g_strCXProInfo.strAllTrafficLightInfo, 0, sizeof(g_strCXProInfo.strAllTrafficLightInfo));
    for(i =0; i< MAX_LIGHT_TYPE; i++)
	{
		switch(g_EPLaneSwitchInfo[0].strTrafficL[i].cLightType)
    	{
    		case LIGHT_DIR_LEFT:
    			cLightType = e_cxLeftArrowLightSta;
    			break;
    		case LIGHT_DIR_STRAIGHT:
    			cLightType = e_cxDirArrowLightSta;
    			break;
    		case LIGHT_DIR_RIGHT:
    			cLightType = e_cxRightArrowLightSta;
    			break;
    		case LIGHT_DIR_RETURN:
    			cLightType = e_cxTurnAroundLightSta;
    			break;
    		default:
    			cLightType = e_cxDirArrowLightSta;
    			break;
    	}

   		g_strCXProInfo.strAllTrafficLightInfo[cLightType].cSta = 'N';

	    if( 0 < g_EPLaneSwitchInfo[0].strTrafficL[i].cEnable )
	    {
	    	if( g_EPLaneSwitchInfo[0].strTrafficL[i].cLSta == RED_LIGHT_ON )
	    	{
				g_strCXProInfo.strAllTrafficLightInfo[cLightType].cSta = 'R';
	    	}
			else
			{
				g_strCXProInfo.strAllTrafficLightInfo[cLightType].cSta = 'G';
			}

		    gmtime_r( (time_t *)&(g_EPLaneSwitchInfo[0].uiLightTypeBT[i] ), &tmPlanStart);
		    
		    sprintf(g_strCXProInfo.strAllTrafficLightInfo[cLightType].szBT,"%02d%02d%02d",\
	        tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec);

       	}
    }
    return 0;
}

/* �жϺ�ƺ��Ƿ�Ϊ�ɱ䳵������ƣ�����0��ʾ������仯������1��ʾ���Ա� */

int switchLaneLightNoCheck(int iVehicle)
{
	int i =0;
	for(i=0; i< MAX_VEHICLE_NUM; i++)
	{
		if(g_EPLaneSwitchInfo[i].cSwitchLaneEn > 0)
		{
			if(iVehicle == g_EPLaneSwitchInfo[i].cSwitchLaneLeftNo-1)
			{
				return 0;
			}
			
			if(iVehicle == g_EPLaneSwitchInfo[i].cSwitchLaneStraightNo-1)
			{
				return 0;
			}
		}
	}
	return 1;
}

int kjRedLightHandle(unsigned char ucVehId, unsigned char ucSta, char cRLDecType)
{
    int i =0;
    int k =0;
    int ret =0;
    int iAssistEn =0;
    int iDirSum =0;
    unsigned int iLanId =0;
    /* ��ĳ�����Ѿ��Ǻ���ˣ��ڱ�ĳ�����Ϊ���ʱ�����ظ����ú��ʱ�� */
    int iSta =0;
    /* ֻҪ��һ��״̬�仯�ˣ���Ҫ���½��к��״̬������ */
    int iChangeSta =0;

    /* �ɱ䳵�������仯�� */
    int iSwitchLanSta =0;
    
    /* �ɱ䳵����״̬ */
	int iSwitchLightSta =0;

    unsigned char ucTmpSta =0;

    char cSeq =0;
    char cTmpSta =1;

    lane_nd_red_light_flag strDspRedLightInfoSet;

    struct timeval tv;
	struct timezone tz;

    EP_ASSIST_RED_LIGHT_STA strEpAssistRDSta;
    
	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;

    
    memset(&strEpAssistRDSta, 0, sizeof(strEpAssistRDSta));

    if( cRLDecType == RED_DETECT_VIDEO )
    {
        /* ���ڳ��������Ǵ�0��ʼ�ģ�������Ƶ��Ƽ��Ҳ��Ҫ��Ӧ�ļ�1 */
        ucVehId --;
    }

    SYSHANDLE_INFO("2013.6.27 func:%s,index:%d,red light sta:%d,dec type:%d\n", __FUNCTION__,ucVehId, ucSta, cRLDecType);

    if( cRLDecType != g_iRLDecType )
    {
        SYSHANDLE_INFO("2013.6.27,warning: red light dec type not equal,conf is %d, real is %d.\n", g_iRLDecType, cRLDecType);
        return 0;
    }
    
    /* �ȸ�ȫ�ֺ����Ϣ��ֵ��Ȼ���ٰ��յ�ǰ״̬֪ͨDSP */
    for(iLanId=0; iLanId < g_uiClientLaneSum; iLanId++)
    {
    	iSwitchLanSta =0;
		if(g_EPLaneSwitchInfo[iLanId].cSwitchLaneEn > 0)
		{
			if(ucVehId == g_EPLaneSwitchInfo[iLanId].cSwitchLaneLeftNo -1)
			{
				iSwitchLanSta =1;
				if( RED_LIGHT_ON == ucSta )
				{
					g_EPLaneSwitchInfo[iLanId].cRoadDir = 0;
					g_EPLaneSwitchInfo[iLanId].cRoadDir |= LANE_DIR_LEFT;
					g_EPLaneSwitchInfo[iLanId].cSwitchLaneLeftNoSta = RED_LIGHT_ON;
					iSwitchLightSta =1;
				}
				else
				{
					g_EPLaneSwitchInfo[iLanId].cRoadDir &= ~LANE_DIR_LEFT;
					g_EPLaneSwitchInfo[iLanId].cSwitchLaneLeftNoSta = RED_LIGHT_OFF;
					iSwitchLightSta =0;
				}
			}

			if(ucVehId == g_EPLaneSwitchInfo[iLanId].cSwitchLaneStraightNo -1)
			{
				iSwitchLanSta =1;
				if( RED_LIGHT_ON == ucSta )
				{
					g_EPLaneSwitchInfo[iLanId].cRoadDir = 0;				
					g_EPLaneSwitchInfo[iLanId].cRoadDir |= LANE_DIR_STRAIGHT;
					g_EPLaneSwitchInfo[iLanId].cSwitchLaneStraightNoSta = RED_LIGHT_ON;
					iSwitchLightSta =1;
				}
				else
				{
					g_EPLaneSwitchInfo[iLanId].cRoadDir &= ~LANE_DIR_STRAIGHT;
					g_EPLaneSwitchInfo[iLanId].cSwitchLaneStraightNoSta = RED_LIGHT_OFF;
					iSwitchLightSta =0;
				}
			}

			if( ( 1 == iSwitchLanSta ) && ( 0 == iSwitchLightSta ) )
			{
				/* ����ɱ䷽���״̬Ϊ0�������Ĭ�ϳ������� */
				memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
	    		ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, iLanId);
	    		
	            g_EPLaneSwitchInfo[iLanId].cRoadDir = strEPoliceConf.cLaneDir;
			}
			
			/* �޸Ĳ���������ʻ����dsp���� */
			dspEpSetRunNotLaneDir(iLanId, CHANGE_DIR_SET, g_EPLaneSwitchInfo[iLanId].cRoadDir);
		}

/*Start of ITS on 2011-10-24 15:50 2.0.0.1 for һ��ǿ�ƺ����Ч����Ӧ���������н�ͨ���ź�Ҳǿ��Ϊ���״̬ */
        if( RED_LIGHT_ON == g_bForceRed[iLanId] )
        {
            ucTmpSta = RED_LIGHT_ON;
        }
        else
        {
            ucTmpSta = ucSta;
        }
/*End of ITS on 2011-10-24 15:50 2.0.0.1*/

        cSeq =0;
        iSta =0;
        iChangeSta =0;

        memset(&strDspRedLightInfoSet, 0, sizeof(strDspRedLightInfoSet));

        /* �����ж��Ƿ��з���Ҫ��ĺ��״̬���������¼��ȫ�ֺ��״̬���ݽṹ�� */
        iAssistEn = kjAssistRedLightHandle(iLanId, ucVehId, ucTmpSta, &strEpAssistRDSta);
        if(iAssistEn > 0)
        {
            if( strEpAssistRDSta.iValidFlag <= 0 )
            {
                SYSHANDLE_ERROR("assist red light enable but num is error,direct return!\n");
                return 0;
            }
            for(i =0; i< MAX_LIGHT_TYPE; i++)
            {
                if(0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable)
                {
                    g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta = strEpAssistRDSta.iSta;
                    iSta =1;
                    iChangeSta =1;
                }
                
                SYSHANDLE_INFO("assist rd lane:%d,lighttype:%d,vehNum:%d, inNum:%d,enable:%d,roadDir:%d,normal light sta:%d,force sta:%d\n",\
                                iLanId,i,\
                                g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cVehicleNum,\
                                ucVehId,\
                                g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable,\
                                g_EPLaneSwitchInfo[iLanId].cRoadDir,\
                                g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta,\
                                g_bForceRed[iLanId]);
                                
            }
        }
        else
        {
            for(i =0; i< MAX_LIGHT_TYPE; i++)
            {
                if( (g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cVehicleNum == ucVehId) \
                    && (0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable) )
                {
                    g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta = ucTmpSta;
                    iSta =1;
                    iChangeSta =1;

                    gettimeofday (&tv , &tz);
    				g_EPLaneSwitchInfo[iLanId].uiLightTypeBT[i] = tv.tv_sec + 8*3600;

                }
                
                SYSHANDLE_INFO("lane:%d,lighttype:%d,vehNum:%d, inNum:%d,enable:%d,roadDir:%d,normal light sta:%d,force sta:%d\n",\
                                iLanId,i,\
                                g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cVehicleNum,\
                                ucVehId,\
                                g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable,\
                                g_EPLaneSwitchInfo[iLanId].cRoadDir,\
                                g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta,\
                                g_bForceRed[iLanId]);
                                
            }
        }
        
        SYSHANDLE_INFO("lane:%d,iSta is %d,change sta:%d,iSwitchLaneSta:%d.roadDir:%d.\n", iLanId, iSta, iChangeSta,iSwitchLanSta,\
        g_EPLaneSwitchInfo[iLanId].cRoadDir);
        
        if(( iSta == 1 ) || ( iChangeSta == 1)||(iSwitchLanSta ==1))
        {
            strDspRedLightInfoSet.uiLane = iLanId;
            /* ����dsp��ͨ��״̬�µĳ�����ʻ���� */
            for(iDirSum =0; iDirSum < MAX_LANE_DIR_TYPE; iDirSum++)
            {
                if( g_EPLaneSwitchInfo[iLanId].cRoadDir & (0x01 << iDirSum) )
                {
                    strDspRedLightInfoSet.strNDRedLight[iDirSum].ucNormalDirection = (0x01 << iDirSum);
                }
            }

            /* �������ú��ʱ�䣬��Ҫ�жϽ�ͨ���������ĸ���ƺ��복������һ��,��ʱ֧����ת����ת���ϳ��� */
            for(i =0; i< MAX_LIGHT_TYPE; i++)
        	{
        	    if( 0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable )
        	    {
        	        /* ��ת����ת���ϳ�������������ת��ƺ���Ϊ��ƿ�ʼ�ͽ���ʱ�� */
        	        if(g_EPLaneSwitchInfo[iLanId].cRoadDir & LANE_DIR_LEFT)
        	        {
                        if(( g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLightType ==  LIGHT_DIR_LEFT)\
                            && (g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cVehicleNum == ucVehId))
                        {
                            dspRedLightTimeSet(iLanId, RED_LIGHT_ON);
                        }
        	        }
        	        else
        	        {
        	            if( g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cVehicleNum == ucVehId )
        	            {
            	            dspRedLightTimeSet(iLanId, RED_LIGHT_ON);
                        }
        	        }

                    /* �������ת��ʻ�����ܳ�������Ҫ��ֱ�н�ͨ��״̬һ���·���dsp */
        	        if(g_EPLaneSwitchInfo[iLanId].cRoadDir == LANE_DIR_LEFT)
        	        {
                        if(g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLightType ==  LIGHT_DIR_LEFT)
                        {
                            for(k =0; k< MAX_LIGHT_TYPE; k++)
                        	{
                        	    if( 0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[k].cEnable )
                        	    {
                        	        if( g_EPLaneSwitchInfo[iLanId].strTrafficL[k].cLightType ==  LIGHT_DIR_STRAIGHT )
                                    {
                                        /* ����ֱ�н�ͨ��״̬ */
                            	        strDspRedLightInfoSet.strNDRedLight[i].ucAidRedLightFlag \
                            	        = g_EPLaneSwitchInfo[iLanId].strTrafficL[k].cLSta;
                                    }
                        	    }
                        	}
                        }
        	        }
        	        
                    /* ����dsp��ͨ��״̬ */
        	        strDspRedLightInfoSet.strNDRedLight[i].ucRedLightFlag \
        	        = g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta;
   	            }
   	            else
   	            {
   	                /* ����dsp��ͨ��״̬ */
        	        strDspRedLightInfoSet.strNDRedLight[i].ucRedLightFlag = 0;
   	            }
        	}

            /* ��dsp���͵�ǰ������ͨ�Ƽ�������ʻ����״̬ */
			//sem_wait(&sem_dsp);
			ret = CALL_CYC_CMD(NET_DEV_SET_EPVL_LANE_ND_RED_LIGHT, strDspRedLightInfoSet);
			//sem_post(&sem_dsp);
            if(ret != 0)
            {
                SYSHANDLE_ERROR("call NET_DEV_SET_EPVL_LANE_ND_RED_LIGHT failed, error=%d\n", NET_DVE_GetLastError());
            }
            else
            {
                SYSHANDLE_INFO("lane:%d nd red light ok,(%d,%d,%d),(%d,%d,%d),(%d,%d,%d),(%d,%d,%d)\n",\
                strDspRedLightInfoSet.uiLane,\
                strDspRedLightInfoSet.strNDRedLight[0].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[0].ucRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[0].ucAidRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[1].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[1].ucRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[1].ucAidRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[2].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[2].ucRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[2].ucAidRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[3].ucNormalDirection,strDspRedLightInfoSet.strNDRedLight[3].ucRedLightFlag,\
                strDspRedLightInfoSet.strNDRedLight[3].ucAidRedLightFlag);
            }

            /* ����ۺ��жϸó����Ƿ���Ҫ����Ϊ��� */
	        if((g_EPLaneSwitchInfo[iLanId].cRoadDir & LANE_DIR_BICYCLE)||
	            ( g_EPLaneSwitchInfo[iLanId].cRoadDir & LANE_DIR_RIGHT ) )
	        {
                /* �ȼٶ�û�к�ƣ���Ϊ�ǻ�Ĳ��������޳���ʹ�ܣ���ñ��������Ч */         
                cTmpSta =0;

                /* ��Ϊ�ǻ���������ֻ��Ҫ��һ���Ǻ�Ƽ��ж�Ϊ��ƣ���Ϊ��Ĺ�ϵ */
            	for(i =0; i< MAX_LIGHT_TYPE; i++)
            	{
            	    if( 0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable )
            	    {
            	        cTmpSta |= g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta;
            	        cSeq++;
            	    }
            	}
            }
            else
            {
                /* �ȼٶ��к�ƣ���Ϊ����Ĳ��������޳���ʹ�ܣ���ñ��������Ч */         
                cTmpSta =1;

                /* ��ת֧�ִ�ת�����������������������������Ϻ�ƣ������⼸����ƺž�Ϊ���ʱ��ץ�ģ�����Ĺ�ϵ */
            	for(i =0; i< MAX_LIGHT_TYPE; i++)
            	{
            	    if( 0 < g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cEnable )
            	    {
            	        cTmpSta &= g_EPLaneSwitchInfo[iLanId].strTrafficL[i].cLSta;
            	        cSeq++;
            	    }
            	}
            }
            SYSHANDLE_INFO("cSeq is %d, cTmpSta is %d\n", cSeq, cTmpSta);
        	if(cSeq > 0)
        	{
        	    g_EPLaneSwitchInfo[iLanId].cRedLSta = cTmpSta;
                dspSetRedInfo( iLanId );
        	}
        }
    }

	/* ���´�ϪЭ���ϱ����� */
    cxProtoRLInfoGet();
    
    return 0;
}

void kjDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    char szDefault[256];
    unsigned long ulPlanStart =0;

    struct tm tmPlanStart;
    struct timeval tv;
	struct timezone tz;

	gettimeofday (&tv , &tz);
    ulPlanStart = tv.tv_sec + 8*3600;

	gmtime_r((time_t *)&ulPlanStart, &tmPlanStart);
	sprintf(szDefault, "%04d-%02d-%02d %02d:%02d:%02d:%ld",\
		tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
		tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec, tv.tv_usec/1000);

    SYSHANDLE_INFO("times :%d,cap time:%s\n", iNum, szDefault);
    SYSHANDLE_INFO("comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strKjProtocol.ucMsgType);
    if(MSG_TYPE_COIL_STA == pstVehicle->strDevProtocolSet.strKjProtocol.ucMsgType)
    {
        g_iVehicleHeartSum++;
        vehicleStaMonitor();

        char str[1024];
        memset(str,0,sizeof(str));
        sprintf(str,"������:%d,��Ȧλ��:%s,��Ȧ״̬:%s, ����ʻ����:%s,ʱ���:%d,�������:%d.\n",\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
        (pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos==0)?"ǰ":"��",\
        (pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta==1)?"����":"����",\
        (pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir==0?"����":"����"),\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usGroupId);
    #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
        SYSHANDLE_INFO("ʵʱ�����ϱ� %s\n", str);
        kjPPSnapSeqQuery(pstVehicle->iComNum, pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta, \
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usGroupId);
    #elif defined(CYC500XKW4C)||defined(CYC500XAW)
        SYSHANDLE_INFO("ʵʱ�����ϱ� %s\n", str);
        /* Ϊ�˱�֤�羯ץ��λ�õ�׼ȷ�������Ƚ��е羯ץ�� */
        sendMsgToVehiNormalThread(MSG_KJ_VEHI, pstVehicle);
        sendMsgToVehiPPThread(MSG_KJ_VEHI, pstVehicle);
    #endif
    }
    if(MSG_TYPE_RED_LIGHT == pstVehicle->strDevProtocolSet.strKjProtocol.ucMsgType)
    {
        g_iVehicleHeartSum++;
        vehicleStaMonitor();

        unsigned char ucVehicleLaneId = pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strRedLightInfo.ucLightNum;
        unsigned char ucLightSta = pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strRedLightInfo.ucLightSta;

        SYSHANDLE_INFO("redLightNum:0x%x,sta:0x%x\n", ucVehicleLaneId, ucLightSta);
        kjRedLightInform(ucVehicleLaneId, ucLightSta, RED_DETECT_SINGAL);
    }
    if(MSG_TYPE_BREAKDOWN == pstVehicle->strDevProtocolSet.strKjProtocol.ucMsgType)
    {
        /* ��ӿ��ڵظ���Ȧ�쳣���л���������Ȧ���� */
        unsigned char ucBKSta = pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strBreakDownInfo.ucBreakDownSta;
        unsigned char ucLaneSta = pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strBreakDownInfo.ucLaneSta;
        unsigned char ucRLSta = pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strBreakDownInfo.ucRedLightSta;
        SYSHANDLE_INFO("kj vehilce breakdown Sta:0x%x,lane sta:0x%x, rl sta:0x%x\n", ucBKSta, ucLaneSta, ucRLSta);
        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            kjPPCoilStaSwitch(ucBKSta, ucLaneSta);
        #else
            kjEPCoilStaSwitch(ucBKSta, ucLaneSta, ucRLSta);
        #endif
        g_iVehicleHeartSum++;
        vehicleStaMonitor();
    }
}


void ldDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    int i =0;
    unsigned char ucRdSta =0;
    unsigned short usTmpData =0;
    /* ����ͨ�Ƹߵ��ֽ�״̬��Ĭ��״̬Ϊ�̵� */
    static unsigned short s_usRDSta = LD_RED_LIGHT_ORG;
    unsigned short usRdNormalSta =0;
    unsigned short usRdXorSta =0;
    SYSHANDLE_INFO("times :%d\n", iNum);
    SYSHANDLE_INFO("comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strLdProtocol.ucMsgType);
    //if(MSG_TYPE_RED_LIGHT == pstVehicle->strDevProtocolSet.strLdProtocol.ucMsgType)
    {
        usRdNormalSta = pstVehicle->strDevProtocolSet.strLdProtocol.strLDProtoData.ucHighData;
        usRdNormalSta <<= 8;
        usRdNormalSta |= pstVehicle->strDevProtocolSet.strLdProtocol.strLDProtoData.ucLowData;

        usRdXorSta = usRdNormalSta ^ s_usRDSta;
        SYSHANDLE_INFO("ld normal rd sta :0x%x, xor sta:0x%x,last sta:0x%x\n", usRdNormalSta, usRdXorSta, s_usRDSta);
        
        for(i =0; i< LD_PORT_SUM; i++)
        {
            usTmpData = 0x01 << i;
            if( usTmpData & usRdXorSta )
            {
                if(( usTmpData & usRdNormalSta ) > 0)
                {
                    ucRdSta = RED_LIGHT_OFF;
                }
                else
                {
                    ucRdSta = RED_LIGHT_ON;
                }
                SYSHANDLE_INFO("ld vehNum:0x%x, rd sta:0x%x.\n", i, ucRdSta);
                kjRedLightInform(i, ucRdSta, RED_DETECT_SINGAL);
            }
        }
        s_usRDSta = usRdNormalSta;
    }
}

void sjDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("sj times :%d\n", iNum);
    SYSHANDLE_INFO("sj comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strSjProtocol.ucMsgType);
    if(MSG_TYPE_COIL_STA == pstVehicle->strDevProtocolSet.strSjProtocol.ucMsgType)
    {
        char str[1024];
        memset(str,0,sizeof(str));
        sprintf(str,"������:%d,��Ȧλ��:%s,��Ȧ״̬:%s, ����ʻ����:%s,����ʱ��:%d.\n",\
        pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCarChannel,\
        (pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCoilPos==0)?"ǰ":"��",\
        (pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCoilSta==1)?"����":"����",\
        (pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCarDir==0?"����":"����"),\
        pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.usLastTime);
        SYSHANDLE_INFO("sj ʵʱ״̬�ϱ� %s\n", str);
    #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
        sjPPSnapSeqQuery(pstVehicle->iComNum, pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCarChannel,\
        pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCoilPos,\
        pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCoilSta, \
        pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.ucCarDir,\
        pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strCoilInfo.usLastTime);
        /* ����ÿ��Э�鶼�����˹�����Ϣ������ÿ�ζ�Ҫ���й��ϴ��� */
        sjPPCoilStaSwitch(pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strBreakDownInfo.ucCoilSta, 0xff);
    #elif defined(CYC500XKW4C)||defined(CYC500XAW)
        /* Ϊ�˱�֤�羯ץ��λ�õ�׼ȷ�������Ƚ��е羯ץ�� */
        sendMsgToVehiNormalThread(MSG_SJ_VEHI, pstVehicle);
        sendMsgToVehiPPThread(MSG_SJ_VEHI, pstVehicle);
    #endif
    }
    if(MSG_TYPE_BREAKDOWN == pstVehicle->strDevProtocolSet.strSjProtocol.ucMsgType)
    {
        /* ��ӿ��ڵظ���Ȧ�쳣���л���������Ȧ���� */
        //pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strBreakDownInfo.ucCoilSta[i];
        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            sjPPCoilStaSwitch(pstVehicle->strDevProtocolSet.strSjProtocol.strSjProtoData.strBreakDownInfo.ucCoilSta, 0xff);
        #endif
    }
}

void dcDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("times :%d\n", iNum);
    SYSHANDLE_INFO("comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strKjProtocol.ucMsgType);
    if(MSG_TYPE_COIL_STA == pstVehicle->strDevProtocolSet.strKjProtocol.ucMsgType)
    {
        char str[1024];
        memset(str,0,sizeof(str));
        sprintf(str,"������:%d,��Ȧλ��:%s,��Ȧ״̬:%s, ����ʻ����:%s,ʱ���:%d.\n",\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
        (pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos==0)?"ǰ":"��",\
        (pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta==1)?"����":"����",\
        (pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir==0?"����":"����"),\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime);
        SYSHANDLE_INFO("ʵʱ״̬�ϱ� %s\n", str);
    #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
        kjPPSnapSeqQuery(pstVehicle->iComNum, pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta, \
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime,\
        pstVehicle->strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usGroupId);
    #elif defined(CYC500XKW4C)||defined(CYC500XAW)
        /* Ϊ�˱�֤�羯ץ��λ�õ�׼ȷ�������Ƚ��е羯ץ�� */
        sendMsgToVehiNormalThread(MSG_DC_VEHI, pstVehicle);
        sendMsgToVehiPPThread(MSG_DC_VEHI, pstVehicle);
    #endif
    }
}

void adDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("ad times :%d\n", iNum);
    SYSHANDLE_INFO("ad comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strCsProtocol.ucMsgType);

    SYSHANDLE_INFO("ad carSpeed:%d.\n",\
    pstVehicle->strDevProtocolSet.strAdProtocol.strADProtoData.ucCarSpeed);

    adPPSnapSeqQuery(pstVehicle->iComNum, 0,\
                pstVehicle->strDevProtocolSet.strCsProtocol.strCSProtoData.ucCarSpeed,\
                pstVehicle->strDevProtocolSet.strCsProtocol.strCSProtoData.ucCarDir);
}

void csMcDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("csmc times :%d\n", iNum);
    SYSHANDLE_INFO("csmc comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strcsMcProtocol.ucMsgType);

    SYSHANDLE_INFO("csmc no:%d carSpeed:%d\n",\
    	pstVehicle->strDevProtocolSet.strcsMcProtocol.strCSMCProtoData.ucNo,\
        pstVehicle->strDevProtocolSet.strcsMcProtocol.strCSMCProtoData.ucCarSpeed);

    csMcPPSnapSeqQuery(pstVehicle->iComNum, \
    			pstVehicle->strDevProtocolSet.strcsMcProtocol.strCSMCProtoData.ucNo,\
                pstVehicle->strDevProtocolSet.strcsMcProtocol.strCSMCProtoData.ucCarSpeed,\
                0);
}

void csDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("cs times :%d\n", iNum);
    SYSHANDLE_INFO("cs comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strCsProtocol.ucMsgType);

    SYSHANDLE_INFO("cs carSpeed:%d,carDir:%d\n",\
    pstVehicle->strDevProtocolSet.strCsProtocol.strCSProtoData.ucCarSpeed,\
    pstVehicle->strDevProtocolSet.strCsProtocol.strCSProtoData.ucCarDir);

    csPPSnapSeqQuery(pstVehicle->iComNum, 0,\
                pstVehicle->strDevProtocolSet.strCsProtocol.strCSProtoData.ucCarSpeed,\
                pstVehicle->strDevProtocolSet.strCsProtocol.strCSProtoData.ucCarDir);
}

void hcDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("hc times :%d\n", iNum);
    SYSHANDLE_INFO("hc comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strCsProtocol.ucMsgType);

    SYSHANDLE_INFO("hc lane:%d,carSpeed:%d,carDir:%d\n",\
    pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucLaneId,\
    pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarSpeed,\
    pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarDir);

    hcPPSnapSeqQuery(pstVehicle->iComNum, \
                pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucLaneId,\
                pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarSpeed,\
                pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarDir);
}

void wkDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("wk times :%d\n", iNum);
    SYSHANDLE_INFO("wk comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strCsProtocol.ucMsgType);

    SYSHANDLE_INFO("wk lane:%d,carSpeed:%d,carDir:%d\n",\
    pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucLaneId,\
    pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarSpeed,\
    pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarDir);

    wkPPSnapSeqQuery(pstVehicle->iComNum, \
                pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucLaneId,\
                pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarSpeed,\
                pstVehicle->strDevProtocolSet.strHcProtocol.strHCProtoData.ucCarDir);
}

void spiDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("spi times :%d\n", iNum);
#if defined(CYC500XZD)||defined(CYC200XZD)
    spiTransInfoQuery(pstVehicle->strDevProtocolSet.strSpiProtocol.ucData);
#endif
}

void olviaDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("olvia times :%d\n", iNum);
    SYSHANDLE_INFO("olvia comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strolviaProtocol.ucMsgType);

    SYSHANDLE_INFO("olvia no:%d,carSpeed:%d,carDir:%d\n",\
    pstVehicle->strDevProtocolSet.strolviaProtocol.strOlviaProtoData.ucNo,\
    pstVehicle->strDevProtocolSet.strolviaProtocol.strOlviaProtoData.ucCarSpeed,\
    pstVehicle->strDevProtocolSet.strolviaProtocol.strOlviaProtoData.ucCarDir);

    olviaPPSnapSeqQuery(pstVehicle->iComNum, \
    			pstVehicle->strDevProtocolSet.strolviaProtocol.strOlviaProtoData.ucNo,\
                pstVehicle->strDevProtocolSet.strolviaProtocol.strOlviaProtoData.ucCarSpeed,\
                pstVehicle->strDevProtocolSet.strolviaProtocol.strOlviaProtoData.ucCarDir);
}

void cyldDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum)
{
    SYSHANDLE_INFO("cyld times :%d\n", iNum);
    SYSHANDLE_INFO("cyld comNum:%d,devType:%d,msg_type:%d\n",\
    pstVehicle->iComNum,\
    pstVehicle->iDevType, \
    pstVehicle->strDevProtocolSet.strCyldProtocol.ucMsgType);

    SYSHANDLE_INFO("cyld carDir:%d carSpeed:%d\n",\
    	pstVehicle->strDevProtocolSet.strCyldProtocol.strCyldProtoData.ucCarDir,\
        pstVehicle->strDevProtocolSet.strCyldProtocol.strCyldProtoData.ucCarSpeed);

    cyldPPSnapSeqQuery(pstVehicle->iComNum, \
    			0,\
                pstVehicle->strDevProtocolSet.strCyldProtocol.strCyldProtoData.ucCarSpeed,\
                0);
}

int dctlPPSpeedAmend(int iCarSpeed)
{
    int ret =0;
    unsigned short usRelSpeed = iCarSpeed & 0xffff;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
    if(strSysFuncCfg.cSpeedAmendFlag <= 0)
    {
        DSP_INFO("kj vehicle speed amend is not enable!\n");
        return iCarSpeed;
    }
#if 0    
    if(( iCarSpeed >=91 ) && ( iCarSpeed <= 95))
    {
        iCarSpeed += 1;
    }
    else if(( iCarSpeed >=100 ) && ( iCarSpeed <= 103))
    {
        iCarSpeed -= 3;
    }
    else if( ( iCarSpeed >=108 ) && ( iCarSpeed <= 113) )
    {
        iCarSpeed += 5;
    }
    else if( ( iCarSpeed >=120 ) && ( iCarSpeed <= 125) )
    {
        iCarSpeed += 4;
    }
    else if( ( iCarSpeed >=133 ) && ( iCarSpeed <= 141) )
    {
        iCarSpeed += 8;
    }
    else if( ( iCarSpeed >=150 ) && ( iCarSpeed <= 158) )
    {
        iCarSpeed -= 8;
    }
    else if( ( iCarSpeed >=163 ) && ( iCarSpeed <= 170) )
    {
        iCarSpeed += 9;
    }
    else if( ( iCarSpeed >=180 ) && ( iCarSpeed <= 200) )
    {
        iCarSpeed = 179;
    }
#else
    if(usRelSpeed > 90)
    {
        if( strSysFuncCfg.cSpeedAmendX == 0 )
        {
            strSysFuncCfg.cSpeedAmendX = 5;
            DSP_ERROR("kj vehicle speed amend x is 0,change to 5!\n");
        }
        if( strSysFuncCfg.cSpeedAmendX > 0x80 )
        {
            usRelSpeed = usRelSpeed - usRelSpeed/(256-strSysFuncCfg.cSpeedAmendX) - strSysFuncCfg.cSpeedAmendY;
        }
        else
        {
            usRelSpeed = usRelSpeed + usRelSpeed/strSysFuncCfg.cSpeedAmendX - strSysFuncCfg.cSpeedAmendY;
        }
    }
    iCarSpeed &= 0xffff0000;
    iCarSpeed |= usRelSpeed;
#endif
    return iCarSpeed;
}

unsigned short dctlPPVehHighSpeedGet(char cComId)
{
    int ret =0;
    DCTL_CONF_INFO strDctlInfo;
    memset(&strDctlInfo, 0, sizeof(strDctlInfo));
	ret = SysConfGet(e_DCTL_CONF, &strDctlInfo, cComId);
	if ( 0== ret )
	{
	    return strDctlInfo.usMaxMeasureSpeed;
	}
	else
	{
	    return ret;
	}
}


int dctlPPTTLNormalSnap(int iCarRoadId, int iStep, int iSnapType)
{
    int ret =0;
    SNAP_INFO  strSnapInfo;

    memset(&strSnapInfo, 0, sizeof(strSnapInfo));

    strSnapInfo.laneInfo[0].ucRoadId   = iCarRoadId;
    strSnapInfo.laneInfo[0].uiFlashFlg = SNAP_FLASH_FLAG;
    strSnapInfo.laneInfo[0].ucSnapType = iSnapType;
    if( SNAP_TYPE_SINGAL_COIL == iSnapType )
    {
        strSnapInfo.laneInfo[0].ucStepNum =1;
    }
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;
    ret = dctlModuleFlashSnapSeq(&strSnapInfo);
    return ret;
}

int dctlPPTTLForceSnap(int iCarRoadId, int iStep, int iSnapType)
{
    int ret =0;
    SNAP_INFO  strSnapInfo;

    memset(&strSnapInfo, 0, sizeof(strSnapInfo));

    strSnapInfo.laneInfo[0].ucRoadId   = iCarRoadId;
    strSnapInfo.laneInfo[0].ucSnapType = iSnapType;
    if( SNAP_TYPE_SINGAL_COIL == iSnapType )
    {
        strSnapInfo.laneInfo[0].ucStepNum =1;
    }
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;
    ret = dctlModuleFlashForceSnap(&strSnapInfo);
    return ret;
}

/* ��ȡ�״�У������ */
unsigned short ppCsSpeedAmendNumeratorGet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if(0==ret)
	{
	    return strSysFuncCfg.usCsSpeedAmendNumerator;
	}
	else
	{
	    SYSHANDLE_INFO("get sys conf of CsSpeedAmendNumerator failed!\n");
	    return ret;
	}
}

int dctlPPFlashForceSnap(int iComId, int iCarRoadId, int iStep, int iCarSpeed, int iCarDir)
{
    int ret =0;
    SNAP_INFO  strSnapInfo;
    unsigned short usSpeedNumerator =0;
    unsigned char ucCorrectSpeed =0;
    unsigned short usVhMeasureSpeed =0;

	unsigned char  ucCarDir = (iCarDir&0xff);
    unsigned int uiSnapGroup = (iCarDir&0xffff0000);

    uiSnapGroup >>= 16;

    memset(&strSnapInfo, 0, sizeof(strSnapInfo));

    if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  SNAP_FLASH_FLAG;
        if(iStep > 2)
        {
        	strSnapInfo.laneInfo[0].uiFlashFlg =  0;
        }
    }
    else
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  0;
    }
    strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
    if(CAR_RUN_CORRECT_DIR == ucCarDir)
    {
        strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_SPEED;
    }
    else if( CAR_RUN_REVERSE_DIR == ucCarDir )
    {
        strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_REVERSE;
    }
    else
    {
        strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_SINGAL_COIL;
    }

    if(g_PPLaneSwitchInfo[iCarRoadId].cGroudCoilDecType != CS_RADAR_SPEED)
    {
        iCarSpeed =dctlPPSpeedAmend(iCarSpeed);
        usVhMeasureSpeed = dctlPPVehHighSpeedGet(iComId);
        usSpeedNumerator = iCarSpeed&0xffff;

        /* ����ϱ��ٶȴ��ڳ������ܹ�����ٶȵ����ֵ������ٶ��޸�Ϊ��ʾ�ٶȣ���ֹ�쳣 */
        if( usSpeedNumerator > usVhMeasureSpeed)
        {
            SYSHANDLE_INFO("before kj vehilce change, max measue speed:%d,vehicle speed:%d \n", usVhMeasureSpeed, usSpeedNumerator);
            usSpeedNumerator = dspPPLimitSpeedGet(0, iCarRoadId);
            SYSHANDLE_INFO("after kj vehilce change, max measue speed:%d,cur speed:%d \n", usVhMeasureSpeed, usSpeedNumerator);
            iCarSpeed &= 0xffff0000;
            iCarSpeed |= usSpeedNumerator;
        }
        else
        {
            SYSHANDLE_INFO("kj vehilce normal measue speed:%d,cur speed:%d \n", usVhMeasureSpeed, iCarSpeed);
        }
    }
    else
    {
        usSpeedNumerator = ppCsSpeedAmendNumeratorGet();
        ucCorrectSpeed = iCarSpeed*usSpeedNumerator/1000;
        DSP_INFO("cs radar normal speed:%d, correct speed:%d,numerator:%d!\n", \
        iCarSpeed, ucCorrectSpeed, usSpeedNumerator);
        iCarSpeed = ucCorrectSpeed;
    }
    
    if(strSnapInfo.laneInfo[0].ucSnapType == SNAP_TYPE_SINGAL_COIL)
    {
        strSnapInfo.laneInfo[0].ucStepNum =1;
    }
    else
    {
    	if(g_PPLaneSwitchInfo[iCarRoadId].cGroudCoilDecType == CS_RADAR_SPEED)
    	{
        	strSnapInfo.laneInfo[0].ucStepNum =2;
        	if( iStep > strSnapInfo.laneInfo[0].ucStepNum )
        	{
        		/* �����ܲ�����ץ������·� */
        		return 0;
        	}
        }
        else
        {
        	strSnapInfo.laneInfo[0].ucStepNum =3;
        }
    }
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;
    strSnapInfo.laneInfo[0].uiSpeed = iCarSpeed;
    strSnapInfo.laneInfo[0].usGroupId = uiSnapGroup;
    SYSHANDLE_INFO("force snap flash flush flag:%d,step:%d,speed:%d,laneId:%d,snap type:%d,groupId:%d\n",\
    strSnapInfo.laneInfo[0].uiFlashFlg,\
    strSnapInfo.laneInfo[0].ucCurrentStep,\
    strSnapInfo.laneInfo[0].uiSpeed,\
    strSnapInfo.laneInfo[0].ucRoadId,\
    strSnapInfo.laneInfo[0].ucSnapType,\
    strSnapInfo.laneInfo[0].usGroupId);
    ret = dctlModuleFlashForceSnap(&strSnapInfo);
    if( iStep == strSnapInfo.laneInfo[0].ucStepNum )
    {
        g_jpegSnapCountInfo[0].uiAppForceSnapFullGSum += 1;
    }
    return ret;
}


int dctlPPFlashNormalSnap(int iComId, unsigned int iCarRoadId, int iStep, int iCarSpeed, int iCarDir)
{
    int ret =0;
    SNAP_INFO  strSnapInfo;
    unsigned short usSpeedNumerator =0;
    unsigned char ucCorrectSpeed =0;
    unsigned short usVhMeasureSpeed =0;
    unsigned char  ucCarDir = (iCarDir&0xff);
    unsigned int uiSnapGroup = (iCarDir&0xffff0000);

    uiSnapGroup >>= 16;

    #if 0 /* ��������Ȧ��ץ�����ˣ��ظ���Ȧץ�Ļ����� */
    /* �����������Ȧ����ֱ�ӷ��� */
    if( g_PPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta == SWITCH_DEFAULT_VAL)
    {
        if( GROUD_LOOP != g_PPLaneSwitchInfo[iCarRoadId].cCoilDecType)
        {
            SYSHANDLE_INFO("coil type is virtual,return!\n");
            return -1;
        }
    }
    else
    {
        if( GROUD_LOOP != g_PPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta)
        {
            SYSHANDLE_INFO("coil switch sta is virtual,return!\n");
            return -1;
        }
    }
    #endif
    
    if( iCarRoadId >= g_uiClientLaneSum )
    {
        SYSHANDLE_ERROR("lane is big,return!\n");
        return -1;
    }

    memset(&strSnapInfo, 0, sizeof(strSnapInfo));
    if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  SNAP_FLASH_FLAG;
        
/* Ϊ��������Ч�ʣ�����ֱ����ǿ�����ⷽʽ��ע��ƽʱ��Ҫ�ر� */
#if 1
        ret = dctlPPFlashForceSnap(iComId, iCarRoadId, iStep, iCarSpeed, iCarDir);
        return ret;
#endif    

    }
    /* begin add by wangb for ±���ʱ�������ں˶��У�ֱ�ӽ�DSP���� */
    else if( LIGHT_TYPE_HALGON == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  0;
        ret = dctlPPFlashForceSnap(iComId, iCarRoadId, iStep, iCarSpeed, iCarDir);
        return ret;
    }
    /* end add by wangb for ±���ʱ�������ں˶��У�ֱ�ӽ�DSP���� */
    strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
    if(CAR_RUN_CORRECT_DIR == ucCarDir)
    {
        strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_SPEED;
    }
    else if( CAR_RUN_REVERSE_DIR == ucCarDir )
    {
        strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_REVERSE;
    }
    else
    {
        strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_SINGAL_COIL;
    }

    if(g_PPLaneSwitchInfo[iCarRoadId].cGroudCoilDecType != CS_RADAR_SPEED)
    {
        iCarSpeed =dctlPPSpeedAmend(iCarSpeed);
        usVhMeasureSpeed = dctlPPVehHighSpeedGet(iComId);
        usSpeedNumerator = iCarSpeed&0xffff;
        if( usSpeedNumerator > usVhMeasureSpeed)
        {
            SYSHANDLE_INFO("before kj vehilce change, max measue speed:%d,vehicle speed:%d \n", usVhMeasureSpeed, usSpeedNumerator);
            usSpeedNumerator = dspPPLimitSpeedGet(0, iCarRoadId);
            SYSHANDLE_INFO("after kj vehilce change, max measue speed:%d,cur speed:%d \n", usVhMeasureSpeed, usSpeedNumerator);
            iCarSpeed &= 0xffff0000;
            iCarSpeed |= usSpeedNumerator;
        }
        else
        {
            SYSHANDLE_INFO("kj vehilce normal measue speed:%d,cur speed:%d \n", usVhMeasureSpeed, iCarSpeed);
        }
    }
    else
    {
        usSpeedNumerator = ppCsSpeedAmendNumeratorGet();
        ucCorrectSpeed = iCarSpeed*usSpeedNumerator/1000;
        DSP_INFO("cs radar normal speed:%d, correct speed:%d,numerator:%d!\n", \
        iCarSpeed, ucCorrectSpeed, usSpeedNumerator);
        iCarSpeed = ucCorrectSpeed;
    }
    
    if( strSnapInfo.laneInfo[0].ucSnapType == SNAP_TYPE_SINGAL_COIL )
    {
        strSnapInfo.laneInfo[0].ucStepNum =1;
    }
    else
    {
        strSnapInfo.laneInfo[0].ucStepNum =2;
    }
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;
    strSnapInfo.laneInfo[0].uiSpeed = iCarSpeed;
    strSnapInfo.laneInfo[0].usGroupId = uiSnapGroup;
    
    SYSHANDLE_INFO("normal snap flash flush flag:%d,step:%d,speed:%d,laneId:%d,snap type:%d,groupId:%d\n",\
    strSnapInfo.laneInfo[0].uiFlashFlg,\
    strSnapInfo.laneInfo[0].ucCurrentStep,\
    strSnapInfo.laneInfo[0].uiSpeed,\
    strSnapInfo.laneInfo[0].ucRoadId,\
    strSnapInfo.laneInfo[0].ucSnapType,\
    strSnapInfo.laneInfo[0].usGroupId);
    
    ret = dctlModuleFlashSnapSeq(&strSnapInfo);
    if( iStep == strSnapInfo.laneInfo[0].ucStepNum )
    {
        g_jpegSnapCountInfo[0].uiAppNormalSnapFullGSum += 1;
    }
    return ret;
}

int dctlRadarNormalSnap(int iComId, unsigned int iCarRoadId, int iStep, int iCarSpeed, int iCarDir)
{
    int ret =0;
    SNAP_INFO  strSnapInfo;
    unsigned short usSpeedNumerator =0;
    unsigned char ucCorrectSpeed =0;
    PORT_VEHICLE_CONF_INFO strPPortConf;

    usSpeedNumerator = ppCsSpeedAmendNumeratorGet();

    ucCorrectSpeed = iCarSpeed*usSpeedNumerator/1000;
    DSP_INFO("cs radar normal speed:%d, correct speed:%d,numerator:%d!\n", \
    iCarSpeed, ucCorrectSpeed, usSpeedNumerator);

    ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iCarRoadId);

    memset(&strSnapInfo, 0, sizeof(strSnapInfo));

    strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
    strSnapInfo.laneInfo[0].ucSnapType = SNAP_TYPE_RADAR;
    strSnapInfo.laneInfo[0].ucStepNum =1;
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;
    strSnapInfo.laneInfo[0].uiSpeed = ucCorrectSpeed;
    
    SYSHANDLE_INFO("radar snap flash flush flag:%d,step:%d,speed:%d,laneId:%d,snap type:%d\n",\
    strSnapInfo.laneInfo[0].uiFlashFlg,\
    strSnapInfo.laneInfo[0].ucCurrentStep,\
    strSnapInfo.laneInfo[0].uiSpeed,\
    strSnapInfo.laneInfo[0].ucRoadId,\
    strSnapInfo.laneInfo[0].ucSnapType);
    
    if(( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] ) \
    && ( iCarSpeed > strPPortConf.usMaxSpeed ))
    {
        if(g_PPLaneSwitchInfo[iCarRoadId].cCoilDecType == VIRTUAL_LOOP)
        {
            strSnapInfo.laneInfo[0].uiFlashFlg =  0;
        }
        else
        {
            strSnapInfo.laneInfo[0].uiFlashFlg =  SNAP_FLASH_FLAG;
        }
        ret = dctlModuleFlashSnapSeq(&strSnapInfo);
    }
    else if( LIGHT_TYPE_HALGON == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  0;
        ret = dctlModuleFlashForceSnap(&strSnapInfo);
    }    
    ret = dctlModuleFlashSnapSeq(&strSnapInfo);
    if( iStep == strSnapInfo.laneInfo[0].ucStepNum )
    {
        g_jpegSnapCountInfo[iCarRoadId].uiAppNormalSnapFullGSum += 1;
    }
    return ret;
}

/******************************************************************************
 * dctlEPFlashNormalSnap() :
 * 
 * DESCRIPTION:  �ж��Ƿ�Ϊ��ƣ�������֪ͨDSPΪ���״̬���Ա��ڻ�ȡץ��ʱ�䣬��
 *  �������ж�����Ҫ�����̺�ץ�ģ���Ҫ������̵�ץ��
 * Input:
 * Output:
 * Returns:
 * 
 * modification history
 * --------------------
 *01a, 2010.01.16, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlEPFlashNormalSnap(unsigned int iCarRoadId, int iStep, int iSnapType)
{
    int ret =0;
    /* �ñ�������ʾ�������ٶȣ�ֻ��Ϊ�˱�ʾץ��˳�� */
    static unsigned int   s_uiTotalTimes =0;
    static unsigned int   s_ppUiSum =0;
    unsigned int uiTmpTotal =0;
    static unsigned short s_usCarRoad0 =0;
    static unsigned short s_usCarRoad1 =0;
    static unsigned short s_usCarRoad2 =0;
    
    SNAP_INFO  strSnapInfo;
    char szDefault[256];
    unsigned long ulPlanStart =0;

    struct tm tmPlanStart;

    struct timeval tv;
	struct timezone tz;

    char   cPpFlashEn =0;

	gettimeofday (&tv , &tz);

    ulPlanStart = tv.tv_sec + 8*3600;

	gmtime_r((time_t *)&ulPlanStart, &tmPlanStart);
	sprintf(szDefault, "%04d-%02d-%02d %02d:%02d:%02d:%ld",\
		tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
		tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec, tv.tv_usec/1000);

    /* �����������Ȧ����ֱ�ӷ��� */
    if( g_EPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta == SWITCH_DEFAULT_VAL)
    {
        if( GROUD_LOOP != g_EPLaneSwitchInfo[iCarRoadId].cCoilDecType)
        {
            SYSHANDLE_INFO("coil type is virtual,return!\n");
            return -1;
        }
    }
    else
    {
        if( GROUD_LOOP != g_EPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta)
        {
            SYSHANDLE_INFO("coil switch sta is virtual,return!\n");
            return -1;
        }
    }
    if( iCarRoadId >= g_uiClientLaneSum )
    {
        SYSHANDLE_ERROR("lane is big,return!\n");
        return -1;
    }
    
    memset(&strSnapInfo, 0, sizeof(strSnapInfo));

    strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
    strSnapInfo.laneInfo[0].ucSnapType = iSnapType;

    if( SNAP_TYPE_CLIP != iSnapType )
    {
        uiTmpTotal = s_uiTotalTimes;
        uiTmpTotal &= 0xffff;
        uiTmpTotal <<= 16;
        if(0 == iCarRoadId)
        {
            if(iStep == 1)
            {
                s_usCarRoad0 =0;
            }

            s_usCarRoad0++;
            strSnapInfo.laneInfo[0].uiSpeed =  uiTmpTotal | s_usCarRoad0;
        }
        else if(1 == iCarRoadId)
        {
            if(iStep == 1)
            {
                s_usCarRoad1 =0;
            }

            s_usCarRoad1++;
            strSnapInfo.laneInfo[0].uiSpeed =  uiTmpTotal | s_usCarRoad1;
        }
        else
        {
            if(iStep == 1)
            {
                s_usCarRoad2 =0;
            }
            
            s_usCarRoad2++;
            strSnapInfo.laneInfo[0].uiSpeed =  uiTmpTotal | s_usCarRoad2;
        }
        SYSHANDLE_INFO("ep,lane:%d,snap type:%d, ep snap speed:0x%x,time:%s\n", \
                        iCarRoadId, iSnapType, strSnapInfo.laneInfo[0].uiSpeed, szDefault);
    }
    
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;

    if( (SNAP_TYPE_3_PLUS_1 == iSnapType ) || (SNAP_TYPE_3_PLUS_1_LEFT==iSnapType) )
    {
        strSnapInfo.laneInfo[0].ucStepNum =3;
        
        if( iStep == 3)
        {
            s_uiTotalTimes++;
        }
    }
    else if(SNAP_TYPE_3_SEPARATE == iSnapType)
    {
        strSnapInfo.laneInfo[0].ucStepNum =3;
        
        if( iStep == 3)
        {
            s_uiTotalTimes++;
        }

    }
    else if(SNAP_TYPE_4_SEPARATE == iSnapType)
    {
        strSnapInfo.laneInfo[0].ucStepNum =4;
        
        if( iStep == 4)
        {
            s_uiTotalTimes++;
        }

    }
    else if(SNAP_TYPE_6_PLUS_1 == iSnapType)
    {
        strSnapInfo.laneInfo[0].ucStepNum =6;
        if( iStep == 6)
        {
            s_uiTotalTimes++;
        }

    }
    else if( SNAP_TYPE_CLIP == iSnapType )
    {
       	cPpFlashEn = epPpFalshEnFlagGet();
        if( 1 == cPpFlashEn )
        {
            /* ���Ӿ����̵�ץ��ʱ��ʹ������� */
            strSnapInfo.laneInfo[0].uiFlashFlg = SNAP_FLASH_FLAG;
        }
        else
        {
    		/* ���Ӿ����̵�ץ��ʱ������ʹ������� */
            strSnapInfo.laneInfo[0].uiFlashFlg = 0;
        }
        strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
        strSnapInfo.laneInfo[0].ucSnapType = iSnapType;
        strSnapInfo.laneInfo[0].ucStepNum =1;
        strSnapInfo.laneInfo[0].ucCurrentStep = 1;

        uiTmpTotal = s_uiTotalTimes;
        uiTmpTotal &= 0xffff;
        uiTmpTotal <<= 16;
        s_ppUiSum &= 0xffff;
        strSnapInfo.laneInfo[0].uiSpeed = uiTmpTotal | s_ppUiSum;

        SYSHANDLE_INFO("ep,lane:%d,snap type:%d, pp snap speed:0x%x,time:%s.\n", iCarRoadId, iSnapType, strSnapInfo.laneInfo[0].uiSpeed,szDefault);
        s_ppUiSum++;
        if( 1 == cPpFlashEn )
        {   
            if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] )
            {
                strSnapInfo.laneInfo[0].uiFlashFlg =  SNAP_FLASH_FLAG;
                ret = dctlModuleFlashSnapSeq(&strSnapInfo);
            }
            else if( LIGHT_TYPE_HALGON == g_iWorkFlashMode[iCarRoadId] )
            {
                strSnapInfo.laneInfo[0].uiFlashFlg =  0;
                ret = dctlModuleFlashForceSnap(&strSnapInfo);
            }
            else
            {
                SYSHANDLE_ERROR("ep lane:%d, snap flash type:%d error\n", iCarRoadId, g_iWorkFlashMode[iCarRoadId]);
                ret =-1;
                return ret;
            }
	    }
	    else
	    {
    		ret = dctlModuleFlashForceSnap(&strSnapInfo);
        }
        g_jpegSnapCountInfo[iCarRoadId].uiAppNormalSnapFullGSum +=1;
        return ret;
    }
    else
    {
        SYSHANDLE_ERROR("ep snap type:%d error\n", iSnapType);
        ret =-1;
        return ret;
    }

    if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  SNAP_FLASH_FLAG;
        ret = dctlModuleFlashSnapSeq(&strSnapInfo);
        if(strSnapInfo.laneInfo[0].ucStepNum == iStep)
        {
            g_jpegSnapCountInfo[iCarRoadId].uiAppNormalSnapFullGSum +=1;
        }
    }
    /* begin add by wangb for ±���ʱ�������ں˶��У�ֱ�ӽ�DSP���� */
    else if( LIGHT_TYPE_HALGON == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  0;
        ret = dctlModuleFlashForceSnap(&strSnapInfo);
    }
    /* end add by wangb for ±���ʱ�������ں˶��У�ֱ�ӽ�DSP���� */
    else
    {
        SYSHANDLE_ERROR("ep lane:%d, snap flash type:%d error\n", iCarRoadId, g_iWorkFlashMode[iCarRoadId]);
        ret =-1;
        return ret;
    }
    return ret;
}

#if 0
int dctlEPFlashForceSnap(int iCarRoadId, int iStep, int iSnapType, int iCarDir, int iComId)
{
    int ret =0;
    SNAP_INFO  strSnapInfo;
    memset(&strSnapInfo, 0, sizeof(strSnapInfo));

    if( iCarRoadId >= g_uiClientLaneSum )
    {
        SYSHANDLE_INFO("lane is big,return!\n");
        return -1;
    }

    if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] )
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  SNAP_FLASH_FLAG;
    }
    else
    {
        strSnapInfo.laneInfo[0].uiFlashFlg =  0;
    }
    strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
    strSnapInfo.laneInfo[0].ucSnapType = iSnapType;
    if(SNAP_TYPE_3_PLUS_1 == iSnapType)
    {
        strSnapInfo.laneInfo[0].ucStepNum =3;
    }
    else if(SNAP_TYPE_3_SEPARATE == iSnapType)
    {
        strSnapInfo.laneInfo[0].ucStepNum =3;
    }
    else if(SNAP_TYPE_6_PLUS_1 == iSnapType)
    {
        strSnapInfo.laneInfo[0].ucStepNum =6;
    }
    else if(SNAP_TYPE_CLIP == iSnapType)
    {
        /* ���Ӿ����̵�ץ��ʱ������ʹ������� */
        strSnapInfo.laneInfo[0].uiFlashFlg = 0;
        strSnapInfo.laneInfo[0].ucRoadId = iCarRoadId;
        strSnapInfo.laneInfo[0].ucSnapType = iSnapType;
        strSnapInfo.laneInfo[0].ucStepNum =1;
        strSnapInfo.laneInfo[0].ucCurrentStep = 1;
        ret = dctlModuleFlashForceSnap(&strSnapInfo);
        g_jpegSnapCountInfo[iCarRoadId].uiAppForceSnapFullGSum +=1;
        return ret;
    }
    else
    {
        SYSHANDLE_ERROR("ep snap type:%d error\n", iSnapType);
        ret =-1;
        return ret;
    }
    strSnapInfo.laneInfo[0].ucCurrentStep = (unsigned char)iStep;

    ret = dctlModuleFlashForceSnap(&strSnapInfo);
    if(strSnapInfo.laneInfo[0].ucStepNum == iStep)
    {
        g_jpegSnapCountInfo[iCarRoadId].uiAppForceSnapFullGSum +=1;
    }


    return ret;
}
#endif

/******************************************************************************
 * dctlEPFlashConf() :
 * 
 * DESCRIPTION:  �ú�����Ҫ���������ã���һ�����ÿͻ����޸�����ģʽʱ��������.�ڶ���
   ���������⵽������ⲿͬ���źŲ���ʱ����Ҫ�л���±���ģʽ�Է�ֹ��ץ��ͼƬ��
 * Input: cModeΪ0ʱ�����ÿͻ��˲�����Чģʽ,����ֵΪ�Զ��л�ģʽ
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.09.08, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlEPFlashConf(char cMode, unsigned short usType)
{
    int i =0;
    int k =0;
    unsigned int j =0;
    int ret =0;
    FLASH_CONF_INFO  strFlashInfo;
    TTL_FLASH_CONF_INFO strTtlFlashConfInfo;
    
    ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
    ELEC_POLICE_VEHICLE_CONF_INFO strEPLaneConf;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;

    unsigned short usTmpFlashAddr[3];
    unsigned short usFlashSum =0;

    memset(&strFlashInfo, 0x00, sizeof(strFlashInfo));
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if ( 0== ret )
	{
	    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	    for(k =0; k< 3; k++)
	    {
            strTtlFlashConfInfo.szTTLTrigMode[k] = strEPGlobalConf.aucFreqFlashEn[k];

#if 0
            /* ����л�ʹ���ˣ���Ҫ�ɵ�ǰ�л�״̬�������Ƿ�ʹ��Ƶ������ */
            if(( FLASH_FILLIN_DAY_TO_NIGHT == g_iEpPreSta ) && ( k == 0 ))
            {
                if( strTtlFlashConfInfo.szTTLTrigMode[k] ==0 )
                {
                    strTtlFlashConfInfo.szTTLTrigMode[k] = TTL_FLASH_STROBE;
                }
            }
#else
			/* Ƶ���ƹ̶���1�ڣ�����л�ʹ���ˣ���Ҫ�ɵ�ǰ�л�״̬�������Ƿ�ʹ��Ƶ������ */
            if( strTtlFlashConfInfo.szTTLTrigMode[k] == TTL_FLASH_STROBE )
            {
            	if((strEPGlobalConf.ucTimerSwitchEn > 0) || ( strEPGlobalConf.ucSmartSwitchEn > 0 ) )
				{
                	if(FLASH_FILLIN_NIGHT_TO_DAY == g_iEpPreSta ) 
                	{
                		strTtlFlashConfInfo.szTTLTrigMode[k] = TTL_FLASH;
                	}
               	}
            }
#endif
            SYSHANDLE_INFO("ep index:%d TTlTrigMode:%d;\n", k, strTtlFlashConfInfo.szTTLTrigMode[k]);
            
            strTtlFlashConfInfo.szTTLTrigLevel[k]= strEPGlobalConf.aucFlashTrigMode[k];
            strTtlFlashConfInfo.szTTLMpll[k]= strEPGlobalConf.aucStrobeFlashFre[k];
            if( TTL_FLASH_STROBE == strTtlFlashConfInfo.szTTLTrigMode[k] )
            {
                strTtlFlashConfInfo.szTTLStrobeTime[k] = strSysFuncCfg.usFreqFlashDelayUSec;
            }
            else
            {
                strTtlFlashConfInfo.szTTLStrobeTime[k] = strSysFuncCfg.usepFLightDelayUSec;
            }
            strTtlFlashConfInfo.szStrobStartLine[k] = strSysFuncCfg.usFreqFlashStartLine;
            strTtlFlashConfInfo.szTTLTrigStartLine[k] = strSysFuncCfg.usFlashStartLine;
            strTtlFlashConfInfo.szTTLFlashEnable[k] = 1;

            if( FLASH_AC_DAY_TO_NIGHT == cMode )
            {
                strTtlFlashConfInfo.szTTLMACSync[k] = strEPGlobalConf.usNightACPhase;
            }
            else
            {
                strTtlFlashConfInfo.szTTLMACSync[k] = strEPGlobalConf.usDayACPhase;
            }
            SYSHANDLE_INFO("ep num:%d ac sync phase:%d.\n", k, strTtlFlashConfInfo.szTTLMACSync[k]);

			/* ��¼����ϪЭ����Ϣͳ�ƽṹ���� */
			if( TTL_FLASH_STROBE == strTtlFlashConfInfo.szTTLTrigMode[k] )
			{
				g_strCXProInfo.cFillLightSta[k] = 1;
			}
			else
			{
				g_strCXProInfo.cFillLightSta[k] = 0;
			}

        }
        dctlModuleTtlFlashCtlInfo(&strTtlFlashConfInfo); // tangzl 0725

	    if(FLASH_DYNA_SET_MODE == cMode)
        {
            /* �����л���Ĳ��ⷽʽ */
            for(j =0; j < g_uiClientLaneSum; j++)
            {
                memset(&strEPLaneConf, 0x00, sizeof(strEPLaneConf));
                SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneConf, j);
                strEPLaneConf.cInLightSta = usType;
                g_EPLaneSwitchInfo[j].cInLightSta = usType;
	            SysConfSet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneConf, j);
            }

            /* ���ⷽʽ��ֵ��ȫ�ֱ���ʹ�� */
            for(j =0; j < g_uiClientLaneSum; j++)
            {
                memset(&strEPLaneConf, 0x00, sizeof(strEPLaneConf));
                SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneConf, j);
                if( LIGHT_TYPE_FLASH == usType )
                {
                    g_iWorkFlashMode[j] = strEPLaneConf.usFlashLightType;
                }
                else
                {
                    g_iWorkFlashMode[j] = LIGHT_TYPE_HALGON;
                }
            }
        }
        else
        {
            for(j =0; j < g_uiClientLaneSum; j++)
            {
                memset(&strEPLaneConf, 0x00, sizeof(strEPLaneConf));
                SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneConf, j);
                g_iWorkFlashMode[j] = strEPLaneConf.usFlashLightType;
            }
        }
        
        strFlashInfo.uiChannelNum = g_uiClientLaneSum;

        /* ���flash��ַΪ0xff�����ʾ����Ч��ַ����Ҫ��������flash��Ч��ַ */
        usFlashSum =0;
        for(j =0; j < 3; j++)
        {
            if( strEPGlobalConf.ausFlashAddr[j] != 0xff )
            {
                usTmpFlashAddr[usFlashSum] = strEPGlobalConf.ausFlashAddr[j];
                usFlashSum++;
            }
        }
        
        for ( j = 0; j < strFlashInfo.uiChannelNum; j++)
        {
            if( strEPGlobalConf.usFlashLightCtlType == CONNECT_TYPE_COM )
    	    {
    		    strFlashInfo.strFlashConf[j].ucTrigMode = FLASH_TRIG_MODE_RS485;
    	    }
    	    else
    	    {
        	    strFlashInfo.strFlashConf[j].ucTrigMode = FLASH_TRIG_MODE_GPIO;
    	    }
            strFlashInfo.strFlashConf[j].ucFlashType = strEPGlobalConf.cFlashCtlMode;
            strFlashInfo.strFlashConf[j].ucEnable = 1;
   	        strFlashInfo.strFlashConf[j].ucFlashNum = usFlashSum;
            for(i =0; i< usFlashSum; i++)
            {
        	    strFlashInfo.strFlashConf[j].szFlashAddr[i] = usTmpFlashAddr[i];
        	    SYSHANDLE_INFO("ep flash add_%d:0x%x.\n", i, strFlashInfo.strFlashConf[j].szFlashAddr[i]);
            }
        }
        
        dctlModuleFlashCtlInfo(&strFlashInfo);	// tangzl 0725
	}
	return 0;
}

/******************************************************************************
 * dctlPPFlashConf() :
 * 
 * DESCRIPTION:  �ú�����Ҫ���������ã���һ�����ÿͻ����޸�����ģʽʱ��������.�ڶ���
   ���������⵽������ⲿͬ���źŲ���ʱ����Ҫ�л���±���ģʽ�Է�ֹ��ץ��ͼƬ��
 * Input: cModeΪ0ʱ�����ÿͻ��˲�����Чģʽ,����ֵΪ�Զ��л�ģʽ,
    ��ΪFLASH_DYNA_SWITCH_MODE ģʽ�������ⷽʽ���ģ�����ַ��һ���л�
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.09.08, wangbin Create
 * --------------------
 ******************************************************************************/
int dctlPPFlashConf(char cMode, unsigned short usType)
{
    unsigned int j =0;
    int ret =0;
    unsigned char ucTmpData =0;
    FLASH_CONF_INFO  strFlashInfo;
    PORT_VEHICLE_CONF_INFO strPPCarRoadConf;
    TTL_FLASH_CONF_INFO strTtlFlashConfInfo;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    Lane_Flash_Port_t strDspLaneFlashAdd;

    PASSPORT_OVERALL_CONF_INFO strPPOverAllConf;
    
    memset(&strTtlFlashConfInfo, 0, sizeof( strTtlFlashConfInfo ));
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPOverAllConf, 0);
    if( 0 == ret)
    {
        ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
        for(int i=0; i< 3; i++)
        {
            strTtlFlashConfInfo.szTTLTrigMode[i]  = strPPOverAllConf.aucFreqFlashEn[i];
#if 0
            /* ����л�ʹ���ˣ���Ҫ�жϵ�ǰ�л�״̬�������Ƿ�ʹ��Ƶ������ */
            if(( FLASH_FILLIN_DAY_TO_NIGHT == g_iPreSta ) && ( i == 0 ))
            {
                if( strTtlFlashConfInfo.szTTLTrigMode[i] ==0 )
                {
                    strTtlFlashConfInfo.szTTLTrigMode[i] =1;
                }
            }
#else
			/* Ƶ���ƹ̶���1�ڣ�����л�ʹ���ˣ���Ҫ�ɵ�ǰ�л�״̬�������Ƿ�ʹ��Ƶ������ */
            if( strTtlFlashConfInfo.szTTLTrigMode[i] == TTL_FLASH_STROBE )
            {
            	if((strPPOverAllConf.ucTimerSwitchEn > 0) || ( strPPOverAllConf.ucSmartSwitchEn > 0 ) )
				{
                	if(FLASH_FILLIN_NIGHT_TO_DAY == g_iPreSta )
                	{
                		strTtlFlashConfInfo.szTTLTrigMode[i] = TTL_FLASH;
                	}
               	}
            }
#endif            
            SYSHANDLE_INFO("pp index:%d TTlTrigMode:%d;\n", i, strTtlFlashConfInfo.szTTLTrigMode[i]);
            
            strTtlFlashConfInfo.szTTLTrigLevel[i] = strPPOverAllConf.aucFlashTrigMode[i];
            strTtlFlashConfInfo.szTTLMpll[i] = strPPOverAllConf.aucStrobeFlashFre[i];
            if( TTL_FLASH_STROBE == strTtlFlashConfInfo.szTTLTrigMode[i] )
            {
                strTtlFlashConfInfo.szTTLStrobeTime[i]= strSysFuncCfg.usFreqFlashDelayUSec;
            }
            else
            {
                strTtlFlashConfInfo.szTTLStrobeTime[i]= strSysFuncCfg.usppFLightDelayUSec;
            }
            strTtlFlashConfInfo.szStrobStartLine[i] = strSysFuncCfg.usFreqFlashStartLine;
            strTtlFlashConfInfo.szTTLTrigStartLine[i] = strSysFuncCfg.usFlashStartLine;
            strTtlFlashConfInfo.szTTLFlashEnable[i] = 1;
        }
        dctlModuleTtlFlashCtlInfo(&strTtlFlashConfInfo);	// tangzl 0725
    }
    
    memset(&strFlashInfo, 0, sizeof(strFlashInfo));
    memset(&strPPCarRoadConf, 0, sizeof(strPPCarRoadConf));
    for(j =0; j < g_uiClientLaneSum; j++)
    {
    	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPCarRoadConf, j);
    	if ( 0== ret )
    	{
            /* ������Ƶ�ַ���ø�dsp�������������ٶ� */
            strDspLaneFlashAdd.step1port = strPPCarRoadConf.ausFlashAddr[0];
            strDspLaneFlashAdd.step2port = strPPCarRoadConf.ausFlashAddr[1];
        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            // dsp_service_flash_port_set(0, j, &strDspLaneFlashAdd);
			SYSHANDLE_ERROR("dsp_service_flash_port_set method not supported!");
		#endif
    	    if(FLASH_DYNA_SET_MODE == cMode)
            {
                /* �����л���Ĳ��ⷽʽ */
                strPPCarRoadConf.cInLightSta = usType;
                g_PPLaneSwitchInfo[j].cInLightSta = usType;
                SysConfSet(e_PASSPORT_VEHICLE_CONF, &strPPCarRoadConf, j);

                /* ���ⷽʽ��ֵ��ȫ�ֱ���ʹ�� */
                /* �����жϻָ�����Ҫ��ԭΪ�û����ò��� */
                if( LIGHT_TYPE_FLASH == usType )
                {
                    g_iWorkFlashMode[j] = strPPCarRoadConf.usFlashLightType;
                }
                else
                {
                    g_iWorkFlashMode[j] = LIGHT_TYPE_HALGON;
                }
            }
            else
            {
                g_iWorkFlashMode[j] = strPPCarRoadConf.usFlashLightType;
            }

    	    strFlashInfo.uiChannelNum = g_uiClientLaneSum;
    	    strFlashInfo.strFlashConf[j].ucFlashType = strPPCarRoadConf.cFlashCtlMode;
    	    if( strPPCarRoadConf.usFlashLightCtlType == CONNECT_TYPE_COM )
    	    {
    		    strFlashInfo.strFlashConf[j].ucTrigMode = FLASH_TRIG_MODE_RS485;
    	    }
    	    else
    	    {
        	    strFlashInfo.strFlashConf[j].ucTrigMode = FLASH_TRIG_MODE_GPIO;
    	    }
    	    strFlashInfo.strFlashConf[j].ucEnable = 1;
            strFlashInfo.strFlashConf[j].ucFlashNum =2;

            /* �����������Ȧ����ֱ�ӷ��� */
            if( g_PPLaneSwitchInfo[j].cCoilSwitchSta == SWITCH_DEFAULT_VAL)
            {
                if( GROUD_LOOP == g_PPLaneSwitchInfo[j].cCoilDecType)
                {
                    strFlashInfo.strFlashConf[j].szFlashAddr[0] = strPPCarRoadConf.ausFlashAddr[0];
            	    strFlashInfo.strFlashConf[j].szFlashAddr[1] = strPPCarRoadConf.ausFlashAddr[1];
            	    SYSHANDLE_INFO("lane:%d,pp1 flash add0:0x%x.\n", j, strFlashInfo.strFlashConf[j].szFlashAddr[0]);
                }
                else
                {
                    /* ����������Ȧץ��ֻ��һ��ͼƬ���ڵظ���Ȧ�л�Ϊ������Ȧʱ����Ҫʹ�õڶ�����ַ���Ա�֤����
            	    ������ */
            	    strFlashInfo.strFlashConf[j].szFlashAddr[0] = strPPCarRoadConf.ausFlashAddr[1];
            	    strFlashInfo.strFlashConf[j].szFlashAddr[1] = strPPCarRoadConf.ausFlashAddr[0];
            	    SYSHANDLE_INFO("lane:%d,pp2 flash add0:0x%x.\n", j, strFlashInfo.strFlashConf[j].szFlashAddr[0]);
                }
            }
            else
            {
                if( GROUD_LOOP == g_PPLaneSwitchInfo[j].cCoilSwitchSta)
                {
                    strFlashInfo.strFlashConf[j].szFlashAddr[0] = strPPCarRoadConf.ausFlashAddr[0];
            	    strFlashInfo.strFlashConf[j].szFlashAddr[1] = strPPCarRoadConf.ausFlashAddr[1];
            	    SYSHANDLE_INFO("lane:%d,pp3 flash add0:0x%x.\n", j, strFlashInfo.strFlashConf[j].szFlashAddr[0]);
                }
                else
                {
                    /* ����������Ȧץ��ֻ��һ��ͼƬ���ڵظ���Ȧ�л�Ϊ������Ȧʱ����Ҫʹ�õڶ�����ַ���Ա�֤����
            	    ������ */
            	    strFlashInfo.strFlashConf[j].szFlashAddr[0] = strPPCarRoadConf.ausFlashAddr[1];
            	    strFlashInfo.strFlashConf[j].szFlashAddr[1] = strPPCarRoadConf.ausFlashAddr[0];
            	    SYSHANDLE_INFO("lane:%d, pp4 flash add0:0x%x.\n", j, strFlashInfo.strFlashConf[j].szFlashAddr[0]);
                }
            }

            if( FLASH_FILLIN_DAY_TO_NIGHT == cMode )
            {
                ucTmpData = strFlashInfo.strFlashConf[j].szFlashAddr[0];
                strFlashInfo.strFlashConf[j].szFlashAddr[0] = strFlashInfo.strFlashConf[j].szFlashAddr[1];
            	strFlashInfo.strFlashConf[j].szFlashAddr[1] = ucTmpData;
            }
            
    	    SYSHANDLE_INFO("lane:%d,send kernel pp flash add0:0x%x.\n", j, strFlashInfo.strFlashConf[j].szFlashAddr[0]);
    	    SYSHANDLE_INFO("lane:%d,send kernel pp flash add1:0x%x.\n", j, strFlashInfo.strFlashConf[j].szFlashAddr[1]);
    	}
    	else
    	{
    	    SYSHANDLE_ERROR("get pp conf error.\n");
            return -1;
    	}
    }
    dctlModuleFlashCtlInfo(&strFlashInfo);	// tangzl 0725

    return 0;
}

int debugShellLogSet(void)
{
    int ret =0;
    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
    if(0 == ret)
    {
        initShellDebug(strSysFuncCfg.ucDebugLogSaveFlag, 0);
        NET_DEV_GET_PRINT_LEVEL(4, strSysFuncCfg.ucDebugLogSaveFlag);
    }
    return 0;
}

int devCtlParamConf()
{
    int i =0;
    int ret =0;
    DCTL_CAP_COM_CONF strCapInfo[MAX_COM_DEV];
    DCTL_CONF_INFO strDctlInfo;
    memset(strCapInfo, 0, sizeof(strCapInfo));
    for(i =0; i< MAX_COM_DEV; i++)
    {
        ret = SysConfGet(e_DCTL_CONF, &strDctlInfo, i);
        if ( 0 == ret )
        {
            SYSHANDLE_ERROR("paramConf index%d enable:%d,vehilce type:%d.\n", i, strDctlInfo.cEnable, strDctlInfo.uiVehicleType);
            strCapInfo[i].bEnable = strDctlInfo.cEnable;
            if(strDctlInfo.cEnable > 0)
            {
                g_aiVehicleType[i] = strDctlInfo.uiVehicleType;
                strCapInfo[i].usComPort = strDctlInfo.cComId;
                strCapInfo[i].usCapDevType = strDctlInfo.uiVehicleType;
                if(DEV_TYPE_HGD == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = hagdDataPro;
                }
                else if(DEV_TYPE_KJ == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = kjDataPro;
                }
                else if(DEV_TYPE_BEST == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = kjDataPro;
                }
                else if(DEV_TYPE_SJ == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = sjDataPro;
                }
                else if(DEV_TYPE_DC == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = dcDataPro;
                }
				else if(DEV_TYPE_CS == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = csDataPro;
                }
                else if(DEV_TYPE_AD == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = adDataPro;
                }
                else if(DEV_TYPE_LD == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = ldDataPro;
                }
				else if(DEV_TYPE_HC == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = hcDataPro;
                }
                else if(DEV_TYPE_WK == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = wkDataPro;
                }
                else if(DEV_TYPE_OLVIA == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = olviaDataPro;
                }
                else if(DEV_TYPE_CSMC == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = csMcDataPro;
                }
                else if(DEV_TYPE_CY == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = kjDataPro;
                }  
                else if(DEV_TYPE_CYLD == strDctlInfo.uiVehicleType)
                {
                    strCapInfo[i].pProcFunc = cyldDataPro;
                }
                else
                {
                    SYSHANDLE_ERROR("DCTL vehicle type:0x%x is error\n", strDctlInfo.uiVehicleType);
                    ret =-1;
                    return ret;
                }
            }
            else
            {
                g_aiVehicleType[i] = 0;
            }
        }
        else
        {
            SYSHANDLE_ERROR("DCTL read com:%d conf attri error\n", i);
        }
    }
    SYSHANDLE_ERROR("DCTL cap init is here\n");
    dCtlModuleCapConf(strCapInfo, MAX_COM_DEV);

	if(DEV_TYPE_OLVIA == strDctlInfo.uiVehicleType)
    {
  		/* ����olvia���Ͳ�ѯ��ʱ�� */
        olviaSendInit();
    }
    else
    {
    	/* �ر�olvia��ѯ��ʱ�� */
    	olviaSendUnIn();
    }
    return 0;
}


int devCtlInit(void)
{
    DCTL_MODE_DEV_INFO strDctlModeDevInfo;

#if defined(CYC200XZD)||defined(CYC500XZD)
    DCTL_COM_INFO aComDevName[MAX_COM_NUM] ={"/dev/ttyO1","/dev/misc/arm2fpga"};
#else
    DCTL_COM_INFO aComDevName[MAX_COM_NUM] ={"/dev/ttyO1","/dev/ttyO2"};
#endif

    DCTL_COM_INFO aAlarmInDevName[1] ={"/dev/misc/alarmin"};

    DCTL_COM_INFO aGpioOutDevName[1] ={"/dev/misc/alarmout"};

    DCTL_COM_INFO aFlashCtlDevName[1] ={"/dev/misc/tgvd"};

    memset(&strDctlModeDevInfo, 0, sizeof(strDctlModeDevInfo));
    strDctlModeDevInfo.ucChanNum=1;
    strDctlModeDevInfo.ucComNum = MAX_COM_NUM;
    strDctlModeDevInfo.ucAlarmInNum= 1;
    strDctlModeDevInfo.ucGpioOutNum = 1;
    strDctlModeDevInfo.ucFlashCtlNum = 1;
    strDctlModeDevInfo.pComDevName = aComDevName;
    strDctlModeDevInfo.pAlarmInDevName = aAlarmInDevName;
    strDctlModeDevInfo.pGpioOutDevName = aGpioOutDevName;
    strDctlModeDevInfo.pFlashCtlDevName = aFlashCtlDevName;
    dctlModuleInit(&strDctlModeDevInfo);
    
    devCtlParamConf();
    
    /* �򿪱��������豸������ʼ���ձ������� */
    /* ���ڱ������뾭�����󱨣���ʱ�رոù��� */
    dCtlModuleAlarmInConf(dctlRedLightReport);

    dCtlModuleFlashCtlConf();
    return 0;
}



/* ϵͳ����������ģ�� */
int xcsSysReboot(void)
{
    int ret =0;
    /* Ϊ�˱�֤ϵͳ�����ܹ�д��flash�����Եȴ���һ��ʱ�� */
    mcLog(LMT_SYSCALL,LT_ERROR,"xcs reboot system!\n");
    sleep(5);
    ret = system("/sbin/wd_reboot");
    ret >>= 8; /* ��8λ��ʾ�ýű����յ����ź�ֵ����8λ�ű�ʾexit()���ص�ֵ����������8λ */
    system("reboot -f");
    return ret;
}

/* Զ�ָ̻�����Ĭ������ */
int xcsSysRestoreDevConf(void)
{
    int ret =0;
    /* Ϊ�˱�֤ϵͳ�����ܹ�д��flash�����Եȴ���һ��ʱ�� */
    mcLog(LMT_SYSCALL,LT_ERROR,"xcs restore dev conf!\n");
    sleep(1);
    ret = system("rm -rf /mnt/flash/*");
    ret >>= 8; /* ��8λ��ʾ�ýű����յ����ź�ֵ����8λ�ű�ʾexit()���ص�ֵ����������8λ */
    system("killall xcs xcsconf");
    return ret;
}

int xcsSysUpdate(unsigned char *pUrl)
{
    int ret =0;
    char acSystemFile[128];

    if(NULL == pUrl)
    {   
        return -1;
    }
    memset(acSystemFile, 0, sizeof(acSystemFile));
    shell_print("url is %s\n", pUrl);
	sprintf(acSystemFile, "%s %s", "/bin/sys_update", pUrl);
	shell_print("sysupdate is %s\n", acSystemFile);
	int pid = fork();
	if (pid == 0 )
	{
    	ret = execlp("/bin/sys_update","/bin/sys_update",pUrl);
    	shell_print("update exec failed .\n");
	}
    ret >>= 8; /* ��8λ��ʾ�ýű����յ����ź�ֵ����8λ�ű�ʾexit()���ص�ֵ����������8λ */
    return ret;
}

/******************************************************************************
 * sendMsgToMainApp() :
 * 
 * DESCRIPTION:  ��ģ��ͨ���ú��������̷߳�����Ϣ
 *  
 * Input: ģ��ţ���Ϣ���ͺţ����ͻ���ָ�룬���峤��
 * Output: 
 * Returns: -1:��ʾʧ�� ��������ʾ�ܷ��ͳ���
 * 
 * modification history
 * --------------------
 *01a, 2009.12.21, wangbin Create
 * --------------------
 ******************************************************************************/
int sendMsgToMainApp(STR_SYS_APP_MSG *pStrSysMsg)
{
    int ret =-1;
    if(NULL == pStrSysMsg)
    {
        SYSHANDLE_ERROR("in param is null\n");
        return -1;
    }
    SYSHANDLE_INFO("msg send moduleId:%d,msgType:%d\n", pStrSysMsg->uiModuleId, pStrSysMsg->uiMsgType);
    ret = msgQSend(pSysManageQueue, (char *)(pStrSysMsg), sizeof(STR_SYS_APP_MSG),  0);
    return ret;
}


void mainJpegSnapWayInfo()
{
    unsigned char i =0;
    unsigned char cCarSum =0;

    cCarSum =g_uiClientLaneSum;

    shell_print("ARM SNAP INFO:\n");
    shell_print("COM INFO: lossDataNum\t okFrameNum\t lackFrameNum\t throwFrameNum\n");
    shell_print("               %d\t            %d\t          %d\t           %d\n",
    g_jpegSnapCountInfo[i].uiComLoseDataNum, g_jpegSnapCountInfo[i].uiComFullFrameSun,\
    g_jpegSnapCountInfo[i].uiComLackFrameSun, g_jpegSnapCountInfo[i].uiComThrowFullFrameSun);
    shell_print("\n");
    for(i =0; i< cCarSum; i++)
    {
        shell_print("%d_lane info: vehicleSnapOKNum\t manualSnapOKNum\n", i);
        shell_print("                     %d\t               %d\n", \
        g_jpegSnapCountInfo[i].uiAppNormalSnapFullGSum, g_jpegSnapCountInfo[i].uiAppForceSnapFullGSum);
    }
}


/******************************************************************************
 * dspPPMSnap() :
 * 
 * DESCRIPTION:  �����ֶ�ץ�ĺ���
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.22, wangbin Create
 * --------------------
 ******************************************************************************/
int dspPPMSnap(int iCarRoadId, int iCarSpeed, int iCarDir)
{
    int ret =0;
    int iSnapType =0;

    if( g_PPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta == SWITCH_DEFAULT_VAL)
    {
        if( g_PPLaneSwitchInfo[iCarRoadId].cCoilDecType == GROUD_LOOP )
        {
            iSnapType = 1;
        }
        else
        {
            iSnapType = 0;
        }
    }
    else
    {
        if(g_PPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta == GROUD_LOOP)
        {
            iSnapType = 1;
        }
        else
        {
            iSnapType = 0;
        }
    }
    
    if(iSnapType > 0)
    {
        SYSHANDLE_INFO("lane:%d, bayonet hand snap car speed:%d, flash sta:%d.\n", \
        iCarRoadId, iCarSpeed, g_iWorkFlashMode[iCarRoadId]);
        
        if( LIGHT_TYPE_FLASH == g_iWorkFlashMode[iCarRoadId] )
        {
            dctlPPFlashNormalSnap(0, iCarRoadId, 1, 0, iCarDir);
            usleep(300000);
            dctlPPFlashNormalSnap(0, iCarRoadId, 2, iCarSpeed, iCarDir);
            usleep(70000);
            dctlPPFlashNormalSnap(0, iCarRoadId, 3, iCarSpeed, iCarDir);
        }
        else
        {
            dctlPPFlashForceSnap(0, iCarRoadId, 1, 0, iCarDir);
            usleep(300000);
            dctlPPFlashForceSnap(0, iCarRoadId, 2, iCarSpeed, iCarDir);
            usleep(70000);
            dctlPPFlashForceSnap(0, iCarRoadId, 3, iCarSpeed, iCarDir);
        }
    }
    else
    {
		FORCE_SNAP fsp;
		memset(&fsp, 0, sizeof(fsp));
		fsp.chan_id = 0;
		fsp.lane_id = iCarRoadId;
		fsp.iContinuSnapEn = 0;
		ret = NET_DEV_ManualSnap(&fsp, 0);
        if(0 != ret)
        {
            SYSHANDLE_ERROR("%s %d NET_DEV_ManualSnap failed, error:%d.\n",__FUNCTION__,__LINE__, NET_DVE_GetLastError());
            return ret;
        }
        else
        {
            SYSHANDLE_INFO("%s %d NET_DEV_ManualSnap virtual coil snap ok!\n",__FUNCTION__,__LINE__);
        }

        if(g_PPLaneSwitchInfo[iCarRoadId].cGroudCoilDecType == CS_RADAR_SPEED)
        {
            ret = dctlRadarNormalSnap(0, iCarRoadId, 1, iCarSpeed, iCarDir);
            if( 0 != ret)
        	{
        	    DSP_ERROR("dctlRadarNormalSnap ret:0x%x failed\n", ret);
        	}
        	else
        	{
        	    DSP_INFO("cs radar pp mSnap ok,speed is 0x%x!\n", iCarSpeed);
        	}
        }

    }
    return 0;
}



/******************************************************************************
 * dspEPMSnap() :
 * 
 * DESCRIPTION:  ���Ӿ����ֶ�ץ�ĺ���
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.1.12, wangbin Create
 * --------------------
 ******************************************************************************/
int dspEPMSnap(int iCarRoadId)
{
SYSHANDLE_ERROR("#######dfffffffffffffffffffffffff###### iCarRoadId :%d\n",iCarRoadId);


#if 1
#if defined(CYC500XKW4C)||defined(CYC500XAW)
SYSHANDLE_INFO("#######1223456677889990900000000 iCarRoadId :%d\n",iCarRoadId);
    int ret =0;
    int iPicMixType =0;
    unsigned long  ulCurTime =0;
    int iSnapType =0;
    iPicMixType = dspEpSnapTypeGet(0);
    SYSHANDLE_INFO("dspEPMSnap,laneId:%d,picMixType:%d,flashType:%d\n", iCarRoadId, iPicMixType, g_iWorkFlashMode[iCarRoadId]);
    if(g_ucEPComSnapFlag)
    {
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv,&tz);
        ulCurTime = tv.tv_sec*1000 + tv.tv_usec/1000;
        /* ���ظ�ץ��һֱû����ɣ���ʱ90����϶�Ϊ��Ч�����Դ����ֶ�ץ�ģ�������ظ�ץ��״̬�� */
        if( (ulCurTime - g_ulOrgTime) > 90*1000 )
        {
            g_ucEPComSnapFlag =0;
        }
        else
        {
            SYSHANDLE_ERROR("lane snap is doning,hand snap throw!\n");
            return 0;
        }
    }
    /* ����֪ͨDSPȡһ��ʱ�䣬û��Ĺ��� */
    setSimSnapRedTime(iCarRoadId);
    g_ucEPHandSnapFlag =1;

    SYSHANDLE_INFO("coil switch sta:%d,dec type:%d\n", g_EPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta,\
    g_EPLaneSwitchInfo[iCarRoadId].cCoilDecType);
    
    if( g_EPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta == SWITCH_DEFAULT_VAL)
    {
        if( g_EPLaneSwitchInfo[iCarRoadId].cCoilDecType == GROUD_LOOP )
        {
            iSnapType = 1;
        }
        else
        {
            iSnapType = 0;
        }
    }
    else
    {
        if(g_EPLaneSwitchInfo[iCarRoadId].cCoilSwitchSta == GROUD_LOOP)
        {
            iSnapType = 1;
        }
        else
        {
            iSnapType = 0;
        }
    }

    SYSHANDLE_INFO("manual snap type is:%d!\n", iSnapType);
    
    if(iSnapType > 0)
    {
        if((SNAP_TYPE_3_PLUS_1 == iPicMixType) || (SNAP_TYPE_3_PLUS_1_LEFT == iPicMixType) )
        {
            dctlEPFlashNormalSnap(iCarRoadId, 1, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 2, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 3, iPicMixType);
            sleep(1);
        }
        else if(SNAP_TYPE_3_SEPARATE == iPicMixType)
        {
            dctlEPFlashNormalSnap(iCarRoadId, 1, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 2, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 3, iPicMixType);
            sleep(1);
        }
        else if(SNAP_TYPE_4_SEPARATE == iPicMixType)
        {
            dctlEPFlashNormalSnap(iCarRoadId, 1, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 2, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 3, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 4, iPicMixType);
            sleep(1);
        }
        else if(SNAP_TYPE_6_PLUS_1 == iPicMixType)
        {
            dctlEPFlashNormalSnap(iCarRoadId, 1, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 2, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 3, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 4, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 5, iPicMixType);
            sleep(1);
            dctlEPFlashNormalSnap(iCarRoadId, 6, iPicMixType);
            sleep(1);
        }

        /* �����ֶ�ץ�Ŀ���ͼƬ���� */
        if(epPPPicSendFlag() == 1)
        {
            dctlEPFlashNormalSnap(iCarRoadId, 1, SNAP_TYPE_CLIP);
        }
    }
    else
    {
		FORCE_SNAP fsp;
		memset(&fsp, 0, sizeof(fsp));
		fsp.chan_id = 0;
		fsp.lane_id = iCarRoadId;
		fsp.iContinuSnapEn = 0;
		ret = NET_DEV_ManualSnap(&fsp, 1);
        if( 0 != ret )
        {
            SYSHANDLE_ERROR("dsp virtual simu snap failed, error:%d\n", NET_DVE_GetLastError());
        }
        else
        {
            SYSHANDLE_INFO("dsp virtual simu snap is ok\n");
        }
    }
    g_ucEPHandSnapFlag =0;

    
    #endif
    return 0;
#endif
}


/******************************************************************************
 * getTrafficLightSta() :
 * 
 * DESCRIPTION:  ��ȡָ�������Ľ�ͨ��״̬
 *  
 * Input: ����ĳ����ű����ǿͻ����ϵĳ����Ŷ����ǳ������ϵĳ�����
 * Output: 
 * Returns: -1��ʾ����0��ʾ��� 1��ʾ�Ǻ��(��ʱ����)
 * 
 * modification history
 * --------------------
 *01a, 2009.12.19, wangbin Create
 * --------------------
 ******************************************************************************/
char getTrafficLightSta(unsigned int uiCarRoadId)
{
	if ( uiCarRoadId <= g_uiClientLaneSum )
	{
	    SYSHANDLE_INFO("traffic light,lane id:%d normal sta:%d,force sta:%d!\n",\
	    uiCarRoadId, g_EPLaneSwitchInfo[uiCarRoadId].cRedLSta, g_bForceRed[uiCarRoadId]);
		return (g_EPLaneSwitchInfo[uiCarRoadId].cRedLSta || g_bForceRed[uiCarRoadId]);
	}
	else
	{
	    SYSHANDLE_ERROR("error:lane is too big!\n");
		return (char)(-1);
	}
}

/******************************************************************************
 * setSimSnapRedTime() :
 * 
 * DESCRIPTION:  �����Ϊģ��ץ������Ҫ�ڿ�ʼ��ʱ��֪ͨDSPȥ��ȡһ�º��ʱ��
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.28, wangbin Create
 * --------------------
 ******************************************************************************/
int setSimSnapRedTime(unsigned char ucLaneId)
{
	if ( ucLaneId <= g_uiClientLaneSum )
	{
	    SYSHANDLE_INFO("set sim red light time,lane id:%d normal sta:%d,lane id:%d,force sta:%d\n", \
	    ucLaneId, g_EPLaneSwitchInfo[ucLaneId].cRedLSta, ucLaneId, g_bForceRed[ucLaneId]);
        if(( g_EPLaneSwitchInfo[ucLaneId].cRedLSta == RED_LIGHT_OFF ))
        {
            if( g_bForceRed[ucLaneId] == RED_LIGHT_ON )
            {
                /* ����֪ͨDSPȡһ��ʱ�䣬û��Ĺ��� */
                dspRedLightTimeSet(ucLaneId, RED_LIGHT_ON);
            }
		    return 0;
		}
	}
	else
	{
		return (char)(-1);
	}
    return 0;
}

/******************************************************************************
 * redLightStaForceDspSet() :
 * 
 * DESCRIPTION:  ǿ�����ý�ͨ��״̬Ϊ��ƣ��Ա���������Ӿ���ץ��
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.8.17, wangbin Create
 * --------------------
 ******************************************************************************/
int redLightStaForceDspSet(unsigned int uiCarRoadId)
{
    int ret =0;

    /* ����֪ͨDSPȡһ��ʱ�䣬û��Ĺ��� */
    dspRedLightTimeSet(uiCarRoadId, 0);
    ret = dspSetRedInfo( uiCarRoadId);

    return ret;
}

/******************************************************************************
 * redLightStaForceSet() :
 * 
 * DESCRIPTION:  ǿ�����ý�ͨ��״̬Ϊ��ƣ��Ա��ڵ��Ӿ���ץ��
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.23, wangbin Create
 * --------------------
 ******************************************************************************/
int redLightStaForceSet(unsigned int uiCarRoadId, unsigned char ucSta)
{
    g_bForceRed[uiCarRoadId] = ucSta;
    return 0;
}

/******************************************************************************
 * redLightForceStaQuery() :
 * 
 * DESCRIPTION:  ǿ�ƽ�ͨ��Ϊ���״̬��ѯ
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.23, wangbin Create
 * --------------------
 ******************************************************************************/
unsigned char redLightForceStaQuery(unsigned int uiCarRoadId)
{
    return g_bForceRed[uiCarRoadId];
}

/******************************************************************************
 * redLightNormalStaQuery() :
 * 
 * DESCRIPTION:  ��ͨ���������״̬��ѯ
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.23, wangbin Create
 * --------------------
 ******************************************************************************/
unsigned char redLightNormalStaQuery(unsigned int uiCarRoadId)
{
    SYSHANDLE_INFO("%s red light info:\n \
    lane id:%d,red sta:%d;\n \
    type0 sta:%d,enable:%d;\n \
    type1 Sta:%d,enable:%d;\n \
    type2 Sta:%d,enable:%d;\n \
    type3 Sta:%d,enable:%d;\n\
    \n",\
    __FUNCTION__, uiCarRoadId, g_EPLaneSwitchInfo[uiCarRoadId].cRedLSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[0].cLSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[0].cEnable,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[1].cLSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[1].cEnable,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[2].cLSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[2].cEnable,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[3].cLSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].strTrafficL[3].cEnable);

    SYSHANDLE_INFO("lane id:%d;\n \
    coil dec type:%d;\n \
    coil switch flag:%d;\n \
    coil switch sta:%d;\n \
    inlight sta:%d; \n \
    inlight type:%d;\n \
    road dir:%d;\n", \
    uiCarRoadId,\
    g_EPLaneSwitchInfo[uiCarRoadId].cCoilDecType,\
    g_EPLaneSwitchInfo[uiCarRoadId].cCoilSwitchFlag,\
    g_EPLaneSwitchInfo[uiCarRoadId].cCoilSwitchSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].cInLightSta,\
    g_EPLaneSwitchInfo[uiCarRoadId].cInLightType,\
    g_EPLaneSwitchInfo[uiCarRoadId].cRoadDir);
    
    return g_EPLaneSwitchInfo[uiCarRoadId].cRedLSta;
}

/******************************************************************************
 * recSysConfSet() :
 * 
 * DESCRIPTION:  ����ϵͳ¼��ģ�����
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.25, wangbin Create
 * --------------------
 ******************************************************************************/
#ifdef MODULE_REC

int recSysConfSet()
{
	int ret =0;
    int iMsgSum =0;

    ret = checkmount(e_nfs);
	if ( 0 != ret )
	{
	    SYSHANDLE_ERROR("%s %d check nfs mount failed.ret:%d\n",__FUNCTION__,__LINE__, ret);
		return ret;
	}

    REC_CONF_INFO strRecConf;
    memset(&strRecConf, 0, sizeof(strRecConf));
	ret = SysConfGet(e_REC_CONF, &strRecConf, 0);
	if ( 0 != ret )
	{
		SYSHANDLE_ERROR("get rec conf failed .\n");
		return ret;
	}
    if(strRecConf.nStorageRecDays == 0)	
    {
        SYSHANDLE_ERROR("rec store day is 0.\n");
        return -1;
    }
	const unsigned int  APP_SHARE_MEM_SIZE = (0x0400000);/* ���֧�ּ�M���������ü�M */
	g_pRecData = new int[APP_SHARE_MEM_SIZE/4];
	if(NULL == g_pRecData)
	{
	    SYSHANDLE_ERROR("%s %d rec malloc failed .\n",__FUNCTION__,__LINE__);
	    return -1;
	}
	ret = RecInit(1);
	if (ret != 0)
	{
		SYSHANDLE_ERROR("%s %d init failed .\n",__FUNCTION__,__LINE__);
        goto goto_error;
	}

	/* ���õ���Ϣ���и���ԶԶ���ڻ���ռ���Ҫ�ģ���ֹ��Ϣ���и��������� */
    iMsgSum = ((APP_SHARE_MEM_SIZE*8)/0x100000) * 25*2;

    ret = SetBufInfo(0, iMsgSum, (char *)g_pRecData, APP_SHARE_MEM_SIZE);
    if(ret != 0)
    {
        SYSHANDLE_ERROR("%s,%s,%d\t SetBufInfo failed;\n", __FILE__, __FUNCTION__, __LINE__);
   		goto goto_error;
    }
    else
    {
        SYSHANDLE_ERROR("%s,%s,%d\t SetBufInfo ok;msgSum:%d,memSize:0x%x\n", \
                        __FILE__, __FUNCTION__, __LINE__,iMsgSum,APP_SHARE_MEM_SIZE);
    }

	POLICY_PARAM param;
	memset(&param, 0, sizeof(param));
	param.QuotaParam.QuotaDay = strRecConf.nStorageRecDays;
	SYSHANDLE_ERROR("%s,%s,%d\t rec quotaday is %d,planRecTime valid flag:%d\n", __FILE__, __FUNCTION__, __LINE__, param.QuotaParam.QuotaDay,\
	strRecConf.ptdDayPlanRecTime.bIsValid);
	//param.QuotaParam.QuotaSpace = strRecConf.nStorageMaxSpace;
	ret +=SetRecPolicy(0, POLICY_QUOTA, param);
	if (strRecConf.ptdDayPlanRecTime.bIsValid)
	{
		int i;
		for (i = 0;i<DEFAULT_PLANT_TIME_SEGMENT;++i)
		{
			if (strRecConf.ptdDayPlanRecTime.ts[i].uiStartTime != 0 || strRecConf.ptdDayPlanRecTime.ts[i].uiEndTime != 0 )
			{
				ret += AddRecCycleTime(0,0,strRecConf.ptdDayPlanRecTime.ts[i].uiStartTime,\
					strRecConf.ptdDayPlanRecTime.ts[i].uiEndTime);
				if(0 != ret )
				{
				    SYSHANDLE_ERROR("%s,%s,%d\t plan rec time is null!\n", __FILE__, __FUNCTION__, __LINE__);
               		goto goto_error;
				}
			    SYSHANDLE_ERROR("%s,%s,%d\t dayplan rec time\n", __FILE__, __FUNCTION__, __LINE__);
			}
		}
	}
	else
	{
	    SYSHANDLE_ERROR("%s,%s,%d\t plan rec time not enable!\n", __FILE__, __FUNCTION__, __LINE__);
   		goto goto_error;
	}
	#if 0
	int weekday = 0;
	for (weekday = 0;weekday < 7;++weekday)
	{
		if (strRecConf.ptdWeekPlanRecTime[weekday].bIsValid)
		{
			int i;
			for (i = 0;i<DEFAULT_PLANT_TIME_SEGMENT;++i)
			{
				if (strRecConf.ptdWeekPlanRecTime[weekday].ts[i].uiStartTime != 0 || strRecConf.ptdWeekPlanRecTime[weekday].ts[i].uiEndTime != 0 )
				{
					ret += AddRecCycleTime(0,weekday+1,strRecConf.ptdWeekPlanRecTime[weekday].ts[i].uiStartTime,\
						strRecConf.ptdWeekPlanRecTime[weekday].ts[i].uiEndTime);
			        SYSHANDLE_INFO("%s,%s,%d\t week plan rec time\n", __FILE__, __FUNCTION__, __LINE__);							
				}
			}
		}
	}
	if (strRecConf.ptdOncePlanRecTime.bIsValid)
	{
		int i;
		for (i = 0;i<DEFAULT_PLANT_TIME_SEGMENT;++i)
		{
			if (strRecConf.ptdOncePlanRecTime.ts[i].uiStartTime != 0 || strRecConf.ptdOncePlanRecTime.ts[i].uiEndTime != 0 )
			{
				ret += AddRecOnceTime(0,strRecConf.ptdOncePlanRecTime.ts[i].uiStartTime,\
					strRecConf.ptdOncePlanRecTime.ts[i].uiEndTime);
				    SYSHANDLE_INFO("%s,%s,%d\t oncetime plan rec time\n", __FILE__, __FUNCTION__, __LINE__);
			}
		}
	}
	#endif
	ret += AddRecPartition(0, "/ide/disk1", REC_NAS);
	ret += RecStart();
	return ret;

goto_error:
    free(g_pRecData);
	g_pRecData = NULL;

    return ret;
}

#endif /*  MODULE_REC*/

#ifdef MODULE_CIFS

int NetFsCheckInit()
{
    STR_NETFS_INFO  strNetFsInfo;
    NET_HDD_CONF_INFO strNetHddConf;
    int ret = 0;
    
    struct in_addr in;
    char tmp[128];

    memset(tmp,0x00,128);
    memset(&strNetFsInfo,0x00,sizeof(strNetFsInfo));
    
    ret = SysConfGet(e_PIC_NET_HDD_CONF,&strNetHddConf,0);
    if ( ret < 0)
    {
        return -1;
    }
    in.s_addr = strNetHddConf.uiIpAddr;
    in.s_addr = htonl(in.s_addr);
	sprintf(tmp,"%s",inet_ntoa(in));
    
    strncpy(strNetFsInfo.szIpAddr,tmp,sizeof(strNetFsInfo.szIpAddr));
    strncpy(strNetFsInfo.szRootPath,strNetHddConf.szFolderName,sizeof(strNetFsInfo.szRootPath));
    strNetFsInfo.bEnable = strNetHddConf.uiEnable;
    strNetFsInfo.iCheckInteval = 60;
    strNetFsInfo.strMsgId =pSysManageQueue;
    
    CifsStaCheckInit(&strNetFsInfo);

    memset(tmp,0x00,128);
    memset(&strNetFsInfo,0x00,sizeof(strNetFsInfo));
    ret = SysConfGet(e_REC_NET_HDD_CONF,&strNetHddConf,0);
    if ( ret < 0)
    {
        return -1;
    }
    in.s_addr = strNetHddConf.uiIpAddr;
    in.s_addr = htonl(in.s_addr);
	sprintf(tmp,"%s",inet_ntoa(in));
    
    strncpy(strNetFsInfo.szIpAddr,tmp,sizeof(strNetFsInfo.szIpAddr));
    strncpy(strNetFsInfo.szRootPath,strNetHddConf.szFolderName,sizeof(strNetFsInfo.szRootPath));
    strNetFsInfo.bEnable = strNetHddConf.uiEnable;
    strNetFsInfo.iCheckInteval = 60; /* ��λʱ��Ϊ�� */
    strNetFsInfo.strMsgId =pSysManageQueue;
    
    NfsStaCheckInit(&strNetFsInfo);
    return ret;
}

int NetFsProcess(STR_NETFS_STA  *pStrNetFsSta)
{
    int ret = 0;
    NET_HDD_CONF_INFO strNetHddConf;
    int iSta[MAX_VEHICLE_NUM] ={0};
    
    memset(&strNetHddConf, 0, sizeof(strNetHddConf));
    
    pStrNetFsSta->uiNetFsType &= 0x0f;
    
    if (NETFS_NFS == pStrNetFsSta->uiNetFsType )
    {
        ret = SysConfGet(e_REC_NET_HDD_CONF,&strNetHddConf,0);
        if ( ret < 0)
        {
            return -1;
        }
        if(NETFS_STA_OK == pStrNetFsSta->uistatus)
        {
            if( g_cRecInitSta <= 0 )
            {
                xcsRecInit();
                dspEncStaCtl(0, 1);
            }
            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_RECORD_NAS_STA, 0, 0, iSta);
            return 0;
        }
        else if(NETFS_OK2BAD == pStrNetFsSta->uistatus)
        {
            iSta[0] = MODE_ERROR_STA;
            nMMsgSend(NM_RECORD_NAS_STA, 0, 0, iSta);
            xcsSysReboot();
            return 0;
        }
        else if( NETFS_BAD2OK == pStrNetFsSta->uistatus )
        {
            xcsRecUninit();
            xcsRecInit();
            dspEncStaCtl(0, 1);

            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_RECORD_NAS_STA, 0, 0, iSta);
        }
    }
    else if ( NETFS_CIFS == pStrNetFsSta->uiNetFsType)
    {
        ret = SysConfGet(e_PIC_NET_HDD_CONF,&strNetHddConf,0);
        if ( ret < 0)
        {
            return -1;
        }
        if(NETFS_STA_OK == pStrNetFsSta->uistatus)
        {
            if(strNetHddConf.uiAvaiable == 0)
            {
                strNetHddConf.uiAvaiable =1;
                SysConfSet(e_PIC_NET_HDD_CONF,&strNetHddConf,0);
                ftpServerChange();
            }
            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_CIFS_NAS_STA, 0, 0, iSta);
            return 0;
        }
        else if(NETFS_OK2BAD == pStrNetFsSta->uistatus)
        {
            strNetHddConf.uiAvaiable = 0;
            SysConfSet(e_PIC_NET_HDD_CONF,&strNetHddConf,0);
            
            iSta[0] = MODE_ERROR_STA;
            nMMsgSend(NM_CIFS_NAS_STA, 0, 0, iSta);
            xcsSysReboot();
            return 0;
        }
        else if(NETFS_BAD2OK == pStrNetFsSta->uistatus)
        {
            strNetHddConf.uiAvaiable = 1;
            SysConfSet(e_PIC_NET_HDD_CONF,&strNetHddConf,0);
            
            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_CIFS_NAS_STA, 0, 0, iSta);
            ftpServerChange();
            return 0;
        }
    }
 
    return 0;
}

int NetFsUninit()
{
    int ret  = 0;
    ret = NfsStaCheckUninit();
    ret += CifsStaCheckUninit();
    return ret ;
}

#endif /* MODULE_CIFS */

int ntpDebugLevelSet(int iFlag)
{
	int ret =0;
    if ( NULL == g_NtpCheck)
    {
        return -1;
    }
	ret = g_NtpCheck->NtpSwitchDbg(iFlag);
	return ret;
}

int NtpInit(void )
{
    int ret = 0;
    STR_NTP_CLIENT  strNtpClient;
    NTP_CONF_INFO   strNtpConf;
    
    memset(&strNtpClient, 0, sizeof(strNtpClient));
    memset(&strNtpConf, 0, sizeof(strNtpConf));
    if ( NULL == g_NtpCheck )
    {
        g_NtpCheck = new CNtp(pSysManageQueue);
        if ( NULL == g_NtpCheck)
        {
            return -1;
        }
    }
    ret = SysConfGet(e_NTP_CONF, (void *)&strNtpConf, 0);

    if ( 0 != ret )
    {
        SYSHANDLE_ERROR("Get NtpConf failed\n");
        return ret;
    }
    strNtpClient.bEnable        = strNtpConf.uiEnable;
    strNtpClient.uiNtpServerIp  = strNtpConf.uiServerIP;
    strNtpClient.bForced        = false;
    strNtpClient.uiInterval     = strNtpConf.usJudgeCycle*60; /* ����Ϊ��λ */
    strNtpClient.usNtpPort      = 123;
    strNtpClient.usLocalPort    = 0;
    strNtpClient.uiTimeLag      = strNtpConf.ucJudegPrecision;
    
    ret = g_NtpCheck->NtpInit(&strNtpClient);
    if (ret)
        return 0 ;
    else
        return -1;
}

int ntpServerIpSet(void)
{
    int ret = 0;
    STR_NTP_CLIENT  strNtpClient;
    NTP_CONF_INFO   strNtpConf;

    memset(&strNtpClient, 0, sizeof(strNtpClient));
    memset(&strNtpConf, 0, sizeof(strNtpConf));    
    if ( NULL == g_NtpCheck )
    {
        return -1;
    }
    ret = SysConfGet(e_NTP_CONF, (void *)&strNtpConf, 0);
    if ( 0 != ret )
    {
        SYSHANDLE_ERROR("Get NtpConf failed\n");
        return ret;
    }
    strNtpClient.bEnable        = strNtpConf.uiEnable;
    strNtpClient.uiNtpServerIp  = strNtpConf.uiServerIP;
    strNtpClient.bForced        = false;
    strNtpClient.uiInterval     = strNtpConf.usJudgeCycle*60; /* ����Ϊ��λ */
    strNtpClient.usNtpPort      = 123;
    strNtpClient.usLocalPort    = 0;
    strNtpClient.uiTimeLag      = strNtpConf.ucJudegPrecision;
    
    ret = g_NtpCheck->NtpSetServer(&strNtpClient);
    if (ret)
        return 0 ;
    else
        return -1;
}

int NtpUninit()
{
    if ( NULL != g_NtpCheck)
    {
        delete g_NtpCheck;
        g_NtpCheck = NULL;
    }
    return 0;
    
}


int ntpEnFlagGet(void)
{
    int ret = 0;
    NTP_CONF_INFO   strNtpConf;

    memset(&strNtpConf, 0, sizeof(strNtpConf));
    if ( NULL == g_NtpCheck )
    {
        return -1;
    }
    ret = SysConfGet(e_NTP_CONF, (void *)&strNtpConf, 0);
    if ( 0 != ret )
    {
        SYSHANDLE_ERROR("Get NtpConf failed\n");
        return ret;
    }
    return strNtpConf.uiEnable;
}

int SysSetTime(unsigned int time, bool initDsp/* = true*/)
{
    int ret = 0;
    struct timeval tv;

    ret = SetRtc(time);

	tv.tv_sec = time;
	settimeofday(&tv, NULL);
	/* Ϊ�˷�ֹƴ�����ж�ʱ���¿��Ź��޷���ʱ����ι�����Ӷ�ϵͳ���������Խ���һ��ʱ����ӳ�
	   2010-03-10*/

#if defined(CYC200XZD)||defined(CYC500XZD)
    dm368SysTimeSet(time);
#endif

	if (initDsp) {
//		CALL_CYC_CMD(NET_DEV_SET_TIME, time);
	}
	sleep(2);
    return ret;
}

/* ���¶�ʱ������������� */
int sysCamTimeUpdate(void)
{
    int i =0;
    int ret =0;
    CAMERA_TIME_RANGE_INFO strSysCamaTimeRange;
 
    for(i =0; i< MAX_CAM_GROUP_NUM; i++)
    {
        memset(&strSysCamaTimeRange, 0, sizeof(strSysCamaTimeRange));
        ret = SysConfGet(e_CAMERA_TIME_RANGE, (void *)&strSysCamaTimeRange, i);
        if ( 0 != ret )
        {
            SYSHANDLE_ERROR("Get Cam time range  sys conf failed\n");
            return ret;
        }
        memcpy(&g_strCamTimeR[i], &strSysCamaTimeRange, sizeof(strSysCamaTimeRange));
    }
    return ret;
}

/******************************************************************************
 * camTimeRangeJudge() :
 * 
 * DESCRIPTION:  �жϵ�ǰʱ���������ĸ������ʱ�����,���ض����
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.08.26, wangbin Create
 * --------------------
 ******************************************************************************/
int camTimeRangeJudge(void)
{
    int i =0;
    unsigned int iSeconds =0;
    unsigned long ulPlanStart = time(NULL);
	struct tm tmPlanStart;
	ulPlanStart += 8*3600;
	gmtime_r((time_t *)&ulPlanStart, &tmPlanStart);
	iSeconds = tmPlanStart.tm_hour*3600 + tmPlanStart.tm_min*60;
	for(i =0; i< MAX_CAM_GROUP_NUM; i++)
	{
	    if(g_strCamTimeR[i].cEnable)
	    {
	        if((g_strCamTimeR[i].iBeginTime + g_strCamTimeR[i].iLastTime) > DATE_24_HOUR)
	        {
	            if( (iSeconds >= g_strCamTimeR[i].iBeginTime) && \
    	            (iSeconds <= DATE_24_HOUR ) )
    	        {
    	            SYSHANDLE_INFO("OK,Cam time range include this time:%d Sec\n", iSeconds);
    	            return g_strCamTimeR[i].cGroupId;
    	        }
    	        else if ( (iSeconds >= 0) && \
    	                  (iSeconds <= ((g_strCamTimeR[i].iBeginTime + g_strCamTimeR[i].iLastTime) - DATE_24_HOUR)) )
    	        {
    	            SYSHANDLE_INFO("OK2,Cam time range include this time:%d Sec\n", iSeconds);
    	            return g_strCamTimeR[i].cGroupId;
    	        }
    	        else
    	        {
    	            SYSHANDLE_INFO("failed,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
    	            g_strCamTimeR[i].iBeginTime,g_strCamTimeR[i].iLastTime);
    	        }
	        }
	        else
	        {
    	        if( (iSeconds >= g_strCamTimeR[i].iBeginTime) && \
    	            (iSeconds <= (g_strCamTimeR[i].iBeginTime + g_strCamTimeR[i].iLastTime) ) )
    	        {
    	            SYSHANDLE_INFO("OK3,Cam time range include this time:%d Sec\n", iSeconds);
    	            return g_strCamTimeR[i].cGroupId;
    	        }
    	        else
    	        {
    	            SYSHANDLE_INFO("failed,now t:%d,index_%d,bT:%d,eT:%d\n", iSeconds,i,\
    	            g_strCamTimeR[i].iBeginTime,g_strCamTimeR[i].iLastTime);
    	        }
    	    }
    	}
	}
	SYSHANDLE_INFO("Cam time range not include this time:%d Sec,default use group 1 param\n", iSeconds);
	return 0;
}

/******************************************************************************
 * camTimeRangeParamJudge() :
 * 
 * DESCRIPTION:  �жϵ�ǰ�����Ƿ�ͱ�Ĳ������ص�
 *  
 * Input: 
 * Output: 
 * Returns: ���ص��򷵻�-1,���ص��򷵻�0
 * 
 * modification history
 * --------------------
 *01a, 2010.08.26, wangbin Create
 * --------------------
 ******************************************************************************/
int camTimeRangeParamJudge(char cIndex, unsigned int uiBeginTime, unsigned int uiLastTime)
{
    int i =0;
    unsigned int uiEndTime =0;
    for(i =0; i< MAX_CAM_GROUP_NUM; i++)
	{
	    if( (g_strCamTimeR[i].cEnable) && (g_strCamTimeR[i].cGroupId != cIndex) )
	    {
	        uiEndTime = g_strCamTimeR[i].iBeginTime + g_strCamTimeR[i].iLastTime;
	        if(uiEndTime > DATE_24_HOUR)
	        {
	            /* ��ʼʱ�����Ѿ�������Ҫ����ֱ���˳��ˣ����Բ���Ӱ��Խ���ʱ����ж� */
    	        if( (uiBeginTime >= g_strCamTimeR[i].iBeginTime) && \
    	            (uiBeginTime <= DATE_24_HOUR) )
    	        {
    	            SYSHANDLE_INFO("Cam range_1 is error!\n");
    	            return -1;
    	        }
    	        else if((uiBeginTime >= 0) && ( uiBeginTime <= (uiEndTime - DATE_24_HOUR) ))
    	        {
    	            SYSHANDLE_INFO("Cam range_2 is error!\n");
    	            return -1;
    	        }
    	        
    	        if( ( (uiBeginTime + uiLastTime) >= g_strCamTimeR[i].iBeginTime ) && \
    	            ( (uiBeginTime + uiLastTime) <= uiEndTime )) 
         	    {
         	        SYSHANDLE_INFO("Cam range_3 is error!\n");
         	        return -1;
         	    }
    	    }
    	    else
    	    {
    	        if( (uiBeginTime >= g_strCamTimeR[i].iBeginTime) && \
    	            (uiBeginTime <= uiEndTime ) )
    	        {
    	            SYSHANDLE_INFO("Cam range_4 is error!\n");
    	            return -1;
    	        }
    	        if( ( (uiBeginTime + uiLastTime) >= g_strCamTimeR[i].iBeginTime ) && \
    	            ( (uiBeginTime + uiLastTime) <= uiEndTime ))
         	    {
         	        SYSHANDLE_INFO("Cam range_5 is error!\n");
         	        return -1;
         	    }
    	    }
    	}
	}
	return 0;
}

/* ����ʱ�ж��費��Ҫ����������������� */
int camCtlParamRunJudge(char cMode)
{
    char cIndex =0;
    if(CAM_PARAM_MODE_FORCE == cMode)
    {
        cIndex =camTimeRangeJudge();
        dspCamParamSet(cIndex);
        g_iCamParamGroupId = cIndex;
        syslog(LOG_ERR|LOG_USER, "force cam ctl param set,groupId:%d!\n", g_iCamParamGroupId);
    }
    else if( CAM_PARAM_MODE_NORMAL == cMode )
    {
        cIndex =camTimeRangeJudge();
        if(cIndex != g_iCamParamGroupId)
        {
            dspCamParamSet(cIndex);
            g_iCamParamGroupId = cIndex;
            syslog(LOG_ERR|LOG_USER, "normal cam ctl param set,groupId:%d!\n", g_iCamParamGroupId);
        }
    }
    return 0;
}



