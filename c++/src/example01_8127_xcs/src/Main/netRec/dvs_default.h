#ifndef _DVS_DEFAULT_H__
#define _DVS_DEFAULT_H__
#include<netinet/in.h>
#include<string.h>

#ifdef __cplusplus
extern "C"{
#endif

/*Video  Codec Types, etc.*/
#define VTYPE_UNKNOWN           (0x00)
#define VTYPE_NULL              (0x00)
#define VTYPE_MPEG4             (0x10)
#define VTYPE_H264              (0x40)
#define VTYPE_JPEG              (0x16)
/*obsolete*/
#define VTYPE_MPEG4_2           (0x1F)
#define VTYPE_AVS               (0x50)
#define VTYPE_MJPEG             (0x15)
#define VTYPE_OLDMPEG4          (0x11)
#define VTYPE_SPS               (0x20)

#define   SET_CBR                   (1)
#define   SET_VBR                   (0)


/*Audio Codec Types, etc.*/
#define ATYPE_NULL              (0x60)
#define ATYPE_UNKNOWN           (0x60)
#define ATYPE_G711A             (0x70)
/*obsolete*/
#define ATYPE_G711U             (0x71)
#define ATYPE_G723              (0x72)
#define ATYPE_G722              (0x73)
#define ATYPE_G7221             (0x74)
/*Video format,etc.*/
#define VFMT_CIF                (0x02)
#define VFMT_4CIF               (0x05)
#define VFMT_H2CIF              (0x06)
#define VFMT_V2CIF              (0x07)
/*obsolete*/
#define VFMT_QCIF               (0x01)
#define VFMT_D1                 (0x04)

/*Video Quality,etc.*/
#define VQLT_FIXUP              (0x00)

/*
        <��ʾ> ����ǰ����ϸ�˽��û����������������, Ȼ����רҵ��Ա��ָ�������
      Quality = 0:   �̶�����, ֡��ȷ��25֡/��, ��������ܸ�(��), ͼ������(*****);
        Quality = 1:   ��������, ֡���Ե�25֡/��, ��������ϵ�(��), ͼ������(**   );
        Quality = 2:   ֡������, ֡��ȷ��25֡/��, ��������ϸ�(�J), ͼ������(**** );
        Quality = 3:   ��������, ֡��20 ~25֡/��, ��������ϵ�(��), ͼ������(***  );
        Quality = 4:   ��������, ֡��15 ~25֡/��, ��������ϵ�(��), ͼ������(**** );
        Quality = 5:   �������, ֡�� 7 ~25֡/��, ��������ϵ�(��), ͼ������(*****);
        Quality = 256: �̶�����, ֡�ʼ���12֡/��, ��������ܸ�(��), ͼ������(*****);
        Quality = 257: ��������, ֡�ʼ���12֡/��, ��������ϵ�(��), ͼ������(***  );
        Quality = 258: ��������, ֡�ʼ���12֡/��, ��������ϸ�(�J), ͼ������(**** );
        <����> Ĭ������2, ��ͨADSL����ѡ��1, ����ADSL����ѡ��258
*/

/* Frame Types, etc.*/
#define FRAME_KEY                      (0x50)
#define FRAME_NOT_KEY                  (0x61)
#define FRAME_START_CODE_V1            (0x4E574A4B) //NWjk
#define FRAME_END_CODE_V1              (0x6E776A6B) //nwjk
#define FRAME_HEAD_LEN_V1              (16)
#define FRAME_TAIL_LEN_V1              (4)


#define FRAME_START_CODE_V2            (0x6B6A7878) //kyxx
#define FRAME_END_CODE_V2              (0x6279726A) //byrj
#define FRAME_HEAD_LEN_V2              (24)
#define FRAME_TAIL_LEN_V2              (4)


#define FRAME_PAYLOAD_MAX_LEN          (640*1024)



typedef struct tagFramePackageV1
{
    unsigned int  frame_hd;
    unsigned int  frame_len;
    unsigned char frame_type;
    unsigned char frame_rate;
    unsigned char codec_format;
    unsigned char codec_type;
    unsigned int  time_stamp;

    unsigned char data[FRAME_PAYLOAD_MAX_LEN]; /*���ݳ��Ȳ���*/
    unsigned int  frame_tl;
}__attribute__((packed)) FRAME_PACKAGE_V1;

#define FRAME_MAX_LEN_V1       (sizeof(FRAME_PACKAGE_V1))

typedef struct tagFramePackageV2
{
    unsigned int        frame_hd;
    unsigned int        frame_len;
    unsigned char       frame_type;
    unsigned char       frame_rate;
    unsigned char       codec_format;
    unsigned char       codec_type;
    unsigned int        sequence;
    unsigned long long  time_stamp;

    unsigned char       data[FRAME_PAYLOAD_MAX_LEN]; /*���ݳ��Ȳ���*/
    unsigned int        frame_tl;
}__attribute__((packed)) FRAME_PACKAGE_V2;

#define FRAME_MAX_LEN_V2       (sizeof(FRAME_PACKAGE_V2))


typedef struct tagFrameInfo
{
    unsigned int        frame_hd;
    unsigned int        frame_len;
    unsigned char       frame_type;
    unsigned char       frame_rate;
    unsigned char       codec_format;
    unsigned char       codec_type;
    unsigned int        sequence;
    unsigned long long  time_stamp;
    unsigned char       *pdata;
    unsigned int        frame_tl;
    unsigned int        frame_max_len;
    unsigned int        frame_head_len;
    unsigned int        frame_tail_len;
}__attribute__((packed)) FRAME_INFO_S,*LPFRAME_INFO_S;

inline int GetFrameInfo(unsigned char *pFrame, unsigned int nFrameLen, LPFRAME_INFO_S lpFrameInfo)
{
    int ret = 0;

    lpFrameInfo->frame_hd = *(int *)pFrame;

    if(htonl(FRAME_START_CODE_V1) == lpFrameInfo->frame_hd)
    {
        FRAME_PACKAGE_V1 * pDvsFrame= (FRAME_PACKAGE_V1 *)pFrame;
        lpFrameInfo->frame_len       = pDvsFrame->frame_len;
        lpFrameInfo->frame_type      = pDvsFrame->frame_type;
        lpFrameInfo->frame_rate      = pDvsFrame->frame_rate;
        lpFrameInfo->codec_format    = pDvsFrame->codec_format;
        lpFrameInfo->codec_type      = pDvsFrame->codec_type;
        lpFrameInfo->sequence        = 0;
        lpFrameInfo->time_stamp      = pDvsFrame->time_stamp;
        lpFrameInfo->pdata           = pDvsFrame->data;
        lpFrameInfo->frame_tl        = htonl(FRAME_END_CODE_V1);
        lpFrameInfo->frame_max_len   = FRAME_MAX_LEN_V1;
        lpFrameInfo->frame_head_len  = FRAME_HEAD_LEN_V1;
        lpFrameInfo->frame_tail_len  = FRAME_TAIL_LEN_V1;
    }
    else if(htonl(FRAME_START_CODE_V2) == lpFrameInfo->frame_hd)
    {
        FRAME_PACKAGE_V2 * pDvsFrame= (FRAME_PACKAGE_V2 *)pFrame;
        lpFrameInfo->frame_len       = pDvsFrame->frame_len;
        lpFrameInfo->frame_type      = pDvsFrame->frame_type;
        lpFrameInfo->frame_rate      = pDvsFrame->frame_rate;
        lpFrameInfo->codec_format    = pDvsFrame->codec_format;
        lpFrameInfo->codec_type      = pDvsFrame->codec_type;
        lpFrameInfo->sequence        = pDvsFrame->sequence;
        lpFrameInfo->time_stamp      = pDvsFrame->time_stamp;
        lpFrameInfo->pdata           = pDvsFrame->data;
        lpFrameInfo->frame_tl        = htonl(FRAME_END_CODE_V2);
        lpFrameInfo->frame_max_len   = FRAME_MAX_LEN_V2;
        lpFrameInfo->frame_head_len  = FRAME_HEAD_LEN_V2;
        lpFrameInfo->frame_tail_len  = FRAME_TAIL_LEN_V2;
    }
    else
    {
        ret = -1;
    }

    return ret;
}


#ifdef __cplusplus
}
#endif

#endif
