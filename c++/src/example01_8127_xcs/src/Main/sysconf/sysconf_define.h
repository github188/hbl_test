/******************************************************************************
 * sysconf_define.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.12, xcsworker Create
 * --------------------
 ******************************************************************************/

#ifndef _SYS_CONF_DEFINE_H__
#define _SYS_CONF_DEFINE_H__

#include "McLog.h"

extern int g_Sysconf_Debug;

#ifdef CONF_MODULE_DEBUG
	#define CONF_DEBUG(x...)\
	do{\
		if(g_Sysconf_Debug)\
			shell_print("sys conf mode:"x);\
	}while(0);
#else
	#define CONF_DEBUG(x...)
#endif

//用于打印模块错误信息，可以用全局变量g_Sysconf_Debug单独控制
#define CONF_ERROR(x...)\
do{\
	if(g_Sysconf_Debug&0x0001)\
			shell_print("sys conf mode:"x);\
}while(0);

//用于打印模块警告信息，可以用全局变量g_Sysconf_Debug单独控制
#define CONF_WARNING(x...)\
do{\
	if(g_Sysconf_Debug&0x0002)\
			shell_print("sys conf mode:"x);\
}while(0);

//用于打印模块提示信息，可以用全局变量g_Sysconf_Debug单独控制
#define CONF_INFO(x...)\
do{\
	if(g_Sysconf_Debug&0x0004)\
			shell_print("sys conf mode:"x);\
}while(0);

#include <stdio.h>
#include <semaphore.h>


//用于保存不能随意删除的配置信息文件
static const char SYS_CONF_FILE[2][64] = {"/mnt/flash/system.ini","/mnt/flash/system.ini.bak"};


typedef struct tagSysConfModule
{
	int								bSysConfInit;
	sem_t							semSaveOP;
	NET_CONF_INFO					s_NetConfInfo;
	RTSP_CONF_INFO					s_RtspConfInfo;
	VIDEO_CONF_INFO					s_VideoConfInfo[TOTAL_CHANNEL_NUM];
	REC_CONF_INFO					s_RecConfInfo[TOTAL_CHANNEL_NUM];
	PASSPORT_OVERALL_CONF_INFO		s_PassPortOverallConfInfo;
	PORT_VEHICLE_CONF_INFO			s_PortVehicleConfInfo[MAX_VEHICLE_NUM];
	ELEC_POLICE_OVERALL_CONF_INFO	s_ElecPoliceOverallConfInfo;
	ELEC_POLICE_VEHICLE_CONF_INFO	s_ElecPoliceVehicleConfInfo[MAX_VEHICLE_NUM];
	VL_CONF_INFO 					s_VlConfInfo[MAX_VEHICLE_NUM];
	FTP_CONF_INFO					s_PortFtpConfInfo;
	FTP_CONF_INFO					s_ElecPoliceFtpConfInfo;
	NTP_CONF_INFO					s_NtpConfInfo;
	CAMERA_TIME_RANGE_INFO			s_CamTimeRange[MAX_CAM_GROUP_NUM];
	CAMERA_CTRL_CONF_INFO			s_CameraCtrlConfInfo[MAX_CAM_GROUP_NUM][MAX_CAM_CTRL_NUM];
	NET_HDD_CONF_INFO				s_PicNetHddConfInfo;
	NET_HDD_CONF_INFO				s_RecNetHddConfInfo;
	DCTL_CONF_INFO					s_DctlConfInfo[MAX_DCTL_NUM];
    PLATFORM_330_CONF_INFO          s_Platform330ConfInfo;
    PLATFORM_V3_CONF_INFO           s_PlatformV3ConfInfo;
    VIDICON_MENU_CONF_INFO          s_VidiconMenuConfInfo[1];
	SYS_DEF_CONF_INFO				s_SysConfInfo;
	SYS_NM_CONF_INFO				s_sysNMConfInfo;
    int                             s_AppMode;
    SYS_FUNC_ENABLE_INFO			s_FuncEnableInfo;
    EP_TIME_RANGE_INFO				s_EpTimeRange[MAX_VEHICLE_NUM][EP_FORBID_MAX_DIR];
    EP_V_LOOP_STEP__INFO			s_EpVLoopStepInfo;
    PLATFORM_330_CONF_INFO          s_EpPlatform330ConfInfo;
    PLATFORM_V3_CONF_INFO           s_PlatformV2ConfInfo;
	EP_ASSIST_RED_LIGHT_INFO		s_EpAssistRedLight[MAX_VEHICLE_NUM][EP_ASSIST_RED_LIGHT_NUM];
    EP_IRREG_CONF_INFO				s_EpIrregConfInfo;
    EP_LPR_CONF_INFO				s_EpLprConfInfo;
    PP_FUNC_ENABLE_INFO				s_PpFuncConfInfo;
    EP_STOP_ON_TRAFFIC_NO_ENTRY		s_EpSOTNoEntry[MAX_VEHICLE_NUM];
    BAYONET_JPG_OSD_CONF			s_PpJpgOsdConf;
    EP_JPG_OSD_CONF					s_EpJpgOsdConf;
    PP_IRREG_CONF_INFO				s_PpIrregConfInfo;
	FTP_CONF_INFO					s_PortBackupsFtpConfInfo;
    NET_REC_MANAGE_INFO				s_NrmConfInfo;
    FTP_CONF_INFO					s_epBackupsFtpConfInfo;
    EP_NEW_ADD_IRREG_CONF_INFO		s_EpNewAddIrregConfInfo;
    GB28181_CONF_INFO           	s_Gb28181ConfInfo;    
} SYS_CONF_MODULE;

