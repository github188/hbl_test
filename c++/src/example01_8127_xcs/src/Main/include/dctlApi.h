/******************************************************************************
 * dctlApi.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.04.15, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_DCTL_API_H__
#define _DEV_DCTL_API_H__

#include "vehicleBasePro.h"
#include "Ieee1394.h"

#ifndef BOOL
#define BOOL unsigned int
#endif

/*�����豸������󳤶�*/
 #define DEVNAMELEN 20 

//�������ͣ��Ǹ���XCSЭ���������
#define    DEV_TYPE_HGD	    1        //�����󳵼���
#define    DEV_TYPE_KJ	    2        //�ƿ�������
#define    DEV_TYPE_BEST	3        //��˼�س�����
#define    DEV_TYPE_SJ	    4        //�ս�������
#define    DEV_TYPE_CS	    5        //�����״�
#define    DEV_TYPE_DC      7        //�Ĵ��ش��豸

#define    DEV_TYPE_SPI     8        //368 SPIͨ��
#define    DEV_TYPE_AD      9        //�����״�ͨ��

#define    DEV_TYPE_LD		10		 //�����Ƽ����

#define    DEV_TYPE_HC		11		 //�۲��״�ͨ��

#define    DEV_TYPE_WK		12		 //�Ŀ��״�ͨ��

#define    DEV_TYPE_OLVIA	13		 //����ά���״�ͨ��

#define    DEV_TYPE_CSMC	14		 //���ٶ೵���״�ͨ��

#define    DEV_TYPE_CY	    15       //���޳�����

#define    DEV_TYPE_CYLD    16       //��Զ�׵��״�

/*����1:��ʾ�ڼ���ͨ������������2:��ʾ״̬0:���� 1:����*/
typedef int (* PFUNALARM )(int, int);  

typedef struct
{
	char cComDevName[DEVNAMELEN];   // �ⲿ�����豸����
} DCTL_COM_INFO;

/* ��DCTLģ��������Ӧ�豸�����Ͷ�Ӧ���ƣ��Ժ���豸�ű�����������õ�˳��һ�� */
typedef struct
{
    unsigned char ucChanNum;            //ͨ����
    unsigned char ucComNum;             //�������豸����
    unsigned char ucAlarmInNum;         //���������豸����
    unsigned char ucGpioOutNum;         //gpio����豸����
    unsigned char ucFlashCtlNum;        //����ƿ����豸����
    DCTL_COM_INFO *pComDevName;         //��Ӧ�����豸����
    DCTL_COM_INFO *pAlarmInDevName;     //���������豸����
    DCTL_COM_INFO *pGpioOutDevName;     //gpio����豸����
    DCTL_COM_INFO *pFlashCtlDevName;    //����ƿ����豸����
}DCTL_MODE_DEV_INFO;


typedef struct tagDCapConf
{
    BOOL  bEnable;			            //�Ƿ�����
    unsigned short      usComPort;      //ͨѶ�ڴ�0��ʼ����Ӧʵ��������
    unsigned short      usCapDevType;   //�ⲿ�ɼ��豸����
    DEVPROTOPROCFUN     pProcFunc;      //�ⲿ�ɼ��豸Э������������ϱ��ص�����
}DCTL_CAP_COM_CONF;

int DctlDebugSwitch(int nLevel);

int dctlModuleInit(DCTL_MODE_DEV_INFO *pstrDevInfo);
int dCtlModuleCapConf(DCTL_CAP_COM_CONF * pstrCapComParam, int iNum);
int dCtlModuleCapWirte(char cDevType, char *szBuf, int iLen);
int dctlModuleCapDel();
int dCtlModuleAlarmInConf(PFUNALARM pAlarmProcFun);
int dCtlModuleFlashCtlConf(void);
int dctlModuleFlashCtlInfo(FLASH_CONF_INFO *pstrFlashConfInfo);
int dctlModuleFlashSnapSeq(SNAP_INFO *pstrSnapInfo);
int dctlModuleFlashForceSnap(SNAP_INFO *pstrSnapInfo);
int dctlModuleFlashIntNumGet(unsigned int *puiNum);
int dctlModuleTtlFlashCtlInfo(TTL_FLASH_CONF_INFO *pstrTtlFlashConfInfo);

#endif /* _DEV_DCTL_API_H__ */

