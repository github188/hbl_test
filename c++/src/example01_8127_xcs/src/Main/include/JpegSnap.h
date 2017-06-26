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

/* DSP�ϱ�����ϸΥ������ */

#define EPPUNISH_DATATYPE_REDLAMP           (21)  /* ����� */

/* ������Υ�������־ָʾ �紳���󡢴�������ʻ��β�����еȡ� */
#define EPPUNISH_DATATYPE_FORBIDLEFT        (22)  /* ���� */
#define EPPUNISH_DATATYPE_FORBIDRIGHT       (23)  /* ���� */
#define EPPUNISH_DATATYPE_FORBIDSTRAIGHT    (24)  /* ��ֱ */

#define EPPUNISH_DATATYPE_FORBID_ODDANDEVEN (30)  /* ��˫������ */

#define SNAP_PUNISHTYPE_REVERSE             (25)  /* ���� */

/* ������Υ����ֹ����ָʾ ��ѹ��ʵ�ߡ�ѹ������ʻ */
#define SNAP_PUNISHTYPE_PRESSSOLIDE 		(26)  /* ѹ�� */
#define SNAP_PUNISHTYPE_CHANGELANE  		(27)  /* ��� */

/*  ͨ��·����ֹͣ�ź�ʱ��ͣ��ֹͣ�����ڻ�·����
 �紳�����Ϊ������ʻ��һ��ֹͣ(�׳� ��ת��) */
#define EPPUNISH_DATATYPE_RED_SHIFT_GREEN  (31)   /* ��ת�� */

/* �����������涨������ʻ */
#define EPPUNISH_DATATYPE_LEFT_SHIFT_DIR   (32) /* ��ת��ֱ�� */
#define EPPUNISH_DATATYPE_LEFT_SHIFT_RIGHT (33) /* ��ת����ת */

#define EPPUNISH_DATATYPE_DIR_SHIFT_LEFT   (34) /* ֱ�б���ת */
#define EPPUNISH_DATATYPE_DIR_SHIFT_RIGHT  (35) /* ֱ�б���ת */

#define EPPUNISH_DATATYPE_RIGHT_SHIFT_DIR  (36) /* ��ת��ֱ�� */
#define EPPUNISH_DATATYPE_RIGHT_SHIFT_LEFT (37) /* ��ת����ת */

#define EPPUNISH_DATATYPE_LEFTDIR_SHIFT_RIGHT (40) /*��תֱ�б���ת*/
#define EPPUNISH_DATATYPE_DIRRIGHT_SHIFT_LEFT (41) /*ֱ����ת����ת*/

/* ������Υ���涨ͣ�š���ʱͣ���Ҽ�ʻ�˲����ֳ���������������������ͨ�е� */
#define EPPUNISH_DATATYPE_TEMPORARY_PARK   (38) /* �̵�ʱ��Ȼͣ����Ӱ�������ͨ�� */

/* ���������ڻ�����������ʻ��  */
#define EPPUNISH_DATATYPE_NO_CARLANE_DRIVER (39)

/* ·�����н�ͨ����ʱδ���εȺ��(���½���) */
#define EPPUNISH_DATATYPE_STOP_ON_TRAFFIC_NO_ENTRY 	  (44)

/* �о�����·�ڲ����涨�г�����ͣ���� */
#define EPPUNISH_DATATYPE_REDLIGHT_PARK 	  (45)

/* ���ٹ�·�ϲ����涨������ʻ�� */
#define HIGHWAY_EPPUNISH_RUN				(46)  /* ���ٲ����涨������ʻ */

/* ����ʵ�ʶ����Υ�����ʹ��� */

/* ���������ڻ�����������ʻ��  */
#define EP_IRRE_CAR_NO_DRIVER_IN_CAR_LANE   (1018)

/* �������������򳵵���ʻ�� */
#define EP_IRRE_NO_RUN_TRACK      (1208)

/*  ͨ��·����ֹͣ�ź�ʱ��ͣ��ֹͣ�����ڻ�·����
 �紳�����Ϊ������ʻ��һ��ֹͣ(�׳� ��ת��) */
#define EP_IRRE_RED_SHIFT_GREEN   (1211)

/*������Υ�������־ָʾ (�紳���󡢴�������ʻ��β������)*/
#define EP_IRRE_FORBID_TRACK      (1229)

/* ·�����н�ͨ����ʱδ���εȺ��(���½���) */
#define EP_SOT_NO_ENTRY      	  (1228)

/* ������Υ����ֹ����ָʾ (��ѹ��ʵ�ߡ�ѹ������ʻ) */
#define EP_IRRE_PRESSSOLIDE	      (1230)