typedef enum
{
	e_NET_CONF_INFO = 0x0,
	e_RTSP_CONF_INFO,
	e_VIDEO_CONF_INFO,
	e_REC_CONF_INFO,
	e_PASSPORT_OVERALL_CONF_INFO,           /* 卡口全局配置信息 */
	e_PASSPORT_VEHICLE_CONF_INFO,           /* 卡口车道配置信息 */
	e_ELEC_POLICE_OVERALL_CONF_INFO,        /* 电子全局警察配置 */
	e_ELEC_POLICE_VEHICLE_CONF_INFO,        /* 电子警察车道配置信息 */
	e_VL_CONF_INFO,                         /* 虚拟线圈高级控制配置信息 */
	e_PASSPORT_FTP_CONF_INFO,               /* 卡口FTP 配置信息 */
	e_ELEC_POLICE_FTP_CONF_INFO,            /* 电子警察 FTP 配置信息 */
	e_NTP_CONF_INFO,                        /* NTP 配置信息 */
	e_CAMERA_CTRL_CONF_INFO,                /* 摄像机控制配置信息 */
	e_PIC_NET_HDD_CONF_INFO,                /* 图片网络硬盘配置信息 */
	e_REC_NET_HDD_CONF_INFO,                /* 录像网络硬盘配置信息 */
	e_DCTL_CONF_INFO,                       /* 外设控制配置信息 */
	e_PLATFORM_CONF_INFO,                   /* 平台配置信息 */
	e_V3_CONF_INFO,                         /* V3平台配置信息 */
	e_SYS_DEF_CONF_INFO,                    /* 系统默认配置信息 */
	e_APPMODE_CONF_INFO,                    /* 应用程序工作模式 */
    e_NM_CONF_INFO,               			/* 网管平台配置 */
    e_CAM_TIME_RANGE_INFO,             		/* 摄像机时间段配置 */
    e_FUNC_ENABLE_FLAG_INFO,				/* 功能使能标志配置 */
    e_EP_TIME_RANGE_INFO,					/* 电警车道禁止行驶时间段配置 */
    e_EP_VLOOP_STEP_INFO,					/* 电警虚拟线圈抓拍步数间隔配置 */
    e_EP_330_CONF_INFO,                     /* 电警330平台配置信息 */
    e_V2_CONF_INFO,                         /* V2平台配置信息 */
    e_EP_ASSIST_REDLIGHT_INFO,              /* 电警辅助红灯配置信息 */	
    e_EP_IRREG_CONF_INFO,					/* 电警违章类型描述及代码配置信息 */
    e_EP_LPR_CONF_INFO,						/* 电警车牌识别配置信息 */
    e_PP_FUNC_CONF_INFO,					/* 卡口相关功能配置信息 */
    e_EP_SOT_NO_ENTRY_INFO,    		        /* 电警路口遇堵禁入配置信息 */	
    e_PP_JPG_OSD_CONF_INFO,					/* 卡口图片osd叠加配置信息 */
    e_EP_JPG_OSD_CONF_INFO,					/* 电警图片osd叠加配置信息 */
    e_PP_IRREG_CONF_INFO,					/* 卡口违章类型描述及代码配置信息 */
	e_PASSPORT_BACKUPS_FTP_CONF_INFO,       /* 备份用卡口FTP 配置信息 */
    e_NRM_CONF_INFO,                        /* NRM配置信息 */
    e_EP_BACKUPS_FTP_CONF_INFO,       		/* 备份用卡口FTP 配置信息 */
    e_EP_NEWADD_IRREG_CONF_INFO,			/* 电警新增违章类型描述及代码配置信息 */
    e_GB28181_CONF_INFO,                    /* GB28181平台配置信息 */
}SYS_INI;

static const char *sys_ini_section[] = 
{
	"NET_CONF_INFO",
	"RTSP_CONF_INFO",
	"VIDEO_CONF_INFO",
	"REC_CONF_INFO",
	"PASSPORT_OVERALL_CONF_INFO",
	"PASSPORT_VEHICLE_CONF_INFO",
	"ELEC_POLICE_OVERALL_CONF_INFO",
	"ELEC_POLICE_VEHICLE_CONF_INFO",
	"VL_CONF_INFO",
	"PASSPORT_FTP_CONF_INFO",
	"ELEC_POLICE_FTP_CONF_INFO",
	"NTP_CONF_INFO",
	"CAMERA_CTRL_CONF_INFO",
	"PIC_NET_HDD_CONF_INFO",
	"REC_NET_HDD_CONF_INFO",
	"DCTL_CONF_INFO",
	"PLATFORM_330_CONF_INFO",
	"PLATFORM_V3_CONF_INFO",
	"SYS_DEF_CONF_INFO",
	"APPMODE_CONF_INFO",
    "PLATFORM_NM_CONF_INFO",
    "CAM_TIME_RANGE_INFO",
    "FUNC_ENABLE_FLAG_INFO",
    "EP_TIME_RANGE_INFO",
    "EP_V_LOOP_STEP_INFO",
    "EP_330_CONF_INFO",
    "PLATFORM_V2_CONF_INFO",
    "EP_ASSIST_REDLIGHT_INFO",
    "EP_IRREG_CONF_INFO",
    "EP_LPR_CONF_INFO",
    "PP_FUNC_CONF_INFO",
    "EP_SOT_NO_ENTRY_INFO",
    "PP_JPG_OSD_CONF_INFO",
    "EP_JPG_OSD_CONF_INFO",
    "PP_IRREG_CONF_INFO",
	"PASSPORT_BACKUPS_FTP_CONF_INFO",
    "NRM_CONF_INFO",
    "EP_BACKUPS_FTP_CONF_INFO",
    "EP_NEWADD_IRREG_CONF_INFO",
    "GB28181_CONF_INFO",
	NULL
};

/**摄像机参数组名**/
typedef enum
{
	e_CAM_NAME_NORMAL = 0x0,
	e_CAM_NAME_BACKLIGHT,
	e_CAM_NAME_FRONTLIGHT,
	e_CAM_NAME_DUST,
}CAM_NAME_CONF;

