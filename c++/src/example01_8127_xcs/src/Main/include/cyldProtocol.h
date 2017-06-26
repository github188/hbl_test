/******************************************************************************
 * cyldProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ��Զ�׵��״�Э�����ݽṹ����
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

�ٶ��ϱ�Э��˵��:

[�ֽ�1]:����ģʽ
		��ֹƽ��  		0xf7	(����)
����������ֹ����ͬ�� 	0xf2	(ץ�ĳ�β)
		��ֹ���Ƿ���  	0xf4	(ץ�ĳ�ͷ)
[�ֽ�2]:ץ��ȷ���ֽ�	0xfb	
[�ֽ�3]:�����ٶȸ��ֽ�
[�ֽ�4]:�����ٶ����ֽ�
[�ֽ�5]:�����ٶȵ��ֽ�

[�ֽ�6]:Ŀ�공�ٸ��ֽ�
[�ֽ�7]:Ŀ�공�����ֽ�
[�ֽ�8]:Ŀ�공�ٵ��ֽ�

[�ֽ�9] :cpi��Ŀ���ֽ�
[�ֽ�10]:cpi��Ŀ���ֽ�

*/



#define CYLD_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */
#define CYLD_MSG_FRAME_LEN   0x0A   /* ֡�� */

#define CYLD_HEAD_FLAG1		0xF2	/* ��һ��֡ͷ */
#define CYLD_HEAD_FLAG2		0xF4	/* �ڶ���֡ͷ */
#define CYLD_HEAD_FLAG3		0xF7	/* ������֡ͷ */

/* ����״̬ */
typedef struct
{
    unsigned char ucRadarSta;	   /* �״����״̬ */
}CYLD_BREAKDOWN_INFO;


typedef struct
{
	unsigned char		ucCarSpeed;			/* ��ǰ���� */
	unsigned char		ucCarDir;			/* ���з��� */
    CYLD_BREAKDOWN_INFO 	strBreakDownInfo;	/* ����״̬ */
}CYLD_PROTO_DATA_STR;

typedef struct
{
    unsigned char ucMsgType;            /* ʵ���ϴ���Ϣ���ͣ���ʱ���� */
	unsigned char ucRcv[3];     		/* �����ֽ� */
    CYLD_PROTO_DATA_STR strCyldProtoData;   /* ʵ���ϱ�Э������ */
}CYLD_PROTO_STR;

#endif /* _DEV_CYLD_PROTOCOL_H */

