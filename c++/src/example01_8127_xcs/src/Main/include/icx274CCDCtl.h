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

//用于打印模块错误信息，可以用全局变量g_Nwdvs_Debug单独控制
#define ICX274CTL_ERROR(x...)\
do{\
	if(g_icx274Ctl_Debug&0x0001)\
		shell_print(x);\
}while(0);

//用于打印模块警告信息，可以用全局变量g_Nwdvs_Debug单独控制
#define ICX274CTL_WARNING(x...)\
do{\
	if(g_icx274Ctl_Debug&0x0002)\
		shell_print(x);\
}while(0);

//用于打印模块提示信息，可以用全局变量g_Nwdvs_Debug单独控制
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
//	unsigned int feature_shutter_available;				//快门控制特性是否可用
//	unsigned int feature_gain_available;				//增益控制特性是否可用
//	unsigned int feature_bakclight_compensate_available;//背光补偿特性是否可用
//	unsigned int feature_aperture_available; 			//光圈控制特性是否可用
//
//	unsigned int image_width;
//	unsigned int image_height;
//	unsigned int totalpixels;							//该摄像机得最大支持像素数--140万、200万、500万
//
//	//Camera Control Related Param
//	float MinExposureValue; 	//exposure --以 ev为单位
//	float MaxExposureValue;
//
//	//Camera Control Related Param
//	float MinShutterValue;		//Shutter --以 s为单位
//	float MaxShutterValue;
//
//	float MinGainValue;			//Gain --以dB为单位
//	float MaxGainValue;
//
//	float MinBrightNessValue;	//BrightNess ---以 % 为单位
//	float MaxBrightNessValue;
//
//	float MinHueValue;			//Hue ---以deg 为单位
//	float MaxHueValue; 
//
//	float MinSharpNessValue;	//SharpNess ---无单位
//	float MaxSharpNessValue;
//
//	float MinSaturationValue;	//Saturation ----以 %为单位
//	float MaxSaturationValue;
//
//	float MinGammaValue;		//Gamma ----无单位
//	float MaxGammaValue;
//
//	float MinContrastValue;		//对比度 ----无单位
//	float MaxContrastValue;
//
//	float MinIsoValue;			//对比度 ----无单位
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
