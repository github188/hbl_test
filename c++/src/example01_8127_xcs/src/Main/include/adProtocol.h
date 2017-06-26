/******************************************************************************
 * adProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  安道雷达协议数据结构定义
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

速度上报协议说明:

[字节1]:起始字节0xFF
[字节2]:速度高字节
[字节3]:速度低字节
[字节4]:校验字节
*/



#define AD_MSG_FIFO_LEN    120     /* 开辟的缓冲池大小 */

#define AD_MSG_START_FLAG  0xFF    /* 起始帧字节 */
#define AD_MSG_END_FLAG    0xAA    /* 结束帧字节 */

#define AD_MSG_FRAME_LEN   0x04   /* 帧长 */

/* 故障状态 */
typedef struct
{
    unsigned char ucRadarSta;	   /* 雷达故障状态 */
}AD_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucCarSpeed;			/* 当前车速 */
	unsigned char		ucCarDir;			/* 车行方向 */
    AD_BREAKDOWN_INFO 	strBreakDownInfo;	/* 故障状态 */
}AD_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     		/* 保留字节 */
    AD_PROTO_DATA_STR strADProtoData;   /* 实际上报协议数据 */
}AD_PROTO_STR;

#endif /* _DEV_AD_PROTOCOL_H */