/* ������������ʻ */
#define EP_IRRE_REVERSE_TRACK     (1301)

/* ������������ͨ�źŵƹ涨ͨ�� (�紳�����ʻ) */
#define EP_IRRE_REDLAMP_TRACK     (1302)

/* �����������涨������ʻ (��ռ��ר�ù����������ǻ�������) */
#define EP_IRRE_NO_DIR_TRACK      (1042)

/* ������Υ���涨ͣ�š���ʱͣ���Ҽ�ʻ�˲����ֳ���������������������ͨ�е� */
#define EP_IRRE_TEMPORARY_PARK 	  (1039)

/* ��ʻ�������ڸ��ٹ�·�ϲ����涨������ʻ�� */
#define EP_HIGHWAY_EPPUNISH_RUN	  (4312)

/* �о�����·�ڲ����涨�г�����ͣ���� */
#define EP_PUNISH_DATATYPE_REDLIGHT_PARK (16252)

typedef struct
{
	char cSta;		/* ״̬:N R Y G S */
	char szBT[7];	/* ��״̬��ʼʱ��(HHMMSS) */
}strTrafficLightInfo;

typedef enum
{
    e_cxDiskLightSta =0,     		/* "Բ�̵�״̬" */
    e_cxLeftArrowLightSta,			/* "���ͷ��״̬" */
    e_cxDirArrowLightSta, 			/* "ֱ��ͷ��״̬" */
    e_cxRightArrowLightSta, 		/* "�Ҽ�ͷ��״̬" */
    e_cxTurnAroundLightSta, 		/* "��ͷ��״̬" */
    e_cxLeftArrowAroundLightSta, 	/* "���ͷ��ͷ��״̬" */
}enumTrafficLightSta;

/* ��Ϫ������Ϣ�ռ��ṹ�� */
typedef struct
{
	char cFillLightSta[3];		/* �����״̬ */
	unsigned char ucLimitSpeed;	/* ��·���� */

	char szTrafficLSet[12];		/* �źŵ�������� */
	
	strTrafficLightInfo strAllTrafficLightInfo[6];	/* �����״̬����ʼʱ�� */
	
	char cYellowFlashSta;	/* ������״̬ */
	char cLaneLightSta;		/* ������״̬ */
	char cDriverTrack;		/* ��ʻ�켣:R D L T S W */
	char cDevTemprature;	/*�豸�¶�*/

	char cLaneId;			/* ������ */
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
    unsigned int   m_confidence;//���Ŷ� 
    short m_left/*����*/;
    short m_top;
    short m_right;
    short m_bottom;
    short m_type;/*ָʾ����������Ħ�г�,VL�ⷵ��ֵ,0-����,1-Ħ��*/
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
    unsigned int uiPicAddrOffset;	/* ����ڻ���buf��ַ��ƫ�� */
    unsigned int uiPicLen;			/* ����ͼƬ��С */
}STR_FTP_FACE_PIC_INFO;

/* ˫��Ȧ������Ȧǰ������Ȧ�� */
#define  FTP_VIRTUAL_COIL_TYPE					(0x00)	//������Ȧ
#define  FTP_GROUD_DUBAL_COIL					(0x01)	//�ظ�˫��Ȧ
#define  FTP_GROUD_SINGAL_COIL_FRONT			(0x02)	//�ظе���Ȧǰ
#define  FTP_GROUD_SINGAL_COIL_BEHIND			(0x03)	//�ظе���Ȧ��

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
        char			  cChanDir;				/* ������ʻ���� */
        char			  cFileType;
        /*Start of 2.0.2.0 on 2011-6-9 11:12 its*/
        char			  cAsfSendFalg;
        char              cEpPpSelStep0Or1;
        /*End of 2.0.2.0 on 2011-6-9 11:12 its*/
        char					szPortId[8];
        
        unsigned char			ucSnapType;	//ץ������
		unsigned char			ucCoilDecType;	//�ظ���Ȧ��ⷽʽ
        unsigned short  		usSnapGroupId;	//�ظ���Ȧץ�ĳ������
        		
        unsigned int            uiStatisBeginTime;
        unsigned int            uiStatisEndTime;
        unsigned int            uiTrafficVolume;
        unsigned int            uiXmlType;

        strLPRect	  lpRect;           //��λ�����Ƶ�λ�ã��Ա��ڷŴ�
        char 		  lpStr[12];        //���ƺ���
	    unsigned int  LPREnable;        //����ʶ��ʹ��
	    unsigned int  lpColor;          //������ɫ
	    unsigned int  lpType;           //��������
	    unsigned int  lpConfidenct;     //�������Ŷ�
	    unsigned int  lpFCConfidenct;   //�������ַ����Ŷ�

	    char	      szDirectionId[8];
        char	      szLaneId[8];

        FTP_SNAP_PIC_TIME  strSanpPicTime[6];//��ץ�Ĳ�����ץ��ʱ��(������ͺ���)

        char		  szPpDevCode[FTP_MAX_DEV_CODE_LEN];

		//�������
		unsigned char	ucFaceIdenEn;
		unsigned char	ucFaceSum;
		unsigned char 	ucFaceOvenlayEn;	//��������Ƿ����ʹ��
		unsigned char 	ucSaveSmallPicEn;	/* �Ƿ���СͼƬ */

		strLPRect		strFaceSearchArea;
		strLPRect		aStrFaceIden[MAX_FACE_NUM];

		STR_FTP_FACE_PIC_INFO aStrFacePicInfo[MAX_FACE_NUM];

		unsigned int	auiBigPicSize[MAX_PIC_NUM];		/* ����СͼƬʱ��ԭʼ��ͼ�Ĵ�С */

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
    bool m_bFtpEnable;/*FTP �Ƿ�ʹ��*/
    bool m_bNasEnable;/* samba �洢�Ƿ�ʹ��*/
    int  m_iSendFlag;/* xml�ļ��Ƿ���Ҫ���� */
}FtpServerInfo;

