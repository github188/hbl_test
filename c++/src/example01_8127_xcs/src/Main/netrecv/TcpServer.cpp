/******************************************************************************
 * TcpServer.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.03, kj Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "net_recv_define.h"
#include "TcpServer.h"
#include "SysHandle.h"

CTcpServer::CTcpServer()
{
	m_sockServer = -1;
	memset(m_szServerIp,0x00,sizeof(m_szServerIp));
	m_nListenPort = 0;
	m_threadTcpServer = 0;
	m_bServerThreadQuit = 1;
			
	m_threadTcpClient = 0;
	m_nClientNum = 0;
	m_pXcsClient = NULL;
	
	int nRet = 0;
	nRet = sem_init(&m_semTcpServer,0,1);
	if(0!=nRet)
		RECV_DEBUG("%s:%d sem_init() failed.\n",__FUNCTION__,__LINE__);
}

CTcpServer::~CTcpServer()
{
    int i =0;
	int nRet = 0;
	int nEvent = 0;
	
	StopTcpService();
	
	if(m_pXcsClient)
	{
		for(i=0; i<m_nClientNum; i++)
		{
			if(-1!=m_pXcsClient[i].sockClient)
			{
				nEvent = EVENT_DISCONN;
				nRet = msgQSend(m_pXcsClient[i].msgQEvent, (char *)(&nEvent), sizeof(int), 0);
				if( sizeof(int) != nRet )
					RECV_DEBUG("tcpServer disconn msgQSend() failed.\n");
			}
		}
		
		usleep(200*1000);
		
		for(i=0; i<m_nClientNum; i++)
		{
			if( NULL != m_pXcsClient[i].msgQEvent )
			{
				nEvent = EVENT_EXIT;
				nRet = msgQSend(m_pXcsClient[i].msgQEvent,(char *)&nEvent,sizeof(int),0);
				if(sizeof(int)!=nRet)
					RECV_DEBUG("tcpServer exit msgQSend() failed.\n");
			}
		}
		
		for(i=0; i<m_nClientNum; i++)
		{
			if( 0 != m_pXcsClient[i].threadTcpClinet )
			{
				void *thread_result = NULL;
				pthread_join( m_pXcsClient[i].threadTcpClinet, &thread_result );
				m_pXcsClient[i].threadTcpClinet = 0;
			}
		}
		
		for(i=0; i<m_nClientNum; i++)
		{
			if( -1!=m_pXcsClient[i].sockClient )
			{
				nRet = shutdown(m_pXcsClient[i].sockClient, 2);
				if(0 > nRet)
				{
				    RECV_DEBUG("shutdown %d socket failed.\n", m_pXcsClient[i].sockClient);
				}
				nRet = close(m_pXcsClient[i].sockClient);
				if(0 > nRet)
				{
				    RECV_DEBUG("close %d socket failed.\n", m_pXcsClient[i].sockClient);
				}
				m_pXcsClient[i].sockClient = -1;
			}
			
			if(NULL != m_pXcsClient[i].msgQEvent)
			{
				msgQDelete( m_pXcsClient[i].msgQEvent );
				m_pXcsClient[i].msgQEvent = NULL;
			}
			
			if(m_pXcsClient[i].pRecvBuf)
			{
				delete [] m_pXcsClient[i].pRecvBuf;
				m_pXcsClient[i].pRecvBuf = NULL;
			}
			
			if(m_pXcsClient[i].pSendBuf)
			{
				delete [] m_pXcsClient[i].pSendBuf;
				m_pXcsClient[i].pSendBuf = NULL;
			}
            sem_destroy(&m_pXcsClient[i].sem);
		}
		
		delete [] m_pXcsClient;
		m_pXcsClient = NULL;
	}	
	
	sem_wait(&m_semTcpServer);
	m_bServerThreadQuit = 1;
	sem_post(&m_semTcpServer);
	
	if(m_threadTcpServer)
	{
		void *thread_result = NULL;
		pthread_join(m_threadTcpServer,&thread_result);
		m_threadTcpServer = 0;
	}
	
	sem_destroy(&m_semTcpServer);
}

int CTcpServer::TcpServerOpen(unsigned short nListenPort,int nClientNum,MSG_Q_ID msgQ)
{
	int nRet = 0;

	if( -1!=m_sockServer )
	{
		RECV_DEBUG("TcpServer socke have opened\n");
		return (-1);
	}
	
	if(( nListenPort < 1 )||( nClientNum < 1 )||( nClientNum > TCP_CLIENT_NUM ) )//||(nListenPort>65535))
	{
		RECV_DEBUG("TcpServer nListenPort:%d param error.\n", nListenPort);
		return (-1);
	}
	
	if( m_nClientNum && ( m_nClientNum != nClientNum ) )
	{
		RECV_DEBUG("TcpServer nClientNum:%d param error.\n", nClientNum);
		return (-1);
	}

    if (msgQ == NULL)
    {
		RECV_DEBUG("TcpServer msgQ is null,param error.\n");
		return (-1);
    }
	
	m_nClientNum = nClientNum;
	m_nListenPort = nListenPort;
	
	m_pXcsClient = new XCS_CLIENT[m_nClientNum];
	if(NULL==m_pXcsClient)
	{
		RECV_DEBUG("new() error.\n");
		goto error_ret;
	}
	
	memset(m_pXcsClient,0x00,sizeof(XCS_CLIENT)*m_nClientNum);

    
	for(int i=0; i< m_nClientNum; i++)
	{
		if( -1 != m_pXcsClient[i].sockClient )
			close(m_pXcsClient[i].sockClient);
			
		m_pXcsClient[i].sockClient = -1;
		m_pXcsClient[i].sin_addr = 0;
		m_pXcsClient[i].sin_port = 0;
		if(NULL==m_pXcsClient[i].pRecvBuf)
		{
			m_pXcsClient[i].pRecvBuf = (char *)(new int[TCP_RECV_BUF_LEN/4]);
			if(NULL == m_pXcsClient[i].pRecvBuf)
			{
			    RECV_DEBUG("new pRecvBuf error.\n");
        		goto error_ret;
			}
		}
		m_pXcsClient[i].nRecvBufLen = TCP_RECV_BUF_LEN;
		m_pXcsClient[i].nRecvLen = 0;
		m_pXcsClient[i].nPackLen = 0;
		if(NULL==m_pXcsClient[i].pSendBuf)
		{
			m_pXcsClient[i].pSendBuf = (char *)(new int[TCP_SEND_BUF_LEN/4]);
			if(NULL == m_pXcsClient[i].pRecvBuf)
			{
			    RECV_DEBUG("new pSendBuf error.\n");
        		goto error_ret;
			}
		}
		m_pXcsClient[i].nSendBufLen = TCP_SEND_BUF_LEN;
		m_pXcsClient[i].nSendLen = NT_PACK_HEAD_LENGTH;
		m_pXcsClient[i].nTTL = 0;
        m_pXcsClient[i].msgQOutput = msgQ;
		
		if( NULL == m_pXcsClient[i].msgQEvent )
		{
			m_pXcsClient[i].msgQEvent = msgQCreate(16,sizeof(int),0);
			if(NULL == m_pXcsClient[i].msgQEvent)
			{
			    RECV_DEBUG("create msgQ error.\n");
        		goto error_ret;
			}
        }

        sem_init(&m_pXcsClient[i].sem, 0, 1);


        
		m_pXcsClient[i].param.nIndex = i;
		m_pXcsClient[i].param.pArg = (void *)this;
		if( 0 == m_pXcsClient[i].threadTcpClinet )
			nRet = pthread_create(&m_pXcsClient[i].threadTcpClinet,NULL,CTcpServer::UserThreadFunc,(void *)&m_pXcsClient[i].param);
			
		if( 0 == m_pXcsClient[i].threadTcpClinet )
		{
			RECV_DEBUG("client pthread_create() error.\n");
			goto error_ret;
		}
	}
	
	if( 0==m_threadTcpServer )
	{
		sem_wait(&m_semTcpServer);
		m_bServerThreadQuit = 0;
		sem_post(&m_semTcpServer);

        nRet = StartTcpService();
        if(nRet)
    	{
    		RECV_ERROR("ERROR:CTcpServer::TcpServerOpen() StartTcpService failed.\n");
    		goto error_ret;
    	}		

		nRet = pthread_create(&m_threadTcpServer, NULL, CTcpServer::ServerThreadFunc, (void*)this);
		if(0 != nRet)
		{
			RECV_DEBUG("server pthread_create() error.\n");
			goto error_ret;
		}
	}

	return 0;
	
error_ret:
		
	m_nListenPort = 0;
	
	return (-1);
}

int CTcpServer::TcpServerClose()
{
    int i =0;
	int nRet = 0;
	int nEvent = 0;
	
	StopTcpService();
	
	if(m_pXcsClient)
	{
		for(i=0; i<m_nClientNum; i++)
		{	
			if( -1 != m_pXcsClient[i].sockClient )
			{
				nEvent = EVENT_DISCONN;
				nRet = msgQSend(m_pXcsClient[i].msgQEvent, (char *)(&nEvent), sizeof(int), 0);
				if( sizeof(int) != nRet )
					RECV_ERROR("tcpServer close msgQSend() failed.\n");
			}
		}
		
		usleep(200*1000);
		
		for(i=0; i<m_nClientNum; i++)
		{
			if(-1!=m_pXcsClient[i].sockClient)
			{
				shutdown(m_pXcsClient[i].sockClient, 2);
				close(m_pXcsClient[i].sockClient);
				m_pXcsClient[i].sockClient = -1;
			}
		}
	}
	return 0;
}

int CTcpServer::StartTcpService()
{
	int nRet;
	int bFlag = 0;
	
	StopTcpService();

	sem_wait(&m_semTcpServer);
	
	// 1. create tcp socket
	if( -1 == m_sockServer )
	{
		m_sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

		if(-1==m_sockServer)
		{
			RECV_DEBUG("creat socket() failed.\n");
			goto error_ret;
		}
	}

	// 2. set socket option SO_REUSEADDR to reuse listen port
	bFlag = 1;
	nRet = setsockopt(	m_sockServer,                           // socket
											SOL_SOCKET,			// socket level
											SO_REUSEADDR,		// socket option
											(char *)(&bFlag),   // option value
											sizeof (bFlag));	// size of value
	if(nRet != 0)
		goto error_ret;

	// 3. set socket option SO_KEEPALIVE to enable the use of keep-alive packets on TCP connections 
	bFlag = 1;
	nRet = setsockopt(	m_sockServer,                           // socket 
											SOL_SOCKET,			// socket level 
											SO_KEEPALIVE,		// socket option 
											(char *)(&bFlag),   // option value 
											sizeof (bFlag));    // size of value 
	if (nRet != 0)
		goto error_ret;

	// 4. set socket option SO_LINGER to controls the action taken when unsent data is queued on a socket
	struct linger   ltemp;
	ltemp.l_onoff = 1;
	ltemp.l_linger = 0;
	nRet = setsockopt(	m_sockServer,                               // socket
											SOL_SOCKET,			    // socket level
											SO_LINGER,			    // socket option
											(char *)(&ltemp),       // option value
											sizeof (struct linger));// size of value
	if (nRet != 0)
		goto error_ret;

	// 5. set socket to no-blocking mode
	bFlag = fcntl(m_sockServer, F_GETFL, 0);
	nRet = fcntl(m_sockServer, F_SETFL, O_NONBLOCK|bFlag);
	if (nRet != 0)
		goto error_ret;

	// 6. init this data of struct sockaddr_in
	sockaddr_in  addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(m_nListenPort);
	
	// 7. tcp bind port
	nRet = bind(m_sockServer, (struct sockaddr *)&addr, sizeof(addr));
	if(nRet!=0)
	{
		RECV_DEBUG("server socket bind() failed.\n");
		goto error_ret;
	}

	// 8. listen tcp connect
	nRet = listen(m_sockServer, TCP_CLIENT_NUM);
	if(nRet < 0)
	{
		RECV_DEBUG("server socket listen() failed.\n");
		goto error_ret;
	}
	
	sem_post(&m_semTcpServer);
	
	RECV_INFO("INFO:Tcp Server listen at port(%d) ...\n",m_nListenPort);
	
	return 0;
	
	error_ret:
		if(-1!=m_sockServer)
		{
			close(m_sockServer);
			m_sockServer = -1;
		}
		
		sem_post(&m_semTcpServer);

		return (-1);
}

int CTcpServer::StopTcpService()
{
    int ret =-1;
	if(-1!=m_sockServer)
	{
		sem_wait(&m_semTcpServer);
		ret = shutdown(m_sockServer, 2);
        if( 0 > ret)
		{
		    RECV_INFO("shutdown %d socket failed\n", m_sockServer);
    		sem_post(&m_semTcpServer);
		    return -1;
		}
		ret = close(m_sockServer);
		if( 0 > ret)
		{
		    RECV_INFO("close %d socket failed\n", m_sockServer);
    		sem_post(&m_semTcpServer);
		    return -1;
		}
		m_sockServer = -1;
		sem_post(&m_semTcpServer);
		
		RECV_INFO("INFO:Tcp Server stop listen at port(%d) ...\n",m_nListenPort);
	}
	
	return 0;
}

int CTcpServer::AddNewClient()
{
	int newClientSock;
	sockaddr_in newAddr;
	int nAddrSize;
	int bFlag, nRet,i;
	int nEvent = 0;

	if( (-1==m_sockServer)||(NULL==m_pXcsClient) )
		return (-1);

	// 1. accept
	nAddrSize = sizeof(sockaddr_in);
	newClientSock = accept(m_sockServer, (struct sockaddr *)&newAddr, (socklen_t*)&nAddrSize);
	if (-1==newClientSock)
	{
		RECV_DEBUG("%s:%d accept() failed.\n",__FUNCTION__,__LINE__);
		goto error_ret;
	}

	// 2. set socket option SO_KEEPALIVE to enable the use of keep-alive packets on TCP connections 
	bFlag = 1;
	nRet = setsockopt(	newClientSock,                      // socket 
											SOL_SOCKET,     // socket level 
											SO_KEEPALIVE,   // socket option 
											(char *)&bFlag, // option value 
											sizeof (bFlag));// size of value 
	if (nRet != 0)
		goto error_ret;

	// 3. set socket option SO_LINGER to controls the action taken when unsent data is queued on a socket
	struct linger   ltemp;
	ltemp.l_onoff = 1;
	ltemp.l_linger = 0;
	nRet = setsockopt( newClientSock,                               // socket
											SOL_SOCKET,             // socket level
											SO_LINGER,              // socket option
											(char *)&ltemp,         //option value
											sizeof (struct linger));// size of value
	if (nRet != 0)
		goto error_ret;
			
	// 4. set socket option SO_SNDBUF to modify send buffer length
	bFlag = TCP_BUF_LEN;
	nRet = setsockopt(	newClientSock,                      // socket 
											SOL_SOCKET,     // socket level 
											SO_SNDBUF,      // socket option 
											(char *)&bFlag, // option value 
											sizeof (bFlag));// size of value 
	if (nRet != 0)
		goto error_ret;
	
	// 5. set socket option SO_RCVBUF to modify recv buffer length
	bFlag = 2*TCP_BUF_LEN;
	nRet = setsockopt(	newClientSock,                      // socket 
											SOL_SOCKET,     // socket level 
											SO_RCVBUF,      // socket option 
											(char *)&bFlag, // option value 
											sizeof (bFlag));// size of value 
	if (nRet != 0)
		goto error_ret;	
			
	// 6. Disables the Nagle algorithm for send coalescing
	bFlag = 1;
	nRet = setsockopt(	newClientSock,                      // socket 
											IPPROTO_TCP,    // TCP level 
											TCP_NODELAY,    // socket option 
											(char *)&bFlag, // option value 
											sizeof (bFlag));// size of value 
	if (nRet != 0)
		goto error_ret;

	// 7. set socket to no-blocking mode
	bFlag = fcntl(newClientSock, F_GETFL, 0);
	nRet = fcntl(newClientSock, F_SETFL, O_NONBLOCK|bFlag);
	if (nRet != 0)
		goto error_ret;

	// 8. find a idle tcp client
	for (i=0; i<m_nClientNum; i++)
	{
		if (-1==m_pXcsClient[i].sockClient)
		{
			m_pXcsClient[i].sockClient = newClientSock;
			m_pXcsClient[i].sin_addr = newAddr.sin_addr.s_addr;
			m_pXcsClient[i].sin_port = newAddr.sin_port;
			m_pXcsClient[i].nRecvLen = 0;
			m_pXcsClient[i].nPackLen = 0;
			m_pXcsClient[i].nSendLen = NT_PACK_HEAD_LENGTH;
			m_pXcsClient[i].nTTL = TCP_TTL_TIMEOUT;
			m_pXcsClient[i].uiHeartBit= 2;
			m_pXcsClient[i].uiHBFlag= 0;
			break;
		}
	}

	if(i==m_nClientNum)
	{
		RECV_WARNING("WARNING:Tcp Server reach the max(%d) tcp client.num is %d.\n",m_nClientNum, i);
		goto error_ret;
	}

	nEvent = EVENT_START;
	nRet = msgQSend(m_pXcsClient[i].msgQEvent, (char *)(&nEvent), sizeof(int), 0);
	if(sizeof(int)!=nRet)
		RECV_ERROR("add new client, start msgQSend() failed.\n");
		
	RECV_INFO("INFO:Tcp client connect from [%s:%d].\n",inet_ntoa(newAddr.sin_addr),ntohs(newAddr.sin_port));
					
	return 0;
	
error_ret:
	
	if(-1!=newClientSock)
		close(newClientSock);

	return -1;
}

void *CTcpServer::ServerThreadFunc(void *pArg)
{
    int i =0;
	int bQuit = 0;
	int sockServer = -1;
	int bSelect = 0;
	int nDelay = 0;
	int nRet = 0;
	int nEvent = 0;
	int nRunTime = 0;
	unsigned int iMaxFd =0;
	int iTimeOutTimes =0;
	CTcpServer *pThis = (CTcpServer *)pArg;	
	
	struct timeval   timeout;
	fd_set     readFds;
	
	nRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(0!=nRet)
	{
		RECV_DEBUG("%s:%d pthread_setcancelstate() failed.\n",__FUNCTION__,__LINE__);
		pthread_exit(&nRet);
	}
	
	nRet = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
	if(0!=nRet)
	{
		RECV_DEBUG("%s:%d pthread_setcanceltype() failed.\n",__FUNCTION__,__LINE__);
		pthread_exit(&nRet);
	}
	writeThreadInfo("tcpServer");
    sockServer = pThis->m_sockServer;
	while(1)
	{
		sem_wait(&pThis->m_semTcpServer);
		bQuit = pThis->m_bServerThreadQuit;
		sem_post(&pThis->m_semTcpServer);
		
		if(bQuit)
			break;

		if(nRunTime>=(60*1000))
		{
			RECV_INFO("INFO: Tcp Server thread is listening at port[%d].\n",pThis->m_nListenPort);
			nRunTime = 0;
		}
		
		if(-1==sockServer)
		{
			usleep(1000*1000);
			nRunTime += 1000;
			shell_print("INFO: Tcp Server socket is not open\n");
			continue;
		}
		
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		
		FD_ZERO(&readFds);
		FD_SET((unsigned int)sockServer, &readFds);

        iMaxFd = sockServer;
		
		for(i=0; i < pThis->m_nClientNum; i++)
		{
			if( (-1 == pThis->m_pXcsClient[i].sockClient) && (pThis->m_pXcsClient[i].nTTL <= 0) )
			{
			    //RECV_INFO("tcp Server sockClient is %d,nttl is %d.\n", pThis->m_pXcsClient[i].sockClient, pThis->m_pXcsClient[i].nTTL);
				continue;
			}
			if(pThis->m_pXcsClient[i].nTTL > nDelay)
			{
			    //防止关闭时出问题，加上锁
	            sem_wait(&pThis->m_pXcsClient[i].sem);

				pThis->m_pXcsClient[i].nTTL -= nDelay;
				//shell_print("tcp server %d client,nttl:%d,nDelay:%d\n", i, pThis->m_pXcsClient[i].nTTL, nDelay);
				if( pThis->m_pXcsClient[i].sockClient >= 0 )
				{
					FD_SET((unsigned int)(pThis->m_pXcsClient[i].sockClient), &readFds);
					if( iMaxFd < (unsigned int)(pThis->m_pXcsClient[i].sockClient) )
					{
					    iMaxFd = (unsigned int)(pThis->m_pXcsClient[i].sockClient);
					}
				}
				sem_post(&pThis->m_pXcsClient[i].sem);

			}
			else
			{
				//超时断开
				pThis->m_pXcsClient[i].nTTL = 0;
				
				nEvent = EVENT_DISCONN;
				nRet = msgQSend(pThis->m_pXcsClient[i].msgQEvent,(char *)&nEvent,sizeof(nEvent),0 );
				if( nRet != sizeof(nEvent) )
				{
					RECV_ERROR("tcp server timeout disconn msgQSend() failed.nRet is %d\n", nRet);
				    //防止关闭时出问题，加上锁
                    sem_wait(&pThis->m_pXcsClient[i].sem);

					close(pThis->m_pXcsClient[i].sockClient);
					pThis->m_pXcsClient[i].sockClient = -1;
					#if 0
					pThis->m_pXcsClient[i].sin_addr = 0;
					pThis->m_pXcsClient[i].sin_port = 0;
					pThis->m_pXcsClient[i].nRecvLen = 0;
					pThis->m_pXcsClient[i].nSendLen = 0;
					pThis->m_pXcsClient[i].nPackLen = 0;
					pThis->m_pXcsClient[i].nTTL = 0;
					#endif
					RECV_ERROR("tcp server timeout disconn msgQSend() failed，is here\n");
                    sem_post(&pThis->m_pXcsClient[i].sem);
                    continue;
			    }
			    else
			    {
			        RECV_ERROR("tcp Server disconn msg msgQSend() ok.\n");
			    }
			}
		}
		
		bSelect = select( (iMaxFd +1), (fd_set *)(&readFds), 0, 0, &timeout);
		if(bSelect>0)//event 
		{
			//client net event
			for (i=0; i<pThis->m_nClientNum; i++)
			{
			    //防止关闭时出问题，加上锁
	            sem_wait(&pThis->m_pXcsClient[i].sem);

				if( -1 != pThis->m_pXcsClient[i].sockClient )
				{
					if (FD_ISSET((unsigned int)(pThis->m_pXcsClient[i].sockClient), &readFds))
					{
						sem_post(&pThis->m_pXcsClient[i].sem);
						
						nEvent = EVENT_READY;
						nRet = msgQSend(pThis->m_pXcsClient[i].msgQEvent,(char *)&nEvent,sizeof(nEvent),0 );
						if(nRet != sizeof(nEvent))
						{
						    iTimeOutTimes++;
						    if(1 == iTimeOutTimes/200 )
						    {
						        iTimeOutTimes =0;
    							RECV_DEBUG("xcs client thread is busy maybe dead lock!\n");
    						}
							usleep(100*1000);
					    }
					}
					else
					{
						sem_post(&pThis->m_pXcsClient[i].sem);
					}
                    nEvent = EVENT_HEART;
					nRet = msgQSend(pThis->m_pXcsClient[i].msgQEvent,(char *)&nEvent,sizeof(nEvent),0 );
				}
				else
				{
					sem_post(&pThis->m_pXcsClient[i].sem);
				}
			}
			//server net event
			if (FD_ISSET(sockServer, &readFds))
				nRet = pThis->AddNewClient();
			
			usleep(1*1000);
			nRunTime+=1;
		}
		else if(0==bSelect)//timeout
		{
			nDelay = 1000;
			nRunTime += 1*1000;
			//RECV_INFO("CTcpServer thread select timeout [port=%d].\n",pThis->m_nListenPort);
		}
		else//error 
		{
			nDelay = TCP_TTL_TIMEOUT;
			usleep(TCP_TTL_TIMEOUT);
			nRunTime += TCP_TTL_TIMEOUT;
			RECV_ERROR("TcpServer select() error,ret:0x%x,errno:%d, strerrno:%s.\n", bSelect, errno, strerror(errno));
			if((9 == errno) || ( 4 == errno ))
			{
	        	syslog(LOG_ERR|LOG_USER, "TcpServer select() error,ret:0x%x,errno:%d, strerrno:%s.continue run\n", bSelect, errno, strerror(errno));
				continue;
			}
        	syslog(LOG_ERR|LOG_USER, "TcpServer select() error,ret:0x%x,errno:%d, strerrno:%s.killall xcs.\n", bSelect, errno, strerror(errno));
        	system("killall -9 xcs");
		}
	}
	
	RECV_INFO("INFO:Tcp Server thread end.\n");
	pthread_exit(&nRet);
}

void *CTcpServer::UserThreadFunc(void *pArg)
{
    if( NULL == pArg )
    {
        RECV_ERROR("in param error\n");
        return NULL;
    }
	int nRet = 0;
	int nEvent = 0;
	int bQuit = 0;
	CTP *param = (CTP *)pArg;
	int nIndex = param->nIndex;
	CTcpServer* pThis = (CTcpServer* )(param->pArg);
	CHandleXcsPtl *pHKjXcs = new CHandleXcsPtl;
	unsigned int nRunTime = 0;
	unsigned int uiConnectSta = 0;

    pHKjXcs->m_uiThreadFlag = XCS_PROTO_TYPE;
	
	nRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(0!=nRet)
	{
		RECV_DEBUG("pthread_setcancelstate() failed.\n");
		pthread_exit(&nRet);
	}
	
	nRet = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
	if(0!=nRet)
	{
		RECV_DEBUG("pthread_setcanceltype() failed.\n");
		pthread_exit(&nRet);
	}
	writeThreadInfo("tcpClient");
	while(1)
	{
		if(bQuit)
			break;
		if(nRunTime>=(60*1000))
		{
			RECV_INFO("INFO: Tcp Server UserThread[%d] is Running nRunTime = %d.\n",nIndex,nRunTime);
			nRunTime = 0;
		}
		nRet = msgQReceive(pThis->m_pXcsClient[nIndex].msgQEvent, (char *)(&nEvent), sizeof(int), 3*1000);
		
		if( sizeof(int) == nRet )
		{
			switch(nEvent)
			{
				case EVENT_READY:
					{
						if( -1 == pThis->m_pXcsClient[nIndex].sockClient )
						{
							RECV_DEBUG("nIndex:%d clean up unresolved events.\n", nIndex);
							continue;
						}
                        // add lock between sync data & async data
                        sem_wait( &(pThis->m_pXcsClient[nIndex].sem) );

						nRet = recv(pThis->m_pXcsClient[nIndex].sockClient,\
									pThis->m_pXcsClient[nIndex].pRecvBuf + pThis->m_pXcsClient[nIndex].nRecvLen,\
									pThis->m_pXcsClient[nIndex].nRecvBufLen - pThis->m_pXcsClient[nIndex].nRecvLen,\
									0);
						
						if(nRet>0)
						{
							pThis->m_pXcsClient[nIndex].nRecvLen += nRet;
							pHKjXcs->DataPump( &(pThis->m_pXcsClient[nIndex]) );
							pThis->TcpClientSendSyncData(&(pThis->m_pXcsClient[nIndex]));
							pThis->m_pXcsClient[nIndex].nTTL = TCP_TTL_TIMEOUT;
						}
						else if(nRet==0)//TCP异常断开
						{
							RECV_ERROR("nIndex:%d,TCP Disconnect Exceptional.\n", nIndex);
							{
							    int ret =0;
							    struct in_addr in;
                            	in.s_addr = pThis->m_pXcsClient[nIndex].sin_addr;
                            	
                            	char szLog[256] = {0};
                            	sprintf(szLog,"nIndex:%d,Xcs User except Logout From Ip[%s] .\n", nIndex,inet_ntoa(in));
                            	mcLog(LMT_DVSPH,LT_INFO,szLog);
                            	RECV_ERROR("INFO:%s",szLog);

                                
                            	if(pThis->m_pXcsClient[nIndex].bUserValid)
                            	{
                                    STR_EXCEPT_DISCON_CLIENT strClientInfo;
                            		STR_SYS_APP_MSG strTmpSysMsg;

                                    strClientInfo.uiIp = pThis->m_pXcsClient[nIndex].sin_addr;
                                    strClientInfo.usPort = pThis->m_pXcsClient[nIndex].sin_port;
                                    strClientInfo.uiSessionId = pThis->m_pXcsClient[nIndex].uiSessionId;

                                    strClientInfo.uiDataUploadIp    = pHKjXcs->uiDataUploadIp;
                                    strClientInfo.usDataUploadPort  = pHKjXcs->usDataUploadPort;

                                    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
                                    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
                                    strTmpSysMsg.uiMsgType = MSG_XCS_EXCEPT_DISCONNECT;
                                    memcpy(strTmpSysMsg.aucData, &strClientInfo, sizeof(strClientInfo));
                                    ret = sendMsgToMainApp(&strTmpSysMsg);
                                    if(-1 == ret)
                                    {
                                        RECV_ERROR("nIndex:%d send client except disconnect msg failed.\n", nIndex);
                                    }
                                    else
                                    {
                                        RECV_ERROR("nIndex:%d send client except disconnect msg ok.\n", nIndex);
                                    }
                            	}
                                
                                pThis->m_pXcsClient[nIndex].bUserValid = 0;
                                pThis->m_pXcsClient[nIndex].uiHeartBit= 1;
                                pThis->m_pXcsClient[nIndex].uiThreadTime = 0;
                                pThis->m_pXcsClient[nIndex].uiHBFlag= 0;
                                
								nEvent = EVENT_DISCONN;
								nRet = msgQSend(pThis->m_pXcsClient[nIndex].msgQEvent, (char *)(&nEvent), sizeof(int),0);
								if(sizeof(int)!=nRet)
								{
									RECV_ERROR("nIndex:%d msgQSend to self failed.\n", nIndex);
							    }
							    else
							    {
							        RECV_ERROR("nIndex:%d tcp except disconn send msg ok.\n", nIndex);
							    }
							}
						}
						else
						{
							RECV_ERROR("nIndex:%d client socket recv() failed.\n", nIndex);
						}
                        sem_post(&pThis->m_pXcsClient[nIndex].sem);
					}
					break;
				case EVENT_DISCONN://TCP断开
					shell_print("nIndex:%d EVENT [EVENT_DISCONN]_ok\n", nIndex);
					RECV_ERROR("tcp client:index_%d,socket:%d,sin_addr:0x%x,port:%d,nTtl:%d.\n",\
                        nIndex,
                    	pThis->m_pXcsClient[nIndex].sockClient,
    					pThis->m_pXcsClient[nIndex].sin_addr,
    					pThis->m_pXcsClient[nIndex].sin_port,
    					pThis->m_pXcsClient[nIndex].nTTL)
    					
					
					{
                        //防止关闭时出问题，加上锁
                        sem_wait(&pThis->m_pXcsClient[nIndex].sem);

						close(pThis->m_pXcsClient[nIndex].sockClient);
						pThis->m_pXcsClient[nIndex].sockClient = -1;
						pThis->m_pXcsClient[nIndex].sin_addr = 0;
						pThis->m_pXcsClient[nIndex].sin_port = 0;
						pThis->m_pXcsClient[nIndex].nRecvLen = 0;
						pThis->m_pXcsClient[nIndex].nSendLen = 0;
						pThis->m_pXcsClient[nIndex].nPackLen = 0;
						pThis->m_pXcsClient[nIndex].nTTL = 0;
						RECV_INFO("nIndex TCP Disconnect.\n", nIndex);
						{
							nEvent = EVENT_END;
							nRet = msgQSend(pThis->m_pXcsClient[nIndex].msgQEvent, (char *)&nEvent, sizeof(int), 0);
							if(sizeof(int)!=nRet)
								RECV_ERROR("nIndex:%d user thread event_end msgQSend() failed.\n",nIndex);
						}
						
						/* 同时关闭心跳socket */
                        if( nIndex == 0)
                        {
                            nEvent = EVENT_DISCONN;
							nRet = msgQSend(pThis->m_pXcsClient[nIndex+1].msgQEvent, (char *)&nEvent, sizeof(int), 0);
							if(sizeof(int)!=nRet)
								RECV_ERROR("nIndex:%d user thread EVENT_DISCONN msgQSend() failed.\n",nIndex+1);
                        }
						
                        sem_post(&pThis->m_pXcsClient[nIndex].sem);
					}
					break;
				case EVENT_START:
					RECV_DEBUG("nIndex:%d EVENT [EVENT_START]\n", nIndex);
					pHKjXcs->InitClient(&(pThis->m_pXcsClient[nIndex]));
					break;
				case EVENT_END:
					RECV_DEBUG("nIndex:%d EVENT [EVENT_END]\n", nIndex);
					pHKjXcs->UninitClient(&(pThis->m_pXcsClient[nIndex]));
					break;
				case EVENT_EXIT:
					RECV_DEBUG("nIndex:%d EVENT [EVENT_EXIT]\n", nIndex);
					bQuit = 1;
					break;
				case EVENT_HEART:
				    pThis->m_pXcsClient[nIndex].nTTL = TCP_TTL_TIMEOUT;
				    break;
				default:
					RECV_DEBUG("nIndex:%d Unknow EVENT [%d]\n", nIndex,nEvent);
					break;
			}
			nRunTime+=100;
			uiConnectSta =0;
		}
		else
		{
			nRunTime+=3*1000;
			uiConnectSta++;
		}
		
		if( (uiConnectSta >= 5) && (nIndex ==1)&& ( -1 != pThis->m_pXcsClient[nIndex].sockClient) )
		{
		    RECV_ERROR("ERROR: nIndex:%d Tcp client connect is timeout,send disconnect msg.\n", nIndex);
			uiConnectSta = 0;
			
			int ret =0;
		    struct in_addr in;
        	in.s_addr = pThis->m_pXcsClient[nIndex].sin_addr;
        	
        	char szLog[256] = {0};
        	sprintf(szLog,"nIndex:%d Xcs User except Logout From Ip[%s] .\n", nIndex, inet_ntoa(in));
        	mcLog(LMT_DVSPH,LT_INFO,szLog);
        	RECV_ERROR("INFO:%s",szLog);

            
        	if(pThis->m_pXcsClient[nIndex].bUserValid)
        	{
                STR_EXCEPT_DISCON_CLIENT strClientInfo;
        		STR_SYS_APP_MSG strTmpSysMsg;

                strClientInfo.uiIp = pThis->m_pXcsClient[nIndex].sin_addr;
                strClientInfo.usPort = pThis->m_pXcsClient[nIndex].sin_port;
                strClientInfo.uiSessionId = pThis->m_pXcsClient[nIndex].uiSessionId;
                
                strClientInfo.uiDataUploadIp    = pHKjXcs->uiDataUploadIp;
                strClientInfo.usDataUploadPort  = pHKjXcs->usDataUploadPort;
                memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
                strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
                strTmpSysMsg.uiMsgType = MSG_XCS_EXCEPT_DISCONNECT;
                memcpy(strTmpSysMsg.aucData, &strClientInfo, sizeof(strClientInfo));
                ret = sendMsgToMainApp(&strTmpSysMsg);
                if(-1 == ret)
                {
                    RECV_ERROR("nIndex:%d send client except disconnect msg failed.\n", nIndex);
                }
                else
                {
                    RECV_INFO("nIndex:%d send client except disconnect msg ok.\n", nIndex);
                }
        	}
            
            pThis->m_pXcsClient[nIndex].bUserValid = 0;
            pThis->m_pXcsClient[nIndex].uiHeartBit= 1;
            pThis->m_pXcsClient[nIndex].uiThreadTime = 0;
            pThis->m_pXcsClient[nIndex].uiHBFlag= 0;
            
			nEvent = EVENT_DISCONN;
			nRet = msgQSend(pThis->m_pXcsClient[nIndex].msgQEvent, (char *)(&nEvent), sizeof(int),0);
			if(sizeof(int)!=nRet)
			{
				RECV_ERROR("nIndex:%d msgQSend to self failed.\n", nIndex);
		    }
		    else
		    {
		        RECV_ERROR("nIndex:%d tcp_2 except disconn send msg ok.\n", nIndex);
		    }

		    nEvent = EVENT_DISCONN;
			nRet = msgQSend(pThis->m_pXcsClient[nIndex-1].msgQEvent, (char *)(&nEvent), sizeof(int),0);
			if(sizeof(int)!=nRet)
			{
				RECV_ERROR("nIndex:%d msgQSend to self failed.\n", nIndex-1);
		    }
		    else
		    {
		        RECV_ERROR("nIndex:%d tcp except disconn send msg ok.\n", nIndex-1);
		    }
		}
	}
    delete pHKjXcs;
    pHKjXcs = NULL;
	RECV_INFO("INFO:Tcp Server UserThread[%d] end.\n",nIndex);
	pthread_exit(&nRet);
}

