/******************************************************************************
 * protoXcs.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.09, wangbin Create
 * --------------------
 ******************************************************************************/

#ifndef _PROTOCOL_XCS__H__
#define _PROTOCOL_XCS__H__
#include <time.h>

#include "xcsGlobalDef.h"

#define XCS_PROTOCOL_VER    (1)

#define XCS_SYNC_WORD   0x5A5B

/* 1. login protocol  */
#define  CPH_LOG                        0x00        /* ��½Э��ͷ */
/* 1.1 �û���¼ */
#define  CPE_LOG_USER_LOGIN             0x00

/* 1.2 �û��˳� */
#define  CPE_LOG_USER_LOGOUT            0x01

/* 2. video channel protocol  */
#define  CPH_VCHANN                     0x01        /* ��Ƶͨ��Э��ͷ */
/* 2.1 ����ʵʱͼ�� */
#define  CPE_VCHANN_REQ_RTVIDEO         0x00

/* 2.2 �Ͽ�ʵʱͼ�� */
#define  CPE_VCHANN_CUT_RTVIDEO         0x01

/* 2.3 ǰ��ͼƬ�ֶ�ץ�� */
#define  CPE_VCHANN_SNAP_SHOT           0x02

/* 2.4 ���� */

/* 2.5 ץ�Ĳ�����ѯ */
#define  CPE_VCHANN_SNAP_PARAM_QUERY    0x04

/* 2.6 ץ�Ĳ������� */
#define  CPE_VCHANN_SNAP_PARAM_SET      0x05

/* 2.7 ��Ƶ������ѯ */
#define  CPE_VCHANN_VIDEO_PARAM_QUERY   0x06

/* 2.8 ��Ƶ�������� */
#define  CPE_VCHANN_VIDEO_PARAM_CONFIG  0x07

/* 2.9 �����ϱ� */
#define  CPE_VCHANN_DATA_REPORT         0x08
/* 2.10 �޸��ϱ���������*/
#define  CPE_VCHAN_DATA_REPORT_SET      0x09

/* 3.0 �������Э�� */
#define  CPH_DEV_CTL                    0x02        /* �������Э��ͷ */

/* 3.1 �ɼ��豸���Բ�ѯ */
#define  CPE_DEV_CTL_CAP_QUERY          0x00

/* 3.2 �ɼ��豸�������� */
#define  CPE_DEV_CTL_CAP_SET            0x01



/* 4. query protocol  */
#define  CPH_XCS_QUERY                  0x03       /* ��ѯЭ��ͷ */

/* 4.1 ϵͳʱ���ѯ */
#define  CPE_SYS_TIME_QUERY             0x00

/* 4.2 ϵͳʱ������ */
#define  CPE_SYS_TIME_SET               0x01

/* 4.3 ϵͳ�汾��ѯ */
#define  CPE_SYS_RELEASE_QUERY          0x02

/* 4.4 ϵͳ��־��ѯ */
#define  CPE_SYS_LOG_QUERY              0x03

/* 4.5 ϵͳ��־���� */
#define  CPE_SYS_LOG_DOWN               0x04

/* 4.6 Զ������ */
#define  CPE_REMOTE_UPDATE              0x05

/* 4.7 �������ȱ��� */
#define  CPE_UPDATE_PROGRESS            0x06

/* 4.8 Զ������ */
#define  CPE_REMOTE_REBOOT              0x07

/* 4.9 NTP��������ѯ */
#define  CPE_NTP_SERVER_QUERY           0x08

/* 4.10 NTP���������� */
#define  CPE_NTP_SERVER_SET             0x09

/* 4.11 Զ�ָ̻��豸�������� */
#define  CPE_REMOTE_RESTORE_DEV_PARAM   0x0a

/* 4.12 ��ʱ����ʱ���ѯ */
#define  CPE_RESTART_TIME_QUERY         0x0b

/* 4.13 ��ʱ����ʱ������ */
#define  CPE_RESTART_TIME_SET           0x0c

/* 5.Bayonet protocol */
#define CPH_BAYONET                     0x04        /* �������Э��ͷ */

/* 5.1 ����ȫ����Ϣ��ѯ */
#define CPE_BAYONET_GLOBAL_INFO_QUERY   0x00

/* 5.2 ����ȫ����Ϣ���� */
#define CPE_BAYONET_GLOBAL_INFO_SET     0x01

/* 5.3 ���ڳ�����Ϣ��ѯ */
#define CPE_BAYONET_CAR_CHANNEL_INFO_QUERY  0x02

/* 5.4 ���ڳ�����Ϣ���� */
#define CPE_BAYONET_CAR_CHANNEL_INFO_SET    0x03

/* 5.5 ����������Ȧ�߼���Ϣ��ѯ */
#define CPE_BAYONET_VIRTUAL_LOOP_INFO_QUERY 0x04

/* 5.6 ����������Ȧ�߼���Ϣ���� */
#define CPE_BAYONET_VIRTUAL_LOOP_INFO_SET   0x05

/* 5.7 �����ֶ�ץ�� */
#define CPE_BAYONET_HAND_SNAP               0x06

/* 5.8 ���ڱ��� */
#define CPE_BAYONET_RESERVE                 0x07

/* 5.9 ����FTP��Ϣ��ѯ */
#define CPE_BAYONET_FTP_INFO_QUERY          0x08

/* 5.10 ����FTP��Ϣ���� */
#define CPE_BAYONET_FTP_INFO_SET            0x09

/* 5.11 ����ͼƬ��Ļ������Ϣ��ѯ */
#define CPE_BAYONET_JPG_OSD_CONF_QUERY      0x0a

/* 5.12 ����ͼƬ��Ļ������Ϣ���� */
#define CPE_BAYONET_JPG_OSD_CONF_SET        0x0b

/* 5.13 ����Υ�����������������ѯ */
#define CPE_BAYONET_IRREG_INFO_QUERY		0x0c

/* 5.14 ����Υ�������������������� */
#define CPE_BAYONET_IRREG_INFO_SET			0x0d

/* 5.15 �����ÿ���FTP��Ϣ��ѯ */
#define CPE_BAYONET_BACKUPS_FTP_INFO_QUERY  0x0e

/* 5.16 �����ÿ���FTP��Ϣ���� */
#define CPE_BAYONET_BACKUPS_FTP_INFO_SET    0x0f


/* 6. electronic police protocol */
#define CPH_ELECT_POLICE                        0x05        /* ���Ӿ������Э��ͷ */

/* 6.1 ���Ӿ���ȫ����Ϣ��ѯ */
#define CPE_ELECT_POLICE_GLOBAL_INFO_QUERY      0x00

/* 6.2 ���Ӿ���ȫ����Ϣ���� */
#define CPE_ELECT_POLICE_GLOBAL_INFO_SET        0x01

/* 6.3 ���Ӿ��쳵����Ϣ��ѯ */
#define CPE_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY 0x02

/* 6.4 ���Ӿ��쳵����Ϣ���� */
#define CPE_ELECT_POLICE_CAR_CHANNEL_INFO_SET   0x03

/* 6.5 ���Ӿ�����״̬��ѯ */
#define CPE_ELECT_POLICE_RED_LIGHT_STA_QUERY    0x04

/* 6.6 ģ�⳵����ʻץ�� */
#define CPE_ELECT_POLICE_SIMU_RUN_SNAP          0x05

/* 6.7 ǿ�ƺ�Ʋ�ѯ */
#define CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY 0x06

/* 6.8 ǿ�ƺ������ */
#define CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_SET   0x07

/* 6.9 ���Ӿ���FTP��Ϣ��ѯ */
#define CPE_ELECT_POLICE_FTP_INFO_QUERY          0x08

/* 6.10 ���Ӿ���FTP��Ϣ���� */
#define CPE_ELECT_POLICE_FTP_INFO_SET            0x09

/* 6.11 ���Ӿ����ֹ��ʻʱ��β�ѯ */
#define CPE_ELECT_POLICE_TIME_RANGE_QUERY        0x0a

/* 6.12 ���Ӿ����ֹ��ʻʱ������� */
#define CPE_ELECT_POLICE_TIME_RANGE_SET          0x0b


/* 6.13 ���Ӿ��츨����Ʋ�ѯ */
#define CPE_EP_ASSIST_RED_LIGHT_QUERY        	 0x0c

/* 6.14 ���Ӿ��츨��������� */
#define CPE_EP_ASSIST_RED_LIGHT_SET		         0x0d

/* 6.15 �羯����ʶ����Ϣ��ѯ */
#define CPE_ELECT_POLICE_LPR_QUERY				 0x0e

/* 6.16 �羯����ʶ����Ϣ���� */
#define CPE_ELECT_POLICE_LPR_SET				 0x0f

/* 6.17 �羯Υ�����������������ѯ */
#define CPE_ELECT_POLICE_IRREG_INFO_QUERY		 0x10

/* 6.18 �羯Υ�������������������� */
#define CPE_ELECT_POLICE_IRREG_INFO_SET			 0x11

/* 6.19 ���Ӿ������½����ѯ */
#define CPE_EP_SOT_NO_ENTRY_QUERY        	 	 0x12

/* 6.20 ���Ӿ������½������� */
#define CPE_EP_SOT_NO_ENTRY_SET			         0x13

/* 6.21 �羯ͼƬ��Ļ������Ϣ��ѯ */
#define CPE_EP_JPG_OSD_CONF_QUERY   			 0x14

/* 6.22 �羯ͼƬ��Ļ������Ϣ���� */
#define CPE_EP_JPG_OSD_CONF_SET        			 0x15

/* 6.23 �����ÿ���FTP��Ϣ��ѯ */
#define CPE_EP_BACKUPS_FTP_INFO_QUERY  			 0x16

/* 6.24 �����ÿ���FTP��Ϣ���� */
#define CPE_EP_BACKUPS_FTP_INFO_SET    			 0x17

/* 6.25 �羯����Υ�����������������ѯ */
#define CPE_ELECT_POLICE_NEWADD_IRREG_INFO_QUERY 0x18

/* 6.26 �羯����Υ�������������������� */
#define CPE_ELECT_POLICE_NEWADD_IRREG_INFO_SET	 0x19

/* 7. record protocol */
#define CPH_XCS_RECORD                      0x06

/* 7.1 ������Ϣ��ѯ */
#define CPE_RECORD_DISK_INFO_QUERY          0x00

/* 7.2 ���� */

/* 7.3 ¼�����ء��طŷ�ʽ��ѯ */
#define CPE_RECORD_DOWNLOAD_PLAYTYPE_QUERY  0x02

/* 7.4 ¼�����ػطŷ�ʽ���� */
#define CPE_RECORD_DOWNLOAD_PLAYTYPE_SET    0x03

/* 7.5 ������¼��ƻ���ѯ */
#define CPE_RECORD_PERIOD_PLAN_QUERY        0x04

