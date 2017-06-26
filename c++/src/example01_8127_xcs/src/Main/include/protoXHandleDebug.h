/******************************************************************************
 * protoXHandleDebug.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.09, wangbin Create
 * --------------------
 ******************************************************************************/

#ifndef __PROTO_XCS_HANDLE_DEBUG_H_
#define __PROTO_XCS_HANDLE_DEBUG_H_

extern int gXcsProtocolDebug;

#include "McLog.h"

#ifdef XCSPTL_MODULE_DEBUG
	#define XCSPTL_DEBUG(x...)\
	do{\
		if(gXcsProtocolDebug)\
			shell_print("PROTO mode:"x);\
	}while(0);
#else
	#define XCSPTL_DEBUG(x...)
#endif

//用于打印模块错误信息，可以用全局变量gXcsProtocolDebug单独控制
#define XCSPTL_ERROR(x...)\
do{\
	if(gXcsProtocolDebug&0x0001)\
		shell_print("PROTO mode:"x);\
}while(0);

//用于打印模块警告信息，可以用全局变量gXcsProtocolDebug单独控制
#define XCSPTL_WARNING(x...)\
do{\
	if(gXcsProtocolDebug&0x0002)\
		shell_print("PROTO mode:"x);\
}while(0);

//用于打印模块提示信息，可以用全局变量gXcsProtocolDebug单独控制
#define XCSPTL_INFO(x...)\
do{\
	if(gXcsProtocolDebug&0x0004)\
		shell_print("PROTO mode:"x);\
}while(0);

#endif /* __PROTO_XCS_HANDLE_DEBUG_H_ */