int CTcpServer::TcpClientSendSyncData(XCS_CLIENT* pDvsClient)
{
	int iSendSum =0;
	int iSendNum =0;
    char *pTmpData = NULL;

	if((NULL==pDvsClient)||(-1==pDvsClient->sockClient))
		return -1;

	if(pDvsClient->nSendLen<=0)
		return -1;

	iSendNum = send(pDvsClient->sockClient, (char*)pDvsClient->pSendBuf, pDvsClient->nSendLen, MSG_NOSIGNAL);
	if(iSendNum == -1)
	{
	    RECV_ERROR("%s %d,send data is failed.\n",__FUNCTION__,__LINE__);
	}
	else
	{
	    iSendSum = iSendNum;

        while(iSendSum < pDvsClient->nSendLen)
        {
            pTmpData = pDvsClient->pSendBuf + iSendSum;
            iSendNum = send(pDvsClient->sockClient, pTmpData, pDvsClient->nSendLen - iSendSum, MSG_NOSIGNAL);
            if ( -1 == iSendNum)
            {
                RECV_ERROR("%s %d,send data is failed.\n",__FUNCTION__,__LINE__);
                return iSendNum;
            }
            else
            {
                iSendSum += iSendNum;
            }
        }
	}
	pDvsClient->nSendLen = 0;

	return iSendSum;
}

int CTcpServer::TcpClientSendAsyncData(char *pBuf,int nLen)
{
	int nRet = 0;
    int i;
    
	for (i=0; i<m_nClientNum; i++)
	{
        if (m_pXcsClient[i].sockClient != -1)
        {
            sem_wait(&m_pXcsClient[i].sem);

            // 确认数据对这个连接是否要上传
            
        	nRet += send(m_pXcsClient[i].sockClient, (char*)pBuf, nLen, MSG_NOSIGNAL);
            

            sem_post(&m_pXcsClient[i].sem);
        }
	}
    
	return nRet;
}