static const char *camNameInfoKey[] = 
{
	"日常模式",
	"背光模式",
	"顺光模式",
	"黄昏模式",
	NULL
};


/**RTSP 配置信息**/
typedef enum
{
	e_SupportDownLoad = 0 ,
	e_DownLoadType ,
	e_DownLoadIp ,
	e_DownLoadPort,
	e_SupportReplay,
	e_ReplayType,
	e_ReplayIP,
	e_ReplayPort,
}RTSP_CONF;

typedef enum
{
	e_vVideoChannelID = 0,
	e_vVQuality,
	e_vFrameRate,
	e_vBitRate,
	e_vKeyFinterval,
	e_vImageResol,
	e_vEncType,
	e_vVBR,
	e_vXPos,
	e_vYPos,
	e_vContent,
	e_vDebugFrameX,
	e_vDebugFrameY,
	e_vDebugFrameEn,

	e_vTimeDisMode,
	e_vTimeXPos,
	e_vTimeYPos,
	
}VIDEO_CONF;

static const char *VideoConfInfoKey[] =
{
	"VideoChannelID",
	"VQuality ",
	"FrameRate ",
	"BitRate",
	"KeyFinterval",
	"ImageResol",
	"EncodeType",
	"VBR",
	"Xpos",
	"Ypos",
	"Content",
	"debugFrameX",
	"debugFrameY",
	"debugFrameEn",

	"vTimeDisMode",
	"vTimeXPos",
	"vTimeYPos",

	NULL
};

/**录像配置信息**/
typedef enum
{
	e_rRecChannelID = 0,
	e_rSysRecMode,
	e_rLoopRecFlag,
	e_rStorageRecDays,
	e_rRecPath,
	e_rManualRecTime,
	e_rOncePlanRecTime,
	e_rDayPlanRecTime,
	e_rMondayPlanRecTime,
	e_rTuesdayPlanRecTime,
	e_rWednesdayPlanRecTime,
	e_rThursdayPlanRecTime,
	e_rFridayPlanRecTime,
	e_rSaturdayPlanRecTime,
	e_rSundayPlanRecTime,
	e_rRecFileFormat,
}REC_CONF;

static const char *RecConfInfoKey[] =
{
	"ChannelID",
	"SysRecMode",
	"LoopRecFlag",
	"StorageRecDays",
	"RecPath",
	"ManualRecTime",
	"OncePlanRecTime",
	"DayPlanRecTime",
	"MondayPlanRecTime",
	"TuesdayPlanRecTime",
	"WednesdayPlanRecTime",
	"ThursdayPlanRecTime",
	"FridayPlanRecTime",
	"SaturdayPlanRecTime",
	"SundayPlanRecTime",
	"RecFileFormat",
	NULL
};

/**卡口全局配置信息**/
typedef enum
{
	e_paPassPortName = 0,
	e_paDirection,
	e_paPassPortId,
	e_paDirId,
	e_paOSDClolr,
	e_paValidLanes,
	e_paVLWorkMode,
	
	e_paLPREn,
	e_paLPRString,
	e_paLPRCConfiden,
	e_paVSMeaFlag,
	e_paVSMeaLen,
	e_paFreqFlashEn,
	e_paFlashTrigMode,
	e_paStrobeFlashFreq,
	e_paPpDevCode,

    e_paFillInLSwitchEn,
    e_paGrayThresold,
    e_paCheckGrayCycle,
    e_paExPpDevCode,

    e_paSmartSwitchEn,
	e_paTimerSwitchEn,
	e_paFILightBeginTime,
	e_paFILightLastTime,
}PORT_OVERALL;
static const char *PassPortOverallConfInfoKey[]=
{
    "PassPortName",
    "Direction",
    "PassPortId",
    "DirId",
    "ppOSDColor",
    "ppValidLanes",
    "ppVLWorkMode",
    
    "ppLPREn",
    "ppLPRFString",
    "ppLRPFConfiden",
    "ppVSMeaFlag",
    "ppVSMeaLen",
    "ppFreqFlashEn",
    "ppFlashTrigMode",
    "ppStrobeFlashFreq",
    "ppPpDevCode",
    
    "paFillInLSwitchEn",
    "paGrayThresold",
    "paCheckGrayCycle",
    "paExPpDevCode",

    "paSmartSwitchEn",
	"paTimerSwitchEn",
	"paFILightBeginTime",
	"paFILightLastTime",
	
    NULL
};

