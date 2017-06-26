#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <error.h>
#include <string.h>
#include <syslog.h>
#include <time.h>


#include "McLog.h"

#include "TcpConn.h"
//#include <sys/time.h>


CTcpConn::CTcpConn()
{
    m_CurSocket = -1;
    m_nNumBytes = 0;
    m_nSend = 0;
	sem_init(&m_sem,0,1);

    m_SocketList.clear();
}

CTcpConn::~CTcpConn()
{
    m_CurSocket = -1;
    m_nNumBytes = 0;
    m_nSend = 0;
    sem_destroy(&m_sem);
    list<SOCKET_TABLE *>::iterator iter;
    SOCKET_TABLE *p_SkNode = NULL;
    for(iter=m_SocketList.begin(); iter!=m_SocketList.end(); ++iter)
    {
        p_SkNode = *iter;

        if(p_SkNode->Skfd>0)
        {
            close(p_SkNode->Skfd);
            m_SocketList.erase(iter);
            delete p_SkNode;
        }
    }

}

int CTcpConn::Connect(unsigned int Addr, unsigned short nPort)
{
    int nRet = -1;
    int bFlag = 0;
    int sockRet = -1;
    struct sockaddr_in their_addr;

    if((sockRet = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        shell_print("%s\t%s-%d: socket failed\n", __FILE__, __FUNCTION__, __LINE__);
        return nRet;
    }
    else
    {
        shell_print("%s\t%s-%d: socket creat,val is %d.\n", __FILE__, __FUNCTION__, __LINE__,sockRet);
    }

    #if 1
    // 1. set socket option SO_REUSEADDR to reuse listen port
    bFlag = 1;
    nRet = setsockopt(sockRet,	
                                SOL_SOCKET,
                                SO_REUSEADDR,
                                (char *)&bFlag,
                                sizeof (bFlag));
    if (nRet != 0)
    {
        shell_print("%s\t%s-%d:  setsockopt() failed.\n", __FILE__, __FUNCTION__, __LINE__);
        close(sockRet);
        return nRet;
    }

    // 2. set socket option SO_KEEPALIVE to enable the use of keep-alive packets on TCP connections 
    bFlag = 1;
    nRet = setsockopt(sockRet,
                                SOL_SOCKET,
                                SO_KEEPALIVE,
                                (char *)&bFlag, 
                                sizeof (bFlag));
    if (nRet != 0)
    {
        close(sockRet);
        shell_print("%s\t%s-%d:  setsockopt() failed.\n", __FILE__, __FUNCTION__, __LINE__);
        return nRet;
    }

    // 3. set socket option SO_LINGER to controls the action taken when unsent data is queued on a socket
    struct linger   ltemp;
    ltemp.l_onoff = 1;
    ltemp.l_linger = 5;
    nRet = setsockopt(sockRet,
                                SOL_SOCKET,
                                SO_LINGER,
                                (char *)&ltemp,
                                sizeof (ltemp));
    if (nRet != 0)
    {
        close(sockRet);
        shell_print("%s\t%s-%d:  setsockopt() failed.\n", __FILE__, __FUNCTION__, __LINE__);
        return nRet;
    }

    // 4. set socket option TCP_NODELAY 
    bFlag = 1;
    nRet = setsockopt(sockRet,
                                IPPROTO_TCP,
                                TCP_NODELAY,
                                (char *)&bFlag,
                                sizeof (bFlag));
    if (nRet != 0)
    {
        close(sockRet);
        shell_print("%s\t%s-%d:  setsockopt() failed.\n", __FILE__, __FUNCTION__, __LINE__);
        return nRet;
    }
    
    /* tcp发送缓冲区大小增加到32K */
    int nSendBuf=32*1024;//设置为32K
    setsockopt(sockRet,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));

    #endif
    
    struct timeval timeo;		
    timeo.tv_sec = 3;	
    timeo.tv_usec = 0;
    if (setsockopt(sockRet,SOL_SOCKET,SO_SNDTIMEO,&timeo,sizeof(timeo)) == -1)	
    {
        close(sockRet);
        return -1;
    }   


    their_addr.sin_family = AF_INET;
    their_addr.sin_addr.s_addr = htonl(Addr);
    their_addr.sin_port = htons(nPort);


    #if 0
    bFlag = fcntl(sockRet,F_GETFL,0);
    nRet = fcntl(sockRet,F_SETFL,O_NONBLOCK|bFlag);
    if (nRet != 0)
    {
        printf("%s\t%s-%d:  fcntl() failed.\n", __FILE__, __FUNCTION__, __LINE__);
        //return nRet;
    }
    #endif

    // 为保证程序及时返回，只连接一次
    if(0 == (nRet = connect(sockRet, (sockaddr*)&their_addr, sizeof(sockaddr_in))))
    {
        return sockRet;
    }		
    else
    {
        close(sockRet);
        nRet = -1;
        shell_print("%s\t%s-%d:  connect() failed.\n", __FILE__, __FUNCTION__, __LINE__);
        return nRet;
    }
}



