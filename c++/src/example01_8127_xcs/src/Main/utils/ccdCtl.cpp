/******************************************************************************
 * ccdCtl.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.05.05, wb Create
 * --------------------
 ******************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>

#include "ccdCtl.h"
#include "xcsGlobalDef.h"
#include "SysHandle.h"

#include "zebraCtl.h"

#if defined(CYC200XZD)||defined(CYC500XZD)

#define VPFE_CMD_START          (1)
#define VCAP_CMD_GET_REG        (5)
#define VCAP_CMD_SET_REG        (6)
#define VCAP_CMD_CAM_INFO_GET   (7)

#define DEV_VCAP        "/dev/misc/vpif"

extern char g_cCCDCtlSta;
extern cam_info_t HDConf;

#pragma pack(4)
typedef struct
{
	unsigned long offset_header;    //偏移量
	unsigned long value;            //值
}camera_param_t;
#pragma pack()


#define DC1394_FEATURE_MIN           DC1394_FEATURE_BRIGHTNESS
#define DC1394_FEATURE_MAX           DC1394_FEATURE_CAPTURE_QUALITY
#define DC1394_FEATURE_NUM          (DC1394_FEATURE_MAX - DC1394_FEATURE_MIN + 1)

#define FEATURE_TO_ABS_VALUE_OFFSET(feature, offset)                  \
    {                                                                 \
    if ( (feature > DC1394_FEATURE_MAX) || (feature < DC1394_FEATURE_MIN) )  \
    {                                                                 \
        return -1;                                        \
    }                                                                 \
    else if (feature < DC1394_FEATURE_ZOOM)                           \
    {                                                                 \
        offset= REG_CAMERA_FEATURE_ABS_HI_BASE;                       \
        feature-= DC1394_FEATURE_MIN;                                 \
    }                                                                 \
    else                                                              \
    {                                                                 \
        offset= REG_CAMERA_FEATURE_ABS_LO_BASE;                       \
        feature-= DC1394_FEATURE_ZOOM;                                \
                                                                      \
        if (feature >= DC1394_FEATURE_CAPTURE_SIZE)                   \
        {                                                             \
            feature+= 12;                                             \
        }                                                             \
                                                                      \
    }                                                                 \
                                                                      \
    offset+= feature * 0x04U;                                         \
    }


#define FEATURE_TO_VALUE_OFFSET(feature, offset)                                 \
    {                                                                            \
    if ( (feature > DC1394_FEATURE_MAX) || (feature < DC1394_FEATURE_MIN) )      \
      return -1;                                                     \
    else if (feature < DC1394_FEATURE_ZOOM)                                      \
      offset= REG_CAMERA_FEATURE_HI_BASE+(feature - DC1394_FEATURE_MIN)*0x04U;   \
    else if (feature >= DC1394_FEATURE_CAPTURE_SIZE)                             \
      offset= REG_CAMERA_FEATURE_LO_BASE +(feature+12-DC1394_FEATURE_ZOOM)*0x04U;\
    else                                                                         \
      offset= REG_CAMERA_FEATURE_LO_BASE +(feature-DC1394_FEATURE_ZOOM)*0x04U;   \
    }

static int iCcdCtlFd =-1;
static int s_iSta = -1;

int g_ccd_debug_module = 0xffff;

int ccdCtlDebugSwitch(int level)
{ 
	g_ccd_debug_module = level;
	return 0;
}

int ccdRegCtlInit(void)
{
	int ret = -1;

    iCcdCtlFd = open(DEV_VCAP, O_RDWR);
	if (iCcdCtlFd < 0)
	{
	    CCDCTL_ERROR("vpif dev open failed!\n");
	    s_iSta = -1;
		return s_iSta;
	}
	
	#if defined(CYC200XZD)
    ret = ioctl(iCcdCtlFd, VPFE_CMD_START, 1);
    #else
    ret = ioctl(iCcdCtlFd, VPFE_CMD_START, 0);
    #endif
    
	if (ret < 0)
	{
	    CCDCTL_ERROR("vpif1394 driver start failed!\n");
        s_iSta = -2;
		return s_iSta;
	}
	else
	{
	    CCDCTL_INFO("vpif1394 driver start ok!\n");
	}
	s_iSta =0;
	return 0;
}

int ccdRegCtlUninit(void)
{
    int ret = -1;
    ret = close(iCcdCtlFd);
    return ret;
}

int ccdCtlRegValSet(int iAdd, unsigned int uiVal)
{
#if defined(CYC200XZD)||defined(CYC500XZD)
    zebraCtlRegValSet(iAdd, uiVal);
    return 0;
#else
    int ret =-1;
    strCamParam param;
    memset(&param, 0, sizeof(param));

    if(s_iSta != 0)
    {
        return s_iSta;
    }
        
    if(-1 == iCcdCtlFd)
    {
        CCDCTL_ERROR("fd error\n");
        return -1;
    }

    param.offset_header = iAdd;
    param.value = uiVal;
    CCDCTL_INFO("ccdRegCtl set addr:0x%x,val:0x%x\n", param.offset_header, param.value);
    ret = ioctl(iCcdCtlFd, VCAP_CMD_SET_REG, &param);
	if (ret < 0)
	{
	    CCDCTL_ERROR("vpif1394 driver set reg failed!\n");
	    return -1;
	}
	else
	{
	    //CCDCTL_INFO("vpif1394 driver set reg ok!\n");
	}
    return 0;
#endif
}

int ccdCtlRegValGet(int iAddr, unsigned int *puiVal)
{
#if defined(CYC200XZD)||defined(CYC500XZD)
	int ret =-1;
    ret = zebraCtlRegValGet(iAddr, puiVal);
    return ret;
#else
    int ret =-1;
    strCamParam param;
    memset(&param, 0, sizeof(param));

    if(s_iSta != 0)
    {
        return s_iSta;
    }
    
    if( -1 == iCcdCtlFd )
    {
        CCDCTL_ERROR("param error\n");
        return -1;
    }
    param.offset_header = iAddr;
    ret = ioctl(iCcdCtlFd, VCAP_CMD_GET_REG, &param);
	if (ret < 0)
	{
	    CCDCTL_ERROR("vpif1394 driver get reg failed!\n");
	    return -1;
	}
	else
	{
	    *puiVal = param.value;
	    CCDCTL_INFO("ccdRegCtl get ok. addr:0x%x,val:0x%x\n", param.offset_header, param.value);
	}
    return 0;
#endif
}


int ccdCamInfoGet(cam_info_t *pCamerainfo)
{
    int ret =-1;

    if(s_iSta != 0)
    {
        return s_iSta;
    }
    
    if( -1 == iCcdCtlFd )
    {
        CCDCTL_ERROR("param error\n");
        return -1;
    }
#if defined(CYC200XZD)||defined(CYC500XZD)

    #if defined(CYC200XZD)
    pCamerainfo->image_width  = 1600;
    pCamerainfo->image_height = 1200;
    #elif defined(CYC500XZD)
    pCamerainfo->image_width  = 2592;
    pCamerainfo->image_height = 1932;
    #endif

    pCamerainfo->totalpixels  = pCamerainfo->image_width*pCamerainfo->image_height;
    
    pCamerainfo->MaxExposureValue = 2.1;
    pCamerainfo->MinExposureValue = -7.58;
    
    pCamerainfo->MaxShutterValue = 0.063303;
    pCamerainfo->MinShutterValue = 0.000008;

    pCamerainfo->MaxGainValue = 24;
    pCamerainfo->MinGainValue = -1.9;

    pCamerainfo->MaxBrightNessValue = 6.2;
    pCamerainfo->MinBrightNessValue = 0;
    
    pCamerainfo->MaxSharpNessValue = 4095;
    pCamerainfo->MinSharpNessValue = 0;

    pCamerainfo->MaxGammaValue = 3.9;
    pCamerainfo->MinGammaValue = 0.5;

    pCamerainfo->MinSaturationValue =0;
    pCamerainfo->MaxSaturationValue =400;


#else
    ret = ioctl(iCcdCtlFd, VCAP_CMD_CAM_INFO_GET, pCamerainfo);
	if (ret < 0)
	{
	    CCDCTL_ERROR("vpif1394 driver get cam info failed!\n");
	    return -1;
	}
#endif
    return 0;
}

float judgeCamCtlVal(unsigned int uiCamCtlType, float fVal)
{
    if(g_cCCDCtlSta != 1)
        return fVal;
    switch (uiCamCtlType)
    {
        case ESHUTTER:
            if( (fVal >= HDConf.MinShutterValue) && (fVal <= HDConf.MaxShutterValue) )
            {
                return fVal;
            }
            else if (fVal < HDConf.MinShutterValue)
            {
                return HDConf.MinShutterValue;
            }
            else
            {
                return HDConf.MaxShutterValue;
            }
        break;
        case EGAIN:
            if( (fVal >= HDConf.MinGainValue) && (fVal <= HDConf.MaxGainValue) )
            {
                return fVal;
            }
            else if ( fVal < HDConf.MinGainValue )
            {
                return HDConf.MinGainValue;
            }
            else
            {
                return HDConf.MaxGainValue;
            }
        break;

        case EEXPOSURE:
            if( (fVal >= HDConf.MinExposureValue) && (fVal <= HDConf.MaxExposureValue) )
            {
                return fVal;
            }
            else if ( fVal < HDConf.MinExposureValue )
            {
                return HDConf.MinExposureValue;
            }
            else
            {
                return HDConf.MaxExposureValue;
            }
        break;
        case EBRIGHTNESS:
            if( (fVal >= HDConf.MinBrightNessValue) && (fVal <= HDConf.MaxBrightNessValue) )
            {
                return fVal;
            }
            else if ( fVal < HDConf.MinBrightNessValue )
            {
                return HDConf.MinBrightNessValue;
            }
            else
            {
                return HDConf.MaxBrightNessValue;
            }
        break;
        case EGAMMA:
            if( (fVal >= HDConf.MinGammaValue) && (fVal <= HDConf.MaxGammaValue) )
            {
                return fVal;
            }
            else if ( fVal < HDConf.MinGammaValue )
            {
                return HDConf.MinGammaValue;
            }
            else
            {
                return HDConf.MaxGammaValue;
            }
        break;
        case ESHARPNESS:
            if( (fVal >= HDConf.MinSharpNessValue) && (fVal <= HDConf.MaxSharpNessValue) )
            {
                return fVal;
            }
            else if ( fVal < HDConf.MinSharpNessValue )
            {
                return HDConf.MinSharpNessValue;
            }
            else
            {
                return HDConf.MaxSharpNessValue;
            }
        break;
        case ESATURATION:
            if( (fVal >= HDConf.MinSaturationValue) && (fVal <= HDConf.MaxSaturationValue) )
            {
                return fVal;
            }
            else if(fVal < HDConf.MinSaturationValue)
            {
                return HDConf.MinSaturationValue;
            }
            else
            {
                return HDConf.MaxSaturationValue;
            }
        break;
    }
    return fVal;
}

int dspCameraInfoShow(void)
{
    int ret =-1;
    memset(&HDConf, 0, sizeof(cam_info_t));
    
    ret = ccdCamInfoGet(&HDConf);
#if defined(XCSH140)||defined(XCSH500)
    if( HDConf.image_width == WIDTH_200W )
    {
        g_uiCamLaneSum = 2;
    }
    else if ( HDConf.image_width == WIDTH_500W )
    {
        g_uiCamLaneSum = 4;
    }
    else
    {
        g_uiCamLaneSum = 1;
    }
#elif defined(XCSH200T)
    unsigned int uiTmp =0;
    /* 注意:200万摄像头，右转90度 */
    if( HDConf.image_width == WIDTH_200W )
    {
        uiTmp = HDConf.image_height;
        HDConf.image_height = HDConf.image_width;
        HDConf.image_width = uiTmp;
        g_uiCamLaneSum = 2;
    }
    else
    {
        g_uiCamLaneSum = 2;
    }
