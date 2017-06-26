/******************************************************************************
 * SysHandle.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.18, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DCTL_VEHICLE_PRO_THREAD_H
#define _DCTL_VEHICLE_PRO_THREAD_H


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/sem.h>

#include "McLog.h"

#include "SysHandle.h"
#include "sysHandle_define.h"

#include "dspParamManage.h"


#define MSG_KJ_VEHI 	0
#define MSG_SJ_VEHI 	1
#define MSG_HGD_VEHI 	2
#define MSG_DC_VEHI 	3

typedef struct tag_VehiInfoMsgStr
{
	unsigned int    		uiMsgType;    	/* 消息类型 */
	VEHICLE_CALLBACK_STR 	stVehicleInfo;	/* 车检器信息 */
}VEHI_PRO_INFO_MSG;

int sendMsgToVehiNormalThread(int iMsgType, VEHICLE_CALLBACK_STR *pstVehicle);
int sendMsgToVehiPPThread(int iMsgType, VEHICLE_CALLBACK_STR *pstVehicle);
int dctlThreadProInit(void);

#endif

