/******************************************************************************
 * SysConf.h:
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
#ifndef _SYS_CONF_H__
#define _SYS_CONF_H__

#include "xcsGlobalDef.h"
#include "MmsSendDefine.h"

#define _PACKED_ __attribute__((packed))


typedef enum
{
	e_NET_CONF = 0x0,                   /* 网络配置信息 */
	e_RTSP_CONF,                        /* RTSP配置信息 */
	e_VIDEO_CONF,                       /* 视频配置信息 */
	e_REC_CONF,                         /* 录像配置信息 */
	e_PASSPORT_OVERALL_CONF,            /* 卡口全局配置信息 */
	e_PASSPORT_VEHICLE_CONF,            /* 卡口车道配置信息 */
	e_ELEC_POLICE_OVERALL_CONF,         /* 电子全局警察配置 */
	e_ELEC_POLICE_VEHICLE_CONF,         /* 电子警察车道配置信息 */
	e_VL_CONF,                          /* 虚拟线圈高级配置信息 */
	e_PASSPORT_FTP_CONF,                /* 卡口FTP 配置信息 */
	e_ELEC_POLICE_FTP_CONF,             /* 电子警察ftp 配置信息 */
	e_NTP_CONF,                         /* NTP 配置信息 */
	e_CAMERA_CTRL_CONF,                 /* 摄像机控制配置信息 */
	e_PIC_NET_HDD_CONF,                 /* 图片网络硬盘配置信息 */
	e_REC_NET_HDD_CONF,                 /* 录像网络硬盘配置信息 */
	e_DCTL_CONF,                        /* 外设配置信息 */
	e_PLATFORM_CONF,                    /* 平台配置信息 */
	e_V3_CONF,                          /* V3平台配置信息 */
	e_SYS_DEF_CONF,                     /* 系统默认配置信息 */
	e_APPMODE_CONF,                     /* APP Mode 配置信息 */
    e_NM_CONF,                          /* 网管配置信息 */
    e_CAMERA_TIME_RANGE,                /* 摄像机时间段控制 */
    e_SYS_FUNC_ENABLE_CONF,				/* 系统功能是否使能标志 */
    e_EP_RANGE_TIME_CONF,				/* 电警车道禁止行驶时间段配置 */
    e_EP_V_LOOP_STEP_INVER,				/* 电警虚拟线圈抓拍间隔设置 */
    e_EP_330_CONF,						/* 电警330配置 */
    e_V2_CONF,                          /* V2平台配置信息 */
    e_EP_ASSIST_REDLIGHT_CONF,			/* 电警辅助红灯参数配置 */	
    e_EP_IRREG_INFO_CONF,				/* 电警违章类型信息配置 */
    e_EP_LPR_INFO_CONF,					/* 电警车牌识别信息配置 */
    e_PP_FUNC_ENABLE_CONF,				/* 卡口相关功能配置信息 */
    e_EP_SOT_NOENTRY_CONF,				/* 电警遇堵禁入信息配置 */
    e_PP_JPG_OSD_CONF,					/* 卡口图片osd信息叠加配置 */
    e_EP_JPG_OSD_CONF,					/* 电警图片osd信息叠加配置 */
    e_PP_IRREG_INFO_CONF,				/* 卡口违章类型信息配置 */
    e_PASSPORT_BACKUPS_FTP_CONF,        /* 备份卡口FTP 配置信息 */
    e_NET_REC_MANAGE_CONF,				/* 云存储NRM设备信息配置 */
    e_EP_BACKUPS_FTP_CONF,        		/* 备份电警FTP 配置信息 */
    e_EP_NEWADD_IRREG_INFO_CONF,		/* 电警新增违章类型信息配置 */
    e_GB28181_CONF,                     /* Gb28181平台配置信息 */
}CONF_TYPE;


/* Net Conf */
typedef struct NetConfInfo
{
	unsigned int    uiIPAddr;			//IP地址
	unsigned int	uiNetMask;			//子网掩码
	unsigned int	uiGateWay;			//网关
} NET_CONF_INFO;

/* Rtsp Conf */
#if 0
typedef struct RtspConfInfo
{
	char            cSupportDownLoad ;          /* 设备是否支持录像下载 */
    char            cDownLoadType ;             /* 录像下载类型         */
	char			cRsv[2];
    unsigned int    uiProxyDownLoadIp;          /* 录像下载IP地址       */
    unsigned short  usProxyDownLoadPort;        /* 录像下载端口         */
    char            cSupportReplay;             /* 设备是否支持回放     */
    char            cReplayType;                /* 回放类型             */
    unsigned int    uiProxyReplayIp;            /* 回放IP               */
    unsigned short  usProxyReplayPort;          /* 回放端口             */
    char			cRsv1[2];
} RTSP_CONF_INFO;
#else
typedef struct tagRtspConfInfo
{
	int nRtspMode;					//0=NAT方式, 1=本地RTSP,2=代理RTSP 
	unsigned int dwRtspIpAddr;			//RTSP服务器地址
	unsigned int dwRtspPort;			//RTSP服务器端口
	unsigned int dwRtspDownLoadPort;    //RTSP服务器下载端口
	char	szRtspPath[32];				//RTSP路径
	char	szRtspUsername[20];			//RTSP 用户名
	char	szRtspPassword[20];			//RTSP 密码
} RTSP_CONF_INFO;
#endif
/* Video Conf */
/* 2.7 视频参数查询 */
typedef struct StrVChannelVParamQuery
{
    unsigned short  usChannel;          /* 视频通道号 从1开始 */
    char            cVQuality;          /* 视频质量 1~5 */
    char            cFrameRate;
    unsigned int    uiBitRate;
    char            cKeyFInterval;      /* 关键帧间隔 */
    char            cImageResol;        /* 图像分辨率 */
    char            cEncodeType;
    char            cVBR;               /* 是否变码率 */
    unsigned short  usXPos;             /* 字幕在屏幕上坐标的x值 */
    unsigned short  usYPos;             /* 字幕在屏幕上坐标的y值 */
    char            szContent[64];      /* 字幕内容 */

    unsigned short  usDebugFrameX;      /* 调试框选位置X */
    unsigned short  usDebugFrameY;      /* 调试框选位置Y */
    
    unsigned char   ucDebugFrameEn;		/* 调试框选使能标志 */
    unsigned char   ucTimeDisMode;		/* 时间显示格式 0:无毫秒 1:有毫秒 */
	unsigned short  usTimeXPos;			/* 时间显示坐标 X */

	unsigned short  usTimeYPos;			/* 时间显示坐标 Y */
	char			cRsv[2];
}_PACKED_  VIDEO_CONF_INFO;



