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
//用于打印模块错误信息，可以用全局变量g_Mms_Debug单独控制
#if 1
#define MMSSEND_ERROR(x...)\
do{\
    if(g_Mms_Debug&0x0001)\
        shell_print("MMS mode:"x);\
}while(0);
#else
#define MMSSEND_ERROR(x...)printf(x)
#endif

//用于打印模块警告信息，可以用全局变量g_Mms_Debug单独控制
#if 1
#define MMSSEND_WARNING(x...)\
do{\
    if(g_Mms_Debug&0x0002)\
        shell_print("MMS mode:"x);\
}while(0);
#else
#define MMSSEND_WARNING(x...)   printf(x)
#endif

//用于打印模块提示信息，可以用全局变量g_Mms_Debug单独控制
#if 1
#define MMSSEND_INFO(x...)\
do{\
    if(g_Mms_Debug&0x0004)\
        shell_print("MMS mode:"x);\
}while(0);
#else
#define MMSSEND_INFO(x...) printf(x)
#endif

#define KJ_XC_MANUCODE      (33)//kejun 在信产V3平台上的厂商代号

#define MAX_CHANNEL_NUM (1)

#define MAX_SEND_QUEUE_NUM  (128)
#define MAX_EVENT_QUEUE_NUM (32)

#define NWJK_PACKET_HEAD    (10)
#if defined(RTP_SUPPORT)//新增加的
#define NWJK_PACKET_PAYLOAD (1380)   
#else
#define NWJK_PACKET_PAYLOAD (1012)   
#endif

#define NWJK_PACKET_LEN     (NWJK_PACKET_HEAD+NWJK_PACKET_PAYLOAD)

#define RTP_PACKET_HEAD (20)
#if defined(RTP_SUPPORT)//新增加的
#define RTP_PACKET_PAYLOAD  (1360)   
#else
#define RTP_PACKET_PAYLOAD  (1002)   
#endif
#define RTP_PACKET_LEN      (RTP_PACKET_HEAD+RTP_PACKET_PAYLOAD)

#define CONST_MAX_SPLIT_TYPES   (12+1)
#define CONST_MAX_TRANS_TYPES   (4)

#define MMS_TIME_INTERVAL   (18)

#define RTP_XC_PACKET_HEAD (16+16 + NWJK_PACKET_HEAD)  //头部16字节，扩展头部16字节，

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
#if defined(RTP_SUPPORT)//新增加的
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
    unsigned short tcpMagic;    //字节用于tcp
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
}__attribute__((packed))RtpXCHead; // 共16字节

/*信产RTP扩张字段*/
typedef struct 
{
    unsigned short manuCode; //填unsigned short类型值  制造商代号 主机字节序
    unsigned short nLen;//表示扩展头长度占多少个四字节，填5
    unsigned char PUID[8]; //设备ID，long long型转化成字符串型不够18位时，高位补0
    unsigned char frameType;//帧类型标识
                        //=0x36,标识视频I帧；
                        //=0x37,标识视频P帧；
                        //=0x38,表示视频B帧；
                        //=0x39,标识音频帧；
    unsigned char EndMark;//当前帧结束包标记；0x30，表示该包不是结束包，0x31，表示该包是结束包；
    unsigned char mediatype;//用于区分NAT打洞包
                            //填 0x30，表示是音视频数据；
                            //填 0x31，表示打洞包,媒体流保活包；
                            //填 0x32，表示前端录像回放数据；
                            //填 0x33，表示RTCP包；
    unsigned char channelID;//通道编号；
//  unsigned char CUID[8];
}__attribute__((packed))RtpXCExtern;//共16字节

/*信产RTP媒体包*/
typedef struct 
{
    /*RTP头部及扩展头部*/
    RtpXCHead rtpxcHeadFlag;
    RtpXCExtern rtpxcExternFlag;
    
    /*NWJK头部*/
    unsigned char codec_type;
    unsigned char frame_type;
    unsigned int  seq_num;
    unsigned short data_len;
    unsigned short resesrved;   
    
    unsigned char data[NWJK_PACKET_PAYLOAD];
    
}__attribute__((packed))rtpxcMediaPacket;

/*信产RTP媒体包*/
typedef struct 
{
    /*RTP头部及扩展头部*/
    RtpXCHead rtpxcHeadFlag;
    RtpXCExtern rtpxcExternFlag;

    unsigned char data[NWJK_PACKET_PAYLOAD];
    
}__attribute__((packed))rtpxcAudioPacket;/*信产RTP网络包*/


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
    RTP_XC = 0x0A,  //信产RTP

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

