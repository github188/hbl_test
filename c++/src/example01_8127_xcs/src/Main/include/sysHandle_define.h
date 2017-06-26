/******************************************************************************
 * sysHandle_define.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.28, xcsworker Create
 * --------------------
 ******************************************************************************/
#ifndef _SYS_HANDLE_DEFINE_H__
#define _SYS_HANDLE_DEFINE_H__

#include "McLog.h"

extern int g_SysHandle_Debug;
extern int g_Recv_Debug;
//#define SYSHANDLE_MODULE_DEBUG

#ifdef SYSHANDLE_MODULE_DEBUG
	#define SYSHANDLE_DEBUG(x...)\
	do{\
		if(eg_SysHandle_Debug)\
			shell_print(x);\
	}while(0)
#else
	#define SYSHANDLE_DEBUG(x...)
#endif

		//shell_print("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);\

//用于打印模块错误信息，可以用全局变量g_SysHandle_Debug单独控制
#define SYSHANDLE_ERROR(x...)\
do{\
	if(g_SysHandle_Debug&0x0001)\
	{\
		shell_print("sysHandle mod err:"x);\
	}\
}while(0)

//用于打印模块警告信息，可以用全局变量g_SysHandle_Debug单独控制
#define SYSHANDLE_WARNING(x...)\
do{\
	if(g_SysHandle_Debug&0x0002)\
	{\
		shell_print("sysHandle mod warn:"x);\
	}\
}while(0)

//用于打印模块提示信息，可以用全局变量g_SysHandle_Debug单独控制
#define SYSHANDLE_INFO(x...)\
do{\
	if(g_SysHandle_Debug&0x0004)\
	{\
		shell_print("sysHandle mod info:"x);\
	}\
}while(0)

#endif
