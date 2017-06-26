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
	e_NET_CONF = 0x0,                   /* ����������Ϣ */
	e_RTSP_CONF,                        /* RTSP������Ϣ */
	e_VIDEO_CONF,                       /* ��Ƶ������Ϣ */
	e_REC_CONF,                         /* ¼��������Ϣ */
	e_PASSPORT_OVERALL_CONF,            /* ����ȫ��������Ϣ */
	e_PASSPORT_VEHICLE_CONF,            /* ���ڳ���������Ϣ */
	e_ELEC_POLICE_OVERALL_CONF,         /* ����ȫ�־������� */
	e_ELEC_POLICE_VEHICLE_CONF,         /* ���Ӿ��쳵��������Ϣ */
	e_VL_CONF,                          /* ������Ȧ�߼�������Ϣ */
	e_PASSPORT_FTP_CONF,                /* ����FTP ������Ϣ */
	e_ELEC_POLICE_FTP_CONF,             /* ���Ӿ���ftp ������Ϣ */
	e_NTP_CONF,                         /* NTP ������Ϣ */
	e_CAMERA_CTRL_CONF,                 /* ���������������Ϣ */
	e_PIC_NET_HDD_CONF,                 /* ͼƬ����Ӳ��������Ϣ */
	e_REC_NET_HDD_CONF,                 /* ¼������Ӳ��������Ϣ */
	e_DCTL_CONF,                        /* ����������Ϣ */
	e_PLATFORM_CONF,                    /* ƽ̨������Ϣ */
	e_V3_CONF,                          /* V3ƽ̨������Ϣ */
	e_SYS_DEF_CONF,                     /* ϵͳĬ��������Ϣ */
	e_APPMODE_CONF,                     /* APP Mode ������Ϣ */
    e_NM_CONF,                          /* ����������Ϣ */
    e_CAMERA_TIME_RANGE,                /* �����ʱ��ο��� */
    e_SYS_FUNC_ENABLE_CONF,				/* ϵͳ�����Ƿ�ʹ�ܱ�־ */
    e_EP_RANGE_TIME_CONF,				/* �羯������ֹ��ʻʱ������� */
    e_EP_V_LOOP_STEP_INVER,				/* �羯������Ȧץ�ļ������ */
    e_EP_330_CONF,						/* �羯330���� */
    e_V2_CONF,                          /* V2ƽ̨������Ϣ */
    e_EP_ASSIST_REDLIGHT_CONF,			/* �羯������Ʋ������� */	
    e_EP_IRREG_INFO_CONF,				/* �羯Υ��������Ϣ���� */
    e_EP_LPR_INFO_CONF,					/* �羯����ʶ����Ϣ���� */
    e_PP_FUNC_ENABLE_CONF,				/* ������ع���������Ϣ */
    e_EP_SOT_NOENTRY_CONF,				/* �羯���½�����Ϣ���� */
    e_PP_JPG_OSD_CONF,					/* ����ͼƬosd��Ϣ�������� */
    e_EP_JPG_OSD_CONF,					/* �羯ͼƬosd��Ϣ�������� */
    e_PP_IRREG_INFO_CONF,				/* ����Υ��������Ϣ���� */
    e_PASSPORT_BACKUPS_FTP_CONF,        /* ���ݿ���FTP ������Ϣ */
    e_NET_REC_MANAGE_CONF,				/* �ƴ洢NRM�豸��Ϣ���� */
    e_EP_BACKUPS_FTP_CONF,        		/* ���ݵ羯FTP ������Ϣ */
    e_EP_NEWADD_IRREG_INFO_CONF,		/* �羯����Υ��������Ϣ���� */
    e_GB28181_CONF,                     /* Gb28181ƽ̨������Ϣ */
}CONF_TYPE;


/* Net Conf */
typedef struct NetConfInfo
{
	unsigned int    uiIPAddr;			//IP��ַ
	unsigned int	uiNetMask;			//��������
	unsigned int	uiGateWay;			//����
} NET_CONF_INFO;

