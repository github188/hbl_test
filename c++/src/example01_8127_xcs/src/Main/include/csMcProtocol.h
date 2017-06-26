/******************************************************************************
 * csMcProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  川速多车道雷达协议数据结构定义
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

速度上报协议说明:
[字节0]:0xfc
[字节1]:雷达编号 0xf1~0xf4
[字节2]:速度

*/

#define CSMC_MSG_FIFO_LEN    240	/* 开辟的缓冲池大小 */

#define CSMC_MSG_START_FLAG  0xF1	/* 起始帧字节 */
#define CSMC_MSG_END_FLAG  	 0xFC	/* 结束帧字节，乱填一个 */

#define CSMC_MSG_FRAME_LEN   0x02	/* 帧长 */

#define CSMC_MSG_BEGIN_FLAG  0xFC	/* 0xFC起始字节，川速多车道第二种协议起始字节 */

typedef struct
{
	unsigned char		ucNo;		/* 当前雷达编号 */
	unsigned char		ucCarSpeed;	/* 当前车速 */
	unsigned char		ucRsv[2];
}CSMC_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            	/* 实际上传消息类型，暂时无用 */
	unsigned char ucRcv[3];     			/* 保留字节 */
    CSMC_PROTO_DATA_STR strCSMCProtoData;	/* 实际上报协议数据 */
}CSMC_PROTO_STR;

#endif /* _DEV_CSMC_PROTOCOL_H */
