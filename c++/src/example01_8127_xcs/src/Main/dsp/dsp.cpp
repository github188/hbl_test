#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/time.h>

#include "McLog.h"
#include "dsp.h"
#include "dsp_service.h"

#include "dsp_define.h"

#include "SysConf.h"
#include "xcsGlobalDef.h"
#include "xcsCommDef.h"

#include "MmsSend.h"

#include "JpegSnap.h"
#include "FtpEpSend.h"
#include "FtpPpSend.h"

#include "dspParamManage.h"
#include "SysHandle.h"

#include "Record.h"
#include "DataUpLoad.h"
#include "RawRec.h"
#if defined(CYC200XZD)||defined(CYC500XZD)
#include "ccdCtl.h"
#else
#include "icx274CCDCtl.h"
#endif

#include "nMCtlApi.h"
#include "epRecApi.h"

#ifdef MODULE_ONVIF

#include "onvifSend.h"
#include "memShm.h"

#endif

#ifdef MODULE_NETREC

#include "Storage.h"
#include "memShm.h"

#endif


sem_t sem_dsp;
sem_t sem_dsp_data;

cam_info_t HDConf = {0};

extern char g_cTemperature;
extern CDataUpLoad *g_DataReport;
extern int          g_bTcpDataOk;

extern char g_cOnvifVideoSta;
extern char g_cNetRecVideoSta;

extern char g_cRecInitSta;
extern char g_cMmsInitSta;
extern char g_cFtpInitSta;
extern char g_cXcsInitSta;
extern char g_cCCDCtlSta;
extern char g_cAlarmRecSta;

extern char                 g_bForceRed[MAX_VEHICLE_NUM];   /* 模拟通道为红灯，优先级最高，只要为真即表示为红灯 */
extern LANE_SWITCH_STA_INFO  g_EPLaneSwitchInfo[MAX_VEHICLE_NUM]; /* 电警车道切换状态 */
extern PP_LANE_SWITCH_STA_INFO g_PPLaneSwitchInfo[MAX_VEHICLE_NUM]; /* 卡口车道切换状态 */
extern void mainAppShowDate();
extern int dspEpVLoopStepConf(void);
extern FtpPpSend *g_CPPFtp;

extern strCXProInfo g_strCXProInfo;

#if defined(CYC800JX)||defined(CYC800XKW4C)
    #define MAX_JPEG_PIC_LEN 0x200000
#else
    #define MAX_JPEG_PIC_LEN 0x180000
#endif

#define MIN_JPEG_PIC_LEN 0x400

/* 水印密码长度 */
#define PIC_WATERMARK_PWD_LEN 8

/* 当有小图片时，为了防止防篡改修改后续的小图片，所以加了一定的空余空间 */
#define SMALL_PIC_RESV_ROOM	0x400

#if defined(CYC500XKW4C)||defined(CYC500XAW)
extern FtpEpSend *g_CEPFtp;
#endif


int *g_debug_level      = NULL;
int g_dsp_debug_module  = 0xffffffff;

dsp_version_t sharelib_version_info, firmware_version_info, link_ep_version_info, link_lpr_version_info, link_osd_version_info, link_rlt_version_info;

/* 为了给报警录像提供FTP所需信息，所以采用全局变量保存DSP上传信息 */
typedef struct tag_gJpegSendInfo
{
    int          iAsfSendFlag;
    JpegSendInfo strEpJSendInfo;
}GJpedSendInfo;

GJpedSendInfo  g_strEpJSendInfo = {0};


/* 抓拍图片路径统计信息结构 */
#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)
extern STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[1];
#elif defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
extern STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[MAX_VEHICLE_NUM];
#else
extern STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[MAX_VEHICLE_NUM];
#endif

STR_VIDEO_RECSTA g_strVRSta;

int irregRecFileDetail(EP_Alarm_Rec_Frame_Info* pstrAlarmInfo);


//void RcvStreamCallBack(int iChannel, char* pBuffer, unsigned int dwBufSize, unsigned int type)
void RcvStreamCallBack(int iChannel, char* pBuffer, unsigned int dwBufSize, unsigned int dwUser)
//void RcvStreamCallBack(data_param_t *param)
{
#if 0
    /* 为了防止录像异常后网管频繁上报状态，所以记录异常次数 */
    int ret = 0;
    int len = 0;
    //int chanNo = param->chan_id;
    int chanNo = iChannel;
    unsigned char *addr =NULL;

	int iAlarmStrLen = sizeof(EP_Alarm_Rec_Frame_Info);

    sem_wait(&sem_dsp_data);
    //addr = (unsigned char *)(param->buf);
    addr = (unsigned char *)pBuffer;
    if(0 != (unsigned int)addr%4)
    {
        DSP_ERROR("-----------error:%s,%s,%d\t\t rcvStream addr is: 0x%x\n", __FILE__, __FUNCTION__, __LINE__,(unsigned int)addr);
    }
    //len = param->buf_len;
    len = dwBufSize;
    if ( len <= sizeof(VENC_DATA_FIELD) + sizeof(VIDEO_DATA_HEADER)+ sizeof(EP_Alarm_Rec_Frame_Info) )
    {
    	DSP_ERROR("error:%s,%s,%d\t buf len is too small.\n", __FILE__, __FUNCTION__, __LINE__, len);
        sem_post(&sem_dsp_data);
        return;
    }
    
    /* 由于帧头添加了违章报警结构体，所以缓存地址需要往后统一偏移,长度也要相应减少 */
	addr += iAlarmStrLen;
	len -= iAlarmStrLen;

    //{
        PVENC_DATA_FIELD pstrVencDataField = (PVENC_DATA_FIELD)(addr);
        //DSP_INFO("bitrate:%d,flags:%d,is_cbr:%d,is_pal:%d,seconds:%d.\n",pstrVencDataField->bitrate,pstrVencDataField->flags,\
        //pstrVencDataField->is_cbr,pstrVencDataField->is_pal,pstrVencDataField->seconds);
        //VIDEO_DATA_HEADER *video_data_header = (PVIDEO_DATA_HEADER)(addr + sizeof(VENC_DATA_FIELD) );
        //DSP_DEBUG(" chan %d timestamp=%d .\n",chanNo,video_data_header->timeStamp);
//        
        #if 0
        if(0x50 == video_data_header->frameType)
        {
            shell_print("package len:0x%x,I frame len is 0x%x\n", len, video_data_header->frameLength);
            mainAppShowDate();
        }
        else
        {
            //shell_print("P frame len is 0x%x,ts is 0x%x\n", video_data_header->frameLength, video_data_header->timeStamp);
        }
        #endif
        #if 0
        char szDefault[256];
        struct tm tmPlanStart;
        memset(szDefault,0x00,sizeof(szDefault));
        
        gmtime_r((time_t *)&(pstrVencDataField->seconds), &tmPlanStart);
        sprintf(szDefault,"%04d-%02d-%02d %02d:%02d:%02d",\
        tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
        tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec);
        shell_print("stream seconds:%s\n", szDefault);
        DSP_ERROR("timeStamp:0x%x\n", video_data_header->timeStamp);
        #endif
        //video_data_header->encType = 0x40;
//        shell_print(" chan %d len = %d frame len = %d .\n",chanNo,len,video_data_header->frameLength);
//        shell_print(" chan %d frame head :0x%x frame tail : 0x%x .\n",chanNo,video_data_header->headFlag,frame_tail);
    //}

    if(g_cMmsInitSta)
    {
        ret = MmsDataWrite(chanNo, addr+sizeof(VENC_DATA_FIELD), len-sizeof(VENC_DATA_FIELD), pstrVencDataField->bitrate);
        if ( 0 != ret )
        {
            DSP_ERROR("%s %d mms invalid chanNo %d ret %d.\n",__FUNCTION__,__LINE__,chanNo,ret);
        }
        else
        {
            // DSP_INFO("%s %d ret valid chanNo %d ret %d.\n",__FUNCTION__,__LINE__,chanNo,ret);
            g_strVRSta.iVideoSum[chanNo]++;
        }
    }
    
#ifdef MODULE_REC
    if ((chanNo == 0) && (g_cRecInitSta))
    {
        ret = RecDataWrite(chanNo,addr+sizeof(VENC_DATA_FIELD), len-sizeof(VENC_DATA_FIELD) );
        if ( 0!= ret)
        {
            DSP_ERROR("%s %d ret invalid chanNo %d ret %d.\n",__FUNCTION__,__LINE__,chanNo,ret);
        }
        else
        {
            g_strVRSta.iRecSum[chanNo]++;
        }
    }
#endif

#ifdef MODULE_ALARMREC
    #if defined(CYC500XKW4C)||defined(CYC500XAW)
    if ( (chanNo == 0) && (g_cAlarmRecSta) )
    {
		/* 进行违章信息处理 */
		EP_Alarm_Rec_Frame_Info* pstrAlarmInfo = NULL;

		pstrAlarmInfo = (EP_Alarm_Rec_Frame_Info *)(pBuffer);
		irregRecFileDetail(pstrAlarmInfo);

        ret = epRecModuleFrameInput((XCS_FRAME *)(addr+sizeof(VENC_DATA_FIELD)), len-sizeof(VENC_DATA_FIELD) );
        if ( 0 > ret)
        {
            DSP_ERROR("%s %d ep rec failed, ret %d.\n",__FUNCTION__,__LINE__, ret);
        }
    }
    #endif
#endif

#if 1
    long data_len = 0;
    void *shareBuff = NULL;
    unsigned char *p_frame = addr+sizeof(VENC_DATA_FIELD);
#endif

#ifdef MODULE_ONVIF
    static int s_iCount =0;
    
    data_len = (len - sizeof(VENC_DATA_FIELD) + 3)&~0x3; //确保帧长度是4字节对齐

    if(chanNo < TOTAL_CHANNEL_NUM)
    {
        s_iCount++;

        shareBuff =  memShmMalloc(data_len);
        if (!shareBuff)
        {
        	if( s_iCount % 83 == 0 )
            {
            	DSP_ERROR("%s %d DSPCOMM: memShmMalloc failed data_len:%d chan:%d\n", __FUNCTION__, __LINE__, data_len,chanNo);
				//syslog(LOG_ERR|LOG_USER,"%s %d DSPCOMM: memShmMalloc failed data_len:%d chan:%d\n", __FUNCTION__, __LINE__, data_len,chanNo);
			}
            sem_post(&sem_dsp_data);
            return;
        }
        else
        {
            if( s_iCount % 83 == 0 )
            {
                DSP_INFO("memShmMalloc ok data_len:%d chan:%d\n", data_len,chanNo);
            }
        }

        memcpy(shareBuff, p_frame, data_len);
        //memset(shareBuff, s_iCount, data_len/4);
        //DSP_INFO("shareBuff data is 0x%x\n", *(int *)shareBuff);

        int offset = memShmOffset(shareBuff);
    
        STREAMDATA msg = {0, 0, 0};

    	msg.nChannel = chanNo;
    	msg.offset = offset;
    	msg.nLen = data_len;

        ret = args_send( (const char *)&msg, sizeof(msg));
        if(ret < 0)
        {
            memShmFree(shareBuff);
            if( s_iCount % 83 == 0 )
            {
				DSP_INFO("onvif send args message failed, ret:%d, errno:%d,errorstr:%s,reInit socket\n", ret, errno, strerror(errno));		
          		syslog(LOG_ERR|LOG_USER, "onvif send args message failed, ret:%d, errno:%d,errorstr:%s,reInit socket\n", ret, errno, strerror(errno));		
			}
        }
        else
        {
            if( s_iCount % 83 == 0 )
            {
                DSP_INFO("onvif args_send ok,count:%d.\n", s_iCount);
            }
        }
    }
#endif

#ifdef MODULE_NETREC
    data_len = (len - sizeof(VENC_DATA_FIELD) + 3)&~0x3; //确保帧长度是4字节对齐

    if(chanNo < TOTAL_CHANNEL_NUM)
    {
        shareBuff =  memShmMalloc(data_len);
        if (!shareBuff)
        {
            DSP_ERROR("%s %d DSPCOMM: memShmMalloc failed data_len:%d chan:%d\n", __FUNCTION__, __LINE__, data_len,chanNo);
            sem_post(&sem_dsp_data);
            return;
        }
        else
        {
        	DSP_INFO("%s %d DSPCOMM: memShmMalloc ok data_len:%d chan:%d\n", __FUNCTION__, __LINE__, data_len,chanNo);
        }
        
        memcpy(shareBuff, p_frame, data_len);

		ret = StorageDataWrite(chanNo, shareBuff, data_len);
		if(ret < 0)
		{
			DSP_INFO("%s %d chan:%d,StorageDataWrite failed,ret:%d.\n", __FUNCTION__, __LINE__, chanNo, ret);
			memShmFree(shareBuff);
		}
		else
		{
			DSP_INFO("%s %d chan:%d,StorageDataWrite ok,ret:%d.\n", __FUNCTION__, __LINE__, chanNo, ret);
		}
    }
#endif

    sem_post(&sem_dsp_data);
#endif
#if 1
    int ret = 0;
    int len = 0;
    int chanNo = iChannel;
    unsigned char *addr =NULL;
	int iAlarmStrLen = sizeof(EP_Alarm_Rec_Frame_Info);
    sem_wait(&sem_dsp_data);

    addr = (unsigned char *)pBuffer;
 
    len = dwBufSize;
    if ( len <= sizeof(VENC_DATA_FIELD) + sizeof(VIDEO_DATA_HEADER) )
    {
    	DSP_ERROR("error:%s,%s,%d\t buf len is too small.\n", __FILE__, __FUNCTION__, __LINE__, len);
        sem_post(&sem_dsp_data);
        return;
    }
    
    /* 由于帧头添加了违章报警结构体，所以缓存地址需要往后统一偏移,长度也要相应减少 */
///	addr += iAlarmStrLen;
///	len -= iAlarmStrLen;
	PVENC_DATA_FIELD pstrVencDataField = (PVENC_DATA_FIELD)(addr);
    if(g_cMmsInitSta)
    {
        ret = MmsDataWrite(0, addr+sizeof(VENC_DATA_FIELD), len-sizeof(VENC_DATA_FIELD),4000);
        if ( 0 != ret )
        {
            DSP_ERROR("%s %d mms invalid chanNo %d ret %d.\n",__FUNCTION__,__LINE__,chanNo,ret);
        }
        else
        {
            // DSP_INFO("%s %d ret valid chanNo %d ret %d.\n",__FUNCTION__,__LINE__,chanNo,ret);
            g_strVRSta.iVideoSum[chanNo]++;
        }
    }

    sem_post(&sem_dsp_data);

#if 1  
	#ifdef MODULE_ONVIF
	 long data_len = 0;
    void *shareBuff = NULL;
    unsigned char *p_frame = addr+sizeof(VENC_DATA_FIELD);
    static int s_iCount =0;
    
    data_len = (len - sizeof(VENC_DATA_FIELD) + 3)&~0x3; //确保帧长度是4字节对齐

    if(chanNo < TOTAL_CHANNEL_NUM)
    {
        s_iCount++;

        shareBuff =  memShmMalloc(data_len);
        if (!shareBuff)
        {
        	if( s_iCount % 83 == 0 )
            {
            	DSP_ERROR("%s %d DSPCOMM: memShmMalloc failed data_len:%d chan:%d\n", __FUNCTION__, __LINE__, data_len,chanNo);
				//syslog(LOG_ERR|LOG_USER,"%s %d DSPCOMM: memShmMalloc failed data_len:%d chan:%d\n", __FUNCTION__, __LINE__, data_len,chanNo);
			}
            sem_post(&sem_dsp_data);
            return;
        }
        else
        {
            if( s_iCount % 83 == 0 )
            {
                DSP_INFO("memShmMalloc ok data_len:%d chan:%d\n", data_len,chanNo);
            }
        }

        memcpy(shareBuff, p_frame, data_len);
        //memset(shareBuff, s_iCount, data_len/4);
        //DSP_INFO("shareBuff data is 0x%x\n", *(int *)shareBuff);

        int offset = memShmOffset(shareBuff);
    
        STREAMDATA msg = {0, 0, 0};

    	msg.nChannel = chanNo;
    	msg.offset = offset;
    	msg.nLen = data_len;

        ret = args_send( (const char *)&msg, sizeof(msg));
        if(ret < 0)
        {
            memShmFree(shareBuff);
            if( s_iCount % 83 == 0 )
            {
				DSP_INFO("onvif send args message failed, ret:%d, errno:%d,errorstr:%s,reInit socket\n", ret, errno, strerror(errno));		
          		syslog(LOG_ERR|LOG_USER, "onvif send args message failed, ret:%d, errno:%d,errorstr:%s,reInit socket\n", ret, errno, strerror(errno));		
			}
        }
        else
        {
            if( s_iCount % 83 == 0 )
            {
                DSP_INFO("onvif args_send ok,count:%d.\n", s_iCount);
            }
        }
    }
#endif

 #endif
 
 #endif
}