/* Rec Conf */
typedef struct TimeSegment{
	unsigned int uiStartTime;
	unsigned int uiEndTime;
}TimeSegment;

typedef struct PlantTimeDescription{
	int bIsValid;
	TimeSegment ts[DEFAULT_PLANT_TIME_SEGMENT];
}PTD;

typedef struct RecConfInfo
{
	unsigned char	nChannelID;         /*  通道号，从1开始      */
	char			cRsv[3];
	unsigned int	nSysRecMode;        /*  0 禁止录像           */
										/*  全天录像             */
										/*  报警录像             */
										/*  计划录像             */
	unsigned int	nLoopRecFlag;       /*  1 磁盘满就停         */
	                                    /*  2 循环覆盖           */
	                                    /*  3 保留指定天数录像   */
    int    nPreRecSec;
    int    nRecSpace;
	int	nStorageRecDays;
	char szRecPath[DEFAULT_REC_DISK_PER_CHANNEL][DEFAULT_MAX_PATH_LEN];
	PTD ptdManualRecTime;
	PTD ptdOncePlanRecTime;
	PTD ptdDayPlanRecTime;
	PTD ptdWeekPlanRecTime[7];
	unsigned char nRecFileFormat;
	char		  cRsv1[3];
	
}REC_CONF_INFO;


/* PassPort Overall Conf */
typedef struct tagSTR_PASSPORT_OVERALL_CONF_INFO
{
	char    szPassPortName[32];             /* 卡口名 */
	char    szDirection[32];                /* 方向信息 */
	char    szPassPortId[8];				/* 卡口ID */
	char	cOSDColor;				 		/* 图片叠加OSD颜色 */
	char    cValidLanes;			 		/* 实际使用车道数 */
	char    cVLWorkMode;			 		/* 虚拟线圈工作模式 */

	unsigned char ucLPREn;					/* 车牌识别使能标志 */
	char    szLPRFirstChar[32];				/* 车牌首字符 */
	unsigned char ucLPRCConfidence;			/* 车牌首字符置信度 */
	unsigned char ucVSpeedMeaFlag;			/* 视频测速使能标志 */
	unsigned short usVSpeedMeaLen;			/* 视频测速实际距离(单位:厘米) */

    unsigned char    aucFreqFlashEn[4];      /* 是否使能频闪功能 */
    unsigned char    aucFlashTrigMode[4];    /* 闪光灯触发方式 */
    unsigned char    aucStrobeFlashFre[4];   /* 频闪灯倍频系数 */
    char            szPpDevCode[MAX_DEV_CODE_LEN];/* 卡口设备编号 */
    char     szDirId[8];                        /* 方向ID */
    
    unsigned char   ucFillInLSwitchEn;        /* 补光切换使能标志 */
    unsigned char   ucGrayThresholdVal;        /* 切换灰度阀值 */
    unsigned char    ucCheckSwitchCycle;        /* 检测视频灰度值周期 */
    char            Rsv1;
    
    char            szExPpDevCode[EXT_DEV_CODE_LEN];/* 扩展设备编号字符串 */

	unsigned char	ucSmartSwitchEn;		/* 补光智能切换使能 */
	unsigned char	ucTimerSwitchEn;		/* 补光定时切换使能 */
	char			cRsv2[2];
	unsigned int	uiFillInLightBeginTime;	/* 补光起始时间 */
	unsigned int	uiFillInLightLastTime;	/* 持续时间 */    
}PASSPORT_OVERALL_CONF_INFO;

/* PassPort Vehicle Conf */
/* 卡口车道信息配置 */



/* nCoilDesp COM 类型时的结构定义 */
typedef struct tagSTR_SYS_COIL_COM_DESP
{
    char    cComId;
    char    cBoardId;                    /*板卡号*/
    char    cFrontCoilId;               /* 车检器前线圈 */
    char    cBehindCoilId;              /* 车检器后线圈 */
    char    cGroupId;
	char 	cRsv[3];
}_PACKED_ STR_SYS_COIL_COM_DESP;

/* nCoilDesp GPIO 类型时的结构定义 */
typedef struct STRCOILGPIODESP
{   
    char    cBoardId;                /*板卡号*/
    char    cFrontCoilId;           /* 车检器前线圈 */
    char    cBehindCoilId;          /* 车检器后线圈 */
	char 	cRsv[1];
}_PACKED_ STR_SYS_COIL_GPIO_DESP, *PSTR_SYS_COIL_GPIO_DESP;

typedef union STR_SYS_COIL_DESP
{
    STR_SYS_COIL_COM_DESP   strComCoilDesp;
    STR_SYS_COIL_GPIO_DESP  strGpioCoilDesp;
}_PACKED_ STR_SYS_COIL_DESP, *PSTR_SYS_COIL_DESP;


/* 所有协议里面关于范围的定义，均用此结构 */
typedef struct tagStrArea
{
    unsigned short usAreaPosX;      /* 区域左上角坐标X */
    unsigned short usAreaPosY;      /* 区域左上角坐标Y */
    unsigned short usAreaWidth;     /* 区域宽度 */
    unsigned short usAreaHeight;    /* 区域高度 */
}_PACKED_ STR_SYS_AREA, *PSTR_SYS_AREA;

typedef struct tagStrTimeSlotSnapObj
{
    unsigned int uiBeginTimeSecs;	/* 起始时间 */
    unsigned int uiLastTimeSecs;	/* 持续时间 */
    unsigned char ucSnapObj;		/* 抓拍目标 */
    unsigned char ucRsv[3];
}_PACKED_ STR_SYS_TIME_SLOT_SNAP_OBJ, *PSTR_SYS_TIME_SLOT_SNAP_OBJ;


