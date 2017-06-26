#ifndef __TCP_CONN_H__
#define __TCP_CONN_H__

#include <list>
#include <semaphore.h>
using     namespace   std;


typedef struct t_SockTab
{
    unsigned int     Addr;
    int Skfd;
    unsigned short nPort;
    unsigned short usRsv;
}SOCKET_TABLE;


class CTcpConn
{
public:
    CTcpConn();
    ~CTcpConn();

    int TcpSend(unsigned int Addr, unsigned short nPort, unsigned char *pData, int nDataLen);
	int TcpClearSock(unsigned int Addr, unsigned short nPort);
private:
    int m_CurSocket;
    int m_nSend;
    int m_nNumBytes;
    list<SOCKET_TABLE *>  m_SocketList;
	sem_t m_sem;

    int MatchSocket(unsigned int Addr, unsigned short nPort);
    int AddSocket(unsigned int Addr, unsigned short nPort, int Sockfd);
    int DelSocket(int Sockfd);
    int Connect(unsigned int Addr, unsigned short nPort);
    int DisConn(unsigned int Addr, unsigned short nPort);


};

#endif














