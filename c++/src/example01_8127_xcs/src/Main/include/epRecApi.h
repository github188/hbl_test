/******************************************************************************
 * epRecApi.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.09.25, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _EP_REC_API_H__
#define _EP_REC_API_H__

#include "xcsCommDef.h"

/* �ⲿ����ص��������Ͷ��� */
/*  char *:���ظ��ص������Ķ�ʱ¼���ļ�����·����
 */
typedef void(*TIMERECFILEFUN)(char *);

#define _PACKED_ __attribute__((packed))

/* ¼�����ͺ궨�� */
#define EP_REC_TYPE 	1	/* ����¼�� */

#define EP_REC_STATE_BEGIN	1	/* ��ʼ¼�� */
#define EP_REC_STATE_END	0	/* ֹͣ¼�� */

#define MAX_PATH_NAME_LEN (128)

/* ÿ������¼���ļ����ʱ������(��λ��) */
#define ALARM_FILE_MAX_TIME  (12)
/* ÿ������¼���ļ��������(��λB)������libmemasf������֧��5M,���Բ����ټӴ��ˣ�����ftpģ��
   ���ֻ֧��4M��С���ļ������Ի����޶���4M */
#define ALARM_FILE_MAX_SIZE  (4000*1024)

/* ������Ϣ�е����ͺ궨�� */
#define VIRTUAL_MSG_TYPE_ID            2       /* ������Ȧ¼����ϢID */
#define GROUND_MSG_TYPE_ID             3       /* �ظ���Ȧ¼����ϢID */
#define EP_REC_OVER_MSG_TYPE_ID        5       /* ����¼��ʱ��ﵽ���ֵ������¼���ļ����ͳ�ȥ */

typedef struct tagEpRecMsgStr
{
	unsigned int    uiMsgType;                 /* ��Ϣ���� */
    unsigned char   ucChannelId;               /* ͨ���� */
    unsigned char   ucRecSta;				   /* ¼������״̬ */
	unsigned char   ucRsv[2];
}_PACKED_ STR_EP_REC_CTL_MSG;

int epRecDebugSwitch(int nLevel);

int epRecModuleInit();
int epRecModuleUnInit();

int epRecModuleFileNameSet(int iRecType, const char *sFileName);
int epRecModuleFileNameGet(int iRecType, char *sFileName);
int epRecModuleFileExitStaSet(int iRecType, int iSta);
int epRecModuleFileExitStaGet(int iRecType, int *piSta);

int epRecModuleFrameInput(XCS_FRAME * pFrame, int iLen);
int epRecModuleAlarmRecCallBackFunSet(TIMERECFILEFUN alarmRecFileFunc);
int epRecModuleMsgSend(unsigned int uiMsgType, unsigned char ucData);
int epRecModuleRecStaSet(int iRecType, int iSta, unsigned char ucLaneId);
int epRecModuleRecStaGet(int iRecType, int *piSta, unsigned char ucLaneId);

int epRecModuleMemFileRead(int iRecType, void ** buf);
int epRecModuleMemFileReadEnd(int iRecType);

#endif /* _EP_REC_API_H__ */

