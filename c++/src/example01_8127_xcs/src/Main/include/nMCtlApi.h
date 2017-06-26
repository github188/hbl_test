/******************************************************************************
 * nMCtlApi.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.08.4, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _NM_CTL_API_H__
#define _NM_CTL_API_H__

#include "xcsGlobalDef.h"

#define _PACKED_ __attribute__((packed))

/* 网管消息类型ID宏定义 */
#define NM_V3_PLAT_FORM_STA 	1
#define NM_VIDEO_STA 			2
#define NM_RECORD_STA 			3
#define NM_VIR_LOOP_STA			4
#define NM_COIL_LOOP_STA		5
#define NM_SIGNAL_LAMP_STA		6
#define NM_PLATE_LOCATE_STA		7
#define NM_CIFS_NAS_STA		    8
#define NM_RECORD_NAS_STA		9
#define NM_CAM_SYNC_INT_STA		10
#define NM_CAM_TEMPERATURE_STA	11
#define NM_V2_PLAT_FORM_STA 	12

/* 模块状态宏定义 */
#define MODE_OK_STA   		1
#define MODE_ERROR_STA   	-1
#define MODE_SHUTDOWN_STA   0

typedef struct tagNetManageMsg
{
	unsigned int    uiMsgType;      /* 消息类型ID */
    unsigned char   ucChannelId;    /* 通道号 */
    unsigned char   ucCarRoadId;    /* 车道号 */
	unsigned char   ucRsv[2];
	int 			iData[MAX_VEHICLE_NUM];		/* 消息内容 */
}_PACKED_ STR_NET_MANAGE_MSG;

int nMDebugSwitch(int nLevel);
int nMModuleInit(const char *host, unsigned short usPort);
int nMServerInfoSet(const char *host, unsigned short usPort);
int nMMsgSend(int iMsgType, int iChanNum, int iLaneNum, int *piData);
int nMVehicleSumSet(unsigned char ucMaxVehiSum);

#endif /* _NM_CTL_API_H__ */