/* Rtsp Conf */
#if 0
typedef struct RtspConfInfo
{
	char            cSupportDownLoad ;          /* �豸�Ƿ�֧��¼������ */
    char            cDownLoadType ;             /* ¼����������         */
	char			cRsv[2];
    unsigned int    uiProxyDownLoadIp;          /* ¼������IP��ַ       */
    unsigned short  usProxyDownLoadPort;        /* ¼�����ض˿�         */
    char            cSupportReplay;             /* �豸�Ƿ�֧�ֻط�     */
    char            cReplayType;                /* �ط�����             */
    unsigned int    uiProxyReplayIp;            /* �ط�IP               */
    unsigned short  usProxyReplayPort;          /* �طŶ˿�             */
    char			cRsv1[2];
} RTSP_CONF_INFO;
#else
typedef struct tagRtspConfInfo
{
	int nRtspMode;					//0=NAT��ʽ, 1=����RTSP,2=����RTSP 
	unsigned int dwRtspIpAddr;			//RTSP��������ַ
	unsigned int dwRtspPort;			//RTSP�������˿�
	unsigned int dwRtspDownLoadPort;    //RTSP���������ض˿�
	char	szRtspPath[32];				//RTSP·��
	char	szRtspUsername[20];			//RTSP �û���
	char	szRtspPassword[20];			//RTSP ����
} RTSP_CONF_INFO;
#endif
/* Video Conf */
/* 2.7 ��Ƶ������ѯ */
typedef struct StrVChannelVParamQuery
{
    unsigned short  usChannel;          /* ��Ƶͨ���� ��1��ʼ */
    char            cVQuality;          /* ��Ƶ���� 1~5 */
    char            cFrameRate;
    unsigned int    uiBitRate;
    char            cKeyFInterval;      /* �ؼ�֡��� */
    char            cImageResol;        /* ͼ��ֱ��� */
    char            cEncodeType;
    char            cVBR;               /* �Ƿ������ */
    unsigned short  usXPos;             /* ��Ļ����Ļ�������xֵ */
    unsigned short  usYPos;             /* ��Ļ����Ļ�������yֵ */
    char            szContent[64];      /* ��Ļ���� */

    unsigned short  usDebugFrameX;      /* ���Կ�ѡλ��X */
    unsigned short  usDebugFrameY;      /* ���Կ�ѡλ��Y */
    
    unsigned char   ucDebugFrameEn;		/* ���Կ�ѡʹ�ܱ�־ */
    unsigned char   ucTimeDisMode;		/* ʱ����ʾ��ʽ 0:�޺��� 1:�к��� */
	unsigned short  usTimeXPos;			/* ʱ����ʾ���� X */

	unsigned short  usTimeYPos;			/* ʱ����ʾ���� Y */
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
	unsigned char	nChannelID;         /*  ͨ���ţ���1��ʼ      */
	char			cRsv[3];
	unsigned int	nSysRecMode;        /*  0 ��ֹ¼��           */
										/*  ȫ��¼��             */
										/*  ����¼��             */
										/*  �ƻ�¼��             */
	unsigned int	nLoopRecFlag;       /*  1 ��������ͣ         */
	                                    /*  2 ѭ������           */
	                                    /*  3 ����ָ������¼��   */
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
	char    szPassPortName[32];             /* ������ */
	char    szDirection[32];                /* ������Ϣ */
	char    szPassPortId[8];				/* ����ID */
	char	cOSDColor;				 		/* ͼƬ����OSD��ɫ */
	char    cValidLanes;			 		/* ʵ��ʹ�ó����� */
	char    cVLWorkMode;			 		/* ������Ȧ����ģʽ */

	unsigned char ucLPREn;					/* ����ʶ��ʹ�ܱ�־ */
	char    szLPRFirstChar[32];				/* �������ַ� */
	unsigned char ucLPRCConfidence;			/* �������ַ����Ŷ� */
	unsigned char ucVSpeedMeaFlag;			/* ��Ƶ����ʹ�ܱ�־ */
	unsigned short usVSpeedMeaLen;			/* ��Ƶ����ʵ�ʾ���(��λ:����) */

    unsigned char    aucFreqFlashEn[4];      /* �Ƿ�ʹ��Ƶ������ */
    unsigned char    aucFlashTrigMode[4];    /* ����ƴ�����ʽ */
    unsigned char    aucStrobeFlashFre[4];   /* Ƶ���Ʊ�Ƶϵ�� */
    char            szPpDevCode[MAX_DEV_CODE_LEN];/* �����豸��� */
    char     szDirId[8];                        /* ����ID */
    
    unsigned char   ucFillInLSwitchEn;        /* �����л�ʹ�ܱ�־ */
    unsigned char   ucGrayThresholdVal;        /* �л��Ҷȷ�ֵ */
    unsigned char    ucCheckSwitchCycle;        /* �����Ƶ�Ҷ�ֵ���� */
    char            Rsv1;
    
    char            szExPpDevCode[EXT_DEV_CODE_LEN];/* ��չ�豸����ַ��� */

	unsigned char	ucSmartSwitchEn;		/* ���������л�ʹ�� */
	unsigned char	ucTimerSwitchEn;		/* ���ⶨʱ�л�ʹ�� */
	char			cRsv2[2];
	unsigned int	uiFillInLightBeginTime;	/* ������ʼʱ�� */
	unsigned int	uiFillInLightLastTime;	/* ����ʱ�� */    
}PASSPORT_OVERALL_CONF_INFO;

/* PassPort Vehicle Conf */
/* ���ڳ�����Ϣ���� */



/* nCoilDesp COM ����ʱ�Ľṹ���� */
typedef struct tagSTR_SYS_COIL_COM_DESP
{
    char    cComId;
    char    cBoardId;                    /*�忨��*/
    char    cFrontCoilId;               /* ������ǰ��Ȧ */
    char    cBehindCoilId;              /* ����������Ȧ */
    char    cGroupId;
	char 	cRsv[3];
}_PACKED_ STR_SYS_COIL_COM_DESP;

/* nCoilDesp GPIO ����ʱ�Ľṹ���� */
typedef struct STRCOILGPIODESP
{   
    char    cBoardId;                /*�忨��*/
    char    cFrontCoilId;           /* ������ǰ��Ȧ */
    char    cBehindCoilId;          /* ����������Ȧ */
	char 	cRsv[1];
}_PACKED_ STR_SYS_COIL_GPIO_DESP, *PSTR_SYS_COIL_GPIO_DESP;

typedef union STR_SYS_COIL_DESP
{
    STR_SYS_COIL_COM_DESP   strComCoilDesp;
    STR_SYS_COIL_GPIO_DESP  strGpioCoilDesp;
}_PACKED_ STR_SYS_COIL_DESP, *PSTR_SYS_COIL_DESP;


/* ����Э��������ڷ�Χ�Ķ��壬���ô˽ṹ */
typedef struct tagStrArea
{
    unsigned short usAreaPosX;      /* �������Ͻ�����X */
    unsigned short usAreaPosY;      /* �������Ͻ�����Y */
    unsigned short usAreaWidth;     /* ������ */
    unsigned short usAreaHeight;    /* ����߶� */
}_PACKED_ STR_SYS_AREA, *PSTR_SYS_AREA;

typedef struct tagStrTimeSlotSnapObj
{
    unsigned int uiBeginTimeSecs;	/* ��ʼʱ�� */
    unsigned int uiLastTimeSecs;	/* ����ʱ�� */
    unsigned char ucSnapObj;		/* ץ��Ŀ�� */
    unsigned char ucRsv[3];
}_PACKED_ STR_SYS_TIME_SLOT_SNAP_OBJ, *PSTR_SYS_TIME_SLOT_SNAP_OBJ;


