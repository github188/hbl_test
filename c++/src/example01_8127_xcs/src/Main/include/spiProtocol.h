/******************************************************************************
 * spiProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �ƿ�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2012.04.10, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_SPI_PROTOCOL_H
#define _DEV_SPI_PROTOCOL_H

/*
Э��˵��:
[��ʼ�ֽ�0]��0xef
[�ֽ�1,���������]:���ּ���� 1-6
[�ֽ�2,����״̬]:����״̬ 1:�г�  0:�޳�
[�����ֽ�]:0x5c
*/

/* modify by wangb 2010-2-3 */
#define SPI_MSG_FIFO_LEN    2480    /* ���ٵĻ���ش�С */

#define SPI_MSG_START_FLAG1 0xAA    /* ��ʼ֡�ֽ�1 */
#define SPI_MSG_START_FLAG2 0xBB    /* ��ʼ֡�ֽ�2 */

#define SPI_MSG_END_FLAG1 	0xCC    /* ����֡�ֽ�1 */
#define SPI_MSG_END_FLAG2 	0xDD    /* ����֡�ֽ�2 */

#define SPI_MSG_FRAME_LEN   248     /* ֡�� */


typedef struct tagSpiDataStruct
{
	unsigned char ucStartFlag1;
	unsigned char ucStartFlag2;
	unsigned char ucData[244];
	unsigned char ucEndFlag1;
	unsigned char ucEndFlag2;
}SPI_DATA_STRUCT;

#endif /* _DEV_SPI_PROTOCOL_H */

