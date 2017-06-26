/******************************************************************************
 * spiProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  科骏协议数据结构定义
 *     
 * modification history
 * --------------------
 * 01a, 2012.04.10, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_SPI_PROTOCOL_H
#define _DEV_SPI_PROTOCOL_H

/*
协议说明:
[起始字节0]：0xef
[字节1,检测器号码]:区分检测器 1-6
[字节2,车辆状态]:车辆状态 1:有车  0:无车
[结束字节]:0x5c
*/

/* modify by wangb 2010-2-3 */
#define SPI_MSG_FIFO_LEN    2480    /* 开辟的缓冲池大小 */

#define SPI_MSG_START_FLAG1 0xAA    /* 起始帧字节1 */
#define SPI_MSG_START_FLAG2 0xBB    /* 起始帧字节2 */

#define SPI_MSG_END_FLAG1 	0xCC    /* 结束帧字节1 */
#define SPI_MSG_END_FLAG2 	0xDD    /* 结束帧字节2 */

#define SPI_MSG_FRAME_LEN   248     /* 帧长 */


typedef struct tagSpiDataStruct
{
	unsigned char ucStartFlag1;
	unsigned char ucStartFlag2;
	unsigned char ucData[244];
	unsigned char ucEndFlag1;
	unsigned char ucEndFlag2;
}SPI_DATA_STRUCT;

#endif /* _DEV_SPI_PROTOCOL_H */

