/*
 * FileName:       trafficLightManage.cpp
 * Author:         its  Version: 2.0.0.1  Date: 2011-10-19
 * Description:    创建消息队列和线程，接收外设发送过来的交通灯信号，当红灯持续时间超过3分钟时，主动
 * 把交通灯切换为绿灯，防止误抓拍。
 * Version:        
 * Function List:  
 *                 1.
 * History:        
 *     <author>   <time>    <version >   <desc>
 */

#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <ctype.h>
#include <syslog.h>
#include <fcntl.h>

#include "msgQLib.h"
#include "dsp_service.h"

#include "SysHandle.h"
#include "SysConf.h"
#include "McLog.h"

typedef struct tag_TRAFFIC_LIGHT_STA_PARAM
{
    int iLightSta;              /* 红灯还是绿灯 */
    unsigned int iBeginTimeS;   /* 车检器上报秒时间 */
    unsigned int iBeginTimeMs;  /* 车检器上报毫秒时间 */
    unsigned int iRedLValidTimeS;/* 红灯实际生效时间 */
    int iDecType;               /* 交通灯检测模式 */
    int iHandleFlag;            /* 是否已经处理过 */
}TRAFFIC_LIGHT_STA_PARAM;

static pthread_t trafficLMPid;
static int s_iThreadExit = 0;

/* 红灯延时秒数静态变量 */
static unsigned char s_ucRedLightDelay =1;

MSG_Q_ID pTrafficLManageQueue = NULL;

/* 红灯检测方式全局变量，主要用于切换判断 */
extern int g_iRLDecType;

extern sem_t sem_dsp;

extern unsigned char g_aucRLSta[MAX_TL_NUM];
extern unsigned char g_aucVideoRLSta[MAX_TL_NUM];

int trafficLDelaySecSet(unsigned char ucVal)
{
    s_ucRedLightDelay = ucVal;
    return 0;
}

int sendMsgToTrafficLM(STR_SYS_APP_MSG *pStrSysMsg)
{
    int ret =-1;
    if(NULL == pStrSysMsg)
    {
        SYSHANDLE_ERROR("in param is null\n");
        return -1;
    }
    SYSHANDLE_INFO("traffic msg send moduleId:%d,msgType:%d\n", pStrSysMsg->uiModuleId, pStrSysMsg->uiMsgType);
    ret = msgQSend(pTrafficLManageQueue, (char *)(pStrSysMsg), sizeof(STR_SYS_APP_MSG),  0);
    return ret;
}

int getTrafficMsgQSize(void)
{
	int ret = 0;
	ret = msgQNumMsgs(pTrafficLManageQueue);
    return ret;
}

