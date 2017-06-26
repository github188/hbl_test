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

//���ڴ�ӡģ�������Ϣ��������ȫ�ֱ���gXcsProtocolDebug��������
#define XCSPTL_ERROR(x...)\
do{\
	if(gXcsProtocolDebug&0x0001)\
		shell_print("PROTO mode:"x);\
}while(0);

//���ڴ�ӡģ�龯����Ϣ��������ȫ�ֱ���gXcsProtocolDebug��������
#define XCSPTL_WARNING(x...)\
do{\
	if(gXcsProtocolDebug&0x0002)\
		shell_print("PROTO mode:"x);\
}while(0);

//���ڴ�ӡģ����ʾ��Ϣ��������ȫ�ֱ���gXcsProtocolDebug��������
#define XCSPTL_INFO(x...)\
do{\
	if(gXcsProtocolDebug&0x0004)\
		shell_print("PROTO mode:"x);\
}while(0);

#endif /* __PROTO_XCS_HANDLE_DEBUG_H_ */