typedef struct STR_SYS_PORT_VEHICLE_CONF_INFO
{
	unsigned int        uiCarRoadId;            /* ������ */
	unsigned short      usOSDPosX;              /* OSD��ʾx���� */
	unsigned short	    usOSDPosY;              /* OSD��ʾy���� */
	char 			    szOSDContent[32];       /* OSD���� */
	STR_SYS_AREA        strRoadArea;
	unsigned short		usFlashLightType;       /* �����/��������� */
	unsigned short		usFlashLightCtlType;    /* ����ƿ��Ʒ�ʽ */
	char    		    cComId;                 /* ����ƿ��ƴ��ں� */
	char    		    cFlashDevType;          /* ������豸���� */
	char				cFlashCtlMode;			/* ����ƿ���ģʽ */
	char				cSingalCoilPos;			/* ����Ȧλ�ö��� */
	unsigned short		ausFlashAddr[4];        /* �����485��ַ */
	unsigned short		usDetectType;           /* ��Ȧ���� */
    unsigned short      usIrreSpeed;            /* Υ���ٶ����� */    
	STR_SYS_AREA        strVLArea;
	unsigned short		usVehicleType;          /* ���������� */
	unsigned short   	usVehicleLinkMode;      /* ���������ӷ�ʽ */
	STR_SYS_COIL_DESP   strCoilDesp;            /* �ظ���Ȧ���� */
	unsigned short		usMaxSpeed;             /* ����������� */
	unsigned short		usMinSpeed;             /* ����������� */
    unsigned int        uiCoilDist;             /* ��Ȧ֮��ľ��룬����Ϊ��λ */
    char				cVLDispFlag;			/* ������Ȧ�Ƿ���ʾ��־ */
    char				cLaneDispFlag;			/* ������Χ��Ȧ�Ƿ���ʾ��־ */
	char				cGroudCoilDecType;		/* �ظ���Ȧ�������:˫��Ȧ����Ȧ */
	char				cSingalCoilSnapType;	/* ����Ȧץ������:����Ȧ�����Ȧץ�� */
    char				ucRsv4[4];				/* ���� */

    char				cCoilSwitchFlag;		/* ��Ȧ�л�ʹ�ܱ�־ */
   	char				cInLightSta;	     	/* ���ⷽʽ״̬ */
	char				cCoilSwitchSta;		 	/* ��Ȧ���ģʽ�л�״̬ */
	char				cVLSnapDir;				/* ������Ȧץ�ķ��� */

	char 				cVLDecEn;				/* ������Ȧ���ʹ�� */
	unsigned short  	usSingalCoilHigh;		/* ����Ȧ�µ�ǰ��Ȧ�߶� */
    unsigned char 		ucDaySnapObj;			/* ����ץ��Ŀ������ */
    
	unsigned char 		ucNightSnapObj;			/* ����ץ��Ŀ������ */
	unsigned char       ucSnapPicNum;			/* ץ��ͼƬ�������� */
	unsigned short		usSnapPicInter;			/* ����ͼƬץ�ļ��ʱ��(��λ:����) */

    STR_SYS_TIME_SLOT_SNAP_OBJ strTimeSlotSnapObj;/* ʱ���ץ��Ŀ������ */
    unsigned char		ucInLightEn;      		/* �Ƿ�ʹ�ܸ������� */
    char				cRsv5[3];
    char				szLaneId[8];			/* ����id */
    
    char				cSnapDir;				/* ץ�ķ���(0:����;1:����) */
	char				cSDEnFlag;				/* �Ƿ�ʹ�� */
	char				cTruckEnFlag;			/* �����ץ��ʹ�� */
	char 				cRsv[1];
	unsigned int 		iSDBeginTime1;
	unsigned int 		iSDLastTime1; 			/* ����ʱ��1 */
	unsigned int 		iSDBeginTime2;
	unsigned int 		iSDLastTime2; 			/* ����ʱ��2 */	
	unsigned int 		iSDBeginTime3;
	unsigned int 		iSDLastTime3; 			/* ����ʱ��3 */
}PORT_VEHICLE_CONF_INFO;


typedef struct STR_SYS_RED_LIGHT_AREA
{
    STR_SYS_AREA    strArea;           /* ��������� */
} _PACKED_ STR_SYS_RED_LIGHT_AREA, *PSTR_SYS_RED_LIGHT_AREA;


typedef struct STR_SYS_RED_LIGHT_MODE_SET
{
	unsigned char ucLightStrobeEn;		/* Ƶ�������Ƿ�ʹ�� */
	unsigned char ucLightStrobeFrame;	/* Ƶ������֡�� */
	unsigned char ucSingalColLight;		/* �Ƿ��ǵ�ɫ��(0:��ɫ��;1:��ɫ��) */
	unsigned char ucMapRDNo;			/* ӳ�䵽��Ƽ�����ĺ�ƺ� */
} _PACKED_ STR_SYS_RED_LIGHT_MODE_SET, *PSTR_SYS_RED_LIGHT_MODE_SET;

/* ����Υ���������Ƽ����������Ϣ */
typedef struct STR_PP_IRREG_CONF_INFO
{
	char	szOverSpeedSmallPercent50Code[8];	/* ����С��50%��Υ�´��� */
	char	szOverSpeedSmallPercent50Name[64];	/* ����С��50%��Υ������ */

	char	szOverSpeedLargePercent50Code[8];	/* ���ٴ���50%��Υ�´��� */
	char	szOverSpeedLargePercent50Name[64];	/* ���ٴ���50%��Υ������ */
	
	char	szReverseRunCode[8];				/* ����Υ�´��� */
	char	szReverseRunName[64];				/* ����Υ������ */

	char	szInfringeProhibCode[8]; 			/* ������Υ�������־ָʾ��Υ�´��� */
	char	szInfringeProhibName[64];			/* ������Υ�������־ָʾ��Υ������ */

}PP_IRREG_CONF_INFO;

/* ���Ӿ���ȫ����Ϣ���� */