/* �������Ϣ�ṹ*/
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

/* �羯Υ���������Ƽ����������Ϣ */
typedef struct STR_FTP_EP_IRREG_INFO
{
	char 			cDispFlag;			/* Υ�������Ƿ���ӵ�ͼƬ�� */
	char			cRsv[3];
	char			szRunRedCode[MAX_IRREG_CODE_LEN];	/* �����Υ�´��� */
	char			szRunRedName[MAX_IRREG_NAME_LEN];	/* �����Υ������ */
	
	char			szReverseRunCode[MAX_IRREG_CODE_LEN];/* ����Υ�´��� */
	char			szReverseRunName[MAX_IRREG_NAME_LEN];/* ����Υ������ */

	char			szInfringForbidLineCode[MAX_IRREG_CODE_LEN]; /* Υ����ֹ������ʻΥ�´��� */
	char			szInfringForbidLineName[MAX_IRREG_NAME_LEN];/* Υ����ֹ������ʻΥ������ */

	char			szInfringDirLaneRunCode[MAX_IRREG_CODE_LEN]; /* �������򳵵���ʻΥ�´��� */
	char			szInfringDirLaneRunName[MAX_IRREG_NAME_LEN]; /* �������򳵵���ʻΥ������ */

	char			szStopOnTrafficNoEntryCode[MAX_IRREG_CODE_LEN]; /* ·�����½�ֹ����Υ�´��� */
	char			szStopOnTrafficNoEntryName[MAX_IRREG_NAME_LEN]; /* ·�����½�ֹ����Υ������ */

	char			szHighwayInfringDirLaneCode[MAX_IRREG_CODE_LEN]; /* ����·�������򳵵���ʻΥ�´��� */
	char			szHighwayInfringDirLaneName[MAX_IRREG_NAME_LEN]; /* ����·�������򳵵���ʻΥ������ */
	
	char			szEpDevCode[FTP_MAX_DEV_CODE_LEN];	
	
	char			szEpSnCode[FTP_MAX_SN_LEN];
}FTP_EP_IRREG_INFO;

/* ����Υ���������Ƽ����������Ϣ */
typedef struct STR_FTP_PP_IRREG_INFO
{
	char	szOverSpeedSmallPercent50Code[8];	/* ����С��50%��Υ�´��� */
	char	szOverSpeedSmallPercent50Name[64];	/* ����С��50%��Υ������ */

	char	szOverSpeedLargePercent50Code[8];	/* ���ٴ���50%��Υ�´��� */
	char	szOverSpeedLargePercent50Name[64];	/* ���ٴ���50%��Υ������ */
	
	char	szReverseRunCode[8];				/* ����Υ�´��� */
	char	szReverseRunName[64];				/* ����Υ������ */

	char	szInfringeProhibCode[8]; 			/* ������Υ�������־ָʾ��Υ�´��� */
	char	szInfringeProhibName[64];			/* ������Υ�������־ָʾ��Υ������ */

	char	szPpDevCode[FTP_MAX_DEV_CODE_LEN];
	
	char	szPpSnCode[FTP_MAX_SN_LEN];
}FTP_PP_IRREG_INFO;

#endif
