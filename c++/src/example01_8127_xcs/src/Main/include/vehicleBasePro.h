/******************************************************************************
 * vehicleBasePro.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ������������ص�Э�鶼������������ϣ�����Э���ɸ���Э��ͷ�ļ�����
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.24, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef __VEHICLE_BASE_PRO_H__
#define __VEHICLE_BASE_PRO_H__

#include "hagdProtocol.h"
#include "kjProtocol.h"
#include "sjProtocol.h"
#include "csProtocol.h"
#include "dcProtocol.h"
#include "spiProtocol.h"
#include "adProtocol.h"
#include "ldProtocol.h"
#include "hcProtocol.h"
#include "wkProtocol.h"
#include "olviaProtocol.h"
#include "csMcProtocol.h"
#include "cyldProtocol.h"

/* ��Ȧλ�ú궨�� */
#define COIL_FRONT_POS  0 /* ��ʾ��Ȧ1 �������Ƚ�����Ǹ���Ȧ */
#define COIL_BEHIND_POS 1 /* ��ʾ��Ȧ2 ��������뿪���Ǹ���Ȧ */

/* ��������Ȧ����궨�� */
#define CAR_RUN_CORRECT_DIR 0 /* ������ʻ */
#define CAR_RUN_REVERSE_DIR 1 /* ������ʻ */
#define CAR_RUN_NO_DIR 		2 /* �Ƿ�ͨ��ģʽ */


/* ������Ȧ�ϵ�״̬ */
#define CAR_ENTER_COIL  0 /* ��������Ȧ */
#define CAR_LEAVE_COIL  1 /* ���뿪��Ȧ */

/* ���״̬ */
#define RED_LIGHT_ON    1 		/* ����� */
#define RED_LIGHT_OFF   0 		/* ����� */
#define RED_LIGHT_ORG   0xff 	/* ��Ƴ�ʼ״̬ */

/* ����ͨ�Ƴ�ʼ״̬ */
#define LD_RED_LIGHT_ON    0 		/* ����� */
#define LD_RED_LIGHT_OFF   1 		/* ����� */
#define LD_RED_LIGHT_ORG   0xffff 	/* ��Ƴ�ʼ״̬Ϊ�̵� */
#define LD_PORT_SUM		   0x10		/* ֧�ּ�·���� */

/* ���������Э�鴦�� */
typedef union DEVPROTOCOLUNION
{
    HAGD_PROTO_STR  strHagdProtoCol;     //������Э��
    KJ_PROTO_STR    strKjProtocol;       //�ƿ�Э�飬�ش�Ҳ���ÿƿ���Э���ϱ�
    SJ_PROTO_STR    strSjProtocol;       //�ս�Э��
    CS_PROTO_STR    strCsProtocol;       //�����״�Э��
    SPI_DATA_STRUCT	strSpiProtocol;		 //spiͨ��Э��
    AD_PROTO_STR    strAdProtocol;       //�Ϻ������״�Э��
    LD_PROTO_STR	strLdProtocol;		 //����ͨ�Ƽ����
    HC_PROTO_STR    strHcProtocol;       //�Ϻ��۲��״�Э��
    WK_PROTO_STR    strWKProtocol;       //�Ϸ��Ŀ��״�Э��
    OLVIA_PROTO_STR strolviaProtocol;    //����ά���״�Э��
    CSMC_PROTO_STR  strcsMcProtocol;     //���ٶ೵���״�Э��
    CYLD_PROTO_STR  strCyldProtocol;   	 //��Զ�׵��״�Э��
}DEV_PROTOCOL_SET;


/* ���账��ص��������ݽṹ���� */
typedef struct vehicleCallBackStr
{
    int iComNum;//���ں�
    int iDevType;//�������ͣ���������
    DEV_PROTOCOL_SET strDevProtocolSet;         //����Э�鹲���嶨��
}VEHICLE_CALLBACK_STR;

/* �ⲿ����ص��������Ͷ��� */
/*  VEHICLE_CALLBACK_STR *:���ظ��ص�������Э����Ϣ
 *  int :���˼��������Ľṹ�壬��Ӧ�ⲿ������������һ��ȡֵ1
 */
typedef void(*DEVPROTOPROCFUN)(VEHICLE_CALLBACK_STR *, int);


#endif /* __VEHICLE_BASE_PRO_H__ */