#elif defined(XCSH500T)
    unsigned int uiTmp =0;
    if ( HDConf.image_width == WIDTH_500W )
    {
        g_uiCamLaneSum = 4;
    }
    else if( HDConf.image_width == WIDTH_200W )
    {
        uiTmp = HDConf.image_height;
        HDConf.image_height = HDConf.image_width;
        HDConf.image_width = uiTmp;
        g_uiCamLaneSum = 2;
    }
    else
    {
        g_uiCamLaneSum = 1;
    }
#elif defined(CYC200XZD)
    HDConf.image_width = 1600;
    HDConf.image_height = 1200;
    HDConf.totalpixels = HDConf.image_width*HDConf.image_height;
    g_uiCamLaneSum = 2;

#elif defined(CYC500XZD)
    HDConf.image_width = 2592;
    HDConf.image_height = 1932;
    HDConf.totalpixels = HDConf.image_width*HDConf.image_height;
    g_uiCamLaneSum = 3;

#endif   

    CCDCTL_INFO("1394 Cam info:\n");
    CCDCTL_INFO("total pixels:%d width:%d height:%d \n", HDConf.totalpixels, HDConf.image_width, HDConf.image_height);
    CCDCTL_INFO("exposure   minVal:%f   maxVal:%f\n", HDConf.MinExposureValue, HDConf.MaxExposureValue);
    CCDCTL_INFO("Gain       minVal:%f   maxVal:%f\n", HDConf.MinGainValue, HDConf.MaxGainValue);
    CCDCTL_INFO("shutter    minVal:%f   maxVal:%f\n", HDConf.MinShutterValue, HDConf.MaxShutterValue);
    CCDCTL_INFO("BrightNess minVal:%f   maxVal:%f\n", HDConf.MinBrightNessValue, HDConf.MaxBrightNessValue);    
    CCDCTL_INFO("Gamma      minVal:%f   maxVal:%f\n", HDConf.MinGammaValue, HDConf.MaxGammaValue);
    CCDCTL_INFO("Saturation minVal:%f   maxVal:%f\n", HDConf.MinSaturationValue, HDConf.MaxSaturationValue);
    CCDCTL_INFO("SharpNess  minVal:%f   maxVal:%f\n", HDConf.MinSharpNessValue, HDConf.MaxSharpNessValue);
    /* 由于曝光补偿和增益在最大值时容易出现白屏，所以人为地降低曝光补偿和增益的最大值 */
    HDConf.MaxExposureValue = 2.1;
    HDConf.MaxGainValue = 24;
    return ret;
}


