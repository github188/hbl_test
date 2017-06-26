/******************************************************************************
 * Jpeg_snap.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.23, xcsworker Create
 * --------------------
 ******************************************************************************/
#ifndef _JPEG_SNAP_H
#define  _JPEG_SNAP_H

#define MAX_PIC_NUM         (4)
#define MAX_FILE_NUM        (5)
#define MAX_LANE_NUM        (4)

#define   	DBG_ERR    1
#define		DBG_INFO   2
#define 	DBG_WARN   3
#define 	DBG_ALL    4

#define 	MAX_FACE_NUM 5

#define   TRV_FILE_TYPE   	(0x66)
#define   ASF_FILE_TYPE  	(0x55)
#define   JPG_FILE_TYPE		(0x11)
#define   XML_FILE_TYPE   	(0x22)


#define	  MAX_IRREG_CODE_LEN	8
#define	  MAX_IRREG_NAME_LEN	64

#define   FTP_MAX_DEV_CODE_LEN  20

#define   FTP_MAX_SN_LEN  		17

/* DSP上报的详细违章类型 */

#define EPPUNISH_DATATYPE_REDLAMP           (21)  /* 闯红灯 */

/* 机动车违反禁令标志指示 如闯禁左、闯禁右行驶，尾号限行等。 */
#define EPPUNISH_DATATYPE_FORBIDLEFT        (22)  /* 禁左 */
#define EPPUNISH_DATATYPE_FORBIDRIGHT       (23)  /* 禁右 */
#define EPPUNISH_DATATYPE_FORBIDSTRAIGHT    (24)  /* 禁直 */

#define EPPUNISH_DATATYPE_FORBID_ODDANDEVEN (30)  /* 单双号限行 */

#define SNAP_PUNISHTYPE_REVERSE             (25)  /* 逆行 */

/* 机动车违反禁止标线指示 如压白实线、压黄线行驶 */
#define SNAP_PUNISHTYPE_PRESSSOLIDE 		(26)  /* 压线 */
#define SNAP_PUNISHTYPE_CHANGELANE  		(27)  /* 变道 */

/*  通过路口遇停止信号时，停在停止线以内或路口内
 如闯红灯行为车辆行驶到一半停止(俗称 红转绿) */
#define EPPUNISH_DATATYPE_RED_SHIFT_GREEN  (31)   /* 红转绿 */

/* 机动车不按规定车道行驶 */
#define EPPUNISH_DATATYPE_LEFT_SHIFT_DIR   (32) /* 左转变直行 */
#define EPPUNISH_DATATYPE_LEFT_SHIFT_RIGHT (33) /* 左转变右转 */

#define EPPUNISH_DATATYPE_DIR_SHIFT_LEFT   (34) /* 直行变左转 */
#define EPPUNISH_DATATYPE_DIR_SHIFT_RIGHT  (35) /* 直行变右转 */

#define EPPUNISH_DATATYPE_RIGHT_SHIFT_DIR  (36) /* 右转变直行 */
#define EPPUNISH_DATATYPE_RIGHT_SHIFT_LEFT (37) /* 右转变左转 */

#define EPPUNISH_DATATYPE_LEFTDIR_SHIFT_RIGHT (40) /*左转直行变右转*/
#define EPPUNISH_DATATYPE_DIRRIGHT_SHIFT_LEFT (41) /*直行右转变左转*/

/* 机动车违反规定停放、临时停车且驾驶人不在现场，妨碍其它车辆、行人通行的 */
#define EPPUNISH_DATATYPE_TEMPORARY_PARK   (38) /* 绿灯时忽然停车，影响后续车通行 */

/* 机动车不在机动车道内行驶的  */
#define EPPUNISH_DATATYPE_NO_CARLANE_DRIVER (39)

/* 路口遇有交通阻塞时未依次等候的(遇堵禁行) */
#define EPPUNISH_DATATYPE_STOP_ON_TRAFFIC_NO_ENTRY 	  (44)

/* 行经交叉路口不按规定行车或者停车的 */
#define EPPUNISH_DATATYPE_REDLIGHT_PARK 	  (45)

/* 高速公路上不按规定车道行驶的 */
#define HIGHWAY_EPPUNISH_RUN				(46)  /* 高速不按规定车道行驶 */

/* 交警实际定义的违章类型代码 */