typedef struct STR_SYS_PORT_VEHICLE_CONF_INFO
{
	unsigned int        uiCarRoadId;            /* 车道号 */
	unsigned short      usOSDPosX;              /* OSD显示x坐标 */
	unsigned short	    usOSDPosY;              /* OSD显示y坐标 */
	char 			    szOSDContent[32];       /* OSD内容 */
	STR_SYS_AREA        strRoadArea;
	unsigned short		usFlashLightType;       /* 闪光灯/补光灯设置 */
	unsigned short		usFlashLightCtlType;    /* 闪光灯控制方式 */
	char    		    cComId;                 /* 闪光灯控制串口号 */
	char    		    cFlashDevType;          /* 闪光灯设备类型 */
	char				cFlashCtlMode;			/* 闪光灯控制模式 */
	char				cSingalCoilPos;			/* 单线圈位置定义 */
	unsigned short		ausFlashAddr[4];        /* 闪光灯485地址 */
	unsigned short		usDetectType;           /* 线圈类型 */
    unsigned short      usIrreSpeed;            /* 违章速度门限 */    
	STR_SYS_AREA        strVLArea;
	unsigned short		usVehicleType;          /* 车检器类型 */
	unsigned short   	usVehicleLinkMode;      /* 车检器连接方式 */
	STR_SYS_COIL_DESP   strCoilDesp;            /* 地感线圈描述 */
	unsigned short		usMaxSpeed;             /* 超速最大门限 */
	unsigned short		usMinSpeed;             /* 超速最低门限 */
    unsigned int        uiCoilDist;             /* 线圈之间的距离，厘米为单位 */
    char				cVLDispFlag;			/* 虚拟线圈是否显示标志 */
    char				cLaneDispFlag;			/* 车道范围线圈是否显示标志 */
	char				cGroudCoilDecType;		/* 地感线圈检测类型:双线圈或单线圈 */
	char				cSingalCoilSnapType;	/* 单线圈抓拍类型:进线圈或出线圈抓拍 */
    char				ucRsv4[4];				/* 保留 */

    char				cCoilSwitchFlag;		/* 线圈切换使能标志 */
   	char				cInLightSta;	     	/* 补光方式状态 */
	char				cCoilSwitchSta;		 	/* 线圈检测模式切换状态 */
	char				cVLSnapDir;				/* 虚拟线圈抓拍方向 */

	char 				cVLDecEn;				/* 虚拟线圈检测使能 */
	unsigned short  	usSingalCoilHigh;		/* 单线圈下的前线圈高度 */
    unsigned char 		ucDaySnapObj;			/* 白天抓拍目标类型 */
    
	unsigned char 		ucNightSnapObj;			/* 晚上抓拍目标类型 */
	unsigned char       ucSnapPicNum;			/* 抓拍图片张数配置 */
	unsigned short		usSnapPicInter;			/* 两张图片抓拍间隔时长(单位:毫秒) */

    STR_SYS_TIME_SLOT_SNAP_OBJ strTimeSlotSnapObj;/* 时间段抓拍目标配置 */
    unsigned char		ucInLightEn;      		/* 是否使能辅助补光 */
    char				cRsv5[3];
    char				szLaneId[8];			/* 车道id */
    
    char				cSnapDir;				/* 抓拍方向(0:正向;1:逆向) */
	char				cSDEnFlag;				/* 是否使能 */
	char				cTruckEnFlag;			/* 大货车抓拍使能 */
	char 				cRsv[1];
	unsigned int 		iSDBeginTime1;
	unsigned int 		iSDLastTime1; 			/* 持续时间1 */
	unsigned int 		iSDBeginTime2;
	unsigned int 		iSDLastTime2; 			/* 持续时间2 */	
	unsigned int 		iSDBeginTime3;
	unsigned int 		iSDLastTime3; 			/* 持续时间3 */
}PORT_VEHICLE_CONF_INFO;


typedef struct STR_SYS_RED_LIGHT_AREA
{
    STR_SYS_AREA    strArea;           /* 红灯区域定义 */
} _PACKED_ STR_SYS_RED_LIGHT_AREA, *PSTR_SYS_RED_LIGHT_AREA;


typedef struct STR_SYS_RED_LIGHT_MODE_SET
{
	unsigned char ucLightStrobeEn;		/* 频闪修正是否使能 */
	unsigned char ucLightStrobeFrame;	/* 频闪修正帧数 */
	unsigned char ucSingalColLight;		/* 是否是单色灯(0:单色灯;1:复色灯) */
	unsigned char ucMapRDNo;			/* 映射到红灯检测器的红灯号 */
} _PACKED_ STR_SYS_RED_LIGHT_MODE_SET, *PSTR_SYS_RED_LIGHT_MODE_SET;

/* 卡口违章类型名称及编号配置信息 */
typedef struct STR_PP_IRREG_CONF_INFO
{
	char	szOverSpeedSmallPercent50Code[8];	/* 超速小于50%的违章代码 */
	char	szOverSpeedSmallPercent50Name[64];	/* 超速小于50%的违章名称 */

	char	szOverSpeedLargePercent50Code[8];	/* 超速大于50%的违章代码 */
	char	szOverSpeedLargePercent50Name[64];	/* 超速大于50%的违章名称 */
	
	char	szReverseRunCode[8];				/* 逆行违章代码 */
	char	szReverseRunName[64];				/* 逆行违章名称 */

	char	szInfringeProhibCode[8]; 			/* 机动车违反禁令标志指示的违章代码 */
	char	szInfringeProhibName[64];			/* 机动车违反禁令标志指示的违章名称 */

}PP_IRREG_CONF_INFO;

/* 电子警察全局信息配置 */

