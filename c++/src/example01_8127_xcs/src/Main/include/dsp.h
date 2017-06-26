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
#define OUTPUT_CHANNEL_NUM          (MAX_CHANNEL_NUM)//必须大于等于MAX_CHANNEL_NUM
#define INPUT_CHANNEL_NUM           (MAX_CHANNEL_NUM)//必须小于等于TOTAL_CHANNEL_NUM


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

/* 卡口和电警字幕叠加项宏定义 */
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
/*设置通道名字显示*/
int dspSetNamePos(int chanid,const NameParam* pParam);

/*设置卡口抓拍名称*/
int dspSetSnapOsd(int chanid,int vehicleId,const NameParam *pParam);

/*设置电警抓拍名称*/
int dspEpSetSnapOsd(int chanid,int vehicleId,const NameParam *pParam);

/* 设置dsp jpg图片叠加字幕 */
int dspSetJpgOsdString(int chanid,int vehicleId, char cType, const NameParam *pParam);


/*设置通道编码状态*/
int dspEncStaCtl(int chanid,int status);



typedef struct
{
	int nShowArea;
	int startX;
	int startY;
	int width;
	int height;
}VLParam;
/* 设置虚拟线圈参数*/
int dspSetVLParam(int chanid,int iLaneId, int iShowFlag, VLParam *pParam);
//
//typedef enum
//{
//    CONTINUE=0,
//    SINGLE
//}HDMode;
///*设置高清摄像机工作模式*/
//int dspSetHDMode(int cameraid,HDMode mode);

/* 红灯产生时间通知 */
int dspRedLightTimeSet(int chanid,int status);
/* 红灯状态通知DSP */
int dspSetRedInfo(int lane);

/* 定时录像回调函数 */
//void TimerRecFileSend(char *pcFileName);
/* 报警录像回调函数 */
void alarmRecFileSend(char *pcLaneId);


	
/*初始化*/
int dspInit();

/*注销*/
int dspUnInit();

int DspAppDebugSwitch(int nLevel);

/* 打印版本 */
int dspPrintVer();

////获取service_lib和firmware以及firmware中库版本信息。
//int dsp_service_get_version();

int getCameraInfo(int *pWidth,int * pHeight,int * pPixels);

// set the area of the snap
int dspSetSnapArea(int vid,int startX,int startY,int width,int height, int iShowFlag);


#ifdef __cplusplus
}
#endif

#endif

