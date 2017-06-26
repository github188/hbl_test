#ifndef __DSP_H__
#define __DSP_H__

#include <time.h>

#include "xcsGlobalDef.h"
#include "MmsSendDefine.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_BUFFERD_FRAME_NUM               (50)
//#define TOTAL_CHANNEL_NUM           (MAX_CHANNEL_NUM*2)
#define OUTPUT_CHANNEL_NUM          (MAX_CHANNEL_NUM)//������ڵ���MAX_CHANNEL_NUM
#define INPUT_CHANNEL_NUM           (MAX_CHANNEL_NUM)//����С�ڵ���TOTAL_CHANNEL_NUM


#define DEFAULT   1
#define INTERVAL  2
#define AUTO      3
#define FIX		  4
#define DISABEL	  5

#define IRR_TYPE_OVER_HALF_SPEED         (1603)
#define IRR_TYPE_REVERSE_DIR             (1301)
#define IRR_TYPE_OVER_SPEED              (1303)


#define IRR_TYPE_RIDE_LINES              (14021)

#define IRR_TYPE_TRUCK_FORBID            (13442)

/* ���ں͵羯��Ļ������궨�� */
#define PP_OSD_ROAD_NAME_SET	1
#define PP_OSD_ROAD_DIR_SET		2
#define PP_OSD_ROAD_DESP_SET	3
#define PP_OSD_ROAD_LANE_SET	4

#define EP_OSD_ROAD_NAME_SET	5
#define EP_OSD_ROAD_DIR_SET		6
#define EP_OSD_ROAD_DESP_SET	7
#define EP_OSD_ROAD_LANE_SET	8


typedef struct tagVideoRecSta
{
    int iVideoEnable;
    int iVideoSum[TOTAL_CHANNEL_NUM];
    int iRecEnable;
    int iRecSum[TOTAL_CHANNEL_NUM];
}STR_VIDEO_RECSTA;

typedef struct 
{
	int encType;
	int imgFmt;
	int keyIntv;
	int bitRate;/*kbps*/
	int frameRate;
	int nEncQuality;
	int nMotionDetect;
	int is_cbr;
}EncParam;

int dspSetEncParam(int chanid,EncParam* pParam);

typedef struct
{
	int startX;
	int startY;
	int width;
	int height;
	char name[128];
}NameParam; 
/*����ͨ��������ʾ*/
int dspSetNamePos(int chanid,const NameParam* pParam);

/*���ÿ���ץ������*/
int dspSetSnapOsd(int chanid,int vehicleId,const NameParam *pParam);

/*���õ羯ץ������*/
int dspEpSetSnapOsd(int chanid,int vehicleId,const NameParam *pParam);

/* ����dsp jpgͼƬ������Ļ */
int dspSetJpgOsdString(int chanid,int vehicleId, char cType, const NameParam *pParam);


/*����ͨ������״̬*/
int dspEncStaCtl(int chanid,int status);



typedef struct
{
	int nShowArea;
	int startX;
	int startY;
	int width;
	int height;
}VLParam;
/* ����������Ȧ����*/
int dspSetVLParam(int chanid,int iLaneId, int iShowFlag, VLParam *pParam);
//
//typedef enum
//{
//    CONTINUE=0,
//    SINGLE
//}HDMode;
///*���ø������������ģʽ*/
//int dspSetHDMode(int cameraid,HDMode mode);

/* ��Ʋ���ʱ��֪ͨ */
int dspRedLightTimeSet(int chanid,int status);
/* ���״̬֪ͨDSP */
int dspSetRedInfo(int lane);

/* ��ʱ¼��ص����� */
//void TimerRecFileSend(char *pcFileName);
/* ����¼��ص����� */
void alarmRecFileSend(char *pcLaneId);


	
/*��ʼ��*/
int dspInit();

/*ע��*/
int dspUnInit();

int DspAppDebugSwitch(int nLevel);

/* ��ӡ�汾 */
int dspPrintVer();

////��ȡservice_lib��firmware�Լ�firmware�п�汾��Ϣ��
//int dsp_service_get_version();

int getCameraInfo(int *pWidth,int * pHeight,int * pPixels);

// set the area of the snap
int dspSetSnapArea(int vid,int startX,int startY,int width,int height, int iShowFlag);


#ifdef __cplusplus
}
#endif

#endif

