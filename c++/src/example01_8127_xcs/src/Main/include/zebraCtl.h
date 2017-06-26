#ifndef _ZEBRA_CTL_H
#define _ZEBRA_CTL_H

#include "McLog.h"

#ifdef __cplusplus
extern "C" {
#endif


extern int g_zebra_debug_module;


#define ZEBRACTL_DEBUG(x...)\
do{\
	if(g_zebra_debug_module)\
		shell_print("zebraCtl mode:"x);\
}while(0)


#define ZEBRACTL_ERROR(x...)\
do{\
	if(g_zebra_debug_module&0x0001)\
		shell_print("zebraCtl mode:"x);\
}while(0)

#define ZEBRACTL_WARNING(x...)\
do{\
	if(g_zebra_debug_module&0x0002)\
		shell_print("zebraCtl mode:"x);\
}while(0)

#define ZEBRACTL_INFO(x...)\
do{\
	if(g_zebra_debug_module&0x0004)\
		shell_print("zebraCtl mode:"x);\
}while(0)

/* spiͨ�������ֺ궨�� */

/* dm6467��dm368д��Ϣ */
#define Dm6467ToDm368	0

/* dm368��dm6467д��Ϣ */
#define Dm368ToDm6467	1


/* ��Ҫ��ѯ��zebra�Ĵ�����ַ */
#define CMD_DM6467_QUERY_ZEBRA_REG_ADDR	0x10

/* ��ȡ��ѯ��zebra�Ĵ������� */
#define CMD_DM6467_RESP_ZEBRA_REG_VAL	0x11

/* ����zebra�Ĵ�����ַ��ֵ */
#define CMD_DM6467_SET_ZEBRA_REG_ADDR_VAL 0x12

/* ��λZebra����� */
#define CMD_DM368_RESET_ZEBRA_CAM		0x20

/* ��λDm368Ӧ�ó��� */
#define CMD_DM368_RESET_XCS				0x21

/* ��λDm368���� */
#define CMD_DM368_RESET_BOARD			0x22

/* ����dm368����������� */
#define CMD_DM368_SET_DEBUG_LEVEL		0x30

/* ����dm368���������ʽ */
#define CMD_DM368_DEBUG_OUT_MODE		0x31

/* ����dm368ϵͳʱ�� */
#define CMD_DM368_SYS_TIME_SET			0x32

/* dm368���͵�����Ϣ */
#define CMD_DM368_SEND_DEBUG_INFO		0x80

/* dm368����spi����״̬ */
#define CMD_DM368_SEND_SPI_STA			0x81

/* dm6467���Ͳ�ѯspi����״̬ */
#define CMD_DM6467_QUERY_SPI_STA		0x82

/* ǰ������֪ͨ��ˣ���Ҫ�����������������,һ����ǰ���쳣�������ϵ������ */
#define CMD_FRONT_REQUEST_SET_CAM		0x83


/* ��˸�λǰ���豸�ϵ�pg��� */
#define CMD_DM6467_REBOOT_PG_CAM		0x8A

/* ��˲�ѯǰ���豸����汾��,ǰ�˷��ذ汾˵���ַ��� */
#define CMD_DM6467_QUERY_VERSION		0xF0


#define SPI_DATA_BUF_SIZE				242

typedef struct
{
	unsigned char 	ucStartFlag1;
	unsigned char	ucStartFlag2;
    char   		  	ucProType;  /* ͨ������ */
    unsigned char 	ucProSeq;	/* ͨ����ţ���˷��ͣ�ǰ��ԭ�����أ���Ҫ���ں����ǰ�˲�ѯ */
    char   			ucData[SPI_DATA_BUF_SIZE];
	unsigned char ucEndFlag1;
	unsigned char ucEndFlag2;
}spi_param_t;


typedef struct tagZebraRegVal
{
    char aRegAdd[4];
    char aRegVal[8];
    char cRsv;
    unsigned char ucProSeq;
}STRZEBRAREGVAL;

int zebraCtlDebugSwitch(int level);
int zebraCtlRegValSet(int iAdd, unsigned int uiVal);
int zebraCtlRegValGet(int iAddr, unsigned int *puiVal);
int dm368DebugLevelSet(char cLevel);
int dm368SysTimeSet(unsigned int uiSecs);
int dm368SpiBusStaQuery(void);
int dm368PGCamReboot(void);
int dm368ProgramVerQuery(void);

#ifdef __cplusplus
}
#endif


#endif /* _ZEBRA_CTL_H */