int dc1394_feature_set_mode(dc1394feature_t feature, dc1394feature_mode_t mode)
{
    int ret =-1;
    unsigned int offset;
    unsigned int curval;

    if ( (feature<DC1394_FEATURE_MIN) || (feature>DC1394_FEATURE_MAX) )
        return -1;

    if ( (mode<DC1394_FEATURE_MODE_MIN) || (mode>DC1394_FEATURE_MODE_MAX) )
        return -2;

    if (feature == DC1394_FEATURE_TRIGGER) {
        return -3;
    }

    FEATURE_TO_VALUE_OFFSET(feature, offset);

    ret = ccdCtlRegValGet(offset, &curval);

    curval &= 0x00ffffffUL;
    if ( mode==DC1394_FEATURE_MODE_AUTO )
    {
        curval |= 0x83000000UL;
        ret = ccdCtlRegValSet(offset, curval);
    }
    else if ( mode==DC1394_FEATURE_MODE_MANUAL ) 
    {
        curval &= 0xFEFFFFFFUL;
        curval |= 0x82000000UL;
        ret = ccdCtlRegValSet(offset, curval);
    }
    else if ( mode==DC1394_FEATURE_MODE_ONE_PUSH_AUTO )
    {
        curval |= 0x04000000UL;
        curval |= 0x82000000UL;
        ret = ccdCtlRegValSet(offset, curval);
    }
    return ret;
}


