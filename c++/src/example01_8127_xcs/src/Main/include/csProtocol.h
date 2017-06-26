/******************************************************************************
 * csProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �����״�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2011.09.02, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_CS_PROTOCOL_H
#define _DEV_CS_PROTOCOL_H

#include "kjProtocol.h"

/*
�����·�Э��˵��:
[�ֽ�1]:��ʼ�ֽ�0xFA
[�ֽ�2]:��������
[�ֽ�3]:����1
[�ֽ�4]:����2
[�ֽ�5]:�����ֽ�0xFB
*/

/*
���û�ӦЭ��˵��:
[�ֽ�1]:��ʼ�ֽ�0xFA
[�ֽ�2]:��4λ��ʾ�����峤��n
.
.
.
[�ֽ�n+2]:
[�ֽ�n+3]:�����ֽ�0xFB
*/

/*

�ٶ��ϱ�Э��˵��:
��ǰֻ֧���ٶ��ϱ����ֽ�ģʽ

*/

#define CS_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */

#define CS_MSG_START_FLAG  0xFA    /* ��ʼ֡�ֽ� */
#define CS_MSG_END_FLAG    0xFB    /* ����֡�ֽ� */

#define CS_DIR_COME_FLAG   0xF9	   /* ���з���Ϊ�� */
#define CS_DIR_GO_FLAG     0xF8	   /* ���з���Ϊȥ */
#define CS_DIR_RSV_FLAG    0xF7	   /* ���з���Ϊ��ȷ�� */

#define CS_MSG_FRAME_LEN   0x01   /* ֡�� */

/* ����״̬ */
typedef struct
{
    unsigned char ucRadarSta;	   /* �״����״̬ */
}CS_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucCarSpeed;			/* ��ǰ���� */
	unsigned char		ucCarDir;			/* ���з��� */
    CS_BREAKDOWN_INFO 	strBreakDownInfo;	/* ����״̬ */
}CS_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */
    CS_PROTO_DATA_STR strCSProtoData;   /* ʵ���ϱ�Э������ */
}CS_PROTO_STR;

#endif /* _DEV_CS_PROTOCOL_H */
