/******************************************************************************
 * adProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �����״�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2012.04.16, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_AD_PROTOCOL_H
#define _DEV_AD_PROTOCOL_H

#include "kjProtocol.h"

/*

�ٶ��ϱ�Э��˵��:

[�ֽ�1]:��ʼ�ֽ�0xFF
[�ֽ�2]:�ٶȸ��ֽ�
[�ֽ�3]:�ٶȵ��ֽ�
[�ֽ�4]:У���ֽ�
*/



#define AD_MSG_FIFO_LEN    120     /* ���ٵĻ���ش�С */

#define AD_MSG_START_FLAG  0xFF    /* ��ʼ֡�ֽ� */
#define AD_MSG_END_FLAG    0xAA    /* ����֡�ֽ� */

#define AD_MSG_FRAME_LEN   0x04   /* ֡�� */

/* ����״̬ */
typedef struct
{
    unsigned char ucRadarSta;	   /* �״����״̬ */
}AD_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucCarSpeed;			/* ��ǰ���� */
	unsigned char		ucCarDir;			/* ���з��� */
    AD_BREAKDOWN_INFO 	strBreakDownInfo;	/* ����״̬ */
}AD_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */
    AD_PROTO_DATA_STR strADProtoData;   /* ʵ���ϱ�Э������ */
}AD_PROTO_STR;

#endif /* _DEV_AD_PROTOCOL_H */