/* ����DSPֻ֧��6�������Դ�16��Ϊ��6 */
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
  char 		                szCarRoadName[32];          /* ��·�� */
  char  	                szDirection[32];            /* ���� */
  unsigned short  	        usPicStyle;                 /* ͼƬ�ϳɷ�ʽ */
  unsigned short  	        usFlashLightType;           /* �����/��������� */
  unsigned short  	        usFlashLightCtlType;        /* ����ƿ��Ʒ�ʽ */
  char  	                cComId;                     /* ���ƴ��ں� */
  char  	                cFlashDevType;              /* ������豸���� */
  unsigned short  	        ausFlashAddr[4];            /* �����485��ַ */
  unsigned char 			aucFreqFlashEn[4];			/* Ƶ�����Ƿ�ʹ�ܱ�־ */
  unsigned char				aucFlashTrigMode[4];    	/* ����ƴ�����ʽ */
  unsigned char				aucStrobeFlashFre[4];		/* Ƶ���Ʊ�Ƶϵ�� */  
  char						szEpDevCode[MAX_DEV_CODE_LEN];						/* �羯�豸��� */
  STR_SYS_RED_LIGHT_AREA   	strRedArea[MAX_AREA_GROUP_NUM][MAX_AREA_NUM];       /* ������� */
  STR_LIGHT                 strRedLow;                   /*�����ֵ����*/
  STR_LIGHT                 strRedHigh;                  /*�����ֵ����*/
  STR_LIGHT                 strGreenLow;                 /*�̵���ֵ����*/
  STR_LIGHT                 strGreenHigh;                /*�̵���ֵ����*/
  STR_LIGHT                 strYellowLow;                /*�Ƶ���ֵ����*/
  STR_LIGHT                 strYellowHigh;               /*�Ƶ���ֵ����*/
  char  	                szRemarks[32];               /* ·�ڱ�ע��Ϣ */
  char						cDetectType;           	 	 /* �����Ȧ���� */
  char						cValidChanSum;				 /* ʵ����Ч�������� */
  char						cCarDistance;				 /* ���һ��ͼƬ��������ͣ���߾��루��λ�ף� */
  char						cFrameInterval;				 /* ץ�ļ��֡������Χ1~15�� */
  char						cLightAmendDisFlag;		     /* ���������Ȧ�Ƿ���ʾ��־ */
  char						cFlashCtlMode;			     /* ����ƿ���ģʽ */
  unsigned short            usXcsProVer;			     /* xcsСЭ��汾�� */
  char						szRoadId[8];			     /* ·��id */
  
  char						cLightAmendEn;				 /* ��������Ƿ�ʹ�� */
  char						cRLDectType;				 /* ��Ƽ�ⷽʽ */
  char						cOSDColour;				 	 /* ����������ɫ(0:�ڰ� 1:��ɫ) */
  char						cRLAmendLevel;			 	 /* ���������ǳ�ȼ� */
  char						cPPSendEn;				 	 /* ����ͼƬ�Ƿ��ϱ���־ */
  char						cPPFlashEn;					 /* ����ͼƬ�Ƿ������־ */
  char                      cRsv[2];

  char	 					szDirId[8];					 /* ����id */
  STR_SYS_RED_LIGHT_MODE_SET strRedLightMode[MAX_AREA_GROUP_NUM];/* ����������ģʽ���� */

  char						cACSyncSwitchEn;			 /* AC������λ�л�ʹ�� */
  unsigned char				ucACSwitchThreshold;		 /* AC������λ�л��Ҷȷ�ֵ */
  unsigned char				ucACDecCycle;				 /* AC������λ������� */
  
  char						cRedModifyMode;			 	 /* �������������1����ɰ汾 2���Ͼɰ汾 
															3���·�����ȫ�Զ���4���·������Զ�1��
															5���·������ֶ�,���ް׵㣩
															6���·������ֶ����ް׵㣩*/
															
  unsigned short			usDayACPhase;				 /* ����AC������λֵ */
  unsigned short			usNightACPhase;				 /* ����AC������λֵ */  
  
  char						szExEpDevCode[EXT_DEV_CODE_LEN];/* ��չ�豸����ַ��� */

  unsigned char				ucSmartSwitchEn;		/* ���������л�ʹ�� */
  unsigned char				ucTimerSwitchEn;		/* ���ⶨʱ�л�ʹ�� */
  char						cRsv2[2];
  unsigned int				uiFillInLightBeginTime;	/* ������ʼʱ�� */
  unsigned int				uiFillInLightLastTime;	/* ����ʱ�� */
  
}ELEC_POLICE_OVERALL_CONF_INFO;

/* ���Ӿ��쳵����ѯ��Ϣ */


/* strRedLDesp COM ����ʱ�Ľṹ���� */
typedef struct STR_SYS_RED_LIGHT_COM_DESP
{
    char    cComId;
    char    cCarRoadId;
	char 	cRsv[2];
}_PACKED_ STR_SYS_RED_LIGHT_COM_DESP, *PSTR_SYS_RED_LIGHT_COM_DESP;

/* strRedLDesp GPIO ����ʱ�Ľṹ���� */
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

/* �羯������Ȧ������ṹ���� */
typedef struct STR_VL_POINT
{
	unsigned short usVLX1;
	unsigned short usVLY1;
}_PACKED_ STR_VL_POINT, *PSTR_VL_POINT;

/* �羯��ͨ�ƽṹ�嶨�� */
typedef struct TAG_STR_SYS_TRAFFIC_LIGHT
{
	char cEnable;
	char cLightType;
	char cVehicleNum;
	char cResv[1];
}_PACKED_ STR_SYS_TRAFFIC_LIGHT, *PSTR_SYS_TRAFFIC_LIGHT;