/**卡口车道配置信息**/
typedef enum
{
	e_pvVersion,
	e_pvVehicleId,
	e_pvCaptionX,
	e_pvCaptionY,
	e_pvVehicleName,
	e_pvAreaStartX,
	e_pvAreaStartY,
	e_pvAreaWidth,
	e_pvAreaHeight,
	e_pvLightType,
	e_pvFlashControlType,
	e_pvComId,
	e_pvFlashDevType,
	e_pvFlashCtlMode,
	e_pvFlashAddr,
	e_pvDetectType,
	e_pvIrreSpeed,
	e_pvVLStartX,
	e_pvVLStartY,
	e_pvVLWidth,
	e_pvVLHeight,
	e_pvVehicleDetectType,
	e_pvVehicleDetectConnectMode,
	e_pvCoilDesp,
	e_pvMaxSpeed,
	e_pvMinSpeed,
	e_pvCoilDist,
	e_pvVLDispFlag,
	e_pvLaneDispFlag,
	e_pvLaneId,
	e_pvSingalCoilPos,
	e_pvGroudCoilDecType,
	e_pvSingalCoilSnapType,
	e_pvCoilSwitchFlag,
	e_pvInLightSta,
	e_pvCoilSwitchSta,
	e_pvVLSnapDir,
	e_pvVLDecEn,
	e_pvSingalCoilHigh,
	e_pvDaySnapObj,
	e_pvNightSnapObj,
	e_pvSnapPicNum,
	e_pvSnapPicInte,
	e_pvInLightEn,
	e_pvBeginTimeSec,
	e_pvLastTimeSec,
	e_pvTimeSlotObj,

    e_pvSnapDir,
	e_pvSDEnFlag,
	e_pvSDBeginTime1,
	e_pvSDLastTime1,
	e_pvSDBeginTime2,
	e_pvSDLastTime2,
	e_pvSDBeginTime3,
	e_pvSDLastTime3,

	e_pvTruckEnFlag,
}PORT_VEHICLE;
static const char *PassPortVehicleConfInfoKey[]=
{
	"Version",
	"VehicleId",
	"CaptionX",
	"CaptionY",
	"VehicleName",
	"AreaStartX",
	"AreaStartY",
	"AreaWidth",
	"AreaHeight",
	"LightType",
	"FlashControlType",
	"ComId",
	"FlashDevType",
	"FlashCtlMode",
	"FlashAddr",
	"DetectType",
	"IrreSpeed",
	"VLStartX",
	"VLStartY",
	"VLWidth",
	"VLHeight",
	"VehicleDetectType",
	"VehicleDetectConnectMode",
	"CoilDesp",
	"MaxSpeed",
	"MinSpeed",
	"CoilDist",
	"VLDispFlag",
	"LaneDispFlag",
	"LaneId",
	"SingalCoilPos",
	"GroudCoilDecType",
	"SingalCoilSnapType",
	"ppCoilSwitchFlag",
	"ppInlightSta",
	"ppCoilSwitchSta",
	"ppVLSnapDir",
	"ppVLDecEn",
	"ppSingalCoilHigh",
	"ppDaySnapObj",
	"ppNightSnapObj",
	"ppSnapPicNum",
	"ppSnapPicInter",
	"ppInLightEn",
	"ppBeighTimeSec",
	"ppLastTimeSec",
	"ppTimeSlotObj",
	
	"pvSnapDir",
	"pvSDEnFlag",
	"pvSDBeginTime1",
	"pvSDLastTime1",
	"pvSDBeginTime2",
	"pvSDLastTime2",
	"pvSDBeginTime3",
	"pvSDLastTime3",

	"pvTruckEnFlag",
	NULL
};

/**卡口违章类型描述及代码配置**/
typedef enum
{
	e_ppOverSpeedSmallPercent50Code,
	e_ppOverSpeedSmallPercent50Name,

	e_ppOverSpeedLargePercent50Code,
	e_ppOverSpeedLargePercent50Name,
	
	e_ppReverseRunCode,
	e_ppReverseRunName,

	e_ppInfringeProhibCode,
	e_ppInfringeProhibName,

};

static const char *SysPpIrregInfoKey[]=
{
	"OverSpeedSmallPercent50Code",
	"OverSpeedSmallPercent50Name",

	"OverSpeedLargePercent50Code",
	"OverSpeedLargePercent50Name",
	
	"ReverseRunCode",
	"ReverseRunName",

	"InfringeProhibCode",
	"InfringeProhibName",

	NULL
};

/**电子警察全局信息配置**/
typedef enum
{
  e_eaElecPoliceName,
  e_eaDirection,
  e_eaPicStyle,
  e_eaLightType,
  e_eaFlashControlType,
  e_eaComId,
  e_eaFlashDevType,
  e_eaFlashAddr,
  e_eaRedArea,
  e_eaRedLow,
  e_eaRedHigh,
  e_eaGreenLow,
  e_eaGreenHigh,
  e_eaYellowLow,
  e_eaYellowigh,
  e_roadRemarks,

  e_eaVLType,
  e_eaValidChanSum,
  e_eaCarDistance,
  e_eaSnapInterval,
  e_eaVLDisFlag,
  e_eaFlashCtlMode,
  e_eaRoadId,
  e_eaDirId,
  
  e_eaXcsProVer,  
  e_eaRLAmendEn,
  e_eaRLDectType,

  e_eaOSDColour,
  e_eaRLAmendLevel,
  e_eaPPSendEn,
  e_eaFreqFlashEn,
  e_eaRedLightModify,
  e_eaFlashTrigMode,
  e_eaPPFlashEn,
  e_eaStrobeFlashFreq,
  e_eaEpDevCode,
  
  e_eaACSyncSwitchEn,
  e_eaACSwitchThreshold,
  e_eaACDecCycle,
  e_eaDayACPhase,
  e_eaNightACPhase,

  e_eaRedModifyMode,
  e_eaExEpDevCode,

  e_eaSmartSwitchEn,
  e_eaTimerSwitchEn,
  e_eaFILightBeginTime,
  e_eaFILightLastTime,

}ELEC_OVERALL;

