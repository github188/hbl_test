/******************************************************************************
 * DataUpLoad.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.25, xcsworker Create
 * --------------------
 ******************************************************************************/
#ifndef __DATAUPLOAD_H
#define  __DATAUPLOAD_H

/*上报数据 参数头*/
typedef struct _tagInformParam
{
    unsigned int    uiVersion;
    unsigned int    uiType;
    time_t   tTime;
}_PACKED_ INFORM_PARAM;

/*上报数据内容*/
typedef struct _tagInfromContent
{
    INFORM_PARAM  strInformParam;
    char *pInformContent;
}_PACKED_ INFORM_CONTENT;

/* 上报数据抓拍参数*/
typedef struct _tagSnapInfrom
{
    unsigned int    uiParamLen;       /*   参数长度，以字节为单位, */
    char   *pucParamData;             /*   参数数据 */
    unsigned char   ucPicNum;         /*    图片数目*/
    unsigned char   resv1[3];          /*   保留*/
    unsigned int    *auiPicLen;       /*    图片长度*/
    char   *pszPicData;               /*    图片内容*/
}_PACKED_ SNAP_INFORM;


#define   E_MANUAL_SNAP         (0xA001)
#define   E_COMIN                (0xB001)
#define   E_COMOUT               (0xB002)
#define   E_PASSPORT_SNAP       (0xD001)
#define   E_ELECPOLICE_SNAP     (0xD002)
typedef struct
{
    unsigned short usAreaPosX;      /* 区域左上角坐标X */
    unsigned short usAreaPosY;      /* 区域左上角坐标Y */
    unsigned short usAreaWidth;     /* 区域宽度 */
    unsigned short usAreaHeight;    /* 区域高度 */
}_PACKED_ STR_LICENCE_AREA, *PSTR_LICENCE_AREA;

/* 卡口抓拍参数*/
typedef struct _tagPassPortSnapParam
{
    unsigned int    uiSubVersion;        /* 版本号*/
    unsigned char   ucCarChannel;        /* 车道号*/
    unsigned char   ucType;               /* 车辆类型*/
    unsigned char   resv1;                /* 保留*/
    unsigned char   uidirection;         /* 方向*/
    unsigned short  usCarLen;            /* 车长*/
    unsigned short  usSpeed;             /*  车速 */
    unsigned char   ucInfringeSta;      /*  是否违章*/
    unsigned char   ucInfringeType;     /*  违章类型*/
    STR_LICENCE_AREA  strLienceArea;    /* 车牌区域*/
    unsigned char   ucConfident;        /*  置信度*/
    unsigned char   resv2;               /* 保留*/
}_PACKED_ PASSPORT_SNAP_PARAM;

/* 电子警察抓拍参数*/
typedef struct _tagElecPoliceSnapParam
{
    unsigned int    uiSubVersion;       /*  版本号*/
    unsigned char   ucCarChannel;       /*  车道号*/
    unsigned char   Resv1[2];            /* 保留*/
    unsigned char   ucConfident;        /*  置信度*/
    STR_LICENCE_AREA    strLienceArea;  /* 车牌区域*/
}_PACKED_ ELECPOLICE_SNAP_PARAM;

/* 串口数据上报*/
typedef struct _tagComData
{
    unsigned int    uiSubVersion;
    unsigned int    uiDataLen;
    char   *pucData;
}_PACKED_ COM_INFORM;


class CTcpSend;

class   CDataUpLoad
{
public:
        CDataUpLoad();
        ~CDataUpLoad();
        bool    UpLoadInit();
        bool    AddServer(unsigned int uiIpAddr,unsigned short usPort);
        bool    DelServer(unsigned int uiIpAddr,unsigned short usPort);
        bool    UpLoadData(INFORM_CONTENT *pInform );
        bool    ShutDown(unsigned int IpAddr,unsigned short Port);
        bool    UpLoadUninit();
private:
        CTcpSend    *m_cTcpSendHandle;
};
#endif

