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

/*串口设备名称最大长度*/
 #define DEVNAMELEN 20 

//控制类型，是根据XCS协议来定义的
#define    DEV_TYPE_HGD	    1        //哈工大车检器
#define    DEV_TYPE_KJ	    2        //科骏车检器
#define    DEV_TYPE_BEST	3        //百思特车检器
#define    DEV_TYPE_SJ	    4        //苏江车检器
#define    DEV_TYPE_CS	    5        //川速雷达
#define    DEV_TYPE_DC      7        //四川地磁设备

#define    DEV_TYPE_SPI     8        //368 SPI通信
#define    DEV_TYPE_AD      9        //安道雷达通信

#define    DEV_TYPE_LD		10		 //联大红灯检测器

#define    DEV_TYPE_HC		11		 //慧昌雷达通信

#define    DEV_TYPE_WK		12		 //文康雷达通信

#define    DEV_TYPE_OLVIA	13		 //奥利维亚雷达通信

#define    DEV_TYPE_CSMC	14		 //川速多车道雷达通信

#define    DEV_TYPE_CY	    15       //藏愚车检器

#define    DEV_TYPE_CYLD    16       //超远雷电雷达

/*参数1:表示第几个通道报警，参数2:表示状态0:消警 1:报警*/
typedef int (* PFUNALARM )(int, int);  

typedef struct
{
	char cComDevName[DEVNAMELEN];   // 外部串口设备名称
} DCTL_COM_INFO;

/* 对DCTL模块设置相应设备个数和对应名称，以后的设备号必须和这里设置的顺序一致 */
typedef struct
{
    unsigned char ucChanNum;            //通道号
    unsigned char ucComNum;             //物理串口设备个数
    unsigned char ucAlarmInNum;         //报警输入设备个数
    unsigned char ucGpioOutNum;         //gpio输出设备个数
    unsigned char ucFlashCtlNum;        //闪光灯控制设备个数
    DCTL_COM_INFO *pComDevName;         //相应串口设备名称
    DCTL_COM_INFO *pAlarmInDevName;     //报警输入设备名称
    DCTL_COM_INFO *pGpioOutDevName;     //gpio输出设备名称
    DCTL_COM_INFO *pFlashCtlDevName;    //闪光灯控制设备名称
}DCTL_MODE_DEV_INFO;


typedef struct tagDCapConf
{
    BOOL  bEnable;			            //是否启用
    unsigned short      usComPort;      //通讯口从0开始，对应实际物理串口
    unsigned short      usCapDevType;   //外部采集设备类型
    DEVPROTOPROCFUN     pProcFunc;      //外部采集设备协议解析后数据上报回调函数
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

