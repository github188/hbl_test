/******************************************************************************
 * ldProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  联大红灯检测器协议数据结构定义
 *     
 * modification history
 * --------------------
 * 01a, 2012.06.06, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_LD_PROTOCOL_H
#define _DEV_LD_PROTOCOL_H

/*
协议说明:
[起始字节0]：0xa0
[字节1,数据长度]:
[字节2,低字节红灯状态]:按位计算: 1:灯灭  0:灯亮
[字节3,高字节红灯状态]:按位计算: 1:灯灭  0:灯亮
[字节4,数据和校验]
[字节5,数据和取反校验]
*/

#define LD_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */

#define LD_MSG_START_FLAG  0xA0    /* 起始帧字节 */
#define LD_MSG_DATA_LEN	   0x05	   /* 数据长度为固定值 */
#define LD_MSG_FRAME_LEN   0x6     /* 帧长 */

typedef struct
{
	unsigned char ucLowData;
	unsigned char ucHighData;
}LD_DATA_STRUCT;

typedef struct
{
    unsigned char ucMsgType;            /* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[1];     		/* 保留字节 */
    LD_DATA_STRUCT strLDProtoData;		/* 实际上报协议数据 */
}LD_PROTO_STR;

#endif /* _DEV_LD_PROTOCOL_H */

