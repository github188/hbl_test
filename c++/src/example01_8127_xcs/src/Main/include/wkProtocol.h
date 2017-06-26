/******************************************************************************
 * wkProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  文康雷达协议数据结构定义
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

速度上报协议说明:

[字节1]:起始字节1
[字节2]:起始字节2
[字节3]:速度值
[字节3]:RCS值
*/



#define WK_MSG_FIFO_LEN    120     /* 开辟的缓冲池大小 */

#define WK_MSG_REV_START_FLAG0 0xFC    /* 反向起始帧字节1 */
#define WK_MSG_REV_START_FLAG1 0xFA    /* 反向起始帧字节1 */

#define WK_MSG_DIR_START_FLAG0 0xFB    /* 正向起始帧字节1 */
#define WK_MSG_DIR_START_FLAG1 0xFD    /* 正向起始帧字节1 */


#define WK_MSG_FRAME_LEN   0x04    /* 帧长 */

/* 故障状态 */
typedef struct
{
    unsigned char ucRadarSta;	   /* 雷达故障状态 */
}WK_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucLaneId;			/* 车道号 */
	unsigned char		ucCarSpeed;			/* 当前车速 */
	unsigned char		ucCarDir;			/* 车行方向 */
    WK_BREAKDOWN_INFO 	strBreakDownInfo;	/* 故障状态 */
}WK_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     		/* 保留字节 */
    WK_PROTO_DATA_STR strWKProtoData;   /* 实际上报协议数据 */
}WK_PROTO_STR;

#endif /* _DEV_WK_PROTOCOL_H */