int dspSetTimePos(int chanid,int startX,int startY)
{
    int ret;
    time_param_t param;
    memset(&param, 0, sizeof(param));
	param.chan_id = chanid;
    param.startX = startX;
    param.startY = startY;

    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_OSD_TIME_TITLE, param);
    //sem_post(&sem_dsp);

    return ret;

}

#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
static int dspStationConfParam()
{
    unsigned int i =0;
    int ret =0;
    int chanid = 0;

    ret = camCtlParamRunJudge(CAM_PARAM_MODE_FORCE);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d cam param set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:camCtlParamRunJudge ok\n");
    }

    ret = dspEncParamSet(chanid);
    if( 0!= ret)
    {
        DSP_ERROR("error:%s %d enc param set. ret:%d\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:pp enc param set ok\n");
    }
    
    ret = dspSetTimePos(chanid, 0x30, 0x60);   
    if ( 0!=ret)
    {
        DSP_ERROR("%s %d set TimePos ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dspSetTimePos ok");
    }
    
    ret = dspLPRParamSet(0);
    if ( 0!=ret)
    {
        DSP_ERROR("%s %d set LPR param ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dspLPRParamSet ok");
    }

    ret = dspPPIrregNameInfoSet();
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d set pp irregNmae.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspPPIrregNameInfoSet ok\n");
    }

    ret = dspPpJpgOsdConfSet(0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d dspPpJpgOsdConfSet set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspPpJpgOsdConfSet ok\n");
    }
    
    for(i =0; i< g_uiClientLaneSum; i++)
    {
        ret = dspPPLimitSpeedSet(chanid, i);
        if ( 0 != ret)
        {
            DSP_ERROR("%s %d set Snap limit speed ret=%d error .\n",__FUNCTION__,__LINE__,ret);
        }
        else
        {
            DSP_INFO("dspPPLimitSpeedSet ok");
        }
        ret = dspPPVLParamSet(chanid, i);
        if ( 0!=ret)
        {
            DSP_ERROR("%s %d lane:%d station vl param set,ret=%d error .\n",__FUNCTION__,__LINE__,i, ret);
        }
        else
        {
            DSP_INFO("lane:%d,Set Station VL Param Ok.\n", i);
        }
        ret = dspPPSnapAreaSet(chanid, i);
        if ( 0!=ret)
        {
            DSP_ERROR("%s %d lane:%d, pp snap area set,ret=%d error .\n",__FUNCTION__,__LINE__, i, ret);
        }
        else
        {
            DSP_INFO("lane:%d Set dspPPSnapAreaSet Ok.\n", i);
        }
        ret = dspPPCoilTypeSet(chanid, i);
        if ( 0!=ret)
        {
            DSP_ERROR("%s %d lane:%d,station coil type set,ret=%d error .\n",__FUNCTION__,__LINE__, i, ret);
        }
        else
        {
            DSP_INFO("lane:%d dspPPCoilTypeSet Ok.\n", i);
        }
        ret = dspPPSnapOsdSet(chanid, i);
        if ( 0 != ret)
        {
            DSP_ERROR("%s %d set osdSnap laneId:%d, ret=%d error .\n", __FUNCTION__, __LINE__,i, ret);
        }
        else
        {
            DSP_INFO("lane:%d,dspPPSnapOsdSet ok!\n", i);
        }
        ret = dspPPUnidirLaneTimeSet(chanid, i);
        if ( 0 != ret)
        {
            DSP_ERROR("%s %d set dspPPUnidirLaneTimeSet laneId:%d, ret=%d error .\n", __FUNCTION__, __LINE__,i, ret);
        }
        else
        {
            DSP_INFO("lane:%d,dspPPUnidirLaneTimeSet ok!\n", i);
        }
        
    }
    dspPPVSpeedFlagSet();

	ret = dspPpFunEnSet(0,0);
	if ( 0!=ret)
	{
		DSP_ERROR("error:%s %d. ret=%d error .\n",__FUNCTION__,__LINE__,ret);
	}
	else
	{
	    DSP_INFO("dsp:dspPpFunEnSet ok\n");
	}

	/* modify by wangb 2010-2-22 for 为了节约DSP性能，在不连接视频时不进行编码，若有录像则一直编码 */
	ret = dspEncStaCtl(chanid, 0);
	if ( 0!=ret)
	{
		DSP_ERROR("%s %d set vstatus ret=%d error .\n",__FUNCTION__,__LINE__,ret);
	}
	else
	{
	    DSP_INFO("start enc ok\n");
	}
	DSP_INFO("Set Station Conf Ok.\n");
	return 0;
}
#endif


static int dspFlowRateConf()
{
    int iRet =0;
    unsigned int iLaneId =0;
    FlowStatInfo strFSInfo;
    memset(&strFSInfo, 0, sizeof(strFSInfo));
    iRet = epFlowRateFlagGet();
    if( iRet > 0 )
    {
        iRet = epFlowRateInterGet();
        strFSInfo.inter_time = 60*iRet;
        strFSInfo.report_flag = 1;
        //sem_wait(&sem_dsp);
        for(iLaneId=0; iLaneId < g_uiCamLaneSum; iLaneId++)
        {
            strFSInfo.lane_id = iLaneId;
            CALL_CYC_CMD(NET_DEV_SET_CAR_FLOWSTAT, strFSInfo);
        }
        //sem_post(&sem_dsp);
    }
    return iRet;
}

void epFlowRateHandle(flowstat_status_t *param)
{
    int ret =0;
    JpegSendInfo  strEpJSendInfoTmp;
    memset(&strEpJSendInfoTmp, 0, sizeof(strEpJSendInfoTmp));

    ELEC_POLICE_VEHICLE_CONF_INFO strEPChanConf;
    ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
    memset(&strEPChanConf, 0, sizeof(strEPChanConf));
    memset(&strEPGlobalConf, 0, sizeof(strEPGlobalConf));
    
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPChanConf, param->lane_id);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get ep vehicle conf failed!\n",__FUNCTION__,__LINE__);
    }

    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, param->lane_id);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get ep overall conf failed!\n",__FUNCTION__,__LINE__);
    }
    
    sem_wait(&sem_dsp_data);
    ftpEPLaneNameSet(&strEpJSendInfoTmp, param->lane_id);
    strEpJSendInfoTmp.iLaneIndex = param->lane_id;
    strEpJSendInfoTmp.cFileType = TRV_FILE_TYPE;
    strEpJSendInfoTmp.iPicNum = 0;
    strEpJSendInfoTmp.uiStatisBeginTime = param->begin_time;
    strEpJSendInfoTmp.uiStatisEndTime = param->end_time;
    strEpJSendInfoTmp.uiTrafficVolume = param->flow_num;
    strEpJSendInfoTmp.uiSnapTimeSecs = param->begin_time;
    strEpJSendInfoTmp.uiSnapTimeMsSecs = 0;

    strcpy(strEpJSendInfoTmp.szDirectionId, strEPGlobalConf.szDirId);
    strcpy(strEpJSendInfoTmp.szPortId, strEPGlobalConf.szRoadId);
    strcpy(strEpJSendInfoTmp.szLaneId, strEPChanConf.szLaneId);

    DSP_INFO("%s %d laneId:%d, flow sum:%d\n",__FUNCTION__,__LINE__, param->lane_id, param->flow_num);
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    ret = g_CEPFtp->FtpSendPicture(&strEpJSendInfoTmp);	// tangzl 0725
    if( !ret )
    {
        DSP_ERROR("%s %d ftp send flow rate failed.\n",__FUNCTION__,__LINE__);
    }
    else
    {
        DSP_INFO("%s %d ftp send flow rate ok.len:%d\n",__FUNCTION__,__LINE__, ret);
    }
#endif
    sem_post(&sem_dsp_data);
}