typedef struct STR_SYS_ELEC_POLICE_VEHICLE_CONF_INFO
{
	unsigned int                uiCarRoadId;        /* �����ţ���1��ʼ */
	unsigned short		        usOSDPosX;          /* ץ��ͼƬ����ʾ��X���� */
	unsigned short              usOSDPosY;          /* ץ��ͼƬ����ʾ��Y���� */
	char		                szOSDContent[32];   /* ץ��ͼƬ��OSD���� */
	STR_SYS_AREA                strRoadArea;        /* ���������� */
	unsigned short              usRedLDetectType;   /* ��Ƽ�ⷽʽ */
	unsigned short              usRedLConnectMode;  /* ��Ƽ�������豸���ӷ�ʽ */
	STR_SYS_EP_RED_LIGHT_DESP   strRedLDesp;        /* ��Ƽ������ */
	unsigned short              usVehicleType;      /* ���������� */
    unsigned short              usVehicleLinkMode;  /* ���������ӷ�ʽ */
    STR_SYS_COIL_DESP           strCoilDesp;        /* �ظ���Ȧ���� */
    unsigned int                uiForcedRedLight;	/* ǿ�ƺ�� */
    STR_VL_POINT				strVLPoint[4];		/* ������Ȧ����λ�ö��� */
    STR_SYS_AREA				strTrafficLArea;	/* ������Ȧ��ÿ������Ӧ�Ľ�ͨ���������� */
    STR_SYS_TRAFFIC_LIGHT		strTrafficLight[MAX_LIGHT_TYPE];	/* ��ͨ�������Ϣ�ṹ�� */
    char                        cLaneDir;			/* ���������� */
    char						cVLDispFlag;		/* ������Ȧ�Ƿ���ʾ��־ */
    char						cLaneLoopDispFlag;	/* ������Χ��Ȧ�Ƿ���ʾ��־ */
    char						cCarNoDir;			/* ���з������Զ��� */
	char						cRsv5[4];			/* �ֽڱ��� */
	char						cCarDistance;		/* ���һ��ͼƬ��������ͣ���߾��루��λ�ף� */
  	char						cFrameInterval;		/* ץ�ļ��֡������Χ1~15�� */
    
    unsigned short	    		usFlashLightType;    /* ��������� */
	char        	    		cDetectType;         /* �����Ȧ���� */
	char						cCoilSwitchFlag;	 /* ��Ȧ���ģʽ�л�ʹ�ܱ�־ */
	char						cCoilSwitchSta;		 /* ��Ȧ���ģʽ�л�״̬ */
	char						cRedLightDectSta;	 /* ��Ƽ��ģʽ״̬��ѯ */
	char						cInLightSta;	     /* ���ⷽʽ״̬ */
	char						cVLDecEn;			 /* ������Ȧ�Ƿ���ʹ�� */
    char						cCarSnapDir;		 /* �Ƿ��жϳ��з��� */
	char						cCarTrackEn;		 /* �켣��������ʹ��λ */
	STR_VL_POINT				strNoLaneVLPoint[2]; /* ����������ʻ����������λ�ö��� */
	unsigned char        	ucMinPlatePixelSize; 	 /* ��С�������� */
	unsigned char        	ucMaxPlatePixelSize; 	 /* ��������� */
	unsigned char        	ucCorrEveningThresh; 	 /* �ƻ�����ֵ */
	char 					cRsv;
    unsigned short        	usCoilDist;              /* ��Ȧ֮��ľ��룬����Ϊ��λ */
    unsigned char			ucPPDelaySnapFrames;	 /* ����ͼƬ��ʱץ��֡�� */
    char 					cRsv1[1];
    char					szLaneId[8];			 /* ������id */
    
    char					cSwitchLaneEn;			 /* �Ƿ�Ϊ�ɱ䳵�� */
    char					cSwitchLaneLeftNo;		 /* �ɱ䳵����ת����ƺ� */
    char					cSwitchLaneStraightNo;	 /* �ɱ䳵��ֱ�з���ƺ� */
    char					cRsv3;
}ELEC_POLICE_VEHICLE_CONF_INFO;

/* ������Ȧ���� */
typedef struct STR_SYS_VL_CONF_INFO
{
	unsigned int                uiCarRoadId;        /* ������,��1��ʼ */
	unsigned short              usThrDayToNight;    /* ����תΪҹ��Ĳο����� */
	unsigned short              usThrNightToDay;    /* ҹ��תΪ����Ĳο����� */
	unsigned int                uiCorrNightThresh;  /* ҹ�����ֵ���� */
	unsigned int                uiCorrDayThresh;    /* �������ֵ���� */
	unsigned int                uiCorrMotorThresh;  /* Ħ�г����ֵ���� */
	unsigned int                uiCorrEveningThresh;/* �ƻ�����ֵ */
	unsigned int                uiDayMotorWidth;    /* ����Ħ�г��������ֵ */
	unsigned int                uiNightMotorWidth;  /* ҹ��Ħ�г��������ֵ */
	unsigned char				ucCapDelayInterval; /* �ɼ��ӳ�֡�� */

	char						cCapDelayNum;		/* ���Ƽ����֡�� */
	char        		        cMinPlatePixelSize;	/* ��С�������� */
	char        		        cMaxPlatePixelSize;	/* ��������� */
}VL_CONF_INFO;

/* Sys default Conf and sta*/
typedef struct SysDefConfInfo
{
	int  iVehiSta;	/* �������ϵ繤��״̬��Ĭ��Ϊ���� */
}SYS_DEF_CONF_INFO;

/* �羯Υ���������Ƽ����������Ϣ */
typedef struct STR_EP_IRREG_CONF_INFO
{
	char 			cDispFlag;			/* Υ�������Ƿ���ӵ�ͼƬ�� */
	char			cRsv[3];
	char			szRunRedCode[8];	/* �����Υ�´��� */
	char			szRunRedName[64];	/* �����Υ������ */
	
	char			szReverseRunCode[8];/* ����Υ�´��� */
	char			szReverseRunName[64];/* ����Υ������ */

	char			szInfringForbidLineCode[8]; /* Υ����ֹ������ʻΥ�´��� */
	char			szInfringForbidLineName[64];/* Υ����ֹ������ʻΥ������ */

	char			szInfringDirLaneRunCode[8]; /* �������򳵵���ʻΥ�´��� */
	char			szInfringDirLaneRunName[64];/* �������򳵵���ʻΥ������ */

	char			szStopOnTrafficNoEntryCode[8]; /* ·�����½�ֹ����Υ�´��� */
	char			szStopOnTrafficNoEntryName[64];/* ·�����½�ֹ����Υ������ */

	char			szHighwayInfringDirLaneCode[8]; /* ����·�������򳵵���ʻΥ�´��� */
	char			szHighwayInfringDirLaneName[64];/* ����·�������򳵵���ʻΥ������ */

	char			szStoppedVehicleCode[8]; 		/* Υ��ͣ��Υ�´��� */
	char			szStoppedVehicleName[64];		/* Υ��ͣ��Υ������ */

}EP_IRREG_CONF_INFO;