/******************************************************************************
 * dc1394_feature_absolute_mode_conf() :
 * 
 * DESCRIPTION:  使能或不使能绝对值寄存器读写功能
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.05.06, wangbin Create
 * --------------------
 ******************************************************************************/
int dc1394_feature_absolute_mode_conf(dc1394feature_t feature, int iSta)
{
	int ret =-1;
	unsigned int offset;
	unsigned int curval;

	if ( (feature<DC1394_FEATURE_MIN) || (feature>DC1394_FEATURE_MAX) )
	{
		return -1;
	}

	if (feature == DC1394_FEATURE_TRIGGER)
	{
		return -3;
	}

	FEATURE_TO_VALUE_OFFSET(feature, offset);

	ret = ccdCtlRegValGet(offset, &curval);

	curval&= 0xFAFFFFFFUL; /* 先配置为手动模式 */
	if( 1 == iSta )
	{
		curval |= 0xc2000000UL;
	}
	else
	{
		curval &= 0xBFFFFFFFUL;
		curval |= 0x02000000UL;
	}
	ret = ccdCtlRegValSet(offset, curval);

	//ret = ccdCtlRegValGet(offset, &curval);
	return ret;
}


/********************************************************************************/
/* Get/Set Absolute Control Registers                                           */
/********************************************************************************/

