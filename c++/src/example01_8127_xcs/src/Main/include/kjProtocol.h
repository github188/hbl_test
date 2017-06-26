/******************************************************************************
 * kjProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �ƿ�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.25, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_KJ_PROTOCOL_H
#define _DEV_KJ_PROTOCOL_H

/*
Э��˵��:
[��ʼ�ֽ�0]��0xaa
[�ֽ�1,��Ϣ����]:���ֳ�������ơ�����
[�ֽ�2,������Ϣ]:�ĸ��������ĸ���ƣ���Щ�����ϵ�
[�ֽ�3]:0x00(Ԥ������ʱ����ٶ���Ϣ)
[�ֽ�4]:0x00(Ԥ������ʱ����ٶ���Ϣ)
[�����ֽ�]:0x55
*/

/* modify by wangb 2010-2-3 */
#define KJ_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */

#define KJ_MSG_START_FLAG  0xAA    /* ��ʼ֡�ֽ� */
#define KJ_MSG_END_FLAG    0x55    /* ����֡�ֽ� */
#define KJ_MSG_FRAME_LEN   0x6     /* ֡�� */

/* ��Ϣ���ͺ궨�� */
#define MSG_TYPE_COIL_STA       0 /* ������ */
#define MSG_TYPE_RED_LIGHT      1 /* ���������Ϣ */
#define MSG_TYPE_BREAKDOWN      2 /* ������Ϣ */


/* ����״̬ */
#define BREAKDOWN_STA_OK        0 /* ����״̬���������Ϊ��������10��������û�յ����ʾ������� */
#define BREAKDOWN_STA_FAILED    1 /* ����״̬ */
#define BREAKDOWN_STA_STOP      2 /* ����������״̬ */
#define BREAKDOWN_ALL_STA       3 /* �������ۺ�״̬�ϱ� */
#define BREAKDOWN_STA_WORK      4 /* ����������״̬ */

/* �����豸�б� */
#define BREAKDOWN_CAR_CHANNEL_1_FRONT_COIL  1   /* ����1��Ȧ1 */
#define BREAKDOWN_CAR_CHANNEL_1_BEHIND_COIL 2   /* ����1��Ȧ2 */
#define BREAKDOWN_CAR_CHANNEL_2_FRONT_COIL  3   /* ����2��Ȧ1 */
#define BREAKDOWN_CAR_CHANNEL_2_BEHIND_COIL 4   /* ����2��Ȧ2 */
#define BREAKDOWN_CAR_CHANNEL_3_FRONT_COIL  5   /* ����3��Ȧ1 */
#define BREAKDOWN_CAR_CHANNEL_3_BEHIND_COIL 6   /* ����3��Ȧ2 */

#define BREAKDOWN_RED_LIGHT_CAR_CHANNEL_1  0x10 /* ����1��� */
#define BREAKDOWN_RED_LIGHT_CAR_CHANNEL_2  0x11 /* ����2��� */
#define BREAKDOWN_RED_LIGHT_CAR_CHANNEL_3  0x12 /* ����3��� */

/* ������Ȧ״̬ */
typedef struct
{
    unsigned char ucCarChannel; /* ������ 1~3 */
    unsigned char ucCoilPos;    /* ��Ȧ�ڳ����ϵ�λ�� */
    unsigned char ucCoilSta;    /* ������Ȧ�ϵ�״̬ */
    unsigned char ucCarDir;     /* ����ʻ���� */
    unsigned short usLastTime;  /* ������ǰ����Ȧ��ʱ��� ��λ:ms */
    unsigned short usGroupId;   /* ������� */
}COIL_INFO;

/* ���״̬ */
typedef struct
{
    unsigned char ucLightNum;   /* ��ʾ�ĸ��� */
    unsigned char ucLightSta;   /* ���״̬ */
    unsigned char ucRcv[2];     /* �����ֽ� */
}RED_LIGHT_INFO;



/* ����״̬ */
typedef struct
{
    unsigned char ucBreakDownSta;   /* ����״̬ */
    unsigned char ucDevNum;         /* �����豸��� */
    unsigned char ucLaneSta;		/* ����������״̬ */
    unsigned char ucRedLightSta;	/* �������״̬ */
}BREAKDOWN_INFO;


typedef union
{
    COIL_INFO strCoilInfo;          /* ������Ȧ״̬ */
    RED_LIGHT_INFO strRedLightInfo; /* ���״̬ */
    BREAKDOWN_INFO strBreakDownInfo;/* ����״̬ */
}KJ_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */    
    KJ_PROTO_DATA_STR strKjProtoData;   /* ʵ���ϱ��ƿ�Э������ */
}KJ_PROTO_STR;

#endif /* _DEV_KJ_PROTOCOL_H */