static void *trafficLightMThreadFun(void *arg)
{
    int i = 0;
    int ret =0;
    unsigned char ucVehId =0;
    unsigned char ucTLSta =0;
    unsigned char ucDecType =0;
    STR_SYS_APP_MSG strTLMMsg;
    unsigned int ulTLStartSec =0;
    unsigned int ulTLStartMS =0;
    struct timeval tv;
    struct timezone tz;
    	
    TRAFFIC_LIGHT_STA_PARAM aStrTLM[MAX_TL_NUM];

    writeThreadInfo("trafficLightManage");
    memset(aStrTLM, 0, sizeof(aStrTLM));

    DEV_TL_STATUS_INFO strDspRDSta;
    
    while(1)
    {
        if( s_iThreadExit == 1 )
        {
            break;
        }
        ret = msgQReceive(pTrafficLManageQueue, (char *)&strTLMMsg, sizeof(strTLMMsg), 100);

        gettimeofday(&tv,&tz);

        ulTLStartMS  = tv.tv_usec/1000;
        ulTLStartSec = tv.tv_sec  + 8*3600;
        
        if( sizeof(strTLMMsg) == ret )
		{
		    SYSHANDLE_INFO("trafficLightMThreadFun rcv moduleId:%d,sysType:%d g_iRLDecType:%d\n", strTLMMsg.uiModuleId, strTLMMsg.uiMsgType,g_iRLDecType);
			shell_print("###ucVehId:%d ucTLSta:%d ucDecType:%d\n",strTLMMsg.aucData[0],strTLMMsg.aucData[1],strTLMMsg.aucData[2]);
			if( TRAFFIC_LIGHT_MANA_MODULE_ID == strTLMMsg.uiModuleId )
            {
                switch( strTLMMsg.uiMsgType )
                {
                    case MSG_TRAFFIC_LIGHT_INFO_SET:
                        {
                            ucVehId = strTLMMsg.aucData[0];
                            ucTLSta = strTLMMsg.aucData[1];
                            ucDecType = strTLMMsg.aucData[2];

							if( RED_DETECT_VIDEO == g_iRLDecType )
                            {
                                if( RED_DETECT_VIDEO != ucDecType )
                                {
                                	/* 在视频红灯检测模式下，红灯检测器上报的状态不进入处理流程 */
                                	SYSHANDLE_INFO("g_iRLDecType:%d != ucDecType%d,dir return.\n", g_iRLDecType, ucDecType);
                                	break;
                                }
                            }

                            aStrTLM[ucVehId].iLightSta    = ucTLSta;
                            aStrTLM[ucVehId].iHandleFlag  = 0;
                            aStrTLM[ucVehId].iDecType     = ucDecType;
                            aStrTLM[ucVehId].iBeginTimeS  = ulTLStartSec;
                            aStrTLM[ucVehId].iBeginTimeMs = ulTLStartMS;
                            if( RED_LIGHT_ON == ucTLSta )
                            {
                                /* 红灯延迟几秒后生效，当时不做处理 */
                                aStrTLM[ucVehId].iRedLValidTimeS = ulTLStartSec + s_ucRedLightDelay;
                            }
                            else
                            {
                                aStrTLM[ucVehId].iRedLValidTimeS = 0;
                                kjRedLightHandle(ucVehId, ucTLSta, ucDecType);
                            }
                        }

                        memset(&strDspRDSta, 0xff, sizeof(strDspRDSta));
                        for(i =0; i< MAX_TL_NUM; i++)
                        {
                            /* 通知dsp当前红灯状态 */
                            if( RED_DETECT_VIDEO == g_iRLDecType )
                            {
                                if(RED_DETECT_VIDEO == aStrTLM[i].iDecType)
                                {
                                    strDspRDSta.tlInfo[i].cTLNum = i;
                                    if( strDspRDSta.tlInfo[i].cTLNum > 0 )
                                    {
                                        strDspRDSta.tlInfo[i].cTLNum--;
                                    }
                                    strDspRDSta.tlInfo[i].cTLStatus = aStrTLM[i].iLightSta;
                                }
                            }
                            else if( RED_DETECT_SINGAL == g_iRLDecType )
                            {
                                if(RED_DETECT_SINGAL == aStrTLM[i].iDecType)
                                {
                                    strDspRDSta.tlInfo[i].cTLNum = i;
                                    strDspRDSta.tlInfo[i].cTLStatus = aStrTLM[i].iLightSta;
                                }
                            }
							else
							{
								shell_print("ERROR:###ucVehId:%d ucTLSta:%d ucDecType:%d\n",ucVehId,ucTLSta,ucDecType);
							}
                        }
                        //sem_wait(&sem_dsp);
                        shell_print("&&&&NET_DEV_SET_TL_STATUS_INFO iDecType:%d\n",aStrTLM[ucVehId].iDecType);
						shell_print("cTLNum:%d cTLStatus:%d\n",strDspRDSta.tlInfo[ucVehId].cTLNum,strDspRDSta.tlInfo[ucVehId].cTLStatus);
                        ret = CALL_CYC_CMD(NET_DEV_SET_TL_STATUS_INFO, strDspRDSta);
                        if(ret != 0)
					    {
					    	syslog(LOG_ERR|LOG_USER, "11_call NET_DEV_SET_TL_STATUS_INFO failed.\n");
						}
						//sem_post(&sem_dsp);
                        break;
                        
                    case MSG_TRAFFIC_LIGHT_MODE_SET:
                    
                        for(i =0; i< MAX_TL_NUM; i++)
                        {
                            ucVehId = i;
                            ucTLSta = RED_LIGHT_OFF;
                            ucDecType = g_iRLDecType;
                            
                            aStrTLM[ucVehId].iLightSta    = ucTLSta;
                            aStrTLM[ucVehId].iHandleFlag  = 0;
                            aStrTLM[ucVehId].iDecType     = ucDecType;
                            aStrTLM[ucVehId].iBeginTimeS  = ulTLStartSec;
                            aStrTLM[ucVehId].iBeginTimeMs = ulTLStartMS;

                            g_aucRLSta[i] = RED_LIGHT_OFF;
                            g_aucVideoRLSta[i] = RED_LIGHT_OFF;
                            
                            kjRedLightHandle(ucVehId, ucTLSta, ucDecType);
                        }

                        memset(&strDspRDSta, 0xff, sizeof(strDspRDSta));
                        for(i =0; i< MAX_TL_NUM; i++)
                        {
                             strDspRDSta.tlInfo[i].cTLNum = i;
                        }
                        
                        //sem_wait(&sem_dsp);
                        ret = CALL_CYC_CMD(NET_DEV_SET_TL_STATUS_INFO, strDspRDSta);
                        if(ret < 0)
					    {
					    	syslog(LOG_ERR|LOG_USER, "22_call NET_DEV_SET_TL_STATUS_INFO failed.\n");
						}
						//sem_post(&sem_dsp);
						
                        memset(aStrTLM, 0, sizeof(aStrTLM));
                        break;
                        
                    default:
                        break;
                }
            }
		}
        for(i =0; i< MAX_TL_NUM; i++)
        {
            /* 首先判断该信号有没有被处理过 */
            if( 0 == aStrTLM[i].iHandleFlag )
            {
                if( RED_LIGHT_ON == aStrTLM[i].iLightSta )
                {
                    /* 首先判断红灯延迟1秒信号有没有下发 */
                    if(( aStrTLM[i].iRedLValidTimeS > 0 ) && ( ulTLStartSec >= aStrTLM[i].iRedLValidTimeS ))
                    {
                        aStrTLM[i].iRedLValidTimeS =0;
                        kjRedLightHandle(i, aStrTLM[i].iLightSta, aStrTLM[i].iDecType);
                    }
                    
                    /* 若是红灯，则超过3分钟后，需要强制切换为绿灯 */
                    if( ulTLStartSec >= (aStrTLM[i].iBeginTimeS + RED_LIGHT_LAST_TIME))
                    {
                    	if(switchLaneLightNoCheck(i) > 0)
                    	{
	                        aStrTLM[i].iHandleFlag = 1;
	                        aStrTLM[i].iLightSta = RED_LIGHT_OFF;
	                        
	                        g_aucRLSta[i] = RED_LIGHT_OFF;
	                        g_aucVideoRLSta[i] = RED_LIGHT_OFF;
	                        
	                        kjRedLightHandle(i, RED_LIGHT_OFF, aStrTLM[i].iDecType);
						}
                    }
                }
            }
        }
    }
    pthread_exit(0);
}

int trafficLMInit(void)
{
    int ret = 0;
    pTrafficLManageQueue = msgQCreate(60, 1024, 0);
    if (pTrafficLManageQueue == NULL )
    {
        printf("trafficLight msgQCreate failed.\n");
        SYSHANDLE_ERROR("creat trafficLight manage message Queue failed .\n");
        syslog(LOG_ERR|LOG_USER, "%s trafficLight msgQCreat failed...", DVS_DEVICE_TYPE_NAME);
        return -1;
    }
    else
    {
        SYSHANDLE_ERROR("trafficLight msgQCreate success.\n");
        printf("trafficLight msgQCreate success.\n");
    }
    ret = pthread_create(&trafficLMPid, 0, &trafficLightMThreadFun, NULL);
    return 0;
}

int trafficLMUninit(void)
{
    s_iThreadExit = 1;
    if(trafficLMPid)
    {
        pthread_join(trafficLMPid, NULL);
    }
    return 0;
}