unsigned int s_auiPGCamRegQuery[6][2]=
{
	{0x700,0x3c024c},
	{0x704,0x3c0240},
	{0x714,0x3c0260},
	{0x718,0x3c0250},
	{0x71c,0x3c0244},
	{0x720,0x3c0248}
};

static int QueryAbsoluteCSROffset(dc1394feature_t feature, int *offset)
{
	int ret =-1;
	int j =0;
    int absoffset, retval;
    unsigned int quadlet=0;

    FEATURE_TO_ABS_VALUE_OFFSET(feature, absoffset);

#if 0
    retval=ccdCtlRegValGet(absoffset, &quadlet);
#else
	for(j=0; j< 6; j++)
	{
		if( s_auiPGCamRegQuery[j][0] == absoffset )
		{
			quadlet = s_auiPGCamRegQuery[j][1];
			break;
		}
	}
#endif
	//shell_print("22pgCam:absoffset:0x%x,quadlet:0x%x.\n", absoffset, quadlet);
    
    *offset=quadlet * 0x04;
    *offset &= 0xfff;
    return retval;
}

static int dc1394_get_absolute_register(unsigned int feature, int offset, unsigned int *value)
{
    int absoffset;

    QueryAbsoluteCSROffset(feature, &absoffset);

    return ccdCtlRegValGet(absoffset+offset, value);
}

int dc1394_set_absolute_register(unsigned int feature, int offset, unsigned int value)
{
    int absoffset;

    QueryAbsoluteCSROffset(feature, &absoffset);

    return ccdCtlRegValSet(absoffset+offset, value);
}


int dc1394_feature_get_absolute_value(dc1394feature_t feature, float *value)
{
    int ret =-1;
    if ( (feature > DC1394_FEATURE_MAX) || (feature < DC1394_FEATURE_MIN) ) {
        return -1;
    }
    ret = dc1394_get_absolute_register(feature, REG_CAMERA_ABS_VALUE, (unsigned int *)value);
    return ret;
}


int dc1394_feature_get_absolute_minvalue(dc1394feature_t feature, float *value)
{
    int ret =-1;
    if ( (feature > DC1394_FEATURE_MAX) || (feature < DC1394_FEATURE_MIN) ) {
        return -1;
    }
    ret = dc1394_get_absolute_register(feature, REG_CAMERA_ABS_MIN, (unsigned int *)value);
    return ret;
}

