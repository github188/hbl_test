/******************************************************************************
 * wkProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �Ŀ��״�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2012.07.11, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_WK_PROTOCOL_H
#define _DEV_WK_PROTOCOL_H

#include "kjProtocol.h"

/*

�ٶ��ϱ�Э��˵��:

[�ֽ�1]:��ʼ�ֽ�1
[�ֽ�2]:��ʼ�ֽ�2
[�ֽ�3]:�ٶ�ֵ
[�ֽ�3]:RCSֵ
*/



#define WK_MSG_FIFO_LEN    120     /* ���ٵĻ���ش�С */

#define WK_MSG_REV_START_FLAG0 0xFC    /* ������ʼ֡�ֽ�1 */
#define WK_MSG_REV_START_FLAG1 0xFA    /* ������ʼ֡�ֽ�1 */

#define WK_MSG_DIR_START_FLAG0 0xFB    /* ������ʼ֡�ֽ�1 */
#define WK_MSG_DIR_START_FLAG1 0xFD    /* ������ʼ֡�ֽ�1 */


#define WK_MSG_FRAME_LEN   0x04    /* ֡�� */

/* ����״̬ */
typedef struct
{
    unsigned char ucRadarSta;	   /* �״����״̬ */
}WK_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucLaneId;			/* ������ */
	unsigned char		ucCarSpeed;			/* ��ǰ���� */
	unsigned char		ucCarDir;			/* ���з��� */
    WK_BREAKDOWN_INFO 	strBreakDownInfo;	/* ����״̬ */
}WK_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */
    WK_PROTO_DATA_STR strWKProtoData;   /* ʵ���ϱ�Э������ */
}WK_PROTO_STR;

#endif /* _DEV_WK_PROTOCOL_H */



