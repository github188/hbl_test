/******************************************************************************
 * TcpSend.cpp:
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

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     /* gethostbyname */
#include <arpa/inet.h>
#include <vector>
#include "protoXcs.h"
#include "TcpConn.h"
#include "DataUpLoad.h"
#include "msgQLib.h"
#include  "TcpSend.h"
#include "McLog.h"

#include "SysHandle.h"

extern int           g_bTcpDataOk;

CTcpSend::CTcpSend()
{
     for (int i = 0; i<TCPSEND_MAX_PIC_NUM ;i++)
     {
        m_bSendAvai[i]  = true ;
        m_bSending[i]   = false;
     }
     m_bQuit = false;
}

CTcpSend::~CTcpSend()
{
    pthread_join(m_TcpSendPid,NULL);

    for (int i = 0; i<TCPSEND_MAX_PIC_NUM ;i++)
    {
        if ( NULL != m_pcSendBuf[i] )
        {
            delete m_pcSendBuf[i];
            m_pcSendBuf[i] = NULL;
        }
    }
    
}
void *CTcpSend::pThreadSendData(void *arg)
{
    CTcpSend *pThis = (CTcpSend *)arg;
    int ret =0;
    STR_SEND_INFO  strSendInfo;
    
    bool  bQuit = pThis->m_bQuit;
    writeThreadInfo("tcpSend");
    while (1)
	{
	    if (bQuit)
	    {
	        break;
	    }
	    else
        {   
            ret = msgQReceive(pThis->m_TcpSendMsgQue, (char * )&strSendInfo, 
    			               sizeof(STR_SEND_INFO), 1*1000);
    		if( ret == sizeof (STR_SEND_INFO))
    		{
    		    
    		    if(g_bTcpDataOk > 0)
    		    {
        			pThis->DoTcpSendData(&strSendInfo);  
        		}
        		else
        		{
        		    shell_print("socket not connect throw data!\n");
        		}
    		}
            else
            {
              
            }
        }
	}
    pthread_exit(0);
}
bool CTcpSend::TcpSendData(INFORM_CONTENT *pInform)
{
    int ret = 0;
    unsigned int TotalLen = 0;
    char PicNum = 0;
    unsigned int uiType =0;

    
    char *pBuf =NULL;
   
    int index = 0;
    int i = 0;

    if ( NULL == pInform)
    {
        return false;
    }
    for ( i = 0;i<TCPSEND_MAX_PIC_NUM;i++)
    {
        if (!m_bSending[i])
        {
            index = i;
            m_bSendAvai[i] = true;
            m_bSending[i] = true;
            break;
        }
    }
    if ( TCPSEND_MAX_PIC_NUM <= i)
    {
       shell_print("last pic is sending,plz wait\n");
       return false;
    }

    
    pBuf = m_pcSendBuf[index];
    m_strSendInfo[index].uiIndex = index;
   
    
    TotalLen =sizeof(STR_PACKET_HEADER);//协议头
    m_pstrInformParam = (INFORM_PARAM *)(pBuf + TotalLen);

    
    pBuf+=TotalLen;
    
    TotalLen =0;
    
    memcpy((void*)m_pstrInformParam,(void *)&pInform->strInformParam,sizeof(INFORM_PARAM));//
    TotalLen += sizeof(INFORM_PARAM);//上报信息参数头
    pBuf      += sizeof(INFORM_PARAM);
    
    uiType = m_pstrInformParam->uiType;

    m_pstrInformParam->uiVersion = htonl(pInform->strInformParam.uiVersion);
    m_pstrInformParam->uiType    = htonl(pInform->strInformParam.uiType);
    m_pstrInformParam->tTime     = htonl(pInform->strInformParam.tTime);
    
    
    if ((uiType == E_PASSPORT_SNAP) ||(uiType == E_ELECPOLICE_SNAP))//抓拍数据上报
    {
        SNAP_INFORM  *pXcsSnapInfo  = NULL;

        int ParamLen = 0;
        pXcsSnapInfo   = (SNAP_INFORM *)(pInform->pInformContent);
        ParamLen = pXcsSnapInfo->uiParamLen;//上报信息参数长度
        memcpy(pBuf,&ParamLen,sizeof(unsigned int));
        
        TotalLen += sizeof(unsigned int);
        pBuf      += sizeof(unsigned int);
        
        if (uiType == E_PASSPORT_SNAP)
        {
            memcpy(pBuf,pXcsSnapInfo->pucParamData,ParamLen);
        }
        else if (uiType == E_ELECPOLICE_SNAP )
        {
           memcpy(pBuf,pXcsSnapInfo->pucParamData,ParamLen);
        }
        pBuf+= ParamLen;
        TotalLen += ParamLen;
      
        PicNum= pXcsSnapInfo->ucPicNum;
      
        memcpy(pBuf,&PicNum,4);//图片数目+保留字段
        pBuf    += 4;
        TotalLen += 4;

        memcpy(pBuf,pXcsSnapInfo->auiPicLen,PicNum*sizeof(unsigned int));

        pBuf+= PicNum*sizeof(unsigned int);
        TotalLen += PicNum*sizeof(unsigned int);
        
        unsigned int pic_len =0;

        for (int i=0;i<PicNum;i++)
        {
            pic_len+= pXcsSnapInfo->auiPicLen[i];
        }
        TotalLen += pic_len;
        if ( TotalLen > TCPSEND_MAX_BUFF_SIZE )
        {
            m_bSendAvai[index] = true;
            m_bSending[index ] = false;
            return false;  
        }
        memcpy(pBuf,pXcsSnapInfo->pszPicData,pic_len);
        
        
    }
    else if ( (uiType == E_COMIN) || (uiType == E_COMOUT))//串口数据上报
    {
        COM_INFORM  *pXcsComInfo  = NULL;
        int dataLen = 0;
          
        pXcsComInfo  = (COM_INFORM *)(pInform->pInformContent);
        dataLen = pXcsComInfo->uiDataLen;

        memcpy(pBuf,pXcsComInfo,2*sizeof(unsigned int));
        pBuf     += 8;
        TotalLen += 8;

        TotalLen += dataLen;
        if ( TotalLen > TCPSEND_MAX_BUFF_SIZE )
        {
            m_bSendAvai[index] = true;
            m_bSending[index]  = false;
            return false;   
        }
        memcpy(pBuf,pXcsComInfo->pucData,dataLen);  
        
        
    }

    m_strSendInfo[index].uiTotalLen = TotalLen;
  
    //return true;
    ret = msgQSend(m_TcpSendMsgQue, (char *)&m_strSendInfo[index], sizeof(STR_SEND_INFO), 0);	
    
    return (ret == sizeof(STR_SEND_INFO ))?true:false;   
}