/* �羯����Υ���������Ƽ����������Ϣ */
typedef struct STR_EP_NEW_ADD_IRREG_CONF_INFO
{
	char			szRunInNoLaneCode[8]; 		/* ���ڻ�����������ʻΥ�´��� */
	char			szRunInNoLaneName[64];		/* ���ڻ�����������ʻΥ������ */

}EP_NEW_ADD_IRREG_CONF_INFO;

/* �羯����ʶ��������Ϣ */
typedef struct tagSTR_EP_LPR_CONF_INFO
{
	unsigned char ucLPREn;					/* ����ʶ��ʹ�ܱ�־ */
	unsigned char ucLPRCConfidence;			/* �������ַ����Ŷ� */
	char		  cRsv[2];
	char    szLPRFirstChar[6];				/* �������ַ� */
	unsigned short usLPRWidth;				/* ����ʶ���� */
	unsigned short usLPRHeight;				/* ����ʶ��߶� */
}EP_LPR_CONF_INFO;

/* FTP ������Ϣ */
typedef struct STR_SYS_FTP_CONF_INFO
{
	unsigned int    uiEnable;           /* �Ƿ�ʹ��FTP */
	unsigned int    uiFtpIP;            /* FTP��ַ */
	unsigned short  usFtpPort;          /* �˿ں� */
	char			cRsv[2];
    char 		    szUserName[128];     /* �û��� */
    char 		    szUserPasswd[128];   /* ���� */
    char			cTRecEnable;
    unsigned char   ucTRecLast;
    unsigned short  usTRecInterval;
}FTP_CONF_INFO;

/* NTP ������Ϣ */
typedef struct STR_SYS_NTP_CONF_INFO
{
	unsigned int uiEnable;		/* �Ƿ�ʹ�� */
	unsigned int uiServerIP;	/* NTP������ */
	unsigned short usJudgeCycle;/* ��ʱ����(��λ:����) */
	unsigned char  ucJudegPrecision;/* ��ʱ����(��λ:��) */
	unsigned char ucRsv;
}NTP_CONF_INFO;

/* �����ʱ��ο��� */
typedef struct STR_SYS_CAMERA_TIME_RANGE
{
	char		cGroupId;
	char		cEnable;
	char		szSceneName[30];
	unsigned int iBeginTime;
	unsigned int iLastTime; /* ����ʱ�� */

	char		cIrisPos;	/* �Զ���Ȧλ�� */
	char		cRsv[3];
}CAMERA_TIME_RANGE_INFO;


/* �羯����ʱ��ο��� */
typedef struct STR_EP_TIME_RANGE
{
	char		cForbidType;
	char		cRange1Enable;
	char		cRange2Enable;
	char 		cRsv1;
	int			iBeginTime1;
	int			iLastTime1; 	/* ����ʱ��1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* ����ʱ��2 */	
}EP_TIME_RANGE_INFO;

/* �羯������ƿ��� */
typedef struct STR_EP_ASSIST_RED_LIGHT
{
	char		cRedLightNum;
	char		cEnableFlag;
	char 		cRsv[2];
	unsigned int iBeginTime1;
	unsigned int iLastTime1; 	/* ����ʱ��1 */
	unsigned int iBeginTime2;
	unsigned int iLastTime2; 	/* ����ʱ��2 */	
	unsigned int iBeginTime3;
	unsigned int iLastTime3; 	/* ����ʱ��3 */
}EP_ASSIST_RED_LIGHT_INFO;

/* �羯���½������� */
typedef struct STR_EP_STOP_ON_TRAFFIC_NO_ENTRY
{
	char		cEnableFlag;
	char 		cRsv[3];
	unsigned int iBeginTime1;
	unsigned int iLastTime1; 	/* ����ʱ��1 */
	unsigned int iBeginTime2;
	unsigned int iLastTime2; 	/* ����ʱ��2 */	
	unsigned int iBeginTime3;
	unsigned int iLastTime3; 	/* ����ʱ��3 */
}EP_STOP_ON_TRAFFIC_NO_ENTRY;

typedef struct STR_BAYONET_JPG_OSD_CONF
{ 
    unsigned char   ucDevSnOsdEn;		/* �豸��� */
	unsigned char	ucPpNameOsdEn;		/* ������ */
	unsigned char	ucDirNameOsdEn;		/* ���� */
	unsigned char	ucLaneNameOsdEn;	/* ������ */
	
	unsigned char	ucSnapTimeOsdEn;	/* ץ��ʱ�� */
	unsigned char	ucCarDirOsdEn;		/* ���з��� */
	unsigned char	ucBusNumOsdEn;		/* ���ƺ��� */
	unsigned char	ucLPColorOsdEn;		/* ������ɫ */
	
	unsigned char	ucCarTypeOsdEn;		/* ���� */
	unsigned char	ucCarColorOsdEn;	/* ������ɫ */
	unsigned char	ucCurSpeedOsdEn;	/* ���� */
	unsigned char	ucPunishSpeedOsdEn;	/* �����ٶ� */
	
	unsigned char	ucMarkSpeedOsdEn;	/* ��ʾ�ٶ� */
	unsigned char	ucSpeedRatioOsdEn;	/* ���ٱ��� */
	unsigned char	ucReguCodeOsdEn;	/* Υ�´��� */
	unsigned char	ucStandReguDesOsdEn;/* ��׼Υ������ */
	
	unsigned char	ucIntelReguDesOsdEn;/* �ڲ�Υ������ */
    char            cRsv[3];
} BAYONET_JPG_OSD_CONF;

typedef struct STR_EP_JPG_OSD_CONF
{
    unsigned char   ucDevSnOsdEn;		/* �豸��� */
	unsigned char	ucRoadNameOsdEn;	/* ·���� */
	unsigned char	ucDirNameOsdEn;		/* ���� */
	unsigned char	ucLaneNameOsdEn;	/* ������ */
	
	unsigned char	ucSnapTimeOsdEn;	/* ץ��ʱ�� */
	unsigned char	ucBusNumOsdEn;		/* ���ƺ��� */
	unsigned char	ucLPColorOsdEn;		/* ������ɫ */
	unsigned char	ucCarTypeOsdEn;		/* ���� */
	
	unsigned char	ucCarColorOsdEn;	/* ������ɫ */
	unsigned char	ucReguCodeOsdEn;	/* Υ�´��� */
	unsigned char	ucStandReguDesOsdEn;/* ��׼Υ������ */
	unsigned char	ucIntelReguDesOsdEn;/* �ڲ�Υ������ */
	
	unsigned char	ucRDBeginTimeOsdEn; /* ��ƿ�ʼʱ�� */
	unsigned char	ucRDCycleOsdEn;		/* ������� */
	unsigned char	ucRoadDespOsdEn;	/* ·������ */
	char            cRsv[1];


}EP_JPG_OSD_CONF;

