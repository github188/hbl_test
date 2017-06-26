#ifndef _MMSSEND_H__
#define _MMSSEND_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ACTIVELEN 1024
#define DIRECTSEND_BANDWITH     (0xFFFFFFFF)

typedef struct
{
	unsigned int dwAddr;
	unsigned short nPort;
	unsigned int splitMode;
	unsigned int transMode;
}ConnInfo;

typedef struct 
{
       unsigned int nPackLen;                    // 激活包长度
       char ucPackBuf[ACTIVELEN];                     // 激活包内容
} __attribute__((packed)) TActivePack, * LPTActivePack;


/*nMaxChanNum=总的视频通道数+1路音频通道数*/
int MmsSendInit(unsigned int nMaxChanNum,unsigned int nMaxBandWidth );
int MmsSendInitEx(unsigned int nMaxChanNum,unsigned int nMaxBandWidth, int nSmoothSend );
int MmsSendUninit();



/*=================================================================================
MmsStart函数:

nChannel:      视频通道号，从0开始到TOTAL_CHANNEL_NUM-1,音频通道号是用TOTAL_CHANNEL_NUM
nActivate:	激活选项，定义见视频请求协议
dwAddr:		请求视频IP	
nPort:		请求视频端口	
splitMode:	请求视频分包模式	(NWJK,RTP 等)
transMode:	请求视频传输模式	(UDP,TCP等)
pAactivePack:	激活包数据
nBandWidth:	每路码流发送带宽


返回值:
            //   该通道目前不存在	0x01
            //-1 传输模式错误
            //-2 分包模式错误
            //-3 激活包长度错误
            //-4 开启连接失败
            //0  连接成功
=================================================================================*/
int MmsStart(int nChannel,int nActivate,unsigned int dwAddr,unsigned short nPort,int splitMode,int transMode,LPTActivePack pActivePack);

int MmsStartEx(int nChannel,
                    int nActivate,
                    unsigned int dwAddr,
                    unsigned short nPort,
                    int splitMode,
                    int transMode,
                    LPTActivePack pActivePack,
                    unsigned char *pSessionId,
                    int nSessionIdLen,
                    int nBufferSendFlag);



//返回与视频MmsStart相同
int MmsStop(int nChannel,unsigned int dwAddr,unsigned short nPort,int splitMode,int transMode);

int MmsStopEx(unsigned char *pSessionId, int nSessionIdLen);
/*=================================================================================
//0		表示成功
//-1	发送模块未初始化
//-2	通道无效或帧数据未空
//-3	帧数据无效
=================================================================================*/
#if defined(_BAND_WIDTH_)
int MmsDataWrite(int nChannel,unsigned char *pFrame,int nFrameLen, int nBandWidth);
#else
int MmsDataWrite(int nChannel,unsigned char *pFrame,int nFrameLen);
#endif
int MmsBufferDataWrite(int nChannel,unsigned char *pFrame,int nFrameLen);
int MmsChannelGetBufStatus(int nChannel);
int MmsChannelSetBufStatus(int nChannel, int BufferStatus);

/*=================================================================================
//-1	通道无效
//-2	nMaxCount==0或pConnInfo==NULL
//0		表示无连接，大于0表示该通道连接数
=================================================================================*/
int MmsGetConnect(int nChannel,ConnInfo *pConnInfo,int nMaxCount);




int MmsDebugSwitch(int nLevel);
int MmsDump();

//设置发送带宽，单位kbps
//Bandwidth:发送带宽
//SendStrategy:发送策略 0:平滑发送 1:最快发送
int setBandwidth(int Bandwidth, int SendStrategy);

#ifdef __cplusplus
}
#endif

#endif