int CTcpConn::MatchSocket(unsigned int Addr, unsigned short nPort)
{
    list<SOCKET_TABLE *>::iterator iter;
    SOCKET_TABLE *pSkNode = NULL;
    for(iter=m_SocketList.begin(); iter!=m_SocketList.end(); ++iter)
    {
        pSkNode = *iter;
        if((Addr == pSkNode->Addr) && (nPort == pSkNode->nPort))
        {
            return pSkNode->Skfd;
        }
    }

    return -1;
}



int CTcpConn::AddSocket(unsigned int Addr, unsigned short nPort, int Sockfd)
{
    SOCKET_TABLE *pSkNode = NULL;
    
    list<SOCKET_TABLE *>::iterator iter;
    for(iter=m_SocketList.begin(); iter!=m_SocketList.end(); ++iter)
    {
        pSkNode = *iter;
        if((Addr == pSkNode->Addr) && (nPort == pSkNode->nPort))
        {
            return (-1);
        }
    }

    pSkNode=(SOCKET_TABLE *)(new int[sizeof(SOCKET_TABLE)/4+1]);
    if(NULL == pSkNode)
    {
        shell_print("%s,%s:%d\t malloc pSKNode failed\n", __FILE__, __FUNCTION__, __LINE__);
    }
    pSkNode->Addr = Addr;
    pSkNode->nPort = nPort;
    pSkNode->Skfd = Sockfd;
    m_SocketList.push_back(pSkNode);

    return 0;
}



int CTcpConn::DelSocket(int Sockfd)
{
    list<SOCKET_TABLE *>::iterator iter;
    SOCKET_TABLE *pSkNode = NULL;
    for(iter=m_SocketList.begin(); iter!=m_SocketList.end(); ++iter)
    {
        pSkNode = *iter;
        if(Sockfd == pSkNode->Skfd)
        {
            close(Sockfd);
            m_SocketList.erase(iter);
            delete pSkNode;
            return 0;
        }
    }
    return (-1);
}


int CTcpConn::TcpClearSock(unsigned int Addr, unsigned short nPort)
{
	int ret = 0;
	sem_wait(&m_sem);
	if((m_CurSocket = MatchSocket(Addr, nPort)) != -1)
	{
        ret = DelSocket(m_CurSocket);    
	}

	sem_post(&m_sem);
	return ret;
}