static const char *ElecPolicOverallConfInfoKey[]=
{
	"ElecPoliceName",
  	"Direction",
  	"PicStyle",
  	"LightType",
  	"FlashControlType",
  	"ComId",
  	"FlashDevType",
  	"FlashAddr",
  	"RedArea",
  	"RedLow",
  	"RedHigh",
  	"GreenLow",
  	"GreenHigh",
  	"YellowLow",
  	"YellowHigh",
  	"RoadRemarks",

	"VlDecType",
	"ValidChanSum",
	"CarDistance",
	"SnapInterval",
	"VLDisFlag",
	"FlashCtlMode",
	"RoadId",
	"DirId",
	
	"EpGXcsProVer",	
	"RLAmendEn",
	"RLDectType",

	"OSDColur",
	"RLAmendLevel",
	"PPPicSendEn",
	"epFreqFlashEn",
	"epRedLightModify",
	"eaFlashTrigMode",
	"PPFlashEn",
	"eaStrobeFlashFreq",
	"eaEpDevCode",

	"eaACSyncSwitchEn",
	"eaACSwitchThreshold",
	"eaACDecCycle",
	"eaDayACPhase",
	"eaNightACPhase",
	"eaRedModifyMode",
	"eaExEpDevCode",
	
	"eaSmartSwitchEn",
	"eaTimerSwitchEn",
	"eaFILightBeginTime",
	"eaFILightLastTime",
	NULL
};
/**电子警察 车道信息配置**/
typedef enum
{
	e_evVersion,
	e_evVehicleId,
	e_evCaptionX,
	e_evCaptionY,
	e_evVehicleName,
	e_evAreaStartX,
	e_evAreaStartY,
	e_evAreadWidth,
	e_evAreadHeight,
	e_evRedDetectType,
	e_evRedDetectConnectMode,
	e_evRedDetectDesp,
	e_evCoilType,
	e_evVehicleDetectConnectMode,
	e_evCoilDesp,
	e_evForceRedLight,

	e_evVLPoint,
	e_evLaneLightArea,
	e_evLightType,
	e_evLaneDir,
	e_evVLDisFlag,
	e_evLaneDisFlag,
	e_evLaneId,
	
	e_evCarDistance,
	e_evSnapInterval,

	e_evCoilDecType,
	e_evCoilSwitchFlag,
	e_evCoilSwitchSta,
	e_evRedLightDecSta,
	e_evFlashType,
	e_evInLightSta,
	e_evVLDecEn,
	e_evNoDir,
	e_evCarTrackEn,
	e_evCarSnapDir,
	
	e_evMinPlatePix,
	e_evMaxPlatePix,
	e_evEveningThresh,
	e_evCoilDistance,
	e_evPPDelaySnapFrames,
	e_evSwitchLaneEn,
	e_evSwitchLaneLeftNo,
	e_evSwitchLaneRightNo,	
}ELEC_POLICE_VEHICLE_CONF;
static const char *ElecPolicVehicleConfInfoKey[]=
{
	"Version",
	"VehicleId",
	"CaptionX",
	"CaptionY",
	"VehicleName",
	"AreaStartX",
	"AreaStartY",
	"AreadWidth",
	"AreadHeight",
	"RedDetectType",
	"RedDetectConnectMode",
	"RedDetectDesp",
	"CoilType",
	"VehicleDetectConnectMode",
	"CoilDesp",
	"ForceRedLight",

	"VLoopPoint",
	"LaneLightArea",
	"LightType",
	"LaneDir",
	"VLDisFlag",
	"LaneDisFlag",
	"LaneId",

	"evCarDis",
	"evSnapInt",

	"evCoilDecType",
	"evCoilSwitchFlag",
	"evCoilSwitchSta",
	"evRedLightDecSta",
	"evFlashType",

	"evInLightSta",
	"evVLDecEn",
	"evNoDir",
	"evCarTrackEn",
	"evCarSnapDir",

	"evMinPlatePix",
	"evMaxPlatePix",
	"evEveningThresh",
	"evCoilDistance",
	"evPPDelaySnapFrames",

	"evSwitchLaneEn",
	"evSwitchLaneLeftNo",
	"evSwitchLaneRightNo",
	
	NULL
};
/**虚拟线圈高级控制信息**/
typedef enum
{
	e_vlVersion,
	e_vlVehicleId,
	e_vlThrDayToNight,
	e_vlThrNightToDay,
	e_vlCorrNightThresh,
	e_vlCorrDayThresh,
	e_vlCorrMotorThresh,
	e_vlCorrEvenThresh,
	e_vlDayMotorWidth,
	e_vlNightMotorWidth,
	e_vlCapDelayInterval,
	e_vlCapDelayNum,
	e_vlMinPlatePixelSize,
	e_vlMaxPlatePixelSize,
};
static const char *VLConfInfoKey[]=
{
	"Version",
	"VehicleId",
	"ThrDayToNight",
	"ThrNightToDay",
	"CorrNightThresh",
	"CorrDayThresh",
	"CorrMotorThresh",
	"CorrEvenThresh",
	"DayMotorWidth",
	"NightMotorWidth",
	"CapDelayInterval",
	"CapDelayNum",
	"MinPlatePixelSize",
	"MaxPlatePixelSize",
	NULL
};

/**NTP 配置信息**/
typedef enum
{
	e_ntEnable = 0,
	e_ntNtpServer,
	e_ntpJudgeCycle,
	e_ntpJudegPrecision,
};
static const char *NtpConfInfoKey[]=
{
	"Enable",
	"NtpServer",
	"NtpJudgeCycle",
	"NtpJudgePrecision",
	NULL
};

/**网络硬盘配置信息**/
typedef enum
{
	e_hdEnable = 0,
	e_hdType,
	e_hdIpAddr,
	e_hdFolder,
	e_hdUserName,
	e_hdPassWord,
	e_hdAvaiable,
	e_uDiskCheckSum,
};
static const char *NetHddConfInfoKey[]=
{
	"Enable",
	"Type",
	"IpAddr",
	"Folder",
	"UserName",
	"PassWord",
	"Avaiable",
	"uDiskCheckSum",
	NULL
};

/**云存储NRM配置信息**/
typedef enum
{
	e_NRMEn = 0,
	e_MainNRMIP,
	e_BakNRMIP,
	e_MainNRMPort,
	e_BakNRMPort,
	e_NRUPUId,
	e_LastNRUIp,
	e_LastNRUHttpPort,
};
static const char *NrmConfInfoKey[]=
{
	"NRMEn",
	"MainNRMIP",
	"BakNRMIP",
	"MainNRMPort",
	"BakNRMPort",
	"NRUPUId",
	"LastNRUIp",
	"LastNRUHttpPort",
	NULL
};

/**卡口功能是否使能信息**/
typedef enum
{
	e_OsdPlateScopeEn,
    e_PlateScopeIncSize,
	e_RadarSpeedGap,
	e_RadarForwardLim,
	e_RadarBackwardLim,
	e_isShieldReverse,
	e_isDecTaillight,
	e_faceIdenEn,
	e_OlviaRadaParalEn,
	e_faceOverlayEn,
	e_isReverseSnapNorm,
};

