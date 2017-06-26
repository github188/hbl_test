#ifndef __MMS_SEND_DEFINE_H__
#define __MMS_SEND_DEFINE_H__


#include"MmsLimit.h"
#include"msgQLib.h"

#include <assert.h>

extern int g_Mms_Debug;

#if 1
#define ASSERT(x)   assert(x)
#else
#define ASSERT(x)   ((void)0)
#endif

//#define shell_print printf
//#define MMSSEND_DEBUG printf


#ifdef MMS_MODULE_DEBUG
    #define MMSSEND_DEBUG(x...)\
    do{\
        if(g_Mms_Debug)\
            shell_print(x);\
    }while(0);
#else
    #define MMSSEND_DEBUG(x...) printf(x)
#endif
//���ڴ�ӡģ�������Ϣ��������ȫ�ֱ���g_Mms_Debug��������
#if 1
#define MMSSEND_ERROR(x...)\
do{\
    if(g_Mms_Debug&0x0001)\
        shell_print("MMS mode:"x);\
}while(0);
#else
#define MMSSEND_ERROR(x...)printf(x)
#endif

//���ڴ�ӡģ�龯����Ϣ��������ȫ�ֱ���g_Mms_Debug��������
#if 1
#define MMSSEND_WARNING(x...)\
do{\
    if(g_Mms_Debug&0x0002)\
        shell_print("MMS mode:"x);\
}while(0);
#else
#define MMSSEND_WARNING(x...)   printf(x)
#endif

//���ڴ�ӡģ����ʾ��Ϣ��������ȫ�ֱ���g_Mms_Debug��������
#if 1
#define MMSSEND_INFO(x...)\
do{\
    if(g_Mms_Debug&0x0004)\
        shell_print("MMS mode:"x);\
}while(0);
#else
#define MMSSEND_INFO(x...) printf(x)
#endif

#define KJ_XC_MANUCODE      (33)//kejun ���Ų�V3ƽ̨�ϵĳ��̴���

#define MAX_CHANNEL_NUM (1)

#define MAX_SEND_QUEUE_NUM  (128)
#define MAX_EVENT_QUEUE_NUM (32)

#define NWJK_PACKET_HEAD    (10)
#if defined(RTP_SUPPORT)//�����ӵ�
#define NWJK_PACKET_PAYLOAD (1380)   
#else
#define NWJK_PACKET_PAYLOAD (1012)   
#endif

#define NWJK_PACKET_LEN     (NWJK_PACKET_HEAD+NWJK_PACKET_PAYLOAD)

#define RTP_PACKET_HEAD (20)
#if defined(RTP_SUPPORT)//�����ӵ�
#define RTP_PACKET_PAYLOAD  (1360)   
#else
#define RTP_PACKET_PAYLOAD  (1002)   
#endif
#define RTP_PACKET_LEN      (RTP_PACKET_HEAD+RTP_PACKET_PAYLOAD)

#define CONST_MAX_SPLIT_TYPES   (12+1)
#define CONST_MAX_TRANS_TYPES   (4)

#define MMS_TIME_INTERVAL   (18)

#define RTP_XC_PACKET_HEAD (16+16 + NWJK_PACKET_HEAD)  //ͷ��16�ֽڣ���չͷ��16�ֽڣ�

#define RTP_XC_PACKET_PAYLOAD   (NWJK_PACKET_PAYLOAD)
#define RTP_XC_PACKET_LEN   (RTP_XC_PACKET_HEAD+RTP_XC_PACKET_PAYLOAD)

typedef struct tagAuxInfo
{
    unsigned char cBufRemain[NWJK_PACKET_LEN];
    int nRemainlen;
    unsigned char nEncType;
    unsigned char nFrameType;
    unsigned char nFrameTypePre;
    unsigned int nSequenceNum;
}AuxInfo;

typedef struct tagNetSendMsg
{
    int nChannel;
    int nSplitMode;
    unsigned char *pPacket;
    unsigned int nPacketLen;
}NetSendMsg;

typedef struct tagEventMsg
{
    int nEvent;
    int nChannel;
    int splitMode;
    unsigned int transMode;
    unsigned int nBandWidth;
    unsigned int dwAddr;
    unsigned short nPort;
    char state;
    char reserve;
    void *pArg;
}EventMsg;

