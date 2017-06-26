#ifndef _NET_RECV_DEFINE_H__
#define _NET_RECV_DEFINE_H__

extern int g_Recv_Debug;

#include "McLog.h"

#define RECV_MODULE_DEBUG 1

#ifdef RECV_MODULE_DEBUG

	#define RECV_DEBUG(x...)\
	do{\
		if(g_Recv_Debug)\
			shell_print(x);\
	}while(0);

#else
	#define RECV_DEBUG(x...)
#endif

#define RECV_ERROR(x...)\
do{\
	if(g_Recv_Debug&0x0001)\
		shell_print("TCP SERVER mode:"x);\
}while(0);

#define RECV_WARNING(x...)\
do{\
	if(g_Recv_Debug&0x0002)\
		shell_print("TCP SERVER mode:"x);\
}while(0);

#define RECV_INFO(x...)\
do{\
	if(g_Recv_Debug&0x0004)\
		shell_print("TCP SERVER mode:"x);\
}while(0);

#define TCP_RECV_BUF_LEN 	(64*1024)
#define TCP_SEND_BUF_LEN 	(64*1024)

#define TCP_BUF_LEN 		(64*1024)
#define TCP_CLIENT_NUM		(5)
#define TCP_TTL_TIMEOUT	    (60*1000)//60ms 以便于在网络掉线后客户端能够及时连接上

#endif

