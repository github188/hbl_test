/******************************************************************************
 * hcProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �۲��״�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2012.07.11, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_HC_PROTOCOL_H
#define _DEV_HC_PROTOCOL_H

#include "kjProtocol.h"

/*

�ٶ��ϱ�Э��˵��:

[�ֽ�1]:��ʼ�ֽ�0xFA
[�ֽ�2]:������
[�ֽ�3]:�ٶ�ֵ
*/



#define HC_MSG_FIFO_LEN    120     /* ���ٵĻ���ش�С */

#define HC_MSG_START_FLAG  0xFA    /* ������ʼ֡�ֽ� */

#define HC_MSG_FRAME_LEN   0x03    /* ֡�� */

/* ����״̬ */
typedef struct
{
    unsigned char ucRadarSta;	   /* �״����״̬ */
}HC_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucLaneId;			/* ������ */
	unsigned char		ucCarSpeed;			/* ��ǰ���� */
	unsigned char		ucCarDir;			/* ���з��� */
    HC_BREAKDOWN_INFO 	strBreakDownInfo;	/* ����״̬ */
}HC_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */
    HC_PROTO_DATA_STR strHCProtoData;   /* ʵ���ϱ�Э������ */
}HC_PROTO_STR;

#endif /* _DEV_HC_PROTOCOL_H */


