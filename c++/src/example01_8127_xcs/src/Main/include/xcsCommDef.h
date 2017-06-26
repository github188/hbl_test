#ifndef _XCS_COMM_DEF_H__
#define _XCS_COMM_DEF_H__

#include <string.h>

/* 200万摄像头像素宏定义 */
#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)
#define WIDTH_200W    1600
#define HEIGHT_200W    1200
#elif defined(CYC200KX3)||defined(CYC200XKW)
#define WIDTH_200W    1200
#define HEIGHT_200W    1600
#elif defined(CYC200KW)
#define WIDTH_200W    1920
#define HEIGHT_200W    1072
#elif defined(CYC500XKW4C)||defined(CYC500XAW)
#define WIDTH_500W    2448
#define HEIGHT_500W    2048
#elif defined(CYC500JX)
#define WIDTH_500W    2500
#define HEIGHT_500W    2000
#elif defined(CYC800JX)||defined(CYC800XKW4C)
#define WIDTH_800W    3296
#define HEIGHT_800W    2574
#endif

/*Video  Codec Types, etc.*/
#define VTYPE_UNKNOWN            (0x00)
#define VTYPE_NULL                (0x00)
#define VTYPE_MPEG4                (0x10)
#define VTYPE_H264                (0x40)
#define VTYPE_JPEG                (0x16)
/*obsolete*/
#define VTYPE_MPEG4_2            (0x1F)
#define VTYPE_AVS                (0x50)
#define VTYPE_MJPEG                (0x15)
#define VTYPE_OLDMPEG4            (0x11)
#define VTYPE_SPS                (0x20)

#define   SET_CBR                    (1)
#define   SET_VBR                    (0)

#ifdef __cplusplus
inline const char *VENC_TYPE_STR(unsigned int nVEncType )
{
    switch(nVEncType)
    {
        case VTYPE_MPEG4:
            return "MPEG4";
        case VTYPE_H264:
            return "H264";
        case VTYPE_JPEG:
            return "JPEG";
        default :
            break;
    }

    return "UNKNOWN";
}

inline int VENC_TYPE_NUM(const char * szVEncType )
{
    if(0==strcmp("MPEG4",szVEncType))
    {
        return VTYPE_MPEG4;
    }
    else if(0==strcmp("H264",szVEncType))
    {
        return VTYPE_H264;
    }
    else if(0==strcmp("JPEG",szVEncType))
    {
        return VTYPE_JPEG;
    }
    else if(0==strcmp("UNKNOWN",szVEncType))
    {
        return VTYPE_UNKNOWN;
    }

    return VTYPE_UNKNOWN;
}
#endif

/*Audio Codec Types, etc.*/
#define ATYPE_NULL                (0x60)
#define    ATYPE_UNKNOWN            (0x60)
#define ATYPE_G711A                (0x70)
/*obsolete*/
#define    ATYPE_G711U                (0x71)
#define    ATYPE_G723                (0x72)
#define ATYPE_G722                (0x73)
#define ATYPE_G7221                (0x74)
#ifdef __cplusplus

inline const char * AENC_TYPE_STR( unsigned int nAEncType )
{
    switch(nAEncType)
    {
        case ATYPE_G711A:
            return "G711A";
        default :
            break;
    }

    return "UNKNOWN";
}

inline int AENC_TYPE_NUM(const char * szAEncType)
{
    if(0==strcmp("G711A",szAEncType))
    {
        return ATYPE_G711A;
    }
    else if(0==strcmp("UNKNOWN",szAEncType))
    {
        return ATYPE_UNKNOWN;
    }
    
    return ATYPE_UNKNOWN;
}
#endif
/*Video format,etc.*/
#define VFMT_CIF                (0x02)
#define VFMT_4CIF                (0x05)
#define VFMT_H2CIF                (0x06)
#define VFMT_V2CIF                (0x07)
/*obsolete*/
#define VFMT_QCIF                (0x01)
#define VFMT_D1                    (0x04)
#define VFMT_720P                (0xb)
#define VFMT_1080P                (0xc)
#define VFMT_960P                (0xd)

#ifdef __cplusplus

inline const char *VFMT_TYPE_STR(unsigned int nFmtType )
{
    switch(nFmtType)
    {
        case VFMT_CIF:
            return "CIF";
        case VFMT_4CIF:
            return "4CIF";
        case VFMT_H2CIF:
            return "H2CIF";
        case VFMT_V2CIF:
            return "V2CIF";
        case VFMT_720P:
            return "V720P";
        case VFMT_960P:
            return "V960P";
        case VFMT_1080P:
            return "V1080P";
        default :
            break;
    }

    return "UNKNOWN";
}

