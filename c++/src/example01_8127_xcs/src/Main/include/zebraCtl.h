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

/* spi通信命令字宏定义 */

/* dm6467往dm368写信息 */
#define Dm6467ToDm368	0

/* dm368往dm6467写信息 */
#define Dm368ToDm6467	1


/* 需要查询的zebra寄存器地址 */
#define CMD_DM6467_QUERY_ZEBRA_REG_ADDR	0x10

/* 获取查询的zebra寄存器内容 */
#define CMD_DM6467_RESP_ZEBRA_REG_VAL	0x11

/* 设置zebra寄存器地址及值 */
#define CMD_DM6467_SET_ZEBRA_REG_ADDR_VAL 0x12

/* 复位Zebra摄像机 */
#define CMD_DM368_RESET_ZEBRA_CAM		0x20

/* 复位Dm368应用程序 */
#define CMD_DM368_RESET_XCS				0x21

/* 复位Dm368板子 */
#define CMD_DM368_RESET_BOARD			0x22

/* 设置dm368调试输出级别 */
#define CMD_DM368_SET_DEBUG_LEVEL		0x30

/* 设置dm368调试输出方式 */
#define CMD_DM368_DEBUG_OUT_MODE		0x31

/* 设置dm368系统时间 */
#define CMD_DM368_SYS_TIME_SET			0x32

/* dm368发送调试信息 */
#define CMD_DM368_SEND_DEBUG_INFO		0x80

/* dm368发送spi总线状态 */
#define CMD_DM368_SEND_SPI_STA			0x81

/* dm6467发送查询spi总线状态 */
#define CMD_DM6467_QUERY_SPI_STA		0x82

/* 前端主动通知后端，需要重新配置摄像机参数,一般在前端异常重启及上电后请求 */
#define CMD_FRONT_REQUEST_SET_CAM		0x83


/* 后端复位前端设备上的pg相机 */
#define CMD_DM6467_REBOOT_PG_CAM		0x8A

/* 后端查询前端设备程序版本号,前端返回版本说明字符串 */
#define CMD_DM6467_QUERY_VERSION		0xF0


#define SPI_DATA_BUF_SIZE				242

typedef struct
{
	unsigned char 	ucStartFlag1;
	unsigned char	ucStartFlag2;
    char   		  	ucProType;  /* 通信类型 */
    unsigned char 	ucProSeq;	/* 通信序号，后端发送，前端原样返回，主要用于后端向前端查询 */
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
