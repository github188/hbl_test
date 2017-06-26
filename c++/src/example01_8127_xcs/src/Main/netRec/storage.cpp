#include <sys/stat.h>
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "SysHandle.h"
#include "MmsSend.h"
#include "storage_post.h"
#include "storage_define.h"
#include "dsp.h"
#include "memShm.h"
#include "dvs_default.h"

#if defined(HI_PLATFORM)
#include <syslog.h>
#endif

#if defined(NRU)
#include "storage_manager.h"
#endif

#if MODULE_NETREC

#define TIME_DELAY       10  //线程调度间隔 10
#define RTCP_PERIOD      20  //RTCP发送间隔 20

#if defined(NRU)
void *StorageListenNruFunc(void *pArg);
void *StorageNrmManageFunc(void *pArg);
#endif
int StorageThreadCreate(void);
void *StorageThread(void *pArg);

typedef enum _enumBufStatus
{
	BUF_STATUS_NO_NEED,
	BUF_STATUS_NO_SENDING,
	BUF_STATUS_SENDING,
	BUF_STATUS_SENT_FINDSH
}EMmsBufStatus;

typedef struct  __DSP_STOR_DATA_INFO
{
    void* frame_data[MAX_BUFFERD_FRAME_NUM];
    int frame_len[MAX_BUFFERD_FRAME_NUM];
    long long cur_data_index;//视频数据接收索引
    long long cur_send_index;//视频数据发送索引
    int first_key_frame;
}DSP_STOR_DATA_INFO;

typedef struct __DSP_BUF_MSG
{
    unsigned int nChannel;
    unsigned char *pFrame;
    unsigned int nFrameLen;
    unsigned int num;
}DSP_BUF_MSG;

static DSP_STOR_DATA_INFO dsp_stor_data_info[OUTPUT_CHANNEL_NUM] = {{0,},};
#define MAX_BUF_MSG_NUM 50

pthread_t thread_storage;
static sem_t s_sem_storage_input;
static MSG_Q_ID s_msgq_storage_output;

extern volatile char s_ChannelStorageStatus[OUTPUT_CHANNEL_NUM];

int g_Storage_Debug = 0xff;

static int s_iNrmManageThread  =0;
static int s_iNrmStorageThread =0;

int StorageDebugSwitch(int nLevel)
{
    g_Storage_Debug = nLevel;
    return 0;
}

int StorageInit(void)
{
    int nRet = -1;
    pthread_t threadRecListenNru;
    pthread_t threadNrmManageFunc;
    
    nRet = StorageCfgInit();
    if(0!=nRet)
    {
        STG_ERROR("%s:%d StorageCfgInit() failed.\n",__FUNCTION__,__LINE__);
        goto error_ret;
    }
    STG_ERROR("%s:%d StorageCfgInit() success.\n",__FUNCTION__,__LINE__);
    
  	#if defined(NRU)
  	
    nRet = pthread_create(&threadRecListenNru,NULL,StorageListenNruFunc,NULL);
    if(0 != nRet)
    {
        STG_ERROR("%s:%d pthread_create() failed.\n",__FUNCTION__,__LINE__);
        goto error_ret;
    }
    STG_ERROR("%s:%d pthread_create() StorageListenNruFunc success.\n",__FUNCTION__,__LINE__);

    nRet = pthread_create(&threadNrmManageFunc,NULL,StorageNrmManageFunc,NULL);
    if(0 != nRet)
    {
        STG_ERROR("%s:%d pthread_create() failed.\n",__FUNCTION__,__LINE__);
        goto error_ret;
    }
    STG_ERROR("%s:%d pthread_create() StorageNrmManageFunc success.\n",__FUNCTION__,__LINE__);
	
	#endif

    nRet = StorageThreadCreate();
    if(nRet != 0)
    {
        STG_ERROR( "DSP_COM: StorageThreadCreate failed \n");
        syslog(LOG_INFO,"DSP_COM: StorageThreadCreate failed \n");
        return -1;
    }

error_ret:
    return nRet;
}

