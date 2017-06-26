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
       unsigned int nPackLen;                    // ���������
       char ucPackBuf[ACTIVELEN];                     // ���������
} __attribute__((packed)) TActivePack, * LPTActivePack;


/*nMaxChanNum=�ܵ���Ƶͨ����+1·��Ƶͨ����*/
int MmsSendInit(unsigned int nMaxChanNum,unsigned int nMaxBandWidth );
int MmsSendInitEx(unsigned int nMaxChanNum,unsigned int nMaxBandWidth, int nSmoothSend );
int MmsSendUninit();



/*=================================================================================
MmsStart����:

nChannel:      ��Ƶͨ���ţ���0��ʼ��TOTAL_CHANNEL_NUM-1,��Ƶͨ��������TOTAL_CHANNEL_NUM
nActivate:	����ѡ��������Ƶ����Э��
dwAddr:		������ƵIP	
nPort:		������Ƶ�˿�	
splitMode:	������Ƶ�ְ�ģʽ	(NWJK,RTP ��)
transMode:	������Ƶ����ģʽ	(UDP,TCP��)
pAactivePack:	���������
nBandWidth:	ÿ·�������ʹ���


����ֵ:
            //   ��ͨ��Ŀǰ������	0x01
            //-1 ����ģʽ����
            //-2 �ְ�ģʽ����
            //-3 ��������ȴ���
            //-4 ��������ʧ��
            //0  ���ӳɹ�
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



//��������ƵMmsStart��ͬ
int MmsStop(int nChannel,unsigned int dwAddr,unsigned short nPort,int splitMode,int transMode);

int MmsStopEx(unsigned char *pSessionId, int nSessionIdLen);
/*=================================================================================
//0		��ʾ�ɹ�
//-1	����ģ��δ��ʼ��
//-2	ͨ����Ч��֡����δ��
//-3	֡������Ч
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
//-1	ͨ����Ч
//-2	nMaxCount==0��pConnInfo==NULL
//0		��ʾ�����ӣ�����0��ʾ��ͨ��������
=================================================================================*/
int MmsGetConnect(int nChannel,ConnInfo *pConnInfo,int nMaxCount);




int MmsDebugSwitch(int nLevel);
int MmsDump();

//���÷��ʹ�����λkbps
//Bandwidth:���ʹ���
//SendStrategy:���Ͳ��� 0:ƽ������ 1:��췢��
int setBandwidth(int Bandwidth, int SendStrategy);

#ifdef __cplusplus
}
#endif

#endif

