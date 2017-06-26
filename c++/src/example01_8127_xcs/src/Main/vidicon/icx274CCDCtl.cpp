/******************************************************************************
 * sonyCCDCtl.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.06.28, wb Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <math.h>

#include "icx274CCDCtl.h"
#include "xcsGlobalDef.h"

#include "SysHandle.h"
#include "SysConf.h"
#include "xcsCommDef.h"
#include "dsp.h"
#include "ispParam.h"
#include "ispV2Param.h"

#if !defined(CYC500XZD)&&!defined(CYC200XZD)

#define    VPFE_WIN_1280X960   0
#define    VPFE_WIN_1280X720   1
#define    VPFE_WIN_720X576    2

#define VPFE_CMD_START              (1) 
#define DEV_VCAP "/dev/misc/vpif" //"/dev/misc/nwvpfe" 


extern char g_cCCDCtlSta;

extern cam_info_t HDConf;

int g_icx274Ctl_Debug= 0xffff;

static int iIcxCcdFd =-1;

int icx274CtlDebugSwitch(int level)
{

    g_icx274Ctl_Debug = level;
    return 0;
}

int icx274RegCtlInit(void)
{
    int ret = -1;

    iIcxCcdFd = open(DEV_VCAP, O_RDONLY);
    if (iIcxCcdFd < 0)
    { 
        ICX274CTL_ERROR("nwvcap driver open failed!\n"); 
        return -1;
    }
    ret = ioctl(iIcxCcdFd, VPFE_CMD_START, VPFE_WIN_1280X960);
    if (ret < 0)
    {
        ICX274CTL_ERROR("vpif driver start failed!\n");
        return -2;
    }
    else
    {
        ICX274CTL_INFO("vpif driver start ok!\n");
    }
#if !NEW_PIC_ARITHC
    ret = ispCCDParamInit();
    if (ret < 0)
    {
        ICX274CTL_ERROR("ispCCDParamInit failed!\n");
        return -3;
    }
    else
    {
        ICX274CTL_INFO("ispCCDParamInit ok!\n");
    }
#endif    
    return ret;
}

int icx274RegCtlUnInit(void)
{
    int ret = -1;
    ret = close(iIcxCcdFd);
    ICX274CTL_ERROR("vpif driver dev close ok!\n");
    return ret;
}

#if 0
int ccdCamInfoGet(cam_info_t *pCamerainfo)
{
#if defined(CYC200KW)
    pCamerainfo->image_width  = 1920;
    pCamerainfo->image_height = 1072;
#elif defined(CYC500XKW4C)||defined(CYC500XAW)
    pCamerainfo->image_width  = 2592;
    pCamerainfo->image_height = 1932;
#elif defined(CYC800JX)
    pCamerainfo->image_width  = 3312;
    pCamerainfo->image_height = 2432;
#elif defined(CYC800XKW4C)
    pCamerainfo->image_width  = 3200;
    pCamerainfo->image_height = 2466;
#elif defined(CYC500JX)
    pCamerainfo->image_width  = 2496;
    pCamerainfo->image_height = 2048;
#else
    pCamerainfo->image_width  = 1600;
    pCamerainfo->image_height = 1200;
#endif
    pCamerainfo->totalpixels  = pCamerainfo->image_width*pCamerainfo->image_height;

#if NEW_PIC_ARITHC
    pCamerainfo->MaxExposureValue = 255;
    pCamerainfo->MinExposureValue = 0;
#else
    pCamerainfo->MaxExposureValue = 255;
    pCamerainfo->MinExposureValue = 0;
#endif

#if defined(CYC500XKW4C)||defined(CYC500XAW)
    #if NEW_PIC_ARITHC
    pCamerainfo->MaxShutterValue = 0.01;
    pCamerainfo->MinShutterValue = 0.00001;
    #else
    pCamerainfo->MaxShutterValue = 0.05;
    pCamerainfo->MinShutterValue = 0.00007;
    #endif
#elif defined(CYC800JX)||defined(CYC500JX)
    pCamerainfo->MaxShutterValue = 2;
    pCamerainfo->MinShutterValue = 0.00001;
#else
    pCamerainfo->MaxShutterValue = 0.05;
    pCamerainfo->MinShutterValue = 0.00001;
#endif

#if defined(CYC800JX)||defined(CYC800XKW4C)||defined(CYC500JX)
    pCamerainfo->MaxGainValue = 24;
    pCamerainfo->MinGainValue = 0;
#else
    #if NEW_PIC_ARITHC
    pCamerainfo->MaxGainValue = 24;
    pCamerainfo->MinGainValue = 6;
    #else
    pCamerainfo->MaxGainValue = 35;
    pCamerainfo->MinGainValue = 0;
    #endif
#endif

#if NEW_PIC_ARITHC
    pCamerainfo->MaxBrightNessValue = 5;
    pCamerainfo->MinBrightNessValue = 0;
#else
    pCamerainfo->MaxBrightNessValue = 255;
    pCamerainfo->MinBrightNessValue = 0;
#endif

#if NEW_PIC_ARITHC
    pCamerainfo->MaxSharpNessValue = 4;
    pCamerainfo->MinSharpNessValue = 2;

    pCamerainfo->MaxSaturationValue = 5;
    pCamerainfo->MinSaturationValue = 0;
#else
    pCamerainfo->MaxSharpNessValue = 255;
    pCamerainfo->MinSharpNessValue = 0;
    
    pCamerainfo->MaxSaturationValue = 255;
    pCamerainfo->MinSaturationValue = 0;    
#endif
    

    pCamerainfo->MaxContrastValue = 255;
    pCamerainfo->MinContrastValue = 0;
#if NEW_PIC_ARITHC
    pCamerainfo->MaxIsoValue = 5;
    pCamerainfo->MinIsoValue = 0;
#else
    pCamerainfo->MaxIsoValue = 255;
    pCamerainfo->MinIsoValue = 0;
#endif
    return 0;
}
#endif




#if 0
int icx274CamParamGet(int iType, int* piMode, int* pfFixVal, int* pfMinVal, int* pfMaxVal)
{
    int ret =0;
    camera274_param_t str274Param;
    memset(&str274Param, 0, sizeof(str274Param));
    
    str274Param.uiType      = iType;

    ret = cameraRunStaParamGet(0, &str274Param);
    if(ret != 0)
    {
        ICX274CTL_INFO("cam 274 set type:%d failed\n", iType);
    }
    else
    {
        *pfMaxVal = str274Param.fMaxValue;
        *pfMinVal = str274Param.fMinValue;
        *pfFixVal = str274Param.fValue;
        ICX274CTL_INFO("cam 274 get type:%d,mode:%d,fixVal:%f,minVal:%f,maxVal:%f!\n", \
        iType, str274Param.uiMode, str274Param.fValue, str274Param.fMinValue, str274Param.fMaxValue);
    }
    return ret;
}
#endif

int icx274CamParamSet(int iMode, int iType, float fMinVal, float fMaxVal, float fFixVal)
{
    int ret =0;
#if 0    
    //camera274_param_t str274Param;
    //memset(&str274Param, 0, sizeof(str274Param));
    //switch(iMode)
    //{
    //    case FIX:
    //        str274Param.uiMode = 0;
    //        break;
    //    case INTERVAL:
    //        str274Param.uiMode = 1;
    //        break;
    //    case AUTO:
    //        str274Param.uiMode = 2;
    //        break;
    //    case DISABEL:
    //        str274Param.uiMode = 3;
    //        break;
    //    default:
    //        str274Param.uiMode = 0;
    //        break;
    //}

    //if(CAMERA_PARAM_MINSCALE == iType)
    //{
    //    if( fFixVal <= 2)
    //    {
    //        fFixVal = 2;
    //    }
    //}
    //
    //str274Param.uiType      = iType;
    //str274Param.fMaxValue   = fMaxVal;
    //str274Param.fMinValue   = fMinVal;
    //str274Param.fValue      = fFixVal;

    ////ret = dsp_camera274_param_set(0, &str274Param);
    //if(ret != 0)
    //{
    //    ICX274CTL_INFO("cam 274 set type:%d failed\n", iType);
    //}
    //else
    //{
    //    ICX274CTL_INFO("cam 274 set type:%d mode:%d,fixVal:%f,fMinVal:%f,fMaxVal:%f\n", iType, iMode, \
    //    str274Param.fValue, str274Param.fMinValue, str274Param.fMaxValue);
    //}
#endif    
    return ret;
}

#if 0
int icx274CamParamGet(int iType, int* piMode, int* pfFixVal, int* pfMinVal, int* pfMaxVal)
{
    int ret =0;
    camera274_param_t str274Param;
    memset(&str274Param, 0, sizeof(str274Param));
    
    str274Param.uiType      = iType;

    ret = cameraRunStaParamGet(0, &str274Param);
    if(ret != 0)
    {
        ICX274CTL_INFO("cam 274 set type:%d failed\n", iType);
    }
    else
    {
        *pfMaxVal = str274Param.fMaxValue;
        *pfMinVal = str274Param.fMinValue;
        *pfFixVal = str274Param.fValue;
        ICX274CTL_INFO("cam 274 get type:%d,mode:%d,fixVal:%f,minVal:%f,maxVal:%f!\n", \
        iType, str274Param.uiMode, str274Param.fValue, str274Param.fMinValue, str274Param.fMaxValue);
    }
    return ret;
}
#endif

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
        case ECONTRAST:
            if( (fVal >= HDConf.MinContrastValue) && (fVal <= HDConf.MaxContrastValue) )
            {
                return fVal;
            }
            else if ( fVal < HDConf.MinContrastValue )
            {
                return HDConf.MinContrastValue;
            }
            else
            {
                return HDConf.MaxContrastValue;
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
        case EISO:
            if( (fVal >= HDConf.MinIsoValue) && (fVal <= HDConf.MaxIsoValue) )
            {
                return fVal;
            }
            else if(fVal < HDConf.MinIsoValue)
            {
                return HDConf.MinIsoValue;
            }
            else
            {
                return HDConf.MaxIsoValue;
            }
            break;
        default:
            break;
    }
    return fVal;
}

int dspCameraInfoShow(void)
{
    int ret =-1;
    memset(&HDConf, 0, sizeof(cam_info_t));
    
    ret = ccdCamInfoGet(&HDConf);

#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)
    g_uiCamLaneSum = 2;
#elif defined(CYC500XKW4C)||defined(CYC500XAW)
    g_uiCamLaneSum = 3;
#elif defined(CYC200KX3)||defined(CYC200XKW)
    unsigned int uiTmp =0;
    /* 注意:200万摄像头，右转90度 */
    uiTmp = HDConf.image_height;
    HDConf.image_height = HDConf.image_width;
    HDConf.image_width = uiTmp;
    g_uiCamLaneSum = 2;