/* 7.6 ������¼��ƻ����� */
#define CPE_RECORD_PERIOD_PLAN_ADD          0x05

/* 7.7 ������¼��ƻ�ɾ�� */
#define CPE_RECORD_PERIOD_PLAN_DEL          0x06

/* 7.8 ������¼��ƻ����� */
#define CPE_RECORD_PERIOD_PLAN_UPDATE       0x07

/* 7.9 ͨ��¼�������ѯ */
#define CPE_RECORD_CHANNEL_PARAM_QUERY      0x08

/* 7.10 ͨ��¼��������� */
#define CPE_RECORD_CHANNEL_PARAM_SET        0x09

/* 7.11 ����¼���¼ */
#define CPE_XCS_RECORD_SEARCH               0x0A

/* 7.12 ¼���ļ��ط� */
#define CPE_XCS_RECORD_PLAYBACK             0x0B

/* 7.13 ¼���ļ����� */
#define CPE_RECORD_FILE_DOWNLOAD            0x0C

/* 8. CAMARA info protocol */
#define CPH_CAMARA                          0x07

/* 8.1 ������������Բ�ѯ */
#define CPE_CAMARA_ATTRIB_QUERY             0x00

/* 8.2 ���������������Χ��ѯ */
#define CPE_CAMARA_PARAM_QUERY              0x01

/* 8.3 �����״̬������ѯ */
#define CPE_CAMARA_STA_INFO_QUERY           0x02

/* 8.4 ���� */

/* 8.5 ������������ݲ�ѯ */
#define CPE_CAMARA_CTL_INFO_QUERY           0x04

/* 8.6 ����������������� */
#define CPE_CAMARA_CTL_INFO_SET             0x05

/* 8.7 �����ʱ��β�ѯ */
#define CPE_CAMARA_TIME_RANGE_QUERY         0x06

/* 8.8 �����ʱ������� */
#define CPE_CAMARA_TIME_RANGE_SET           0x07

/* 8.9 �����Ĭ�����ò�����ѯ */
#define CPE_CAMARA_DEFAULT_CONF_QUERY       0x08


/* 9. net info protocol */
#define CPH_NET_INFO                        0x08

/* 9.1 ������Ϣ��ѯ */
#define CPE_NET_INFO_QUERY                  0x00

/* 9.2 ������Ϣ���� */
#define CPE_NET_INFO_SET                    0x01

/* 9.3 ͼƬ����洢��Ϣ��ѯ */
#define CPE_NET_PIC_NET_SAVE_INFO_QUERY     0x02

/* 9.4 ͼƬ����Ӳ����Ϣ���� */
#define CPE_NET_PIC_NET_SAVE_INFO_SET       0x03

/* 9.5 ¼������洢��Ϣ��ѯ */
#define CPE_NET_RECORD_NET_SAVE_INFO_QUERY  0x04

/* 9.6 ¼������Ӳ����Ϣ���� */
#define CPE_NET_RECORD_NET_SAVE_INFO_SET    0x05

/* 9.7 �ƴ洢NRM��Ϣ��ѯ */
#define CPE_NET_RECORD_MANAGE_INFO_QUERY	0x06

/* 9.8 �ƴ洢NRM��Ϣ���� */
#define CPE_NET_RECORD_MANAGE_INFO_SET	    0x07

/* 10. dev connect protocol */
#define CPH_DEV_CONNECT_PRO                 0x09

/* 10.1 330ƽ̨������Ϣ��ѯ */
#define CPE_330_PLATFORM_CON_QUERY          0x00

/* 10.2 330ƽ̨������Ϣ���� */
#define CPE_330_PLATFORM_CON_SET            0x01


/* 10.3 v3ƽ̨������Ϣ��ѯ */
#define CPE_V3_PLATFORM_CON_QUERY           0x02

/* 10.4 v3ƽ̨������Ϣ���� */
#define CPE_V3_PLATFORM_CON_SET             0x03

/* 10.5 �羯330ƽ̨������Ϣ��ѯ */
#define CPE_EP_330_PLATFORM_CON_QUERY       0x04

/* 10.6 �羯330ƽ̨������Ϣ���� */
#define CPE_EP_330_PLATFORM_CON_SET         0x05

/* 10.7 v2ƽ̨������Ϣ��ѯ */
#define CPE_V2_PLATFORM_CON_QUERY           0x06

/* 10.8 v2ƽ̨������Ϣ���� */
#define CPE_V2_PLATFORM_CON_SET             0x07

/* 10.9 Gb28181ƽ̨������Ϣ��ѯ */
#define CPE_GB28181_PLATFORM_CON_QUERY      0x08

/* 10.10 Gb28181ƽ̨������Ϣ���� */
#define CPE_GB28181_PLATFORM_CON_SET        0x09


/* 11.0 heartbeat */
#define CPH_HEART_BEAT						0x0a

/* 12.0 netManage Info */
#define CPH_NET_MANAGE_PRO					0x0b

/* 12.1 ������Ϣ��ѯ */
#define CPE_NET_MANAGE_QUERY          		0x00



/* 13. ack protocol  */
#define  CPH_ACK                    0x7A       /* ACKЭ��ͷ */
#define  CPH_NAK                    0x7C       /* NAKЭ��ͷ */

/***********************************************************************************/
/* data structure */
/***********************************************************************************/
#define _PACKED_ __attribute__((packed))



/* ����Э��������ڷ�Χ�Ķ��壬���ô˽ṹ */
typedef struct tagXCSStrArea
{
    unsigned short usAreaPosX;      /* �������Ͻ�����X */
    unsigned short usAreaPosY;      /* �������Ͻ�����Y */
    unsigned short usAreaWidth;     /* ������ */
    unsigned short usAreaHeight;    /* ����߶� */
}_PACKED_ STR_XCS_AREA, *PSTR_XCS_AREA;

typedef struct tagStrPacketHeader
{
	unsigned int	SyncWord:16;
	unsigned int 	Version:4;
	unsigned int	IsAckOrNot:4;
	unsigned int	DataType:8;
	unsigned int	SubDataType:8;
    unsigned int    resv:24;
	unsigned char	ucErrorCode;
    unsigned char   ucResv2[3];
	unsigned int	uiPayloadLength;
}_PACKED_ STR_PACKET_HEADER, *PSTR_PACKET_HEADER;

/* 1.1 �û���¼��ֻ֧�ֵ��û���¼ */
typedef struct tagStrUserLogin
{
	char cUserName[32];		    //�û�����
	char cUserPasswd[32];		//�û�����
	unsigned int uiDecConfVer;  //ָ��ƥ��ͻ��˰汾
	char cResv[124];
}_PACKED_ STR_USER_LOGIN , *PSTR_USER_LOGIN;

typedef struct tagStrUserLoginRep
{
	unsigned int uiDecConfVer;  //ָ��ƥ��ͻ��˰汾
	char cResv[128];
}_PACKED_ STR_USER_LOGIN_REP , *PSTR_USER_LOGIN_REP;

/* 2. video channel protocol  */
/* 2.1 ����ʵʱͼ�� */
typedef struct tagStrRealVQuery
{
    unsigned char   ucChannel;
    char            cVideoConnMode;
    unsigned char   ucConnectOK;    // ���ӳɹ����
						    		//=1���ɹ����
							        //=0�����ʧ�ܣ�ԭ���bReason
	unsigned char   ucConReason;    // ������Ӳ��ɹ���ָ����ԭ��:
                                    //��ͨ��Ŀǰ������			0x01
									//��ͨ��Ŀǰ����Զ�̻ط�	0x02
									//��ͨ��Ŀǰ���ڱ��ػط�	0x04
									//��ͨ��Ŀǰ����Զ�̱���	0X08
									//ͼ��������>=4				0x80
									//���緢�Ͳ�������			0x81
									//��Ƶ��ʧ					0x82
									//ͨ������					0x83
									//��ͨ�����ڵ������		0x84
									//��ͨ����ǰ��Ч����		0x85
    unsigned int    ReceiveIpAddr;
    unsigned short  ReceivePort;
    char            bActive;      
    char            bStream;        
    unsigned int    nPackLen;
}_PACKED_ STR_REAL_VIDEO_QUERY, *PSTR_REAL_VIDEO_QUERY;

typedef struct tagStrRealVQuery STR_REAL_VIDEO_RESULT, *PSTR_REAL_VIDEO_RESULT;

/* 2.2 �Ͽ�ʵʱͼ�� */

typedef struct tagStrRealVQuery STR_REAL_VIDEO_CUT, *PSTR_REAL_VIDEO_STOP;

/* 2.3 ǰ��ͼƬ�ֶ�ץ�� */
typedef struct tagStrVChannelSnapShop
{
    char    cVChannel;          /* ��Ƶͨ���� ��1��ʼ */
    char    cRev[131];
}_PACKED_ STR_VCHANN_SNAP_SHOT, *PSTR_VCHANN_SNAP_SHOT;

/* 2.5 ץ�Ĳ�����ѯ */
typedef struct tagStrVChannelSnapParamQuery
{
    char    cPicQuality;        /* ͼ�������ȼ���1~5 */
    char    cRev[131];
}_PACKED_ STR_VCHANN_SNAP_PARAM_QUERY, *PSTR_VCHANN_SNAP_PARAM_QUERY;

/* 2.6 ץ�Ĳ������� */
typedef STR_VCHANN_SNAP_PARAM_QUERY STR_VCHANN_SNAP_PARAM_SET;

/* 2.7 ��Ƶ������ѯ */
typedef struct tagStrVChannelVParamQuery
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
    char            szContent[64];     /* ��Ļ���� */

	unsigned short  usDebugFrameX;
	unsigned short  usDebugFrameY;

	unsigned char   ucDebugFrameEn;
    unsigned char   ucTimeDisMode;		/* ʱ����ʾ��ʽ 0:�޺��� 1:�к��� */
	unsigned short  usTimeXPos;			/* ʱ����ʾ���� X */

	unsigned short  usTimeYPos;			/* ʱ����ʾ���� Y */
	
    char            cRev[118];
}_PACKED_ STR_VCHANN_VIDEO_PARAM_QUERY, *PSTR_VCHANN_VIDEO_PARAM_QUERY;

/* 2.8 ��Ƶ�������� */
typedef STR_VCHANN_VIDEO_PARAM_QUERY STR_VCHANN_VIDEO_PARAM_SET;

#if 0
/* 2.9 �����ϱ� */
/* ePassPortSnap��ParamData���ݽṹ */
typedef struct tagStrPassPortSnapPic
{
    unsigned int    uiSubVer;
    char            cCarChannel;
    char            cCarType;
    char            cRev1[1];
    char            cCarDir;
    unsigned short  usCarLen;
    unsigned short  usCarSpeed;
    char            cInfringeSta;       /* �Ƿ�Υ�� */
    char            cInfringeTyte;      /* Υ������ */
    STR_AREA        strPlateNumArea;    /* ���������� */
    char            cConfident;         /* ���Ŷ� */
    char            cRev[1];
}_PACKED_ STR_PASS_PORT_SNAP_PIC, *PSTR_PASS_PORT_SNAP_PIC;

