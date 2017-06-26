/******************************************************************************
 * SysHandle.h:
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
#ifndef _SYS_HANDLE_H
#define _SYS_HANDLE_H

#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include "msgQLib.h"
#include "dsp.h"
#include "SysConf.h"
#include "xcsSysMsg.h"
#include "protoXcs.h"
#include "NetFsCheck.h"

#include "JpegSnap.h"
#include "FtpEpSend.h"
#include "FtpPpSend.h"

#include "dctlApi.h"
#include "vehicleProThread.h"

#include "dsp_service.h"

extern char g_cRecInitSta;
extern char g_cMmsInitSta;
extern char g_cFtpInitSta;
extern char g_cXcsInitSta;
extern char g_cDspInitSta;

/* ���������������ȷ���ĳ����� */
extern unsigned int g_uiCamLaneSum;

/* ���ÿͻ���ʵ�����õĳ����� */
extern unsigned int g_uiClientLaneSum;

/* �羯����СЭ��汾�� */
extern unsigned short g_usEpXcsProVer;

#define ORG_IMAG_PICK_OK        1
#define ORG_IMAG_PICK_NG        0

#define FLASH_NORMAL_SET_MODE   0
#define FLASH_DYNA_SET_MODE     1
#define FLASH_DYNA_SWITCH_MODE  2
#define FLASH_FILLIN_DAY_TO_NIGHT 3  /* �������Ƚ���Ƶ����������Ƶ��л� */
#define FLASH_FILLIN_NIGHT_TO_DAY 4  /* �������Ƚ���Ƶ����������Ƶ��л� */

#define FLASH_AC_DAY_TO_NIGHT 	5	 /* �������Ƚ���AC����ͬ�����л� */
#define FLASH_AC_NIGHT_TO_DAY 	6	 /* �������Ƚ���AC����ͬ�����л� */

#define DUBAL_FLASH_NORMAL_SWITCH 7
#define DUBAL_FLASH_FORCE_SWITCH  8

#define RL_NORMAL_SET_MODE   	0
#define RL_DYNA_SET_MODE     	1


#define MODULE_MMS              1     /*��������*/
#define MODULE_MC_LOG           1  /*��־*/
#define MODULE_SYS_CONF         1  /*��ҳϵͳ���� ����*/
#define MODULE_FTP_SEND         1
#define MODULE_XCS_PROTOCOL     1   /*xcs ͨѶЭ��*/
#define MODULE_DSP              1       /*��������*/
#define MODULE_DEV_CTL          1
#define MODULE_CONF_SERVER      1

#define MODULE_REC              1

#define MODULE_NTP              1
#define MODULE_NM               1	/*���� ������*/
#define MODULE_ALARMREC			1

//#define MODULE_NETREC			1	/* ��˾�ƴ洢 */

//#if defined(H264)
#define MODULE_ONVIF		1	
//#endif

//#define MODULE_CMOS				1

/* monitor�����Ѿ��ڼ�أ�����Ҫ���� */
//#define MODULE_CIFS             1

/* ȫ��������ģʽ */
#define ALL_LANE_SET_MODE		1
/* ����������ģʽ */
#define SINGAL_LANE_SET_MODE	2
/* ��������ģʽ */
#define OTHER_SET_MODE			3

//#define MODULE_ICX274_CCD 		1

/*  �ɱ䳵�����з������� */
#define NORMAL_DIR_SET			1
#define CHANGE_DIR_SET			2

/* ע�⣬������scons�½ű����屣��һ�£���ȻͼƬ���޷��ϴ�ftp! */
#define CYC200M_DEV_CODE    0xA122005F
#define CYC200ML_DEV_CODE   0xA1220060
#define CYC200MX_DEV_CODE   0xA1220061
#define CYC200MK_DEV_CODE   0xA1220062
#define CYC200KW_DEV_CODE   0xA1220063
#define CYC500KX_DEV_CODE   0xA1220064

#define CYC200KX3_DEV_CODE  0xA1220065
#define CYC500KX2_DEV_CODE  0xA1220066
#define CYC200K2_DEV_CODE   0xA1220067

#define CYC200K3_DEV_CODE   0xA1220068
#define CYC500K_DEV_CODE    0xA1220069

#define CYC800JX_DEV_CODE   0xA122006A
#define CYC500JX_DEV_CODE   0xA122006B

#define CYT200SDI_DEV_CODE	0xA122006C
#define CYC500XKW_DEV_CODE  0xA122006D
#define CYC500XCKW_DEV_CODE 0xA122006E