static int dspEPoliceConfParam()
{
    int ret =0;
    int chanid = 0;

    ret = dspEncParamSet(chanid);
    if( 0!= ret)
    {
        DSP_ERROR("error:%s %d enc param set. ret:%d\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:ep enc param set ok\n");
    }
    ret = dspSetTimePos(chanid, 0x30, 0x60);
    if ( 0!=ret)
    {
        DSP_ERROR("%s %d set TimePos ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    ret = dspEpPpSnapOsdSet(ALL_LANE_SET_MODE, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d ep pp snap osd set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEpPpSnapOsdSet ok\n");
    }

	ret = dspLPRParamSet(0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d pp lpr param set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspLPRParamSet ok\n");
    }

    ret = dspEpLPRParamSet(0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d ep lpr param set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEpLPRParamSet ok\n");
    }

    ret = dspEpJpgOsdConfSet(0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d dspEpJpgOsdConfSet set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEpJpgOsdConfSet ok\n");
    }

    ret = dspPpJpgOsdConfSet(0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d dspPpJpgOsdConfSet set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspPpJpgOsdConfSet ok\n");
    }

    ret = dspEPSnapOsdSet(ALL_LANE_SET_MODE, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d ep snap osd set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEPSnapOsdSet ok\n");
    }
    ret = dspEPRedLightAreaSet(RL_NORMAL_SET_MODE, 0);
    if ( 0 != ret)
    {
        DSP_ERROR("%s %d set red light area ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    } 
    ret = dspEPVLAreaSet(ALL_LANE_SET_MODE, 0);
    if ( 0 != ret)
    {
        DSP_ERROR("%s %d set virtual coil area ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    } 
    ret = dspEPLaneAreaSet(ALL_LANE_SET_MODE, 0);
    if ( 0 != ret)
    {
        DSP_ERROR("%s %d set lane area ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    } 
    
    ret = camCtlParamRunJudge(CAM_PARAM_MODE_FORCE);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d cam param set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:camCtlParamRunJudge ok\n");
    }

    for(unsigned int iLaneId =0; iLaneId < g_uiCamLaneSum; iLaneId++)
    {
        epTimeRangeJudge(iLaneId);
    }
    ret = dspEPRedLightSet(ALL_LANE_SET_MODE, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("%s %d ret=%d error.\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("set red light ok!\n");
    }

    DSP_INFO("Set Electronic Police Conf Ok,start enc...\n");

    /* 添加流量统计及禁左禁右功能 */
    dspFlowRateConf();
    //dsp_service_hook_flowstat_control(epFlowRateHandle);

    /* 添加虚拟线圈抓拍步数间隔设置 */
    dspEpVLoopStepConf();

    ret = dspEpFuncEn(ALL_LANE_SET_MODE, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d. ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEpFuncEn ok\n");
    }

    ret = dspEPCoilTypeSet(ALL_LANE_SET_MODE, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d coil type set.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEPCoilTypeSet ok\n");
    }

    ret = dspPPIrregNameInfoSet();
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d set pp irregNmae.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspPPIrregNameInfoSet ok\n");
    }

    ret = dspEPIrregNameInfoSet();
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d set ep irregNmae.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEPIrregNameInfoSet ok\n");
    }

    ret = dspEPSOTNoEntrySet(ALL_LANE_SET_MODE, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("error:%s %d dspEPSOTNoEntrySet.ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("dsp:dspEPSOTNoEntrySet ok\n");
    }


    /* modify by wangb 2010-12-30 for 为了节约DSP性能，在不连接视频时不进行编码，若有录像则一直编码 */
    ret = dspEncStaCtl(chanid, 0);
    if ( 0!=ret)
    {
        DSP_ERROR("%s %d set vstatus ret=%d error .\n",__FUNCTION__,__LINE__,ret);
    }
    else
    {
        DSP_INFO("start enc ok\n");
    }
    
    return 0;
}

/******************************************************************************
 * SendGroupPicToServer() :
 * 
 * DESCRIPTION:发送一组图片到web
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2013.12.12, ITS Create
 * --------------------
 ******************************************************************************/
int SendGroupPicToServer(SNAP_INFORM *strPicInfo)
{
    INFORM_CONTENT  inform;
    SNAP_INFORM     snapinform;
    PASSPORT_SNAP_PARAM ppsnap;

    memset(&inform, 0, sizeof(inform));
    memset(&snapinform, 0, sizeof(snapinform));
    memset(&ppsnap, 0, sizeof(ppsnap));

	snapinform = *strPicInfo;
    
    inform.strInformParam.uiVersion = 2;
    inform.strInformParam.uiType    = E_PASSPORT_SNAP;
    inform.strInformParam.tTime     = 0x12345678;
    inform.pInformContent = (char *)(&snapinform);

    
    snapinform.uiParamLen = sizeof(PASSPORT_SNAP_PARAM);
    snapinform.pucParamData = ( char *)(&ppsnap);
   
    memset(&ppsnap,0x00,sizeof(ppsnap));
    
    ppsnap.uiSubVersion = 2;
    ppsnap.usCarLen=10;
    ppsnap.usSpeed = 100;
    ppsnap.strLienceArea.usAreaPosX =   0x55;
    ppsnap.strLienceArea.usAreaPosY =   0x66;
    ppsnap.strLienceArea.usAreaWidth =  0x77;
    ppsnap.strLienceArea.usAreaHeight = 0x88;
    if(g_cXcsInitSta)
    {
        g_DataReport->UpLoadData(&inform);   
    }
    else
    {
        DSP_ERROR("data report init failed\n");
    }
    return 0;
}

/******************************************************************************
 * SendPicToServer() :
 * 
 * DESCRIPTION:  有一些参数写死了，需要的话可以进行修改
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.26, wangbin Create
 * --------------------
 ******************************************************************************/
int SendPicToServer(char *pbuff,unsigned int len)
{
    INFORM_CONTENT  inform;
    SNAP_INFORM     snapinform;
    PASSPORT_SNAP_PARAM ppsnap;

    memset(&inform, 0, sizeof(inform));
    memset(&snapinform, 0, sizeof(snapinform));
    memset(&ppsnap, 0, sizeof(ppsnap));
    
    inform.strInformParam.uiVersion = 2;
    inform.strInformParam.uiType    =E_PASSPORT_SNAP;
    inform.strInformParam.tTime     = 0x12345678;
    inform.pInformContent = (char *)(&snapinform);

    
    snapinform.uiParamLen = sizeof(PASSPORT_SNAP_PARAM);
    snapinform.pucParamData = ( char *)(&ppsnap);
    snapinform.ucPicNum     = 1;
   
   // len = 100;
    snapinform.auiPicLen = &len;
    snapinform.pszPicData   =pbuff;
  //  strcpy(snapinform.pszPicData[0],buff);
    

    memset(&ppsnap,0x00,sizeof(ppsnap));
    
    ppsnap.uiSubVersion = 2;
    ppsnap.usCarLen=10;
    ppsnap.usSpeed = 100;
    ppsnap.strLienceArea.usAreaPosX =   0x55;
    ppsnap.strLienceArea.usAreaPosY =   0x66;
    ppsnap.strLienceArea.usAreaWidth =  0x77;
    ppsnap.strLienceArea.usAreaHeight = 0x88;
    if(g_cXcsInitSta)
    {
    	printf("#### 发送图片到网页\n");
        g_DataReport->UpLoadData(&inform);   
    }
    else
    {
        DSP_ERROR("data report init failed\n");
    }
    return 0;
}

int xcs500xMemAsfFileGet(int iRecType, void **pucBuf)
{
    int iFLen =0;
    int iAsfSta =0;
    
    epRecModuleFileExitStaGet(iRecType, &iAsfSta);
    if(iAsfSta <= 0)
    {
        DSP_ERROR("error,%s %d asf file not exit.\n",__FUNCTION__,__LINE__);
        return -1;
    }
    iFLen = epRecModuleMemFileRead(iRecType, pucBuf);
    if(iFLen > 0)
    {
        DSP_INFO("%s %d read ep mem rec file ok,len is %d!\n",__FUNCTION__,__LINE__, iFLen);
    }
    else
    {
        DSP_INFO("%s %d read ep mem rec file failed,ret is %d!\n",__FUNCTION__,__LINE__, iFLen);
    }
    return iFLen;
}




#if defined(CYC500XKW4C)||defined(CYC500XAW)
static int XCS500RFillJpegInfo(JpegSendInfo *pStrJSendInfo,VEHICLE_DETECT_HEADER *pVehicleHead,unsigned char ucSnapType)
{
    int ret =0;
    int iPicMixType = dspEpSnapTypeGet(0);
    ELEC_POLICE_VEHICLE_CONF_INFO strEPChanConf;
    ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
    memset(&strEPChanConf, 0, sizeof(strEPChanConf));
    memset(&strEPGlobalConf, 0, sizeof(strEPGlobalConf));
    
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPChanConf, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get ep vehicle conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }

    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get ep overall conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }

    if (SNAP_TYPE_6_PLUS_1 == iPicMixType )
    {
        iPicMixType = 6;
    }
    else if (( SNAP_TYPE_3_PLUS_1 == iPicMixType) || ( SNAP_TYPE_EP_REVERSE == iPicMixType )\
    ||( SNAP_TYPE_3_PLUS_1_LEFT == iPicMixType) )
    {
        iPicMixType = 3;
    }
    else if( SNAP_TYPE_3_SEPARATE == iPicMixType )
    {
        iPicMixType = 0;
    }
    else if( SNAP_TYPE_4_SEPARATE == iPicMixType )
    {
        iPicMixType = 4;
    }
    else
    {
        iPicMixType = 0;
    }

    pStrJSendInfo->iCombineType = iPicMixType;
    /* 暂时规定电警330平台接入车道号固定为3，注意FTP模块也要做相应修改 */
    //pStrJSendInfo->iLaneIndex = 3;
    pStrJSendInfo->iLaneIndex = pVehicleHead->lane;
    DSP_INFO("%s %d 500t laneIndex:0x%x.\n",__FUNCTION__,__LINE__, pStrJSendInfo->iLaneIndex);
    pStrJSendInfo->strParam.pos_info.m_left = pVehicleHead->rect.left;
    pStrJSendInfo->strParam.pos_info.m_top = pVehicleHead->rect.top;
    pStrJSendInfo->strParam.pos_info.m_bottom = pVehicleHead->rect.bottom;
    pStrJSendInfo->strParam.pos_info.m_right = pVehicleHead->rect.right;
    pStrJSendInfo->strParam.pos_info.m_type = pVehicleHead->vehicleType;

    pStrJSendInfo->strParam.pos_info.m_confidence = pVehicleHead->confidence;


    pStrJSendInfo->strParam.SnapPicInfo.ep_snap_info.m_speed = pVehicleHead->speed;
    pStrJSendInfo->strParam.SnapPicInfo.ep_snap_info.m_speed_thresh = pVehicleHead->maxspeed;
    
    pStrJSendInfo->strParam.SnapPicInfo.ep_snap_info.m_redlight_begin = pVehicleHead->redLightSeconds;
    pStrJSendInfo->strParam.SnapPicInfo.ep_snap_info.m_noturnbegintime = pVehicleHead->redLightSeconds;
    pStrJSendInfo->strParam.SnapPicInfo.ep_snap_info.m_redlight_duration = pVehicleHead->redLightDuration;
    pStrJSendInfo->cChanDir = strEPChanConf.cLaneDir;
    pStrJSendInfo->cFileType = JPG_FILE_TYPE;

    pStrJSendInfo->ucSnapType = ucSnapType;

    strcpy(pStrJSendInfo->szDirectionId, strEPGlobalConf.szDirId);
    strcpy(pStrJSendInfo->szPortId, strEPGlobalConf.szRoadId);
    strcpy(pStrJSendInfo->szLaneId, strEPChanConf.szLaneId);
    pStrJSendInfo->uiXmlType = pVehicleHead->DataType;
    pStrJSendInfo->cEpPpSelStep0Or1 = eptPpStepNumGet();

    pStrJSendInfo->lpColor        = pVehicleHead->lpColor;
    pStrJSendInfo->lpConfidenct   = pVehicleHead->lpConfidenct;
    pStrJSendInfo->lpFCConfidenct = pVehicleHead->lpFCConfidenct;
    pStrJSendInfo->lpRect.start_x = pVehicleHead->lpRect.left;
    pStrJSendInfo->lpRect.start_y = pVehicleHead->lpRect.top;
    pStrJSendInfo->lpRect.width   = pVehicleHead->lpRect.right - pVehicleHead->lpRect.left;
    pStrJSendInfo->lpRect.height  = pVehicleHead->lpRect.bottom - pVehicleHead->lpRect.top;
    pStrJSendInfo->LPREnable      = pVehicleHead->LPREnable;
    pStrJSendInfo->lpType         = pVehicleHead->lpType;
    strcpy(pStrJSendInfo->lpStr, pVehicleHead->lpStr);

    memcpy( pStrJSendInfo->strSanpPicTime, pVehicleHead->strSnapPicTime, sizeof( pVehicleHead->strSnapPicTime ));
    /*Start of 最后一张图片的时间减去红灯起始时间小于红灯持续时间，则修改红灯持续时间 ITS on 2012-9-29 15:17 2.0.0.1*/

    if( ( pStrJSendInfo->strSanpPicTime[2].uiSec - pVehicleHead->redLightSeconds ) > pVehicleHead->redLightDuration)
    {
        pVehicleHead->redLightDuration = pStrJSendInfo->strSanpPicTime[2].uiSec - pVehicleHead->redLightSeconds +3;
        pStrJSendInfo->strParam.SnapPicInfo.ep_snap_info.m_redlight_duration = pVehicleHead->redLightDuration;
    }
    
    /*End of ITS on 2012-9-29 15:17 2.0.0.1*/
    /*
    DSP_ERROR("%s %d 500t laneIndex:0x%x. dataType:%d.\n",__FUNCTION__,__LINE__, \
    pStrJSendInfo->iLaneIndex, pVehicleHead->DataType);
    */
    //轨迹
    g_strCXProInfo.cDriverTrack = pVehicleHead->cDriverTrack;//
    //温度 
    g_strCXProInfo.cDevTemprature = g_cTemperature;

    g_strCXProInfo.cLaneId = atoi(strEPChanConf.szLaneId);
	
    pStrJSendInfo->strFtpCXProInfo = g_strCXProInfo;

    return 0;
}

void alarmRecFileSend(char *pcLaneId)
{
    int ret =0;
    char *ppBuf = NULL;
    JpegSendInfo  strEpJSendInfoTmp;
#ifdef MODULE_ALARMREC
    if(g_cAlarmRecSta)
    {
        strEpJSendInfoTmp = g_strEpJSendInfo.strEpJSendInfo;
        //epRecModuleRecStaSet(EP_REC_TYPE, EP_REC_STATE_END, ucLaneId);
        ret = xcs500xMemAsfFileGet(EP_REC_TYPE, (void **)&ppBuf);
        if(0 < ret)
        {
            strEpJSendInfoTmp.cFileType = ASF_FILE_TYPE;
            strEpJSendInfoTmp.pcPicBuf = ppBuf;
            strEpJSendInfoTmp.iCurrPicIndex = strEpJSendInfoTmp.iPicNum;
            strEpJSendInfoTmp.auiPicLen[strEpJSendInfoTmp.iCurrPicIndex] = ret;
            //if( g_strEpJSendInfo.iAsfSendFlag == 1)
            {
                ret = g_CEPFtp->FtpSendPicture(&strEpJSendInfoTmp);	// tangzl 0725
                if( !ret )
                {
                    DSP_INFO("lane_%d send asf file failed.fileType:%d,index:%d,picNumSum:%d,groupId:%d,file_len:%d\n", \
                    strEpJSendInfoTmp.iLaneIndex, strEpJSendInfoTmp.cFileType, \
                    strEpJSendInfoTmp.iCurrPicIndex,strEpJSendInfoTmp.iPicNum,\
                    strEpJSendInfoTmp.iGroupId,\
                    strEpJSendInfoTmp.auiPicLen[strEpJSendInfoTmp.iCurrPicIndex]);
                }
                else
                {
                    DSP_INFO("lane_%d send asf file ok.fileType:%d,index:%d,picNumSum:%d,groupId:%d,file_len:%d\n", \
                    strEpJSendInfoTmp.iLaneIndex, strEpJSendInfoTmp.cFileType, \
                    strEpJSendInfoTmp.iCurrPicIndex,strEpJSendInfoTmp.iPicNum,\
                    strEpJSendInfoTmp.iGroupId,\
                    strEpJSendInfoTmp.auiPicLen[strEpJSendInfoTmp.iCurrPicIndex]);
                }
            }
            /* 删除asf文件 */
            epRecModuleFileExitStaSet(EP_REC_TYPE, 0);
            //g_strEpJSendInfo.iAsfSendFlag = 0;
        }
        else
        {
            DSP_INFO("%s %d first get asf file failed.\n",__FUNCTION__,__LINE__);
        }
    }
#endif /* MODULE_ALARMREC */
}

int irregRecFileDetail(EP_Alarm_Rec_Frame_Info* pstrAlarmInfo)
{
	int i =0;
    int iRecSta =0;
    unsigned char ucLaneId =0;
    JpegSendInfo strCurSnapInfo;

	memset(&strCurSnapInfo, 0, sizeof(strCurSnapInfo));
	
	if(pstrAlarmInfo == NULL)
	{
		DSP_ERROR("%s %d inparam is null,dir return.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	if( pstrAlarmInfo->iCarSum <= 0)
	{
		//DSP_ERROR("%s %d inparam is null,dir return.\n",__FUNCTION__,__LINE__);
		return 0;
	}
	else
	{
		for(i=0; i< pstrAlarmInfo->iCarSum; i++)
		{
			DSP_INFO("%s %d carNo:%d,iCarSum:%d,lane:%d,snapTimeSec:%d,snapTimeMsec:%d,goupId:%d,picNum:%d,irregFlag:%d.\n",\
			__FUNCTION__,__LINE__,i,pstrAlarmInfo->iCarSum,\
			pstrAlarmInfo->aStrCarInfo[i].uiLane,\
			pstrAlarmInfo->aStrCarInfo[i].uiSnapSeconds,\
			pstrAlarmInfo->aStrCarInfo[i].uiSnapMSeconds,\
			pstrAlarmInfo->aStrCarInfo[i].uiSnapGroup,\
			pstrAlarmInfo->aStrCarInfo[i].uiSnapPicSum,\
			pstrAlarmInfo->aStrCarInfo[i].uiIrregFlag);
			
	        if( pstrAlarmInfo->aStrCarInfo[i].uiIrregFlag > 0 )
	        {
				ucLaneId = pstrAlarmInfo->aStrCarInfo[i].uiLane;
			    ftpEPLaneNameSet(&strCurSnapInfo, ucLaneId);
				strCurSnapInfo.iLaneIndex = pstrAlarmInfo->aStrCarInfo[i].uiLane;
				strCurSnapInfo.uiSnapTimeSecs = pstrAlarmInfo->aStrCarInfo[i].uiSnapSeconds;
				strCurSnapInfo.uiSnapTimeMsSecs = pstrAlarmInfo->aStrCarInfo[i].uiSnapMSeconds;
				strCurSnapInfo.iGroupId = pstrAlarmInfo->aStrCarInfo[i].uiSnapGroup;
				strCurSnapInfo.iPicNum = pstrAlarmInfo->aStrCarInfo[i].uiSnapPicSum;
				strCurSnapInfo.uiXmlType = pstrAlarmInfo->aStrCarInfo[i].uiIrregFlag;
				
	            epRecModuleRecStaGet(EP_REC_TYPE, &iRecSta, ucLaneId);
	            DSP_INFO("%s %d lane:%d alarm rec sta is %d\n", __FUNCTION__,__LINE__,ucLaneId, iRecSta);
	            
	            if(iRecSta == EP_REC_STATE_BEGIN)
	            {
	                /* 先结束上次报警录像 */
	                epRecModuleRecStaSet(EP_REC_TYPE, EP_REC_STATE_END, ucLaneId);

                    strCurSnapInfo.cAsfSendFalg = 1;
                    /* 再开启本次报警录像 */
                    DSP_INFO("end last alarm rec,laneId:%d\n", ucLaneId);
                    g_strEpJSendInfo.strEpJSendInfo = strCurSnapInfo;
                    epRecModuleRecStaSet(EP_REC_TYPE, EP_REC_STATE_BEGIN, ucLaneId);
	            }
	            else
	            {
	                g_strEpJSendInfo.strEpJSendInfo = strCurSnapInfo;                

	                /* 直接开启本次报警录像 */
	                epRecModuleRecStaSet(EP_REC_TYPE, EP_REC_STATE_BEGIN, ucLaneId);
	                DSP_INFO("begin rec g_param laneId:%d\n", ucLaneId);
	            }
	        }
		}
	}
	return 1;
}

void XCS500RJpegCallBack(data_param_t *param, int iStrLen)
{
    /* FTP模块要求只要其中某一步没有缓存了或DSP给错了都不能上传也不能写缓存 */
    int s_uiFileSeq = 0;
    int s_uiFileSum = 0;
    int iRecSta =0;
    unsigned int uiJpegLen =0;
    unsigned char *pucJpegData = NULL;
    unsigned char *pucFile = NULL;
    JPEG_HEADER *pJpegHeader = NULL;
    unsigned char ucSnapType = 0;
    unsigned char ucLaneId =0;
    JpegSendInfo strCurSnapInfo;
    VEHICLE_DETECT_HEADER *pVehicleHead =NULL;

    if ( (NULL == param->buf) || (0 == param->buf_len) || (param == NULL) )
    {
        DSP_ERROR("%s %d param invalid .\n",__FUNCTION__,__LINE__);
        return ;
    }
    else
    {
        DSP_INFO("%s %d get a jpeg buf all len= %dKB.\n",__FUNCTION__,__LINE__,param->buf_len/1024);
    }

    memset(&strCurSnapInfo, 0, sizeof(strCurSnapInfo));

    pucJpegData = param->buf;
    pJpegHeader = (JPEG_HEADER* )pucJpegData;
    pVehicleHead = (VEHICLE_DETECT_HEADER* )(pucJpegData + sizeof(JPEG_HEADER) );
    uiJpegLen = pJpegHeader->imgLen;

    s_uiFileSeq = pJpegHeader->snapID;
    s_uiFileSum = pJpegHeader->snapNum;
    ucSnapType  = pJpegHeader->snapType;

    ucLaneId = pVehicleHead->lane;

    strCurSnapInfo.auiPicLen[s_uiFileSeq] = uiJpegLen ;
    ftpEPLaneNameSet(&strCurSnapInfo, ucLaneId);

    pucFile = pucJpegData + iStrLen;

    /*如果ftp 使能了 则通过ftp发送图片*/
    if (g_cFtpInitSta)
    {
        strCurSnapInfo.pcPicBuf = (char *)pucFile;
        strCurSnapInfo.auiPicLen[s_uiFileSeq] = uiJpegLen;
        strCurSnapInfo.iPicNum = s_uiFileSum;
        strCurSnapInfo.iGroupId = pJpegHeader->snapGroupID;
        strCurSnapInfo.iCurrPicIndex = s_uiFileSeq;

        strCurSnapInfo.uiSnapTimeSecs     = pJpegHeader->seconds;
        strCurSnapInfo.uiSnapTimeMsSecs   = pJpegHeader->uiMsSeconds/1000;
        
        XCS500RFillJpegInfo(&strCurSnapInfo, pVehicleHead, ucSnapType);

        DSP_INFO("lane:%d ucSnapType is %d\n",ucLaneId, ucSnapType);

        /* 如果是报警录像通知，则直接返回，FTP不做处理 */
        if( ( pJpegHeader->imgLen < MIN_JPEG_PIC_LEN ) ||( pJpegHeader->imgLen > MAX_JPEG_PIC_LEN ) )
        {
            DSP_INFO("lane:%d,red light alarm report,dir return\n", ucLaneId);
            return;
        }

        bool bRet = false;
        bRet = g_CEPFtp->FtpSendPicture(&strCurSnapInfo);	// tangzl 0725
        if( !bRet )
        {
            DSP_INFO("%s %d lane_%d send pic file failed.fileType:%d,index:%d,picNumSum:%d,groupId:%d\n", __FUNCTION__,__LINE__,\
                ucLaneId, strCurSnapInfo.cFileType, \
                strCurSnapInfo.iCurrPicIndex,\
                strCurSnapInfo.iPicNum,\
                strCurSnapInfo.iGroupId);
        }
        else
        {
            DSP_INFO("%s %d lane_%d send pic file ok.fileType:%d,index:%d,picNumSum:%d,groupId:%d\n", __FUNCTION__,__LINE__,\
                ucLaneId, strCurSnapInfo.cFileType, \
                strCurSnapInfo.iCurrPicIndex,\
                strCurSnapInfo.iPicNum,\
                strCurSnapInfo.iGroupId);
        }
    }
    
    return;
}
#endif

static int pPFillJpegInfo(JpegSendInfo *pStrJSendInfo,VEHICLE_DETECT_HEADER *pVehicleHead,unsigned char ucSnapType )
{
    #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
    int ret =0;
    PASSPORT_OVERALL_CONF_INFO strPpOverallInfoGet;
    PORT_VEHICLE_CONF_INFO strChannelInfoGet;
    memset(&strPpOverallInfoGet, 0, sizeof(strPpOverallInfoGet));
    memset(&strChannelInfoGet, 0, sizeof(strChannelInfoGet));
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPpOverallInfoGet, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get pp overall conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strChannelInfoGet, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get pp vehicle conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }

    strcpy(pStrJSendInfo->szDirectionId, strPpOverallInfoGet.szDirId);
    strcpy(pStrJSendInfo->szPortId, strPpOverallInfoGet.szPassPortId);
    strcpy(pStrJSendInfo->szLaneId, strChannelInfoGet.szLaneId);

    memset(pStrJSendInfo->szPpDevCode, 0, sizeof(pStrJSendInfo->szPpDevCode));
    strcat(pStrJSendInfo->szPpDevCode, strPpOverallInfoGet.szPpDevCode);
    pStrJSendInfo->szPpDevCode[MAX_DEV_CODE_LEN -1] =0;
    strcat(pStrJSendInfo->szPpDevCode, strPpOverallInfoGet.szExPpDevCode);
    pStrJSendInfo->szPpDevCode[MAX_DEV_CODE_LEN+EXT_DEV_CODE_LEN -1] =0;
    
    pStrJSendInfo->iLaneIndex = pVehicleHead->lane;

    DSP_INFO("%s %d snapType:%d,speed:%d,irreSpeed:%d.!\n",__FUNCTION__,__LINE__,\
    ucSnapType, pVehicleHead->speed, strChannelInfoGet.usIrreSpeed);
    
    if( (pVehicleHead->speed > strChannelInfoGet.usIrreSpeed) \
        && (( ucSnapType == SNAP_TYPE_SPEED )||( ucSnapType == SNAP_TYPE_RADAR ) ))
    {
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities =   1;
    }
    else
    {
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities =   0; 
    }

    if(( SNAP_TYPE_REVERSE == ucSnapType ) || ( SNAP_TYPE_TRUCK == ucSnapType )\
    ||(SNAP_TYPE_DIR_REVERSE == ucSnapType ) )
    {
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities =   1;
    }


    if ( 1 == pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities )
    {
        if(pVehicleHead->speed > (pVehicleHead->maxspeed/2*3))
        {
            pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = IRR_TYPE_OVER_HALF_SPEED;
        }
        else if (pVehicleHead->speed > (strChannelInfoGet.usIrreSpeed))
        {
            pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = IRR_TYPE_OVER_SPEED;
        }
        
        if (( SNAP_TYPE_REVERSE == ucSnapType)||(SNAP_TYPE_DIR_REVERSE == ucSnapType ) )
        {
            pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = IRR_TYPE_REVERSE_DIR;
        }
        
        if( SNAP_TYPE_TRUCK == ucSnapType)
        {
            pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = IRR_TYPE_TRUCK_FORBID;
        }
    }
    else
    {
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = 0;
    }

    g_strCXProInfo.cLaneId = atoi(strChannelInfoGet.szLaneId);

    pStrJSendInfo->usSnapGroupId = pVehicleHead->usSnapGroupId;

	switch(g_PPLaneSwitchInfo[pVehicleHead->lane].cGroudCoilDecType)
	{
		case GROUD_DUBAL_COIL:
			pStrJSendInfo->ucCoilDecType =  FTP_GROUD_DUBAL_COIL;
			break;
		case GROUD_SINGAL_COIL_NO_SPEED:
		case GROUD_SINGAL_COIL_SPEED:
			if( g_PPLaneSwitchInfo[pVehicleHead->lane].cSingalCoilPos == SINGAL_COIL_POS_FRONT)
			{
				pStrJSendInfo->ucCoilDecType =  FTP_GROUD_SINGAL_COIL_FRONT;
			}
			else
			{
				pStrJSendInfo->ucCoilDecType =  FTP_GROUD_SINGAL_COIL_BEHIND;
			}
			break;
		default:
			pStrJSendInfo->ucCoilDecType =  FTP_VIRTUAL_COIL_TYPE;
			break;
	}
    
    #elif defined(CYC500XKW4C)||defined(CYC500XAW)

    int ret =0;
    PASSPORT_OVERALL_CONF_INFO strPpOverallInfoGet;
    memset(&strPpOverallInfoGet, 0, sizeof(strPpOverallInfoGet));
    ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPpOverallInfoGet, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get pp overall conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }

    memset(pStrJSendInfo->szPpDevCode, 0, sizeof(pStrJSendInfo->szPpDevCode));
    strcat(pStrJSendInfo->szPpDevCode, strPpOverallInfoGet.szPpDevCode);
    pStrJSendInfo->szPpDevCode[MAX_DEV_CODE_LEN -1] =0;
    strcat(pStrJSendInfo->szPpDevCode, strPpOverallInfoGet.szExPpDevCode);
    pStrJSendInfo->szPpDevCode[MAX_DEV_CODE_LEN+EXT_DEV_CODE_LEN -1] =0;
    
    ELEC_POLICE_VEHICLE_CONF_INFO strEPChanConf;
    ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
    memset(&strEPChanConf, 0, sizeof(strEPChanConf));
    memset(&strEPGlobalConf, 0, sizeof(strEPGlobalConf));
    
    ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPChanConf, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get ep vehicle conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }

    ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, pVehicleHead->lane);
    if ( 0 != ret )
    {
        DSP_ERROR("%s %d get ep overall conf failed!\n",__FUNCTION__,__LINE__);
        return -1;
    }
    
    strcpy(pStrJSendInfo->szDirectionId, strEPGlobalConf.szDirId);
    strcpy(pStrJSendInfo->szPortId, strEPGlobalConf.szRoadId);
    strcpy(pStrJSendInfo->szLaneId, strEPChanConf.szLaneId);
    
    /* 注意:需要添加和330平台对应的车道号编码 */
    pStrJSendInfo->iLaneIndex = pVehicleHead->lane;

    /* 增加骑线违章处理 */
    if(SNAP_TYPE_RIDE_LINES == ucSnapType)
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities =   1;
    else
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities =   0; 


    if ( 1 == pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities )
    {
        if(SNAP_TYPE_RIDE_LINES == ucSnapType)
        {
            pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = IRR_TYPE_RIDE_LINES;
        }
    }
    else
    {
        pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type = 0;
    }

    g_strCXProInfo.cLaneId = atoi(strEPChanConf.szLaneId);

    #endif
    DSP_INFO("%s %d 140 laneIndex:0x%x.\n",__FUNCTION__,__LINE__, pStrJSendInfo->iLaneIndex);
    pStrJSendInfo->strParam.pos_info.m_left = pVehicleHead->rect.left;
    pStrJSendInfo->strParam.pos_info.m_top = pVehicleHead->rect.top;
    pStrJSendInfo->strParam.pos_info.m_bottom = pVehicleHead->rect.bottom;
    pStrJSendInfo->strParam.pos_info.m_right = pVehicleHead->rect.right;

    pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_speed = pVehicleHead->speed;
    pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_speed_thresh = pVehicleHead->maxspeed;
    
    DSP_INFO("\n\n%s %d snapType:%d,m_irregularities:%d,type:%d,vehicleType:%d\n",__FUNCTION__,__LINE__, ucSnapType,\
    pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities,\
    pStrJSendInfo->strParam.SnapPicInfo.port_snap_info.m_irregularities_type,pVehicleHead->vehicleType);
    pStrJSendInfo->strParam.pos_info.m_type = pVehicleHead->vehicleType;

    pStrJSendInfo->strParam.pos_info.m_confidence = pVehicleHead->confidence;
    pStrJSendInfo->cFileType = JPG_FILE_TYPE;/*this is modified by yzq 2010-9-20*/

    DSP_INFO("\n\n%s %d.lane:%d, cGroudCoilDecType:%d!\n",__FUNCTION__,__LINE__,\
    pVehicleHead->lane, g_PPLaneSwitchInfo[pVehicleHead->lane].cGroudCoilDecType);
    
    if( g_PPLaneSwitchInfo[pVehicleHead->lane].cGroudCoilDecType == CS_RADAR_SPEED )
    {
        pStrJSendInfo->ucSnapType = SNAP_TYPE_RADAR;
    }
    else
    {
        pStrJSendInfo->ucSnapType = ucSnapType;        
    }

    pStrJSendInfo->lpColor        = pVehicleHead->lpColor;
    pStrJSendInfo->lpConfidenct   = pVehicleHead->lpConfidenct;
    pStrJSendInfo->lpFCConfidenct = pVehicleHead->lpFCConfidenct;
    pStrJSendInfo->lpRect.start_x = pVehicleHead->lpRect.left;
    pStrJSendInfo->lpRect.start_y = pVehicleHead->lpRect.top;
    pStrJSendInfo->lpRect.width   = pVehicleHead->lpRect.right - pVehicleHead->lpRect.left;
    pStrJSendInfo->lpRect.height  = pVehicleHead->lpRect.bottom - pVehicleHead->lpRect.top;
    pStrJSendInfo->LPREnable      = pVehicleHead->LPREnable;
    pStrJSendInfo->lpType         = pVehicleHead->lpType;
    strcpy(pStrJSendInfo->lpStr, pVehicleHead->lpStr);

    for(int i=0; i< 2; i++)
    {
        DSP_INFO("%s %d pic%d sec:%d,msec:%d.\n",__FUNCTION__,__LINE__, i, \
        pVehicleHead->strSnapPicTime[i].uiSec, pVehicleHead->strSnapPicTime[i].uiMSec);
    }
    memcpy( pStrJSendInfo->strSanpPicTime, pVehicleHead->strSnapPicTime, sizeof( pVehicleHead->strSnapPicTime ));
    //温度 
    g_strCXProInfo.cDevTemprature = g_cTemperature;
    pStrJSendInfo->strFtpCXProInfo = g_strCXProInfo;

#if USE_FACE_IDENT
    pStrJSendInfo->ucFaceIdenEn = pVehicleHead->ucFaceIdenEn;
    pStrJSendInfo->ucFaceOvenlayEn = pVehicleHead->ucFaceOvenlayEn;
    
    if( pVehicleHead->ucFaceSum > MAX_FACE_NUM)
    {
    	pVehicleHead->ucFaceSum = MAX_FACE_NUM;
    }
    pStrJSendInfo->ucFaceSum = pVehicleHead->ucFaceSum;
    
    pStrJSendInfo->strFaceSearchArea.start_x = pVehicleHead->strFaceSearchArea.left;
    pStrJSendInfo->strFaceSearchArea.start_y = pVehicleHead->strFaceSearchArea.top;
    pStrJSendInfo->strFaceSearchArea.width   = pVehicleHead->strFaceSearchArea.right - pVehicleHead->strFaceSearchArea.left;
    pStrJSendInfo->strFaceSearchArea.height  = pVehicleHead->strFaceSearchArea.bottom - pVehicleHead->strFaceSearchArea.top;

	for(int i=0; i< pVehicleHead->ucFaceSum; i++)
    {
	    pStrJSendInfo->aStrFaceIden[i].start_x = pVehicleHead->aStrFaceIden[i].left;
	    pStrJSendInfo->aStrFaceIden[i].start_y = pVehicleHead->aStrFaceIden[i].top;
	    pStrJSendInfo->aStrFaceIden[i].width   = pVehicleHead->aStrFaceIden[i].right - pVehicleHead->aStrFaceIden[i].left;
	    pStrJSendInfo->aStrFaceIden[i].height  = pVehicleHead->aStrFaceIden[i].bottom - pVehicleHead->aStrFaceIden[i].top;
    }
#if 1
    pStrJSendInfo->ucSaveSmallPicEn = pVehicleHead->ucSaveSmallPicEn;
    if( pVehicleHead->ucSaveSmallPicEn > 0 )
    {
    	pStrJSendInfo->auiBigPicSize[pStrJSendInfo->iCurrPicIndex] = pStrJSendInfo->auiPicLen[pStrJSendInfo->iCurrPicIndex];
    	for(int i=0; i< pVehicleHead->ucFaceSum; i++)
	    {
		    pStrJSendInfo->aStrFacePicInfo[i].uiPicAddrOffset = pVehicleHead->aStrFacePicInfo[i].uiPicAddrOffset;
			pStrJSendInfo->aStrFacePicInfo[i].uiPicLen = pVehicleHead->aStrFacePicInfo[i].uiPicLen;
			pStrJSendInfo->auiPicLen[pStrJSendInfo->iCurrPicIndex] += pStrJSendInfo->aStrFacePicInfo[i].uiPicLen;
	    }
    }
    else
    {
    	pStrJSendInfo->auiBigPicSize[pStrJSendInfo->iCurrPicIndex] = pStrJSendInfo->auiPicLen[pStrJSendInfo->iCurrPicIndex];
    }
#endif

#endif

    return 0;
}


void pPJpegCallBack(data_param_t *param, int iStrLen)
{
    /* FTP模块要求只要其中某一步没有缓存了或DSP给错了都不能上传也不能写缓存 */

    unsigned int s_uiFileSeq = 0;
    unsigned int s_uiFileSum = 0;
    unsigned int uiJpegLen =0;
    unsigned char *pucJpegData = NULL;
    unsigned char *pucFile = NULL;
    JPEG_HEADER *pJpegHeader = NULL;
    VEHICLE_DETECT_HEADER *pVehicleHead =NULL;
    unsigned char ucSnapType = 0;
    static JpegSendInfo  s_strJSendInfo = {0};
	
    if ( (NULL == param->buf) || (0 == param->buf_len) )
    {
        DSP_ERROR("%s %d param invalid .\n",__FUNCTION__,__LINE__);
        return ;
    }
    else
    {
        DSP_INFO("%s %d get a jpeg buf all len= %dB.\n",__FUNCTION__,__LINE__,param->buf_len);
    }
    
    pucJpegData = param->buf;
    pJpegHeader = (JPEG_HEADER* )pucJpegData;
    pVehicleHead = (VEHICLE_DETECT_HEADER* )(pucJpegData + sizeof(JPEG_HEADER) );

    s_uiFileSeq = pJpegHeader->snapID;
    s_uiFileSum = pJpegHeader->snapNum;
    ucSnapType  = pJpegHeader->snapType;

    DSP_INFO("%s %d snapType= 0x%x,fileSum:%d.\n",__FUNCTION__,__LINE__,ucSnapType, pJpegHeader->snapNum);

    uiJpegLen = pJpegHeader->imgLen;
    s_strJSendInfo.auiPicLen[s_uiFileSeq] = uiJpegLen;

    ftpPPLaneNameSet(&s_strJSendInfo, pVehicleHead->lane);

    pucFile = pucJpegData + iStrLen;
    if (g_cFtpInitSta)
    {
        s_strJSendInfo.pcPicBuf                   = (char *)pucFile;
        s_strJSendInfo.auiPicLen[s_uiFileSeq]     = uiJpegLen;
        s_strJSendInfo.iPicNum                    = s_uiFileSum;
        s_strJSendInfo.iGroupId                   = pJpegHeader->snapGroupID;
        s_strJSendInfo.iCurrPicIndex              = s_uiFileSeq;

        if ( 0 == s_uiFileSeq )
        {
            s_strJSendInfo.uiSnapTimeMsSecs           = pJpegHeader->uiMsSeconds/1000;
            s_strJSendInfo.uiSnapTimeSecs             = pJpegHeader->seconds;
        }

        pPFillJpegInfo(&s_strJSendInfo,pVehicleHead,ucSnapType);

        bool bRet = false;
		#if 0
		int i =0;
		for(i=0; i< s_strJSendInfo.iPicNum; i++)
		{
			DSP_ERROR("auiPicLen_%d is %d,bigPicLen_%d:%d.\n",i, s_strJSendInfo.auiPicLen[i], i, s_strJSendInfo.auiBigPicSize[i]);
		}
		#endif
        bRet = g_CPPFtp->FtpSendPicture(&s_strJSendInfo);	// tangzl 0725

        if( !bRet )
        {
            DSP_INFO("%s %d FtpSendPicture failed.\n",__FUNCTION__,__LINE__);
        }
        else
        {
            DSP_INFO("%s %d FtpSendPicture ok.\n",__FUNCTION__,__LINE__);
        }
    }
    return;
}
#if 0
void TimerRecFileSend(char *pcFileName)
{
    int ret =0;
    char *ppBuf = NULL;
    JpegSendInfo  strJSendInfo = {0};

#ifdef MODULE_ALARMREC
    if(g_cAlarmRecSta)
    {
        ftpPPLaneNameSet(&strJSendInfo, 0);
        ret = xcs500xMemAsfFileGet(TIME_REC_TYPE, (void **)&ppBuf);
        if(0 < ret)
        {
            struct timeval tv;
            struct timezone tz;
            gettimeofday(&tv,&tz);

            strJSendInfo.uiSnapTimeMsSecs = tv.tv_usec/1000;
            strJSendInfo.uiSnapTimeSecs   = tv.tv_sec  + 8*3600;
            //DSP_ERROR("%s %d FtpSendPicture asf file seconds:%ld.\n",__FUNCTION__,__LINE__, strJSendInfo.uiSnapTimeSecs);
            strJSendInfo.cFileType = ASF_FILE_TYPE;
            strJSendInfo.pcPicBuf = ppBuf;
            strJSendInfo.iCurrPicIndex = 0;
            strJSendInfo.auiPicLen[0] = ret;
            strJSendInfo.iPicNum = 1;
            if( NULL!= g_CPPFtp )
            {
                ret = g_CPPFtp->FtpSendPicture(&strJSendInfo);	// tangzl 0725
                if( !ret )
                {
                    DSP_ERROR("%s %d FtpSendPicture asf failed.\n",__FUNCTION__,__LINE__);
                }
                else
                {
                    DSP_INFO("%s %d FtpSendPicture asf ok.len:%d\n",__FUNCTION__,__LINE__, ret);
                }
            }
            else
            {
                DSP_INFO("%s %d g_CPPFtp is null.\n",__FUNCTION__,__LINE__);
            }
            /* 删除asf文件 */
            epRecModuleFileExitStaSet(TIME_REC_TYPE, 0);
       }
       else
       {
            DSP_INFO("%s %d xcs500xMemAsfFileGet ger file failed,ret:%d\n",__FUNCTION__,__LINE__, ret);
       }
   }
#endif /* MODULE_ALARMREC */
}

#endif

int   JpgEncryption(unsigned char *pSrc, int length, unsigned char *pL2Tmp, int L2length)
{
    int i,j;
    int *pInA, *pInB, *pTmp;
    int sum=0;
    int addlen;
    unsigned char *pTemp = NULL;

    pTemp = pSrc;
    pSrc[length]=0; 
    pSrc[length+1] = 0;
    pSrc[length+2] = 0;
    pSrc[length+3] = 0;

    L2length = L2length > (120*1024) ? 120*1024 : L2length;
    L2length = L2length>>1;
    addlen = L2length>>2;
    pL2Tmp +=  (16- ((unsigned int)pL2Tmp)&0xF);
    pInA = (int *)pL2Tmp;
    pInB = (int *)(pL2Tmp + L2length);

        memcpy(pInB, pTemp, L2length);
    for (i=0; i<length/L2length; i++)
    {
       pTmp = pInA;
       pInA = pInB;
       pInB = pTmp;
      
       pTemp += L2length;
       memcpy(pInB, pTemp, L2length);
       for (j=0; j<addlen; j++)
       {
            sum += pInA[j];
       }
    }
    for (j= 0; j<((length+3)>>2)-i*addlen; j++)
    {
        sum += pInB[j];
    }
    pSrc[length] = 'k';
    pSrc[length+1]= 'j';
    *((int *)(pSrc+length+4))= sum;
    return sum;
}

int JpgDefModify(unsigned char *pSrc, int length)
{
    int i =0;
    int iSumTimes =0;
    unsigned int uiSum =0;
    unsigned char *pTmp;
    iSumTimes = length/1024;
    for(i =0; i< iSumTimes; i++)
    {
        pTmp = pSrc;
        pTmp += iSumTimes*1024;
        uiSum += *(unsigned int *)pTmp;
    }
    pSrc[length] = 'k';
    pSrc[length+1]= 'j';
    pSrc[length+2]= 0;
    pSrc[length+3]= 0;
    *((unsigned int *)(pSrc+length+4))= uiSum;
    return uiSum;
}

static void dspJpegProcess(data_param_t *param)
{
DSP_ERROR("11111111111111dspJpegProcess\n");
 int iEndSum =0;
    int i =0;
    unsigned int uiJpegLen =0;
    unsigned char *pucJpegData = NULL;
    unsigned char *pucFile = NULL;
    JPEG_HEADER *pJpegHeader = NULL;
    VEHICLE_DETECT_HEADER *pVehicleHead =NULL;
    unsigned char ucSnapType = 0;
    unsigned int uiSmallPicLen =0;

	/* 回调数据结构体长度，用于定位jepg起始内存地址 */
	int iDataStrLen =0;

    DSP_INFO("enter dspJpegCall!\n");
   

    sem_wait(&sem_dsp_data);
    DSP_INFO("enter1 dspJpegCall!\n");
DSP_ERROR("2222222222dspJpegProcess\n");

    /* 通过TCP走XCS协议把图片发送出去 */
    if ( 1== g_bTcpDataOk)
    {
		DSP_ERROR("### sendPicToServer\n");
              SendPicToServer((char*)param->buf, param->buf_len);
       
        
    }
DSP_ERROR("333333333dspJpegProcess\n");
    /*如果ftp 使能了 则通过ftp发送图片*/
    if (g_cFtpInitSta)
    {
        switch(device_code)
        {
            #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            case CYC200K3_DEV_CODE:
            case CYC500K_DEV_CODE:
            case CYC500KD_DEV_CODE:
            case CYC500KKW2C_DEV_CODE:
            case CYC200KT_DEV_CODE:
            case CYC200K2_DEV_CODE:
            case CYC200MK_DEV_CODE:
                pPJpegCallBack(param, iDataStrLen);
            break;
            #endif


			#if defined(CYC500XKW4C)||defined(CYC500XAW)
            case CYC500XKW4C_DEV_CODE:
            case CYC500XAW_DEV_CODE:
                switch(ucSnapType)
                {
                    case SNAP_TYPE_SPEED:
                    case SNAP_TYPE_REVERSE:
                    case SNAP_TYPE_CLIP:
                    case SNAP_TYPE_RIDE_LINES:
                    case SNAP_TYPE_DIR_REVERSE:
					case SNAP_TYPE_CX_NULL_SNAP:
                        if(( pJpegHeader->imgLen > MIN_JPEG_PIC_LEN ) &&( pJpegHeader->imgLen < MAX_JPEG_PIC_LEN ))
                        {
                            DSP_INFO("go into pp ftp send!\n");
                            pPJpegCallBack(param, iDataStrLen);
                            DSP_INFO("go out pp ftp send!\n");
                        }
                    break;
                    case SNAP_TYPE_3_PLUS_1:
                    case SNAP_TYPE_6_PLUS_1:
                    case SNAP_TYPE_3_SEPARATE:
                    case SNAP_TYPE_EP_REVERSE:
                    case SNAP_TYPE_3_PLUS_1_LEFT:
                    case SNAP_TYPE_4_SEPARATE:
                        DSP_INFO("go into ep ftp send!\n");
                        XCS500RJpegCallBack(param, iDataStrLen);
                        DSP_INFO("go out ep ftp send!\n");
                    break;
                    default:
                    break;
                }
            break;
            #endif
            default:
                DSP_ERROR("dev code is error,not send jpeg!\n");
            break;
        }
    }
    sem_post(&sem_dsp_data);
    DSP_INFO("leave dspJpegCall!\n");
    return;

#if 0
    int iEndSum =0;
    int i =0;
    unsigned int uiJpegLen =0;
    unsigned char *pucJpegData = NULL;
    unsigned char *pucFile = NULL;
    JPEG_HEADER *pJpegHeader = NULL;
    VEHICLE_DETECT_HEADER *pVehicleHead =NULL;
    unsigned char ucSnapType = 0;
    unsigned int uiSmallPicLen =0;

	/* 回调数据结构体长度，用于定位jepg起始内存地址 */
	int iDataStrLen =0;

    DSP_INFO("enter dspJpegCall!\n");
    if ( (NULL == param->buf) || (0 >= param->buf_len) || ( param == NULL ) )
    {
        DSP_ERROR("%s %d param invalid .\n",__FUNCTION__,__LINE__);
        return;
    }

    sem_wait(&sem_dsp_data);
    DSP_INFO("enter1 dspJpegCall!\n");
    pucJpegData = param->buf;
    pJpegHeader = (JPEG_HEADER* )pucJpegData;
    pVehicleHead = (VEHICLE_DETECT_HEADER* )(pucJpegData + sizeof(JPEG_HEADER) );

    {
        DSP_INFO("jpg get a jpeg buf len:%ld,img len:%ld.ucSaveSmallPicEn:%d.param add:0x%x,buf add:0x%x\n", param->buf_len, pJpegHeader->imgLen, pVehicleHead->ucSaveSmallPicEn,param, pucJpegData);
    }

	if( pVehicleHead->ucSaveSmallPicEn > 0)
	{
		//DSP_ERROR("---small pic sum:%d.\n", pVehicleHead->ucFaceSum);
		for(i=0; i< pVehicleHead->ucFaceSum; i++)
		{
			uiSmallPicLen += pVehicleHead->aStrFacePicInfo[i].uiPicLen;
			//DSP_ERROR("---small pic_%d len:%d.\n", i, pVehicleHead->aStrFacePicInfo[i].uiPicLen);
			//DSP_ERROR("---small pic_%d offset:0x%x.\n", i, pVehicleHead->aStrFacePicInfo[i].uiPicAddrOffset);
		}
		iDataStrLen = param->buf_len - pJpegHeader->imgLen - uiSmallPicLen;
	}
	else
	{
		iDataStrLen = param->buf_len - pJpegHeader->imgLen;
	}
	
	if( iDataStrLen	== sizeof(JPEG_HEADER) + sizeof(VEHICLE_DETECT_HEADER) )
	{
		DSP_INFO("strLen:%d is ok.\n", iDataStrLen);
	}
	else
	{
		DSP_ERROR("strLen:%d is failed.act str len:%d.\n", iDataStrLen, sizeof(JPEG_HEADER) + sizeof(VEHICLE_DETECT_HEADER));
		syslog(LOG_ERR|LOG_USER, "error:jpgCallback strHeadLen:%d is failed. act strHeadlen:%d.\n", iDataStrLen, sizeof(JPEG_HEADER) + sizeof(VEHICLE_DETECT_HEADER));	
	}
		

    if((pJpegHeader->imgLen > MAX_JPEG_PIC_LEN) || (pJpegHeader->snapNum > MAX_PIC_NUM))
    {
        DSP_ERROR("%s %d imgLen:%d is too big. pic num:%d. return.\n",__FUNCTION__,__LINE__, pJpegHeader->imgLen, pJpegHeader->snapID);
        syslog(LOG_ERR|LOG_USER, "%s %d imgLen:%d or pic num:%d is too big.return.\n",__FUNCTION__,__LINE__, pJpegHeader->imgLen, pJpegHeader->snapID);
        sem_post(&sem_dsp_data);
        return;
    }
    
    uiJpegLen = ( pJpegHeader->imgLen + 3 )/4*4;

    ucSnapType  = pJpegHeader->snapType;
    
    pucFile = pucJpegData + iDataStrLen;
    
#if 0

    if(( *(pucFile+pJpegHeader->imgLen-1) != 0xd9 ) || ( *(pucFile+pJpegHeader->imgLen-2 ) != 0xff))
    {
        syslog(LOG_ERR|LOG_USER, "dsp report error jpg:no 0xffd9!\n");
    }


    /* add by its 2010-6-25 for 测试DSP上传的图片检验和是否正确 */
    
    iBeginSum = *(int *)(pucFile + uiJpegLen- 4);
    memset(lTmpL2Buf, 0, TMP_L2_LEN);
    iEndSum = JpgEncryption(pucFile, uiJpegLen-8, lTmpL2Buf, TMP_L2_LEN);
    if(iBeginSum != iEndSum)
    {
        struct tm tmPlanStart;
        char szDefault[256];
        gmtime_r((time_t *)&(pJpegHeader->seconds), &tmPlanStart);
        sprintf(szDefault,"%04d-%02d-%02d %02d:%02d:%02d",\
        tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
        tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec);
        DSP_INFO("%s %d dsp sum is error,len is 0x%x.\n",__FUNCTION__,__LINE__,pJpegHeader->imgLen);
        syslog(LOG_ERR|LOG_USER, "dsp pic sum error,date:%s,msec:%d\n", szDefault,pJpegHeader->uiMsSeconds);
    }
    else
    {
        memset(lTmpL2Buf, 0, TMP_L2_LEN);
        iEndSum = JpgEncryption(pucFile, uiJpegLen, lTmpL2Buf, TMP_L2_LEN);
        uiJpegLen += 8;
        pJpegHeader->imgLen = uiJpegLen;
        DSP_INFO("%s %d dsp sum is ok,len is 0x%x.\n",__FUNCTION__,__LINE__,pJpegHeader->imgLen);
        
    }
    
    /* end by its 2010-6-25 for 测试DSP上传的图片检验和是否正确 */
#else

	/* 进行防篡改加密处理 */
	if( pVehicleHead->ucSaveSmallPicEn > 0)
	{
		uiJpegLen -= SMALL_PIC_RESV_ROOM;
	    iEndSum = JpgDefModify(pucFile, uiJpegLen);
	}
	else
	{
		iEndSum = JpgDefModify(pucFile, uiJpegLen);
	    uiJpegLen += PIC_WATERMARK_PWD_LEN;
	    pJpegHeader->imgLen = uiJpegLen;
	}
#endif

    /* 通过TCP走XCS协议把图片发送出去 */
    if ( 1== g_bTcpDataOk)
    {
		#if defined(CYC500XKW4C)||defined(CYC500XAW)
        if(( pJpegHeader->imgLen > MIN_JPEG_PIC_LEN ) &&( pJpegHeader->imgLen < MAX_JPEG_PIC_LEN ))
        {
            DSP_INFO("pic len :%d,snap type:%d\n", pJpegHeader->imgLen, pJpegHeader->snapType);
            SendPicToServer((char*)pucFile, pJpegHeader->imgLen);
        }
        
        #else
        	if( pVehicleHead->ucSaveSmallPicEn > 0)
        	{
        		SNAP_INFORM strSnapInform;
        		unsigned int aiAllPicLen[6]={0}; 

				aiAllPicLen[0] = pJpegHeader->imgLen;
				
				for(i=0; i< pVehicleHead->ucFaceSum; i++)
				{
					aiAllPicLen[i+1] = pVehicleHead->aStrFacePicInfo[i].uiPicLen;
				}
        		
			    strSnapInform.ucPicNum  	= pVehicleHead->ucFaceSum + 1;
			    strSnapInform.auiPicLen 	= aiAllPicLen;
			    strSnapInform.pszPicData   	= (char *)pucFile;

				SendGroupPicToServer(&strSnapInform);
        	}
        	else
        	{
            	SendPicToServer((char*)pucFile, pJpegHeader->imgLen);
			}
            

        #endif
    }

    /*如果ftp 使能了 则通过ftp发送图片*/
    if (g_cFtpInitSta)
    {
        switch(device_code)
        {
            #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            case CYC200K3_DEV_CODE:
            case CYC500K_DEV_CODE:
            case CYC500KD_DEV_CODE:
            case CYC500KKW2C_DEV_CODE:
            case CYC200KT_DEV_CODE:
            case CYC200K2_DEV_CODE:
            case CYC200MK_DEV_CODE:
                pPJpegCallBack(param, iDataStrLen);
            break;
            #endif


			#if defined(CYC500XKW4C)||defined(CYC500XAW)
            case CYC500XKW4C_DEV_CODE:
            case CYC500XAW_DEV_CODE:
                switch(ucSnapType)
                {
                    case SNAP_TYPE_SPEED:
                    case SNAP_TYPE_REVERSE:
                    case SNAP_TYPE_CLIP:
                    case SNAP_TYPE_RIDE_LINES:
                    case SNAP_TYPE_DIR_REVERSE:
					case SNAP_TYPE_CX_NULL_SNAP:
                        if(( pJpegHeader->imgLen > MIN_JPEG_PIC_LEN ) &&( pJpegHeader->imgLen < MAX_JPEG_PIC_LEN ))
                        {
                            DSP_INFO("go into pp ftp send!\n");
                            pPJpegCallBack(param, iDataStrLen);
                            DSP_INFO("go out pp ftp send!\n");
                        }
                    break;
                    case SNAP_TYPE_3_PLUS_1:
                    case SNAP_TYPE_6_PLUS_1:
                    case SNAP_TYPE_3_SEPARATE:
                    case SNAP_TYPE_EP_REVERSE:
                    case SNAP_TYPE_3_PLUS_1_LEFT:
                    case SNAP_TYPE_4_SEPARATE:
                        DSP_INFO("go into ep ftp send!\n");
                        XCS500RJpegCallBack(param, iDataStrLen);
                        DSP_INFO("go out ep ftp send!\n");
                    break;
                    default:
                    break;
                }
            break;
            #endif
            default:
                DSP_ERROR("dev code is error,not send jpeg!\n");
            break;
        }
    }
    sem_post(&sem_dsp_data);
    DSP_INFO("leave dspJpegCall!\n");
    return;

#endif
}

void RcvJpgCallBack(char* pBuffer, unsigned int dwBufSize, unsigned int dwUser)
{
      DSP_ERROR("RcvJpgCallBack xcs  xcs  xcs  xcs xc\n");
	int i =0;
	int iPicNum =0;
	data_param_t strDataParm;
	
		strDataParm.chan_id =0;
		strDataParm.data_type =0;
		strDataParm.buf     = (unsigned char *)pBuffer;
		strDataParm.buf_len = dwBufSize;
		DSP_ERROR("RcvJpgCallBack xcs  xcs  xcs  xcs xc\n");
		DSP_INFO("####RcvJpgCallBack xcs  xcs  xcs  xcs xcs xcs *P=%x size =%d##########\n",pBuffer,dwBufSize);
		DSP_ERROR("RcvJpgCallBack xcs  xcs  xcs  xcs xcs xcs\n *P=%x size =%d#",pBuffer,dwBufSize);
		dspJpegProcess(&strDataParm);
}

void dspRedLightStaCallBack(trafficlight_status_t *param)
{
    int iGreenDecFlag =0;

    iGreenDecFlag = greenLightDectFlagGet();
    sem_wait(&sem_dsp_data);
    if( 0 == iGreenDecFlag )
    {
        if(e_RED_LIGHT == param->status)
        {
            kjRedLightInform(param->index, RED_LIGHT_ON, RED_DETECT_VIDEO);
        }
        else
        {
            kjRedLightInform(param->index, RED_LIGHT_OFF, RED_DETECT_VIDEO);
        }
    }
    else
    {
        if(e_GREEN_LIGHT == param->status)
        {
            kjRedLightInform(param->index, RED_LIGHT_OFF, RED_DETECT_VIDEO);
        }
        else
        {
            kjRedLightInform(param->index, RED_LIGHT_ON, RED_DETECT_VIDEO);
        }
    }
    sem_post(&sem_dsp_data);
    DSP_INFO("dsp report led light sta:%d, index:%d,green dec flag:%d!\n", \
    param->status, param->index, iGreenDecFlag);
}
void dspAlarmStaCallBack(alarm_report_t *param)
{
    int ret =0;
    static int iFailedSec =0;
    alarm_report_t *pAlarmInfo = NULL;
    if(NULL == param)
    {
        DSP_ERROR("dspAlarmStaCallBack param is null\n");
        return;
    }
    sem_wait(&sem_dsp_data);
    iFailedSec++;
    pAlarmInfo = param;
    if( ALARM_CAMEAR_ERROR == pAlarmInfo->alarm_type )
    {
        if( iFailedSec > 30 )
        {
            iFailedSec =0;
            shell_print("camara is failed 30 times,reboot xcs\n");
            syslog(LOG_ERR|LOG_USER, "camara is failed 30 times,reboot xcs\n");
            STR_SYS_APP_MSG strTmpSysMsg;
            memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
            strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
            strTmpSysMsg.uiMsgType = MSG_REMOTE_REBOOT;
            ret = sendMsgToMainApp(&strTmpSysMsg);
            if(-1 == ret)
            {
                DSP_ERROR("send reboot msg failed.\n");
            }
            else
            {
                DSP_ERROR("dsp camara error send reboot msg ok.\n");
            }
        }
    }
    else
    {
        shell_print("dsp alarm type is error!\n");
    }
    sem_post(&sem_dsp_data);
}
void dspServerAlarmMsg(int iCommand, char *pAlarmInfo, int dwBufLen,void *pUser)
{
	flowstat_status_t *pstrCarFlowSta;			/* 车流量 */
	alarm_report_t *pstrAlarmReport;			/* 报警状态 */
	trafficlight_status_t *pstrTrafficLSta;		/* 交通灯状态 */
	if((pAlarmInfo == NULL ) || ( NULL == pUser))
	{
		DSP_ERROR("in param error,return.\n");
		return ;
	}

	switch( iCommand )
	{
		case COMM_ALARM_PDC:
			pstrCarFlowSta = (flowstat_status_t *)pAlarmInfo;
			
			epFlowRateHandle(pstrCarFlowSta);
			break;
		case COMM_ALARM_VL:
			pstrAlarmReport = (alarm_report_t *)pAlarmInfo;
			
			dspAlarmStaCallBack(pstrAlarmReport);
			break;
		case COMM_ALARM_LS:
			pstrTrafficLSta = (trafficlight_status_t *)pAlarmInfo;

			dspRedLightStaCallBack(pstrTrafficLSta);
			break;
		default:
			break;
	}
}
int dspInit()
{
    int ret = 0;
    void * pTmp = (void *)"test";
    unsigned int version;
    unsigned int compile;
    ret = sem_init(&sem_dsp_data,0,1);
    ret = sem_init(&sem_dsp,0,1);
    if ( -1 == ret )
    {
        perror("dspInit sem_init");
        pthread_exit(&ret);
    }
	ret = NET_CYC_Init();
	if (ret != 0)
	{
		DSP_ERROR("%s %d DSP_SERVICE:dsp_service_init failed, error code: %d. \n",__FUNCTION__,__LINE__, NET_DVE_GetLastError());
	}
	else
	{
		version_info_t vi_fw, vi_dsp, vi_hw, vi_ep, vi_lpr, vi_osd, vi_rlt;
		memset(&vi_fw, 0, sizeof(vi_fw));
		memset(&vi_dsp, 0, sizeof(vi_dsp));
		memset(&vi_hw, 0, sizeof(vi_hw));
		memset(&vi_ep, 0, sizeof(vi_ep));
		memset(&vi_lpr, 0, sizeof(vi_lpr));
		memset(&vi_osd, 0, sizeof(vi_osd));
		memset(&vi_rlt, 0, sizeof(vi_rlt));
		vi_fw.cbSize = vi_dsp.cbSize = vi_hw.cbSize = sizeof(version_info_t);
		vi_ep.cbSize = vi_lpr.cbSize = vi_osd.cbSize = vi_rlt.cbSize = sizeof(version_info_t);
		NET_DEV_GET_Version(e_RdkMcfw_type, &vi_fw);
		NET_DEV_GET_Version(e_dsp_type, &vi_dsp);
		NET_DEV_GET_Version(e_hardware_type, &vi_hw);
		NET_DEV_GET_Version(e_link_ep_type, &vi_ep);
		NET_DEV_GET_Version(e_link_lpr_type, &vi_lpr);
		NET_DEV_GET_Version(e_link_osd_type, &vi_osd);
		NET_DEV_GET_Version(e_link_rlt_type, &vi_rlt);
		firmware_version_info.version = vi_fw.version;
		strcpy(firmware_version_info.szCompile, vi_fw.compile_info);
		strcpy(firmware_version_info.szDesc, vi_fw.ver_instr);
		strcpy(link_ep_version_info.szCompile, vi_ep.compile_info);
		strcpy(link_lpr_version_info.szCompile, vi_lpr.compile_info);
		strcpy(link_osd_version_info.szCompile, vi_osd.compile_info);
		strcpy(link_rlt_version_info.szCompile, vi_rlt.compile_info);
		memset(&sharelib_version_info, 0, sizeof(sharelib_version_info));

		DSP_INFO("dsp_service version=%x, compile info:%s.\n", sharelib_version_info.version,sharelib_version_info.szCompile);
	}

    //ret = dsp_service_hook_data(RcvStreamCallBack);
    ret = NET_CYC_SetRealDataCallBack(0, RcvStreamCallBack, 0);
    if(ret != 0)
    {
		DSP_ERROR("%s %d DSP_SERVICE:dsp_service_hook_data failed \n",__FUNCTION__,__LINE__);
	}
	else
	{
		DSP_ERROR("%s %d DSP_SERVICE:dsp_service_hook_data ok \n",__FUNCTION__,__LINE__);
	}

	//ret = dsp_service_hook_jpeg_data(dspJpegCallBack);
	ret = NET_CYC_SetJpegDataCallBack(RcvJpgCallBack, 0);
	if(ret != 0)
	{
		DSP_ERROR("%s %d DSP_SERVICE:RcvJpgCallBack failed \n",__FUNCTION__,__LINE__);
	}
	else
	{
		DSP_ERROR("%s %d DSP_SERVICE:RcvJpgCallBack ok \n",__FUNCTION__,__LINE__);
	}
	
	//ret = dsp_service_hook_alarm(dspAlarmStaCallBack);
	ret = NET_CYC_SetDevMessageCallBack(dspServerAlarmMsg, pTmp);
	if (ret != 0)
	{
		DSP_ERROR("%s %d NET_CYC_SetDevMessageCallBack failed \n",__FUNCTION__,__LINE__);
	}
	else
	{
		DSP_ERROR("%s %d NET_CYC_SetDevMessageCallBack ok \n",__FUNCTION__,__LINE__);
	}

	ret = NET_CYC_Start();
	if (ret != 0)
	{
		DSP_ERROR("%s %d DSP_SERVICE:NET_CYC_Start failed, error code: %d. \n",__FUNCTION__,__LINE__, NET_DVE_GetLastError());
	}
    else
    {
	    DSP_INFO("DSP_SERVICE:dsp service start ok\n");
    }
    
#if 1
	VALID_LANE_NUM_PARAM vlnp;
	memset(&vlnp, 0, sizeof(vlnp));
	vlnp.chan_id = 0;
	vlnp.lane_id = g_uiClientLaneSum;
	#if 0 //add by fj
	CALL_CYC_CMD(NET_DEV_SET_VALID_LANE_NUM, vlnp);
	#endif
#if defined(CYC500KKW2C)
    shell_print("run CYC500KKW2C \n");
    ret = dspStationConfParam();
    #if defined(CYC500K)||defined(CYC500KD)
    ret = dspEpFuncEn(SINGAL_LANE_SET_MODE, 0);
    #endif
#elif defined(CYC500XKW4C)||defined(CYC500XAW)
    shell_print("run CYC500XKW4C \n");
    #if 0  //by fj
    ret = dspEPoliceConfParam();
    #endif
#endif
    if ( 0!=ret )
    {
        DSP_ERROR("%s %d conf Param failed .\n",__FUNCTION__,__LINE__);
    }
#endif

    return 0;
}
int dspSetEncParam(int chanid,EncParam* pParam)
{
    int ret;
	venc_param_t param;

    memset(&param, 0, sizeof(param));
	param.chan_id = chanid;
    param.encType = pParam->encType;
    param.imgFmt  = pParam->imgFmt;
    param.keyIntv = pParam->keyIntv;
    param.bitRate = pParam->bitRate;
    param.frameRate = pParam->frameRate;
    param.nEncQuality = pParam->nEncQuality;
    param.nMotionDetect = pParam->nMotionDetect;
    param.is_cbr  = pParam->is_cbr;
    if (param.encType != VTYPE_H264)
    {
        DSP_ERROR("%s %d encType wrong .\n",__FUNCTION__,__LINE__);
        param.encType = VTYPE_H264;
    }

    if (param.keyIntv <=0)
    {
        DSP_ERROR("%s %d keyIntv:0x%x wrong .\n",__FUNCTION__,__LINE__, param.keyIntv);
        param.keyIntv = 25;
    }
    if (param.bitRate < 128 || param.bitRate > 5*1024)
    {
        DSP_ERROR("%s %d bitRate 0x%x wrong .\n",__FUNCTION__,__LINE__, param.bitRate);
        param.bitRate = 2048;
    }
    if (param.frameRate <=0 || param.frameRate > 25)
    {
        DSP_ERROR("%s %d frameRate:0x%x wrong .\n",__FUNCTION__,__LINE__, param.frameRate);
        param.frameRate = 25;
    }
    
    #if defined(CYC500XZD)||defined(CYC200XZD)
	//dc1394_frameRate_set((float)(param.frameRate));  
	#endif

    if (param.nEncQuality < 0 || param.nEncQuality > 10)
    {
        DSP_ERROR("%s %d nEncQuality:%d wrong .\n",__FUNCTION__,__LINE__, param.nEncQuality);
        param.nEncQuality = 2;
    }

    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_VENC_PARAM, param);
    //sem_post(&sem_dsp);
    return ret;
}
int dspSetNamePos(int chanid,const NameParam *pParam)
{
    int ret;
    size_t namelen;
    channame_param_t param;
    memset(&param, 0, sizeof(param));
	param.chan_id = chanid;
    param.startX = pParam->startX;
    param.startY = pParam->startY;
    param.width  = pParam->width;
    param.height = pParam->height;
    namelen = strlen(pParam->name);
    memset(param.name,0,sizeof(param.name));
    namelen = namelen>=sizeof(param.name)?(sizeof(param.name) -1):namelen;
    memcpy(param.name,pParam->name,namelen);
    DSP_INFO("%s %d osd is:%s namelen:%d\n",__FUNCTION__,__LINE__,pParam->name, namelen);
    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_OSD_CHANNAME, param);
    //sem_post(&sem_dsp);

    return ret;
}
int dspSetJpgOsdString(int chanid,int vehicleId, char cType, const NameParam *pParam)
{
#if USE_NEW_OSD_SET
    int ret;
    size_t namelen;
    osd_param_t strParam;
    memset(&strParam, 0, sizeof(strParam));
	strParam.chan_id = chanid;
	strParam.lane_id = vehicleId;
    strParam.startX = pParam->startX;
    strParam.startY = pParam->startY;
    strParam.width  = pParam->width;
    strParam.height = pParam->height;
    namelen = strlen(pParam->name);
    memset(strParam.name,0,sizeof(strParam.name));
    namelen = namelen>=sizeof(strParam.name)?(sizeof(strParam.name)-1):namelen;
    memcpy(strParam.name,pParam->name,namelen);

    DSP_INFO("dspSetJpgOsdString,chanId %d,laneId %d,type:%d,content:%s.\n",chanid, vehicleId, cType, strParam.name);
    //sem_wait(&sem_dsp);
	switch(cType)
	{
		case PP_OSD_ROAD_NAME_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_PP_ROAD_OSD, strParam);
			break;
		case PP_OSD_ROAD_DIR_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_PP_ROAD_DIRECTION_OSD, strParam);
			break;
		case PP_OSD_ROAD_DESP_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_PP_ROAD_DESC_OSD, strParam);
			break;
		case PP_OSD_ROAD_LANE_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_PP_LANE_OSD, strParam);		
			break;
		case EP_OSD_ROAD_NAME_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_EP_ROAD_OSD, strParam);
			break;
		case EP_OSD_ROAD_DIR_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_EP_ROAD_DIRECTION_OSD, strParam);
			break;
		case EP_OSD_ROAD_DESP_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_EP_ROAD_DESC_OSD, strParam);
			break;
		case EP_OSD_ROAD_LANE_SET:
			ret = CALL_CYC_CMD(NET_DEV_SET_EP_LANE_OSD, strParam);
			break;
		default:
			break;
	}
    //sem_post(&sem_dsp);
    return ret;
