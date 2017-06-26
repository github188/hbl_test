#ifndef _ICX274CCDCTL_H__
#define _ICX274CCDCTL_H__

#if !defined(CYC500XZD)&&!defined(CYC200XZD)

#include "McLog.h"
#include "dsp_service.h"

extern int g_icx274Ctl_Debug;

#define ICX274CTL_DEBUG(x...)\
do{\
	if(g_icx274Ctl_Debug)\
		shell_print(x);\
}while(0);

//���ڴ�ӡģ�������Ϣ��������ȫ�ֱ���g_Nwdvs_Debug��������
#define ICX274CTL_ERROR(x...)\
do{\
	if(g_icx274Ctl_Debug&0x0001)\
		shell_print(x);\
}while(0);

//���ڴ�ӡģ�龯����Ϣ��������ȫ�ֱ���g_Nwdvs_Debug��������
#define ICX274CTL_WARNING(x...)\
do{\
	if(g_icx274Ctl_Debug&0x0002)\
		shell_print(x);\
}while(0);

//���ڴ�ӡģ����ʾ��Ϣ��������ȫ�ֱ���g_Nwdvs_Debug��������
#define ICX274CTL_INFO(x...)\
do{\
	if(g_icx274Ctl_Debug&0x0004)\
		shell_print(x);\
}while(0);

int icx274CtlDebugSwitch(int level);

int icx274RegCtlInit(void);
int icx274RegCtlUnInit(void);

//typedef struct tag_icx274CCDCamInfo
//{
//	unsigned int feature_shutter_available;				//���ſ��������Ƿ����
//	unsigned int feature_gain_available;				//������������Ƿ����
//	unsigned int feature_bakclight_compensate_available;//���ⲹ�������Ƿ����
//	unsigned int feature_aperture_available; 			//��Ȧ���������Ƿ����
//
//	unsigned int image_width;
//	unsigned int image_height;
//	unsigned int totalpixels;							//������������֧��������--140��200��500��
//
//	//Camera Control Related Param
//	float MinExposureValue; 	//exposure --�� evΪ��λ
//	float MaxExposureValue;
//
//	//Camera Control Related Param
//	float MinShutterValue;		//Shutter --�� sΪ��λ
//	float MaxShutterValue;
//
//	float MinGainValue;			//Gain --��dBΪ��λ
//	float MaxGainValue;
//
//	float MinBrightNessValue;	//BrightNess ---�� % Ϊ��λ
//	float MaxBrightNessValue;
//
//	float MinHueValue;			//Hue ---��deg Ϊ��λ
//	float MaxHueValue; 
//
//	float MinSharpNessValue;	//SharpNess ---�޵�λ
//	float MaxSharpNessValue;
//
//	float MinSaturationValue;	//Saturation ----�� %Ϊ��λ
//	float MaxSaturationValue;
//
//	float MinGammaValue;		//Gamma ----�޵�λ
//	float MaxGammaValue;
//
//	float MinContrastValue;		//�Աȶ� ----�޵�λ
//	float MaxContrastValue;
//
//	float MinIsoValue;			//�Աȶ� ----�޵�λ
//	float MaxIsoValue;
//}cam_info_t;

int icx274CtlDebugSwitch(int level);

int dspCameraInfoShow(void);
float judgeCamCtlVal(unsigned int uiCamCtlType, float fVal);
// int ccdCamInfoGet(cam_info_t *pCamerainfo);


int icx274CamParamSet(int iMode, int iType, float fMinVal, float fMaxVal, float fFixVal);
int icx274CamParamGet(int iType, int* piMode, int* pfFixVal, int* pfMinVal, int* pfMaxVal);

#endif

#endif
