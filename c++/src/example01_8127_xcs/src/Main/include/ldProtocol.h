/******************************************************************************
 * ldProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  �����Ƽ����Э�����ݽṹ����
 *     
 * modification history
 * --------------------
 * 01a, 2012.06.06, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_LD_PROTOCOL_H
#define _DEV_LD_PROTOCOL_H

/*
Э��˵��:
[��ʼ�ֽ�0]��0xa0
[�ֽ�1,���ݳ���]:
[�ֽ�2,���ֽں��״̬]:��λ����: 1:����  0:����
[�ֽ�3,���ֽں��״̬]:��λ����: 1:����  0:����
[�ֽ�4,���ݺ�У��]
[�ֽ�5,���ݺ�ȡ��У��]
*/

#define LD_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */

#define LD_MSG_START_FLAG  0xA0    /* ��ʼ֡�ֽ� */
#define LD_MSG_DATA_LEN	   0x05	   /* ���ݳ���Ϊ�̶�ֵ */
#define LD_MSG_FRAME_LEN   0x6     /* ֡�� */

typedef struct
{
	unsigned char ucLowData;
	unsigned char ucHighData;
}LD_DATA_STRUCT;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[1];     		/* �����ֽ� */
    LD_DATA_STRUCT strLDProtoData;		/* ʵ���ϱ�Э������ */
}LD_PROTO_STR;

#endif /* _DEV_LD_PROTOCOL_H */