#else
	return 0;
#endif
}
int dspSetSnapOsd(int chanid,int vehicleId,const NameParam *pParam)
{
	int ret = 0;
	size_t namelen;
	osd_param_t param;
	memset(&param, 0, sizeof(param));
	param.chan_id = chanid;
	param.lane_id = vehicleId;
	param.startX = pParam->startX;
	param.startY = pParam->startY;
	param.width  = pParam->width;
	param.height = pParam->height;
	namelen = strlen(pParam->name);
	memset(param.name,0,sizeof(param.name));
	namelen = namelen>=sizeof(param.name)?(sizeof(param.name)-1):namelen;
	memcpy(param.name,pParam->name,namelen);

	DSP_INFO("pp chanid %d vid %d x %d y %d wid %d hei %d content :%s .\n",chanid,vehicleId,param.startX,param.startY,param.width,param.height,param.name);
	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_PP_OSD_STRING, param);
	//sem_post(&sem_dsp);

	return ret;
}


int dspEpSetSnapOsd(int chanid,int vehicleId,const NameParam *pParam)
{
    int ret;
    size_t namelen;
    osd_param_t param;
    memset(&param, 0, sizeof(param));
	param.chan_id = chanid;
	param.lane_id = vehicleId;
    param.startX = pParam->startX;
    param.startY = pParam->startY;
    param.width  = pParam->width;
    param.height = pParam->height;
    namelen = strlen(pParam->name);
    memset(param.name,0,sizeof(param.name));
    namelen = namelen>=sizeof(param.name)?(sizeof(param.name)-1):namelen;
    memcpy(param.name,pParam->name,namelen);

    DSP_INFO("ep chanid %d vid %d x %d y %d wid %d hei %d content :%s .\n",chanid,vehicleId,param.startX,param.startY,param.width,param.height,param.name);
    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_EP_OSD_SET_STRING, param);
    //sem_post(&sem_dsp);

    return ret;
}

