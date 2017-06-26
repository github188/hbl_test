/******************************************************************************
 * kjProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  科骏协议数据结构定义
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.25, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_KJ_PROTOCOL_H
#define _DEV_KJ_PROTOCOL_H

/*
协议说明:
[起始字节0]：0xaa
[字节1,消息类型]:区分车道、红灯、故障
[字节2,具体消息]:哪个车道，哪个红灯，哪些出故障等
[字节3]:0x00(预留卡口时间或速度信息)
[字节4]:0x00(预留卡口时间或速度信息)
[结束字节]:0x55
*/

/* modify by wangb 2010-2-3 */
#define KJ_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */

#define KJ_MSG_START_FLAG  0xAA    /* 起始帧字节 */
#define KJ_MSG_END_FLAG    0x55    /* 结束帧字节 */
#define KJ_MSG_FRAME_LEN   0x6     /* 帧长 */

/* 消息类型宏定义 */
#define MSG_TYPE_COIL_STA       0 /* 车道号 */
#define MSG_TYPE_RED_LIGHT      1 /* 红灯亮灭信息 */
#define MSG_TYPE_BREAKDOWN      2 /* 故障信息 */


/* 故障状态 */
#define BREAKDOWN_STA_OK        0 /* 正常状态，可以理解为心跳，若10分钟以上没收到则表示外设故障 */
#define BREAKDOWN_STA_FAILED    1 /* 故障状态 */
#define BREAKDOWN_STA_STOP      2 /* 车检器掉电状态 */
#define BREAKDOWN_ALL_STA       3 /* 车检器综合状态上报 */
#define BREAKDOWN_STA_WORK      4 /* 车检器工作状态 */

/* 故障设备列表 */
#define BREAKDOWN_CAR_CHANNEL_1_FRONT_COIL  1   /* 车道1线圈1 */
#define BREAKDOWN_CAR_CHANNEL_1_BEHIND_COIL 2   /* 车道1线圈2 */
#define BREAKDOWN_CAR_CHANNEL_2_FRONT_COIL  3   /* 车道2线圈1 */
#define BREAKDOWN_CAR_CHANNEL_2_BEHIND_COIL 4   /* 车道2线圈2 */
#define BREAKDOWN_CAR_CHANNEL_3_FRONT_COIL  5   /* 车道3线圈1 */
#define BREAKDOWN_CAR_CHANNEL_3_BEHIND_COIL 6   /* 车道3线圈2 */

#define BREAKDOWN_RED_LIGHT_CAR_CHANNEL_1  0x10 /* 车道1红灯 */
#define BREAKDOWN_RED_LIGHT_CAR_CHANNEL_2  0x11 /* 车道2红灯 */
#define BREAKDOWN_RED_LIGHT_CAR_CHANNEL_3  0x12 /* 车道3红灯 */

/* 车道线圈状态 */
typedef struct
{
    unsigned char ucCarChannel; /* 车道号 1~3 */
    unsigned char ucCoilPos;    /* 线圈在车道上的位置 */
    unsigned char ucCoilSta;    /* 车在线圈上的状态 */
    unsigned char ucCarDir;     /* 车行驶方向 */
    unsigned short usLastTime;  /* 车进入前后线圈的时间差 单位:ms */
    unsigned short usGroupId;   /* 车辆编号 */
}COIL_INFO;

/* 红灯状态 */
typedef struct
{
    unsigned char ucLightNum;   /* 表示哪个灯 */
    unsigned char ucLightSta;   /* 红灯状态 */
    unsigned char ucRcv[2];     /* 保留字节 */
}RED_LIGHT_INFO;



/* 故障状态 */
typedef struct
{
    unsigned char ucBreakDownSta;   /* 故障状态 */
    unsigned char ucDevNum;         /* 出错设备序号 */
    unsigned char ucLaneSta;		/* 车检器运行状态 */
    unsigned char ucRedLightSta;	/* 红灯运行状态 */
}BREAKDOWN_INFO;


typedef union
{
    COIL_INFO strCoilInfo;          /* 车道线圈状态 */
    RED_LIGHT_INFO strRedLightInfo; /* 红灯状态 */
    BREAKDOWN_INFO strBreakDownInfo;/* 故障状态 */
}KJ_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型 */
	unsigned char ucRcv[3];     		/* 保留字节 */    
    KJ_PROTO_DATA_STR strKjProtoData;   /* 实际上报科骏协议数据 */
}KJ_PROTO_STR;

#endif /* _DEV_KJ_PROTOCOL_H */