#elif defined(CYC500XKW4C)||defined(CYC500XAW)
    g_uiCamLaneSum = 3;
#else
	g_uiCamLaneSum = 3;
#endif

    ICX274CTL_INFO("icx274 Cam info:\n");
    ICX274CTL_INFO("total pixels:%d width:%d height:%d \n", HDConf.totalpixels, HDConf.image_width, HDConf.image_height);
    ICX274CTL_INFO("Grey          minVal:%f   maxVal:%f\n", HDConf.MinExposureValue, HDConf.MaxExposureValue);
    ICX274CTL_INFO("Gain          minVal:%f   maxVal:%f\n", HDConf.MinGainValue, HDConf.MaxGainValue);
    ICX274CTL_INFO("shutter       minVal:%f   maxVal:%f\n", HDConf.MinShutterValue, HDConf.MaxShutterValue);
    ICX274CTL_INFO("BrightNess    minVal:%f   maxVal:%f\n", HDConf.MinBrightNessValue, HDConf.MaxBrightNessValue);    
    ICX274CTL_INFO("MinScal       minVal:%f   maxVal:%f\n", HDConf.MinSharpNessValue, HDConf.MaxSharpNessValue);
    ICX274CTL_INFO("Colour adjust minVal:%f   maxVal:%f\n", HDConf.MinSaturationValue, HDConf.MaxSaturationValue);
    ICX274CTL_INFO("Auto white balance  minVal:%f   maxVal:%f\n", HDConf.MinContrastValue, HDConf.MaxContrastValue);    
    ICX274CTL_INFO("ISO           minVal:%f   maxVal:%f\n", HDConf.MinIsoValue, HDConf.MaxIsoValue);    
    return ret;
}

#endif