#define CYC500KD_DEV_CODE   0xA122006F
#define CYC500KKW2C_DEV_CODE 0xA1220071

#define CYC500XLKW_DEV_CODE 0xA1220072
#define CYC500XSKW_DEV_CODE 0xA1220073

#define CYC800XKW4C_DEV_CODE  0xA1220074

#define CYC200KT_DEV_CODE  	 0xA1220075

#define CYC200XKW_DEV_CODE   0xA1220076

#define CYC500XKW4C_DEV_CODE 0xA1220077


#define CYC200XZD_DEV_CODE  0xA1220087
#define CYC500XZD_DEV_CODE  0xA1220086

#define CYC500XAW_DEV_CODE  0xA1220090

extern const unsigned int device_code;
extern unsigned int g_uiHardWVer;

extern unsigned char g_ucEPComSnapFlag;
extern unsigned char g_ucEPComBufCleanFlag;
extern unsigned char g_ucEPHandSnapFlag;


#define CAM_PARAM_MODE_FORCE	0  /* ����ʵ�ʸ���һ����������� */
#define CAM_PARAM_MODE_NORMAL	1  /* ���ϴ��Ѹ��£����ظ�������������� */
#define CAM_PARAM_MODE_MANUAL	2  /* ҳ���ֶ����ã���Ҫʵʱ��Ч������״̬����0 */

/* ������ƺ궨�� */
#define ASSIST_RD_DISABLE		0	/* �������û��ʹ�� */
#define ASSIST_RD_IN_RANGE		1	/* �����������Чʱ���� */
#define ASSIST_RD_OUT_RANGE		2	/* ������Ʋ�����Чʱ���� */
#define ASSIST_RD_INVALID_NUM	3	/* �������ʹ���ˣ�����ƺŲ��� */

#define DATE_24_HOUR (24*3600 -1)

#define MAX_COM_DEV 1		/* ֧�ּ�������ͬʱ���� */

#define MAX_COM_NUM 2		/* ֧�ּ������ڣ���ӳ�䵽��ͬ������ */

#define MAIN_PID_FILE_NAME          "/var/run/xcs.pid"


#define CIFS_MOUNT_POINT            "/cifs/disk1"
#define U_DIST_MOUNT_POINT          "/udisk"
#define SD_MOUNT_POINT          	"/sd"

/* ץ��·��ͳ����Ϣ���ݽṹ */
typedef struct jpegSnapInfo
{
    /* ���ڽ��ն�������ͳ�� */
    unsigned int uiComLoseDataNum;
    
    unsigned int uiComFullFrameSun;
    unsigned int uiComLackFrameSun;
    unsigned int uiComThrowFullFrameSun;    
    /* Ӧ�õ����ں�ץ�ĺ�����������ͳ�� */
    unsigned int uiAppNormalSnapFullGSum;
    unsigned int uiAppForceSnapFullGSum;
}STR_JPEG_SNAP_COUNT_INFO;

/* ԭʼ���ݲɼ���Ϣ���ݽṹ */
typedef struct orgImgPickInfo
{
    int iFlag;
    int iPickType;
    int X;
    int Y;
    int width;
    int height;
}STR_ORG_IMG_PICK_INFO;

typedef struct tagLaneSwitchInfo
{
	STR_XCS_TRAFFIC_LIGHT strTrafficL[MAX_LIGHT_TYPE];
	unsigned int 		  uiLightTypeBT[MAX_LIGHT_TYPE]; /* ÿ����״̬�仯��ʼʱ�䣬��λ:�� */
	char  cRedLSta;			/* �����������״̬ */
	char  cRoadDir;			/* ������������ */
	
	/*Start of ITS on 2011-4-6 15:37 2.0.0.1*/
	char  cCoilDecType; 	/* ��Ȧ��ⷽʽ:�ظл����� */
	char  cCoilSwitchFlag;	/* ��Ȧ�Ƿ��л���־ */
	
	char  cCoilSwitchSta;	/* ��Ȧ��ǰ��ⷽʽ:�ظл����� */
	char  cInLightType;		/* ���ⷽʽ */
	char  cInLightSta;		/* ��ǰ����״̬ */
	char  cVLDecEn;			/* ������Ȧ����Ƿ�ʹ�� */
	/*End of ITS on 2011-4-6 15:37 2.0.0.1*/

	char cSwitchLaneEn;		 /* �Ƿ�Ϊ�ɱ䳵�� */
    char cSwitchLaneLeftNo;	 /* �ɱ䳵����ת����ƺ� */
    char cSwitchLaneStraightNo; /* �ɱ䳵��ֱ�з���ƺ� */
    char cRsv;

    char cSwitchLaneLeftNoSta;		/* �ɱ䳵����ת�����״̬ */
    char cSwitchLaneStraightNoSta;  /* �ɱ䳵��ֱ�з����״̬ */
    char cRsv2[2];
}LANE_SWITCH_STA_INFO;


