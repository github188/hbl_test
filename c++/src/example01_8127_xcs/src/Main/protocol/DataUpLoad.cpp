/******************************************************************************
 * DataUpLoad.cpp:
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
#include <time.h>
#include <pthread.h>
#include "protoXcs.h"
#include "msgQLib.h"
#include "TcpConn.h"
#include "DataUpLoad.h"
#include "TcpSend.h"

#include "McLog.h"

CDataUpLoad::CDataUpLoad()
{
    m_cTcpSendHandle = new CTcpSend;
    if(NULL == m_cTcpSendHandle)
    {
        shell_print("new m_cTcpSendHandle failed\n");
    }
}
CDataUpLoad::~CDataUpLoad()
{
    delete m_cTcpSendHandle;
}
bool CDataUpLoad::UpLoadData(INFORM_CONTENT * pInform)
{
    return m_cTcpSendHandle->TcpSendData(pInform);
}
bool CDataUpLoad::AddServer(unsigned int uiIpAddr,unsigned short usPort)
{
    return m_cTcpSendHandle->AddTcpServer(uiIpAddr,usPort);
}
bool CDataUpLoad::DelServer(unsigned int uiIpAddr,unsigned short usPort)
{
    return m_cTcpSendHandle->DelTcpServer(uiIpAddr,usPort);
}

bool CDataUpLoad::ShutDown(unsigned int uiIpAddr,unsigned short usPort)
{
    return m_cTcpSendHandle->TcpShutDown(uiIpAddr,  usPort);
}
bool CDataUpLoad::UpLoadInit()
{
    return m_cTcpSendHandle->TcpSendCreate();
}
bool CDataUpLoad::UpLoadUninit()
{
    return m_cTcpSendHandle->TcpSendDestory();
}