int StorageUninit(void)
{
	s_iNrmManageThread  =0;
	s_iNrmStorageThread =0;
	usleep(10000);
	return 0;
}


#if defined(NRU)

void *StorageListenNruFunc(void *pArg)
{
    int nRet = 0;
    nRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    if(0!=nRet)
    {
        STG_ERROR("%s:%d pthread_setcancelstate() failed.\n",__FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }

    nRet = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    if(0!=nRet)
    {
        STG_ERROR("%s:%d pthread_setcanceltype() failed.\n",__FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }
	writeThreadInfo("nrmListen");
	nasHttp_Listen(8082);
	return NULL;
}


void *StorageNrmManageFunc(void *pArg)
{
    int nRet = 0;
    int nRet1 = 0;
    int nChannel = 0;
    int nRecvChan = 0;
    unsigned int uiCurLocalTime = 0;
    unsigned int second = 1;
    MSG_Q_ID RedirectMsgQ = NULL;
    unsigned int delay;

    nRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    if(0!=nRet)
    {
        STG_ERROR("%s:%d pthread_setcancelstate() failed.\n",__FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }

    nRet = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    if(0!=nRet)
    {
        STG_ERROR("%s:%d pthread_setcanceltype() failed.\n",__FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }

	writeThreadInfo("nrmManage");

	s_iNrmManageThread =1;

    while(s_iNrmManageThread)
    {
        uiCurLocalTime = time(NULL)+UTC_TIME_ZONE;
        for(nChannel=0; nChannel < OUTPUT_CHANNEL_NUM;nChannel++)
        {
            STG_INFO("uiCurLocalTime = %d\n", uiCurLocalTime);

            if(uiCurLocalTime%RTCP_PERIOD == 0)
            {
                nasChannelStorageNruScan(nChannel);//三次未收到RTCP回应
            }

            nasChannelStorageStart(nChannel);
        }

        second = time(NULL);
        delay = TIME_DELAY - (second%TIME_DELAY);

        RedirectMsgQ = nasRedirectMsgQGet();

        nRet1 = msgQReceive(RedirectMsgQ, (char*)(&nRecvChan), sizeof(nRecvChan), 1000*delay);
        if(nRet1 == sizeof(nRecvChan))
        {
        	STG_INFO("%s %s:%d nRet1=%d. nRecvChanlen=%d, redirect!\n",__TIME__,__FUNCTION__,__LINE__, nRet1, sizeof(nRecvChan));
            nasChannelStorageStop(nRecvChan);
        }
        STG_INFO("%s %s:%d second=%d. delay=%d\n",__TIME__,__FUNCTION__,__LINE__, time(NULL), delay);

    }
    
    for(int i =0; i< MAX_CHANNEL_NUM; i++)
    {
    	nasChannelStorageStop(i);
    }
    return NULL;
}

#endif

int StorageDataWrite(int chanId, void *shareBuff, int nLen)
{
    int msg_num = 0;
    int ret = -1;
    DSP_BUF_MSG send_msg = {0,};

    if(NULL == shareBuff)
    {
        STG_ERROR("shareBuff==NULL\n");
        return -5;
    }

	if(!s_ChannelStorageStatus[chanId])//未开启录像则不发送视频
	{
        STG_INFO("chanId:%d not start rec,return!\n");
		return -2;
	}

    /* 有太多消息未处理，则不发送,防止内存耗尽 */
    msg_num = msgQNumMsgs(s_msgq_storage_output);
    if(msg_num >= MAX_BUF_MSG_NUM)
    {
        STG_ERROR("msgQ num is full,free mem and return.\n");
        return -3;
    }

    send_msg.pFrame = (unsigned char *)shareBuff;
    send_msg.nChannel = chanId;
    send_msg.nFrameLen = nLen;
    ret = msgQSend(s_msgq_storage_output, (char*)&send_msg, sizeof(send_msg), 0);
    if(ret != sizeof(DSP_BUF_MSG))
    {
        STG_ERROR("msgQ send failed,free mem and return.\n");
        return -4;
    }
    return ret;
}

int StorageThreadCreate(void)
{
    int nRet;
    int i;

	memset(dsp_stor_data_info, 0, sizeof(dsp_stor_data_info));	

    for(i = 0; i < OUTPUT_CHANNEL_NUM; i++)
    {
        dsp_stor_data_info[i].cur_data_index = MAX_BUFFERD_FRAME_NUM;//索引从最大缓存帧数开始，通过视频数据指针判断有无数据
        dsp_stor_data_info[i].cur_send_index = MAX_BUFFERD_FRAME_NUM;
    }

    if(sem_init(&s_sem_storage_input, 0 ,1) < 0)
    {
        STG_ERROR("%s:%d  sem_init failed\n", __FILE__, __LINE__);
        return -1;
    }

    s_msgq_storage_output = msgQCreate(MAX_BUF_MSG_NUM, sizeof(DSP_BUF_MSG), 0);

    nRet = pthread_create(&thread_storage, NULL, StorageThread, NULL);
    if(0 != nRet)
    {
        STG_ERROR("mm_data_process pthread_create failed\n");
    }
    return nRet;
}

/* 获取缓冲区MAX_BUFFERD_FRAME_NUM个数据中的第一个I帧,返回第一个I帧的索引 */
long long  BufferGetFirstIFrame(int chan_id)
{
    long long buf_pos;
    long long buf_pos_start = 0;
    long long buf_pos_end = 0;
    long long i;
    DSP_STOR_DATA_INFO *pInfo = NULL;
    FRAME_INFO_S frame_info;
    long long nRet = -1;
    int nRet2 = -1;

    pInfo = &dsp_stor_data_info[chan_id];
    buf_pos_start = pInfo->cur_data_index - MAX_BUFFERD_FRAME_NUM;
    buf_pos_end = pInfo->cur_data_index;

    STG_INFO("%s:%d channel%d BUF_STATUS_NO_SENDING\n",__FILE__, __LINE__, chan_id);  
    STG_INFO("%s:%d channel:%d %llu-%llu\n",__FILE__, __LINE__, chan_id, buf_pos_start, buf_pos_end);  
    STG_INFO("%s:%d channel:%d %llu-%llu\n",__FILE__, __LINE__, chan_id, buf_pos_start, buf_pos_end);  

    for(i = buf_pos_start; i < buf_pos_end; i++)
    {
        buf_pos = i%MAX_BUFFERD_FRAME_NUM;

        if(NULL != pInfo->frame_data[buf_pos])
        {
            nRet2 = GetFrameInfo((unsigned char *)pInfo->frame_data[buf_pos], pInfo->frame_len[buf_pos], &frame_info);

            if((0 == nRet2)&&(FRAME_KEY==frame_info.frame_type)&&(frame_info.codec_type<ATYPE_NULL))
            {
                nRet = i;
                STG_INFO("%s:%d channel:%d first_key_frame:%llu %llu\n",__FILE__, __LINE__, chan_id, buf_pos, nRet);  
                break;
            }
            else
            {
                STG_INFO("%s:%d channel:%d not key frame %llu\n",__FILE__, __LINE__, chan_id, buf_pos);
                STG_INFO("%s:%d frame_type:%x %x nRet2:%d\n",__FILE__, __LINE__, frame_info.frame_type, *(int*)pInfo->frame_data[buf_pos],nRet2);
                continue;
            }
        }
        else
        {
            STG_INFO("%s:%d chan_id:%d (NULL == pInfo->frame_data[%llu])\n",__FILE__, __LINE__, chan_id, buf_pos);  
        }
    }
    return nRet;
}

#define SEND_FRAMS_PER_LOOP 2  

int BufferGetCurrentSendDatePos(int chan_id, long long *pos_start, long long *pos_end)
{
    int buf_status = BUF_STATUS_NO_NEED;
    long long buf_pos =0;
    long long buf_pos_start = 0;
    long long buf_pos_end =0;
    DSP_STOR_DATA_INFO *pInfo = &dsp_stor_data_info[chan_id];
    /* 发送缓存数据的速率取决于设备的通道数*SEND_FRAMS_PER_LOOP */
    int ret = -1;

    buf_status = MmsChannelGetBufStatus(chan_id);
    if(BUF_STATUS_NO_SENDING == buf_status)
    {
        buf_pos = BufferGetFirstIFrame(chan_id);
        STG_INFO("%s:%d first i frame:%lld\n",__FILE__, __LINE__, buf_pos);

        /* 找到I帧 */
        if(-1 != buf_pos)
        {
            pInfo->cur_send_index = buf_pos;
            buf_pos_start = pInfo->cur_send_index;

            if(buf_pos_start + SEND_FRAMS_PER_LOOP >= pInfo->cur_data_index)
            {
                buf_pos_end = pInfo->cur_data_index;
                /* 找到I帧，I帧和数据索引相差不到每次发送的数据，缓存数据即将发送完 */
                buf_status = BUF_STATUS_SENT_FINDSH;
            }
            else
            {
                buf_pos_end = buf_pos_start + SEND_FRAMS_PER_LOOP;
                /* 找到I帧，从第一个I帧开始发送 */
                buf_status = BUF_STATUS_SENDING;
            }
            MmsChannelSetBufStatus(chan_id, buf_status);
            //STG_INFO("%s:%d\n",__FILE__, __LINE__);
            STG_INFO("%s:%d channel:%d I %lld-%lld\n",__FILE__, __LINE__, chan_id, 
                                                buf_pos_start%MAX_BUFFERD_FRAME_NUM, 
                                                buf_pos_end%MAX_BUFFERD_FRAME_NUM);
            ret = 0;
        }       
        else
        {
            /* 历史数据没有I帧，不需要发送历史数据 */
            buf_status = BUF_STATUS_SENT_FINDSH;
            MmsChannelSetBufStatus(chan_id, buf_status);
            buf_pos_start = pInfo->cur_data_index;
            buf_pos_end = pInfo->cur_data_index;
            STG_INFO("%s:%d histroy no i frame:%lld\n",__FILE__, __LINE__, buf_pos);
            ret = -1;
        }
    }
    else if(BUF_STATUS_SENDING == buf_status)
    {
        buf_pos_start = pInfo->cur_send_index + 1;
        if(buf_pos_start + SEND_FRAMS_PER_LOOP >= pInfo->cur_data_index)
        {
            buf_pos_end = pInfo->cur_data_index;
            buf_status = BUF_STATUS_SENT_FINDSH;
            MmsChannelSetBufStatus(chan_id, buf_status);
        }
        else
        {
            buf_pos_end = buf_pos_start + SEND_FRAMS_PER_LOOP;
        }
        STG_INFO("%s:%d channel %d BUF_STATUS_SENDING %llu-%llu  %llu\n",__FILE__, __LINE__, chan_id, 
                                                                buf_pos_start%MAX_BUFFERD_FRAME_NUM, 
                                                                buf_pos_end%MAX_BUFFERD_FRAME_NUM, 
                                                                pInfo->cur_data_index%MAX_BUFFERD_FRAME_NUM);
        ret = 0;
    }
    else if(BUF_STATUS_SENT_FINDSH == buf_status)
    {
        if(pInfo->cur_send_index < pInfo->cur_data_index - 1)
        {
            buf_pos_start = pInfo->cur_send_index + 1;
            buf_pos_end = pInfo->cur_data_index;
            ret = 0;
        }
        else
        {
            ret = -1;
        }
    }
    else
    {
        ret = -1;
    }
    *pos_start = buf_pos_start;
    *pos_end = buf_pos_end;
    return ret;
}

void *StorageThread(void *pArg)
{
    int chan_id;
    long long buf_pos;
    long long buf_pos_start = 0;
    long long buf_pos_end =0;
    int ret = -1;
    long long i;
    DSP_STOR_DATA_INFO *pInfo = NULL;
    DSP_BUF_MSG recv_msg = {0, };
    int msg_len;
    long long cur_buf_index;
    int nRet = -1;

    nRet = pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    if(0!=nRet)
    {
        STG_INFO("%s:%d pthread_setcancelstate() failed.\n",__FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }

    nRet = pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    if(0!=nRet)
    {
        STG_INFO("%s:%d pthread_setcanceltype() failed.\n",__FUNCTION__,__LINE__);
        exit(EXIT_FAILURE);
    }

#if defined(DM368) || defined(HI_PLATFORM)
    STG_INFO("%s %s %d  pid:%d \n", __FILE__, __FUNCTION__, __LINE__, gettid());
    STG_INFO("%s %s %d  pid:%d \n", __FILE__, __FUNCTION__, __LINE__, gettid());
#else
    STG_INFO("%s %s %d  pid:%d \n", __FILE__, __FUNCTION__, __LINE__, getpid());
#endif

	writeThreadInfo("nrmStorage");

	s_iNrmStorageThread =1;

    while(s_iNrmStorageThread)
    {
        msg_len = msgQReceive(s_msgq_storage_output, (char*)&recv_msg, sizeof(recv_msg), 120);

        /* 如果收到通道号有误则不处理 */
        if(recv_msg.nChannel >= INPUT_CHANNEL_NUM)
        {
        	STG_ERROR("channel:%d is too big,continue.\n", recv_msg.nChannel);
            continue;
        }
        
        if(msg_len == sizeof(recv_msg))
        {
            pInfo = &dsp_stor_data_info[recv_msg.nChannel];
            cur_buf_index = pInfo->cur_data_index%MAX_BUFFERD_FRAME_NUM;

            if(NULL != pInfo->frame_data[cur_buf_index])
            {
                memShmFree(pInfo->frame_data[cur_buf_index]);
            }
            pInfo->frame_data[cur_buf_index] = recv_msg.pFrame;
            pInfo->frame_len[cur_buf_index] = recv_msg.nFrameLen;
            pInfo->cur_data_index++;

            /* 存储压力测试数据复制第0路数据 */
            if(0 == recv_msg.nChannel)
            {
                for(chan_id = INPUT_CHANNEL_NUM; chan_id < OUTPUT_CHANNEL_NUM; chan_id++)
                {
                    pInfo = &dsp_stor_data_info[chan_id];
                    cur_buf_index = pInfo->cur_data_index%MAX_BUFFERD_FRAME_NUM;
                    
                    pInfo->frame_data[cur_buf_index] = recv_msg.pFrame;
                    pInfo->frame_len[cur_buf_index] = recv_msg.nFrameLen;
                    pInfo->cur_data_index++;
                }
            }
        }
        
        for(chan_id = 0; chan_id < OUTPUT_CHANNEL_NUM; chan_id++)
        {
            pInfo = &dsp_stor_data_info[chan_id];

            ret = BufferGetCurrentSendDatePos(chan_id, &buf_pos_start, &buf_pos_end);
            if(-1 == ret)
            {
            	
                continue;
            }
            
            for(i = buf_pos_start; i < buf_pos_end; i++)
            {
                buf_pos = i%MAX_BUFFERD_FRAME_NUM;
                
                if(NULL != pInfo->frame_data[buf_pos])
                {
                    ret = MmsBufferDataWrite(chan_id, (unsigned char *)pInfo->frame_data[buf_pos], pInfo->frame_len[buf_pos]);
                }
                pInfo->cur_send_index = i;
            }
        }
    }

	for(int i=0; i<OUTPUT_CHANNEL_NUM; i++)
	{
	    pInfo = &dsp_stor_data_info[i];
        cur_buf_index = pInfo->cur_data_index%MAX_BUFFERD_FRAME_NUM;

        if(NULL != pInfo->frame_data[cur_buf_index])
        {
            memShmFree(pInfo->frame_data[cur_buf_index]);
        }
	}
    //STG_INFO("buf_index:%d\n", cur_buf_index);
    return NULL;
}


#endif