static const char *PpFuncEnableKey[]=
{
	"OsdPlateScopeEn",
    "PlateScopeIncSize",
	"RadarSpeedGap",
	"RadarForwardLim",
	"RadarBackwardLim",
	"isShieldReverse",
	"isDecTaillight",
	"faceIdenEn",
	"OlviaRadaParalEn",
	"faceOverlayEn",
	"usIsReverseSnapNorm",
	NULL
};

/**系统功能是否使能信息**/
typedef enum
{
	e_flowRate =0,
	e_alarmRec,
	e_speedAmend,
	e_flowRateInter,
	e_xmlFileSendFlag,
	e_flashStartLine,
	
	e_dspFrmDiffEn,
	e_dspStopDistance,
	e_dspOneLampEn,
	e_redLightDelay,
	e_redTurnGreenSnapEn,
	e_noDirReportSnapEn,
	e_freqFlashDelaySec,
	e_ppFlashLightDelaySec,
	e_epFlashLightDelaySec,
	e_debugShellLogSaveFlag,
	e_arithDebugShowFlag,
	e_vdlGreenDectFlag,
	e_pressLineSanpFlag,
	e_backWordsSanpFlag,
	e_epPpSnap0Or1,
	e_UdiskWeldSta,
	e_freqFlashStartLine,
	e_epCarSpeedOsdEn,
	e_uDiskEn,
	e_EpPpSnapPosition,
	e_RedLReviseShutter,
	e_NetVehiclePort,
	e_UFdiskEnFlag,

	e_GammaVal,
	e_flashLinesMode,
	e_DyncJudgeFlashLines,

	e_speedAmendX,
	e_speedAmendY,

	e_ppCsSpeedAmend,

	e_epPpPicMode,

	e_fpgaSatuPam,
	e_fpgaGPixThresh,

    e_dubalFModeEn,
    e_dubalFShutVal,
    e_dubalFGainVal,

    e_redDectSaturation,
    e_redDectBright,
    e_PlateSegFlag,
    e_PicQuality,
    e_RedLReviseDegree,
    e_ChangeRoadSnapFlag,
    e_crSnapMode,
    e_PicAssistDebug,
    e_CrossLineParkEn,
    e_cPressLine,

	e_SDEn,
	e_SDFdiskEn,
	e_cOnvifEn,
    e_cSipSdkEn,
    e_cV2En,
    e_cV3En,

	e_HaloTakeOutEn,
	e_HaloAreaRange,

	e_cGB28181En,
	
	e_RestartTimeEn,
	e_RestartTime,
};
static const char *SysFuncEnableKey[]=
{
	"FlowRateFlag",
	"AlarmRecFlag",
	"SpeedAmendFlag",
	"FlowRateInter",
	"xmlFileSendFlag",
	"flashStartLine",
	
	"dspFrmDifEn",
	"dspStopDistan",
	"dspOneLanpEn",
	"redLightDelaySec",
	"redTurnGreenSnapEn",
	"noDirReportSnapEn",
	"freqFlashDelaySec",
	"ppFlashLightDelaySec",
	"epFlashLightDelaySec",
	"debugLogSaveFlag",
	"arithDebugShowFlag",
	"vdlGreenDecFlag",
	"pressLineSanpFlag",
	"backWordsSanpFlag",
	"epPpSnap0Or1",
	"UdiskWeldSta",
	"freqFlashStartLine",
	"epCarSpeedOsdEn",
	"uDiskEn",
	"epPpSnapPosition",
	"epRedLReviseShutter",
	"NetVehiclePort",
	"UFdiskEnFlag",
	
	"GammaVal",
	"FlashLinesMode",
	"DyncJudgeFlashLines",

	"speedAmendX",
	"speedAmendY",

	"ppCsSpeedNumerator",

	"epPpPicMode",

	"fpgaSatuPam",
	"fpgaGPixThresh",

    "dubalFModeEn",
    "dubalFShutVal",
    "dubalFGainVal",

    "redDectSaturation",
    "redDectBright",
    "PlateSegFlag",
    "PicQuality",

    "RedLReviseDegree",
    "ChangeRoadSnapFlag",
    "crSnapMode",
    "PicAssistDebug",
    "CrossLineParkEn",
    "cPressLine",
	"SDEn",
	"SDFdiskEn",
	
    "cOnvifEn",
    "cSipSdkEn",
    "cV2En",
    "cV3En",
	"HaloTakeOutEn",
	"HaloAreaRange",

	"cGB28181En",
	"cRestartTimeEn",
	"uiRestartTime",
    NULL
};

/**电警虚拟线圈抓拍步数间隔**/
typedef enum
{
	e_lane0Stp01 = 0,
	e_lane0Stp12,
	e_lane1Stp01,
	e_lane1Stp12,
	e_lane2Stp01,
	e_lane2Stp12,
	e_lane3Stp01,
	e_lane3Stp12,
};
static const char *SysVEpStepInterKey[]=
{
	"EpLane0Step01",
	"EpLane0Step12",
	"EpLane1Step01",
	"EpLane1Step12",
	"EpLane2Step01",
	"EpLane2Step12",
	"EpLane3Step01",
	"EpLane3Step12",
	NULL
};

/**电警车道禁止行驶时间段信息**/
typedef enum
{
	e_cEpForbitType = 0,
	e_cEpRange1Enable,
	e_cEpRange2Enable,
	e_cEpBeginTime1,
	e_cEpLastTime1,
	e_cEpBeginTime2,
	e_cEpLastTime2,
};

static const char *EpTimeRangeKey[]=
{
	"EpForbitType",
	"EpRange1Enable",
	"EpRange2Enable",
	"EpRBeginT1",
	"EpRLastT1",
	"EpRBeginT2",
	"EpRLastT2",
	NULL
};

