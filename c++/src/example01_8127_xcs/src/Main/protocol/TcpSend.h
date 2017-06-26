/******************************************************************************
 * TcpSend.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.25, xcsworker Create
 * --------------------
 ******************************************************************************/
#ifndef   __TCPSEND_H
#define    __TCPSEND_H
#include <list>
#include <semaphore.h>
using     namespace   std;



#define  MAX_SERVER_NUM    (5)

typedef struct t_ServerTab
{
    unsigned int     uiIpAddr;
    unsigned short   usPort;
    unsigned short   usRsv;
}SERVER_TABLE;
typedef struct t_SendInfo
{
    unsigned int  uiTotalLen;
    unsigned int  uiIndex;
}_PACKED_ STR_SEND_INFO;


#define TCPSEND_MAX_PIC_NUM   (4)

#if defined(CYC800JX)||defined(CYC800XKW4C)
    #define TCPSEND_MAX_BUFF_SIZE     (1536*1024)
#else
    #define TCPSEND_MAX_BUFF_SIZE     (1536*1024)    
#endif


class CTcpSend
{
public:
        CTcpSend();
        ~CTcpSend();
        bool    TcpSendCreate();
        bool    AddTcpServer(unsigned int uiIpAddr,unsigned short usPort);
        bool    DelTcpServer(unsigned int uiIpAddr,unsigned short usPort);
        bool    TcpSendData(INFORM_CONTENT *pInform);
        bool    TcpShutDown(unsigned int IpAddr,unsigned short Port);
        
        bool    TcpSendDestory();
private:
        static void *pThreadSendData(void *arg);
        pthread_t               m_TcpSendPid;
        MSG_Q_ID                m_TcpSendMsgQue;
        bool                    DoTcpSendData(STR_SEND_INFO *pStrSendInfo);
        CTcpConn                *m_cTcpConn;
        char                    *m_pcSendBuf[TCPSEND_MAX_PIC_NUM];
        INFORM_PARAM            *m_pstrInformParam;
        bool                    m_bSendAvai[TCPSEND_MAX_PIC_NUM];
        bool                    m_bSending[TCPSEND_MAX_PIC_NUM];
        bool                    m_bQuit;
        STR_SEND_INFO           m_strSendInfo[TCPSEND_MAX_PIC_NUM];
        list<SERVER_TABLE *>    m_ServerList;
        
};

#endif
