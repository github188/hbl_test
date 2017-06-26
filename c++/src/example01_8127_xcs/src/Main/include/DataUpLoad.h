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

/*�ϱ����� ����ͷ*/
typedef struct _tagInformParam
{
    unsigned int    uiVersion;
    unsigned int    uiType;
    time_t   tTime;
}_PACKED_ INFORM_PARAM;

/*�ϱ���������*/
typedef struct _tagInfromContent
{
    INFORM_PARAM  strInformParam;
    char *pInformContent;
}_PACKED_ INFORM_CONTENT;

/* �ϱ�����ץ�Ĳ���*/
typedef struct _tagSnapInfrom
{
    unsigned int    uiParamLen;       /*   �������ȣ����ֽ�Ϊ��λ, */
    char   *pucParamData;             /*   �������� */
    unsigned char   ucPicNum;         /*    ͼƬ��Ŀ*/
    unsigned char   resv1[3];          /*   ����*/
    unsigned int    *auiPicLen;       /*    ͼƬ����*/
    char   *pszPicData;               /*    ͼƬ����*/
}_PACKED_ SNAP_INFORM;


#define   E_MANUAL_SNAP         (0xA001)
#define   E_COMIN                (0xB001)
#define   E_COMOUT               (0xB002)
#define   E_PASSPORT_SNAP       (0xD001)
#define   E_ELECPOLICE_SNAP     (0xD002)
typedef struct
{
    unsigned short usAreaPosX;      /* �������Ͻ�����X */
    unsigned short usAreaPosY;      /* �������Ͻ�����Y */
    unsigned short usAreaWidth;     /* ������ */
    unsigned short usAreaHeight;    /* ����߶� */
}_PACKED_ STR_LICENCE_AREA, *PSTR_LICENCE_AREA;

/* ����ץ�Ĳ���*/
typedef struct _tagPassPortSnapParam
{
    unsigned int    uiSubVersion;        /* �汾��*/
    unsigned char   ucCarChannel;        /* ������*/
    unsigned char   ucType;               /* ��������*/
    unsigned char   resv1;                /* ����*/
    unsigned char   uidirection;         /* ����*/
    unsigned short  usCarLen;            /* ����*/
    unsigned short  usSpeed;             /*  ���� */
    unsigned char   ucInfringeSta;      /*  �Ƿ�Υ��*/
    unsigned char   ucInfringeType;     /*  Υ������*/
    STR_LICENCE_AREA  strLienceArea;    /* ��������*/
    unsigned char   ucConfident;        /*  ���Ŷ�*/
    unsigned char   resv2;               /* ����*/
}_PACKED_ PASSPORT_SNAP_PARAM;

/* ���Ӿ���ץ�Ĳ���*/
typedef struct _tagElecPoliceSnapParam
{
    unsigned int    uiSubVersion;       /*  �汾��*/
    unsigned char   ucCarChannel;       /*  ������*/
    unsigned char   Resv1[2];            /* ����*/
    unsigned char   ucConfident;        /*  ���Ŷ�*/
    STR_LICENCE_AREA    strLienceArea;  /* ��������*/
}_PACKED_ ELECPOLICE_SNAP_PARAM;

/* ���������ϱ�*/
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