/* 由于DSP只支持6区域，所以从16改为了6 */
#define MAX_AREA_NUM               (4)
#define MAX_AREA_GROUP_NUM        (6)
typedef struct tagStrLight
{
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;
    unsigned char ucResv;
}_PACKED_ STR_LIGHT;
typedef struct STR_SYS_ELEC_POLICE_OVERALL_CONF_INFO
{
  char 		                szCarRoadName[32];          /* 道路名 */
  char  	                szDirection[32];            /* 方向 */
  unsigned short  	        usPicStyle;                 /* 图片合成方式 */
  unsigned short  	        usFlashLightType;           /* 闪光灯/补光灯设置 */
  unsigned short  	        usFlashLightCtlType;        /* 闪光灯控制方式 */
  char  	                cComId;                     /* 控制串口号 */
  char  	                cFlashDevType;              /* 闪光灯设备类型 */
  unsigned short  	        ausFlashAddr[4];            /* 闪光灯485地址 */
  unsigned char 			aucFreqFlashEn[4];			/* 频闪灯是否使能标志 */
  unsigned char				aucFlashTrigMode[4];    	/* 闪光灯触发方式 */
  unsigned char				aucStrobeFlashFre[4];		/* 频闪灯倍频系数 */  
  char						szEpDevCode[MAX_DEV_CODE_LEN];						/* 电警设备编号 */
  STR_SYS_RED_LIGHT_AREA   	strRedArea[MAX_AREA_GROUP_NUM][MAX_AREA_NUM];       /* 红灯区域 */
  STR_LIGHT                 strRedLow;                   /*红灯阈值下限*/
  STR_LIGHT                 strRedHigh;                  /*红灯阈值上限*/
  STR_LIGHT                 strGreenLow;                 /*绿灯阈值下限*/
  STR_LIGHT                 strGreenHigh;                /*绿灯阈值上限*/
  STR_LIGHT                 strYellowLow;                /*黄灯阈值下限*/
  STR_LIGHT                 strYellowHigh;               /*黄灯阈值上限*/
  char  	                szRemarks[32];               /* 路口备注信息 */
  char						cDetectType;           	 	 /* 检测线圈类型 */
  char						cValidChanSum;				 /* 实际有效车道总数 */
  char						cCarDistance;				 /* 最后一张图片机动车离停车线距离（单位米） */
  char						cFrameInterval;				 /* 抓拍间隔帧数（范围1~15） */
  char						cLightAmendDisFlag;		     /* 红灯修正线圈是否显示标志 */
  char						cFlashCtlMode;			     /* 闪光灯控制模式 */
  unsigned short            usXcsProVer;			     /* xcs小协议版本号 */
  char						szRoadId[8];			     /* 路口id */
  
  char						cLightAmendEn;				 /* 红灯修正是否使能 */
  char						cRLDectType;				 /* 红灯检测方式 */
  char						cOSDColour;				 	 /* 叠加字体颜色(0:黑白 1:彩色) */
  char						cRLAmendLevel;			 	 /* 红灯修正深浅等级 */
  char						cPPSendEn;				 	 /* 卡口图片是否上报标志 */
  char						cPPFlashEn;					 /* 卡口图片是否闪光标志 */
  char                      cRsv[2];

  char	 					szDirId[8];					 /* 方向id */
  STR_SYS_RED_LIGHT_MODE_SET strRedLightMode[MAX_AREA_GROUP_NUM];/* 红灯修正相关模式配置 */

  char						cACSyncSwitchEn;			 /* AC交流相位切换使能 */
  unsigned char				ucACSwitchThreshold;		 /* AC交流相位切换灰度阀值 */
  unsigned char				ucACDecCycle;				 /* AC交流相位检测周期 */
  
  char						cRedModifyMode;			 	 /* 红灯修正方法：1：最旧版本 2：较旧版本 
															3：新方法（全自动）4：新方法（自动1）
															5：新方法（手动,不修白点）
															6：新方法（手动，修白点）*/
															
  unsigned short			usDayACPhase;				 /* 白天AC交流相位值 */
  unsigned short			usNightACPhase;				 /* 晚上AC交流相位值 */  
  
  char						szExEpDevCode[EXT_DEV_CODE_LEN];/* 扩展设备编号字符串 */

  unsigned char				ucSmartSwitchEn;		/* 补光智能切换使能 */
  unsigned char				ucTimerSwitchEn;		/* 补光定时切换使能 */
  char						cRsv2[2];
  unsigned int				uiFillInLightBeginTime;	/* 补光起始时间 */
  unsigned int				uiFillInLightLastTime;	/* 持续时间 */
  
}ELEC_POLICE_OVERALL_CONF_INFO;

/* 电子警察车道查询信息 */


/* strRedLDesp COM 类型时的结构定义 */
typedef struct STR_SYS_RED_LIGHT_COM_DESP
{
    char    cComId;
    char    cCarRoadId;
	char 	cRsv[2];
}_PACKED_ STR_SYS_RED_LIGHT_COM_DESP, *PSTR_SYS_RED_LIGHT_COM_DESP;

/* strRedLDesp GPIO 类型时的结构定义 */
typedef struct STR_SYS_RED_LIGHT_GPIO_DESP
{
    char    cGpioId;
	char 	cRsv[3];
}_PACKED_ STR_SYS_RED_LIGHT_GPIO_DESP, *PSTR_SYS_RED_LIGHT_GPIO_DESP;

typedef union STR_SYS_EP_RED_LIGHT_DESP
{
    STR_SYS_RED_LIGHT_COM_DESP  strEPComRedLightDesp;
    STR_SYS_RED_LIGHT_GPIO_DESP strEPGpioRedLightDesp;
}_PACKED_ STR_SYS_EP_RED_LIGHT_DESP, *PSTR_SYS_EP_RED_LIGHT_DESP;

/* 电警虚拟线圈点坐标结构定义 */
typedef struct STR_VL_POINT
{
	unsigned short usVLX1;
	unsigned short usVLY1;
}_PACKED_ STR_VL_POINT, *PSTR_VL_POINT;

/* 电警交通灯结构体定义 */
typedef struct TAG_STR_SYS_TRAFFIC_LIGHT
{
	char cEnable;
	char cLightType;
	char cVehicleNum;
	char cResv[1];
}_PACKED_ STR_SYS_TRAFFIC_LIGHT, *PSTR_SYS_TRAFFIC_LIGHT;


typedef struct STR_SYS_ELEC_POLICE_VEHICLE_CONF_INFO
{
	unsigned int                uiCarRoadId;        /* 车道号，从1开始 */
	unsigned short		        usOSDPosX;          /* 抓拍图片上显示的X坐标 */
	unsigned short              usOSDPosY;          /* 抓拍图片上显示的Y坐标 */
	char		                szOSDContent[32];   /* 抓拍图片的OSD内容 */
	STR_SYS_AREA                strRoadArea;        /* 车道区域定义 */
	unsigned short              usRedLDetectType;   /* 红灯检测方式 */
	unsigned short              usRedLConnectMode;  /* 红灯检测器和设备连接方式 */
	STR_SYS_EP_RED_LIGHT_DESP   strRedLDesp;        /* 红灯检测描述 */
	unsigned short              usVehicleType;      /* 车检器类型 */
    unsigned short              usVehicleLinkMode;  /* 车检器连接方式 */
    STR_SYS_COIL_DESP           strCoilDesp;        /* 地感线圈描述 */
    unsigned int                uiForcedRedLight;	/* 强制红灯 */
    STR_VL_POINT				strVLPoint[4];		/* 虚拟线圈坐标位置定义 */
    STR_SYS_AREA				strTrafficLArea;	/* 虚拟线圈下每车道对应的交通灯像素坐标 */
    STR_SYS_TRAFFIC_LIGHT		strTrafficLight[MAX_LIGHT_TYPE];	/* 交通灯相关信息结构体 */
    char                        cLaneDir;			/* 车道方向定义 */
    char						cVLDispFlag;		/* 虚拟线圈是否显示标志 */
    char						cLaneLoopDispFlag;	/* 车道范围线圈是否显示标志 */
    char						cCarNoDir;			/* 禁行方向属性定义 */
	char						cRsv5[4];			/* 字节保留 */
	char						cCarDistance;		/* 最后一张图片机动车离停车线距离（单位米） */
  	char						cFrameInterval;		/* 抓拍间隔帧数（范围1~15） */
    
    unsigned short	    		usFlashLightType;    /* 闪光灯类型 */
	char        	    		cDetectType;         /* 检测线圈类型 */
	char						cCoilSwitchFlag;	 /* 线圈检测模式切换使能标志 */
	char						cCoilSwitchSta;		 /* 线圈检测模式切换状态 */
	char						cRedLightDectSta;	 /* 红灯检测模式状态查询 */
	char						cInLightSta;	     /* 补光方式状态 */
	char						cVLDecEn;			 /* 虚拟线圈是否检测使能 */
    char						cCarSnapDir;		 /* 是否判断车行方向 */
	char						cCarTrackEn;		 /* 轨迹分析功能使能位 */
	STR_VL_POINT				strNoLaneVLPoint[2]; /* 不按车道行驶虚拟线坐标位置定义 */
	unsigned char        	ucMinPlatePixelSize; 	 /* 最小车牌像素 */
	unsigned char        	ucMaxPlatePixelSize; 	 /* 最大车牌像素 */
	unsigned char        	ucCorrEveningThresh; 	 /* 黄昏门限值 */
	char 					cRsv;
    unsigned short        	usCoilDist;              /* 线圈之间的距离，厘米为单位 */
    unsigned char			ucPPDelaySnapFrames;	 /* 卡口图片延时抓拍帧数 */
    char 					cRsv1[1];
    char					szLaneId[8];			 /* 车道号id */
    
    char					cSwitchLaneEn;			 /* 是否为可变车道 */
    char					cSwitchLaneLeftNo;		 /* 可变车道左转方向灯号 */
    char					cSwitchLaneStraightNo;	 /* 可变车道直行方向灯号 */
    char					cRsv3;
}ELEC_POLICE_VEHICLE_CONF_INFO;