int dspEncStaCtl(int chanid,int status)
{
	return 1;
	
    int ret;
    if((g_cRecInitSta) || (g_cAlarmRecSta) || (g_cOnvifVideoSta) || (g_cNetRecVideoSta) )
    {
        status = 1;
        DSP_INFO("rec is on ,enc sta must 1\n");
    }
    else
    {
        DSP_INFO("dsp %d cha enc sta is %d\n", chanid, status);
    }
    if(status > 0)
    {
        g_strVRSta.iVideoEnable =1;
    }
    else
    {
        g_strVRSta.iVideoEnable =0;
    }

	VENC_STATUS_PARAM vsp;
	memset(&vsp, 0, sizeof(VENC_STATUS_PARAM));
	vsp.chan_id = chanid;
	vsp.status = status;

    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_VENC_STATUS, vsp);
    //sem_post(&sem_dsp);
    return ret;
}

int dspSetVLParam(int chanid,int iLaneId, int iShowFlag, VLParam *pParam)
{
    int ret;
    VirtualLoop_feature_t param;
    memset(&param, 0, sizeof(param));
	param.chan_id = chanid;
	param.lane_id = iLaneId;
	param.bShowFlag = iShowFlag;
    param.x1 = pParam->startX;
    param.y1 = pParam->startY;
    param.x2 = param.x1 + pParam->width;
    param.y2 = param.y1 + pParam->height;
    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_VIRTUAL_LOOP, param);
    //sem_post(&sem_dsp);
    DSP_INFO("Set vl param chan %d (%d %d ),(%d %d ) ret = %d .\n",chanid,param.x1,param.y1,param.x2,param.y2,ret);
    return ret;
}

