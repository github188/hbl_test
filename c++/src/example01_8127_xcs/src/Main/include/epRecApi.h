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

/* 外部处理回调函数类型定义 */
/*  char *:返回给回调函数的定时录像文件绝对路径名
 */
typedef void(*TIMERECFILEFUN)(char *);

#define _PACKED_ __attribute__((packed))

/* 录像类型宏定义 */
#define EP_REC_TYPE 	1	/* 报警录像 */

#define EP_REC_STATE_BEGIN	1	/* 开始录像 */
#define EP_REC_STATE_END	0	/* 停止录像 */

#define MAX_PATH_NAME_LEN (128)

/* 每个报警录像文件最大时长限制(单位秒) */
#define ALARM_FILE_MAX_TIME  (12)
/* 每个报警录像文件最大限制(单位B)，由于libmemasf库最大就支持5M,所以不能再加大了，并且ftp模块
   最大只支持4M大小的文件，所以还是限定到4M */
#define ALARM_FILE_MAX_SIZE  (4000*1024)

/* 发送消息中的类型宏定义 */
#define VIRTUAL_MSG_TYPE_ID            2       /* 虚拟线圈录像消息ID */
#define GROUND_MSG_TYPE_ID             3       /* 地感线圈录像消息ID */
#define EP_REC_OVER_MSG_TYPE_ID        5       /* 报警录像超时或达到最大值，即把录像文件发送出去 */

typedef struct tagEpRecMsgStr
{
	unsigned int    uiMsgType;                 /* 消息类型 */
    unsigned char   ucChannelId;               /* 通道号 */
    unsigned char   ucRecSta;				   /* 录像启动状态 */
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