inline int VFMT_TYPE_NUM(const char * szFmtType )
{
    if(0==strcmp("CIF",szFmtType))
    {
        return VFMT_CIF;
    }
    else if(0==strcmp("4CIF",szFmtType))
    {
        return VFMT_4CIF;
    }
    else if(0==strcmp("H2CIF",szFmtType))
    {
        return VFMT_H2CIF;
    }
    else if(0==strcmp("V2CIF",szFmtType))
    {
        return VFMT_V2CIF;
    }
    else if (0==strcmp("V720P",szFmtType))
    {
        return VFMT_720P;
    }
    else if (0==strcmp("V960P",szFmtType))
    {
        return VFMT_960P;
    }
    else if (0==strcmp("V1080P",szFmtType))
    {
        return VFMT_1080P;
    }
    else if(0==strcmp("UNKNOWN",szFmtType))
    {
        return VFMT_CIF;
    }
    
    return VFMT_CIF;
    
}
#endif
/*Video Quality,etc.*/
#define VQLT_FIXUP                        (0x00)

/*  
        <提示> 配置前请详细了解用户需求和网络接入情况, 然后在专业人员的指导下完成
      Quality = 0:   固定量化, 帧率确保25帧/秒, 码流起伏很高(↑), 图像质量(*****);
        Quality = 1:   码率优先, 帧率略低25帧/秒, 码流起伏较低(→), 图像质量(**   );
        Quality = 2:   帧率优先, 帧率确保25帧/秒, 码流起伏较高(↗), 图像质量(**** );
        Quality = 3:   码率优先, 帧率20 ~25帧/秒, 码流起伏较低(→), 图像质量(***  ); 
        Quality = 4:   质量优先, 帧率15 ~25帧/秒, 码流起伏较低(→), 图像质量(**** );
        Quality = 5:   质量最好, 帧率 7 ~25帧/秒, 码流起伏较低(→), 图像质量(*****);
        Quality = 256: 固定量化, 帧率减半12帧/秒, 码流起伏很高(↑), 图像质量(*****);
        Quality = 257: 码率优先, 帧率减半12帧/秒, 码流起伏较低(→), 图像质量(***  );
        Quality = 258: 质量优先, 帧率减半12帧/秒, 码流起伏较高(↗), 图像质量(**** );
        <建议> 默认配置2, 普通ADSL场合选择1, 海南ADSL场合选择258
*/

/* Frame Types, etc.*/
#define FRAME_KEY                (0x50)
#define FRAME_NOT_KEY            (0x61)
#define FRAME_START_CODE    (0x4E574A4B) 
#define FRAME_END_CODE      (0x6E776A6B)
#define FRAME_PAYLOAD_MAX_LEN        (640*1024)

typedef struct tagDvsFrame
{
    unsigned int  frame_hd; 
    int  frame_len;
    unsigned char frame_type;
    unsigned char frame_rate;
    unsigned char codec_format;
    unsigned char codec_type;
    unsigned int  time_stamp;
    
    unsigned char data[FRAME_PAYLOAD_MAX_LEN]; /*数据长度不定*/
    unsigned int  frame_tl;
}__attribute__((packed)) XCS_FRAME;

#define FRAME_MAX_LEN        (sizeof(XCS_FRAME))


/*Record Mode,etc.*/
#define REC_FORBID                    (0)
#define REC_ALWAYS                    (1)
#define REC_ALERT                    (2)
#define REC_PLAN                    (3)

#define REC_FORMAT_NULL    (0x00)
#define REC_FORMAT_ASF        (0x01)
#define REC_FORMAT_KSJ        (0x02)    /*zte 宽视界*/

typedef struct
{
    int nRedAlarmId;
    int status;
    int detectType;//Gpio检测还是视频检测
}RedInfo;


#ifdef __cplusplus


inline const char *REC_FORMAT_STR(unsigned int nRecFormat )
{
    switch(nRecFormat)
    {
        case REC_FORMAT_ASF:
            return "ASF";
        case REC_FORMAT_KSJ:
            return "RAW_KSL";
        default :
            break;
    }

    return "UNKNOWN";
}

inline int REC_FORMAT_NUM(const char * szRecFormat )
{
    if(0==strcmp("ASF",szRecFormat))
    {
        return REC_FORMAT_ASF;
    }
    else if(0==strcmp("RAW_KSL",szRecFormat))
    {
        return REC_FORMAT_KSJ;
    }
    else if(0==strcmp("UNKNOWN",szRecFormat))
    {
        return REC_FORMAT_NULL;
    }

    return REC_FORMAT_NULL;
}
#endif
/*Record Storage,Mode,etc.*/
#define STORAGE_FULL_STOP            (1)
#define STORAGE_FULL_OVERWRITE        (2)
#define STORAGE_SEVERAL_DAYS        (3)

#define MAX_TL_NUM 0x20                 /* 最多几个交通灯 */
#define RED_LIGHT_LAST_TIME     (3*60)  /* 红灯持续3分钟后，强制切换为绿灯 */

#define	USE_NEW_OSD_SET			1

#endif /* _XCS_COMM_DEF_H__ */

