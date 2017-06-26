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
	unsigned int  version; /*����汾��Ϣ-- hex*/
	char szCompile[256];/*���������Ϣ-- dec */
	char szDesc[128];
}dsp_version_t;

typedef struct{
	unsigned int chan_id;   /* ͨ���ţ���0��ʼ*/
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

//����ͳ�ƻص������ӿ�
int dsp_service_hook_flowstat_control(FLOWSTAT_STATUS_FUNC func);

typedef void (*TRAFFICLIGHT_STATUS_FUNC)(trafficlight_status_t *param);

//��ͨ��״̬�ص������ӿ�
int dsp_service_hook_trafficligth_status(TRAFFICLIGHT_STATUS_FUNC func);
//
//typedef struct
//{
//    float fCurGain; //����
//    float fCurShutter; //����
//    float fCurAperture; //��Ȧ
//    float fCurGrey; //�Ҷ�
//    float fCurWhitebalance; //��ƽ��
//    unsigned int uiScale;           //��ǰscaleֵ
//    unsigned int colorAdjust;       //��ɫ����״̬
//    unsigned int luminance;         //����ֵ
//    unsigned int sharpness;         //���ֵ   
//
//    short sI2CAddr;			//I2C������ַ
//	short sI2CData;			//I2C��������
//    unsigned int resversed[4];  
//}CUR_CAMERA_STATUS;


//int dsp_camera274_param_get(int chan_id,CUR_CAMERA_STATUS *param);

typedef void (*CAMERA_PARAMA_FUNC )( CUR_CAMERA_STATUS *param);
int dsp_service_hook_camera_param(CAMERA_PARAMA_FUNC func);

#ifdef __cplusplus
}
#endif

#endif  /* _DSP_SERVICE_H_*/

