/******************************************************************************
 * vehicleBasePro.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  所有与外设相关的协议都在这里进行整合，具体协议由各自协议头文件定义
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

/* 线圈位置宏定义 */
#define COIL_FRONT_POS  0 /* 表示线圈1 即车最先进入的那个线圈 */
#define COIL_BEHIND_POS 1 /* 表示线圈2 即车最后离开的那个线圈 */

/* 车进入线圈方向宏定义 */
#define CAR_RUN_CORRECT_DIR 0 /* 正向行驶 */
#define CAR_RUN_REVERSE_DIR 1 /* 逆向行驶 */
#define CAR_RUN_NO_DIR 		2 /* 是否通过模式 */


/* 车在线圈上的状态 */
#define CAR_ENTER_COIL  0 /* 车进入线圈 */
#define CAR_LEAVE_COIL  1 /* 车离开线圈 */

/* 红灯状态 */
#define RED_LIGHT_ON    1 		/* 红灯亮 */
#define RED_LIGHT_OFF   0 		/* 红灯灭 */
#define RED_LIGHT_ORG   0xff 	/* 红灯初始状态 */

/* 联大交通灯初始状态 */
#define LD_RED_LIGHT_ON    0 		/* 红灯亮 */
#define LD_RED_LIGHT_OFF   1 		/* 红灯灭 */
#define LD_RED_LIGHT_ORG   0xffff 	/* 红灯初始状态为绿灯 */
#define LD_PORT_SUM		   0x10		/* 支持几路输入 */

/* 各类外设的协议处理集 */
typedef union DEVPROTOCOLUNION
{
    HAGD_PROTO_STR  strHagdProtoCol;     //哈工大协议
    KJ_PROTO_STR    strKjProtocol;       //科骏协议，地磁也采用科骏的协议上报
    SJ_PROTO_STR    strSjProtocol;       //苏江协议
    CS_PROTO_STR    strCsProtocol;       //川速雷达协议
    SPI_DATA_STRUCT	strSpiProtocol;		 //spi通信协议
    AD_PROTO_STR    strAdProtocol;       //上海安道雷达协议
    LD_PROTO_STR	strLdProtocol;		 //联大交通灯检测器
    HC_PROTO_STR    strHcProtocol;       //上海慧昌雷达协议
    WK_PROTO_STR    strWKProtocol;       //合肥文康雷达协议
    OLVIA_PROTO_STR strolviaProtocol;    //奥利维亚雷达协议
    CSMC_PROTO_STR  strcsMcProtocol;     //川速多车道雷达协议
    CYLD_PROTO_STR  strCyldProtocol;   	 //超远雷电雷达协议
}DEV_PROTOCOL_SET;


/* 外设处理回调函数数据结构定义 */
typedef struct vehicleCallBackStr
{
    int iComNum;//串口号
    int iDevType;//外设类型，车检器等
    DEV_PROTOCOL_SET strDevProtocolSet;         //各类协议共用体定义
}VEHICLE_CALLBACK_STR;

/* 外部处理回调函数类型定义 */
/*  VEHICLE_CALLBACK_STR *:返回给回调函数的协议信息
 *  int :给了几个这样的结构体，对应外部几个车道，现一般取值1
 */
typedef void(*DEVPROTOPROCFUN)(VEHICLE_CALLBACK_STR *, int);


#endif /* __VEHICLE_BASE_PRO_H__ */