int dspSetRedInfo(int lane)
{
    int ret;
    int iRedFlag =0;
    /* begin add 给DSP传递车检器红灯实际状态 */
    char cVehRedLSta = g_EPLaneSwitchInfo[lane].cRedLSta;
    iRedFlag = cVehRedLSta;
    iRedFlag <<= 16;
    /* end add */
    iRedFlag |= getTrafficLightSta(lane);
    DSP_INFO("%s %d lane:%d,iRedFlag:0x%x\n",__FUNCTION__,__LINE__,lane,iRedFlag);

	EP_REDLIGHT_FLAG_PARAM param;
	memset(&param, 0, sizeof(param));
	param.lane_id = lane;
	param.bRedFlag = iRedFlag;

    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_EP_REDLIGHT_FLAG, param);
    if(ret != 0)
    {
    	syslog(LOG_ERR|LOG_USER, "call NET_DEV_SET_EP_REDLIGHT_FLAG command failed.\n");
	}
    //sem_post(&sem_dsp);
    return ret;
}

int dspRedLightTimeSet(int chanid,int status)
{
    int ret;
    DSP_INFO("vid %d status %d .\n",chanid,status);
    struct timeval tv;
    struct timezone tz;

    gettimeofday (&tv , &tz);

    DSP_INFO("tv_sec:  %d\n", tv.tv_sec + 8*3600) ;
    DSP_INFO("tv_usec: %d\n", tv.tv_usec);

	NOTIFY_RED_LIGHT_PARAM param;
	memset(&param, 0, sizeof(NOTIFY_RED_LIGHT_PARAM));
	param.chan_id = 0;
	param.lane_id = chanid;
	param.sec = tv.tv_sec + 8 * 3600;
	param.msec = tv.tv_usec / 1000;
    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_NOTIFY_RED_LIGHT, param);
    if(ret < 0)
    {
    	syslog(LOG_ERR|LOG_USER, "call NET_DEV_SET_NOTIFY_RED_LIGHT command failed!\n");
		DSP_ERROR("Call NET_DEV_SET_NOTIFY_RED_LIGHT command failed, error code: %d.\n", NET_DVE_GetLastError());
    }
    //sem_post(&sem_dsp);

    return ret;
}