/* �����������Ϣ */
typedef struct STR_SYS_CAMERA_CTRL_CONF_INFO
{
	unsigned int 	uiCamaraType;   /* ����������� */
	float			fMaxVal;        /* ���ֵ */
	float			fMinVal;        /* ��Сֵ */
	float			fFixVal;	    /* �̶�ֵ */
	char			cCtlType;       /* ��������Ʒ�ʽ */
	char 			cCamGroup;		/* ������������ */
	char 			cRsv2[1];
	char 			cRsv3[1];
}CAMERA_CTRL_CONF_INFO;

/* ����Ӳ����Ϣ */
typedef struct STR_SYS_NET_HDD_CONF_INFO
{
	unsigned int  uiEnable;             /* �Ƿ�ʹ������洢 */
	unsigned int  uiStorageType;        /* �洢���� */
	unsigned int  uiIpAddr;             /* IP */
	char		  szFolderName[32];     /* Ŀ¼ */
    char	        szUserName[32];
    char	        szPassWord[32];
    unsigned int  uiAvaiable;
    unsigned int  uiUDiskSum;
}NET_HDD_CONF_INFO;

/* NRM�豸������Ϣ */
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

/* �ɼ��豸��Ϣ */
typedef struct STR_SYS_DCTL_CONF_INFO
{
	char   			cEnable;           /* �Ƿ�ʹ�� */
	char   			cComId;             /* ���ں� */
	char			cRecv[2];
	unsigned int   	uiVehicleType;      /* ������� */
    char            cDatabits;
    char            cStopbits;
    char            cParity;
	char			cRsv;
    unsigned int    uiBaudrate;
    unsigned short  usMaxMeasureSpeed;
	char 			cRecv2[2];
}DCTL_CONF_INFO;

/*ƽ̨�豸��Ϣ*/
typedef struct STR_SYS_PLATFORM_CONF_INFO
{
    char            cEnable;
    char			cRsv[3];
    char            sz330DriveWayId[4];
    char            sz330DevId[128];
    char            sz330DirId[16];
    char            sz330Key[256];
}PLATFORM_330_CONF_INFO;

/* ���ܽ���������Ϣ */
typedef struct STR_SYS_NM_CONF_INFO
{
    unsigned int    uiNMSerIp;
    unsigned short  usNMPort;
    char	        cEnable;
	char 			cDefaultGWEn;	/* Ĭ������ʹ�ܱ�־ */
    unsigned int    uiDefaultGWIp;	/* Ĭ������ip��ַ */
    unsigned int 	uiDefaultHttpdPort;/* Ĭ��httpd�����������˿ں� */
}SYS_NM_CONF_INFO;