typedef struct tagPPLaneSwitchInfo
{
	char  cCoilDecType; 		/* ��Ȧ��ⷽʽ:�ظл����� */
	char  cCoilSwitchFlag;		/* ��Ȧ�Ƿ��л���־ */
	char  cCoilSwitchSta;		/* ��Ȧ��ǰ��ⷽʽ:�ظл����� */
	char  cInLightType;			/* ���ⷽʽ */
	char  cInLightSta;			/* ��ǰ����״̬ */
	char  cSingalCoilPos;		/* �ظ���Ȧ��Ȧλ�ã�ǰ��Ȧ�����Ȧ */
	char  cSingalCoilSnapType;	/* ����Ȧץ��ģʽ */
	char  cGroudCoilDecType;	/* �ظ���Ȧץ������:˫��Ȧ����Ȧ�����״���ٹ��� */
}PP_LANE_SWITCH_STA_INFO;

typedef struct tagAssistRedLightSta
{
	int iLaneNum;		/* ������ */
	int iRedLightNum;	/* ��ƺ� */
	int iValidFlag;		/* �Ƿ���Ч��־ */
	int iSta;			/* ���״̬ */
}EP_ASSIST_RED_LIGHT_STA;

/* ץ��ʱ�Ƿ������־ */
#define SNAP_FLASH_FLAG  0x1

/* �������Ч��ַ�궨�� */
#define FLASH_INVALID_ADDR 0xffff

int xcsRecInit(void);
int xcsRecUninit(void);
int DebugSwitch(char *szModule,int nLevel);

int initMmsSendMode(void);

int writeThreadInfo(char *pcThreadInfo);

char ePCarDistanceGet();
char ePLaneCarDistanceGet(int iLane);

int ftpSendInit(void);
int dctlPPLoadSwitchInfo(void);
int dctlEPLoadRedInfo(char cMode, unsigned int uiLane);
int dctlRedLightReport(int iChannel, int iSta);
void hagdDataPro(VEHICLE_CALLBACK_STR *pstVehicle, int iNum);
int devCtlInit(void);
int xcsSysReboot(void);
int xcsSysRestoreDevConf(void);
int xcsSysUpdate(unsigned char *pUrl);
void mainJpegSnapWayInfo();

int epGXcsProVerGet(void);

int sendMsgToMainApp(STR_SYS_APP_MSG *pStrSysMsg);
int ftpServerChange(void);
int epPPPicSendFlag(void);
int ftpPPChannelNameSet(void);
int ftpEPChannelNameSet(void);
int ftpEPRoadRemarksSet(void);
int ftpTEPChannelNameSet(void);
int ftpPPLaneNameSet(JpegSendInfo *pstrJpgInfo, char cLaneId);
int ftpEPLaneNameSet(JpegSendInfo *pstrJpgInfo, char cLaneId);
int ftpPP330PlatFormInfoSet(void);
int ftpEP330PlatFormInfoSet(void);
int ftpEPIrregNameInfoSet(void);
int ftpPPIrregNameInfoSet(void);

int vehicleStaMonitor(void);

int setSimSnapRedTime(unsigned char ucLaneId);
char getTrafficLightSta(unsigned int uiCarRoadId);
int redLightStaForceSet(unsigned int uiCarRoadId, unsigned char ucSta);
int redLightStaForceDspSet(unsigned int uiCarRoadId);
unsigned char redLightForceStaQuery(unsigned int uiCarRoadId);
unsigned char redLightNormalStaQuery(unsigned int uiCarRoadId);
int dspPPMSnap(int iCarRoadId, int iCarSpeed, int iCarDir);
int dspEPMSnap(int iCarRoadId);

int switchLaneLightNoCheck(int iVehicle);
int vehicleNMStaProc(void);
int kjRedLightHandle(unsigned char ucVehId, unsigned char ucSta, char cRLDecType);
int kjRedLightInform(unsigned char ucVehId, unsigned char ucSta, char cRLDecType);
int redLightDecTypeChange(unsigned char ucVehId, unsigned char ucSta, char cRLDecType);

