#ifndef  VEHICLE_REPORT_H__
#define  VEHICLE_REPORT_H__

/* 车道车检器信息 */
typedef struct tagPassPortConfInfo
{
    char    cComId;
    char    cBoardId;           /* 板卡号 */
    char    cFrontCoilId;       /* 车检器前线圈 */
    char    cBehindCoilId;      /* 车检器后线圈 */
    char    cModuleId;          /* 模式序号 */
    char	cDecType;			/* 检测类型 */
    unsigned short usCoilDistance;/* 前后线圈距离，单位为厘米 */
    unsigned int uiCarRoadId;   /* 车道号 */
    int		iTotalLane;			/* 总共有几个车道 */
   	unsigned short usSingalCoilHigh; /* 单线圈前线圈高度用于测试车长 */
	char    cRsv[2];
}CARROAD_DECT_CONF;

// HIT vehicle detect device 
int hgdPassPortSnapBegin(unsigned int iComId, int moduleId, unsigned char ucCarDir);
int hgdPassPortSnapEnd(unsigned int iComId, int moduleId, int speed, unsigned char ucCarDir);
int hgdEPSnapCtl(int comid,int nBoardId,int moduleId,int status);

/* 科骏车检器处理函数 */
int kjEPppSnapSeq(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int kjEPSnapSeqQuery(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int kjPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime, unsigned short usGroupId);


/* 苏江车检器处理函数 */
int sjPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);


/* 川速雷达测速处理函数 */
int csPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);
int csMcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* 安道雷达测速处理函数 */
int adPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);


/* 地磁车检器处理函数 */
int dcEPppSnapSeq(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int dcEPSnapSeqQuery(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);
int dcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime);


/* dm368 Spi总线通信处理函数 */
int spiTransInfoQuery(unsigned char *pucData);

/* 慧昌雷达测速处理函数 */
int hcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* 文康雷达测速处理函数 */
int wkPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* 奥利维亚雷达测速处理函数 */
int olviaPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);

/* 超远雷电雷达测速处理函数 */
int cyldPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir);


#endif


