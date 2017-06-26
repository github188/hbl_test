/******************************************************************************
 * olviaProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ����ά���״�Э�����ݽṹ����
 *
 * modification history
 * --------------------
 * 01a, 2013.10.09, its Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_OLIVA_PROTOCOL_H
#define _DEV_OLIVA_PROTOCOL_H

#include "kjProtocol.h"


#define OLVIA_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */

#define OLVIA_DIR_COME_FLAG1  0xFC	  /* ���з���Ϊ�� */
#define OLVIA_DIR_COME_FLAG2  0xFA	  /* ���з���Ϊ�� */

#define OLVIA_DIR_GO_FLAG1    0xFB	  /* ���з���Ϊȥ */
#define OLVIA_DIR_GO_FLAG2    0xFD	  /* ���з���Ϊȥ */

#define OLVIA_MSG_FRAME_LEN   0x04    /* ֡�� */

/* ����״̬ */
typedef struct
{
    unsigned char ucRadarSta;	   /* �״����״̬ */
}OLVIA_BREAKDOWN_INFO;


typedef struct
{
	unsigned char			ucCarSpeed;			/* ��ǰ���� */
	unsigned char			ucCarDir;			/* ���з��� */
	unsigned char			ucNo;				/* �״��� */
    OLVIA_BREAKDOWN_INFO	strBreakDownInfo;	/* ����״̬ */
}OLVIA_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            	/* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     			/* �����ֽ� */
    OLVIA_PROTO_DATA_STR strOlviaProtoData;	/* ʵ���ϱ�Э������ */
}OLVIA_PROTO_STR;

#endif /* _DEV_OLVIA_PROTOCOL_H */