int dc1394_feature_get_absolute_maxvalue(dc1394feature_t feature, float *value)
{
    int ret =-1;
    if ( (feature > DC1394_FEATURE_MAX) || (feature < DC1394_FEATURE_MIN) ) {
        return -1;
    }
    ret = dc1394_get_absolute_register(feature, REG_CAMERA_ABS_MAX, (unsigned int *)value);
    return ret;
}


int dc1394_feature_set_absolute_value(dc1394feature_t feature, float value)
{
    int ret =-1;
    unsigned int tempq;
    
    memcpy(&tempq,&value,4);

    if ( (feature > DC1394_FEATURE_MAX) || (feature < DC1394_FEATURE_MIN) ) {
        return ret;
    }
    
    //CCDCTL_INFO("abs set float val:%f \n", value);
    //CCDCTL_INFO("abs set int val:0x%x \n", tempq);
    ret = dc1394_set_absolute_register(feature, REG_CAMERA_ABS_VALUE, tempq);
    return ret;
}


int dc1394_exposure_set_interval(float MaxValue , float MinValue)
{
    int ret =-1;
    unsigned int reValue = 0;
    unsigned int nMax = 0;
    unsigned int nMin = 0;
    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_EXPOSURE, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_EXPOSURE, MaxValue);
    if(ret)
    {
        return ret;
    }
    
    usleep(100000);
    ret = ccdCtlRegValGet(REG_CAMERA_EXPOSURE, &reValue);

    nMax = reValue & 0xFFF;
    
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_EXPOSURE, MinValue);
    
    usleep(100000);
    ret = ccdCtlRegValGet(REG_CAMERA_EXPOSURE, &reValue);

    nMin = reValue & 0xFFF;

    CCDCTL_INFO("inter_exposure min val:0x%x max val:0x%x\n", nMin, nMax);    
    ret = dc1394_feature_set_mode(DC1394_FEATURE_EXPOSURE, DC1394_FEATURE_MODE_AUTO);
    ret += ccdCtlRegValSet(REG_CAMERA_AUTO_INTER_EXPOSURE, 0x80000000|nMax|nMin<<12);
    if(ret)
    {
        return ret;
    }
    return 0;
}


int dc1394_shutter_set_interval(float MaxValue , float MinValue)
{
    int ret =-1;
    unsigned int reValue = 0;
    unsigned int nMax = 0;
    unsigned int nMin = 0;
    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_SHUTTER, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_SHUTTER, MaxValue);
    if(ret)
    {
        return ret;
    }
    usleep(100000);
    ret = ccdCtlRegValGet(REG_CAMERA_SHUTTER, &reValue);

    nMax = reValue & 0xFFF;
    
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_SHUTTER, MinValue);
    if(ret)
    {
        return ret;
    }
    usleep(100000);
    ret = ccdCtlRegValGet(REG_CAMERA_SHUTTER, &reValue);

    nMin = reValue & 0xFFF;

    CCDCTL_INFO("inter_shutter min val:0x%x max val:0x%x\n", nMin, nMax);
    ret = dc1394_feature_set_mode(DC1394_FEATURE_SHUTTER, DC1394_FEATURE_MODE_AUTO);
    ret += ccdCtlRegValSet(REG_CAMERA_AUTO_INTER_SHUTTER, 0x82000000|nMax|nMin<<12);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_gain_set_interval(float MaxValue , float MinValue)
{
    int ret =-1;
	unsigned int reValue = 0;
    unsigned int nMax = 0;
	unsigned int nMin = 0;
	
    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_GAIN, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_GAIN, MaxValue);
    if(ret)
    {
        return ret;
    }
    usleep(100000);
    ret = ccdCtlRegValGet(REG_CAMERA_GAIN, &reValue);

    nMax = reValue & 0xFFF;

    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_GAIN, MinValue);
    if(ret)
    {
        return ret;
    }
    usleep(100000);
    ret = ccdCtlRegValGet(REG_CAMERA_GAIN, &reValue);

    nMin = reValue & 0xFFF;

    CCDCTL_INFO("inter_gain min val:0x%x max val:0x%x\n", nMin, nMax);
    ret = dc1394_feature_set_mode(DC1394_FEATURE_GAIN, DC1394_FEATURE_MODE_AUTO);
    ret += ccdCtlRegValSet(REG_CAMERA_AUTO_INTER_GAIN, 0x82000000|nMax|nMin<<12);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_set_interval_off(int iRegOff)
{
    int ret =-1;
    CCDCTL_INFO("ccd interval fun disable!\n");
    ret = ccdCtlRegValSet(iRegOff, 0x00000000);
    return ret;
}