typedef struct tagSplitMsg
{
    unsigned int nChannel;
    unsigned char *pFrame;
    unsigned int nFrameLen;
    unsigned int num;
}SplitMsg;

#ifndef _SENDBOX_
#if defined(RTP_SUPPORT)//�����ӵ�
    #define SEND_BOX_LEN 28
#else
    #define SEND_BOX_LEN 56
#endif
#else
    #define SEND_BOX_LEN 0
#endif

#define TCP_OFFSET 4
typedef struct
{
    unsigned short tcpMagic;    //�ֽ�����tcp
    unsigned short tcpPackLen;
}__attribute__((packed))tcpHead;

typedef struct 
{
    unsigned char codec_type;
    unsigned char frame_type;
    unsigned int  seq_num;
    unsigned short data_len;
    unsigned short resesrved;
    unsigned char data[NWJK_PACKET_PAYLOAD];
}__attribute__((packed))NwjkMediaPacket;

typedef struct
{   
    unsigned char pSendBox[SEND_BOX_LEN+TCP_OFFSET];
    NwjkMediaPacket nwjkData;
}__attribute__((packed))nwjkNetPacket;

typedef struct rtpxc
{
    unsigned char cc:4;
    unsigned char exten:1;
    unsigned char pad:1;    
    unsigned char ver:2;
    unsigned char payLoad:7;
    unsigned char marker:1;
    
    unsigned short seq;
    
    unsigned int timeStamps;
    unsigned char ssrc[4];
    unsigned char csrc[4];
}__attribute__((packed))RtpXCHead; // ��16�ֽ�

/*�Ų�RTP�����ֶ�*/
typedef struct 
{
    unsigned short manuCode; //��unsigned short����ֵ  �����̴��� �����ֽ���
    unsigned short nLen;//��ʾ��չͷ����ռ���ٸ����ֽڣ���5
    unsigned char PUID[8]; //�豸ID��long long��ת�����ַ����Ͳ���18λʱ����λ��0
    unsigned char frameType;//֡���ͱ�ʶ
                        //=0x36,��ʶ��ƵI֡��
                        //=0x37,��ʶ��ƵP֡��
                        //=0x38,��ʾ��ƵB֡��
                        //=0x39,��ʶ��Ƶ֡��
    unsigned char EndMark;//��ǰ֡��������ǣ�0x30����ʾ�ð����ǽ�������0x31����ʾ�ð��ǽ�������
    unsigned char mediatype;//��������NAT�򶴰�
                            //�� 0x30����ʾ������Ƶ���ݣ�
                            //�� 0x31����ʾ�򶴰�,ý�����������
                            //�� 0x32����ʾǰ��¼��ط����ݣ�
                            //�� 0x33����ʾRTCP����
    unsigned char channelID;//ͨ����ţ�
//  unsigned char CUID[8];
}__attribute__((packed))RtpXCExtern;//��16�ֽ�

/*�Ų�RTPý���*/
typedef struct 
{
    /*RTPͷ������չͷ��*/
    RtpXCHead rtpxcHeadFlag;
    RtpXCExtern rtpxcExternFlag;
    
    /*NWJKͷ��*/
    unsigned char codec_type;
    unsigned char frame_type;
    unsigned int  seq_num;
    unsigned short data_len;
    unsigned short resesrved;   
    
    unsigned char data[NWJK_PACKET_PAYLOAD];
    
}__attribute__((packed))rtpxcMediaPacket;

/*�Ų�RTPý���*/
typedef struct 
{
    /*RTPͷ������չͷ��*/
    RtpXCHead rtpxcHeadFlag;
    RtpXCExtern rtpxcExternFlag;

    unsigned char data[NWJK_PACKET_PAYLOAD];
    
}__attribute__((packed))rtpxcAudioPacket;/*�Ų�RTP�����*/


typedef struct 
{
    unsigned char pSendBox[SEND_BOX_LEN+TCP_OFFSET];
    rtpxcMediaPacket rtpxcData;
}__attribute__((packed))rtpxcNetPacket;

