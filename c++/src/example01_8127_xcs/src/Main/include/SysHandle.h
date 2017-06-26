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

/* 按照摄像机像素来确定的车道数 */
extern unsigned int g_uiCamLaneSum;

/* 配置客户端实际配置的车道数 */
extern unsigned int g_uiClientLaneSum;

/* 电警最新小协议版本号 */
extern unsigned short g_usEpXcsProVer;

#define ORG_IMAG_PICK_OK        1
#define ORG_IMAG_PICK_NG        0

#define FLASH_NORMAL_SET_MODE   0
#define FLASH_DYNA_SET_MODE     1
#define FLASH_DYNA_SWITCH_MODE  2
#define FLASH_FILLIN_DAY_TO_NIGHT 3  /* 根据亮度进行频闪灯与闪光灯的切换 */
#define FLASH_FILLIN_NIGHT_TO_DAY 4  /* 根据亮度进行频闪灯与闪光灯的切换 */

#define FLASH_AC_DAY_TO_NIGHT 	5	 /* 根据亮度进行AC交流同步的切换 */
#define FLASH_AC_NIGHT_TO_DAY 	6	 /* 根据亮度进行AC交流同步的切换 */

#define DUBAL_FLASH_NORMAL_SWITCH 7
#define DUBAL_FLASH_FORCE_SWITCH  8

#define RL_NORMAL_SET_MODE   	0
#define RL_DYNA_SET_MODE     	1


#define MODULE_MMS              1     /*码流发送*/
#define MODULE_MC_LOG           1  /*日志*/
#define MODULE_SYS_CONF         1  /*网页系统参数 配置*/
#define MODULE_FTP_SEND         1
#define MODULE_XCS_PROTOCOL     1   /*xcs 通讯协议*/
#define MODULE_DSP              1       /*码流接收*/
#define MODULE_DEV_CTL          1
#define MODULE_CONF_SERVER      1

#define MODULE_REC              1

#define MODULE_NTP              1
#define MODULE_NM               1	/*网管 服务器*/
#define MODULE_ALARMREC			1

//#define MODULE_NETREC			1	/* 我司云存储 */

//#if defined(H264)
#define MODULE_ONVIF		1	
//#endif

//#define MODULE_CMOS				1

/* monitor进程已经在监控，不需要启动 */
//#define MODULE_CIFS             1

/* 全车道配置模式 */
#define ALL_LANE_SET_MODE		1
/* 单车道配置模式 */
#define SINGAL_LANE_SET_MODE	2
/* 其他配置模式 */
#define OTHER_SET_MODE			3

//#define MODULE_ICX274_CCD 		1

/*  可变车道禁行方向配置 */
#define NORMAL_DIR_SET			1
#define CHANGE_DIR_SET			2

/* 注意，必须与scons下脚本定义保持一致，不然图片会无法上传ftp! */
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


#define CAM_PARAM_MODE_FORCE	0  /* 必须实际更新一次摄像机参数 */
#define CAM_PARAM_MODE_NORMAL	1  /* 若上次已更新，则不重复更新摄像机参数 */
#define CAM_PARAM_MODE_MANUAL	2  /* 页面手动配置，需要实时生效，并且状态机清0 */

/* 辅助红灯宏定义 */
#define ASSIST_RD_DISABLE		0	/* 辅助红灯没有使能 */
#define ASSIST_RD_IN_RANGE		1	/* 辅助红灯在有效时间内 */
#define ASSIST_RD_OUT_RANGE		2	/* 辅助红灯不在有效时间内 */
#define ASSIST_RD_INVALID_NUM	3	/* 辅助红灯使能了，但红灯号不对 */

#define DATE_24_HOUR (24*3600 -1)

#define MAX_COM_DEV 1		/* 支持几个外设同时工作 */

#define MAX_COM_NUM 2		/* 支持几个串口，可映射到不同的外设 */

#define MAIN_PID_FILE_NAME          "/var/run/xcs.pid"


#define CIFS_MOUNT_POINT            "/cifs/disk1"
#define U_DIST_MOUNT_POINT          "/udisk"
#define SD_MOUNT_POINT          	"/sd"

/* 抓拍路径统计信息数据结构 */
typedef struct jpegSnapInfo
{
    /* 串口接收丢弃数据统计 */
    unsigned int uiComLoseDataNum;
    
    unsigned int uiComFullFrameSun;
    unsigned int uiComLackFrameSun;
    unsigned int uiComThrowFullFrameSun;    
    /* 应用调用内核抓拍函数完整参数统计 */
    unsigned int uiAppNormalSnapFullGSum;
    unsigned int uiAppForceSnapFullGSum;
}STR_JPEG_SNAP_COUNT_INFO;

/* 原始数据采集消息数据结构 */
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
	unsigned int 		  uiLightTypeBT[MAX_LIGHT_TYPE]; /* 每个灯状态变化初始时间，单位:秒 */
	char  cRedLSta;			/* 车道红灯最终状态 */
	char  cRoadDir;			/* 车道方向属性 */
	
	/*Start of ITS on 2011-4-6 15:37 2.0.0.1*/
	char  cCoilDecType; 	/* 线圈检测方式:地感或虚拟 */
	char  cCoilSwitchFlag;	/* 线圈是否切换标志 */
	
	char  cCoilSwitchSta;	/* 线圈当前检测方式:地感或虚拟 */
	char  cInLightType;		/* 补光方式 */
	char  cInLightSta;		/* 当前补光状态 */
	char  cVLDecEn;			/* 虚拟线圈检测是否使能 */
	/*End of ITS on 2011-4-6 15:37 2.0.0.1*/

	char cSwitchLaneEn;		 /* 是否为可变车道 */
    char cSwitchLaneLeftNo;	 /* 可变车道左转方向灯号 */
    char cSwitchLaneStraightNo; /* 可变车道直行方向灯号 */
    char cRsv;

    char cSwitchLaneLeftNoSta;		/* 可变车道左转方向灯状态 */
    char cSwitchLaneStraightNoSta;  /* 可变车道直行方向灯状态 */
    char cRsv2[2];
}LANE_SWITCH_STA_INFO;


typedef struct tagPPLaneSwitchInfo
{
	char  cCoilDecType; 		/* 线圈检测方式:地感或虚拟 */
	char  cCoilSwitchFlag;		/* 线圈是否切换标志 */
	char  cCoilSwitchSta;		/* 线圈当前检测方式:地感或虚拟 */
	char  cInLightType;			/* 补光方式 */
	char  cInLightSta;			/* 当前补光状态 */
	char  cSingalCoilPos;		/* 地感线圈线圈位置，前线圈或后线圈 */
	char  cSingalCoilSnapType;	/* 单线圈抓拍模式 */
	char  cGroudCoilDecType;	/* 地感线圈抓拍类型:双线圈或单线圈或者雷达测速功能 */
}PP_LANE_SWITCH_STA_INFO;

typedef struct tagAssistRedLightSta
{
	int iLaneNum;		/* 车道号 */
	int iRedLightNum;	/* 红灯号 */
	int iValidFlag;		/* 是否生效标志 */
	int iSta;			/* 红灯状态 */
}EP_ASSIST_RED_LIGHT_STA;

/* 抓拍时是否闪光标志 */
#define SNAP_FLASH_FLAG  0x1

/* 闪光灯无效地址宏定义 */
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

/* 交通灯异常处理接口函数 */
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

