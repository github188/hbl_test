/******************************************************************************
 * cyldProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  超远雷电雷达协议数据结构定义
 *     
 * modification history
 * --------------------
 * 01a, 2014.05.26, its Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_CYLD_PROTOCOL_H
#define _DEV_CYLD_PROTOCOL_H

#include "kjProtocol.h"

/*

速度上报协议说明:

[字节1]:工作模式
		静止平行  		0xf7	(不用)
　　　　静止定角同向 	0xf2	(抓拍车尾)
		静止定角反向  	0xf4	(抓拍车头)
[字节2]:抓拍确认字节	0xfb	
[字节3]:本车速度高字节
[字节4]:本车速度中字节
[字节5]:本车速度低字节

[字节6]:目标车速高字节
[字节7]:目标车速中字节
[字节8]:目标车速低字节

[字节9] :cpi数目高字节
[字节10]:cpi数目低字节

*/



#define CYLD_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */
#define CYLD_MSG_FRAME_LEN   0x0A   /* 帧长 */

#define CYLD_HEAD_FLAG1		0xF2	/* 第一种帧头 */
#define CYLD_HEAD_FLAG2		0xF4	/* 第二种帧头 */
#define CYLD_HEAD_FLAG3		0xF7	/* 第三种帧头 */

/* 故障状态 */
typedef struct
{
    unsigned char ucRadarSta;	   /* 雷达故障状态 */
}CYLD_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucCarSpeed;			/* 当前车速 */
	unsigned char		ucCarDir;			/* 车行方向 */
    CYLD_BREAKDOWN_INFO 	strBreakDownInfo;	/* 故障状态 */
}CYLD_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     		/* 保留字节 */
    CYLD_PROTO_DATA_STR strCyldProtoData;   /* 实际上报协议数据 */
}CYLD_PROTO_STR;

#endif /* _DEV_CYLD_PROTOCOL_H */