/* 虚拟线圈配置 */
typedef struct STR_SYS_VL_CONF_INFO
{
	unsigned int                uiCarRoadId;        /* 车道号,从1开始 */
	unsigned short              usThrDayToNight;    /* 白天转为夜间的参考门限 */
	unsigned short              usThrNightToDay;    /* 夜间转为白天的参考门限 */
	unsigned int                uiCorrNightThresh;  /* 夜间相关值门限 */
	unsigned int                uiCorrDayThresh;    /* 白天相关值门限 */
	unsigned int                uiCorrMotorThresh;  /* 摩托车相关值门限 */
	unsigned int                uiCorrEveningThresh;/* 黄昏门限值 */
	unsigned int                uiDayMotorWidth;    /* 白天摩托车宽度门限值 */
	unsigned int                uiNightMotorWidth;  /* 夜间摩托车宽度门限值 */
	unsigned char				ucCapDelayInterval; /* 采集延迟帧数 */

	char						cCapDelayNum;		/* 车牌检测间隔帧数 */
	char        		        cMinPlatePixelSize;	/* 最小车牌像素 */
	char        		        cMaxPlatePixelSize;	/* 最大车牌像素 */
}VL_CONF_INFO;

/* Sys default Conf and sta*/
typedef struct SysDefConfInfo
{
	int  iVehiSta;	/* 车检器上电工作状态，默认为正常 */
}SYS_DEF_CONF_INFO;

/* 电警违章类型名称及编号配置信息 */
typedef struct STR_EP_IRREG_CONF_INFO
{
	char 			cDispFlag;			/* 违章名称是否叠加到图片上 */
	char			cRsv[3];
	char			szRunRedCode[8];	/* 闯红灯违章代码 */
	char			szRunRedName[64];	/* 闯红灯违章名称 */
	
	char			szReverseRunCode[8];/* 逆行违章代码 */
	char			szReverseRunName[64];/* 逆行违章名称 */

	char			szInfringForbidLineCode[8]; /* 违反禁止标线行驶违章代码 */
	char			szInfringForbidLineName[64];/* 违反禁止标线行驶违章名称 */

	char			szInfringDirLaneRunCode[8]; /* 不按导向车道行驶违章代码 */
	char			szInfringDirLaneRunName[64];/* 不按导向车道行驶违章名称 */

	char			szStopOnTrafficNoEntryCode[8]; /* 路口遇堵禁止进入违章代码 */
	char			szStopOnTrafficNoEntryName[64];/* 路口遇堵禁止进入违章描述 */

	char			szHighwayInfringDirLaneCode[8]; /* 高速路不按导向车道行驶违章代码 */
	char			szHighwayInfringDirLaneName[64];/* 高速路不按导向车道行驶违章名称 */

	char			szStoppedVehicleCode[8]; 		/* 违章停车违章代码 */
	char			szStoppedVehicleName[64];		/* 违章停车违章名称 */

}EP_IRREG_CONF_INFO;

/* 电警新增违章类型名称及编号配置信息 */
typedef struct STR_EP_NEW_ADD_IRREG_CONF_INFO
{
	char			szRunInNoLaneCode[8]; 		/* 不在机动车道内行驶违章代码 */
	char			szRunInNoLaneName[64];		/* 不在机动车道内行驶违章名称 */

}EP_NEW_ADD_IRREG_CONF_INFO;

/* 电警车牌识别配置信息 */
typedef struct tagSTR_EP_LPR_CONF_INFO
{
	unsigned char ucLPREn;					/* 车牌识别使能标志 */
	unsigned char ucLPRCConfidence;			/* 车牌首字符置信度 */
	char		  cRsv[2];
	char    szLPRFirstChar[6];				/* 车牌首字符 */
	unsigned short usLPRWidth;				/* 车牌识别宽度 */
	unsigned short usLPRHeight;				/* 车牌识别高度 */
}EP_LPR_CONF_INFO;

/* FTP 配置信息 */
typedef struct STR_SYS_FTP_CONF_INFO
{
	unsigned int    uiEnable;           /* 是否使能FTP */
	unsigned int    uiFtpIP;            /* FTP地址 */
	unsigned short  usFtpPort;          /* 端口号 */
	char			cRsv[2];
    char 		    szUserName[128];     /* 用户名 */
    char 		    szUserPasswd[128];   /* 密码 */
    char			cTRecEnable;
    unsigned char   ucTRecLast;
    unsigned short  usTRecInterval;
}FTP_CONF_INFO;

/* NTP 配置信息 */
typedef struct STR_SYS_NTP_CONF_INFO
{
	unsigned int uiEnable;		/* 是否使能 */
	unsigned int uiServerIP;	/* NTP服务器 */
	unsigned short usJudgeCycle;/* 对时周期(单位:分钟) */
	unsigned char  ucJudegPrecision;/* 对时精度(单位:秒) */
	unsigned char ucRsv;
}NTP_CONF_INFO;

/* 摄像机时间段控制 */
typedef struct STR_SYS_CAMERA_TIME_RANGE
{
	char		cGroupId;
	char		cEnable;
	char		szSceneName[30];
	unsigned int iBeginTime;
	unsigned int iLastTime; /* 持续时间 */

	char		cIrisPos;	/* 自动光圈位置 */
	char		cRsv[3];
}CAMERA_TIME_RANGE_INFO;


