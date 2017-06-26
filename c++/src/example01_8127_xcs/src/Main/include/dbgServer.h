#ifndef _DBG_SERVER_H
#define _DBG_SERVER_H

#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
const int MAX_DBG_BUF_LEN = 64*1024;
#define DBG_LISTEN_NAME        "/tmp/dbgServer"

class CDbgServer
{
		public :
			CDbgServer();
			~CDbgServer();
			int DbgServerOpen(char *localSockName);
			int DbgServerClose();	
		private:
			static void *DbgThread(void *ptr);
            void v3RespSend(void);
            void v2RespSend(void);
			int m_sockFd;
			bool m_bThreadExit;
			char m_szSockName[128];
			char *m_recvBuf;
			char *m_sendBuf;
			struct sockaddr_un m_dbgAddr;
			struct sockaddr_un m_fromAddr;
			pthread_t m_threadId;
};

#endif