typedef struct rtp
{
    unsigned char cc:4;
    unsigned char exten:1;
    unsigned char pad:1;    
    unsigned char ver:2;
    unsigned char payLoad:7;
    unsigned char marker:1;
    
    unsigned short seq;
    
    unsigned int timeStamps;
    unsigned char ssrc[4];
}__attribute__((packed))RtpHead;

typedef struct 
{
    unsigned short profile;
    unsigned short nLen;
    unsigned char bReserved;
    unsigned char bExtTimeStamps;
    unsigned char factoryCode:7;
    unsigned char s:1;  
    unsigned char frameFomat:5; 
    unsigned char frameType:3;
}__attribute__((packed))RtpExtern;

typedef struct 
{
    RtpHead rtpHeadFlag;
    RtpExtern rtpExternFlag;
    unsigned char data[RTP_PACKET_PAYLOAD];
}__attribute__((packed))rtpMediaPacket;

typedef struct 
{
    unsigned char pSendBox[SEND_BOX_LEN+TCP_OFFSET];
    rtpMediaPacket rtpData;
}__attribute__((packed))rtpNetPacket;


#ifndef _SENDBOX_
typedef enum{
    UNKNOW,
    SENDBOX_MODE = 1,
    CAST_MODE ,
    TCP_MODE ,
    UDP_MODE ,
} NET_MODE;
inline const char *TRANS_MODE_MAP(unsigned int transMode)
{
    switch(transMode)
    {
        case UNKNOW:
            return "UNKNOWN";
        case SENDBOX_MODE:
            return "SENDBOX";
        case CAST_MODE:
            return "MCAST";
        case TCP_MODE:
            return "TCP";
        case UDP_MODE:
            return "UDP";
        default :
            break;
    }

}
#else
typedef enum{
    UNKNOW,
    UDP_MODE =1,
    CAST_MODE ,
    TCP_MODE ,
    SENDBOX_MODE ,
} NET_MODE;
inline const char *TRANS_MODE_MAP(unsigned int transMode)
{
    switch(transMode)
    {
        case UNKNOW:
            return "UNKNOWN";
        case UDP_MODE:
            return "UDP";
        case CAST_MODE:
            return "MCAST";
        case TCP_MODE:
            return "TCP";
        case SENDBOX_MODE:
            return "SENDBOX";
        default :
            break;
    }

}
#endif

typedef enum{
    NWJK_MODE = 0,
    ZTE_CNC ,   
    NW_GE ,
    ALC_TEL ,
    ALC_CNC ,
    RTP_MODE = 5,
    NAL_CDMA,
    NAL_EVDO,
    NAL_WIFI,
    RTP_NAL,
    RTP_XC = 0x0A,  //�Ų�RTP

} SPLIT_MODE;
inline const char *SPLIT_MODE_MAP(unsigned int splitMode)
{
    switch(splitMode)
    {
        case NWJK_MODE:
            return "KEJUN";
        case ZTE_CNC:
            return "CNC_ZTE";
        case NW_GE:
            return "KJ_GE";
        case ALC_TEL:
            return "ALC_TEL";
        case ALC_CNC:
            return "ALC_CNC";
        case RTP_MODE:
            return "RTP";
        case NAL_CDMA:
            return "NAL_CDMA";
        case NAL_EVDO:
            return "NAL_EVDO";
        case NAL_WIFI:
            return "NAL_WIFI";
        case RTP_XC:
            return "XC_RTP";
        default :
             return "UNKNOWN";
            break;
    }
}

#define NET_SEND_BUF    (64*1024)
#define MULTICAST_TTL   (10)

#define IS_NET_MODE_VALID(NetMode)  (NetMode>=1&&(NET_MODE)NetMode<=CONST_MAX_TRANS_TYPES)
#define IS_SPLIT_MODE_VALID(SplitMode)  (SplitMode>=NWJK_MODE&&SplitMode<=CONST_MAX_SPLIT_TYPES)

#define SESSION_EVENT 1
#define SEND_PACKET 2
#define SEND_EVENT 3
#define SPLIT_EVENT 4

#define ADDEVENT 1
#define DELEVENT 2
#define GETMESSAGE 3
#define SENDSTATE 4
extern "C" int shell_print(const char* format, ...);


#endif