/* 电警车道时间段控制 */
typedef struct STR_EP_TIME_RANGE
{
	char		cForbidType;
	char		cRange1Enable;
	char		cRange2Enable;
	char 		cRsv1;
	int			iBeginTime1;
	int			iLastTime1; 	/* 持续时间1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* 持续时间2 */	
}EP_TIME_RANGE_INFO;

/* 电警辅助红灯控制 */
typedef struct STR_EP_ASSIST_RED_LIGHT
{
	char		cRedLightNum;
	char		cEnableFlag;
	char 		cRsv[2];
	unsigned int iBeginTime1;
	unsigned int iLastTime1; 	/* 持续时间1 */
	unsigned int iBeginTime2;
	unsigned int iLastTime2; 	/* 持续时间2 */	
	unsigned int iBeginTime3;
	unsigned int iLastTime3; 	/* 持续时间3 */
}EP_ASSIST_RED_LIGHT_INFO;

/* 电警遇堵禁入配置 */
typedef struct STR_EP_STOP_ON_TRAFFIC_NO_ENTRY
{
	char		cEnableFlag;
	char 		cRsv[3];
	unsigned int iBeginTime1;
	unsigned int iLastTime1; 	/* 持续时间1 */
	unsigned int iBeginTime2;
	unsigned int iLastTime2; 	/* 持续时间2 */	
	unsigned int iBeginTime3;
	unsigned int iLastTime3; 	/* 持续时间3 */
}EP_STOP_ON_TRAFFIC_NO_ENTRY;

typedef struct STR_BAYONET_JPG_OSD_CONF
{ 
    unsigned char   ucDevSnOsdEn;		/* 设备编号 */
	unsigned char	ucPpNameOsdEn;		/* 卡口名 */
	unsigned char	ucDirNameOsdEn;		/* 方向 */
	unsigned char	ucLaneNameOsdEn;	/* 车道名 */
	
	unsigned char	ucSnapTimeOsdEn;	/* 抓拍时间 */
	unsigned char	ucCarDirOsdEn;		/* 车行方向 */
	unsigned char	ucBusNumOsdEn;		/* 车牌号码 */
	unsigned char	ucLPColorOsdEn;		/* 车牌颜色 */
	
	unsigned char	ucCarTypeOsdEn;		/* 车型 */
	unsigned char	ucCarColorOsdEn;	/* 车身颜色 */
	unsigned char	ucCurSpeedOsdEn;	/* 车速 */
	unsigned char	ucPunishSpeedOsdEn;	/* 处罚速度 */
	
	unsigned char	ucMarkSpeedOsdEn;	/* 标示速度 */
	unsigned char	ucSpeedRatioOsdEn;	/* 超速比例 */
	unsigned char	ucReguCodeOsdEn;	/* 违章代码 */
	unsigned char	ucStandReguDesOsdEn;/* 标准违章描述 */
	
	unsigned char	ucIntelReguDesOsdEn;/* 内部违章描述 */
    char            cRsv[3];
} BAYONET_JPG_OSD_CONF;

typedef struct STR_EP_JPG_OSD_CONF
{
    unsigned char   ucDevSnOsdEn;		/* 设备编号 */
	unsigned char	ucRoadNameOsdEn;	/* 路口名 */
	unsigned char	ucDirNameOsdEn;		/* 方向 */
	unsigned char	ucLaneNameOsdEn;	/* 车道名 */
	
	unsigned char	ucSnapTimeOsdEn;	/* 抓拍时间 */
	unsigned char	ucBusNumOsdEn;		/* 车牌号码 */
	unsigned char	ucLPColorOsdEn;		/* 车牌颜色 */
	unsigned char	ucCarTypeOsdEn;		/* 车型 */
	
	unsigned char	ucCarColorOsdEn;	/* 车身颜色 */
	unsigned char	ucReguCodeOsdEn;	/* 违章代码 */
	unsigned char	ucStandReguDesOsdEn;/* 标准违章描述 */
	unsigned char	ucIntelReguDesOsdEn;/* 内部违章描述 */
	
	unsigned char	ucRDBeginTimeOsdEn; /* 红灯开始时间 */
	unsigned char	ucRDCycleOsdEn;		/* 红灯周期 */
	unsigned char	ucRoadDespOsdEn;	/* 路口描述 */
	char            cRsv[1];


}EP_JPG_OSD_CONF;

/* 摄像机控制信息 */
typedef struct STR_SYS_CAMERA_CTRL_CONF_INFO
{
	unsigned int 	uiCamaraType;   /* 摄像机控制项 */
	float			fMaxVal;        /* 最大值 */
	float			fMinVal;        /* 最小值 */
	float			fFixVal;	    /* 固定值 */
	char			cCtlType;       /* 摄像机控制方式 */
	char 			cCamGroup;		/* 摄像机参数组号 */
	char 			cRsv2[1];
	char 			cRsv3[1];
}CAMERA_CTRL_CONF_INFO;

/* 网络硬盘信息 */
typedef struct STR_SYS_NET_HDD_CONF_INFO
{
	unsigned int  uiEnable;             /* 是否使用网络存储 */
	unsigned int  uiStorageType;        /* 存储类型 */
	unsigned int  uiIpAddr;             /* IP */
	char		  szFolderName[32];     /* 目录 */
    char	        szUserName[32];
    char	        szPassWord[32];
    unsigned int  uiAvaiable;
    unsigned int  uiUDiskSum;
}NET_HDD_CONF_INFO;

/* NRM设备配置信息 */
typedef struct STR_SYS_NET_REC_MANAGE_INFO
{
	char			cEnable;
	char			cRsv[3];
    unsigned int	uiMainNRMIp;
    unsigned int	uiBakNRMIp;
    unsigned short	usMainNRMPort;
    unsigned short	usBakNRMPort;
	char			szPuID[32];
	unsigned int	uiLastNRUIp[MAX_CHANNEL_NUM];
    unsigned short	usLastNRUHttpPort[MAX_CHANNEL_NUM];
}NET_REC_MANAGE_INFO;

/* 采集设备信息 */
typedef struct STR_SYS_DCTL_CONF_INFO
{
	char   			cEnable;           /* 是否使能 */
	char   			cComId;             /* 串口号 */
	char			cRecv[2];
	unsigned int   	uiVehicleType;      /* 检测类型 */
    char            cDatabits;
    char            cStopbits;
    char            cParity;
	char			cRsv;
    unsigned int    uiBaudrate;
    unsigned short  usMaxMeasureSpeed;
	char 			cRecv2[2];
}DCTL_CONF_INFO;

/*平台设备信息*/
typedef struct STR_SYS_PLATFORM_CONF_INFO
{
    char            cEnable;
    char			cRsv[3];
    char            sz330DriveWayId[4];
    char            sz330DevId[128];
    char            sz330DirId[16];
    char            sz330Key[256];
}PLATFORM_330_CONF_INFO;

