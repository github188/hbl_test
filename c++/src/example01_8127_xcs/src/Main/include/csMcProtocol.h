/******************************************************************************
 * csMcProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ���ٶ೵���״�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2014.03.04, its Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_CSMC_PROTOCOL_H
#define _DEV_CSMC_PROTOCOL_H

#include "kjProtocol.h"

/*

�ٶ��ϱ�Э��˵��:
[�ֽ�0]:0xfc
[�ֽ�1]:�״��� 0xf1~0xf4
[�ֽ�2]:�ٶ�

*/

#define CSMC_MSG_FIFO_LEN    240	/* ���ٵĻ���ش�С */

#define CSMC_MSG_START_FLAG  0xF1	/* ��ʼ֡�ֽ� */
#define CSMC_MSG_END_FLAG  	 0xFC	/* ����֡�ֽڣ�����һ�� */

#define CSMC_MSG_FRAME_LEN   0x02	/* ֡�� */

#define CSMC_MSG_BEGIN_FLAG  0xFC	/* 0xFC��ʼ�ֽڣ����ٶ೵���ڶ���Э����ʼ�ֽ� */

typedef struct
{
	unsigned char		ucNo;		/* ��ǰ�״��� */
	unsigned char		ucCarSpeed;	/* ��ǰ���� */
	unsigned char		ucRsv[2];
}CSMC_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            	/* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     			/* �����ֽ� */
    CSMC_PROTO_DATA_STR strCSMCProtoData;	/* ʵ���ϱ�Э������ */
}CSMC_PROTO_STR;

#endif /* _DEV_CSMC_PROTOCOL_H */