int CTcpConn::TcpSend(unsigned int Addr, unsigned short nPort, unsigned char * pData, int nDataLen)
{
    static int s_iSendTime=0;
    unsigned char *pTmpBuf = NULL;

    struct sockaddr_in sa;
    int len = sizeof(sa);

    unsigned long ulPlanStart = time(NULL) + 8*3600;
    static unsigned int s_uiLastTimeSec = ulPlanStart;

	sem_wait(&m_sem);
    if((m_CurSocket = MatchSocket(Addr, nPort)) == -1)
    {
        if((m_CurSocket = Connect(Addr, nPort)) == -1)
        {
            shell_print("web pic tcp reconnect failed:  Ip:%s Port:%d!\n", inet_ntoa(*(in_addr *)&Addr), nPort);
            goto error_ret;
        }
        else
        {
            shell_print("web pic tcp connect ok:  Ip:%s Port:%d\n", inet_ntoa(*(in_addr *)&Addr), nPort);
            AddSocket(Addr, nPort, m_CurSocket);
        }
    }
    
    if((ulPlanStart - s_uiLastTimeSec) > 300)
    {   
        s_uiLastTimeSec = ulPlanStart;
        
        shell_print( "web pic socket time out, reconnect!\n");
        DelSocket(m_CurSocket);
        
        if((m_CurSocket = Connect(Addr, nPort)) == -1)
        {
            shell_print("web pic tcp time out reconnect failed:  Ip:%s Port:%d!\n", inet_ntoa(*(in_addr *)&Addr), nPort);
            goto error_ret;
        }
        else
        {
            shell_print("web pic tcp time out connect ok:  Ip:%s Port:%d\n", inet_ntoa(*(in_addr *)&Addr), nPort);
            AddSocket(Addr, nPort, m_CurSocket);
        }
    }
    
    if(!getpeername(m_CurSocket, (struct sockaddr *)&sa, (socklen_t *)&len))
    {
        if(s_iSendTime%241 == 0)
        {
            shell_print( "getpeername web pic socket server iP:%s,", inet_ntoa(sa.sin_addr));
            shell_print( "PORT:%d.\n", ntohs(sa.sin_port));
        }
    }
    else
    {
        shell_print( "getpeername can't get server IP,close socket,and reconnect!\n", inet_ntoa(sa.sin_addr));
        DelSocket(m_CurSocket);
        
        if((m_CurSocket = Connect(Addr, nPort)) == -1)
        {
            shell_print("getpeername web pic tcp reconnect failed:  Ip:%s Port:%d!\n", inet_ntoa(*(in_addr *)&Addr), nPort);
            goto error_ret;
        }
        else
        {
            shell_print("getpeername web pic tcp connect ok:  Ip:%s Port:%d\n", inet_ntoa(*(in_addr *)&Addr), nPort);
            AddSocket(Addr, nPort, m_CurSocket);
        }
    }


    if ( 0 == nDataLen )
    {
        m_nSend = 0;
        goto all_done;
    }
    m_nNumBytes = 0;
    m_nSend = 0;
    // 添加检测该套接字是否已经被关闭
    {
        fd_set readset;
        int maxfdp1;
        int ret;
        struct timeval tm;
        char buf[1024] __attribute__((aligned(32)));
        FD_ZERO( &readset);
        FD_SET(m_CurSocket,&readset);
        maxfdp1 = m_CurSocket +  1;
        tm.tv_sec = 0;
        tm.tv_usec = 0;
        ret = select(maxfdp1,&readset,NULL,NULL,&tm);
        if (ret > 0 )
        {
            if ( FD_ISSET(m_CurSocket,&readset) )
            {
                if (read(m_CurSocket,buf,1024) == 0 )
                {
                    shell_print("socket closed by server .\n");
                    DelSocket(m_CurSocket);     
                    goto error_ret;
                }
            }
        }
        
        if(ret < 0)
        {
        	syslog(LOG_ERR|LOG_USER, "TcpConn post select error,killall xcs!\n");
        	system("killall -9 xcs");
        }
    }
    
    if((m_nNumBytes = send(m_CurSocket, pData, nDataLen, MSG_NOSIGNAL)) == -1)
    {
        DelSocket(m_CurSocket);
        goto error_ret;
    }
    else
    {
        m_nSend = m_nNumBytes;

        while(m_nSend < nDataLen)
        {
            pTmpBuf = pData + m_nSend;
            m_nNumBytes = send(m_CurSocket,pTmpBuf,nDataLen - m_nSend, MSG_NOSIGNAL);
            if ( -1 == m_nNumBytes)
            {
                goto error_ret;
            }
            else
            {
                m_nSend += m_nNumBytes;
            }
        }
    }

all_done:
    s_iSendTime++;
    if(s_iSendTime%240 == 0)
    {
        DelSocket(m_CurSocket);
        shell_print("web pic socket closed is here.sendTimes is %d.\n", s_iSendTime);
    }
	sem_post(&m_sem);
    return m_nSend;
error_ret:
	sem_post(&m_sem);
	return -1;
}