/* EElectronicPoliceSnap��ParamData���ݽṹ */
typedef struct tagStrElectPoliceSnapPic
{
    unsigned int    uiSubVer;
    char            cCarChannel;
    char            cRev1[2];
    char            cConfident;         /* ���Ŷ� */
    STR_AREA        strPlateNumArea;    /* ���������� */
    char            cRev[1];
}_PACKED_ STR_ELECT_POLICE_SNAP_PIC, *PSTR_ELECT_POLICE_SNAP_PIC;

/* ��������ΪEManualSnap,ePassPortSnap��EElectronicPoliceSnapʱ��Ϣ�������ݽṹ���� */
typedef struct tagStrDataReportConnect
{
    unsigned int    dwParamLen;
    char            *pbParamData;
    char            bPicNum;
    char            bRev1[3];
    unsigned int    *pPicLen;           /* ÿ��ͼƬ���ȣ������ͼƬ������̬�����û��峤��:4*bPicNum */
    unsigned int    *pPicData;          /* ͼƬʵ�����ݣ�4�ֽڶ��� */
}_PACKED_ STR_DATA_REPORT_CONTENT, *PSTR_DATA_REPORT_CONTENT;

/* ��������ΪEComCaptureInfo, EComOutputInfoʱ��Ϣ�������ݽṹ���� */
typedef struct tagStrDataReportContent
{
    unsigned int   uiSubVer;            /* �汾�� */
    unsigned int   uiDATALen;           /* ���ݳ��� */
    unsigned int   *pDataContent;       /* �������� */
}_PACKED_ STR_DATA_REPORT_CONTENT, *PSTR_DATA_REPORT_CONTENT;

/* �����ϱ����ݽṹ���� */
typedef struct tagStrVChannelDataReport
{
    unsigned int    uiVersion;          /* �汾�� */
    unsigned int    uiDataType;
    time_t          tTime;              /* ֪ͨ��Ϣ����ʱ�� */
    unsigned int    *pData;             /* �ϱ��������� */
}_PACKED_ STR_VCHANN_DATA_REPORT, *PSTR_VCHANN_DATA_REPORT;
#endif
/* 2.10  �����ϱ������޸� */

typedef struct tagStrDataReportSet
{
    unsigned int   uiRecvIp;
    unsigned short usRecvPort;
    unsigned short usConnectType;
    unsigned int   uiRecvType;
    unsigned char  resv[128];
}_PACKED_ STR_DATA_REPORT_SET;


/* 3.0 �������Э�� */

/* 3.1 �ɼ��豸���Բ�ѯ */
typedef struct tagStrDevCtlCapQuery
{
    char            cEnable;            /* ʹ��λ */
    char            cComId;             /* ���ں�id */
    char            cDataBits;          /* ����λ */
    char            cStopBits;          /* ֹͣλ */
    char            cParity;            /* ��żУ��λ */
    char            cRev1[3];
    unsigned int    uiBaudrate;         /* ������ */
    unsigned int    uiVehicleType;      /* ���������� */
    unsigned short  usVehHighSpeed;     /* ����������ܲ��ٶ� */
    char            cRev2[126];
}_PACKED_ STR_DEV_CTL_CAP_QUERY, *PSTR_DEV_CTL_CAP_QUERY;

/* 3.2 �ɼ��豸�������� */
typedef struct tagStrDevCtlCapQuery STR_DEV_CTL_CAP_SET;

/* 4. query protocol  */

/* 4.1 ϵͳʱ���ѯ */
typedef struct tagStrSysTimeQuery
{
    unsigned int    uiSysTime;          /* ϵͳʱ�䣬����Ϊ��λ */
    char            cRev[128];
}_PACKED_ STR_SYS_TIME_QUERY, *PSTR_SYS_TIME_QUERY;

/* 4.2 ϵͳʱ������ */
typedef STR_SYS_TIME_QUERY STR_SYS_TIME_SET;

/* 4.3 ϵͳ�汾��ѯ */
typedef struct tagStrSysReleaseQuery
{
    unsigned int    uiProcType;         /* ��Ʒ�ͺ� */
    char            cSerialNum[16];     /* ���к� */
    unsigned int    uiHardwareVer;      /* Ӳ���汾�� */
    unsigned int    uiSoftwareVer;      /* ����汾�� */
    unsigned int    uiProtocolVer;      /* Э��汾�� */
    char			szcMac[18];			/* MAC��ַ */
    char            cRev[128];
}_PACKED_ STR_SYS_RELEASE_QUERY, *PSTR_SYS_RELEASE_QUERY;

/* 4.4 ϵͳ��־��ѯ */
typedef struct tagStrSysLogPcQuery
{
    unsigned int    uiLogType;
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    char            cRev[128];
}_PACKED_ STR_SYS_LOG_PC_QUERY, *PSTR_SYS_LOG_PC_QUERY;

typedef struct tagStrSysLogQueryAck
{
    unsigned int    uiLogType;
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    unsigned int    uiLogLen;
    char            *pbLogData;
}_PACKED_ STR_SYS_LOG_QUERY_ACK, *PSTR_SYS_LOG_QUERY_ACK;

/* 4.5 ϵͳ��־���� */
typedef struct tagStrSysLogPcDown
{
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    char            cRev[128];
}_PACKED_ STR_SYS_LOG_PC_DOWN, *PSTR_SYS_LOG_PC_DOWN;

typedef struct tagStrSysLogDownAck
{
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    unsigned int    uiLogLen;
    char            *pbLogData;
}_PACKED_ STR_SYS_LOG_DOWN_ACK, *PSTR_SYS_LOG_DOWN_ACK;

/* 4.6 Զ������ */
typedef struct tagStrRemoteUpdate
{
    unsigned char   ucURL[256];
    unsigned char   ucRcv[128];
}_PACKED_ STR_REMOTE_UPDATE, *PSTR_REMOTE_UPDATE;

/* 4.7 �������ȱ��� */
typedef struct tagStrUpdateProgress
{
    unsigned int    uiUpdateStep;       /* ��������1:���������� 2:���� 3:д�� */
    unsigned int    uiPercentage;       /* �ٷֱȣ���ʾ������д���ʱ�� */
    char            cRev[128];
}_PACKED_ STR_UPDATE_PROGRESS, *PSTR_UPDATE_PROGRESS;


/* 4.9 NTP��������ѯ */
typedef struct tagStrNtpServerQuery
{
    unsigned int    uiEnable;
    unsigned int    uiServerIp;
    unsigned short  usJudgeCycle;		/* ��ʱ����(��λ:����) */
	unsigned char   ucJudegPrecision;	/* ��ʱ����(��λ:��) */
    char            cRev[125];
}_PACKED_ STR_NTP_SERVER_QUERY, *PSTR_NTP_SERVER_QUERY;

/* 4.10 NTP���������� */
typedef STR_NTP_SERVER_QUERY STR_NTP_SERVER_SET;

/* 4.11 ��ʱ����ʱ���ѯ */
typedef struct tagStrRestartTimeQuery
{
    unsigned int    uiRestartTime;          /* ��ʱ����ʱ�䣬����Ϊ��λ */
    char			cEnFlag;				/* ��ʱ����ʹ�ܱ�־ */
    char            cRev[127];
}_PACKED_ STR_RESTART_TIME_QUERY, *PSTR_RESTART_TIME_QUERY;

/* 4.12 ��ʱ����ʱ������ */
typedef STR_RESTART_TIME_QUERY STR_RESTART_TIME_SET;

/* 5.Bayonet protocol */

/* 5.1 ����ȫ����Ϣ��ѯ */
typedef struct tagStrBayonetGlobalInfoQuery
{
    char    szRoadName[32];
    char    szDirName[32];
    char	szPassPortId[8];	 	 /* ����ID */
	char	cOSDColor;				 /* ͼƬ����OSD��ɫ */
	char    cValidLanes;			 /* ʵ��ʹ�ó����� */
	char    cVLWorkMode;			 /* ������Ȧ����ģʽ */

	unsigned char ucLPREn;			 /* ����ʶ��ʹ�ܱ�־ */
	char    szLPRFirstChar[32];		 /* �������ַ� */
	unsigned char ucLPRCConfidence;	 /* �������ַ����Ŷ� */
	unsigned char ucVSpeedMeaFlag;	 /* ��Ƶ����ʹ�ܱ�־ */

	unsigned short usVSpeedMeaLen;	 /* ��Ƶ����ʵ�ʾ���(��λ:����) */

	unsigned char aucFreqFlashEn[4];  /* �Ƿ�ʹ��Ƶ������ */
    unsigned char aucFlashTrigMode[4];/* ����ƴ�����ʽ */
    unsigned char aucStrobeFlashFre[4];/* Ƶ���Ʊ�Ƶϵ�� */
    char     szPpDevCode[MAX_DEV_CODE_LEN];/* �����豸��� */
    char     szDirId[8];                 /* ����ID */
    
    unsigned char   ucFillInLSwitchEn;    /* �����л�ʹ�ܱ�־ */
    unsigned char   ucGrayThresholdVal;    /* �л��Ҷȷ�ֵ */
    unsigned char    ucCheckSwitchCycle;    /* �����Ƶ�Ҷ�ֵ���� */
    
    char            cRsv;
    char            szExPpDevCode[EXT_DEV_CODE_LEN];/* ��չ�豸����ַ��� */

	unsigned char	ucSmartSwitchEn;		/* ���������л�ʹ�� */
	unsigned char	ucTimerSwitchEn;		/* ���ⶨʱ�л�ʹ�� */
	char			cRsv2[2];
	unsigned int	uiFillInLightBeginTime;	/* ������ʼʱ�� */
	unsigned int	uiFillInLightLastTime;	/* ����ʱ�� */    


    char     cRev[24];
}_PACKED_ STR_BAYONET_GLOBAL_INFO_QUERY, *PSTR_BAYONET_GLOBAL_INFO_QUERY;

/* 5.2 ����ȫ����Ϣ���� */
typedef struct tagStrBayonetGlobalInfoQuery STR_BAYONET_GLOBAL_INFO_SET;


/* 5.3 ���ڳ�����Ϣ��ѯ */
typedef struct tagStrBayonetCarChannelInfoPcQuery
{
    unsigned int   uiCarRoadId;     /* ��ѯ������ID */
}_PACKED_ STR_BAYONET_CAR_CHANNEL_INFO_PC_QUERY, *PSTR_BAYONET_CAR_CHANNEL_INFO_PC_QUERY;

