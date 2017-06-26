
#ifndef _TCP_SERVER_H__
#define _TCP_SERVER_H__

#include <pthread.h>
#include <semaphore.h>
#include "msgQLib.h"
#include "protoXHandle.h"

#define  EVENT_EXIT			(0x10)
#define  EVENT_START		(0x20)
#define  EVENT_END			(0x40)
#define  EVENT_READY		(0x80)
#define  EVENT_DISCONN		(0x100)
#define  EVENT_HEART		(0x200)

class CTcpServer
{
		public :
			CTcpServer();
			~CTcpServer();
			
			int TcpServerOpen(unsigned short nListenPort,int nClientNum,MSG_Q_ID msgQ);
			int TcpServerClose();

			int TcpClientSendSyncData(XCS_CLIENT* pDvsClient);
			int TcpClientSendAsyncData(char* pBuf,int nLen);
			
			
		private:
			
			int TcpSendData(char *pData,int nLen);
			int StartTcpService();
			int StopTcpService();
			int AddNewClient();
			static void *ServerThreadFunc(void *pArg);
			static void *UserThreadFunc(void *pArg);
			
			int m_sockServer;
			char m_szServerIp[16];
			unsigned short m_nListenPort;
			pthread_t m_threadTcpServer;
			sem_t		m_semTcpServer;
			int m_bServerThreadQuit;
			
			pthread_t m_threadTcpClient;
			int m_nClientNum;
			XCS_CLIENT *m_pXcsClient;
			
};

#endif