int DspAppDebugSwitch(int level)
{ 
    g_dsp_debug_module = level;
    return 0;
}

int dspUnInit()
{
   dspEncStaCtl(0,0);
   NET_CYC_Stop();
   NET_CYC_Cleanup();
   sem_destroy(&sem_dsp);
   sem_destroy(&sem_dsp_data);
   return 0;
} 

int dspReset()
{
    dspUnInit();
    dspInit();
    return 0;
}

int dspPrintVer()
{
    shell_print("dsp_sharelib version=%x, compile info:%s, description: %s.\n", sharelib_version_info.version, sharelib_version_info.szCompile, sharelib_version_info.szDesc);
    shell_print("dsp_firmware version=%x, compile info:%s, description: %s.\n", firmware_version_info.version, firmware_version_info.szCompile, firmware_version_info.szDesc);
	shell_print("link_epolice version info:%s.\n", link_ep_version_info.szCompile);
	shell_print("link_lpr version info:%s.\n", link_lpr_version_info.szCompile);
	shell_print("link_osd version info:%s.\n", link_osd_version_info.szCompile);
	shell_print("link_redlight version info:%s.\n", link_rlt_version_info.szCompile);
    return 0;
}

int getCameraInfo(int *pWidth,int *pHeight,int *pPixels)
{
    if ( (NULL == pWidth) || (NULL == pHeight) || (NULL == pPixels) )
    {
        return -1;
    }
    *pWidth = HDConf.image_width;
    *pHeight = HDConf.image_height;
    *pPixels = HDConf.totalpixels/10000;
    if(*pWidth == 0)
    {
        *pWidth = 1600;
    }
    if(*pHeight == 0)
    {
        *pHeight = 1216;
    }
    if(*pPixels == 0)
    {
        *pPixels = 200;
    }
    return 0;
}

int dspSetSnapArea(int vid,int startX,int startY,int width,int height, int iShowFlag)
{
    int ret;
    HSLRECT rect;
    memset(&rect, 0, sizeof(rect));
    
	rect.chan_id = 0;
	rect.lane_id = vid;
    rect.startX = startX;
    rect.startY = startY;
    rect.width = width;
    rect.height = height;
    rect.bShow = iShowFlag;
    DSP_INFO("X %d Y %d width %d height %d. showFlag:%d\n",rect.startX,rect.startY,rect.width,rect.height, rect.bShow);
    //sem_wait(&sem_dsp);
    ret = CALL_CYC_CMD(NET_DEV_SET_GT5000_SNAPSHOT_SIZE_SET, rect);
    if ( ret != 0)
    {
        DSP_ERROR("%s %d return failed .\n",__FUNCTION__,__LINE__);
    }
    //sem_post(&sem_dsp);
    return ret;
}