/* 机动车不在机动车道内行驶的  */
#define EP_IRRE_CAR_NO_DRIVER_IN_CAR_LANE   (1018)

/* 机动车不按导向车道行驶的 */
#define EP_IRRE_NO_RUN_TRACK      (1208)

/*  通过路口遇停止信号时，停在停止线以内或路口内
 如闯红灯行为车辆行驶到一半停止(俗称 红转绿) */
#define EP_IRRE_RED_SHIFT_GREEN   (1211)

/*机动车违反禁令标志指示 (如闯禁左、闯禁右行驶，尾号限行)*/
#define EP_IRRE_FORBID_TRACK      (1229)

/* 路口遇有交通阻塞时未依次等候的(遇堵禁行) */
#define EP_SOT_NO_ENTRY      	  (1228)

/* 机动车违反禁止标线指示 (如压白实线、压黄线行驶) */
#define EP_IRRE_PRESSSOLIDE	      (1230)

/* 机动车逆向行驶 */
#define EP_IRRE_REVERSE_TRACK     (1301)

/* 机动车不按交通信号灯规定通行 (如闯红灯行驶) */
#define EP_IRRE_REDLAMP_TRACK     (1302)

/* 机动车不按规定车道行驶 (如占用专用公交车道，非机动车道) */
#define EP_IRRE_NO_DIR_TRACK      (1042)

/* 机动车违反规定停放、临时停车且驾驶人不在现场，妨碍其它车辆、行人通行的 */
#define EP_IRRE_TEMPORARY_PARK 	  (1039)

/* 驾驶机动车在高速公路上不按规定车道行驶的 */
#define EP_HIGHWAY_EPPUNISH_RUN	  (4312)

/* 行经交叉路口不按规定行车或者停车的 */
#define EP_PUNISH_DATATYPE_REDLIGHT_PARK (16252)

typedef struct
{
	char cSta;		/* 状态:N R Y G S */
	char szBT[7];	/* 本状态开始时间(HHMMSS) */
}strTrafficLightInfo;

typedef enum
{
    e_cxDiskLightSta =0,     		/* "圆盘灯状态" */
    e_cxLeftArrowLightSta,			/* "左箭头灯状态" */
    e_cxDirArrowLightSta, 			/* "直箭头灯状态" */
    e_cxRightArrowLightSta, 		/* "右箭头灯状态" */
    e_cxTurnAroundLightSta, 		/* "调头灯状态" */
    e_cxLeftArrowAroundLightSta, 	/* "左箭头调头灯状态" */
}enumTrafficLightSta;

/* 慈溪接入信息收集结构体 */
typedef struct
{
	char cFillLightSta[3];		/* 补光灯状态 */
	unsigned char ucLimitSpeed;	/* 道路限速 */

	char szTrafficLSet[12];		/* 信号灯设置组合 */
	
	strTrafficLightInfo strAllTrafficLightInfo[6];	/* 六组灯状态及开始时间 */
	
	char cYellowFlashSta;	/* 黄闪灯状态 */
	char cLaneLightSta;		/* 车道灯状态 */
	char cDriverTrack;		/* 行驶轨迹:R D L T S W */
	char cDevTemprature;	/*设备温度*/

	char cLaneId;			/* 车道号 */
	char cRsv[3];
}strCXProInfo;

typedef struct
{
	int start_x;
	int start_y;
	int width;
	int height;
}strLPRect;



typedef enum
{   
    eSnapPort=1, // port 
    eSnapEP=2, // ep
}SNAP_TYPE;

typedef enum
{
    eEP_PIC_OF_XML_DESP= 21,
    eNOLEFT_PIC_OF_XML_DESP= 22,
    eNORIGHT_PIC_OF_XML_DESP= 23,
    ePORT_PIC_OF_XML_DESP = 12,
    eLANE_OF_XML_DESP=11,
    ePORT_OF_XML_DESP=10,
    eEP_OF_XML_DESP=20,
    eTRAFFIC_VOLUME_XML_DESP = 31,
}XML_DESP;



typedef enum
{
    eOverSpeed = 1,
    eJayWorking = 2,// red light
}IrregType;

#define VEHICLE_TYPE_CAR  (0x01)
#define VEHICLE_TYPE_MOTO (0x02)