int dctlVehilceStaSet(int iSta);
int dctlVehilceStaGet(void);
int dctlLoadPPCoil();
int dCtlLoadEPCoil();
int dctlPPFlashConf(char cMode, unsigned short usType);
int dctlEPFlashConf(char cMode, unsigned short usType);
int dctlPPFlashNormalSnap(int iComId,unsigned int iCarRoadId, int iStep, int iCarSpeed, int iCarDir);
int dctlPPFlashForceSnap(int iComId, int iCarRoadId, int iStep, int iCarSpeed, int iCarDir);
int dctlEPFlashNormalSnap(unsigned int iCarRoadId, int iStep, int iSnapType);
int dctlRadarNormalSnap(int iComId, unsigned int iCarRoadId, int iStep, int iCarSpeed, int iCarDir);


int dctlPPTTLForceSnap(int iCarRoadId, int iStep, int iSnapType);
int dctlPPTTLNormalSnap(int iCarRoadId, int iStep, int iSnapType);

int devCtlParamConf();
int epGetUsedLaneSum(void);
int ppGetUsedLaneSum();

int  sjPPCoilStaSwitch(unsigned char *pucSta, unsigned char ucMonitroSta);

int  kjPPCoilStaSwitch(unsigned char ucVehicleSta, unsigned char ucLaneSta);
int  kjEPCoilStaSwitch(unsigned char ucVehicleSta, unsigned char ucLaneSta, unsigned char ucRLSta);

int recSysConfSet();

int DataReportInit(void);
int DataReportUninit(void);
int SetDataReportParam(STR_DATA_REPORT_SET  *pstrDataReportParam);

int NetFsCheckInit();
int NetFsProcess(STR_NETFS_STA  *pStrNetFsSta);
int NetFsUninit();

int NtpInit(void);
int ntpServerIpSet(void);
int ntpEnFlagGet(void);
int NtpUninit();
int SysSetTime(unsigned int time, bool initDsp = true);

int dspCamABSModeSet(void);
int dspShowFeature(CAMERA_CTRL_CONF_INFO *pStrCam);

int sysCamTimeUpdate(void);
int camTimeRangeJudge(void);
int camTimeRangeParamJudge(char cIndex, unsigned int uiBeginTime, unsigned int uiEndTime);
int camCtlParamRunJudge(char cMode);

unsigned char uDiskEnFlagGet(void);
unsigned char sdEnFlagGet(void);
unsigned short netVehiclePortGet(void);
int epFlowRateFlagGet(void);
int epFlowRateInterGet(void);
int epAlarmRecFlagGet(void);
int ftpXmlFileSendFlagGet(void);
int redLightDelaySecGet(void);

float kodarGreyValGet(char cGroup);
float kodarGammaValGet(char cGroup);
float kodarContrastValGet(char cGroup);
float kodarDeNoiseValGet(char cGroup);
float kodarSharpValGet(char cGroup);
float kodarSaturationValGet(char cGroup);
float kodarAWBValGet(char cGroup);
float kodarIsoValGet(char cGroup);


char kodarJudgeFlashLinesConf(void);
char kodarJudgeFlashLinesSave(char cMode, char cLines);

char kodarFpgaNewParamSet(void);
char kodarFpgaNewParamSave(unsigned short usSaturation, unsigned short usGPixThresh);

char kodarDubalShutParamSave(char cEnable, float fShutVal, float fGainVal);
char kodarDubalShutParamSet(char cMode);

char redDectThresholdParamSave(char cRedDectSatu, char cRedDectBright);

#if 0
char newRGBRedRepareParamSave(REDLIGHT_REPAIR_PARAM strRedRepairParam);
char newRGBRedRepareParamConf(void);
#endif

/* ��ͨ���쳣����ӿں��� */
int getTrafficMsgQSize(void);
int sendMsgToTrafficLM(STR_SYS_APP_MSG *pStrSysMsg);
int trafficLMInit(void);
int trafficLMUninit(void);
int redLightChangeInformDsp(unsigned int iLanId, char cMode);
int trafficLDelaySecSet(unsigned char ucVal);
int greenLightDectFlagGet(void);


int debugShellLogSet(void);
int eptPpStepNumGet(void);
unsigned char epPpSnapPositionGet(void);

unsigned char ePReservSnapFlagGet(void);

int cameraRunStaParamGet(int chan_id, CUR_CAMERA_STATUS *param);

int ntpDebugLevelSet(int iFlag);

unsigned short ppCsSpeedAmendNumeratorGet(void);

#endif

