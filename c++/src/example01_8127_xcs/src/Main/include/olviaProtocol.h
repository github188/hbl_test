/******************************************************************************
 * olviaProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  奥利维亚雷达协议数据结构定义
 *
 * modification history
 * --------------------
 * 01a, 2013.10.09, its Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_OLIVA_PROTOCOL_H
#define _DEV_OLIVA_PROTOCOL_H

#include "kjProtocol.h"


#define OLVIA_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */

#define OLVIA_DIR_COME_FLAG1  0xFC	  /* 车行方向为来 */
#define OLVIA_DIR_COME_FLAG2  0xFA	  /* 车行方向为来 */

#define OLVIA_DIR_GO_FLAG1    0xFB	  /* 车行方向为去 */
#define OLVIA_DIR_GO_FLAG2    0xFD	  /* 车行方向为去 */

#define OLVIA_MSG_FRAME_LEN   0x04    /* 帧长 */

/* 故障状态 */
typedef struct
{
    unsigned char ucRadarSta;	   /* 雷达故障状态 */
}OLVIA_BREAKDOWN_INFO;


typedef struct
{
	unsigned char			ucCarSpeed;			/* 当前车速 */
	unsigned char			ucCarDir;			/* 车行方向 */
	unsigned char			ucNo;				/* 雷达编号 */
    OLVIA_BREAKDOWN_INFO	strBreakDownInfo;	/* 故障状态 */
}OLVIA_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            	/* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     			/* 保留字节 */
    OLVIA_PROTO_DATA_STR strOlviaProtoData;	/* 实际上报协议数据 */
}OLVIA_PROTO_STR;

#endif /* _DEV_OLVIA_PROTOCOL_H */