/* 网管接入配置信息 */
typedef struct STR_SYS_NM_CONF_INFO
{
    unsigned int    uiNMSerIp;
    unsigned short  usNMPort;
    char	        cEnable;
	char 			cDefaultGWEn;	/* 默认网关使能标志 */
    unsigned int    uiDefaultGWIp;	/* 默认网关ip地址 */
    unsigned int 	uiDefaultHttpdPort;/* 默认httpd服务器监听端口号 */
}SYS_NM_CONF_INFO;


/* 功能是否使能标志 */
typedef struct STR_SYS_FUNC_ENABLE_INFO
{
	char		 cFlowRateFlag;
	char		 cAlarmRecFlag;
	char		 cSpeedAmendFlag;
	char		 cFlowRateInter;		/* 流量统计上报时间间隔 */
	
	char		 cXmlFileSendFlag;		/* xml文件是否发送使能 */
	unsigned char ucDspFramDifEn;		/* dsp帧差检测算法是否使能 */
	unsigned char ucDspStopDistan;		/* dsp检测范围实际距离，单位为米 */
	unsigned char ucDspOneLampEn;		/* dsp是否检测单车灯 */

	unsigned short usFlashStartLine;	/* kodar设备闪光延时行数 */
	char		  cSpeedAmendX;			/* 速度校正分母值 */
	char		  cSpeedAmendY;			/* 速度校正减去值 */
	
	unsigned char ucRedLightDelay;		/* 红灯延时生效秒数 */
	unsigned char ucRedTurnGreenSnapEn;	/* 红转绿或绿转红需不需要抓拍 */
	unsigned char ucNoDirReportSnapEn;	/* 算法未报方向时需不需要抓拍 */
	unsigned char ucEpPpPicMode;		/* 卡口图片合成模式(0:全景+特写;1:全图)  */
	
	unsigned short usFreqFlashDelayUSec; /* 频闪灯延时时长配置 */
	unsigned short usppFLightDelayUSec;	/* 卡口闪光灯延时时长配置 */
	unsigned short usepFLightDelayUSec;	/* 电警闪光灯延时时长配置 */
	unsigned char ucDebugLogSaveFlag;	/* 调试信息是否保存为文件标志配置 */
	unsigned char ucArithDebugShowFlag;	/* 算法跟踪调试信息是否显示标志 */
	
	unsigned char ucVDLGreenMode;		/* 交通灯视频检测检绿灯标志 */
	unsigned char ucPressLineSanpFlag;	/* 压线是否抓拍标志 */
	unsigned char ucBackWordsSanpFlag;	/* 逆行是否抓拍标志 */	
	unsigned char ucEpPpSnap0Or1;		/* 电警卡口使用第0步还是第1步 */

	unsigned char  ucUdiskWeldSta;		/* 配置该硬件板子有没有焊接U盘 */
	unsigned char ucEpCarSpeedOsdEn;	/* 电警车速值OSD是否叠加到图片上 */
	unsigned short usFreqFlashStartLine;/* kodar设备频闪灯延时行数 */

	unsigned char  ucUdiskEn;			/* U盘是否使用 */
	unsigned char  ucUFdiskEn;			/* 是否进行强制格式化U盘 */
	unsigned short usNetVehiclePort;	/* 网口交通灯检测器端口号 */
	
	unsigned char	ucEpPpSnapPosition;	/* 电警地感线圈卡口抓拍位置，0进前线圈1，1进线圈2，2表示出线圈1,3表示出线圈2 */
	unsigned char	ucPicQuality;		/* jpeg质量因子，越大质量越好 */
	unsigned short  usRedLReviseShutter;/* 红灯修正快门阀值，当前快门值<=配置值时表示晚上要修
											>配置值时晚上不修 */

	char		   cGammaVal;			/* gamma值 */
	char		   cFlashLineMode;		/* 动态快门调整模式 */
	char	 	   cDyncJudFlashLines;	/* 动态快门行数调整 */
	unsigned char  ucPlateSegFlag;		/* 是否开启车牌分割功能 */

	unsigned short usCsSpeedAmendNumerator;	/* 雷达速度校正分子(以1000为分母) */
	char		   cDubalFEn;				/* 双快门模式是否使能标志 */
	unsigned char  ucRedLReviseDegree;		/* 晚上红灯修正程度 */

	unsigned short  usSaturation;			/* 饱和度参数配置 */
	unsigned short  usGPixsThresh;			/* G分量像素个数阀值 */

	float			fDubalFShut;			/* 双快门模式快门值 */
	float			fDubalFGain;			/* 双快门模式增益值 */

	char			cRedDecSaturation;		/* 红灯检测饱和度 */
	char			cRedDecBright;			/* 红灯检测亮度 */
	unsigned char 	ucChangeRoad;			//是否支持变道抓拍
	unsigned char   ucPicAssistDebug;		//是否使能图片调试信息上报
	
	unsigned char	ucCrossLineParkEn;		//是否使能越线停车违章抓拍
	char 			cLaneChgType[3];

	unsigned char   ucSDEn;					/* SD卡是否使用 */
	unsigned char   ucSDFdiskEn;			/* 是否进行强制格式化SD卡 */
	char			cRsv[1];
	char			aucPressLine[5];
	char			cOnvifEn;				//onvif程序是否启动标志
	char			cSipSdkEn;				//sipsdk程序是否启动标志
	char			cV2En;					//v2程序是否启动标志
	char			cV3En;					//v3程序是否启动标志

	char			cHaloTakeOutEn;			//红灯光晕修正使能
	char			cHaloAreaRange;			//红灯光晕外扩范围
	char			cGB28181En;				//GB28181程序是否启动标志
	char			cRestartTimeEn;			//定时重启使能

	unsigned int	uiRestartTime;			//定时重启时间
}SYS_FUNC_ENABLE_INFO;

/* 卡口相关功能配置 */
typedef struct STR_PP_FUNC_ENABLE_INFO
{   
	unsigned char   ucOsdPlateScopeEn;	//是否在图片上叠加车牌识别区域
	unsigned char	ucisShieldReverse;	//是否屏蔽逆行上报图片
    unsigned short  usPlateScopeIncSize;//(相对于算法报的)车牌范围所增加的值
    
    unsigned char   ucRadarSpeedGap;	//雷达间断值(连续间断多少帧的速度后，再来速度时认定是另一辆车)
    unsigned char   ucRadarForwardLim;	//雷达速度向前范围(速度提前报可同步到车辆值)
    unsigned char   ucRadarBackwardLim;	//雷达速度向后范围(速度延迟报可同步到车辆值)
	unsigned char   ucIsDecTaillight;	//是否使能尾灯检测
	
	unsigned char	ucFaceIdenEn;		//人脸识别使能
	unsigned char	ucOlviaRadaParalEn;	//奥利维亚雷达并行模式使能，默认为1
	unsigned char 	ucFaceOvenlayEn;	//人脸检测是否叠加使能
	unsigned char   usIsReverseSnapNorm;	//0-正常，1-逆行不按逆行抓拍而按正常行驶抓拍，只抓拍一张上报

	//unsigned char   ucRsv[1];
}PP_FUNC_ENABLE_INFO;


