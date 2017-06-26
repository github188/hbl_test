/******************************************************************************
 * csProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  川速雷达协议数据结构定义
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
配置下发协议说明:
[字节1]:起始字节0xFA
[字节2]:配置类型
[字节3]:参数1
[字节4]:参数2
[字节5]:结束字节0xFB
*/

/*
配置回应协议说明:
[字节1]:起始字节0xFA
[字节2]:低4位表示数据体长度n
.
.
.
[字节n+2]:
[字节n+3]:结束字节0xFB
*/

/*

速度上报协议说明:
当前只支持速度上报单字节模式

*/

#define CS_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */

#define CS_MSG_START_FLAG  0xFA    /* 起始帧字节 */
#define CS_MSG_END_FLAG    0xFB    /* 结束帧字节 */

#define CS_DIR_COME_FLAG   0xF9	   /* 车行方向为来 */
#define CS_DIR_GO_FLAG     0xF8	   /* 车行方向为去 */
#define CS_DIR_RSV_FLAG    0xF7	   /* 车行方向为不确定 */

#define CS_MSG_FRAME_LEN   0x01   /* 帧长 */

/* 故障状态 */
typedef struct
{
    unsigned char ucRadarSta;	   /* 雷达故障状态 */
}CS_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucCarSpeed;			/* 当前车速 */
	unsigned char		ucCarDir;			/* 车行方向 */
    CS_BREAKDOWN_INFO 	strBreakDownInfo;	/* 故障状态 */
}CS_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     		/* 保留字节 */
    CS_PROTO_DATA_STR strCSProtoData;   /* 实际上报协议数据 */
}CS_PROTO_STR;

#endif /* _DEV_CS_PROTOCOL_H */