/**电警车道辅助红灯配置信息**/
typedef enum
{
	e_cEpAssistRedLight = 0,
	e_cEpAssistRedLightNum,	
	e_cEpARLEnable,
	e_cARLBeginTime1,
	e_cARLEndTime1,
	e_cARLBeginTime2,
	e_cARLEndTime2,
	e_cARLBeginTime3,
	e_cARLEndTime3,
};

static const char *EpAssistRedLightKey[]=
{
	"EpAssistRedLight",
	"Num",
	"Enable",
	"BeginT1",
	"LastT1",
	"BeginT2",
	"LastT2",
	"BeginT3",
	"LastT3",
	NULL
};

/**电警车道遇堵禁入配置信息**/
typedef enum
{
	e_cEpSOTNEEnable=0,
	e_cSOTNEBeginTime1,
	e_cSOTNEEndTime1,
	e_cSOTNEBeginTime2,
	e_cSOTNEEndTime2,
	e_cSOTNEBeginTime3,
	e_cSOTNEEndTime3,
};

static const char *EpSOTNoEntryKey[]=
{
	"Enable",
	"BeginT1",
	"LastT1",
	"BeginT2",
	"LastT2",
	"BeginT3",
	"LastT3",
	NULL
};

/**电警违章类型描述及代码配置**/
typedef enum
{
	e_epIrregDispFlag = 0,
	
	e_epRunRDCode,
	e_epRunRDName,
	e_epReverseRunCode,
	e_epReverseRunName,
	
	e_epInfringForbidLineCode,
	e_epInfringForbidLineName,

	e_epInfringDirLaneRunCode,
	e_epInfringDirLaneRunName,

	e_epStopOnTrafficNoEntryCode,
	e_epStopOnTrafficNoEntryName,

	e_HighwayInfringDirLaneCode,
	e_HighwayInfringDirLaneName,

	e_StoppedVehicleCode,
	e_StoppedVehicleName,
};

static const char *SysEpIrregInfoKey[]=
{
	"epIrregDispFlag",
	
	"epRunRDCode",
	"epRunRDName",
	"epReverseRunCode",
	"epReverseRunName",
	
	"epInfringForbidLineCode",
	"epInfringForbidLineName",

	"epInfringDirLaneRunCode",
	"epInfringDirLaneRunName",

	"epStopOnTrafficNoEntryCode",
	"epStopOnTrafficNoEntryName",

	"HighwayInfringDirLaneCode",
	"HighwayInfringDirLaneName",

	"StoppedVehicleCode",
	"StoppedVehicleName",

	NULL
};


/**电警新增违章类型描述及代码配置**/
typedef enum
{
	e_epRunInNoLaneCode,
	e_epRunInNoLaneName,
};

static const char *SysEpNewAddIrregInfoKey[]=
{
	"epRunInNoLaneCode",
	"epRunInNoLaneName",

	NULL
};

/**电警车牌识别配置信息**/
typedef enum
{
	e_evLPREn,
	e_evLPRFirstCode,
	e_evLPRCConfiden,
	e_evLPRWidth,
	e_evLPRHeight,
};


static const char *epLPRConfInfoKey[]=
{
	"evLPREn",
	"evLPRFirstCode",
	"evLPRCConfiden",
	"evLPRWidth",
	"evLPRHeight",
	NULL
};

/** 电警图片osd叠加配置 **/
typedef enum
{
	e_epDevSnOsdEn,
	e_epRoadNameOsdEn,
	e_epDirNameOsdEn,
	e_epLaneNameOsdEn,
	e_epSnapTimeOsdEn,
	e_epBusNumOsdEn,
	e_epLPColorOsdEn,
	e_epCarTypeOsdEn,
	e_epCarColorOsdEn,
	e_epReguCodeOsdEn,
	e_epStandReguDesOsdEn,
	e_epIntelReguDesOsdEn,
	e_epRDBeginTimeOsdEn,
	e_epRDCycleOsdEn,
	e_epRDDespOsdEn,
};

static const char *epJpgOsdConfInfoKey[]=
{
	"epDevSnOsd",
	"epRoadNameOsd",
	"epDirNameOsd",
	"epLaneNameOsd",
	"epSnapTimeOsd",
	"epBusNumOsd",
	"epLPColorOsd",
	"epCarTypeOsd",
	"epCarColorOsd",
	"epReguCodeOsd",
	"epStandReguDesOsd",
	"epIntelReguDesOsd",
	"epRDBeginTimeOsd",
	"epRDCycleOsd",
	"epRDDespOsd",
	NULL
};

/** 卡口图片osd叠加配置 **/
typedef enum
{
	e_paDevSnOsdEn,
	e_paPpNameOsdEn,
	e_paDirNameOsdEn,
	e_paLaneNameOsdEn,
	e_paSnapTimeOsdEn,
	e_paCarDirOsdEn,
	e_paBusNumOsdEn,
	e_paLPColorOsdEn,
	e_paCarTypeOsdEn,
	e_paCarColorOsdEn,
	e_paCurSpeedOsdEn,
	e_paPunishSpeedOsdEn,
	e_paMarkSpeedOsdEn,
	e_paSpeedRatioOsdEn,
	e_paReguCodeOsdEn,
	e_paStandReguDesOsdEn,
	e_paIntelReguDesOsdEn,
};

static const char *paJpgOsdConfInfoKey[]=
{
	"DevSnOsd",
	"PpNameOsd",
	"DirNameOsd",
	"LaneNameOsd",
	"SnapTimeOsd",
	"CarDirOsd",
	"BusNumOsd",
	"LPColorOsd",
	"CarTypeOsd",
	"CarColorOsd",
	"CurSpeedOsd",
	"PunishSpeedOsd",
	"MarkSpeedOsd",
	"SpeedRatioOsd",
	"ReguCodeOsd",
	"StandReguDesOsd",
	"IntelReguDesOsd",

	NULL
};