/* nCoilDesp COM ����ʱ�Ľṹ���� */
typedef struct tagStrCoilComDesp
{
    char    cComId;
    char    cBoardId;               /* �忨�� */
    char    cFrontCoilId;           /* ������ǰ��Ȧ */
    char    cBehindCoilId;          /* ����������Ȧ */
    char    cGroupId;               /* �ظ���Ȧ��ţ���Ӧ�ƿ������������� */
    char    cRev[13];
}_PACKED_ STR_COIL_COM_DESP, *PSTR_COIL_COM_DESP;

/* nCoilDesp GPIO ����ʱ�Ľṹ���� */
typedef struct tagSTRCOILGPIODESP
{
    char    cFrontCoilId;           /* ������ǰ��Ȧ */
    char    cBehindCoilId;          /* ����������Ȧ */
    char    cRev[16];
}_PACKED_ STR_COIL_GPIO_DESP, *PSTR_COIL_GPIO_DESP;

typedef union tagSTR_COIL_DESP
{
    STR_COIL_COM_DESP strComCoilDesp;
    STR_COIL_GPIO_DESP strGpioCoilDesp;
}_PACKED_ STR_COIL_DESP, *PSTR_COIL_DESP;

typedef struct tagStrXcsTimeSlotSnapObj
{
    unsigned int uiBeginTimeSecs;	/* ��ʼʱ�� */
    unsigned int uiLastTimeSecs;	/* ����ʱ�� */
    unsigned char ucSnapObj;		/* ץ��Ŀ�� */
    unsigned char ucRsv[3];
}_PACKED_ STR_XCS_TIME_SLOT_SNAP_OBJ, *PSTR_XCS_TIME_SLOT_SNAP_OBJ;

typedef struct tagSTR_BAYONET_CAR_CHANNEL_INFO_QUERY
{
    unsigned int    uiVersion;
    unsigned int    uiCarRoadId;
    unsigned short  usOSDPosX;              /* ץ��ͼƬ�ϵ�OSD��ʾx���� */
    unsigned short  usOSDPosY;              /* ץ��ͼƬ�ϵ�OSD��ʾy���� */
    char            szOSDContent[32];       /* ץ��ͼƬ�ϵ�OSD���� */
    STR_XCS_AREA    strRoadArea;            /* ���������� */
    unsigned short  usFlashLightType;       /* ����ƻ򲹹������ */
    unsigned short  usFlashLightCtlType;    /* ����ƻ򲹹�ƿ��Ʒ�ʽ */
    char            cFlashComId;            /* ����ƿ��ƴ��ں� */
    char            cLightDevType;          /* ������豸���� */
    char            cLightCtlMode;          /* ����ƿ���ģʽ */
	/*Start of ITS on 2011-5-3 11:3 2.0.0.1*/
	char			cSingalCoilPos;			/* ����Ȧλ�ö��� */
	/*End of ITS on 2011-5-3 11:3 2.0.0.1*/
    unsigned short  ausFlashAddr[4];        /*  �����485��ַ�����߿��ƶ˿ڣ�ǰ�����Ч��
                                                ��ʹ��ʱ��Ϊ-1������Ƶ�ַ��������豸�Լ�������
                                                GPIO���ƶ˿ںŴ�1��ʼ��*/
    unsigned short  usDetectType;           /* �������:�����ظ���Ȧ */
    
	/*Start of ITS on 2011-5-3 11:5 2.0.0.1*/
	char			cGroudCoilDecType;		/* �ظ���Ȧ�������:˫��Ȧ����Ȧ */
	char			cSingalCoilSnapType;	/* ����Ȧץ������:����Ȧ�����Ȧץ�� */
	/*End of ITS on 2011-5-3 11:5 2.0.0.1*/

    STR_XCS_AREA    strVLArea;              /* ������Ȧ������ */
    unsigned short  usVehicleLinkMode;      /* ���������ӷ�ʽ */
    STR_COIL_DESP   strCoilDesp;            /* �ظ���Ȧ���� */
    
	char			cVLSnapDir;				/* ������Ȧץ�ķ��� */
	char 			cVLDecEn;				/* ������Ȧ���ʹ�� */
	unsigned short  usSingalCoilHigh;		/* ����Ȧ�µ�ǰ��Ȧ�߶� */
	unsigned char   ucDaySnapObj;			/* ץ��Ŀ�����ͣ�1:ȫ���ͣ�2:������;3:�ǻ����� */
	unsigned char   ucNightSnapObj;			/* ץ��Ŀ�����ͣ�1:ȫ����(�貹��)��2:������;3:�ǻ����� */	

	unsigned char   ucRsv1[4];    
	unsigned char   ucSnapPicNum;			/* ץ��ͼƬ�������� */
	unsigned short	usSnapPicInter;			/* ����ͼƬץ�ļ��ʱ��(��λ:����) */

    STR_XCS_TIME_SLOT_SNAP_OBJ strTimeSlotSnapObj;/* ʱ���ץ��Ŀ������ */
    
	char			cCoilSwitchSta;		 	/* ��Ȧ���ģʽ�л�״̬ */
   	char			cInLightSta;	     	/* ���ⷽʽ״̬ */
    char			cCoilSwitchFlag;		/* ��Ȧ�л�ʹ�ܱ�־ */

    unsigned short  usIrreSpeed;            /* Υ���ٶ����� */
    unsigned short  usMaxSpeed;             /* ����������� */
    unsigned short  usMinSpeed;             /* ������С���� */
    unsigned short  usCoilDistance;         /* ǰ��ظ���Ȧ����(����Ϊ��λ) */
    char			cVLDispFlag;    		/* ������Ȧ�Ƿ���ʾ��־ */
    char			cLaneDispFlag;    		/* ������Χ�Ƿ���ʾ��־ */
    unsigned short  usPPProtoVer;			/* ���ڳ�����ϢЭ��С�汾�� */
	char			cRsv5[4];				/* �����ֽ� */

    unsigned char	ucInLightEn;      		/* �Ƿ�ʹ�ܸ������� */
    char			szLaneId[8];			/* ����id */

    char			cSnapDir;				/* ץ�ķ���(0:����;1:����) */
	char			cSDEnFlag;				/* �Ƿ�ʹ�� */
	char			cTruckEnFlag;			/* ������Ƿ�ץ��ʹ�� */
	char 			cRsv[1];
	unsigned int 	iSDBeginTime1;
	unsigned int 	iSDLastTime1; 			/* ����ʱ��1 */
	unsigned int 	iSDBeginTime2;
	unsigned int 	iSDLastTime2; 			/* ����ʱ��2 */	
	unsigned int 	iSDBeginTime3;
	unsigned int 	iSDLastTime3; 			/* ����ʱ��3 */
	
    char            cRev3[83];
}_PACKED_ STR_BAYONET_CAR_CHANNEL_INFO_QUERY, *PSTR_BAYONET_CAR_CHANNEL_INFO_QUERY;


/* 5.4 ���ڳ�����Ϣ���� */
typedef struct tagSTR_BAYONET_CAR_CHANNEL_INFO_QUERY STR_BAYONET_CAR_CHANNEL_INFO_SET;

/* 5.5 ����������Ȧ�߼���Ϣ��ѯ */
typedef struct tagSTR_BAYONET_VL_INFO_PC_QUERY
{
    unsigned int   uiCarRoadId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_BAYONET_VL_INFO_PC_QUERY, *PSTR_BAYONET_VL_INFO_PC_QUERY;

typedef struct tagSTR_BAYONET_VL_INFO
{   
    unsigned int    uiVersion;
    unsigned int    uiCarRoadId;            /* ������id */
    unsigned short  usThrDayToNight;        /* ����תΪҹ��Ĳο����� */
    unsigned short  usThrNightToDay;        /* ҹ��תΪ����Ĳο����� */
    unsigned int    uiCorrNightThresh;      /* ҹ�����ֵ���� */
    unsigned int    uiCorrDayThresh;        /* �������ֵ���� */
    unsigned int    uiCorrMotorThresh;      /* Ħ�г����ֵ���� */
    unsigned int    uiCorrEveningThresh;    /* �ƻ�����ֵ */
    unsigned int    uiDayMotorWidth;        /* ����Ħ�г��������ֵ */
    unsigned int    uiNightMotorWidth;      /* ҹ��Ħ�г��������ֵ */
    unsigned char 	ucCapDelayInterval;		/* �ɼ��ӳ�֡�� */

    char			cCapDelayNum;			/* ���Ƽ����֡�� */
	char        	cMinPlatePixelSize;		/* ��С�������� */
	char        	cMaxPlatePixelSize;		/* ��������� */	
    char            cResv[124];
}_PACKED_ STR_BAYONET_VL_INFO, *PSTR_BAYONET_VL_INFO;
    
/* 5.6 ����������Ȧ�߼���Ϣ���� */
typedef STR_BAYONET_VL_INFO STR_BAYONET_VL_INFO_SET;

/* 5.7 �����ֶ�ץ�� */
typedef struct tagSTR_BAYONET_HAND_SNAP
{ 
    unsigned int    uiCarRoadId;
    unsigned int    uiCarSpeed;
    char            cResv[128];
}_PACKED_ STR_BAYONET_HAND_SNAP, *PSTR_BAYONET_HAND_SNAP;

/* 5.8 ���ڱ��� */

/* 5.9 ����FTP��Ϣ��ѯ */
typedef struct tagSTR_BAYONET_FTP_INFO_QUERY
{ 
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char            cResv1[18];
    char            szUserName[32];
    char            szUserPasswd[32];
    char			cTRecEnable;
    unsigned char   ucTRecLast;
    unsigned short  usTRecInterval;
    char            cResv2[124];
}_PACKED_ STR_BAYONET_FTP_INFO_QUERY, *PSTR_BAYONET_FTP_INFO_QUERY;

/* 5.10 ����FTP��Ϣ���� */
typedef STR_BAYONET_FTP_INFO_QUERY STR_BAYONET_FTP_INFO_SET;

/* 5.11 ����ͼƬOSD��Ļ������Ϣ��ѯ */
typedef struct tagSTR_BAYONET_JPG_OSD_CONF_QUERY
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
    char            cRsv[127];
    
}_PACKED_ STR_BAYONET_JPG_OSD_CONF_QUERY, *PSTR_BAYONET_JPG_OSD_CONF_QUERY;

/* 5.12 ����ͼƬOSD��Ļ������Ϣ���� */
typedef STR_BAYONET_JPG_OSD_CONF_QUERY STR_BAYONET_JPG_OSD_CONF_SET;

/* 5.13 ��ѯ����Υ���������������� */
typedef struct tagSTR_PP_IRREG_CONF_INFO_QUERY
{
	char	szOverSpeedSmallPercent50Code[8];	/* ����С��50%��Υ�´��� */
	char	szOverSpeedSmallPercent50Name[64];	/* ����С��50%��Υ������ */

	char	szOverSpeedLargePercent50Code[8];	/* ���ٴ���50%��Υ�´��� */
	char	szOverSpeedLargePercent50Name[64];	/* ���ٴ���50%��Υ������ */
	
	char	szReverseRunCode[8];				/* ����Υ�´��� */
	char	szReverseRunName[64];				/* ����Υ������ */

	char	szInfringeProhibCode[8]; 			/* ������Υ�������־ָʾ��Υ�´��� */
	char	szInfringeProhibName[64];			/* ������Υ�������־ָʾ��Υ������ */

	char	cRsv[256];
}PP_IRREG_CONF_INFO_QUERY,*PPP_IRREG_CONF_INFO_QUERY;

