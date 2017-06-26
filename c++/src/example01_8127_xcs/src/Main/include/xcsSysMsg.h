/******************************************************************************
 * xcsSysMsg.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.16, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef XCS_SYS_MSGQ_H
#define XCS_SYS_MSGQ_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* ��ģ�鷢����Ϣ�е�ģ��ID�궨�� */
#define SYS_MSG_DCTL_MODULE_ID              1       /* �������ģ��ID */
#define SYS_MSG_NTP_MODULE_ID               2       /* NTP��ʱģ��ID */
#define SYS_MSG_DSP_MODULE_ID               3       /* DSPģ��ID */
#define SYS_MSG_PROTOCOL_MODULE_ID          4       /* PROTOCOL,XCSЭ��ģ��ID */
#define SYS_MSG_NETFS_MODULE_ID             5       /* �����ļ�ϵͳ��� */
#define SYS_MSG_DEBUG_SHELL_MODULE_ID       6       /* dbg����ģ��ID */

#define TRAFFIC_LIGHT_MANA_MODULE_ID		7		/* ��ͨ�ƹ���ģ�� */

/* DSPģ�������߳�ͨ����Ϣ���ͺ궨�� */

/* ץ�Ĳ���������Ϣ */
#define  MSG_VCHANN_SNAP_PARAM     0x00
/* ��Ƶ����������Ϣ */
#define  MSG_VIDEO_PARAM_SET       0x01
/* �����ϱ���Ϣ */
#define  MSG_VCHANN_DATA_REPORT    0x02

/* �ɼ��豸����������Ϣ */
#define  MSG_DEV_CTL_CAP           0x03
#define  MSG_DEV_CTL_RED_LIHGT     0x04

/* ϵͳʱ��������Ϣ */
#define  MSG_SYS_TIME_SET          0x09
/* Զ��������Ϣ */
#define  MSG_REMOTE_UPDATE         0x0a
/* Զ��������Ϣ */
#define  MSG_REMOTE_REBOOT         0x0b
/* NTP������������Ϣ */
#define  MSG_NTP_SERVER_SET        0x0c

/* Զ�ָ̻��豸�������� */
#define  MSG_REMOTE_RESTORE_PARAM  0x0d


/* ����ȫ����Ϣ���� */
#define MSG_BAYONET_GLOBAL_INFO_SET         0x11
/* ���ڳ�����Ϣ���� */
#define MSG_BAYONET_CAR_CHANNEL_INFO_SET    0x12
/* ����������Ȧ�߼���Ϣ���� */
#define MSG_BAYONET_HIGH_VL_INFO_SET        0x13
/* �����ֶ�ץ�� */
#define MSG_BAYONET_MANUAL_SNAP             0x14
/* ����FTP��Ϣ���� */
#define MSG_BAYONET_FTP_INFO_SET            0x15

/* ���Ӿ���ȫ����Ϣ���� */
#define MSG_EP_GLOBAL_INFO_SET              0x21
/* ���Ӿ��쳵����Ϣ���� */
#define MSG_EP_CAR_CHANNEL_INFO_SET         0x22
/* ģ����Ӿ����ֶ�ץ�� */
#define MSG_ELECT_POLICE_SIMU_RUN_SNAP      0x23
/* ǿ�ƺ������ */
#define MSG_EP_RED_LIGHT_ENFORCE_SET        0x24
/* ���Ӿ���FTP��Ϣ���� */
#define MSG_EP_FTP_INFO_SET                 0x25
/* ���Ӿ��쳵����ֹ��ʻʱ������� */
#define MSG_EP_TIME_RANGE_INFO_SET          0x26

/* ���Ӿ��쳵������������� */
#define MSG_EP_ASSIST_RED_LIGHT_SET         0x27

/* ���Ӿ��쳵��ʶ������ */
#define MSG_EP_LPR_INFO_SET			        0x28

/* ���Ӿ���Υ�������������������� */
#define MSG_EP_IRREG_INFO_SET			    0x29

/* ���Ӿ������½������� */
#define MSG_EP_SOT_NO_ENTRY_SET		        0x2a

