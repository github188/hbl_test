#ifndef _DSP_SERVICE_H_
#define _DSP_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* for time_t*/
#include <time.h>
#include "xcsGlobalDef.h"
#include "dm8127_dsp_service.h"

#define CALL_CYC_CMD(cmd, param) NET_CYC_SetDEVConfig(cmd, (void*)&param, (int)sizeof(param))
//#define CALL_CYC_CMD(cmd, param) NET_CYC_SetDEVConfig_Test(cmd, (void*)&param, (int)sizeof(param))

typedef struct{
	unsigned int  version; /*程序版本信息-- hex*/
	char szCompile[256];/*程序编译信息-- dec */
	char szDesc[128];
}dsp_version_t;

typedef struct{
	unsigned int chan_id;   /* 通道号，从0开始*/
	unsigned int data_type; /* */
	unsigned char *buf;  
	int buf_len; 
} data_param_t;
typedef void (*RCV_DATA_FUNC)(data_param_t *param);

enum en_ALARM_TYPE
{ALARM_VLOSS = 1, ALARM_VMOTION =2, ALARM_CAMEAR_ERROR = 3}; 
 
typedef void (*REPORT_ALARM_FUNC)(alarm_report_t *param);

int dsp_service_hook_alarm(REPORT_ALARM_FUNC func);

int   dsp_service_hook_jpeg_data(RCV_DATA_FUNC  func);

typedef void (*FLOWSTAT_STATUS_FUNC)(flowstat_status_t *param);

//流量统计回调函数接口
int dsp_service_hook_flowstat_control(FLOWSTAT_STATUS_FUNC func);

typedef void (*TRAFFICLIGHT_STATUS_FUNC)(trafficlight_status_t *param);

//交通灯状态回调函数接口
int dsp_service_hook_trafficligth_status(TRAFFICLIGHT_STATUS_FUNC func);
//
//typedef struct
//{
//    float fCurGain; //增益
//    float fCurShutter; //快门
//    float fCurAperture; //光圈
//    float fCurGrey; //灰度
//    float fCurWhitebalance; //白平衡
//    unsigned int uiScale;           //当前scale值
//    unsigned int colorAdjust;       //颜色矫正状态
//    unsigned int luminance;         //亮度值
//    unsigned int sharpness;         //锐度值   
//
//    short sI2CAddr;			//I2C反馈地址
//	short sI2CData;			//I2C反馈数据
//    unsigned int resversed[4];  
//}CUR_CAMERA_STATUS;


//int dsp_camera274_param_get(int chan_id,CUR_CAMERA_STATUS *param);

typedef void (*CAMERA_PARAMA_FUNC )( CUR_CAMERA_STATUS *param);
int dsp_service_hook_camera_param(CAMERA_PARAMA_FUNC func);

#ifdef __cplusplus
}
#endif

#endif  /* _DSP_SERVICE_H_*/

