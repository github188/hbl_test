#ifndef  VEHICLE_REPORT_H__
#define  VEHICLE_REPORT_H__

/* ������������Ϣ */
typedef struct tagPassPortConfInfo
{
    char    cComId;
    char    cBoardId;           /* �忨�� */
    char    cFrontCoilId;       /* ������ǰ��Ȧ */
    char    cBehindCoilId;      /* ����������Ȧ */
    char    cModuleId;          /* ģʽ��� */
    char	cDecType;			/* ������� */
    unsigned short usCoilDistance;/* ǰ����Ȧ���룬��λΪ���� */
    unsigned int uiCarRoadId;   /* ������ */
    int		iTotalLane;			/* �ܹ��м������� */
   	unsigned short usSingalCoilHigh; /* ����Ȧǰ��Ȧ�߶����ڲ��Գ��� */
	char    cRsv[2];
}CARROAD_DECT_CONF;

// HIT vehicle detect device 
int hgdPassPortSnapBegin(unsigned int iComId, int moduleId, unsigned char ucCarDir);
int hgdPassPortSnapEnd(unsigned int iComId, int moduleId, int speed, unsigned char ucCarDir);
int hgdEPSnapCtl(int comid,int nBoardId,int moduleId,int status);

/* �ƿ������������� */
int kjEPppSnapSeq(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int kjEPSnapSeqQuery(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int kjPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime, unsigned short usGroupId);


/* �ս������������� */
int sjPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);


/* �����״���ٴ����� */
int csPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);
int csMcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* �����״���ٴ����� */
int adPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);


/* �شų����������� */
int dcEPppSnapSeq(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int dcEPSnapSeqQuery(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int dcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);


/* dm368 Spi����ͨ�Ŵ����� */
int spiTransInfoQuery(unsigned char *pucData);

/* �۲��״���ٴ����� */
int hcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* �Ŀ��״���ٴ����� */
int wkPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* ����ά���״���ٴ����� */
int olviaPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* ��Զ�׵��״���ٴ����� */
int cyldPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);


#endif


