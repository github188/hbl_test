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
#ifndef _DEV_DC_PROTOCOL_H
#define _DEV_DC_PROTOCOL_H

/*
协议说明:
[起始字节0]：0xef
[字节1,检测器号码]:区分检测器 1-6
[字节2,车辆状态]:车辆状态 1:有车  0:无车
[结束字节]:0x5c
*/

/* modify by wangb 2010-2-3 */
#define DC_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */

#define DC_MSG_START_FLAG  0xEF    /* 起始帧字节 */
#define DC_MSG_END_FLAG    0x5C    /* 结束帧字节 */
#define DC_MSG_FRAME_LEN   0x4     /* 帧长 */



typedef struct
{
	unsigned char header;
	unsigned char id;
	unsigned char status;
	unsigned char tailer;
}DC_DATA_STRUCT;

#endif /* _DEV_DC_PROTOCOL_H */
