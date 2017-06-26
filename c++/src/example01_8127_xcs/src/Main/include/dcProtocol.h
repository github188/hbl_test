/******************************************************************************
 * kjProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �ƿ�Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.25, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_DC_PROTOCOL_H
#define _DEV_DC_PROTOCOL_H

/*
Э��˵��:
[��ʼ�ֽ�0]��0xef
[�ֽ�1,���������]:���ּ���� 1-6
[�ֽ�2,����״̬]:����״̬ 1:�г�  0:�޳�
[�����ֽ�]:0x5c
*/

/* modify by wangb 2010-2-3 */
#define DC_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */

#define DC_MSG_START_FLAG  0xEF    /* ��ʼ֡�ֽ� */
#define DC_MSG_END_FLAG    0x5C    /* ����֡�ֽ� */
#define DC_MSG_FRAME_LEN   0x4     /* ֡�� */



typedef struct
{
	unsigned char header;
	unsigned char id;
	unsigned char status;
	unsigned char tailer;
}DC_DATA_STRUCT;

#endif /* _DEV_DC_PROTOCOL_H */