bool  CTcpSend::DoTcpSendData(STR_SEND_INFO *pStrSendInfo)
{
    
    STR_PACKET_HEADER  *pXcsHead;
    int len = 0;
    unsigned int TcpServerIp;
    unsigned short TcpServerPort;
    unsigned int dataLen = pStrSendInfo->uiTotalLen;
    unsigned int index = pStrSendInfo->uiIndex;
    int ret = 0;

    if (index >= TCPSEND_MAX_PIC_NUM)
    {
        return false;
    }
    pXcsHead = (STR_PACKET_HEADER*)m_pcSendBuf[index];
    memset(pXcsHead, 0, sizeof(STR_PACKET_HEADER));
	pXcsHead->SyncWord = htons(XCS_SYNC_WORD);
	pXcsHead->Version = XCS_PROTOCOL_VER;
	pXcsHead->IsAckOrNot = 1;
	pXcsHead->DataType = CPH_VCHANN;
	pXcsHead->SubDataType = CPE_VCHANN_DATA_REPORT;
	pXcsHead->uiPayloadLength = htonl(dataLen);


    len = sizeof(STR_PACKET_HEADER) + dataLen;
    
    SERVER_TABLE *pServerNode = NULL;
        
    list<SERVER_TABLE *>::iterator iter;
    for(iter=m_ServerList.begin(); iter!=m_ServerList.end(); ++iter)
    {
        pServerNode = *iter;

        if ( NULL != pServerNode )
        {
      
            TcpServerIp     = pServerNode->uiIpAddr;
            TcpServerPort   = pServerNode->usPort;
           
            ret = m_cTcpConn->TcpSend(TcpServerIp,TcpServerPort, (unsigned char*)m_pcSendBuf[index], len);
            if ( ret >0)
            {
               //shell_print("Tcp send picture %d ok\n",index);   
            }
            else
            {
                shell_print("Tcp send picture %d failed\n",index);     
            }
       }
    }
    m_bSending [index ]=  false;
    return true;
}
bool CTcpSend::TcpShutDown(unsigned int IpAddr,unsigned short Port)
{
    int ret = 0;

    shell_print("shutdown tcp connect \n");
    ret = m_cTcpConn->TcpClearSock(IpAddr, Port);

    if (ret <0)
    {
        return false;
    }
    
    return true;
}
bool CTcpSend::AddTcpServer(unsigned int uiIpAddr,unsigned short usPort)
{
    SERVER_TABLE *pServerNode = NULL;

    list<SERVER_TABLE *>::iterator iter;
    for(iter=m_ServerList.begin(); iter!=m_ServerList.end(); ++iter)
    {
        pServerNode = *iter;
        if((uiIpAddr == pServerNode->uiIpAddr) && (usPort == pServerNode->usPort))
        {
            shell_print("server exist\n");
            return false;
        }
    }
   
    pServerNode=(SERVER_TABLE *)(new int[sizeof(SERVER_TABLE)/4 +1]);
    if(NULL == pServerNode)
    {
        shell_print("%s,%s:%d\t malloc pServerNode failed\n", __FILE__, __FUNCTION__, __LINE__);
    }

    pServerNode->uiIpAddr   = uiIpAddr;
    pServerNode->usPort     = usPort;
    
    m_ServerList.push_back(pServerNode);
    return true;
}
bool CTcpSend::DelTcpServer(unsigned int uiIpAddr,unsigned short usPort)
{
    list<SERVER_TABLE *>::iterator iter;
    SERVER_TABLE *pServerNode = NULL;
    int i = 0;
    
    shell_print("del server 0x%x\n",uiIpAddr);
    
    for(iter=m_ServerList.begin(); iter!=m_ServerList.end(); ++iter)
    {
        pServerNode = *iter; 
        if((uiIpAddr == pServerNode->uiIpAddr)&&(usPort == pServerNode->usPort))
        {   
            TcpShutDown(uiIpAddr, usPort);
            m_ServerList.erase(iter);
            delete pServerNode;

            for ( i = 0;i<TCPSEND_MAX_PIC_NUM;i++)
            {
                m_bSending[i] = false;
            }
            return true;
        }
    }
    return true;
}
bool CTcpSend::TcpSendCreate()
{
    int ret = 0;

    m_cTcpConn = new CTcpConn;

    ret = pthread_create(&m_TcpSendPid,0,&pThreadSendData,this);

    if ( ret < 0 )
    {
        shell_print("pthread create failed \n");
        return false;
    }
    m_TcpSendMsgQue = msgQCreate(4, sizeof(STR_SEND_INFO), 0);

    if ( NULL == m_TcpSendMsgQue )
    {
        return false;
    }
    for ( int i = 0;i<TCPSEND_MAX_PIC_NUM;i++)
    {
        m_pcSendBuf[i] = (char*)(new int[TCPSEND_MAX_BUFF_SIZE/4]);
        if ( NULL == m_pcSendBuf[i] )
        {
             for ( int j = 0;j<i;j++)
             {
                delete m_pcSendBuf[j];
                m_pcSendBuf[j] = NULL;
             }
            return false;
        }
    }
    shell_print("tcp send create ok\n");
    return true;
    
}
bool CTcpSend::TcpSendDestory()
{
    if ( NULL != m_TcpSendMsgQue )
    {
        msgQDelete(m_TcpSendMsgQue);
    }
    for ( int i = 0;i<TCPSEND_MAX_PIC_NUM;i++)
    {
        if ( NULL != m_pcSendBuf[i] )
        {
            delete m_pcSendBuf[i];
            m_pcSendBuf[i] = NULL;
        }
    }
    m_bQuit = true;
    return true;   
}

