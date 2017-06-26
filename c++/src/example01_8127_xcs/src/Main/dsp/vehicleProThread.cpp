#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/sem.h>

#include "McLog.h"

#include "SysHandle.h"
#include "sysHandle_define.h"

#include "vehicleProThread.h"
#include "vehicleReport.h"

MSG_Q_ID pVehiNormalQ = NULL;
MSG_Q_ID pVehiPPQ = NULL;

int sendMsgToVehiNormalThread(int iMsgType, VEHICLE_CALLBACK_STR *pstVehicle)
{
    int ret =-1;
    if(NULL == pstVehicle)
    {
        SYSHANDLE_ERROR("in param is null\n");
        return -1;
    }
    VEHI_PRO_INFO_MSG strVehiMsg;
    strVehiMsg.uiMsgType = iMsgType;
    strVehiMsg.stVehicleInfo = *pstVehicle;
    ret = msgQSend(pVehiNormalQ, (char *)(&strVehiMsg), sizeof(strVehiMsg),  0);
    return ret;
}

int sendMsgToVehiPPThread(int iMsgType, VEHICLE_CALLBACK_STR *pstVehicle)
{
    int ret =-1;
    if(NULL == pstVehicle)
    {
        SYSHANDLE_ERROR("in param is null\n");
        return -1;
    }
    VEHI_PRO_INFO_MSG strVehiMsg;
    strVehiMsg.uiMsgType = iMsgType;
    strVehiMsg.stVehicleInfo = *pstVehicle;
    ret = msgQSend(pVehiPPQ, (char *)(&strVehiMsg), sizeof(strVehiMsg),  0);
    return ret;
}

void *vehiNormalThreadFunc(void *pArg)
{
    int i =0;
    int iMsgSize =0;
	int ret = 0;
	VEHI_PRO_INFO_MSG  strVehiInfo;
    
	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(0!=ret)
	{
		SYSHANDLE_DEBUG("pthread_setcancelstate() failed.\n");
		pthread_exit(&ret);
	}
	
	ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
	if(0!=ret)
	{
		SYSHANDLE_DEBUG("pthread_setcanceltype() failed.\n");
		pthread_exit(&ret);
	}
	writeThreadInfo("vehiNormalPro");
	while(1)
	{
        ret = msgQReceive(pVehiNormalQ, (char *)&strVehiInfo, sizeof(VEHI_PRO_INFO_MSG), 30);
        if( sizeof(strVehiInfo) == ret )
		{
            switch(strVehiInfo.uiMsgType)
            {
                case MSG_KJ_VEHI:
                    kjEPSnapSeqQuery(strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta, \
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime);
                    break;
                case MSG_SJ_VEHI:
                    
                    break;
                case MSG_HGD_VEHI:
                    
                    break;
                case MSG_DC_VEHI:
                    dcEPSnapSeqQuery(strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta, \
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime);
                    break;
                default:
                    break;
            }
        }
        if(g_ucEPComBufCleanFlag == 1)
        {
            g_ucEPComBufCleanFlag =0;
            /* 在延迟抓拍过程中积累的消息均需要全部清除，以保证抓拍位置准确率 */
            iMsgSize = msgQNumMsgs(pVehiNormalQ);
            for(i=0; i< iMsgSize; i++)
            {
                msgQReceive(pVehiNormalQ, (char *)&strVehiInfo, sizeof(VEHI_PRO_INFO_MSG), 0);
            }
        }
	}
	pthread_exit(&ret);
}


void *vehiPPThreadFunc(void *pArg)
{
	int ret = 0;
	VEHI_PRO_INFO_MSG  strVehiInfo;
    
	ret = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
	if(0!=ret)
	{
		SYSHANDLE_DEBUG("pthread_setcancelstate() failed.\n");
		pthread_exit(&ret);
	}
	
	ret = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
	if(0!=ret)
	{
		SYSHANDLE_DEBUG("pthread_setcanceltype() failed.\n");
		pthread_exit(&ret);
	}
	writeThreadInfo("vehiPPPro");
	while(1)
	{
        ret = msgQReceive(pVehiPPQ, (char *)&strVehiInfo, sizeof(strVehiInfo), 30);
        if( sizeof(strVehiInfo) == ret )
		{
            switch(strVehiInfo.uiMsgType)
            {
                case MSG_KJ_VEHI:
                    kjEPppSnapSeq(strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta, \
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime);
                    break;
                case MSG_SJ_VEHI:
                    
                    break;
                case MSG_HGD_VEHI:
                    
                    break;
                case MSG_DC_VEHI:
                    dcEPppSnapSeq(strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarChannel,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilPos,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCoilSta, \
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.ucCarDir,\
                    strVehiInfo.stVehicleInfo.strDevProtocolSet.strKjProtocol.strKjProtoData.strCoilInfo.usLastTime);
                    break;
                default:
                    break;
            }
        }
	}
	pthread_exit(&ret);
}

int dctlThreadProInit(void)
{
    int ret =0;
    pthread_t  threadVehiNormal;
    pthread_t  threadVehiPP;
    /* 创建普通线程处理消息队列 */
    pVehiNormalQ = msgQCreate(60, 1024, 0);
    if (pVehiNormalQ == NULL )
    {
        printf("creat normal vehicle  pro message Queue failed .\n");
        SYSHANDLE_ERROR("creat normal vehicle  pro message Queue failed .\n");
    }
    else
    {
        SYSHANDLE_INFO("creat normal vehicle  pro message Queue ok .\n");
        printf("creat normal vehicle  pro message Queue ok .\n");
    }

    /* 创建卡口线程处理消息队列 */
    pVehiPPQ = msgQCreate(60, 1024, 0);
    if (pVehiPPQ == NULL )
    {
        printf("creat pp vehicle  pro message Queue failed .\n");
        SYSHANDLE_ERROR("creat pp vehicle  pro message Queue failed .\n");
    }
    else
    {
        SYSHANDLE_INFO("creat pp vehicle  pro message Queue ok .\n");
        printf("creat pp vehicle  pro message Queue ok .\n");
    }

    if( ret = pthread_create(&threadVehiNormal, NULL, vehiNormalThreadFunc, NULL) != 0)
    {
        SYSHANDLE_ERROR("%s,%s,%d\t vehicle normal Thread create failed;\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }

    if(ret = pthread_create(&threadVehiPP, NULL, vehiPPThreadFunc, NULL) != 0)
    {
        SYSHANDLE_ERROR("%s,%s,%d\t vehicle normal Thread create failed;\n", __FILE__, __FUNCTION__, __LINE__);
        return -1;
    }
    return 0;
}


