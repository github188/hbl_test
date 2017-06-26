/******************************************************************************
 * sjProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  苏江协议数据结构定义
 *     
 * modification history
 * --------------------
 * 01a, 2010.12.31, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_SJ_PROTOCOL_H
#define _DEV_SJ_PROTOCOL_H

#include "kjProtocol.h"

/*
协议说明:
[字节1,检测数据帧，心跳帧]:线圈下标，进出线圈状态
[字节2]:过车时间高字节
[字节3]:过车时间低字节
[字节4]:检测器地址及线圈故障状态
*/

#define SJ_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */

#define SJ_MSG_START_FLAG  0x40    /* 起始帧字节，为最大值不是固定值 */
#define SJ_MSG_END_FLAG    0x00    /* 结束帧字节，暂时规定为0，以后需要在配置客户端配置 */
#define SJ_MSG_FRAME_LEN   0x4     /* 帧长 */

#define SJ_HEART_BEAT_FLAG 0xe4    /* 心跳帧标志 */

/* 故障状态 */
typedef struct
{
    unsigned char ucCoilSta[4]; /* 每个线圈故障状态 */
}SJ_BREAKDOWN_INFO;


typedef struct
{
    COIL_INFO 			strCoilInfo;        /* 车道线圈状态 */
    RED_LIGHT_INFO 		strRedLightInfo; 	/* 红灯状态 */
    SJ_BREAKDOWN_INFO 	strBreakDownInfo;	/* 故障状态 */
}SJ_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型 */
	unsigned char ucRcv[3];     		/* 保留字节 */
    SJ_PROTO_DATA_STR strSjProtoData;   /* 实际上报苏江协议数据 */
}SJ_PROTO_STR;

#endif /* _DEV_SJ_PROTOCOL_H */