typedef struct
{
    unsigned int   m_confidence;//置信度 
    short m_left/*矩形*/;
    short m_top;
    short m_right;
    short m_bottom;
    short m_type;/*指示是汽车还是摩托车,VL库返回值,0-汽车,1-摩托*/
    short m_resv;
}SnapPosInfo;
typedef struct
{
    int m_speed;
    int m_speed_thresh;
    int m_irregularities;
    int m_irregularities_type;
}PassPortSnapInfo;

typedef struct
{
    unsigned int m_redlight_begin;
    unsigned int m_redlight_duration;
    unsigned int m_noturnbegintime;

    int m_speed;
    int m_speed_thresh;
}EpSnapInfo;

typedef struct
{
    union 
    {
        PassPortSnapInfo    port_snap_info;
        EpSnapInfo			ep_snap_info;
	}SnapPicInfo;
    SnapPosInfo     pos_info;
}JpegSnapParam;

typedef struct _FTP_SNAP_PIC_TIME
{
    unsigned int uiSec;
    unsigned int uiMSec;
}FTP_SNAP_PIC_TIME;

typedef struct tag_FTP_FACE_PIC_INFO_
{
    unsigned int uiPicAddrOffset;	/* 相对于缓存buf基址的偏移 */
    unsigned int uiPicLen;			/* 人脸图片大小 */
}STR_FTP_FACE_PIC_INFO;

/* 双线圈、单线圈前、单线圈后 */
#define  FTP_VIRTUAL_COIL_TYPE					(0x00)	//虚拟线圈
#define  FTP_GROUD_DUBAL_COIL					(0x01)	//地感双线圈
#define  FTP_GROUD_SINGAL_COIL_FRONT			(0x02)	//地感单线圈前
#define  FTP_GROUD_SINGAL_COIL_BEHIND			(0x03)	//地感单线圈后

typedef struct
{
        int                          iPicNum;
        int                          iCurrPicIndex;
        int                          iGroupId; 
        char                       szDirection[128];     
        int                          iLaneIndex;
        char                        szLaneName[128];    
        char                       *pcPicBuf;
        unsigned int            auiPicLen[MAX_PIC_NUM];
        int                         iCombineType;
        int                         iSelPicIndex;
        unsigned int            uiSnapTimeSecs;
        unsigned int            uiSnapTimeMsSecs;
        JpegSnapParam 	  strParam;
        char			  cChanDir;				/* 车道行驶方向 */
        char			  cFileType;
        /*Start of 2.0.2.0 on 2011-6-9 11:12 its*/
        char			  cAsfSendFalg;
        char              cEpPpSelStep0Or1;
        /*End of 2.0.2.0 on 2011-6-9 11:12 its*/
        char					szPortId[8];
        
        unsigned char			ucSnapType;	//抓拍类型
		unsigned char			ucCoilDecType;	//地感线圈检测方式
        unsigned short  		usSnapGroupId;	//地感线圈抓拍车辆编号
        		
        unsigned int            uiStatisBeginTime;
        unsigned int            uiStatisEndTime;
        unsigned int            uiTrafficVolume;
        unsigned int            uiXmlType;

        strLPRect	  lpRect;           //定位出车牌的位置，以便于放大
        char 		  lpStr[12];        //车牌号码
	    unsigned int  LPREnable;        //车牌识别使能
	    unsigned int  lpColor;          //车牌颜色
	    unsigned int  lpType;           //车牌类型
	    unsigned int  lpConfidenct;     //车牌置信度
	    unsigned int  lpFCConfidenct;   //车牌首字符置信度

	    char	      szDirectionId[8];
        char	      szLaneId[8];

        FTP_SNAP_PIC_TIME  strSanpPicTime[6];//按抓拍步骤存放抓拍时间(包含秒和毫秒)

        char		  szPpDevCode[FTP_MAX_DEV_CODE_LEN];

		//人脸检测
		unsigned char	ucFaceIdenEn;
		unsigned char	ucFaceSum;
		unsigned char 	ucFaceOvenlayEn;	//人脸检测是否叠加使能
		unsigned char 	ucSaveSmallPicEn;	/* 是否有小图片 */

		strLPRect		strFaceSearchArea;
		strLPRect		aStrFaceIden[MAX_FACE_NUM];

		STR_FTP_FACE_PIC_INFO aStrFacePicInfo[MAX_FACE_NUM];

		unsigned int	auiBigPicSize[MAX_PIC_NUM];		/* 在有小图片时，原始大图的大小 */

		strCXProInfo 	strFtpCXProInfo;
}JpegSendInfo;

