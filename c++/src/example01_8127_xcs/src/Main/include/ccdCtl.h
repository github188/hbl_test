#ifndef _CCD_CTL_H
#define _CCD_CTL_H

#if defined(CYC200XZD)||defined(CYC500XZD)

#include "McLog.h"

#ifdef __cplusplus
extern "C" {
#endif


extern int g_ccd_debug_module;


#define CCDCTL_DEBUG(x...)\
do{\
	if(g_ccd_debug_module)\
		shell_print("ccdCtl mode:"x);\
}while(0)


#define CCDCTL_ERROR(x...)\
do{\
	if(g_ccd_debug_module&0x0001)\
		shell_print("ccdCtl mode:"x);\
}while(0)

#define CCDCTL_WARNING(x...)\
do{\
	if(g_ccd_debug_module&0x0002)\
		shell_print("ccdCtl mode:"x);\
}while(0)

#define CCDCTL_INFO(x...)\
do{\
	if(g_ccd_debug_module&0x0004)\
		shell_print("ccdCtl mode:"x);\
}while(0)


/* Absolute feature */

#define REG_CAMERA_FEATURE_ABS_HI_BASE      0x700U
#define REG_CAMERA_FEATURE_ABS_LO_BASE      0x780U

#define REG_CAMERA_ABS_MIN                  0x000U
#define REG_CAMERA_ABS_MAX                  0x004U
#define REG_CAMERA_ABS_VALUE                0x008U

/* Command registers offsets */

#define REG_CAMERA_INITIALIZE               0x000U

#define REG_CAMERA_V_FORMAT_INQ             0x100U
#define REG_CAMERA_V_MODE_INQ_BASE          0x180U
#define REG_CAMERA_V_RATE_INQ_BASE          0x200U
#define REG_CAMERA_V_REV_INQ_BASE           0x2C0U
#define REG_CAMERA_V_CSR_INQ_BASE           0x2E0U

#define REG_CAMERA_BASIC_FUNC_INQ           0x400U
#define REG_CAMERA_FEATURE_HI_INQ           0x404U
#define REG_CAMERA_FEATURE_LO_INQ           0x408U
#define REG_CAMERA_OPT_FUNC_INQ             0x40CU
#define REG_CAMERA_ADV_FEATURE_INQ          0x480U

#define REG_CAMERA_PIO_CONTROL_CSR_INQ      0x484U
#define REG_CAMERA_SIO_CONTROL_CSR_INQ      0x488U
#define REG_CAMERA_STROBE_CONTROL_CSR_INQ   0x48CU

#define REG_CAMERA_FEATURE_HI_BASE_INQ      0x500U
#define REG_CAMERA_FEATURE_LO_BASE_INQ      0x580U

#define REG_CAMERA_FRAME_RATE               0x600U
#define REG_CAMERA_VIDEO_MODE               0x604U
#define REG_CAMERA_VIDEO_FORMAT             0x608U
#define REG_CAMERA_ISO_DATA                 0x60CU
#define REG_CAMERA_POWER                    0x610U
#define REG_CAMERA_ISO_EN                   0x614U

#define REG_CAMERA_MEMORY_SAVE              0x618U

#define REG_CAMERA_ONE_SHOT                 0x61CU

#define REG_CAMERA_MEM_SAVE_CH              0x620U
#define REG_CAMERA_CUR_MEM_CH               0x624U

#define REG_CAMERA_VMODE_ERROR_STATUS       0x628U

#define REG_CAMERA_SOFT_TRIGGER             0x62CU
#define REG_CAMERA_DATA_DEPTH               0x630U
#define REG_CAMERA_FEATURE_ERR_HI_INQ       0x640h
#define REG_CAMERA_FEATURE_ERR_LO_INQ       0x644h

#define REG_CAMERA_FEATURE_HI_BASE          0x800U
#define REG_CAMERA_FEATURE_LO_BASE          0x880U

#define REG_CAMERA_BRIGHTNESS               0x800U
#define REG_CAMERA_EXPOSURE                 0x804U
#define REG_CAMERA_SHARPNESS                0x808U
#define REG_CAMERA_WHITE_BALANCE            0x80CU
#define REG_CAMERA_HUE                      0x810U
#define REG_CAMERA_SATURATION               0x814U
#define REG_CAMERA_GAMMA                    0x818U
#define REG_CAMERA_SHUTTER                  0x81CU
#define REG_CAMERA_GAIN                     0x820U
#define REG_CAMERA_IRIS                     0x824U
#define REG_CAMERA_FOCUS                    0x828U
#define REG_CAMERA_TEMPERATURE              0x82CU

#define REG_CAMERA_TRIGGER_MODE             0x830U
#define REG_CAMERA_TRIGGER_DELAY            0x834U

#define REG_CAMERA_WHITE_SHADING            0x838U
#define REG_CAMERA_FRAME_RATE_FEATURE       0x83CU
#define REG_CAMERA_ZOOM                     0x880U
#define REG_CAMERA_PAN                      0x884U
#define REG_CAMERA_TILT                     0x888U
#define REG_CAMERA_OPTICAL_FILTER           0x88CU
#define REG_CAMERA_CAPTURE_SIZE             0x8C0U
#define REG_CAMERA_CAPTURE_QUALITY          0x8C4U

#define REG_CAMERA_TEST_PATTERN             0x104CU

#define REG_CAMERA_AUTO_INTER_EXPOSURE      0x1088U
#define REG_CAMERA_AUTO_INTER_SHUTTER       0x1098U
#define REG_CAMERA_AUTO_INTER_GAIN          0x10A0U


/**
 * Enumeration of camera features
 */
typedef enum featureType
{
    DC1394_FEATURE_BRIGHTNESS= 416,
    DC1394_FEATURE_EXPOSURE,
    DC1394_FEATURE_SHARPNESS,
    DC1394_FEATURE_WHITE_BALANCE,
    DC1394_FEATURE_HUE,
    DC1394_FEATURE_SATURATION,
    DC1394_FEATURE_GAMMA,
    DC1394_FEATURE_SHUTTER,
    DC1394_FEATURE_GAIN,
    DC1394_FEATURE_IRIS,
    DC1394_FEATURE_FOCUS,
    DC1394_FEATURE_TEMPERATURE,
    DC1394_FEATURE_TRIGGER,
    DC1394_FEATURE_TRIGGER_DELAY,
    DC1394_FEATURE_WHITE_SHADING,
    DC1394_FEATURE_FRAME_RATE,
    DC1394_FEATURE_ZOOM,
    DC1394_FEATURE_PAN,
    DC1394_FEATURE_TILT,
    DC1394_FEATURE_OPTICAL_FILTER,
    DC1394_FEATURE_CAPTURE_SIZE,
    DC1394_FEATURE_CAPTURE_QUALITY
} dc1394feature_str;

typedef int dc1394feature_t;

typedef struct
{
	unsigned int feature_shutter_available;				//快门控制特性是否可用
	unsigned int feature_gain_available;				//增益控制特性是否可用
	unsigned int feature_bakclight_compensate_available;//背光补偿特性是否可用
	unsigned int feature_aperture_available; 			//光圈控制特性是否可用
	unsigned int image_width;
	unsigned int image_height;
	unsigned int totalpixels;							//该摄像机得最大支持像素数--140万、200万、500万

	//Camera Control Related Param
	float MinExposureValue; 	//exposure --以 ev为单位
	float MaxExposureValue;

	//Camera Control Related Param
	float MinShutterValue;		//Shutter --以 s为单位
	float MaxShutterValue;

	float MinGainValue;			//Gain --以dB为单位
	float MaxGainValue;

	float MinBrightNessValue;	//BrightNess ---以 % 为单位
	float MaxBrightNessValue;

	float MinHueValue;			//Hue ---以deg 为单位
	float MaxHueValue; 

	float MinSharpNessValue;	//SharpNess ---无单位
	float MaxSharpNessValue;

	float MinSaturationValue;	//Saturation ----以 %为单位
	float MaxSaturationValue;

	float MinGammaValue;		//Gamma ----无单位
	float MaxGammaValue;

	float MinContrastValue;		//
	float MaxContrastValue;

	float MinIsoValue;			//
	float MaxIsoValue;

	//-----当前采集使用的Buf数目
	int   CapBufCnt;
}cam_info_t;

/**
 * Control modes for a feature (excl. absolute control)
 */
typedef enum featureMode
{
    DC1394_FEATURE_MODE_MANUAL= 736,
    DC1394_FEATURE_MODE_AUTO,
    DC1394_FEATURE_MODE_ONE_PUSH_AUTO
} dc1394feature_mode_str;

typedef int dc1394feature_mode_t;


#define DC1394_FEATURE_MODE_MIN      DC1394_FEATURE_MODE_MANUAL
#define DC1394_FEATURE_MODE_MAX      DC1394_FEATURE_MODE_ONE_PUSH_AUTO
#define DC1394_FEATURE_MODE_NUM     (DC1394_FEATURE_MODE_MAX - DC1394_FEATURE_MODE_MIN + 1)

#pragma pack(4)
typedef struct camParamstr
{
	unsigned long offset_header;    //偏移量
	unsigned long value;            //值
}strCamParam;
#pragma pack()

int ccdCtlDebugSwitch(int level);

int ccdRegCtlInit(void);
int ccdRegCtlUninit(void);

int ccdCtlRegValSet(int iAdd, unsigned int uiVal);
int ccdCtlRegValGet(int iAddr, unsigned int *puiVal);

int ccdCSRValGet(int iAdd, unsigned int *puiVal);
int ccdCSRValSet(int iAdd, unsigned int uiVal);

int dc1394_feature_absolute_mode_conf(dc1394feature_t feature, int iSta);
int dc1394_feature_set_absolute_value(dc1394feature_t feature, float value);
int dc1394_feature_get_absolute_value(dc1394feature_t feature, float *value);
int dc1394_feature_set_mode(dc1394feature_t feature, dc1394feature_mode_t mode);
int dc1394_set_interval_off(int iRegOff);
int dc1394_set_interval_on(int iRegOff, unsigned int uiVal);
int dc1394_feature_set_absolute_value(dc1394feature_t feature, float value);
int dc1394_exposure_set_interval(float MaxValue , float MinValue);
int dc1394_shutter_set_interval(float MaxValue , float MinValue);
int dc1394_gain_set_interval(float MaxValue , float MinValue);
int ccdCamInfoGet(cam_info_t *pCamerainfo);
float judgeCamCtlVal(unsigned int uiCamCtlType, float fVal);
int dspCameraInfoShow(void);

int dc1394_frameRate_set(float fValue);
int dc1394_frameRate_get(float *pfValue);

int dc1394_Hue_set(float fValue);
int dc1394_Hue_get(float *fValue);

int dc1394_Saturation_set(float fValue);
int dc1394_Saturation_get(float *fValue);

int dc1394_Gamma_set(float fValue);
int dc1394_Gamma_get(float *fValue);

int dc1394_Brightness_set(float fValue);
int dc1394_Brightness_get(float *fValue);

#ifdef __cplusplus
}
#endif

#endif

#endif /* _CCD_CTL_H */