/* 5.14 ���ÿ���Υ���������������� */
typedef struct tagSTR_PP_IRREG_CONF_INFO_QUERY PP_IRREG_CONF_INFO_SET;

/* 5.15 ���ݿ���FTP��Ϣ��ѯ */
typedef struct tagSTR_BAYONET_BACKUPS_FTP_INFO_QUERY
{
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char            cResv1[18];
    char            szUserName[32];
    char            szUserPasswd[32];
    char            cResv2[128];
}_PACKED_ STR_BAYONET_BACKUPS_FTP_INFO_QUERY, *PSTR_BAYONET_BACKUPS_FTP_INFO_QUERY;

/* 5.16 ���ݿ���FTP��Ϣ���� */
typedef STR_BAYONET_BACKUPS_FTP_INFO_QUERY STR_BAYONET_BACKUPS_FTP_INFO_SET;

/* 6. electronic police protocol */

/* ����������ݽṹ���� */
typedef struct tagSTR_RED_LIGHT_AREA
{
    STR_XCS_AREA        strArea;           /* ��������� */
}_PACKED_ STR_RED_LIGHT_AREA, *PSTR_RED_LIGHT_AREA;

typedef struct STR_XCS_RED_LIGHT_MODE_SET
{
	unsigned char ucLightStrobeEn;		/* Ƶ�������Ƿ�ʹ�� */
	unsigned char ucLightStrobeFrame;	/* Ƶ������֡�� */
	unsigned char ucSingalColLight;		/* �Ƿ��ǵ�ɫ��(0:��ɫ��;1:��ɫ��) */
	unsigned char ucMapRDNo;			/* ӳ�䵽��Ƽ�����ĺ�ƺ� */
} _PACKED_ STR_XCS_RED_LIGHT_MODE_SET, *PSTR_XCS_RED_LIGHT_MODE_SET;



/* ��ͨ����ɫ��ֵ */
typedef struct tagSTR_LIGHT_THRESHOLD
{
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;
    unsigned char ucResv;
}_PACKED_ STR_LIGHT_THRESHOLD, *PSTR_LIGHT_THRESHOLD;

/* 6.1 ���Ӿ���ȫ����Ϣ��ѯ */
typedef struct tagSTR_ELECT_POLICE_GLOBAL_INFO_QUERY
{ 
    char                szCarRoadName[32];    /* ������ */
    char	            szDirection[32];
    unsigned short      usPicMixStyle;        /* ͼƬ�ϳɷ�ʽ:3��1��6��1 */
    unsigned short	    usFlashLightType;     /* ��������� */
    unsigned short	    usFlashCtlType;       /* ��������:���ڻ�IO�� */
    char	            cFlashComId;          /* ����ƿ��ƴ��ں� */
    char	            cFlashDevType;        /* ������豸���� */
    unsigned short	    ausFlashAddr[4];	  /* ����� */
  	unsigned char 		aucFreqFlashEn[4];	  /* Ƶ�����Ƿ�ʹ�ܱ�־ */
	unsigned char		aucFlashTrigMode[4];  /* ����ƴ�����ʽ */
    unsigned char		aucStrobeFlashFre[4]; /* Ƶ���Ʊ�Ƶϵ�� */
	char				szEpDevCode[MAX_DEV_CODE_LEN];	  /* �羯�豸��� */

    STR_RED_LIGHT_AREA	strRedArea[6][4];
    
    STR_LIGHT_THRESHOLD strRedLow;
    STR_LIGHT_THRESHOLD strRedHigh;
    STR_LIGHT_THRESHOLD strGreenLow;
    STR_LIGHT_THRESHOLD strGreenHigh;
    STR_LIGHT_THRESHOLD strYellowLow;
    STR_LIGHT_THRESHOLD strYellowHigh;
    char				szRoadRemarks[32];
	char        	    cDetectType;           	 /* �����Ȧ���� */
	char				cValidChanSum;			 /* ʵ����Ч�������� */
	char				cCarDistance;			 /* ���һ��ͼƬ��������ͣ���߾��루��λ�ף� */
	char				cFrameInterval;			 /* ץ�ļ��֡������Χ1~5�� */
	char				cLightAmendDisFlag;		 /* ���������Ȧ�Ƿ���ʾ��־ */

	/*Start of ITS on 2011-5-9 14:48 2.0.0.1*/
	char				cPPSendEn;				 /* ����ͼƬ�Ƿ��ϱ���־ */
	/*End of ITS on 2011-5-9 14:48 2.0.0.1*/
	
	unsigned short		usEPGProVer;			 /* �羯ȫ����ϢЭ��С�汾�� */
	char            	cLightCtlMode;			 /* ����ƿ���ģʽ */
	
	/*Start of ITS on 2011-4-19 9:23 2.0.0.1*/
	char				cOSDColour;				 /* ����������ɫ(0:�ڰ� 1:��ɫ) */
	char				cRLAmendLevel;			 /* ���������ǳ�ȼ� */
	/*End of ITS on 2011-4-19 9:23 2.0.0.1*/
	
	char				cRLAmednEn;			 	 /* �������ʹ�ܱ�־ */
	char				szRoadId[8];			 /* ·��ID */

	char				cRLDecType;				 /* ��Ƽ�ⷽʽ */
	
	STR_XCS_RED_LIGHT_MODE_SET strRedLightMode[6];/* ����������ģʽ���� */
	char				cPPFlashEn;				 /* ����ͼƬ�Ƿ����� */
	char				szDirId[8];				 /* ����ID */

	char				cACSyncSwitchEn;		 /* AC������λ�л�ʹ�� */
	unsigned char		ucACSwitchThreshold;	 /* AC������λ�л��Ҷȷ�ֵ */
	unsigned char		ucACDecCycle;			 /* AC������λ������� */
	
	char				cRedModifyMode;			 /* �������������1����ɰ汾 2���Ͼɰ汾 
													3���·�����ȫ�Զ���4���·������Զ�1��
													5���·������ֶ�,���ް׵㣩
													6���·������ֶ����ް׵㣩*/
													
	unsigned short		usDayACPhase;			 /* ����AC������λֵ */
	unsigned short		usNightACPhase;			 /* ����AC������λֵ */

	char				szExEpDevCode[EXT_DEV_CODE_LEN];/* ��չ�豸����ַ��� */
	unsigned char		ucSmartSwitchEn;		/* ���������л�ʹ�� */
	unsigned char		ucTimerSwitchEn;		/* ���ⶨʱ�л�ʹ�� */
	char				cRsv2[2];
	unsigned int		uiFillInLightBeginTime;	/* ������ʼʱ�� */
	unsigned int		uiFillInLightLastTime;	/* ����ʱ�� */
    char	            cResv[14];
}_PACKED_ STR_ELECT_POLICE_GLOBAL_INFO_QUERY, *PSTR_ELECT_POLICE_GLOBAL_INFO_QUERY;

/* 6.2 ���Ӿ���ȫ����Ϣ���� */
typedef struct tagSTR_ELECT_POLICE_GLOBAL_INFO_QUERY STR_ELECT_POLICE_GLOBAL_INFO_SET;