/**摄像机时间段信息**/
typedef enum
{
	e_camTGroup = 0,
	e_camTEnable,
	e_camTNmae,
	e_camTBeginTime,
	e_camTEndTime,
	e_irisPos,
};
static const char *CamTimeRangeKey[]=
{
	"GroupId",
	"EnableFlag",
	"CamTName",
	"CamTBeginT",
	"CamTEndT",
	"IrisPos",
	NULL
};

/**摄像机控制配置信息**/
typedef enum
{
	e_camType = 0,
	e_camMaxVal,
	e_camMinVal,
	e_camFixVal,
	e_camManner,
	e_camGroup,
};
static const char *CameraCtrlConfInfoKey[]=
{
	"Type",
	"MaxVal",
	"MinVal",
	"FixVal",
	"Manner",
	"GroupNum",
	NULL
};


/**FTP 配置信息**/
typedef enum
{
	e_fEnable = 0,
	e_fFtpIp,
	e_fFtpPort,
	e_fUserName,
	e_fUserPasswd,
	e_nEpTimerEnable,
	e_nEpTimerRecInt,
    e_nEpTimerRecLast,
};
static const char *FtpConfInfoKey[]=
{
	"Enable",
	"FtpIp",
	"FtpPort",
	"UserName",
	"UserPasswd",
	"TRecEnable"
	"intervalTime",
    "lastTime",
	NULL
};

/**系统配置信息**/
typedef enum
{
	e_BootMode = 0,
	e_DiskCheck
};


/**外设 配置信息**/
typedef enum
{
	e_dEnable = 0,
	e_dComId,
	e_dVehDect,
	e_dDatabits,
	e_dStopbits,
	e_dParity,
	e_dBaudrate,
	e_dMaxMeasureSpeed
};
static const char *DctlConfInfoKey[]=
{
	"Enable",
	"ComId",
	"VehDect",
	"Databits",
	"Stopbits",
	"Parity",
	"Baudrate",
	"MaxMeasureSpeed",
	NULL
};

/**  卡口平台 配置信息**/
typedef enum
{
	e_p330Enable = 0,
    e_p330DriveWayId,
	e_p330DevId,
	e_p330DirId,
	e_p330Key
};
static const char *PlatformConfInfoKey[]=
{
	"p330Enable",
    "p330DriveWayId",
	"p330DevId",
	"p330DirId",
	"p330Key",
	NULL
};

/**电警平台 配置信息**/
typedef enum
{
	e_epP330Enable = 0,
    e_epP330DriveWayId,
	e_epP330DevId,
	e_epP330DirId,
	e_epP330Key
};
static const char *ep330ConfInfoKey[]=
{
	"epP330Enable",
    "epP330DriveWayId",
	"epP330DevId",
	"epP330DirId",
	"epP330Key",
	NULL
};

/* begin add by wangb 2010-3-22 for v3 platform */
/* v3平台接入参数配置信息 */
typedef enum
{
	e_v3ServerIp = 0,
    e_v3ServerPort,
	e_v3DevNo,
	e_v3DevPwd,
	e_v3ServerSeq,
	e_v3TimeSyn,
	e_v3Enable
};

static const char *v3ConfInfoKey[]=
{
	"v3ServerIp",
    "v3ServerPort",
	"v3DevNo",
	"v3DevPwd",
	"v3ServerSeq",
	"v3TimeSyn",
	"v3Enable",
	NULL
};
/* end add by wangb 2010-3-22 for v3 platform */

/* begin add by wangb 2012-1-10 for v2 platform */
/* v2平台接入参数配置信息 */
typedef enum
{
	e_v2ServerIp = 0,
    e_v2ServerPort,
	e_v2DevNo,
	e_v2DevPwd,
	e_v2ServerSeq,
	e_v2TimeSyn,
	e_v2Enable
};

static const char *v2ConfInfoKey[]=
{
	"v2ServerIp",
    "v2ServerPort",
	"v2DevNo",
	"v2DevPwd",
	"v2ServerSeq",
	"v2TimeSyn",
	"v2Enable",
	NULL
};

/* end add by wangb 2012-1-10 for v2 platform */

typedef enum
{
	e_Gb28181ServerId,
	e_Gb28181ServerDomain,
	e_Gb28181ServerIp,
	e_Gb28181ServerPort,
	e_Gb28181RegisterExpiry,
	e_Gb28181KeepLiveTime,
	e_Gb28181ConMode,
	e_Gb28181ClientId,
	e_Gb28181ClientUserName,
	e_Gb28181ClientPasswd,
	e_Gb28181PackMode,
	e_Gb28181EnFlag,
};

static const char *gb28181ConfInfoKey[]=
{
	"ServerId",
	"ServerDomain",
	"ServerIp",
	"ServerPort",
	"RegisterExpiry",
	"KeepLiveTime",
	"ConMode",
	"ClientId",
	"ClientUserName",
	"ClientPasswd",
	"PackMode",
	"EnFlag",
	NULL
};

/* begin add by wangb 2010-8-6 for nm platform */
/* 网管平台接入参数配置信息 */
typedef enum
{
	e_nMServerIp = 0,
    e_nMServerPort,
    e_nMEnable,
    e_defaultGWEn,
    e_defaultGWIp,
    e_defaultHttpPort,
};

static const char *nmConfInfoKey[]=
{
	"nmServerIp",
    "nmServerPort",
    "nmEnable",
    "defaultGWEn",
    "defaultGWIp",
    "defaultHttpPort",
	NULL
};
/* end add by wangb 2010-3-22 for v3 platform */

typedef enum
{
    e_appmode
};

static const char *AppModeConfInfoKey[]=
{
	"appmode",
     NULL
};


#define  SYS_ASSERT(min,max)   \
do {\
        if ( (index <min) ||(index >= max)) \
        {\
            return -1;\
        }\
}while(0);


#endif