/* �羯ͼƬosd�������� */
#define MSG_EP_JPG_OSD_SET		        	0x2b

/* ����ͼƬosd�������� */
#define MSG_PP_JPG_OSD_SET		        	0x2c

/* ����Υ�������������������� */
#define MSG_PP_IRREG_INFO_SET			    0x2d

/* ¼�����ػطŷ�ʽ���� */
#define MSG_RECORD_DOWNLOAD_PLAYTYPE_SET    0x31
/* ������¼��ƻ����� */
#define MSG_RECORD_PERIOD_PLAN_ADD          0x32
/* ������¼��ƻ�ɾ�� */
#define MSG_RECORD_PERIOD_PLAN_DEL          0x33
/* ������¼��ƻ����� */
#define MSG_RECORD_PERIOD_PLAN_UPDATE       0x34
/* ͨ��¼��������� */
#define MSG_RECORD_CHANNEL_PARAM_SET        0x35

/* �����ʱ������� */
#define MSG_CAMARA_TIME_RANGE_SET           0x40
/* ����������������� */
#define MSG_CAMARA_CTL_INFO_SET             0x41
/* ������Ϣ���� */
#define MSG_NET_INFO_SET                    0x42
/* ͼƬ����Ӳ����Ϣ���� */
#define MSG_NET_PIC_NET_SAVE_INFO_SET       0x43
/* ¼������Ӳ����Ϣ���� */
#define MSG_NET_RECORD_NET_SAVE_INFO_SET    0x44
/* �����ϱ���������*/
#define MSG_DATA_REPORT_PARAM_SET           0x45
/* NTP��ʱ����*/
#define MSG_NTP_TIME_REPORT                 0x46

/* 330ƽ̨������Ϣ���� */
#define MSG_330_DEV_CONNECT_INFO_SET        0x51

/* �羯330ƽ̨������Ϣ���� */
#define MSG_EP_330_DEV_CONNECT_INFO_SET     0x52

/* �ƴ洢��Ϣ���ñ䶯 */
#define MSG_NRM_INFO_SET     				0x53


/* ԭʼ�������вɼ�������Ϣ */
#define MSG_ORG_IMAG_INFO_SET               0x61

/* ��ͨ�ƹ���ģ����Ϣ */
#define MSG_TRAFFIC_LIGHT_INFO_SET          0x71

/* ��ͨ�Ƽ��ģʽ�����仯 */
#define MSG_TRAFFIC_LIGHT_MODE_SET          0x72

/* XCS�ͻ����쳣�˳� */
#define MSG_XCS_EXCEPT_DISCONNECT           0xa0

/* ģ����Ӿ����ֶ�ץ�� */
#define MSG_UART_SIMU_RUN_SNAP      0x73

/* ϵͳ��Ϣ���г��� */
#define SYS_MSG_MAX_DATA_LEN  256
#define _PACKED_ __attribute__((packed))


typedef struct tagsysRecMsgStr
{
	unsigned int    uiModuleId;                     /* ������Ϣ��ģ��ID */
	unsigned int    uiMsgType;                      /* ��Ϣ���� */
    unsigned char   ucChannelId;                    /* ͨ���� */
    unsigned char   ucCarRoadId;                    /* ������ */
	unsigned char   ucRsv[2];
	unsigned char   aucData[SYS_MSG_MAX_DATA_LEN] __attribute__((aligned(32)));  /* ��Ϣ���ݣ�256�ֽ� */
}_PACKED_ STR_SYS_APP_MSG;

/* �����ֶ�ץ����Ϣ�ṹ */

/* �ͻ����쳣�˳���Ϣ�ṹ */
typedef struct tagClientDisconExcept
{
    unsigned int    uiIp;
    unsigned int    uiSessionId;
    unsigned short  usPort;
    unsigned short  usDataUploadPort;   
    unsigned int    uiDataUploadIp;
}_PACKED_ STR_EXCEPT_DISCON_CLIENT;

#endif /* XCS_SYS_MSGQ_H */
