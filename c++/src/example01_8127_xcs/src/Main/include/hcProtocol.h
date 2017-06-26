/******************************************************************************
 * hcProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  慧昌雷达协议数据结构定义
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

速度上报协议说明:

[字节1]:起始字节0xFA
[字节2]:车道号
[字节3]:速度值
*/



#define HC_MSG_FIFO_LEN    120     /* 开辟的缓冲池大小 */

#define HC_MSG_START_FLAG  0xFA    /* 正常起始帧字节 */

#define HC_MSG_FRAME_LEN   0x03    /* 帧长 */

/* 故障状态 */
typedef struct
{
    unsigned char ucRadarSta;	   /* 雷达故障状态 */
}HC_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucLaneId;			/* 车道号 */
	unsigned char		ucCarSpeed;			/* 当前车速 */
	unsigned char		ucCarDir;			/* 车行方向 */
    HC_BREAKDOWN_INFO 	strBreakDownInfo;	/* 故障状态 */
}HC_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     		/* 保留字节 */
    HC_PROTO_DATA_STR strHCProtoData;   /* 实际上报协议数据 */
}HC_PROTO_STR;

#endif /* _DEV_HC_PROTOCOL_H */


