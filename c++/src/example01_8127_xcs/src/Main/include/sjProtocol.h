/******************************************************************************
 * sjProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �ս�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2010.12.31, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_SJ_PROTOCOL_H
#define _DEV_SJ_PROTOCOL_H

#include "kjProtocol.h"

/*
Э��˵��:
[�ֽ�1,�������֡������֡]:��Ȧ�±꣬������Ȧ״̬
[�ֽ�2]:����ʱ����ֽ�
[�ֽ�3]:����ʱ����ֽ�
[�ֽ�4]:�������ַ����Ȧ����״̬
*/

#define SJ_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */

#define SJ_MSG_START_FLAG  0x40    /* ��ʼ֡�ֽڣ�Ϊ���ֵ���ǹ̶�ֵ */
#define SJ_MSG_END_FLAG    0x00    /* ����֡�ֽڣ���ʱ�涨Ϊ0���Ժ���Ҫ�����ÿͻ������� */
#define SJ_MSG_FRAME_LEN   0x4     /* ֡�� */

#define SJ_HEART_BEAT_FLAG 0xe4    /* ����֡��־ */

/* ����״̬ */
typedef struct
{
    unsigned char ucCoilSta[4]; /* ÿ����Ȧ����״̬ */
}SJ_BREAKDOWN_INFO;


typedef struct
{
    COIL_INFO 			strCoilInfo;        /* ������Ȧ״̬ */
    RED_LIGHT_INFO 		strRedLightInfo; 	/* ���״̬ */
    SJ_BREAKDOWN_INFO 	strBreakDownInfo;	/* ����״̬ */
}SJ_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */
    SJ_PROTO_DATA_STR strSjProtoData;   /* ʵ���ϱ��ս�Э������ */
}SJ_PROTO_STR;

#endif /* _DEV_SJ_PROTOCOL_H */