int dc1394_set_interval_on(int iRegOff, unsigned int uiVal)
{
    int ret =-1;
    CCDCTL_INFO("ccd interval fun enable,reg:0x%x val:0x%x\n", iRegOff, uiVal);
    ret = ccdCtlRegValSet(iRegOff, uiVal);
    return ret;
}


int ccdCSRValGet(int iAdd, unsigned int *puiVal)
{
    int ret =-1;
    ret = ccdCtlRegValGet(iAdd, puiVal);
    return ret;
}


int ccdCSRValSet(int iAdd, unsigned int uiVal)
{
    int ret =-1;
    unsigned int uiTmpVal =0;
    strCamParam param;
    memset(&param, 0, sizeof(param));

    if(s_iSta != 0)
    {
        return s_iSta;
    }

    if(-1 == iCcdCtlFd)
    {
        	CCDCTL_ERROR("fd error\n");
        	return -1;
    }

    ret = ccdCtlRegValGet(iAdd, &uiTmpVal);
    
    uiTmpVal  &= 0xfffff000;
    uiVal = uiVal&0xfff;
    uiTmpVal  |=  uiVal;
    
    param.offset_header = iAdd;
    param.value = uiTmpVal;
    #if defined(CYC200XZD)||defined(CYC500XZD)

    zebraCtlRegValSet(iAdd, uiTmpVal);
    return 0;
    
    #else
    //CCDCTL_INFO("ccdCSRValSet set addr:0x%x,val:0x%x\n", param.offset_header, param.value);
    ret = ioctl(iCcdCtlFd, VCAP_CMD_SET_REG, &param);
    if (ret < 0)
    {
        	CCDCTL_ERROR("vpif1394 driver set reg failed!\n");
        	return -1;
    }
    else
    {
        	//CCDCTL_INFO("vpif1394 driver set reg ok!\n");
    }
    #endif
    
    return 0;
}


int dc1394_frameRate_set(float fValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_FRAME_RATE, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_FRAME_RATE, fValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_frameRate_get(float *pfValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_FRAME_RATE, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_get_absolute_value(DC1394_FEATURE_FRAME_RATE, pfValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}



int dc1394_Hue_set(float fValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_HUE, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_HUE, fValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_Hue_get(float *pfValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_HUE, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_get_absolute_value(DC1394_FEATURE_HUE, pfValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}


int dc1394_Saturation_set(float fValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_SATURATION, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_SATURATION, fValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_Saturation_get(float *pfValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_SATURATION, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_get_absolute_value(DC1394_FEATURE_SATURATION, pfValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_Gamma_set(float fValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_GAMMA, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_GAMMA, fValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_Gamma_get(float *pfValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_GAMMA, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_get_absolute_value(DC1394_FEATURE_GAMMA, pfValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_Brightness_set(float fValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_BRIGHTNESS, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_set_absolute_value(DC1394_FEATURE_BRIGHTNESS, fValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

int dc1394_Brightness_get(float *pfValue)
{
    int ret =-1;

    ret = dc1394_feature_absolute_mode_conf(DC1394_FEATURE_BRIGHTNESS, 1);
    if(ret)
    {
        return ret;
    }
    ret = dc1394_feature_get_absolute_value(DC1394_FEATURE_BRIGHTNESS, pfValue);
    if(ret)
    {
        return ret;
    }
    return 0;
}

#endif