typedef struct
{
    bool            bRealSend;
    int               iBufIndex;
    char            *pcStart1[MAX_PIC_NUM];
    unsigned int    uiLen1[MAX_PIC_NUM];
    char            *pcStart2[MAX_PIC_NUM];
    unsigned int    uiLen2[MAX_PIC_NUM];
    JpegSendInfo    strJpegInfo;
}SendInfo;
typedef struct
{
    char m_host[128];
	int  m_port;
    char m_user[128];
	char m_passwd[128];
    bool m_bFtpEnable;/*FTP 是否使能*/
    bool m_bNasEnable;/* samba 存储是否使能*/
    int  m_iSendFlag;/* xml文件是否需要发送 */
}FtpServerInfo;

/* 摄像机信息结构*/
typedef struct
{
    unsigned int  uiCameraId;
    unsigned int  uiCameraIp;
    unsigned int  uiCameraMac;
    unsigned int  uiCameraType;
}CameraInfo;

typedef struct
{
    int             i330Connect;
    char            szDriveWayId[4];
    char            szDevId[128];
    char            szDirId[16];
    char            szKey[256];
}StrPlatformInfo;

/* 电警违章类型名称及编号配置信息 */
typedef struct STR_FTP_EP_IRREG_INFO
{
	char 			cDispFlag;			/* 违章名称是否叠加到图片上 */
	char			cRsv[3];
	char			szRunRedCode[MAX_IRREG_CODE_LEN];	/* 闯红灯违章代码 */
	char			szRunRedName[MAX_IRREG_NAME_LEN];	/* 闯红灯违章名称 */
	
	char			szReverseRunCode[MAX_IRREG_CODE_LEN];/* 逆行违章代码 */
	char			szReverseRunName[MAX_IRREG_NAME_LEN];/* 逆行违章名称 */

	char			szInfringForbidLineCode[MAX_IRREG_CODE_LEN]; /* 违反禁止标线行驶违章代码 */
	char			szInfringForbidLineName[MAX_IRREG_NAME_LEN];/* 违反禁止标线行驶违章名称 */

	char			szInfringDirLaneRunCode[MAX_IRREG_CODE_LEN]; /* 不按导向车道行驶违章代码 */
	char			szInfringDirLaneRunName[MAX_IRREG_NAME_LEN]; /* 不按导向车道行驶违章名称 */

	char			szStopOnTrafficNoEntryCode[MAX_IRREG_CODE_LEN]; /* 路口遇堵禁止进入违章代码 */
	char			szStopOnTrafficNoEntryName[MAX_IRREG_NAME_LEN]; /* 路口遇堵禁止进入违章描述 */

	char			szHighwayInfringDirLaneCode[MAX_IRREG_CODE_LEN]; /* 高速路不按导向车道行驶违章代码 */
	char			szHighwayInfringDirLaneName[MAX_IRREG_NAME_LEN]; /* 高速路不按导向车道行驶违章名称 */
	
	char			szEpDevCode[FTP_MAX_DEV_CODE_LEN];	
	
	char			szEpSnCode[FTP_MAX_SN_LEN];
}FTP_EP_IRREG_INFO;

/* 卡口违章类型名称及编号配置信息 */
typedef struct STR_FTP_PP_IRREG_INFO
{
	char	szOverSpeedSmallPercent50Code[8];	/* 超速小于50%的违章代码 */
	char	szOverSpeedSmallPercent50Name[64];	/* 超速小于50%的违章名称 */

	char	szOverSpeedLargePercent50Code[8];	/* 超速大于50%的违章代码 */
	char	szOverSpeedLargePercent50Name[64];	/* 超速大于50%的违章名称 */
	
	char	szReverseRunCode[8];				/* 逆行违章代码 */
	char	szReverseRunName[64];				/* 逆行违章名称 */

	char	szInfringeProhibCode[8]; 			/* 机动车违反禁令标志指示的违章代码 */
	char	szInfringeProhibName[64];			/* 机动车违反禁令标志指示的违章名称 */

	char	szPpDevCode[FTP_MAX_DEV_CODE_LEN];
	
	char	szPpSnCode[FTP_MAX_SN_LEN];
}FTP_PP_IRREG_INFO;

#endif