/* �����Ƿ�ʹ�ܱ�־ */
typedef struct STR_SYS_FUNC_ENABLE_INFO
{
	char		 cFlowRateFlag;
	char		 cAlarmRecFlag;
	char		 cSpeedAmendFlag;
	char		 cFlowRateInter;		/* ����ͳ���ϱ�ʱ���� */
	
	char		 cXmlFileSendFlag;		/* xml�ļ��Ƿ���ʹ�� */
	unsigned char ucDspFramDifEn;		/* dsp֡�����㷨�Ƿ�ʹ�� */
	unsigned char ucDspStopDistan;		/* dsp��ⷶΧʵ�ʾ��룬��λΪ�� */
	unsigned char ucDspOneLampEn;		/* dsp�Ƿ��ⵥ���� */

	unsigned short usFlashStartLine;	/* kodar�豸������ʱ���� */
	char		  cSpeedAmendX;			/* �ٶ�У����ĸֵ */
	char		  cSpeedAmendY;			/* �ٶ�У����ȥֵ */
	
	unsigned char ucRedLightDelay;		/* �����ʱ��Ч���� */
	unsigned char ucRedTurnGreenSnapEn;	/* ��ת�̻���ת���費��Ҫץ�� */
	unsigned char ucNoDirReportSnapEn;	/* �㷨δ������ʱ�費��Ҫץ�� */
	unsigned char ucEpPpPicMode;		/* ����ͼƬ�ϳ�ģʽ(0:ȫ��+��д;1:ȫͼ)  */
	
	unsigned short usFreqFlashDelayUSec; /* Ƶ������ʱʱ������ */
	unsigned short usppFLightDelayUSec;	/* �����������ʱʱ������ */
	unsigned short usepFLightDelayUSec;	/* �羯�������ʱʱ������ */
	unsigned char ucDebugLogSaveFlag;	/* ������Ϣ�Ƿ񱣴�Ϊ�ļ���־���� */
	unsigned char ucArithDebugShowFlag;	/* �㷨���ٵ�����Ϣ�Ƿ���ʾ��־ */
	
	unsigned char ucVDLGreenMode;		/* ��ͨ����Ƶ�����̵Ʊ�־ */
	unsigned char ucPressLineSanpFlag;	/* ѹ���Ƿ�ץ�ı�־ */
	unsigned char ucBackWordsSanpFlag;	/* �����Ƿ�ץ�ı�־ */	
	unsigned char ucEpPpSnap0Or1;		/* �羯����ʹ�õ�0�����ǵ�1�� */

	unsigned char  ucUdiskWeldSta;		/* ���ø�Ӳ��������û�к���U�� */
	unsigned char ucEpCarSpeedOsdEn;	/* �羯����ֵOSD�Ƿ���ӵ�ͼƬ�� */
	unsigned short usFreqFlashStartLine;/* kodar�豸Ƶ������ʱ���� */

	unsigned char  ucUdiskEn;			/* U���Ƿ�ʹ�� */
	unsigned char  ucUFdiskEn;			/* �Ƿ����ǿ�Ƹ�ʽ��U�� */
	unsigned short usNetVehiclePort;	/* ���ڽ�ͨ�Ƽ�����˿ں� */
	
	unsigned char	ucEpPpSnapPosition;	/* �羯�ظ���Ȧ����ץ��λ�ã�0��ǰ��Ȧ1��1����Ȧ2��2��ʾ����Ȧ1,3��ʾ����Ȧ2 */
	unsigned char	ucPicQuality;		/* jpeg�������ӣ�Խ������Խ�� */
	unsigned short  usRedLReviseShutter;/* ����������ŷ�ֵ����ǰ����ֵ<=����ֵʱ��ʾ����Ҫ��
											>����ֵʱ���ϲ��� */

	char		   cGammaVal;			/* gammaֵ */
	char		   cFlashLineMode;		/* ��̬���ŵ���ģʽ */
	char	 	   cDyncJudFlashLines;	/* ��̬������������ */
	unsigned char  ucPlateSegFlag;		/* �Ƿ������Ʒָ�� */

	unsigned short usCsSpeedAmendNumerator;	/* �״��ٶ�У������(��1000Ϊ��ĸ) */
	char		   cDubalFEn;				/* ˫����ģʽ�Ƿ�ʹ�ܱ�־ */
	unsigned char  ucRedLReviseDegree;		/* ���Ϻ�������̶� */

	unsigned short  usSaturation;			/* ���ͶȲ������� */
	unsigned short  usGPixsThresh;			/* G�������ظ�����ֵ */

	float			fDubalFShut;			/* ˫����ģʽ����ֵ */
	float			fDubalFGain;			/* ˫����ģʽ����ֵ */

	char			cRedDecSaturation;		/* ��Ƽ�ⱥ�Ͷ� */
	char			cRedDecBright;			/* ��Ƽ������ */
	unsigned char 	ucChangeRoad;			//�Ƿ�֧�ֱ��ץ��
	unsigned char   ucPicAssistDebug;		//�Ƿ�ʹ��ͼƬ������Ϣ�ϱ�
	
	unsigned char	ucCrossLineParkEn;		//�Ƿ�ʹ��Խ��ͣ��Υ��ץ��
	char 			cLaneChgType[3];

	unsigned char   ucSDEn;					/* SD���Ƿ�ʹ�� */
	unsigned char   ucSDFdiskEn;			/* �Ƿ����ǿ�Ƹ�ʽ��SD�� */
	char			cRsv[1];
	char			aucPressLine[5];
	char			cOnvifEn;				//onvif�����Ƿ�������־
	char			cSipSdkEn;				//sipsdk�����Ƿ�������־
	char			cV2En;					//v2�����Ƿ�������־
	char			cV3En;					//v3�����Ƿ�������־

	char			cHaloTakeOutEn;			//��ƹ�������ʹ��
	char			cHaloAreaRange;			//��ƹ���������Χ
	char			cGB28181En;				//GB28181�����Ƿ�������־
	char			cRestartTimeEn;			//��ʱ����ʹ��

	unsigned int	uiRestartTime;			//��ʱ����ʱ��
}SYS_FUNC_ENABLE_INFO;

/* ������ع������� */
typedef struct STR_PP_FUNC_ENABLE_INFO
{   
	unsigned char   ucOsdPlateScopeEn;	//�Ƿ���ͼƬ�ϵ��ӳ���ʶ������
	unsigned char	ucisShieldReverse;	//�Ƿ����������ϱ�ͼƬ
    unsigned short  usPlateScopeIncSize;//(������㷨����)���Ʒ�Χ�����ӵ�ֵ
    
    unsigned char   ucRadarSpeedGap;	//�״���ֵ(������϶���֡���ٶȺ������ٶ�ʱ�϶�����һ����)
    unsigned char   ucRadarForwardLim;	//�״��ٶ���ǰ��Χ(�ٶ���ǰ����ͬ��������ֵ)
    unsigned char   ucRadarBackwardLim;	//�״��ٶ����Χ(�ٶ��ӳٱ���ͬ��������ֵ)
	unsigned char   ucIsDecTaillight;	//�Ƿ�ʹ��β�Ƽ��
	
	unsigned char	ucFaceIdenEn;		//����ʶ��ʹ��
	unsigned char	ucOlviaRadaParalEn;	//����ά���״ﲢ��ģʽʹ�ܣ�Ĭ��Ϊ1
	unsigned char 	ucFaceOvenlayEn;	//��������Ƿ����ʹ��
	unsigned char   usIsReverseSnapNorm;	//0-������1-���в�������ץ�Ķ���������ʻץ�ģ�ֻץ��һ���ϱ�

	//unsigned char   ucRsv[1];
}PP_FUNC_ENABLE_INFO;


/* �羯������Ȧ�������������Ϣ */
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


/* 330ƽ̨����������Ϣ */
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

/* GB28181ƽ̨����������Ϣ */
typedef struct STR_SYS_GB28181_CONF_INFO
{
	char			szServerId[32];			/* ������id */
	char			szServerDomain[32];		/* �������� */
	char			szServerIp[32];			/* ������ip */
    unsigned int    uiServerPort;			/* ������port */
    unsigned int	uiRegisterExpiry;		/* ע����Чʱ�� */
    unsigned int    uiKeepLiveTime;			/* �������� */
    unsigned int    uiConMode;				/* ���ӷ�ʽ: 0:udp 1:tcp */
    char            szClientId[32];
    char            szClientUserName[32];
    char            szClientPasswd[32];
    unsigned int    uiPackMode;				/* rtc �����ʽ: 0:PS 1:H264 */

    char			cEnFlag;				/* ���ñ�־ */
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

/* ���в����ĺϷ����ɵ���������֤  */


int SysConfGet(CONF_TYPE nConfType,void *pConf,int index);
int SysConfSet(CONF_TYPE nConfType,void *pConf,int index);
const char * GetProductSerialNo();

int SaveWriteFile(void);
int  SaveCamCtrlConf(void);


//add by wangb for �����������ϱ�ģ��

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