/* 6.3 ���Ӿ��쳵����Ϣ��ѯ */
typedef struct tagSTR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY
{ 
    unsigned int   uiCarRoadId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY, *PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY;



/* nRedDetectDesp COM ����ʱ�Ľṹ���� */
typedef struct tagSTR_RED_LIGHT_COM_DESP
{
    char    cComId;
    char    cCarRoadId;
    char    cRev[28];
}_PACKED_ STR_RED_LIGHT_COM_DESP, *PSTR_RED_LIGHT_COM_DESP;

/* nRedDetectDesp GPIO ����ʱ�Ľṹ���� */
typedef struct tagSTR_RED_LIGHT_GPIO_DESP
{
    char    cGpioId;
    char    cRev[29];
}_PACKED_ STR_RED_LIGHT_GPIO_DESP, *PSTR_RED_LIGHT_GPIO_DESP;

typedef union tagSTR_EP_RED_LIGHT_DESP
{
    STR_RED_LIGHT_COM_DESP    strEPComRedLightDesp;
    STR_RED_LIGHT_GPIO_DESP   strEPGpioRedLightDesp;
}_PACKED_ STR_EP_RED_LIGHT_DESP, *PSTR_EP_RED_LIGHT_DESP;

/* �羯������Ȧ�ĵ�����ṹ���� */
typedef struct STR_EP_VL_POINT
{
	unsigned short usVLX1;
	unsigned short usVLY1;
}_PACKED_ STR_EP_VL_POINT, *PSTR_EP_VL_POINT;

/* �羯��ͨ�ƽṹ�嶨�� */
typedef struct TAG_STR_XCS_TRAFFIC_LIGHT
{
	char cEnable;
	char cLightType;
	char cVehicleNum;
	char cLSta;			/* ���״̬����Э��ģ�����Ǳ����ֽڣ���ʵ��ʹ�����Ǻ��״ֵ̬ */
}_PACKED_ STR_XCS_TRAFFIC_LIGHT, *PSTR_XCS_TRAFFIC_LIGHT;

typedef struct tagSTR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY
{
    unsigned int            uiVersion;
    unsigned int            uiCarRoadId;
    unsigned short          usOSDPosX;           /* ץ��ͼƬ�ϵ�OSD��ʾx���� */
    unsigned short          usOSDPosY;           /* ץ��ͼƬ�ϵ�OSD��ʾy���� */
    char                    szOSDContent[32];    /* ץ��ͼƬ�ϵ�OSD���� */
    STR_XCS_AREA            strRoadArea;         /* ���������� */
    unsigned short          usRedLDetectType;    /* ��Ƽ�ⷽʽ */
    unsigned short          usRedLConnectMode;   /* �������ģʽ */
    STR_EP_RED_LIGHT_DESP   strRedLDesp;         /* ��Ƽ�ⷽʽ���� */
    
    unsigned char        	ucMinPlatePixelSize; /* ��С�������� */
	unsigned char        	ucMaxPlatePixelSize; /* ��������� */
	unsigned char        	ucCorrEveningThresh; /* �ƻ�����ֵ */
    unsigned short        	usCoilDist;          /* ��Ȧ֮��ľ��룬����Ϊ��λ */
    unsigned char           ucPPDelaySnapFrames; /* ����ͼƬ��ʱץ��֡�� */
    char                    cRev1[24];

    unsigned short          usVehicleLinkMode;   /* ���������ӷ�ʽ */
    STR_COIL_DESP           strCoilDesp;         /* �ظ���Ȧ���� */
    unsigned char           ucSimSnapFlag;       /* ģ����ץ�ı�־ */
    

	/*Start of ITS on 2011-4-2 13:47 2.0.0.1 for ���ݶ���������Ȧ���ģʽ�л����� */
	char        	    	cDetectType;         /* �����Ȧ���� */
	char					cCoilSwitchFlag;	 /* ��Ȧ���ģʽ�л�ʹ�ܱ�־ */
	char					cCoilSwitchSta;		 /* ��Ȧ���ģʽ�л�״̬ */
	/*End of ITS on 2011-4-2 13:47 2.0.0.1*/
	

    /* begin add for 500-x version 2010-8-10 */
    STR_EP_VL_POINT			strVLPoint[4];		 			/* ������Ȧ����λ�ö��� */
    STR_XCS_AREA			strTrafficLigntArea; 			/* ������Ȧ��ÿ������Ӧ�Ľ�ͨ���������� */
    STR_XCS_TRAFFIC_LIGHT   strTrafficLight[MAX_LIGHT_TYPE];/* ��ͨ������ */
    char                    cLaneDir;			 			/* ������ʻ������ */
    char					cVLDispFlag;					/* ������Ȧ�Ƿ���ʾ��־ */
    char					cLaneLoopDispFlag;				/* ������Χ��Ȧ�Ƿ���ʾ��־ */
    /* end add for 500-x version 2010-8-10 */

    /*Start of ITS on 2011-4-19 9:25 2.0.0.1*/
    char					cCarNoDir;						/* ���з������Զ��� */
    /*End of ITS on 2011-4-19 9:25 2.0.0.1*/


    unsigned short 			usEPProVer; 					/* �羯������ϢЭ��С�汾�� */

    /*Start of ITS on 2011-4-2 13:55 2.0.0.1*/
    unsigned short	    	usFlashLightType;     			/* ��������� */
    /*End of ITS on 2011-4-2 13:55 2.0.0.1*/
    
    char					cRsv5[4];						/* �����ֽ� */
    
	char				    cCarDistance;			 		/* ���һ��ͼƬ��������ͣ���߾��루��λ�ף� */
	char				    cFrameInterval;			 		/* ץ�ļ��֡������Χ1~5�� */
	
	/*Start of ITS on 2011-4-2 13:55 2.0.0.1*/
	char					cRedLightDectSta;				/* ��Ƽ��ģʽ״̬��ѯ */
	/*End of ITS on 2011-4-2 13:55 2.0.0.1*/

	char					cInLightSta;					/* ���ⷽʽ״̬��ѯ */
	char					cVLDecEn;						/* ������Ȧ�Ƿ�ʹ�ܼ���־ */

	/*Start of ITS on 2011-4-19 9:26 2.0.0.1*/
	STR_EP_VL_POINT			strNoLaneVLPoint[2];		 	/* ����������ʻ����������λ�ö��� */
	/*End of ITS on 2011-4-19 9:26 2.0.0.1*/

	/*Start of ITS on 2011-5-3 11:8 2.0.0.1*/
	char					cCarTrackEn;					/* �켣��������ʹ��λ */
	/*End of ITS on 2011-5-3 11:8 2.0.0.1*/

	/*Start of ITS on 2011-6-25 17:5 2.0.0.1*/
	char					cCarSnapDir;					/* �Ƿ��жϳ���ʻ���� */
	/*End of ITS on 2011-6-25 17:5 2.0.0.1*/
	char					szLaneId[8];					/* ����Id */

	char					cSwitchLaneEn;			 		/* �Ƿ�Ϊ�ɱ䳵�� */
    char					cSwitchLaneLeftNo;		 		/* �ɱ䳵����ת����ƺ� */
    char					cSwitchLaneStraightNo;		 	/* �ɱ䳵��ֱ�з���ƺ� */
    char                    cRsv3[48];
}_PACKED_ STR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY, *PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY;

/* 6.4 ���Ӿ��쳵����Ϣ���� */
typedef STR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY STR_ELECT_POLICE_CAR_CHANNEL_INFO_SET;

/* 6.5 ���Ӿ�����״̬��ѯ */

typedef struct tagSTR_ELECT_POLICE_RED_L_STA_PC_QUERY
{ 
    unsigned int   uiCarRoadId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_ELECT_POLICE_RED_L_STA_PC_QUERY, *PSTR_ELECT_POLICE_RED_L_STA_PC_QUERY;


typedef struct tagSTR_BAYONET_RED_L_STA_QUERY
{
    unsigned int    uiCarRoadId;
    unsigned char    ucRedSta;           /* 1:��� 0:�Ǻ�� */
    char            cRsv[3];
    STR_XCS_TRAFFIC_LIGHT   strTrafficLight[MAX_LIGHT_TYPE];/* ��ͨ������ */
    
    char 			cSwitchLaneLeftNo;	 		/* �ɱ䳵����ת����ƺ� */
    char 			cSwitchLaneLeftNoSta;		/* �ɱ䳵����ת�����״̬ */

    char 			cSwitchLaneStraightNo; 		/* �ɱ䳵��ֱ�з���ƺ� */    
    char 			cSwitchLaneStraightNoSta;  	/* �ɱ䳵��ֱ�з����״̬ */
    char            cResv[108];
}_PACKED_ STR_ELEC_POLICE_RED_L_STA_QUERY, *PSTR_ELEC_POLICE_RED_L_STA_QUERY;

/* 6.6 ģ�⳵����ʻץ�� */
typedef struct tagSTR_BAYONET_SIMU_RUN_SNAP
{
    unsigned int    uiCarRoadId;
    char	        cResv[128];
}_PACKED_ STR_ELECT_POLICE_SIMU_RUN_SNAP, *PSTR_ELECT_POLICE_SIMU_RUN_SNAP;

/* 6.7 ǿ�ƺ�Ʋ�ѯ */
typedef struct tagSTR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY
{ 
    unsigned int    uiCarRoadId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY, *PSTR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY;


typedef struct tagSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY
{
    unsigned int	uiCarRoadId;
    unsigned char	ucForceRedSta;      /* 1:ǿ�ƺ�� 0:��ǿ�ƺ�� */
    char            cResv[131];
}_PACKED_ STR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY, *PSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY;

/* 6.8 ǿ�ƺ������ */
typedef struct tagSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY STR_ELECT_POLICE_RED_LIGHT_ENFORCE_SET;

/* 6.9 ���Ӿ���FTP��Ϣ��ѯ */
typedef struct tagSTR_ELECT_POLICE_FTP_INFO_QUERY
{
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char	        cResv1[18];
    char	        szUserName[32];
    char	        szUserPasswd[32];
    char	        cResv2[128];
}_PACKED_ STR_ELECT_POLICE_FTP_INFO_QUERY, *PSTR_ELECT_POLICE_FTP_INFO_QUERY;

/* 6.10 ���Ӿ���FTP��Ϣ���� */
typedef STR_ELECT_POLICE_FTP_INFO_QUERY STR_ELECT_POLICE_FTP_INFO_SET;

/* 6.11 ��ѯ�羯��ֹ��ʻʱ�����Ϣ */
typedef struct tagSTR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY
{ 
    unsigned char    ucLaneId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY, *PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY;

typedef struct tagSTR_FORBID_DIR_INFO
{
	char		cForbidDirType;
	char		cRange1Enable;
	char		cRange2Enable;
	char		cRsv1;
	int			iBeginTime1;
	int			iLastTime1; 	/* ����ʱ��1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* ����ʱ��2 */	
}STR_FORBID_DIR_INFO;

typedef struct tagSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY
{
	char				cLaneId;
	/*Start of ITS on 2011-4-19 10:12 2.0.0.1*/
	char				cRsv2[3];
	STR_FORBID_DIR_INFO strForbidDirInfo[4];
	/*End of ITS on 2011-4-19 10:12 2.0.0.1*/
	char        		cResv[128];
}_PACKED_ STR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY, *PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY;

/* 6.12 ���õ羯��ֹ��ʻʱ�����Ϣ */
typedef struct tagSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY STR_ELECT_POLICE_FORBIT_TIME_RANGE_SET;

/* 6.13 ��ѯ�羯�������������Ϣ */
typedef struct tagSTR_EP_ASSIST_RD_PC_QUERY
{ 
    unsigned char    ucLaneId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_EP_ASSIST_RD_PC_QUERY, *PSTR_EP_ASSIST_RD_PC_QUERY;

typedef struct tagSTR_EP_ASSIST_RD_INFO
{
	char		cRedLightNum;
	char		cEnableFlag;
	char 		cRsv[2];
	int			iBeginTime1;
	int			iLastTime1; 	/* ����ʱ��1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* ����ʱ��2 */	
	int			iBeginTime3;
	int			iLastTime3; 	/* ����ʱ��3 */
}STR_EP_ASSIST_RD_INFO;

typedef struct tagSTR_EP_ASSIST_RD_QUERY
{
	char				cLaneId;
	char				cRsv2[3];
	STR_EP_ASSIST_RD_INFO strEpAssistRDInfo[4];
	char        		cResv[128];
}_PACKED_ STR_EP_ASSIST_RD_QUERY, *PSTR_EP_ASSIST_RD_QUERY;

/* 6.14 ���õ羯�������ʱ�����Ϣ */
typedef struct tagSTR_EP_ASSIST_RD_QUERY STR_EP_ASSIST_RD_SET;

/* 6.15 ��ѯ�羯Υ���������������� */
/* �羯����ʶ��������Ϣ */
typedef struct tagSTR_EP_LPR_CONF_INFO_QUERY
{
	unsigned char ucLPREn;					/* ����ʶ��ʹ�ܱ�־ */
	unsigned char ucLPRCConfidence;			/* �������ַ����Ŷ� */
	char    szLPRFirstChar[6];				/* �������ַ� */
	unsigned short usLPRWidth;				/* ����ʶ���� */
	unsigned short usLPRHeight;				/* ����ʶ��߶� */

	char           cResv[128];
}EP_LPR_CONF_INFO_QUERY,*PEP_LPR_CONF_INFO_QUERY;

/* 6.16 ���õ羯Υ���������������� */
typedef struct tagSTR_EP_LPR_CONF_INFO_QUERY EP_LPR_CONF_INFO_SET;

/* 6.17 ��ѯ�羯Υ���������������� */
typedef struct tagSTR_EP_IRREG_CONF_INFO_QUERY
{
	char 			cDispFlag;			/* Υ�������Ƿ���ӵ�ͼƬ�� */

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
	
	char			cRsv[40];
}EP_IRREG_CONF_INFO_QUERY,*PEP_IRREG_CONF_INFO_QUERY;

/* 6.18 ���õ羯Υ���������������� */
typedef struct tagSTR_EP_IRREG_CONF_INFO_QUERY EP_IRREG_CONF_INFO_SET;

/* 6.19 ��ѯ�羯���½���������Ϣ */
typedef struct tagSTR_EP_SOT_NO_ENTRY_PC_QUERY
{ 
    unsigned char    ucLaneId;        /* ��ѯ�ĳ����� */
}_PACKED_ STR_EP_SOT_NO_ENTRY_PC_QUERY, *PSTR_EP_SOT_NO_ENTRY_PC_QUERY;

typedef struct tagSTR_EP_SOT_NO_ENTRY_INFO
{
	char		cEnableFlag;
	char 		cRsv[3];
	int			iBeginTime1;
	int			iLastTime1; 	/* ����ʱ��1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* ����ʱ��2 */	
	int			iBeginTime3;
	int			iLastTime3; 	/* ����ʱ��3 */
}STR_EP_SOT_NO_ENTRY_INFO;

typedef struct tagSTR_EP_SOT_NOENTRY_QUERY
{
	char				cLaneId;
	char				cRsv2[3];
	STR_EP_SOT_NO_ENTRY_INFO strEpSOTNoEntryInfo;
	char        		cResv[128];
}_PACKED_ STR_EP_SOT_NOENTRY_QUERY, *PSTR_EP_SOT_NOENTRY_QUERY;

/* 6.20 ���õ羯���½���������Ϣ */
typedef struct tagSTR_EP_SOT_NOENTRY_QUERY STR_EP_SOT_NOENTRY_SET;

/* 6.21 �羯ͼƬOSD��Ļ������Ϣ��ѯ */
typedef struct tagSTR_EP_JPG_OSD_CONF_QUERY
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
	char            cRsv[125];

}_PACKED_ STR_EP_JPG_OSD_CONF_QUERY, *PSTR_EP_JPG_OSD_CONF_QUERY;

/* 6.22 �羯ͼƬOSD��Ļ������Ϣ���� */
typedef STR_EP_JPG_OSD_CONF_QUERY STR_EP_JPG_OSD_CONF_SET;

/* 6.23 ���ݵ羯FTP��Ϣ��ѯ */
typedef struct tagSTR_EP_BACKUPS_FTP_INFO_QUERY
{
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char            cResv1[18];
    char            szUserName[32];
    char            szUserPasswd[32];
    char            cResv2[128];
}_PACKED_ STR_EP_BACKUPS_FTP_INFO_QUERY, *PSTR_EP_BACKUPS_FTP_INFO_QUERY;

/* 6.24 ���ݵ羯FTP��Ϣ���� */
typedef STR_EP_BACKUPS_FTP_INFO_QUERY STR_EP_BACKUPS_FTP_INFO_SET;

/* 6.25 ��ѯ�羯����Υ���������������� */
typedef struct tagSTR_EP_NEWADD_IRREG_CONF_INFO_QUERY
{
	char			szRunInNoLaneCode[8]; 		/* ���ڻ�����������ʻΥ�´��� */
	char			szRunInNoLaneName[64];		/* ���ڻ�����������ʻΥ������ */

	char			cRsv[512];
}EP_NEWADD_IRREG_CONF_INFO_QUERY,*PEP_NEWADD_IRREG_CONF_INFO_QUERY;

/* 6.26 ���õ羯����Υ���������������� */
typedef struct tagSTR_EP_NEWADD_IRREG_CONF_INFO_QUERY EP_NEWADD_IRREG_CONF_INFO_SET;

/* 7. record protocol */

/* 7.1 ������Ϣ��ѯ */
typedef struct tagSTR_RECORD_DISK_INFO_QUERY
{
    unsigned int	uiDiskNum;      /* ���̷������ */
    unsigned int	uiCap;          /* �������� */
    unsigned int	uiUsedCap;
    char            cResv[128];
}_PACKED_ STR_RECORD_DISK_INFO_QUERY, *PSTR_RECORD_DISK_INFO_QUERY;

/* 7.2 ���� */

/* 7.3 ¼�����ء��طŷ�ʽ��ѯ */
typedef struct tagSTR_RTSP_CONF_QUERY
{
	char	cRtspMode;
	char	cRsv1[3];
	char	szRtspIP[16];
	char	szRtspPath[32];
	short	sRtspPort;
	short	sHttpPort;
	char	szUserName[20];
	char	szPassword[20];
	char	cRsv2[128];
}_PACKED_ STR_RTSP_CONF_QUERY,*LPSTR_RTSP_CONF_QUERY;

/* 7.4 ¼�����ػطŷ�ʽ���� */
typedef struct tagSTR_RTSP_CONF_QUERY STR_RTSP_CONF_SET;

/* 7.5 ������¼��ƻ���ѯ */
typedef struct tagSTR_RECORD_PERIOD_PLAN_PC_QUERY
{
    unsigned short    usChannelId;
}_PACKED_ STR_RECORD_PERIOD_PLAN_PC_QUERY, *PSTR_RECORD_PERIOD_PLAN_PC_QUERY;

typedef struct tagSTR_RECORD_PERIOD_PLAN_QUERY
{
    unsigned short	usVChannel;      
    char	        cWeekDay;       /* �����ĸ������գ� 
                                        =1����������һ��
                                        =2���������ڶ���
                                        ��,
                                        =7��������
                                        =0����ʾÿ�� */
    char	        cBeginHour;
    char	        cBeginMinute;
    char	        cEndHour;
    char	        cEndMinute;
    char	        cResv[17];
}_PACKED_ STR_RECORD_PERIOD_PLAN_QUERY, *PSTR_RECORD_PERIOD_PLAN_QUERY;

/* 7.6 ������¼��ƻ����� */
typedef STR_RECORD_PERIOD_PLAN_PC_QUERY STR_RECORD_PERIOD_PLAN_ADD;

/* 7.7 ������¼��ƻ�ɾ�� */
typedef STR_RECORD_PERIOD_PLAN_PC_QUERY STR_RECORD_PERIOD_PLAN_DEL;

/* 7.8 ������¼��ƻ����� */
typedef STR_RECORD_PERIOD_PLAN_PC_QUERY STR_RECORD_PERIOD_PLAN_UPDATE;

/* 7.9 ͨ��¼�������ѯ */
/* ���洢ģʽΪESMAuto��ESMStop��ESMLoop��ESMCustomʱ������Ӧ������ */
typedef struct tagSTR_RECORD_CHANNEL_PARAM_QUERY
{
    unsigned int	uiVChannel;
    unsigned int	uiRecDays;              /* ���¼�񱣴����� */
    unsigned int	uiPreRecTime;           /* Ԥ¼ʱ�䣬��λ�� */
    unsigned int	uiDiskPolicy;           /* �洢���� */
    char            acDiskIndex[16];
}_PACKED_ STR_RECORD_CHANNEL_PARAM_QUERY, *PSTR_RECORD_CHANNEL_PARAM_QUERY;

/* ���洢ģʽΪESMSpaceʱ������Ӧ������ */
typedef struct tagSTR_RECORD_CHANNEL_PARAM_SPACE_QUERY
{
    unsigned int	uiVChannel;
    unsigned short	usRecDays;              /* ���¼�񱣴����� */
    unsigned short	usRecSpace;             /* ���¼������ */
    unsigned int	uiPreRecTime;           /* Ԥ¼ʱ�䣬��λ�� */
    unsigned int	uiDiskPolicy;           /* �洢���� */
    char            acDiskIndex[16];
}_PACKED_ STR_RECORD_CHANNEL_PARAM_SPACE_QUERY, *PSTR_RECORD_CHANNEL_PARAM_SPACE_QUERY;


/* 7.10 ͨ��¼��������� */
typedef struct tagSTR_RECORD_CHANNEL_PARAM_SET
{
    unsigned int    uiDiskParamNum;
    void            *pVoid;
}_PACKED_ STR_RECORD_CHANNEL_PARAM_SET, *PSTR_RECORD_CHANNEL_PARAM_SET;

/* 7.11 ����¼���¼ */
typedef struct tagSTR_RECORD_PC_SEARCH
{
    unsigned int	uiRecordType;
    unsigned int	uiBeginTime;
    unsigned int	uiEndTime;
    unsigned short	usVideoNum; 
    short   		nDigestType;        //Ԥ��ժҪ�����ͣ����Ϊ0��ʾ������ժҪ��1ΪJPEG
    char        	nReserved[4];       //Ԥ��
}_PACKED_ STR_RECORD_PC_SEARCH, *PSTR_RECORD_PC_SEARCH;

typedef struct tagSTR_QUERY_INDEX_HEAD
{
    unsigned int	uiTotalIndexNum;      /* �������������������� */
    unsigned int	uiCurIndexNum;        /* ��ǰ������������������ */
    unsigned int	uiIndexStrLen;        /* �����ṹ���� */
    unsigned int   nDigestStructLen;  //����ժҪ�ṹ�ĳ��ȣ����Ϊ0��ʾ������
}_PACKED_ STR_QUERY_INDEX_HEAD, *PSTR_QUERY_INDEX_HEAD;

typedef struct tagSTR_QUERY_INDEX
{
    unsigned int	uiRecordType;
    unsigned int	uiStartTime;
    unsigned int	uiEndTime;
    unsigned short	usVideoNum;
    char	        cResv1[2];
    unsigned int	uiFileSize;
	unsigned int    nFileOffset;		//�¼����ļ��е�ƫ��λ��
    char	        sFileName[128];
}_PACKED_ STR_QUERY_INDEX, *PSTR_QUERY_INDEX;

typedef struct
{
    short   nDigestSize;
    short   nDigestType; //Ԥ��ժҪ�����ͣ����Ϊ0��ʾ������ժҪ��1ΪJPEG
    char 	data[164]; //ժҪ���ݣ�������nDigestSizeָ��
}_PACKED_ TRecDigest,*LPTRecDigest ;

typedef struct tagSTR_RECORD_SEARCH
{
    STR_QUERY_INDEX_HEAD strQueryIndexHead;
    STR_QUERY_INDEX      *pStrIndex;
}_PACKED_ STR_RECORD_SEARCH, *PSTR_RECORD_SEARCH;

/* 7.12 ¼���ļ��ط� */
typedef struct tagSTR_RECORD_PC_PLAYBACK
{
    char	cFileName[128];
    char	cResv[128];
}_PACKED_ STR_RECORD_PC_PLAYBACK, *PSTR_RECORD_PC_PLAYBACK;

typedef struct tagSTR_RECORD_PLAYBACK
{
    char	cFileUrl[256];
    char	cResv[128];
}_PACKED_ STR_RECORD_PLAYBACK, *PSTR_RECORD_PLAYBACK;

/* 7.13 ¼���ļ����� */
typedef struct tagSTR_RECORD_PC_FILE_DOWNLOAD
{
    char	cFileName[128];
    char	cResv[128];
}_PACKED_ STR_RECORD_PC_FILE_DOWNLOAD, *PSTR_RECORD_PC_FILE_DOWNLOAD;

typedef struct tagSTR_RECORD_FILE_DOWNLOAD
{
    char	cFileUrl[256];
    char	cResv[128];
}_PACKED_ STR_RECORD_FILE_DOWNLOAD, *PSTR_RECORD_FILE_DOWNLOAD;

/* 8. CAMARA info protocol */

/* 8.1 ������������Բ�ѯ */
typedef struct tagSTR_CAMARA_ATTRIB_PC_QUERY
{
    unsigned int   uiHdCameraId;
}_PACKED_ STR_CAMARA_ATTRIB_PC_QUERY, *PSTR_CAMARA_ATTRIB_PC_QUERY;

typedef struct tagSTR_CAMARA_ATTRIB_QUERY
{
    unsigned int	uiHdCameraId;
    unsigned int	uiWidth;
    unsigned int	uiHeight;
    unsigned int	uiPixels;
    char	        cResv[128];
}_PACKED_ STR_CAMARA_ATTRIB_QUERY, *PSTR_CAMARA_ATTRIB_QUERY;


/* 8.2 ��������ò�����Χ��ѯ */
typedef struct tagSTR_CAMARA_CTL
{
    char	cCamaraType;
    char	cResv1[2];
    char	cParamIndex;
    char 	cCtlType;
    char	cResv2[3];
    float	fMinVal;              /* �̶�ֵ / ��Сֵ */
    float	fMaxVal;              /* ���ֵ */
}_PACKED_ STR_CAMARA_CTL, *PSTR_CAMARA_CTL;

typedef struct tagSTR_CAMARA_CTL_PARAM_QUERY
{
    char	        cCtrlNum;
    char	        cResv[3];
    STR_CAMARA_CTL  pstrCamaraCtl[10];
}_PACKED_ STR_CAMARA_CTL_PARAM_QUERY, *PSTR_CAMARA_CTL_PARAM_QUERY;

/* 8.3 �����״̬������ѯ */
typedef struct tagSTR_CAMARA_STA_INFO_QUERY
{
    float	fShutter;
    float	fGain;
    float	fContrast;
    float	fSaturation;
    float	fBright;
    float	fGamma;
    float	fExposure;
    /* add for xcc200 */
    float   fAperture;
    float	fIso;
    float	fZoom;
    float   fFocus;
    float   fAWB;
    float   fSharpness;
    char	cResv[104];
}_PACKED_ STR_CAMARA_STA_INFO_QUERY, *PSTR_CAMARA_STA_INFO_QUERY;


/* 8.5 ������������ݲ�ѯ */

typedef struct tagSTR_CAMARA_CTL_INFO_PC_QUERY
{
    char	cParamIndex;
}_PACKED_ CAMARA_CTL_INFO_PC_QUERY, *PCAMARA_CTL_INFO_PC_QUERY;

typedef struct tagSTR_CAMARA_CTL_INFO_QUERY
{
    char	        cCtrlNum;
    char	        cResv[2];
    char			cParamIndex;
    STR_CAMARA_CTL  pstrCamaraCtl[MAX_CAM_CTRL_NUM];
}_PACKED_ STR_CAMARA_CTL_INFO_QUERY, *PSTR_CAMARA_CTL_INFO_QUERY;

/* 8.6 ����������������� */
typedef struct tagSTR_CAMARA_CTL_INFO_QUERY  STR_CAMARA_CTL_INFO_SET;

/* 8.7 �����ʱ��β�ѯ */
typedef struct tagSTR_CAMARA_TIME_RANGE_PC_QUERY
{
    char	cParamIndex;
}_PACKED_ CAMARA_TIME_RANGE_PC_QUERY, *PCAMARA_TIME_RANGE_PC_QUERY;

typedef struct tagSTR_XCS_CAMERA_TIME_RANGE
{
	char		cGroupId;
	char		cEnable;
	char		szSceneName[30];
	int			iBeginTime;
	int			iLastTime;
	char		cIrisPos;
	char 		cRsv[127];
}XCS_CAMERA_TIME_RANGE_INFO_QUERY, *PXCS_CAMERA_TIME_RANGE_INFO_QUERY;

/* 8.8 �����ʱ������� */
typedef struct tagSTR_XCS_CAMERA_TIME_RANGE  XCS_CAMERA_TIME_RANGE_INFO_SET;


/* 9. net info protocol */

/* 9.1 ������Ϣ��ѯ */
typedef struct tagSTR_NET_INFO_QUERY
{
    unsigned int	uiIPAddr;
    unsigned int	uiNetMask;
    unsigned int	uiGateWay;
    char	        cResv[128];
}_PACKED_ STR_NET_INFO_QUERY, *PSTR_NET_INFO_QUERY;

/* 9.2 ������Ϣ���� */
typedef STR_NET_INFO_QUERY STR_NET_INFO_SET;

/* 9.3 ͼƬ����洢��Ϣ��ѯ */
typedef struct tagSTR_PIC_NET_SAVE_INFO_QUERY
{
    unsigned int	uiEnable;           /* �Ƿ���������洢 */
    unsigned int	uiStorageType;      /* �洢���ͣ�NFS��SAMBA */
    unsigned int	uiIpAddr;
    char	        szFolderName[32];
    char	        szUserName[32];
    char	        szPassWord[32];
    char	        cResv[128];
}_PACKED_ STR_PIC_NET_SAVE_INFO_QUERY, *PSTR_PIC_NET_SAVE_INFO_QUERY;

/* 9.4 ͼƬ����Ӳ����Ϣ���� */
typedef STR_PIC_NET_SAVE_INFO_QUERY STR_PIC_NET_SAVE_INFO_SET;

/* 9.5 ¼������洢��Ϣ��ѯ */
typedef STR_PIC_NET_SAVE_INFO_QUERY STR_RECORD_NET_SAVE_QUERY;

/* 9.6 ¼������Ӳ����Ϣ���� */
typedef STR_PIC_NET_SAVE_INFO_QUERY STR_RECORD_NET_SAVE_SET;

/* 9.7 �ƴ洢������Ϣ��ѯ */
typedef struct tagSTR_NET_REC_MANAGE_QUERY
{
	char			cEnable;
	char			cRsv[3];
    unsigned int	uiMainNRMIp;
    unsigned int	uiBakNRMIp;
    unsigned short	usMainNRMPort;
    unsigned short	usBakNRMPort;
	char			szPuID[32];
    char	        cResv[128];
}_PACKED_ STR_NET_REC_MANAGE_QUERY, *PSTR_NET_REC_MANAGE_QUERY;

/* 9.8 �ƴ洢������Ϣ���� */
typedef STR_NET_REC_MANAGE_QUERY STR_NET_REC_MANAGE_SET;

/* 10. dev connect protocol */

/* 10.1 330ƽ̨������Ϣ��ѯ */
typedef struct tagSTR_330_CON_QUERY
{
    char cVer;                       /* �汾�� */
    char cEnable;                    /* �Ƿ���� */
    char cLaneSeq1;                  /* ������� */
    char cLaneSeq2;
    char cLaneSeq3;
    char cLaneSeq4;
    char szResv[2];
    char sz330DevId[128];            /* 330�豸ID */
    char sz330DirId[16];             /* 330����ID */
    char sz330Key[256];              /* 330��Կ */
}_PACKED_ STR_330_CON_QUERY, *PSTR_330_CON_QUERY;

/* 10.2 ������Ϣ���� */
typedef struct tagSTR_330_CON_QUERY STR_330_CON_SET;

/* 10.3 v3ƽ̨������Ϣ��ѯ */
typedef struct tagSTR_V3_CON_QUERY
{
    unsigned int    uiV3SerIp;
    unsigned short  usV3Port;
    unsigned char   ucRev[2];
    char            szDevNo[20];
    char            szDevPwd[16];
    char            cV3ServerSeq;
    char            cV3TimeSynFlag;
    char            cV3Enable;
    char            cVersion;
}_PACKED_ STR_V3_CON_QUERY, *PSTR_V3_CON_QUERY;

/* 10.4 v3ƽ̨������Ϣ���� */
typedef struct tagSTR_V3_CON_QUERY STR_V3_CON_SET;

typedef struct tagSmallProtoVer
{
	unsigned short usPPChanelVer;
	unsigned short usEPGlobalVer;
	unsigned short usEPChanelVer;
	unsigned short usRsv;
}_PACKED_ STR_SMALL_PRO_VER;


/* 10.5 �羯330ƽ̨������Ϣ��ѯ */
typedef struct tagSTR_330_CON_QUERY STR_EP_330_CON_QUERY;

/* 10.6 ������Ϣ���� */
typedef struct tagSTR_330_CON_QUERY STR_EP_330_CON_SET;

/* 10.7 Gb28181ƽ̨������Ϣ��ѯ */
typedef struct tagSTR_GB28181_CON_QUERY
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

    char			cEnFlag;				/* ���ñ�־ */
    char			cRsv[127];
}_PACKED_ STR_GB28181_CON_QUERY, *PSTR_GB28181_CON_QUERY;

/* 10.8 Gb28181ƽ̨������Ϣ���� */
typedef struct tagSTR_GB28181_CON_QUERY STR_GB28181_CON_SET;

/* 12. net manage info proto */

/* ������Ϣ��ѯ */
typedef struct tagStrNetManageQuery
{
    unsigned int   uiDataLen;       /* �ַ����� */
    char           *pcData;			/* �ַ����� */
}_PACKED_ STR_NET_MANAGE_QUERY, *PSTR_NET_MANAGE_QUERY;

/***********************************************************************************/
/* some default parameter */
/***********************************************************************************/
#ifndef XCS_NT_PACK_HEAD_LENGTH
#define  NT_PACK_HEAD_LENGTH		16
#endif

/* ���ش�����궨�� */
#define     ERROR_TIME_OUT        0xFF /* ��ʱ */
#define     ERROR_WRITE_CONF      0xFE /* д����ʧ�� */
#define     ERROR_READ_CONF       0xFD /* ������ʧ�� */
#define     ERROR_FUNC_CODE       0xFC /* ������� */
#define     ERROR_PSW_NAME        0x01

/* Э��С�汾�ź궨�� */
#define XCS_PRO_INVALID_VER0 0x0
#define XCS_PRO_INVALID_VER1 0xFFFF

/* ���ڳ���Э��С�汾�� */
#define XCS_PRO_PP_VER		0x1001
#define XCS_PRO_PP_VER_2	0x1002

/* �羯����Э��С�汾�� */
#define XCS_PRO_EP_VER		0x1001
#define XCS_PRO_EP_VER_2	0x1002

/* �羯ȫ��Э��С�汾�� */
#define XCS_PRO_EPG_VER		0x1001
#define XCS_PRO_EPG_VER_2	0x1002

#endif /* _PROTOCOL_XCS__H__ */