/* 电警虚拟线圈步数间隔配置信息 */
typedef struct STR_EP_V_LOOP_STEP_INFO
{
	unsigned int uiLane0Step01;
	unsigned int uiLane0Step12;
	unsigned int uiLane1Step01;
	unsigned int uiLane1Step12;
	unsigned int uiLane2Step01;
	unsigned int uiLane2Step12;
	unsigned int uiLane3Step01;
	unsigned int uiLane3Step12;
	char	 cRsv[16];
}EP_V_LOOP_STEP__INFO;


/* 330平台接入配置信息 */
typedef struct STR_SYS_V3_CONF_INFO
{
    unsigned int    uiV3SerIp;
    unsigned short  usV3Port;
	char 			cRecv[2];
    char            szDevNo[20];
    char            szDevPwd[16];
    char            cV3ServerSeq;
    char            cV3TimeSynFlag;
    char            cV3Enable;
	char			cVersion;
}PLATFORM_V3_CONF_INFO;


typedef struct tagVidiconMenuInfo
{
	int nAperture;
	int nShutter;
	int nGain;
	int nColor;
	int nWhiteBalance;
	int nBackCompensate;
	int nMirror;
	int nBCLeft;
	int nBCRight;
	int nBCTop;
	int nBCBottom;
	short wb_WBRL ;
	short wb_WBRS;
	short wb_WBGL;
	short wb_WBGS;
	short wb_WBBL;
	short wb_WBBS;
}VIDICON_MENU_CONF_INFO;

/* GB28181平台接入配置信息 */
typedef struct STR_SYS_GB28181_CONF_INFO
{
	char			szServerId[32];			/* 服务器id */
	char			szServerDomain[32];		/* 服务器域 */
	char			szServerIp[32];			/* 服务器ip */
    unsigned int    uiServerPort;			/* 服务器port */
    unsigned int	uiRegisterExpiry;		/* 注册有效时长 */
    unsigned int    uiKeepLiveTime;			/* 心跳周期 */
    unsigned int    uiConMode;				/* 连接方式: 0:udp 1:tcp */
    char            szClientId[32];
    char            szClientUserName[32];
    char            szClientPasswd[32];
    unsigned int    uiPackMode;				/* rtc 打包方式: 0:PS 1:H264 */

    char			cEnFlag;				/* 启用标志 */
    char			cRsv[3];
}GB28181_CONF_INFO;


#ifdef __cplusplus
extern "C" {
#endif

int SysConfInit(void);

int SysConfUninit(void);

int SysConfDebugSwitch(int nLevel);


/*  arg1:config_type, arg2:config struct's pointer ,arg3:the index of the config */
/*  e_NET_CONF,	                    NET_CONF_INFO,						0 */
/*  e_RTSP_CONF, 		            RTSP_CONF_INFO,						0 */
/*  e_VIDEO_CONF, 		            VIDEO_CONF_INFO,					0 */
/*  e_REC_CONF, 		            REC_CONF_INFO,						0 */
/*  e_PASSPORT_OVERALL_CONF,        PASSPORT_OVERALL_CONF_INFO,         0 */
/*  e_PASSPORT_VEHICLE_CONF,		PORT_VEHICLE_CONF_INFO,				0--2 */
/*  e_ELEC_POLICE_OVERALL_CONF,		ELEC_POLICE_OVERALL_CONF_INFO,      0 */
/* 	e_ELEC_POLICE_VEHICLE_CONF,		ELEC_POLICE_VEHICLE_CONF,			0--2 */
/*  e_VL_CONF,		                VL_CONF_INFO,						0 */
/*  e_PASSPORT_FTP_CONF	,		    FTP_CONF_INFO,		     		    0 */
/*  e_ELEC_POLICE_FTP_CONF,         FTP_CONF_INFO,			            0 */
/*  e_NTP_CONF,                     NTP_CONF_INFO,						0 */
/*  e_CAMERA_CTRL_CONF,             CAMERA_CTRL_CONF_INFO,              0 */
/*  e_PIC_NET_HDD_CONF,		        PIC_NET_HDD_CONF_INFO,   		    0 */
/*  e_REC_NET_HDD_CONF,             REC_NET_HDD_CONF_INFO,              0 */
/*  e_DCTL_CONF,		            DCTL_CONF_INFO, 					0 */
/*  e_SYS_DEF_CONF,		            SYS_DEF_CONF_INFO,        		    0 */
/* 	e_ELEC_POLICE_VEHICLE_CONF,		ELEC_POLICE_VEHICLE_CONF_INFO,		0--2 */
/*  e_VL_CONF,		                VL_CONF_INFO,						0 */
/*  e_PASSPORT_FTP_CONF	,		    PASSPORT_FTP_CONF_INFO,				0 */
/*  e_ELEC_POLICE_FTP_CONF,         ELEC_POLICE_FTP_CONF_INFO,			0 */
/*  e_NTP_CONF,                     NTP_CONF_INFO,  					0 */
/*  e_CAMERA_CTRL_CONF,             CAMERA_CTRL_CONF_INFO,              0 */
/*  e_PIC_NET_HDD_CONF,		        PIC_NET_HDD_CONF_INFO, 				0 */
/*  e_REC_NET_HDD_CONF,             REC_NET_HDD_CONF_INFO,              0 */
/*  e_DCTL_CONF,		            DCTL_CONF_INFO,					    0 */

/* 所有参数的合法性由调用者来保证  */


int SysConfGet(CONF_TYPE nConfType,void *pConf,int index);
int SysConfSet(CONF_TYPE nConfType,void *pConf,int index);
const char * GetProductSerialNo();

int SaveWriteFile(void);
int  SaveCamCtrlConf(void);


//add by wangb for 车检器报警上报模块

struct tagGroup
{
	int nBoardId;
	int nGroupId;
};

typedef struct tagVehicleConfInfo
{
	int nChannelId;
	int nVDType;
	int nGroupNum;
	struct tagGroup group[3];	
}VEHICLE_DECT_CONF;

typedef struct tagTypeConf
{
	unsigned int nChannelId;
	unsigned int bEnable;
	unsigned int nType;
}VDETECT_TYPE_INFO;

#ifdef __cplusplus
}
#endif

#endif

