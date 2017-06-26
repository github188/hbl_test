/******************************************************************************
 * dspParamManage.cpp:
 *
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 *
 * DESCRIPTION:
 *
 * modification history
 * --------------------
 * 01a, 2009.12.20, wangbin Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dsp.h"
#include "dsp_define.h"
#include "dsp_service.h"


#include "SysConf.h"
#include "SysHandle.h"
#include "xcsGlobalDef.h"
#include "xcsCommDef.h"

#include "JpegSnap.h"
#include "FtpEpSend.h"
#include "FtpPpSend.h"

#include "nMCtlApi.h"

#if defined(CYC200XZD)||defined(CYC500XZD)
#include "ccdCtl.h"
#else
#include "icx274CCDCtl.h"
#include "ispParam.h"
#include "ispV2Param.h"
#endif

/* 闪光工作状态模式全局变量，初始化为闪光灯 */
int g_iWorkFlashMode[MAX_VEHICLE_NUM] = {LIGHT_TYPE_FLASH};

/* 卡口频闪及闪光切换状态 */
int g_iPreSta = 0;

/* 电警频闪灯切换状态 */
int g_iEpPreSta = 0;

extern char g_cCCDCtlSta;

extern sem_t sem_dsp;

extern int g_iErrorTimes;

extern int g_aiCoilSta[MAX_VEHICLE_NUM];
extern int g_aiLampSta[MAX_VEHICLE_NUM];
extern int g_aiVirtualSta[MAX_VEHICLE_NUM];
extern cam_info_t HDConf;

/* 红灯检测方式全局变量，主要用于切换判断 */
extern int g_iRLDecType;
extern LANE_SWITCH_STA_INFO  g_EPLaneSwitchInfo[MAX_VEHICLE_NUM]; /* 电警车道切换状态 */

extern CAMERA_TIME_RANGE_INFO g_strCamTimeR[MAX_CAM_GROUP_NUM];

int dspEpSnapTypeGet(char cChanId);

int dspShowFeature(CAMERA_CTRL_CONF_INFO* pStrCam)
{
	DSP_DEBUG("cam type:0x%x\n", pStrCam->uiCamaraType);
	DSP_DEBUG("ctl type:0x%x\n", pStrCam->cCtlType);
	DSP_DEBUG("fix val:%f\n", pStrCam->fFixVal);
	DSP_DEBUG("max val:%f\n", pStrCam->fMaxVal);
	DSP_DEBUG("min val:%f\n", pStrCam->fMinVal);
	return 0;
}


/* 调整闪光生效行数 */
char kodarJudgeFlashLinesSave(char cMode, char cLines)
{
	int ret = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		strSysFuncCfg.cFlashLineMode = cMode;
		strSysFuncCfg.cDyncJudFlashLines = cLines;
		SysConfSet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("set sys conf of kodar flash lines failed!\n");
		return ret;
	}
}


/* 配置闪光生效行数 */
char kodarJudgeFlashLinesConf(void)
{
	int ret = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		if (strSysFuncCfg.cFlashLineMode == 0)
		{
			//icx274CamParamSet(FIX, CAMERA_FLASH_LINES_ADD, 0, 0, (float)strSysFuncCfg.cDyncJudFlashLines);
		}
		else
		{
			//icx274CamParamSet(FIX, CAMERA_FLASH_LINES_CUT, 0, 0, (float)strSysFuncCfg.cDyncJudFlashLines);
		}
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("get sys conf of kodar flash lines failed!\n");
		return ret;
	}
}

/* 保存饱和度、G分量阀值参数配置 */
char kodarFpgaNewParamSave(unsigned short usSaturation, unsigned short usGPixThresh)
{
	int ret = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		strSysFuncCfg.usSaturation = usSaturation;
		strSysFuncCfg.usGPixsThresh = usGPixThresh;
		SysConfSet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("set sys conf of fpga new param failed!\n");
		return ret;
	}
}


/* 配置fpga饱和度、G分量阀值参数 */
char kodarFpgaNewParamSet(void)
{
	int ret = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		//icx274CamParamSet(FIX, CAMERA_PARAM_SATURATION, 0, 0, (float)strSysFuncCfg.usSaturation);
		//icx274CamParamSet(FIX, CAMERA_PARAM_GPIXELS, 0, 0, (float)strSysFuncCfg.usGPixsThresh);
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("get sys conf of fpag new param failed!\n");
		return ret;
	}
}


/* 保存双快门参数配置 */
char kodarDubalShutParamSave(char cEnable, float fShutVal, float fGainVal)
{
	int ret = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		strSysFuncCfg.cDubalFEn = cEnable;
		strSysFuncCfg.fDubalFShut = fShutVal;
		strSysFuncCfg.fDubalFGain = fGainVal;
		SysConfSet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("set sys conf of dubal shut new param failed!\n");
		return ret;
	}
}


/* 双快门参数设置 */
char kodarDubalShutParamSet(char cMode)
{
	int ret = 0;
	/* 防止经常切换，所以定义为静态变量 */
	static int s_iMode = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	CUR_CAMERA_STATUS strCurCamSta;
	memset(&strCurCamSta, 0, sizeof(CUR_CAMERA_STATUS));
	strCurCamSta.cbSize =  sizeof(CUR_CAMERA_STATUS);

	ret = cameraRunStaParamGet(0, &strCurCamSta);
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	if (DUBAL_FLASH_FORCE_SWITCH == cMode)
	{
		s_iMode = 0;
	}
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		/* 为了防止运动模糊，当当前快门大于双快门时，切换到双快门模式 */
		if ((strSysFuncCfg.cDubalFEn > 0) && (s_iMode != FIX) && (strSysFuncCfg.fDubalFShut < strCurCamSta.fCurShutter))
		{
			syslog(LOG_ERR | LOG_USER, "switch1 to dubalShutter mode,dubalShut:%f,curShutter:%f.\n", \
				   strSysFuncCfg.fDubalFShut, strCurCamSta.fCurShutter);

			s_iMode = FIX;
			//icx274CamParamSet(s_iMode, CAMERA_PARAM_DUBAL_SHUTTER, strSysFuncCfg.fDubalFGain, 0, strSysFuncCfg.fDubalFShut);
		}
		else if (((strSysFuncCfg.cDubalFEn <= 0) || (strSysFuncCfg.fDubalFShut > strCurCamSta.fCurShutter))\
				 && (s_iMode != DISABEL))
		{
			syslog(LOG_ERR | LOG_USER, "exit1 dubalShutter mode,dubalShut:%f,curShutter:%f,s_iMode:%d.\n", \
				   strSysFuncCfg.fDubalFShut, strCurCamSta.fCurShutter, s_iMode);

			s_iMode = DISABEL;
			//icx274CamParamSet(s_iMode, CAMERA_PARAM_DUBAL_SHUTTER, strSysFuncCfg.fDubalFGain, 0, strSysFuncCfg.fDubalFShut);
		}
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("get sys conf of dubal shut param failed!\n");
		return ret;
	}
}

/* 保存红灯检测阀值参数配置 */
char redDectThresholdParamSave(char cRedDectSatu, char cRedDectBright)
{
	int ret = 0;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		strSysFuncCfg.cRedDecSaturation = cRedDectSatu;
		strSysFuncCfg.cRedDecBright = cRedDectBright;
		SysConfSet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
		return 0;
	}
	else
	{
		SYSHANDLE_INFO("set sys conf of red dect threshold param failed!\n");
		return ret;
	}
}

int newIspTotalLinesGet(void)
{
	int ret = 0;
	int iRealFR = 10;
	VIDEO_CONF_INFO strVideoConfInfo;
	ret = SysConfGet(e_VIDEO_CONF, &strVideoConfInfo, 0);
	if (ret != 0)
	{
		DSP_ERROR("%s %d get conf failed.\n", __FUNCTION__, __LINE__);
		return TOTAL_LINES * 125 / 10;
	}
	else
	{
		if (strVideoConfInfo.cFrameRate < 9)
		{
			iRealFR = 125;
		}
		else
		{
			iRealFR = strVideoConfInfo.cFrameRate * 10;
		}
		return TOTAL_LINES * iRealFR / 10;
	}
}

int dspCamParamSet(char cGroup)
{
	int ret = 0;
	int iShuterVal = 0;
	int iGainVal = 0;

	int iMaxShuterVal = 0;
	int iMaxGainVal = 0;

	NET_DEV_CCDPARAMCFG ndccp;
	memset(&ndccp, 0, sizeof(ndccp));

	CAMERA_CTRL_CONF_INFO strCamInfo;

	CAMERA_CTRL_CONF_INFO strGainCamInfo;

	/* 首先调整自动光圈位置 */
	pIrisPosAdjust(g_strCamTimeR[cGroup].cIrisPos);

	/* 快门参数设置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ESHUTTER + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara shutter conf error\n");
		return -1;
	}
	else
	{
		iShuterVal = (int)(1000000 * strCamInfo.fFixVal);
		iMaxShuterVal = (int)(1000000 * strCamInfo.fMaxVal);
		switch (strCamInfo.cCtlType)
		{
		case INTERVAL:
			ndccp.struExposure.byExposureMode = 0;
			ndccp.struExposure.dwMaxExposureVal = iMaxShuterVal;
			ndccp.struExposure.dwMinExposureVal = iShuterVal; // 使用固定值代替最小值
			break;
		case FIX:
			ndccp.struExposure.byExposureMode = 1;
			ndccp.struExposure.dwMinExposureVal = ndccp.struExposure.dwMaxExposureVal = iShuterVal;
			break;
		case DISABEL:
		case DEFAULT:
		case AUTO:
		default:
			ndccp.struExposure.byExposureMode = 0;
			ndccp.struExposure.dwMaxExposureVal = (int)(1000000 * HDConf.MaxShutterValue);
			ndccp.struExposure.dwMinExposureVal = (int)(1000000 * HDConf.MinShutterValue);
			break;
		}
	}

	/* 增益控制设置 */
	memset(&strGainCamInfo, 0, sizeof(strGainCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strGainCamInfo, EGAIN + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara gain conf error\n");
		return -1;
	}
	else
	{
		iGainVal = (int)strGainCamInfo.fFixVal;
		iMaxGainVal = (int)strGainCamInfo.fMaxVal;

		switch (strGainCamInfo.cCtlType)
		{
		case INTERVAL:
			ndccp.struGain.byGainMode = 0;
			ndccp.struGain.dwMaxGainValue = iMaxGainVal;
			ndccp.struGain.dwMinGainValue = iGainVal; // 使用固定值代替最小值
			break;
		case FIX:
			ndccp.struGain.byGainMode = 1;
			ndccp.struGain.dwMaxGainValue = ndccp.struGain.dwMinGainValue = iGainVal;
			break;
		case DISABEL:
		case DEFAULT:
		case AUTO:
		default:
			ndccp.struGain.byGainMode = 0;
			ndccp.struGain.dwMaxGainValue = (int)HDConf.MaxGainValue;
			ndccp.struGain.dwMinGainValue = (int)HDConf.MinGainValue;
			break;
		}
	}

	///* 快门与增益一起调用同一接口 */
	//newIspExposeSet(strCamInfo.cCtlType, strGainCamInfo.cCtlType, iShuterVal, iGainVal, iMaxShuterVal, iMaxGainVal);

	/* 与配置客户端是按曝光补偿通信，与dsp是按亮度来通信的 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EEXPOSURE + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara exposure conf error\n");
		return -1;
	}
	else
	{
		ndccp.struVideoEffect.byGreyLevel = strCamInfo.fFixVal;
		// newIspParamSet( strCamInfo.cCtlType, CAMERA_PARAM_GREY, (int)(strCamInfo.fFixVal));
	}

	/* 颜色校正配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ESATURATION + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara saturation conf error\n");
		return -1;
	}
	else
	{
		ndccp.struVideoEffect.byBrightnessLevel = strCamInfo.fFixVal;
		// newIspParamSet(strCamInfo.cCtlType, CAMERA_PARAM_COLOUR, (int)(strCamInfo.fFixVal));
	}

	/* 亮度配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EBRIGHTNESS + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara brightness conf error\n");
		return -1;
	}
	else
	{
		// 实际为宽动态
		switch ((int)strCamInfo.fFixVal)
		{
		case 0:
			ndccp.struWdr.byWDREnabled = 0;  //关
			break;
		case 1:
			ndccp.struWdr.byWDREnabled = 1;
			ndccp.struWdr.byWDRLevel = 0;	// 自动
			break;
		default:
			ndccp.struWdr.byWDREnabled = 1;
			ndccp.struWdr.byWDRLevel = (int)strCamInfo.fFixVal;	// 自动
			break;
		}
		// newIspParamSet(FIX, CAMERA_HAND_CONTRAST, (int)(strCamInfo.fFixVal));
	}

	/* Minscale配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ESHARPNESS + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara sharpness conf error\n");
		return -1;
	}
	else
	{
		// 实际为对比度
		ndccp.struVideoEffect.byContrastLevel = strCamInfo.fFixVal;
		//if (strCamInfo.cCtlType == FIX)
		//{
		//    newIspParamSet(strCamInfo.cCtlType, CAMERA_PARAM_MINSCALE, (int)(strCamInfo.fFixVal) );
		//}
	}

	/* 自动白平衡配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, ECONTRAST + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara gamma conf error\n");
		return -1;
	}
	else
	{
		// 实际为3D降噪
		switch ((int)strCamInfo.fFixVal)
		{
		case 0:
			ndccp.str3DNoiseRemove.by3DNoiseRemoveEn = 0;
			break;
		case 1:
			ndccp.str3DNoiseRemove.by3DNoiseRemoveEn = 1;
			ndccp.str3DNoiseRemove.by3DNoiseRemoveLevel = 0; // 强
			break;
		case 2:
			ndccp.str3DNoiseRemove.by3DNoiseRemoveEn = 1;
			ndccp.str3DNoiseRemove.by3DNoiseRemoveLevel = 1;	// 中
			break;
		case 3:
			ndccp.str3DNoiseRemove.by3DNoiseRemoveEn = 1;
			ndccp.str3DNoiseRemove.by3DNoiseRemoveLevel = 2;	// 弱
			break;
		}
		// newIspParamSet(strCamInfo.cCtlType, CAMERA_PARAM_WB, (int)(strCamInfo.fFixVal));
	}

	/* 锐度配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EISO + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara sharpness conf error\n");
		return -1;
	}
	else
	{
		ndccp.struVideoEffect.bySaturationLevel = strCamInfo.fFixVal;
		//newIspParamSet(strCamInfo.cCtlType, CAMERA_PARAM_SHARPEN, (int)(strCamInfo.fFixVal));
	}

	/* Gamma配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EGAMMA + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara gamma conf error\n");
		return -1;
	}
	else
	{
		// 实际为清晰度
		ndccp.struVideoEffect.bySharpnessLevel = strCamInfo.fFixVal;
		// newIspParamSet(FIX, CAMERA_GAMMA2, (int)(strCamInfo.fFixVal));
	}

	/* 降噪配置 */
	memset(&strCamInfo, 0, sizeof(strCamInfo));
	ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EAWB + cGroup * MAX_CAM_CTRL_NUM);
	if (0 != ret)
	{
		DSP_ERROR("get camara deNoise conf error\n");
		return -1;
	}
	else
	{
		// 2d降噪
		switch ((int)strCamInfo.fFixVal)
		{
		case 0:
			ndccp.str2DNoiseRemove.by2DNoiseRemoveEn = 0;
			break;
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
			ndccp.str2DNoiseRemove.by2DNoiseRemoveEn = 1;
			ndccp.str2DNoiseRemove.by2DNoiseRemoveLevel = strCamInfo.fFixVal - 1;
			break;
		default:
			ndccp.str2DNoiseRemove.by2DNoiseRemoveEn = 1;
			ndccp.str2DNoiseRemove.by2DNoiseRemoveLevel = strCamInfo.fFixVal - 1;
			break;
		}
		//newIspParamSet(FIX, CAMERA_DENOISE, (int)(strCamInfo.fFixVal));
	}

	/* 快门生效行数调整配置 */
	//kodarJudgeFlashLinesConf();
	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_CCDPARAMCFG, ndccp);
	if (0 != ret)
	{
		DSP_ERROR("%d %d call NET_DEV_SET_CCDPARAMCFG failed, error=%d.\n", __FILE__, __FUNCTION__, NET_DVE_GetLastError());
	}
	//sem_post(&sem_dsp);
	return 0;
}

int dspEncParamSet(int iChanId)
{
	int ret = 0;
	unsigned int iOsdLen = 0;
	EncParam encParam;
	NameParam osdParam;
	memset(&encParam, 0, sizeof(encParam));
	memset(&osdParam, 0, sizeof(osdParam));
	VIDEO_CONF_INFO strVideoConfInfo;
	ret = SysConfGet(e_VIDEO_CONF, &strVideoConfInfo, iChanId);
	if (ret != 0)
	{
		DSP_ERROR("%s %d get conf failed.\n", __FUNCTION__, __LINE__);
		return -1;
	}

	encParam.encType = strVideoConfInfo.cEncodeType;
	encParam.imgFmt  = strVideoConfInfo.cImageResol;
	encParam.keyIntv = strVideoConfInfo.cKeyFInterval;
	encParam.bitRate = strVideoConfInfo.uiBitRate;
	DSP_INFO("%s %d wb BITRATE:%d.\n", __FUNCTION__, __LINE__, strVideoConfInfo.uiBitRate);
	encParam.frameRate = strVideoConfInfo.cFrameRate;
	encParam.nEncQuality = strVideoConfInfo.cVQuality;
	encParam.is_cbr = strVideoConfInfo.cVBR;

	iOsdLen = strlen(strVideoConfInfo.szContent);
	if (iOsdLen > sizeof(osdParam.name))
	{
		iOsdLen = sizeof(osdParam.name);
	}

	osdParam.startX = strVideoConfInfo.usXPos;
	osdParam.startY = strVideoConfInfo.usYPos;
	/* 点阵大小由Dsp字库决定 */
	osdParam.width = 0;//8*iOsdLen;
	osdParam.height = 16;
	memset(osdParam.name, 0, sizeof(osdParam.name));
	memcpy(osdParam.name, strVideoConfInfo.szContent, iOsdLen);

	ret = dspSetEncParam(iChanId, &encParam);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set encParam ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}

	ret = dspSetNamePos(iChanId, &osdParam);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set osdParam ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_INFO("%s %d set osdParam ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}
	//sem_wait(&sem_dsp);
	PLATE_VIDEO_DEBUG_PARAM pvdp;
	memset(&pvdp, 0, sizeof(pvdp));
	pvdp.chan_id = iChanId;
	pvdp.debugEnable = strVideoConfInfo.ucDebugFrameEn;
	pvdp.x = strVideoConfInfo.usDebugFrameX;
	pvdp.y = strVideoConfInfo.usDebugFrameY;
//	ret = CALL_CYC_CMD(NET_DEV_SET_PLATE_VIDEO_DEBUG, pvdp);
	//sem_post(&sem_dsp);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set plate video ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_ERROR("%s %d set plate video ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}

	return 0;
}


int dspLPRParamSet(int iChanId)
{
	int ret = 0;
	PASSPORT_OVERALL_CONF_INFO strPPOverAllConf;
	LICENCE_PLATE_INFO strLPRParam;
	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPOverAllConf, 0);
	if (0 != ret)
	{
		SYSHANDLE_ERROR("read pp overall conf failed\n");
		return -1;
	}
	memset(&strLPRParam, 0, sizeof(strLPRParam));
	strLPRParam.chan_id = iChanId;
	strLPRParam.enable = strPPOverAllConf.ucLPREn;
	strLPRParam.firstCharConf = strPPOverAllConf.ucLPRCConfidence;
	memcpy(&(strLPRParam.defaultFirstChar), strPPOverAllConf.szLPRFirstChar, sizeof(strLPRParam.defaultFirstChar));

	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_LICENCE_PLATE_RECOGNIZE, strLPRParam);
	//sem_post(&sem_dsp);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set LPR ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_ERROR("%s %d set LPR ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}
	return 0;
}

/******************************************************************************
 * dspVLFlashModeSet() :
 *
 * DESCRIPTION:  在使用虚拟线圈时，需要告诉DSP闪光模式，即要不要闪光
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 *01a, 2010.01.12, wangbin Create
 * --------------------
 ******************************************************************************/
int dspVLFlashModeSet()
{
	unsigned int i = 0;
	int ret = 0;
	//sem_wait(&sem_dsp);
	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		GT5000_SHINEMODE_t param;
		memset(&param, 0, sizeof(GT5000_SHINEMODE_t));
		param.chan_id = i;
		if (LIGHT_TYPE_FLASH == g_iWorkFlashMode[i])
		{
			param.shineMode = SHINE_LED_MODE;
			DSP_INFO("lane:%d,%s %d dsp_service_GT5000_ShineOrNot flashFlag:0.\n", i, __FUNCTION__, __LINE__);
		}
		else
		{
			param.shineMode = NO_SHINE_LED_MODE;
			DSP_INFO("lane:%d, %s %d dsp_service_GT5000_ShineOrNot flashFlag:1.\n", i, __FUNCTION__, __LINE__);
		}
		ret = CALL_CYC_CMD(NET_DEV_SET_GT5000_SHINE_OR_NOT, param);
	}
	//sem_post(&sem_dsp);

	return ret;
}

int olviaParalWorkModeGet(void)
{
	int ret = 1;
	PP_FUNC_ENABLE_INFO strPpFuncCfg;
	memset(&strPpFuncCfg, 0, sizeof(strPpFuncCfg));

	ret = SysConfGet(e_PP_FUNC_ENABLE_CONF, &strPpFuncCfg, 0);
	if (0 == ret)
	{
		return strPpFuncCfg.ucOlviaRadaParalEn;
	}
	else
	{
		return ret;
	}
	return ret;
}

/* 卡口相关功能使能配置 */
int dspPpFunEnSet(char cMode, unsigned int uiLane)
{
	int ret = 0;
	PP_FUNC_ENABLE_INFO strPpFuncCfg;
	PPVL_SNAP_DEBUG_INFO strPpVlParamSet;

	memset(&strPpFuncCfg, 0, sizeof(strPpFuncCfg));

	//sem_wait(&sem_dsp);
	ret = SysConfGet(e_PP_FUNC_ENABLE_CONF, &strPpFuncCfg, 0);
	if (0 == ret)
	{
		strPpVlParamSet.chan_id = 0;
		strPpVlParamSet.lane_id = 0;
		strPpVlParamSet.isOsdPlateScope   = strPpFuncCfg.ucOsdPlateScopeEn;
		strPpVlParamSet.plateScopeIncSize = strPpFuncCfg.usPlateScopeIncSize;
		strPpVlParamSet.radarSpeedGap     = strPpFuncCfg.ucRadarSpeedGap;
		strPpVlParamSet.radarForwardLim   = strPpFuncCfg.ucRadarForwardLim;
		strPpVlParamSet.radarBackwardLim  = strPpFuncCfg.ucRadarBackwardLim;
		strPpVlParamSet.isShieldReverse   = strPpFuncCfg.ucisShieldReverse;
		strPpVlParamSet.isDecTaillight    = strPpFuncCfg.ucIsDecTaillight;
		strPpVlParamSet.ucFaceIdenEn      = strPpFuncCfg.ucFaceIdenEn;
		strPpVlParamSet.ucFaceOvenlayEn   = strPpFuncCfg.ucFaceOvenlayEn;

		ret = CALL_CYC_CMD(NET_DEV_SET_PP_DEBUG, strPpVlParamSet);
		if (ret != 0)
		{
			SYSHANDLE_ERROR("call NET_DEV_SET_PP_DEBUG failed!\n");
		}
		else
		{
			SYSHANDLE_INFO("call NET_DEV_SET_PP_DEBUG ok!\n");
		}
	}
	else
	{
		SYSHANDLE_INFO("get pp func conf error!\n");
		return ret;
	}
	//sem_post(&sem_dsp);
	return ret;
}

/* 电子警察模块初始化参数配置 */

int dspEpJpgOsdConfSet(int iChanId)
{
	int ret = 0;
	EP_JPG_OSD_CONF		 strSysEpJpgOsdConfGet;
	DSP_EP_JPG_OSD_CONF	 strEpJpgOsdConf;

	ret = SysConfGet(e_EP_JPG_OSD_CONF, &strSysEpJpgOsdConfGet, 0);
	if (0 != ret)
	{
		SYSHANDLE_ERROR("read ep jpg osd conf failed\n");
		return -1;
	}
	memset(&strEpJpgOsdConf, 0, sizeof(strEpJpgOsdConf));
	memcpy((void*)(&strEpJpgOsdConf) + sizeof(unsigned int), &strSysEpJpgOsdConfGet, sizeof(strSysEpJpgOsdConfGet));
	strEpJpgOsdConf.chan_id = iChanId;
	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_EP_JPG_OSD_CONF, strEpJpgOsdConf);
	//sem_post(&sem_dsp);
	if (0 != ret)
	{
		DSP_ERROR("%s %d ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_ERROR("%s %d ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}
	return 0;
}

int dspEpLPRParamSet(int iChanId)
{
	int ret = 0;
	EP_LPR_CONF_INFO strEpLprInfoGet;
	LICENCE_PLATE_INFO strLPRParam;
	ret = SysConfGet(e_EP_LPR_INFO_CONF, &strEpLprInfoGet, 0);
	if (0 != ret)
	{
		SYSHANDLE_ERROR("read pp overall conf failed\n");
		return -1;
	}
	memset(&strLPRParam, 0, sizeof(strLPRParam));
	strLPRParam.chan_id = iChanId;
	strLPRParam.enable = strEpLprInfoGet.ucLPREn;
	strLPRParam.firstCharConf = strEpLprInfoGet.ucLPRCConfidence;
	memcpy(&(strLPRParam.defaultFirstChar), strEpLprInfoGet.szLPRFirstChar, sizeof(strLPRParam.defaultFirstChar));
	strLPRParam.usLPRWidth = strEpLprInfoGet.usLPRWidth;
	strLPRParam.usLPRHeight = strEpLprInfoGet.usLPRHeight;

	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_EP_LPR, strLPRParam);
	//sem_post(&sem_dsp);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set LPR ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_ERROR("%s %d set LPR ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}
	return 0;
}

int dspEPRedLightDetectTypeGet(unsigned char ucChannelNum)
{
	int ret = 0;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPVehicleConf;
	memset(&strEPVehicleConf, 0, sizeof(strEPVehicleConf));
	ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPVehicleConf, ucChannelNum);
	if (0 == ret)
	{
		return strEPVehicleConf.usRedLDetectType;
	}
	else
	{
		DSP_ERROR("dspEPRedLightDetectTypeGet get e_ELEC_POLICE_VEHICLE_CONF error\n");
	}
	return -1;
}

int dspSetVLStepConf(vlep_offset_param* param)
{
	int ret = 0;
	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_EPVL_OFFSET, param);
	if (ret != 0)
	{
		DSP_ERROR("%s %d call NET_DEV_SET_EPVL_OFFSET failed.lane:%d, error:%d\n", __FUNCTION__, __LINE__, param->lane_id, NET_DVE_GetLastError());
	}
	else
	{
		DSP_ERROR("%s %d ok.lane:%d\n", __FUNCTION__, __LINE__, param->lane_id);
	}
	//sem_post(&sem_dsp);
	return ret;
}

int dspEpVLoopStepConf(void)
{
	int iRet = 0;
	vlep_offset_param strDspVLoopStepParam;
	EP_V_LOOP_STEP__INFO strEpVLoopStepCfg;
	memset(&strEpVLoopStepCfg, 0, sizeof(strEpVLoopStepCfg));

	iRet = SysConfGet(e_EP_V_LOOP_STEP_INVER, &strEpVLoopStepCfg, 0);
	if (0 == iRet)
	{
		strDspVLoopStepParam.chan_id = 0;
		strDspVLoopStepParam.lane_id = 0;
		strDspVLoopStepParam.step0 = strEpVLoopStepCfg.uiLane0Step01;
		strDspVLoopStepParam.step2 = strEpVLoopStepCfg.uiLane0Step12;

		if (strDspVLoopStepParam.lane_id >= g_uiClientLaneSum)
		{
			return 0;
		}
		dspSetVLStepConf(&strDspVLoopStepParam);

		strDspVLoopStepParam.lane_id = 1;
		strDspVLoopStepParam.step0 = strEpVLoopStepCfg.uiLane1Step01;
		strDspVLoopStepParam.step2 = strEpVLoopStepCfg.uiLane1Step12;
		if (strDspVLoopStepParam.lane_id >= g_uiClientLaneSum)
		{
			return 0;
		}
		dspSetVLStepConf(&strDspVLoopStepParam);

		strDspVLoopStepParam.lane_id = 2;
		strDspVLoopStepParam.step0 = strEpVLoopStepCfg.uiLane2Step01;
		strDspVLoopStepParam.step2 = strEpVLoopStepCfg.uiLane2Step12;
		if (strDspVLoopStepParam.lane_id >= g_uiClientLaneSum)
		{
			return 0;
		}
		dspSetVLStepConf(&strDspVLoopStepParam);

		strDspVLoopStepParam.lane_id = 3;
		strDspVLoopStepParam.step0 = strEpVLoopStepCfg.uiLane3Step01;
		strDspVLoopStepParam.step2 = strEpVLoopStepCfg.uiLane3Step12;
		if (strDspVLoopStepParam.lane_id >= g_uiClientLaneSum)
		{
			return 0;
		}
		dspSetVLStepConf(&strDspVLoopStepParam);

	}
	return 0;
}

/* 电警相关功能使能配置 */
int dspEpFuncEn(char cMode, unsigned int uiLane)
{
	unsigned int i = 0;
	int ret = 0;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPLaneInfo;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGInfo;
	SYS_FUNC_ENABLE_INFO strSysFuncCfg;
	ep_assist_detect_param strEpAssistDecParam;
	int iRedLightParam = 0;

	memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
	memset(&strEPGInfo, 0, sizeof(strEPGInfo));
	memset(&strEpAssistDecParam, 0, sizeof(strEpAssistDecParam));

	SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGInfo, 0);
	//sem_wait(&sem_dsp);
	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		if (SINGAL_LANE_SET_MODE == cMode)
		{
			i = uiLane;
		}
		memset(&strEPLaneInfo, 0, sizeof(strEPLaneInfo));
		ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneInfo, i);
		if (0 == ret)
		{
#if defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
#else
			JPG_OSD_COLOR_PARAM param;
			memset(&param, 0, sizeof(param));
			param.lane_id = i;
			param.color = strEPGInfo.cOSDColour;
			CALL_CYC_CMD(NET_DEV_SET_JPG_OSD_COLOR, param);
#endif
			{
				EP_CARTRACK_PARAM param;
				memset(&param, 0, sizeof(param));
				param.lane_id = i;
				param.bEnableFlag = strEPLaneInfo.cCarTrackEn;
				CALL_CYC_CMD(NET_DEV_SET_CARTRACK_ENABLE, param);
			}
			{
				SNAP_DIRECTION_PARAM param;
				memset(&param, 0, sizeof(param));
				param.chan_id = 0;
				param.lane_id = i;
				param.direction = strEPLaneInfo.cCarSnapDir;
				CALL_CYC_CMD(NET_DEV_SET_SNAP_DIRECTION, param);
			}
		}
		else
		{
			DSP_ERROR("dspEpFuncEn get %d_chan e_ELEC_POLICE_VEHICLE_CONF error\n", i);
		}
		if (SINGAL_LANE_SET_MODE == cMode)
		{
			break;
		}

	}

	//iRedLightParam = strEPGInfo.cRedModifyMode;
	//iRedLightParam <<= 24;
	//iRedLightParam |= strEPGInfo.cRLAmendLevel;
	//CALL_CYC_CMD(NET_DEV_SET_REVISE_REDLIGHT_DEGREE, iRedLightParam);

	ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	if (0 == ret)
	{
		strEpAssistDecParam.uiLane = 0;
		strEpAssistDecParam.uiFrmDiffEnable = strSysFuncCfg.ucDspFramDifEn;
		strEpAssistDecParam.uiStopDistance  = strSysFuncCfg.ucDspStopDistan;
		strEpAssistDecParam.ucOneLampEnable = strSysFuncCfg.ucDspOneLampEn;
		strEpAssistDecParam.ucRedToGreenSnap = strSysFuncCfg.ucRedTurnGreenSnapEn;
		strEpAssistDecParam.ucNoDirectionSnap = strSysFuncCfg.ucNoDirReportSnapEn;
		strEpAssistDecParam.ucEPoliceDebug = strSysFuncCfg.ucArithDebugShowFlag;
		strEpAssistDecParam.ucRideLineSnap = strSysFuncCfg.ucPressLineSanpFlag;
		strEpAssistDecParam.ucReverseSnap  = strSysFuncCfg.ucBackWordsSanpFlag;
		strEpAssistDecParam.ucMainPicStep  = strSysFuncCfg.ucEpPpSnap0Or1;
		strEpAssistDecParam.ucOsdSpeed     = strSysFuncCfg.ucEpCarSpeedOsdEn;
		strEpAssistDecParam.usRedLReviseShutter = strSysFuncCfg.usRedLReviseShutter;
		strEpAssistDecParam.ucKakowPicMode = strSysFuncCfg.ucEpPpPicMode;
		strEpAssistDecParam.ucPlateSegFlag = strSysFuncCfg.ucPlateSegFlag;
		strEpAssistDecParam.ucPicQuality = strSysFuncCfg.ucPicQuality;
		strEpAssistDecParam.ucRedLReviseDegree = strSysFuncCfg.ucRedLReviseDegree;
		strEpAssistDecParam.ucChangeRoad = strSysFuncCfg.ucChangeRoad;
		strEpAssistDecParam.ucPicAssistDebug = strSysFuncCfg.ucPicAssistDebug;
		strEpAssistDecParam.ucCrossLineParkEn = strSysFuncCfg.ucCrossLineParkEn;


		memcpy(strEpAssistDecParam.cLaneChgType, strSysFuncCfg.cLaneChgType, sizeof(strSysFuncCfg.cLaneChgType));
		memcpy(strEpAssistDecParam.aucPressLine, strSysFuncCfg.aucPressLine, sizeof(strSysFuncCfg.aucPressLine));

		for (i = 0; i < g_uiClientLaneSum; i++)
		{
			memset(&strEPLaneInfo, 0, sizeof(strEPLaneInfo));
			ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneInfo, i);
			if (0 == ret)
			{
				strEpAssistDecParam.ucKakouSnapDelay[i] = strEPLaneInfo.ucPPDelaySnapFrames;
			}
			else
			{
				DSP_ERROR("dspEpFuncEn get %d_chan e_ELEC_POLICE_VEHICLE_CONF error\n", i);
			}
		}
		strEpAssistDecParam.ucKakouFlash = strEPGInfo.cPPFlashEn;

		ret = CALL_CYC_CMD(NET_DEV_SET_EPVL_LANE_ASSIST_DETECT_PARAM, strEpAssistDecParam);
		if (ret != 0)
		{
			SYSHANDLE_ERROR("call NET_DEV_SET_EPVL_LANE_ASSIST_DETECT_PARAM failed, error=%d\n", NET_DVE_GetLastError());
		}
		else
		{
			SYSHANDLE_INFO("call NET_DEV_SET_EPVL_LANE_ASSIST_DETECT_PARAM ok!\n");
		}
	}
	else
	{
		SYSHANDLE_INFO("get dsp func framDiffEn flag error!\n");
		return ret;
	}
	//sem_post(&sem_dsp);

	return ret;
}


/* 电警虚拟线圈设置 */
int dspEPVLAreaSet(char cMode, unsigned int uiLane)
{
	unsigned int i = 0;
	int ret = 0;
	AREARECT strAreaRect;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPLaneInfo;
	memset(&strAreaRect, 0, sizeof(strAreaRect));

	//sem_wait(&sem_dsp);
	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		if (SINGAL_LANE_SET_MODE == cMode)
		{
			i = uiLane;
		}
		memset(&strEPLaneInfo, 0, sizeof(strEPLaneInfo));
		ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneInfo, i);
		if (0 == ret)
		{
			strAreaRect.Up0.X = strEPLaneInfo.strVLPoint[0].usVLX1;
			strAreaRect.Up0.Y = strEPLaneInfo.strVLPoint[0].usVLY1;

			strAreaRect.Up1.X = strEPLaneInfo.strVLPoint[1].usVLX1;
			strAreaRect.Up1.Y = strEPLaneInfo.strVLPoint[1].usVLY1;

			strAreaRect.Bottom0.X = strEPLaneInfo.strVLPoint[2].usVLX1;
			strAreaRect.Bottom0.Y = strEPLaneInfo.strVLPoint[2].usVLY1;

			strAreaRect.Bottom1.X = strEPLaneInfo.strVLPoint[3].usVLX1;
			strAreaRect.Bottom1.Y = strEPLaneInfo.strVLPoint[3].usVLY1;

			strAreaRect.Leave0.X = strEPLaneInfo.strNoLaneVLPoint[0].usVLX1;
			strAreaRect.Leave0.Y = strEPLaneInfo.strNoLaneVLPoint[0].usVLY1;

			strAreaRect.Leave1.X = strEPLaneInfo.strNoLaneVLPoint[1].usVLX1;
			strAreaRect.Leave1.Y = strEPLaneInfo.strNoLaneVLPoint[1].usVLY1;

			AREARECT param;
			memset(&param, 0, sizeof(param));
			param.chan_id = i;
			param.bShowFlag = strEPLaneInfo.cVLDispFlag;
			param.snapIntv = strEPLaneInfo.cFrameInterval;
			param.snapType = dspEpSnapTypeGet(i);
			param.Up0 = strAreaRect.Up0;
			param.Up1 = strAreaRect.Up1;
			param.Bottom0 = strAreaRect.Bottom0;
			param.Bottom1 = strAreaRect.Bottom1;
			param.Leave0 = strAreaRect.Leave0;
			param.Leave1 = strAreaRect.Leave1;

			if (g_usEpXcsProVer == XCS_PRO_EPG_VER_2) {}
			ret = CALL_CYC_CMD(NET_DEV_SET_EP_VIRTUAL_LOOP, param);
		}
		else
		{
			DSP_ERROR("dspEPVLAreaSet get %d_chan e_ELEC_POLICE_VEHICLE_CONF error\n", i);
		}

		if (SINGAL_LANE_SET_MODE == cMode)
		{
			break;
		}
	}
	//sem_post(&sem_dsp);
	return ret;
}

int dspEPRLDecModeSet(void)
{
	int ret = 0;

	ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;

	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if (0 == ret)
	{
		g_iRLDecType = strEPGlobalConf.cRLDectType;
	}
	return ret;
}

#define ReadLightsCount(info) 	(info & 0xF)
#define ReadLightsDir(info) 	((info >> 4)  & 0xF)
#define ReadLightColor(info) 	(info & 0xF)
#define ReadLightType(info) 	((info >> 4)  & 0xF)
//#define ReadLightTimeout(info) 	((info >> 8)  & 0xF)
#define ReadLightDir(info) 		((info >> 12) & 0xF)

#define ReadLightsTimeout(info) ((info >> 8) & 0xF) // 读黄灯时间
#define ReadLightSensorId(info) ((info >> 8) & 0xF) // 读红灯检器号

int dspEPRedLightAreaSet(char cMode, char cDecFlag)
{
	int i = 0;
	int j = 0;
	int ret = 0;
	unsigned char ucYellowLTime =0;
	TL_TrcLgtsGrpsS param;
	unsigned char ucTmpData =0;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;

    SYS_FUNC_ENABLE_INFO strSysFuncCfg;

    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);

	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if (0 == ret)
	{
		memset(&param, 0, sizeof(param));
		param.bShowRect = strEPGlobalConf.cLightAmendDisFlag;
		param.ucSglUser = strEPGlobalConf.cRLDectType;

		/* 红灯检测方式有变化，需要通知dsp清空红灯状态 */
		if (g_iRLDecType != strEPGlobalConf.cRLDectType)
		{
			/* 通知交通灯管理线程，需要把红灯状态全部清空 */
			redLightDecTypeChange(0, 0, 0);
		}

		/* 若车检器工作状态有变化则红灯检测也要做相应转变 */
		if ((RL_DYNA_SET_MODE == cMode) && (g_EPLaneSwitchInfo[0].cCoilSwitchFlag > 0))
		{
			g_iRLDecType = cDecFlag;
		}
		else
		{
			g_iRLDecType = strEPGlobalConf.cRLDectType;
		}

		if (RED_DETECT_VIDEO == g_iRLDecType)
		{
			param.enable = 1;
			param.ucUsedVDRL =0;
		}
		else
		{
			param.enable = 0;
			param.ucUsedVDRL =1;
		}


		param.cEraseHaloEnable = strSysFuncCfg.cHaloTakeOutEn;
		param.cEraseHaloPixels = strSysFuncCfg.cHaloAreaRange;
		param.ucRLRevise = strEPGlobalConf.cLightAmendEn;
		param.ucRLStrobe = strEPGlobalConf.strRedLightMode[0].ucLightStrobeEn;
		param.ucRLSFrmNum = strEPGlobalConf.strRedLightMode[0].ucLightStrobeFrame;

		param.cRedModifyMode = strEPGlobalConf.cRedModifyMode;
		param.cRLAmendLevel  = strEPGlobalConf.cRLAmendLevel;

		DSP_INFO("%s %d: enable:%d,rlRevise:%d,rlStrobe:%d,rlSFrmNum:%d,ShowRect:%d,ucSglUser:%d,ucUsedVDRL:%d.\n",\
		__FUNCTION__,__LINE__,param.enable,param.ucRLRevise,param.ucRLStrobe,\
		param.ucRLSFrmNum,param.bShowRect,param.ucSglUser,param.ucUsedVDRL);

		DSP_INFO("%s %d: cEraseHaloEnable:%d,cEraseHaloPixels:%d,cRedModifyMode:%d,cRLAmendLevel:%d\n",\
		__FUNCTION__,__LINE__,param.cEraseHaloEnable,param.cEraseHaloPixels,param.cRedModifyMode,param.cRLAmendLevel);

		int nGroupCount = 0;
		for (j = 0; j < MAX_AREA_GROUP_NUM; j++)
		{
			STR_SYS_AREA& area = strEPGlobalConf.strRedArea[j][0].strArea;
			if (area.usAreaHeight == 0 || area.usAreaWidth == 0)
			{
				param.cTLParamSetEnable =0;
				DSP_ERROR("%s %d: group:%d area is 0,break!\n", __FUNCTION__, __LINE__, j);
				break;
			}
			else
			{
				DSP_ERROR("%s %d: group:%d area is not 0,continue!\n", __FUNCTION__, __LINE__, j);
				param.cTLParamSetEnable =1;
			}

			++nGroupCount;
			TL_TrcGrpS& lightsSetting = param.astTrcGrp[j];
			int lightsInfo = strEPGlobalConf.strRedArea[j][1].strArea.usAreaPosX;
			lightsSetting.ucLgtNums = ReadLightsCount(lightsInfo);
			lightsSetting.enGrpDir = (TL_GrpDirE)ReadLightsDir(lightsInfo);
			lightsSetting.stGrpTopLftPos.usPointX = area.usAreaPosX;
			lightsSetting.stGrpTopLftPos.usPointY = area.usAreaPosY;
			lightsSetting.stGrpBtmRgtPos.usPointX = area.usAreaPosX + area.usAreaWidth;
			lightsSetting.stGrpBtmRgtPos.usPointY = area.usAreaPosY + area.usAreaHeight;

			DSP_INFO("%s %d: ucLgtNums:%d,enGrpDir:%d,TopLftX:%d,TopLftY:%d,BtmRgtX:%d,BtmRgtY:%d.\n",\
					__FUNCTION__,__LINE__,lightsSetting.ucLgtNums,lightsSetting.enGrpDir,\
					lightsSetting.stGrpTopLftPos.usPointX,\
					lightsSetting.stGrpTopLftPos.usPointY,\
					lightsSetting.stGrpBtmRgtPos.usPointX,\
					lightsSetting.stGrpBtmRgtPos.usPointY);

			ucYellowLTime = ReadLightsTimeout(lightsInfo);
		
			unsigned short* pLightInfo = (unsigned short*)(&strEPGlobalConf.strRedArea[j][2].strArea);
			for (int iLgt = 0; iLgt < lightsSetting.ucLgtNums; ++iLgt)
			{
				TL_TrcLgtS& lgtSetting = lightsSetting.astTrcLgt[iLgt];
				
				ucTmpData = ReadLightColor(pLightInfo[iLgt]);
				lgtSetting.ucLgtClr = ucTmpData&0x07;
				if( ucTmpData & 0x08 )
				{
					lgtSetting.ucLgtClr |= 0x80;
				}

				lgtSetting.ucMapLgtNO 	= ReadLightSensorId(pLightInfo[iLgt]);
				lgtSetting.ucLgtShp = ReadLightType(pLightInfo[iLgt]);
				lgtSetting.ucLgtYelTime = ucYellowLTime;
				lgtSetting.ucLgtOrt = ReadLightDir(pLightInfo[iLgt]);

				DSP_INFO("%s %d: ltNum:%d,ucLgtClr:%d,ucMapLgtNO:%d,ucLgtShp:%d,ucLgtYelTime:%d,ucLgtOrt:%d.\n",\
					__FUNCTION__,__LINE__,iLgt,lgtSetting.ucLgtClr,lgtSetting.ucMapLgtNO,lgtSetting.ucLgtShp,\
					lgtSetting.ucLgtYelTime, lgtSetting.ucLgtOrt);
			}
		}
		param.ucGrpNums = nGroupCount;
		DSP_INFO("%s %d: RedLight Group Count: %d\n", __FUNCTION__,__LINE__, param.ucGrpNums);
		ret = CALL_CYC_CMD(NET_DEV_SET_TLGRPS_CFG, param);
	}
	else
	{
		DSP_ERROR("##SysConfGet error\n");
	}
	//sem_post(&sem_dsp);
	return ret;
}

/* 不按车道方向行驶配置功能 */

int dspEpSetRunNotLaneDir(int iLane, int iMode, int iDir)
{
	int ret = 0;
	lane_attribut_info_t strLaneNoDir;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;

	memset(&strLaneNoDir, 0, sizeof(strLaneNoDir));
	strLaneNoDir.lane = iLane;
	switch (iMode)
	{
	case NORMAL_DIR_SET:
		strLaneNoDir.direction = iDir;
		break;
	case CHANGE_DIR_SET:
		memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
		ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, iLane);

		strLaneNoDir.direction = strEPoliceConf.cCarNoDir;

		/* 可变车道处理不按车道方向行驶配置 */
		if (strEPoliceConf.cCarNoDir & EP_FORBID_DIR_LEFT)
		{
			if (iDir &  LANE_DIR_LEFT)
			{
				strLaneNoDir.direction &= ~EP_FORBID_DIR_LEFT;
			}
		}

		if (strEPoliceConf.cCarNoDir & EP_FORBID_DIR_STRAIGHT)
		{
			if (iDir &  LANE_DIR_STRAIGHT)
			{
				strLaneNoDir.direction &= ~EP_FORBID_DIR_STRAIGHT;
			}
		}
		break;
	default:

		break;
	}



	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_LANE_FORBID_ATTRIBUTE, strLaneNoDir);
	if (0 != ret)
	{
		DSP_ERROR("%s %d call NET_DEV_SET_LANE_FORBID_ATTRIBUTE failed, error code: %d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
	}
	else
	{
		DSP_INFO("%s %d  lane:%d,in dir:%d,out dir:%d.\n", __FUNCTION__, __LINE__, iLane, iDir, strLaneNoDir.direction);
	}
	//sem_post(&sem_dsp);
	return 0;
}

/* 电警车牌定位线圈区域设置 */
int dspEPLaneAreaSet(char cMode, unsigned int uiLane)
{
	unsigned int i = 0;
	int ret = 0;
	VLParam vlparam;
	EPVLFeature strVLFeature;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;
	memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
	memset(&vlparam, 0, sizeof(vlparam));

	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		if (SINGAL_LANE_SET_MODE ==  cMode)
		{
			i = uiLane;
		}

		ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, i);
		if (0 == ret)
		{
			vlparam.startX = strEPoliceConf.strRoadArea.usAreaPosX;
			vlparam.startY = strEPoliceConf.strRoadArea.usAreaPosY;
			vlparam.width = strEPoliceConf.strRoadArea.usAreaWidth;
			vlparam.height = strEPoliceConf.strRoadArea.usAreaHeight;

			// set area of snap
			ret = dspSetSnapArea(i, vlparam.startX, vlparam.startY, vlparam.width, vlparam.height, strEPoliceConf.cLaneLoopDispFlag);
			if (0 != ret)
			{
				DSP_ERROR("%s %d  dspSetSnapArea ret=%d error .\n", __FUNCTION__, __LINE__, ret);
			}

			dspEpSetRunNotLaneDir(i, NORMAL_DIR_SET, strEPoliceConf.cCarNoDir);

			memset(&strVLFeature, 0, sizeof(strVLFeature));
			strVLFeature.lane = i;
			strVLFeature.dayToNightThr = strEPoliceConf.ucCorrEveningThresh;
			strVLFeature.minPlateWidth = strEPoliceConf.ucMinPlatePixelSize;
			strVLFeature.maxPlateWidth = strEPoliceConf.ucMaxPlatePixelSize;
			//sem_wait(&sem_dsp);
			ret = CALL_CYC_CMD(NET_DEV_SET_EPVL_FEATURE, strVLFeature);
			if (0 != ret)
			{
				DSP_ERROR("%s %d  call NET_DEV_SET_EPVL_FEATURE failed, error:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
			}
			//sem_post(&sem_dsp);

		}

		if (SINGAL_LANE_SET_MODE ==  cMode)
		{
			break;
		}
	}
	return ret;
}

/* 电警红灯信息设置 */
int dspEPRedLightSet(char cMode, unsigned int uiLane)
{
	int ret = 0;
	unsigned int i = 0;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;

	/* 通知交通灯管理线程，需要把红灯状态全部清空 */
	redLightDecTypeChange(0, 0, 0);

	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		if (SINGAL_LANE_SET_MODE == cMode)
		{
			i = uiLane;
		}

		memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
		ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, i);
		if (0 == ret)
		{
			redLightStaForceSet(i, strEPoliceConf.uiForcedRedLight);
			ret = redLightStaForceDspSet(i);
			if (0 != ret)
			{
				DSP_ERROR("%s %d ret=%d error .\n", __FUNCTION__, __LINE__, ret);
			}
			redLightChangeInformDsp(i, cMode);
		}

		if (SINGAL_LANE_SET_MODE == cMode)
		{
			break;
		}
	}
	return ret;
}

int dspEPSnapOsdSet(char cMode, unsigned int uiLane)
{
	unsigned int i = 0;
	int ret = 0;
	NameParam osdParam;
	NameParam strParam;
	unsigned int uiGLaneNameLen = 0;
	unsigned int uiLaneNameLen = 0;

	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
	memset(&strEPGlobalConf, 0, sizeof(strEPGlobalConf));
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if (0 == ret)
	{
		shell_print("%s %d \n",__FUNCTION__,__LINE__);
		memset(&strParam, 0, sizeof(strParam));
		strcpy(strParam.name, strEPGlobalConf.szCarRoadName);
		dspSetJpgOsdString(0, uiLane, EP_OSD_ROAD_NAME_SET, &strParam);
		
		shell_print("%s %d \n",__FUNCTION__,__LINE__);
		memset(&strParam, 0, sizeof(strParam));
		strcpy(strParam.name, strEPGlobalConf.szDirection);
		dspSetJpgOsdString(0, uiLane, EP_OSD_ROAD_DIR_SET, &strParam);

		shell_print("%s %d \n",__FUNCTION__,__LINE__);
		memset(&strParam, 0, sizeof(strParam));
		strcpy(strParam.name, strEPGlobalConf.szRemarks);
		dspSetJpgOsdString(0, uiLane, EP_OSD_ROAD_DESP_SET, &strParam);

		shell_print("%s %d \n",__FUNCTION__,__LINE__);
		for (i = 0; i < g_uiClientLaneSum; i++)
		{
			if (SINGAL_LANE_SET_MODE == cMode)
			{
				i = uiLane;
			}

			memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
			ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, i);
			if (0 == ret)
			{
				memset(&osdParam, 0, sizeof(osdParam));
				memset(&osdParam.name, 0, sizeof(osdParam.name));

				osdParam.startX = strEPoliceConf.usOSDPosX;
				osdParam.startY = strEPoliceConf.usOSDPosY;
				//osdParam.width = 8*strlen(strEPoliceConf.szOSDContent);
				osdParam.height = 16;
				uiGLaneNameLen = strlen(strEPGlobalConf.szCarRoadName);
				uiLaneNameLen = strlen(strEPGlobalConf.szDirection);
				strncpy(osdParam.name, strEPGlobalConf.szCarRoadName, uiGLaneNameLen);
				strncpy(osdParam.name + uiGLaneNameLen, strEPGlobalConf.szDirection, uiLaneNameLen);
				strncpy(osdParam.name + uiGLaneNameLen + uiLaneNameLen, strEPoliceConf.szOSDContent, strlen(strEPoliceConf.szOSDContent));
				ret = dspEpSetSnapOsd(0, strEPoliceConf.uiCarRoadId, &osdParam);

				memset(&strParam, 0, sizeof(strParam));
				strcpy(strParam.name, strEPoliceConf.szOSDContent);
				dspSetJpgOsdString(0, i, EP_OSD_ROAD_LANE_SET, &strParam);

			}

			if (SINGAL_LANE_SET_MODE == cMode)
			{
				break;
			}
		}
	}
	else
	{
		DSP_ERROR("get ep global info error\n");
		return 0;
	}
	return ret;
}

int dspEpPpSnapOsdSet(char cMode, unsigned int uiLane)
{
	unsigned int i = 0;
	int ret = 0;
	NameParam osdParam;
	NameParam strParam;
	unsigned int uiGLaneNameLen = 0;
	unsigned int uiLaneNameLen = 0;

	ELEC_POLICE_VEHICLE_CONF_INFO strEPoliceConf;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	memset(&strOACfgInfoGet, 0, sizeof(strOACfgInfoGet));
	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		memset(&strParam, 0, sizeof(strParam));
		strcpy(strParam.name, strOACfgInfoGet.szPassPortName);
		dspSetJpgOsdString(0, uiLane, PP_OSD_ROAD_NAME_SET, &strParam);

		memset(&strParam, 0, sizeof(strParam));
		strcpy(strParam.name, strOACfgInfoGet.szDirection);
		dspSetJpgOsdString(0, uiLane, PP_OSD_ROAD_DIR_SET, &strParam);

		for (i = 0; i < g_uiClientLaneSum; i++)
		{
			if (SINGAL_LANE_SET_MODE == cMode)
			{
				i = uiLane;
			}

			memset(&strEPoliceConf, 0, sizeof(strEPoliceConf));
			ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPoliceConf, i);
			if (0 == ret)
			{
				memset(&osdParam, 0, sizeof(osdParam));
				memset(&osdParam.name, 0, sizeof(osdParam.name));

				osdParam.startX = strEPoliceConf.usOSDPosX;
				osdParam.startY = strEPoliceConf.usOSDPosY;
				//osdParam.width = 8*strlen(strEPoliceConf.szOSDContent);
				osdParam.height = 16;
				uiGLaneNameLen = strlen(strOACfgInfoGet.szPassPortName);
				uiLaneNameLen = strlen(strOACfgInfoGet.szDirection);
				strncpy(osdParam.name, strOACfgInfoGet.szPassPortName, uiGLaneNameLen);
				strncpy(osdParam.name + uiGLaneNameLen, strOACfgInfoGet.szDirection, uiLaneNameLen);
				strncpy(osdParam.name + uiGLaneNameLen + uiLaneNameLen, strEPoliceConf.szOSDContent, strlen(strEPoliceConf.szOSDContent));
				ret = dspSetSnapOsd(0, strEPoliceConf.uiCarRoadId, &osdParam);

				memset(&strParam, 0, sizeof(strParam));
				strcpy(strParam.name, strEPoliceConf.szOSDContent);
				dspSetJpgOsdString(0, i, PP_OSD_ROAD_LANE_SET, &strParam);

			}

			if (SINGAL_LANE_SET_MODE == cMode)
			{
				break;
			}
		}
	}
	else
	{
		DSP_ERROR("get pp global info error\n");
		return 0;
	}
	return ret;
}

/******************************************************************************
 * dspEpSnapTypeGet() :
 *
 * DESCRIPTION:  由于驱动内部定义图片合成方式宏与协议宏有差别，所以加一
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 *01a, 2010.01.12, wangbin Create
 * --------------------
 ******************************************************************************/
int dspEpSnapTypeGet(char cChanId)
{
	int ret = 0;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, cChanId);
	if (0 == ret)
	{
		switch (strEPGlobalConf.usPicStyle)
		{
		case PIC_STYLE_THREE:
			ret = SNAP_TYPE_3_PLUS_1;
			break;
		case PIC_STYLE_SIX:
			ret = SNAP_TYPE_6_PLUS_1;
			break;
		case PIC_SEPARATE_THREE:
			ret = SNAP_TYPE_3_SEPARATE;
			break;
		case PIC_STYLE_THREE_LEFT:
			ret = SNAP_TYPE_3_PLUS_1_LEFT;
			break;
		case PIC_SEPARATE_FOUR:
			ret = SNAP_TYPE_4_SEPARATE;
			break;
		default:
			break;
		}
		return ret;
	}
	else
	{
		DSP_ERROR("get ep snap pic type error\n");
		return 0;
	}
	return ret;
}

int dspEPCoilTypeGet(char cChanId)
{
	int ret = 0;
	ELEC_POLICE_VEHICLE_CONF_INFO strEPLaneConf;
	memset(&strEPLaneConf, 0, sizeof(strEPLaneConf));
	ret = SysConfGet(e_ELEC_POLICE_VEHICLE_CONF, &strEPLaneConf, cChanId);
	if (0 == ret)
	{
		/* 首先判断是否是虚拟线圈，若是还需要判断是否使能检测 */
		if (VIRTUAL_LOOP == strEPLaneConf.cDetectType)
		{
			if (strEPLaneConf.cVLDecEn == 0)
			{
				return DISABLE_LOOP_DET;
			}
			else
			{
				return strEPLaneConf.cDetectType;
			}
		}
		else if (GROUD_LOOP == strEPLaneConf.cDetectType)
		{
			/* 若是地感线圈则要判断是否已经切换到虚拟线圈了 */
			if ((strEPLaneConf.cCoilSwitchFlag == 1) && (strEPLaneConf.cCoilSwitchSta != SWITCH_DEFAULT_VAL))
			{
				return strEPLaneConf.cCoilSwitchSta;
			}
			else
			{
				return strEPLaneConf.cDetectType;
			}
		}
	}
	else
	{
		DSP_ERROR("get ep coil type error\n");
		return -1;
	}
	return ret;
}


int dspEPCoilTypeSet(char cMode, unsigned int uiLane)
{
	unsigned int i = 0;
	int ret = 0;

	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		if (SINGAL_LANE_SET_MODE == cMode)
		{
			i = uiLane;
		}

		Vehicle_Detect_type_t feature;
		ret = dspEPCoilTypeGet(i);
		if (GROUD_LOOP == ret)
		{
			feature = e_INNERVATION_LOOP;
			/* 触发监控程序发送网管信息 */
			g_aiCoilSta[i] = MODE_OK_STA;
			g_aiVirtualSta[i] = MODE_SHUTDOWN_STA;
			g_iErrorTimes = 201;
		}
		else if (VIRTUAL_LOOP == ret)
		{
			feature = e_VIRTUAL_LOOP;
			/* 触发监控程序发送网管信息 */
			g_aiCoilSta[i] = MODE_SHUTDOWN_STA;
			g_aiVirtualSta[i] = MODE_OK_STA;
			g_iErrorTimes = 201;
		}
		else if (DISABLE_LOOP_DET == ret)
		{
			feature = e_DISABLE_LOOP;
			/* 触发监控程序发送网管信息 */
			g_aiCoilSta[i] = MODE_SHUTDOWN_STA;
			g_aiVirtualSta[i] = MODE_SHUTDOWN_STA;
			g_iErrorTimes = 201;
		}
		else
		{
			DSP_ERROR("%s %d get vehicle_detect_type %d param invalid  .\n", __FUNCTION__, __LINE__, ret);
			return -1;
		}

		EP_VEHICLE_DETECT_TYPE evdt;
		memset(&evdt, 0, sizeof(evdt));
		evdt.lane_id = i;
		evdt.detect_type = feature;
		evdt.bayonetSnap = epPPPicSendFlag();

		//sem_wait(&sem_dsp);
		/* 交通灯由谁检测，现在统一由arm检测 */
		ret = CALL_CYC_CMD(NET_DEV_SET_EP_VI_DETECT_TYPE, evdt);
		//sem_post(&sem_dsp);
		if (0 != ret)
		{
			DSP_ERROR("%s %d call NET_DEV_SET_EP_VI_DETECT_TYPE failed, error:%d iChanel:%d,feature:%d.\n", __FUNCTION__, __LINE__\
					  , NET_DVE_GetLastError(), i, feature);
		}

		if (SINGAL_LANE_SET_MODE == cMode)
		{
			break;
		}
	}
	return ret;
}



/******************************************************************************
 * epTimeRangeJudge() :
 *
 * DESCRIPTION:  判断当前时间是若落在禁止行驶时间段内
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 *01a, 2010.12.13, wangbin Create
 * --------------------
 ******************************************************************************/
int epTimeRangeJudge(char cLaneId)
{
	int iForbidType = 0;
	int ret = 0;
	forbid_direct_param strForbidDirParam;
	memset(&strForbidDirParam, 0, sizeof(strForbidDirParam));
	strForbidDirParam.lane_id = cLaneId;

	EP_TIME_RANGE_INFO strEpTimeRange[EP_FORBID_MAX_DIR];

	ret = SysConfGet(e_EP_RANGE_TIME_CONF, strEpTimeRange, cLaneId);
	if (0 == ret)
	{
		for (iForbidType = 0; iForbidType < EP_FORBID_MAX_DIR; iForbidType++)
		{
			memset(&strForbidDirParam, 0, sizeof(strForbidDirParam));

			strForbidDirParam.groupId       =   iForbidType;
			strForbidDirParam.flag1         =   strEpTimeRange[iForbidType].cRange1Enable;
			strForbidDirParam.begin1_time   =   strEpTimeRange[iForbidType].iBeginTime1;
			strForbidDirParam.last1_time    =   strEpTimeRange[iForbidType].iLastTime1;

			strForbidDirParam.flag2         =   strEpTimeRange[iForbidType].cRange2Enable;
			strForbidDirParam.begin2_time   =   strEpTimeRange[iForbidType].iBeginTime2;
			strForbidDirParam.last2_time    =   strEpTimeRange[iForbidType].iLastTime2;

			strForbidDirParam.forbid_direct = strEpTimeRange[iForbidType].cForbidType;

			//sem_wait(&sem_dsp);
			ret = CALL_CYC_CMD(NET_DEV_SET_FORBID_DIRC, strForbidDirParam);
			if (0 != ret)
			{
				DSP_ERROR("lane:%d, call NET_DEV_SET_FORBID_DIRC failed. error code = %d\n", cLaneId, NET_DVE_GetLastError());
			}
			else
			{
				DSP_ERROR("lane:%d, call NET_DEV_SET_FORBID_DIRC ok. ret = %d\n", cLaneId, ret);
			}
			//sem_post(&sem_dsp);
		}
	}
	return 0;
}

/*
 * Function:
 * Description:
 * Calls:
 * Called By:
 * Table Accessed:
 * Table Updated:
 * Input:  获取检测周期(分钟为单位)
 * Output:
 * Return: 是否进行补光的切换
 * Others:
 */
int dspEPACSwitchParamGet(int* piDecCycle)
{
	int ret = 0;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if (0 == ret)
	{
		*piDecCycle = strEPGlobalConf.ucACDecCycle;
		ret = strEPGlobalConf.cACSyncSwitchEn;
	}
	return ret;
}

int dspEpACPhaseSwitchDec(char cMode)
{
	int ret = 0;
	char cGroup = 0;
	float fMaxGain = 0;
	float fMinGain = 0;
	static int s_iPreSta = 0;
	CAMERA_CTRL_CONF_INFO strCamInfo;
	float fGrayThreshold = 0;
	CUR_CAMERA_STATUS strCurCamSta;
	ELEC_POLICE_OVERALL_CONF_INFO strEPGlobalConf;

	memset(&strCurCamSta, 0, sizeof(CUR_CAMERA_STATUS));
	strCurCamSta.cbSize = sizeof(CUR_CAMERA_STATUS);

	if (CAM_PARAM_MODE_FORCE == cMode)
	{
		s_iPreSta = 0;
		return ret;
	}
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPGlobalConf, 0);
	if (0 == ret)
	{
		fGrayThreshold = (float)(strEPGlobalConf.ucACSwitchThreshold);
		ret = cameraRunStaParamGet(0, &strCurCamSta);

		cGroup = camTimeRangeJudge();
		/* 增益控制设置 */
		memset(&strCamInfo, 0, sizeof(strCamInfo));
		ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EGAIN + cGroup * MAX_CAM_CTRL_NUM);
		if (0 != ret)
		{
			DSP_ERROR("fillin get camara gain conf error\n");
			return -1;
		}
		else
		{
			if (FIX == strCamInfo.cCtlType)
			{
				fMaxGain = strCamInfo.fFixVal - 1;
				fMinGain = strCamInfo.fFixVal + 1;
			}
			else
			{
				fMaxGain = strCamInfo.fMaxVal - 6;
				if (fMaxGain < strCamInfo.fMinVal)
				{
					fMaxGain = strCamInfo.fMinVal;
				}

				fMinGain = strCamInfo.fMinVal + 2;
				if (fMinGain > fMaxGain)
				{
					fMinGain = fMaxGain;
				}
			}
		}


		DSP_INFO("ac sync phase,confMaxGain:%f,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
				 fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
		if ((strCurCamSta.fCurGain > fMaxGain) && (strCurCamSta.fCurGrey < fGrayThreshold))
		{
			if (s_iPreSta != FLASH_AC_DAY_TO_NIGHT)
			{
				s_iPreSta = FLASH_AC_DAY_TO_NIGHT;

				/* 调用相关接口进行AC交流同步的切换 */
				dctlEPFlashConf(s_iPreSta, 0);
				syslog(LOG_ERR | LOG_USER, "FLASH AC,day to night,preSta:%d,confMaxGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					   s_iPreSta, fMaxGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				DSP_INFO("FLASH_AC_DAY_TO_NIGHT is here.\n");
			}
			else
			{
				DSP_INFO("FLASH_AC_DAY_TO_NIGHT same is here.\n");
			}
		}
		else if ((strCurCamSta.fCurGain <= fMinGain) && (strCurCamSta.fCurGrey >= fGrayThreshold))
		{
			if (s_iPreSta != FLASH_AC_NIGHT_TO_DAY)
			{
				s_iPreSta = FLASH_AC_NIGHT_TO_DAY;

				/* 调用相关接口进行AC交流同步的切换 */
				dctlEPFlashConf(s_iPreSta, 0);

				syslog(LOG_ERR | LOG_USER, "FLASH AC,night to day,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					   s_iPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				DSP_INFO("FLASH_AC_NIGHT_TO_DAY is here.\n");
			}
			else
			{
				DSP_INFO("FLASH_AC_NIGHT_TO_DAY same is here.\n");
			}
		}
		else
		{
			syslog(LOG_ERR | LOG_USER, "warning,FLASH AC change:preSta:%d,confMaxGain:%f,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
				   s_iPreSta, fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
		}
	}
	return ret;

}


int dspEPIrregNameInfoSet(void)
{
	int i = 0;
	int ret = 0;
	char szEpDevCodeTmp[EXT_DEV_CODE_LEN + MAX_DEV_CODE_LEN];

	ELEC_POLICE_OVERALL_CONF_INFO strEPOverAllConf;
	DSP_EP_IRREG_INFO strDspIrregInfo;
	EP_IRREG_CONF_INFO strEPIrregConfGet;
	ret = SysConfGet(e_EP_IRREG_INFO_CONF, &strEPIrregConfGet, 0);
	if (0 != ret)
	{
		DSP_ERROR("read ep overall conf failed\n");
		return -1;
	}

	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strEPOverAllConf, 0);
	if (0 != ret)
	{
		SYSHANDLE_ERROR("read ep overall conf failed\n");
		return -1;
	}

	//sem_wait(&sem_dsp);

	strDspIrregInfo.iDispFlag = strEPIrregConfGet.cDispFlag;
	strDspIrregInfo.epIrregCode = e_RedCode;
	memcpy(strDspIrregInfo.szIrregCode, strEPIrregConfGet.szRunRedCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspIrregInfo.szIrregName, strEPIrregConfGet.szRunRedName, MAX_IRREG_NAME_LEN);
	ret = CALL_CYC_CMD(NET_DEV_SET_EP_IRREG_INFO, strDspIrregInfo);

	strDspIrregInfo.epIrregCode = e_ReverseCode;
	memcpy(strDspIrregInfo.szIrregCode, strEPIrregConfGet.szReverseRunCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspIrregInfo.szIrregName, strEPIrregConfGet.szReverseRunName, MAX_IRREG_NAME_LEN);
	ret += CALL_CYC_CMD(NET_DEV_SET_EP_IRREG_INFO, strDspIrregInfo);

	strDspIrregInfo.epIrregCode = e_InfringForbidLineCode;
	memcpy(strDspIrregInfo.szIrregCode, strEPIrregConfGet.szInfringForbidLineCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspIrregInfo.szIrregName, strEPIrregConfGet.szInfringForbidLineName, MAX_IRREG_NAME_LEN);
	ret += CALL_CYC_CMD(NET_DEV_SET_EP_IRREG_INFO, strDspIrregInfo);

	strDspIrregInfo.epIrregCode = e_InfringDirLaneRunCode;
	memcpy(strDspIrregInfo.szIrregCode, strEPIrregConfGet.szInfringDirLaneRunCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspIrregInfo.szIrregName, strEPIrregConfGet.szInfringDirLaneRunName, MAX_IRREG_NAME_LEN);
	ret += CALL_CYC_CMD(NET_DEV_SET_EP_IRREG_INFO, strDspIrregInfo);

	strDspIrregInfo.epIrregCode = e_StopOnTrafficNoEntryRunCode;
	memcpy(strDspIrregInfo.szIrregCode, strEPIrregConfGet.szStopOnTrafficNoEntryCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspIrregInfo.szIrregName, strEPIrregConfGet.szStopOnTrafficNoEntryName, MAX_IRREG_NAME_LEN);
	ret += CALL_CYC_CMD(NET_DEV_SET_EP_IRREG_INFO, strDspIrregInfo);

	strDspIrregInfo.epIrregCode = e_dspHighwayInfringDirLaneCode;
	memcpy(strDspIrregInfo.szIrregCode, strEPIrregConfGet.szHighwayInfringDirLaneCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspIrregInfo.szIrregName, strEPIrregConfGet.szHighwayInfringDirLaneName, MAX_IRREG_NAME_LEN);
	ret += CALL_CYC_CMD(NET_DEV_SET_EP_IRREG_INFO, strDspIrregInfo);

	memset(szEpDevCodeTmp, 0, sizeof(szEpDevCodeTmp));
	strcat(szEpDevCodeTmp, strEPOverAllConf.szEpDevCode);
	szEpDevCodeTmp[MAX_DEV_CODE_LEN - 1] = 0;

	strcat(szEpDevCodeTmp, strEPOverAllConf.szExEpDevCode);
	szEpDevCodeTmp[MAX_DEV_CODE_LEN + EXT_DEV_CODE_LEN - 1] = 0;

	for (i = 0; i < FTP_MAX_DEV_CODE_LEN; i++)
	{
		if (szEpDevCodeTmp[i] > 0x80)
		{
			szEpDevCodeTmp[i] = 0x3f;
		}
	}

	ret = NET_CYC_SetDEVConfig(NET_DEV_SET_EP_SN, szEpDevCodeTmp, EXT_DEV_CODE_LEN + MAX_DEV_CODE_LEN);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set sn name ret=%d error.len:%d.\n", __FUNCTION__, __LINE__, ret, strlen(szEpDevCodeTmp));
	}
	else
	{
		DSP_ERROR("%s %d set sn name ret=%d OK.len:%d.\n", __FUNCTION__, __LINE__, ret, strlen(szEpDevCodeTmp));
	}

	//sem_post(&sem_dsp);

	if (0 != ret)
	{
		DSP_ERROR("%s %d set irreg name ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_ERROR("%s %d set irreg name ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}

	return ret;
}

int dspEPSOTNoEntrySet(char cMode, unsigned int uiLane)
{
	int ret = -1;
	unsigned int i = 0;
	DSP_SOT_NO_ENTRY_INFO strSOTNoEntry;
	memset(&strSOTNoEntry, 0, sizeof(strSOTNoEntry));
	EP_STOP_ON_TRAFFIC_NO_ENTRY strSOTNEConf;
	memset(&strSOTNEConf, 0, sizeof(strSOTNEConf));

	for (i = 0; i < g_uiClientLaneSum; i++)
	{
		if (SINGAL_LANE_SET_MODE == cMode)
		{
			i = uiLane;
		}

		ret = SysConfGet(e_EP_SOT_NOENTRY_CONF, &strSOTNEConf, i);
		if (0 == ret)
		{
			strSOTNoEntry.lane_id = i;
			strSOTNoEntry.cSOTNoEntryEnFlag = strSOTNEConf.cEnableFlag;

			strSOTNoEntry.iNEBeginTime1 = strSOTNEConf.iBeginTime1;
			strSOTNoEntry.iNELastTime1 = strSOTNEConf.iLastTime1;

			strSOTNoEntry.iNEBeginTime2 = strSOTNEConf.iBeginTime2;
			strSOTNoEntry.iNELastTime2 = strSOTNEConf.iLastTime2;

			strSOTNoEntry.iNEBeginTime3 = strSOTNEConf.iBeginTime3;
			strSOTNoEntry.iNELastTime3 = strSOTNEConf.iLastTime3;

			//sem_wait(&sem_dsp);
			ret = CALL_CYC_CMD(NET_DEV_SET_STOP_ON_NO_ENTRY, strSOTNoEntry);
			if (0 != ret)
			{
				DSP_ERROR("%s %d call NET_DEV_SET_STOP_ON_NO_ENTRY failed, error:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
			}
			else
			{
				DSP_ERROR("%s %d call NET_DEV_SET_STOP_ON_NO_ENTRY ok.\n", __FUNCTION__, __LINE__);
			}
			//sem_post(&sem_dsp);

		}
		else
		{
			DSP_ERROR("%s %d  get sot no entry failed,iLaneId:%d.\n", __FUNCTION__, __LINE__, i);
		}

		if (SINGAL_LANE_SET_MODE == cMode)
		{
			break;
		}
	}
	return ret;
}

int dspEPFILightSmartSwitchGet(void)
{
	int ret = 0;
	ELEC_POLICE_OVERALL_CONF_INFO strOACfgInfoGet;
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = strOACfgInfoGet.ucSmartSwitchEn;
	}
	return ret;
}

#define MAX_CHECK_SWITCH_SUM	2	/* 切换判断次数 */

int dspEpFillInLightSwitchDec(char cMode)
{
	int ret = 0;
	unsigned int uiLane = 0;
	char cGroup = 0;
	float fMaxGain = 0;
	float fMinGain = 0;
	CAMERA_CTRL_CONF_INFO strCamInfo;
	float	fGrayThreshold = (float)35;
	CUR_CAMERA_STATUS strCurCamSta;
	ELEC_POLICE_OVERALL_CONF_INFO strOACfgInfoGet;
	static unsigned char s_ucDayToNight = 0;
	static unsigned char s_ucNightToDay = 0;

	memset(&strCurCamSta, 0, sizeof(CUR_CAMERA_STATUS));
	strCurCamSta.cbSize = sizeof(CUR_CAMERA_STATUS);

	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = cameraRunStaParamGet(0, &strCurCamSta);
		if (strOACfgInfoGet.ucSmartSwitchEn > 0)
		{

			if (cMode == CAM_PARAM_MODE_MANUAL)
			{
				g_iEpPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				s_ucNightToDay = 0;
				s_ucDayToNight = 0;

				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行频闪灯到闪光灯的切换 */
					dctlEPFlashConf(g_iEpPreSta, uiLane);
				}
				syslog(LOG_ERR | LOG_USER, "ep smart switch,manual switch night to day!\n");
				DSP_INFO("ep smart switch,manual switch night to day!\n");
			}

			DSP_INFO("float val:%f.\n", fGrayThreshold);
			cGroup = camTimeRangeJudge();
			/* 增益控制设置 */
			memset(&strCamInfo, 0, sizeof(strCamInfo));
			ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EGAIN + cGroup * MAX_CAM_CTRL_NUM);
			if (0 != ret)
			{
				DSP_ERROR("ep smart get camara gain conf error\n");
				return -1;
			}
			else
			{
				if (FIX == strCamInfo.cCtlType)
				{
					fMaxGain = strCamInfo.fFixVal - 1;
					fMinGain = strCamInfo.fFixVal + 1;
				}
				else
				{
					fMaxGain = strCamInfo.fMaxVal - 4;
					if (fMaxGain < strCamInfo.fMinVal)
					{
						fMaxGain = strCamInfo.fMinVal;
					}

					fMinGain = strCamInfo.fMinVal + 4;
					if (fMinGain > fMaxGain)
					{
						fMinGain = fMaxGain;
					}
				}
			}

			DSP_INFO("ep smart switch,confMaxGain:%f,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					 fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);

			if ((strCurCamSta.fCurGain > fMaxGain) && (strCurCamSta.fCurGrey < fGrayThreshold))
			{
				s_ucNightToDay = 0;

				if (g_iEpPreSta != FLASH_FILLIN_DAY_TO_NIGHT)
				{
					s_ucDayToNight++;
				}

				if (cMode == CAM_PARAM_MODE_MANUAL)
				{
					/* 手动模式下需及时进行一次开灯切换操作，关灯可不需要，因为已经在默认状态下关过 */
					s_ucDayToNight = MAX_CHECK_SWITCH_SUM + 1;
				}

				if (s_ucDayToNight > MAX_CHECK_SWITCH_SUM)
				{
					g_iEpPreSta = FLASH_FILLIN_DAY_TO_NIGHT;
					s_ucDayToNight = 0;

					for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
					{
						/* 调用相关接口进行闪光灯到频闪灯的切换 */
						dctlEPFlashConf(g_iEpPreSta, uiLane);
					}
					DSP_INFO("ep smart switch Day_to_night!\n");
					syslog(LOG_ERR | LOG_USER, "ep smart switch day to night,preSta:%d,confMaxGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
						   g_iEpPreSta, fMaxGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				}
				else
				{
					DSP_INFO("ep smart switch Day_to_night check,time is %d!\n", s_ucDayToNight);
				}
			}
			else if ((strCurCamSta.fCurGain <= fMinGain) && (strCurCamSta.fCurGrey >= fGrayThreshold))
			{
				s_ucDayToNight = 0;

				if (g_iEpPreSta != FLASH_FILLIN_NIGHT_TO_DAY)
				{
					s_ucNightToDay++;
				}

				if (s_ucNightToDay > MAX_CHECK_SWITCH_SUM)
				{
					g_iEpPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
					for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
					{
						/* 调用相关接口进行频闪灯到闪光灯的切换 */
						dctlEPFlashConf(g_iEpPreSta, uiLane);
					}
					if (s_ucNightToDay % (MAX_CHECK_SWITCH_SUM + 1) == 0)
					{
						syslog(LOG_ERR | LOG_USER, "ep smart switch night to day,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
							   g_iEpPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
					}
					DSP_INFO("ep smart switch night_to_day is here!\n");

					s_ucNightToDay++;
				}
				else
				{
					DSP_INFO("ep smart switch, night_to_day check,time is %d!\n", s_ucNightToDay);
				}
			}
			else
			{
				s_ucNightToDay = 0;
				s_ucDayToNight = 0;
				/* syslog(LOG_ERR|LOG_USER,"warning:ep smart,can't switch,clean switch time.confMaxGain:%f,confMinGain:%f,curGain:%f;confGrey:%f,curGrey:%f.\n",\
				    fMaxGain,fMinGain,strCurCamSta.fCurGain,fGrayThreshold,strCurCamSta.fCurGrey); */
			}

		}
		else
		{

			if (strOACfgInfoGet.ucTimerSwitchEn <= 0)
			{
				g_iEpPreSta = 0;
				s_ucNightToDay = 0;
				s_ucDayToNight = 0;

				/* 切换回正常工作模式 */
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					dctlEPFlashConf(g_iEpPreSta, uiLane);
				}

				syslog(LOG_ERR | LOG_USER, "ep light cut to normal sta,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					   g_iEpPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				DSP_INFO("ep FillIn light cut to normal sta!\n");
			}
		}

		if (cMode == CAM_PARAM_MODE_FORCE)
		{
			g_iEpPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
			s_ucNightToDay = 0;
			s_ucDayToNight = 0;

			for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
			{
				/* 调用相关接口进行频闪灯到闪光灯的切换 */
				dctlEPFlashConf(g_iEpPreSta, uiLane);
			}
			syslog(LOG_ERR | LOG_USER, "ep force light cut night to day!\n");
			DSP_INFO("ep force FillIn light night_to_day!\n");
		}

	}
	return ret;

}

int dspEpTimerSwitchEnGet(void)
{
	int ret = 0;
	ELEC_POLICE_OVERALL_CONF_INFO strOACfgInfoGet;
	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = strOACfgInfoGet.ucTimerSwitchEn;
	}
	return ret;
}

#if defined(CYC200XZD)||defined(CYC500XZD)

int dspEpTimerSwitchDec(char cMode)
{
	int ret = 0;
	unsigned int uiLane = 0;
	ELEC_POLICE_OVERALL_CONF_INFO strOACfgInfoGet;
	int iDayToNight = 0;
	unsigned int iSeconds = 0;
	unsigned long ulPlanStart = time(NULL);
	struct tm tmPlanStart;
	ulPlanStart += 8 * 3600;

	static int g_iZebraLightSta = 0;

	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		if (strOACfgInfoGet.ucTimerSwitchEn > 0)
		{
			if (CAM_PARAM_MODE_MANUAL == cMode)
			{
				g_iZebraLightSta = 0;
				DSP_INFO("ep timer,manual conf and reset ligth switch sta to 0!\n");
				syslog(LOG_ERR | LOG_USER, "zebra ep timer,manual conf and reset ligth switch sta to 0!\n");
			}

			gmtime_r((time_t*)&ulPlanStart, &tmPlanStart);
			iSeconds = tmPlanStart.tm_hour * 3600 + tmPlanStart.tm_min * 60;

			if ((strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime) > DATE_24_HOUR)
			{
				if ((iSeconds >= strOACfgInfoGet.uiFillInLightBeginTime) && \
					(iSeconds <= DATE_24_HOUR))
				{
					iDayToNight = 1;
				}
				else if ((iSeconds >= 0) && \
						 (iSeconds <= ((strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime) - DATE_24_HOUR)))
				{
					iDayToNight = 1;
				}
			}
			else
			{
				if ((iSeconds >= strOACfgInfoGet.uiFillInLightBeginTime) && \
					(iSeconds <= (strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime)))
				{
					iDayToNight = 1;
				}
			}

			if ((g_iZebraLightSta != FLASH_FILLIN_DAY_TO_NIGHT) && (iDayToNight > 0))
			{
				g_iZebraLightSta = FLASH_FILLIN_DAY_TO_NIGHT;

				zebraCamStrobeFlashEnable();

				DSP_INFO("ep timer switch FillIn light Day_to_night is here!\n");
				syslog(LOG_ERR | LOG_USER, "zebra ep timer switch light cut day to night.\n");
			}

			if ((g_iZebraLightSta != FLASH_FILLIN_NIGHT_TO_DAY) && (iDayToNight <= 0))
			{
				g_iZebraLightSta = FLASH_FILLIN_NIGHT_TO_DAY;

				zebraCamStrobeFlashDisable();

				DSP_INFO("ep timer switch FillIn light night_to_Day is here!\n");
				syslog(LOG_ERR | LOG_USER, "zebra ep timer switch light cut night to day.\n");
			}
		}
		else
		{
			if (strOACfgInfoGet.ucSmartSwitchEn <= 0)
			{
				g_iZebraLightSta = 0;

				zebraCamStrobeFlashDisable();

				DSP_INFO("zebra ep timer switch cut to normal sta!\n");
				syslog(LOG_ERR | LOG_USER, "zebra ep timer switch cut to normal shut sta!\n");
			}
		}
	}
	return ret;

}

#else

int dspEpTimerSwitchDec(char cMode)
{
	int ret = 0;
	unsigned int uiLane = 0;
	ELEC_POLICE_OVERALL_CONF_INFO strOACfgInfoGet;
	int iDayToNight = 0;
	unsigned int iSeconds = 0;
	unsigned long ulPlanStart = time(NULL);
	struct tm tmPlanStart;
	ulPlanStart += 8 * 3600;

	ret = SysConfGet(e_ELEC_POLICE_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		if (strOACfgInfoGet.ucTimerSwitchEn > 0)
		{
			if (CAM_PARAM_MODE_MANUAL == cMode)
			{
				g_iEpPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行闪光灯到频闪灯的切换 */
					dctlEPFlashConf(g_iEpPreSta, uiLane);
				}

				DSP_INFO("ep timer,manual conf and force switch night_to_Day!\n");
				syslog(LOG_ERR | LOG_USER, "ep timer,manual conf and force switch night_to_Day!\n");
			}

			gmtime_r((time_t*)&ulPlanStart, &tmPlanStart);
			iSeconds = tmPlanStart.tm_hour * 3600 + tmPlanStart.tm_min * 60;

			if ((strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime) > DATE_24_HOUR)
			{
				if ((iSeconds >= strOACfgInfoGet.uiFillInLightBeginTime) && \
					(iSeconds <= DATE_24_HOUR))
				{
					iDayToNight = 1;
				}
				else if ((iSeconds >= 0) && \
						 (iSeconds <= ((strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime) - DATE_24_HOUR)))
				{
					iDayToNight = 1;
				}
			}
			else
			{
				if ((iSeconds >= strOACfgInfoGet.uiFillInLightBeginTime) && \
					(iSeconds <= (strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime)))
				{
					iDayToNight = 1;
				}
			}

			if ((g_iEpPreSta != FLASH_FILLIN_DAY_TO_NIGHT) && (iDayToNight > 0))
			{
				g_iEpPreSta = FLASH_FILLIN_DAY_TO_NIGHT;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行闪光灯到频闪灯的切换 */
					dctlEPFlashConf(g_iEpPreSta, uiLane);
				}

				DSP_INFO("ep timer switch FillIn light Day_to_night is here!\n");
				syslog(LOG_ERR | LOG_USER, "ep timer switch light cut day to night.\n");
			}

			if ((g_iEpPreSta != FLASH_FILLIN_NIGHT_TO_DAY) && (iDayToNight == 0))
			{
				g_iEpPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行闪光灯到频闪灯的切换 */
					dctlEPFlashConf(g_iEpPreSta, uiLane);
				}

				DSP_INFO("ep timer switch FillIn light night_to_Day is here!\n");
				syslog(LOG_ERR | LOG_USER, "ep timer switch light cut night to day.\n");
			}
		}
		else
		{
			if (strOACfgInfoGet.ucSmartSwitchEn <= 0)
			{
				g_iEpPreSta = 0;

				/* 切换回正常工作模式 */
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					dctlEPFlashConf(g_iEpPreSta, uiLane);
				}
				DSP_INFO("ep timer switch cut to normal sta!\n");
			}
		}
	}
	return ret;

}

#endif

/* 以下为卡口参数配置模块 */

int dspPpTimerSwitchEnGet(void)
{
	int ret = 0;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = strOACfgInfoGet.ucTimerSwitchEn;
	}
	return ret;
}

int dspPpTimerSwitchDec(char cMode)
{
	int ret = 0;
	unsigned int uiLane = 0;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	int iDayToNight = 0;
	unsigned int iSeconds = 0;
	unsigned long ulPlanStart = time(NULL);
	struct tm tmPlanStart;
	ulPlanStart += 8 * 3600;

	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		if (strOACfgInfoGet.ucTimerSwitchEn > 0)
		{
			if (CAM_PARAM_MODE_MANUAL == cMode)
			{
				g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行闪光灯到频闪灯的切换 */
					dctlPPFlashConf(g_iPreSta, uiLane);
				}

				DSP_INFO("pp timer,manual conf and force switch night_to_Day!\n");
				syslog(LOG_ERR | LOG_USER, "pp timer,manual conf and force switch night_to_Day!\n");
			}

			gmtime_r((time_t*)&ulPlanStart, &tmPlanStart);
			iSeconds = tmPlanStart.tm_hour * 3600 + tmPlanStart.tm_min * 60;

			if ((strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime) > DATE_24_HOUR)
			{
				if ((iSeconds >= strOACfgInfoGet.uiFillInLightBeginTime) && \
					(iSeconds <= DATE_24_HOUR))
				{
					iDayToNight = 1;
				}
				else if ((iSeconds >= 0) && \
						 (iSeconds <= ((strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime) - DATE_24_HOUR)))
				{
					iDayToNight = 1;
				}
			}
			else
			{
				if ((iSeconds >= strOACfgInfoGet.uiFillInLightBeginTime) && \
					(iSeconds <= (strOACfgInfoGet.uiFillInLightBeginTime + strOACfgInfoGet.uiFillInLightLastTime)))
				{
					iDayToNight = 1;
				}
			}

			if ((g_iPreSta != FLASH_FILLIN_DAY_TO_NIGHT) && (iDayToNight > 0))
			{
				g_iPreSta = FLASH_FILLIN_DAY_TO_NIGHT;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行闪光灯到频闪灯的切换 */
					dctlPPFlashConf(g_iPreSta, uiLane);
				}

				DSP_INFO("pp timer switch FillIn light Day_to_night is here!\n");
				syslog(LOG_ERR | LOG_USER, "pp timer switch light cut day to night.\n");
			}

			if ((g_iPreSta != FLASH_FILLIN_NIGHT_TO_DAY) && (iDayToNight == 0))
			{
				g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行闪光灯到频闪灯的切换 */
					dctlPPFlashConf(g_iPreSta, uiLane);
				}

				DSP_INFO("pp timer switch FillIn light night_to_Day is here!\n");
				syslog(LOG_ERR | LOG_USER, "pp timer switch light cut night to day.\n");
			}
		}
		else
		{
			if (strOACfgInfoGet.ucSmartSwitchEn <= 0)
			{
				g_iPreSta = 0;

				/* 切换回正常工作模式 */
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					dctlPPFlashConf(g_iPreSta, uiLane);
				}
				DSP_INFO("pp timer switch cut to normal sta!\n");
			}
		}
	}
	return ret;

}

unsigned short dspPPSnapInterValGet(int iChanId, int iLaneId)
{
	int ret = 0;
	PORT_VEHICLE_CONF_INFO strPPortConf;
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		return strPPortConf.usSnapPicInter;
	}
	else
	{
		DSP_ERROR("pp snap interval get failed,return 75.\n");
		return 75;
	}
}

unsigned short dspPPLimitSpeedGet(int iChanId, int iLaneId)
{
	int ret = 0;
	PORT_VEHICLE_CONF_INFO strPPortConf;
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		return strPPortConf.usMaxSpeed;
	}
	else
	{
		DSP_ERROR("pp snap get limit speed conf failed,return 50.\n");
		return 50;
	}
}

int dspPPLimitSpeedSet(int iChanId, int iLaneId)
{
	int ret = 0;
	PORT_VEHICLE_CONF_INFO strPPortConf;
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	LIMIT_SPEED_PARAM param;
	memset(&param, 0, sizeof(LIMIT_SPEED_PARAM));
	param.chan_id = iChanId;
	param.lane_id = strPPortConf.uiCarRoadId;
	param.max_speed = strPPortConf.usMaxSpeed;
	param.min_speed = strPPortConf.usMinSpeed;

	if (0 == ret)
	{
		//sem_wait(&sem_dsp);
		ret = CALL_CYC_CMD(NET_DEV_SET_LIMIT_SPEED, param);
		//sem_post(&sem_dsp);
	}
	return ret;
}


int dspPPSnapOsdSet(int iChanId, int iLaneId)
{
	int ret = 0;
	unsigned int uiGLaneNameLen = 0;
	unsigned int uiLaneNameLen = 0;

	NameParam osdParam;
	NameParam strParam;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 > ret)
	{
		DSP_ERROR("pp snap osd get conf failed\n");
		return -1;
	}

	memset(&strParam, 0, sizeof(strParam));
	strcpy(strParam.name, strOACfgInfoGet.szPassPortName);
	dspSetJpgOsdString(0, 0, PP_OSD_ROAD_NAME_SET, &strParam);

	memset(&strParam, 0, sizeof(strParam));
	strcpy(strParam.name, strOACfgInfoGet.szDirection);
	dspSetJpgOsdString(0, 0, PP_OSD_ROAD_DIR_SET, &strParam);

	memset(&osdParam, 0, sizeof(osdParam));
	PORT_VEHICLE_CONF_INFO strPPortConf;
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		osdParam.startX = strPPortConf.usOSDPosX;
		osdParam.startY = strPPortConf.usOSDPosY;
		//osdParam.width = 8*strlen(strPPortConf.szOSDContent);
		osdParam.height = 16;

		uiGLaneNameLen = strlen(strOACfgInfoGet.szPassPortName);
		uiLaneNameLen = strlen(strOACfgInfoGet.szDirection);
		strncpy(osdParam.name, strOACfgInfoGet.szPassPortName, uiGLaneNameLen);
		strncpy(osdParam.name + uiGLaneNameLen, strOACfgInfoGet.szDirection, uiLaneNameLen);
		strncpy(osdParam.name + uiGLaneNameLen + uiLaneNameLen, strPPortConf.szOSDContent, strlen(strPPortConf.szOSDContent));
		ret = dspSetSnapOsd(iChanId, iLaneId, &osdParam);
		if (0 != ret)
		{
			DSP_ERROR("%s %d  dspSetSnapOsd ret=%d error .\n", __FUNCTION__, __LINE__, ret);
		}

		JPG_OSD_COLOR_PARAM jocp;
		memset(&jocp, 0, sizeof(jocp));
		jocp.lane_id = iLaneId;
		jocp.color = strOACfgInfoGet.cOSDColor;
		ALG_LOOP_MODE_PARAM almp;
		memset(&almp, 0, sizeof(almp));
		almp.chan_id = iChanId;
		almp.lane_id = iLaneId;
		almp.loop_mode = strOACfgInfoGet.cVLWorkMode;

		//sem_wait(&sem_dsp);
		ret = CALL_CYC_CMD(NET_DEV_SET_JPG_OSD_COLOR, jocp);
		if (0 != ret)
		{
			DSP_ERROR("%s %d call NET_DEV_SET_JPG_OSD_COLOR failed, error code: %d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}
		//sem_post(&sem_dsp);

#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
		ret = CALL_CYC_CMD(NET_DEV_SET_LOOP_MODE, almp);
		if (0 != ret)
		{
			DSP_ERROR("%s %d call NET_DEV_SET_LOOP_MODE failed, error code:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}
#endif

		memset(&strParam, 0, sizeof(strParam));
		strcpy(strParam.name, strPPortConf.szOSDContent);

		dspSetJpgOsdString(0, iLaneId, PP_OSD_ROAD_LANE_SET, &strParam);
	}
	return ret;
}

int dspPPHighVLParamSet(int iChanel, int iLaneId)
{
	int ret;
	VILib_feature_t strVLFeature;
	VL_CONF_INFO strVLConf;
	memset(&strVLFeature, 0, sizeof(strVLFeature));
	ret = SysConfGet(e_VL_CONF, &strVLConf, iLaneId);
	if (0 == ret)
	{
		strVLFeature.chan_id = iChanel;
		strVLFeature.lane_id = iLaneId;
		strVLFeature.corrDayThresh = strVLConf.uiCorrDayThresh;
		strVLFeature.corrMotoThresh = strVLConf.uiCorrMotorThresh;
		strVLFeature.corrNightThresh = strVLConf.uiCorrNightThresh;
		strVLFeature.ThrDayToNight = strVLConf.usThrDayToNight;
		strVLFeature.ThrNightToDay = strVLConf.usThrNightToDay;
		strVLFeature.corrEveningThresh = strVLConf.uiCorrEveningThresh;
		strVLFeature.dayMotorWidth = strVLConf.uiDayMotorWidth;
		strVLFeature.nightMotorWidth = strVLConf.uiNightMotorWidth;

		strVLFeature.capDelay = strVLConf.cCapDelayNum;
		strVLFeature.minPlateWidth = strVLConf.cMinPlatePixelSize;
		strVLFeature.maxPlateWidth = strVLConf.cMaxPlatePixelSize;
		//sem_wait(&sem_dsp);
		ret = CALL_CYC_CMD(NET_DEV_SET_VILIB_PARAM, strVLFeature);
		//sem_post(&sem_dsp);
	}
	else
	{
		DSP_ERROR("%s %d get high vl param invalid.\n", __FUNCTION__, __LINE__);
		return -1;
	}
	return ret;
}


int dspPPVSpeedFlagSet(void)
{
	int ret = 0;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		//sem_wait(&sem_dsp);
		SPEED_VISUAL_DETECTION strVSMParam;
		memset(&strVSMParam, 0, sizeof(strVSMParam));
		strVSMParam.chan_id = 0;
		strVSMParam.enable = strOACfgInfoGet.ucVSpeedMeaFlag;
		strVSMParam.actualDistance = strOACfgInfoGet.usVSpeedMeaLen;
		ret = CALL_CYC_CMD(NET_DEV_SET_VIDEO_SPEED_MEASURE, strVSMParam);
		if (0 != ret)
		{
			DSP_ERROR("%s %d call NET_DEV_SET_VIDEO_SPEED_MEASURE failed, error:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}
		//sem_post(&sem_dsp);
	}
	return ret;
}

int dspPPVLParamSet(int iChanel, int iLaneId)
{
	int ret = 0;
	VLParam strVLParam;
	memset(&strVLParam, 0, sizeof(strVLParam));
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	PORT_VEHICLE_CONF_INFO strPPortConf;
	PPVL_SNAP_STEPS_INFO strPPSnapStepInfo;
	PPVL_SNAP_TARGET_INFO strSnapObjInfo;

	SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		strVLParam.nShowArea =  1;
		strVLParam.startX = strPPortConf.strVLArea.usAreaPosX;
		strVLParam.startY = strPPortConf.strVLArea.usAreaPosY;
		strVLParam.width =  strPPortConf.strVLArea.usAreaWidth;
		strVLParam.height = strPPortConf.strVLArea.usAreaHeight;

		ret = dspPPHighVLParamSet(0, iLaneId);
		if (0 != ret)
		{
			DSP_ERROR("%s %d  ret=%d error .\n", __FUNCTION__, __LINE__, ret);
		}
#if defined(CYC200MK)||defined(CYC200K2)||defined(CYC200K3)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)

		SNAP_DIRECTION_PARAM param;
		memset(&param, 0, sizeof(param));
		param.chan_id = 0;
		param.lane_id = iLaneId;
		param.direction = strPPortConf.cVLSnapDir;

		//sem_wait(&sem_dsp);
		ret = CALL_CYC_CMD(NET_DEV_SET_SNAP_DIRECTION, param);
		if (0 != ret)
		{
			DSP_ERROR("%s %d  call NET_DEV_SET_SNAP_DIRECTION failed, error %d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}

		PP_VL_ENABLE_PARAM pvep;
		memset(&pvep, 0, sizeof(pvep));
		pvep.chan_id = 0;
		pvep.lane_id = iLaneId;
		pvep.bEnableFlag = strPPortConf.cVLDecEn;
		ret = CALL_CYC_CMD(NET_DEV_SET_PPVL_ENABLE, pvep);
		if (0 != ret)
		{
			DSP_ERROR("%s %d  call NET_DEV_SET_PPVL_ENABLE failed, error %d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}
		//sem_post(&sem_dsp);

		ret = dspSetVLParam(iChanel, iLaneId, strPPortConf.cVLDispFlag, &strVLParam);
		if (0 != ret)
		{
			DSP_ERROR("%s %d ret=%d error .\n", __FUNCTION__, __LINE__, ret);
		}

		//sem_wait(&sem_dsp);
		strSnapObjInfo.chan_id = iChanel;
		strSnapObjInfo.lane_id = iLaneId;
		strSnapObjInfo.daySnapTarget = (EN_SNAP_TARGET)(strPPortConf.ucDaySnapObj);
		strSnapObjInfo.nightSnapTarget = (EN_SNAP_TARGET)(strPPortConf.ucNightSnapObj);
		strSnapObjInfo.durationTarget = (EN_SNAP_TARGET)(strPPortConf.strTimeSlotSnapObj.ucSnapObj);
		strSnapObjInfo.beginTime = strPPortConf.strTimeSlotSnapObj.uiBeginTimeSecs;
		strSnapObjInfo.durationTime = strPPortConf.strTimeSlotSnapObj.uiLastTimeSecs;
		strSnapObjInfo.ifPlateLighted = strPPortConf.ucInLightEn;

		ret = CALL_CYC_CMD(NET_DEV_SET_PP_SNAP_TARGET, strSnapObjInfo);
		if (0 != ret)
		{
			DSP_ERROR("%s %d call NET_DEV_SET_PP_SNAP_TARGET failed, error:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}

		memset(&strPPSnapStepInfo, 0, sizeof(strPPSnapStepInfo));
		strPPSnapStepInfo.chan_id = iChanel;
		strPPSnapStepInfo.lane_id = iLaneId;
		strPPSnapStepInfo.SnapStepsAmount = strPPortConf.ucSnapPicNum;
		strPPSnapStepInfo.IntervalTime = strPPortConf.usSnapPicInter;

		ret = CALL_CYC_CMD(NET_DEV_SET_PPVL_SNAPSTEP_INFO, strPPSnapStepInfo);
		if (0 != ret)
		{
			DSP_ERROR("%s %d ret=%d error .\n", __FUNCTION__, __LINE__, ret);
		}
		//sem_post(&sem_dsp);

#endif
	}
	return ret;
}

int dspPPSnapAreaSet(int iChanel, int iLaneId)
{
	int ret = -1;
	VLParam vlparam;
	memset(&vlparam, 0, sizeof(vlparam));
	PORT_VEHICLE_CONF_INFO strPPortConf;
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		vlparam.startX = strPPortConf.strRoadArea.usAreaPosX;
		vlparam.startY = strPPortConf.strRoadArea.usAreaPosY;
		vlparam.width = strPPortConf.strRoadArea.usAreaWidth;
		vlparam.height = strPPortConf.strRoadArea.usAreaHeight;
		ret = dspSetSnapArea(iLaneId, vlparam.startX, vlparam.startY, vlparam.width, vlparam.height, strPPortConf.cLaneDispFlag);
		if (0 != ret)
		{
			DSP_ERROR("%s %d  ret=%d error .\n", __FUNCTION__, __LINE__, ret);
		}
	}
	return ret;
}

int dspPPUnidirLaneTimeSet(int iChanel, int iLaneId)
{
	int ret = -1;
	TIME_PERIOD_DIRECTION strTPDir;
	memset(&strTPDir, 0, sizeof(strTPDir));
	PORT_VEHICLE_CONF_INFO strPPortConf;
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		strTPDir.lane_id = iLaneId;
		strTPDir.cSnapDir = strPPortConf.cSnapDir;
		strTPDir.cSDEnFlag = strPPortConf.cSDEnFlag;

		strTPDir.cTruckEnFlag = strPPortConf.cTruckEnFlag;

		strTPDir.iSDBeginTime1 = strPPortConf.iSDBeginTime1;
		strTPDir.iSDLastTime1 = strPPortConf.iSDLastTime1;

		strTPDir.iSDBeginTime2 = strPPortConf.iSDBeginTime2;
		strTPDir.iSDLastTime2 = strPPortConf.iSDLastTime2;

		strTPDir.iSDBeginTime3 = strPPortConf.iSDBeginTime3;
		strTPDir.iSDLastTime3 = strPPortConf.iSDLastTime3;

		//sem_wait(&sem_dsp);
		ret = CALL_CYC_CMD(NET_DEV_SET_TIME_PERIOD_DIRECTION, strTPDir);
		if (0 != ret)
		{
			DSP_ERROR("%s %d call NET_DEV_SET_TIME_PERIOD_DIRECTION failed. error:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError());
		}
		else
		{
			DSP_ERROR("%s %d call NET_DEV_SET_TIME_PERIOD_DIRECTION ok.\n", __FUNCTION__, __LINE__);
		}
		//sem_post(&sem_dsp);

	}
	else
	{
		DSP_ERROR("%s %d  get pportConf failed,iLaneId:%d.\n", __FUNCTION__, __LINE__, iLaneId);
	}
	return ret;
}

int dspPPCoilTypeGet(int iChanel, int iLaneId)
{
	int ret = 0;
	PORT_VEHICLE_CONF_INFO strPPortConf;
	memset(&strPPortConf, 0, sizeof(strPPortConf));
	ret = SysConfGet(e_PASSPORT_VEHICLE_CONF, &strPPortConf, iLaneId);
	if (0 == ret)
	{
		/* 首先判断是否是虚拟线圈，若是还需要判断是否使能检测 */
		if (VIRTUAL_LOOP == strPPortConf.usDetectType)
		{
			return strPPortConf.usDetectType;
		}
		else if (GROUD_LOOP == strPPortConf.usDetectType)
		{
			/* 若是地感线圈则要判断是否已经切换到虚拟线圈了 */
			if ((strPPortConf.cCoilSwitchFlag == 1) && (strPPortConf.cCoilSwitchSta != SWITCH_DEFAULT_VAL))
			{
				return strPPortConf.cCoilSwitchSta;
			}
			else
			{
				return strPPortConf.usDetectType;
			}
		}
	}
	else
	{
		return -1;
	}
	return 0;
}

int dspPPCoilTypeSet(int iChanel, int iLaneId)
{
	int ret = 0;
	Vehicle_Detect_type_t feature;
	ret = dspPPCoilTypeGet(iChanel, iLaneId);
	if (GROUD_LOOP == ret)
	{
		feature = e_INNERVATION_LOOP;

		g_aiCoilSta[0] = MODE_OK_STA;
		g_aiVirtualSta[0] = MODE_SHUTDOWN_STA;

		/* 触发监控程序发送网管信息 */
		g_iErrorTimes = 201;
	}
	else if (VIRTUAL_LOOP == ret)
	{
		feature = e_VIRTUAL_LOOP;

		g_aiCoilSta[0] = MODE_SHUTDOWN_STA;
		g_aiVirtualSta[0] = MODE_OK_STA;

		/* 触发监控程序发送网管信息 */
		g_iErrorTimes = 201;
	}
	else
	{
		DSP_ERROR("%s %d get vehicle_detect_type %d param invalid  .\n", __FUNCTION__, __LINE__, ret);
		return -1;
	}
	VEHICLE_DETECT_TYPE vdt;
	memset(&vdt, 0, sizeof(vdt));
	vdt.chan_id = iChanel;
	vdt.lane_id = iLaneId;
	vdt.detect_type = feature;

	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_VEHICLE_DETECT_TYPE, vdt);
	if (0 != ret)
	{
		DSP_ERROR("error:%s %d set vehicle_detect_type ret:%d iChanel:%d,laneId:%d,feature:%d.\n", __FUNCTION__, __LINE__\
				  , ret, iChanel, iLaneId, feature);
	}
	else
	{
		DSP_INFO("%s %d set vehicle_detect_type ret:%d iChanel:%d,laneId:%d,feature:%d.\n", __FUNCTION__, __LINE__\
				 , ret, iChanel, iLaneId, feature);
	}
	//sem_post(&sem_dsp);
	return ret;
}


/*
 * Function:
 * Description:
 * Calls:
 * Called By:
 * Table Accessed:
 * Table Updated:
 * Input:  获取检测周期(分钟为单位)
 * Output:
 * Return: 是否进行补光的切换
 * Others:
 */
int dspPPFillInLightParamGet(void)
{
	int ret = 0;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;
	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = strOACfgInfoGet.ucSmartSwitchEn;
	}
	return ret;
}

#if 0
int dspPpFillInLightSwitchDec(char cMode)
{
	int ret = 0;
	unsigned int uiLane = 0;
	char cGroup = 0;
	float fMaxGain = 0;
	float fMinGain = 0;
	CAMERA_CTRL_CONF_INFO strCamInfo;
	unsigned int uiGrayThreshold = 0;
	float fGrayThreshold = 0;
	CUR_CAMERA_STATUS strCurCamSta;
	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;

	memset(&strCurCamSta, 0, sizeof(CUR_CAMERA_STATUS));
	strCurCamSta.cbSize = sizeof(CUR_CAMERA_STATUS);

	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = cameraRunStaParamGet(0, &strCurCamSta);
		if (strOACfgInfoGet.ucFillInLSwitchEn > 0)
		{
			uiGrayThreshold = strOACfgInfoGet.ucGrayThresholdVal;
			fGrayThreshold = (float)uiGrayThreshold;
			DSP_INFO("conf grey threshold val is %d, float val:%f.\n", uiGrayThreshold, fGrayThreshold);
			cGroup = camTimeRangeJudge();
			/* 增益控制设置 */
			memset(&strCamInfo, 0, sizeof(strCamInfo));
			ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EGAIN + cGroup * MAX_CAM_CTRL_NUM);
			if (0 != ret)
			{
				DSP_ERROR("fillin get camara gain conf error\n");
				return -1;
			}
			else
			{
				if (FIX == strCamInfo.cCtlType)
				{
					fMaxGain = strCamInfo.fFixVal - 1;
					fMinGain = strCamInfo.fFixVal + 1;
				}
				else
				{
					fMaxGain = strCamInfo.fMaxVal - 4;
					if (fMaxGain < strCamInfo.fMinVal)
					{
						fMaxGain = strCamInfo.fMinVal;
					}

					fMinGain = strCamInfo.fMinVal + 4;
					if (fMinGain > fMaxGain)
					{
						fMinGain = fMaxGain;
					}
				}
			}

			DSP_INFO("FillIn light,confMaxGain:%f,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					 fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);

			if ((strCurCamSta.fCurGain > fMaxGain) && (strCurCamSta.fCurGrey < fGrayThreshold))
			{
				if (g_iPreSta != FLASH_FILLIN_DAY_TO_NIGHT)
				{
					g_iPreSta = FLASH_FILLIN_DAY_TO_NIGHT;
					for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
					{
						/* 调用相关接口进行闪光灯到频闪灯的切换 */
						dctlPPFlashConf(g_iPreSta, uiLane);
					}
					DSP_INFO("FillIn light Day_to_night is here!\n");
					syslog(LOG_ERR | LOG_USER, "light cut day to night,preSta:%d,confMaxGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
						   g_iPreSta, fMaxGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				}
				else
				{
					DSP_INFO("FillIn light Day_to_night same is here!\n");
				}
			}
			else if ((strCurCamSta.fCurGain <= fMinGain) && (strCurCamSta.fCurGrey >= fGrayThreshold))
			{
				g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行频闪灯到闪光灯的切换 */
					dctlPPFlashConf(g_iPreSta, uiLane);
				}
				syslog(LOG_ERR | LOG_USER, "light cut night to day,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					   g_iPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				DSP_INFO("FillIn light night_to_day is here!\n");
			}
			else
			{
				syslog(LOG_ERR | LOG_USER, "warning: light cut,confMaxGain:%f,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					   fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
			}

		}
		else
		{
			g_iPreSta = 0;
			/* 切换回正常工作模式 */
			for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
			{
				dctlPPFlashConf(g_iPreSta, uiLane);
			}

			syslog(LOG_ERR | LOG_USER, "light cut to normal sta,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
				   g_iPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
			DSP_INFO("FillIn light cut to normal sta is here!\n");
		}

		if (cMode == CAM_PARAM_MODE_FORCE)
		{
			g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
			for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
			{
				/* 调用相关接口进行频闪灯到闪光灯的切换 */
				dctlPPFlashConf(g_iPreSta, uiLane);
			}
			syslog(LOG_ERR | LOG_USER, "force light cut night to day!\n");
			DSP_INFO("force FillIn light night_to_day is here!\n");
		}

	}
	return ret;

}

#else

int dspPpFillInLightSwitchDec(char cMode)
{
	int ret = 0;
	unsigned int uiLane = 0;
	char cGroup = 0;
	float fMaxGain = 0;
	float fMinGain = 0;
	CAMERA_CTRL_CONF_INFO strCamInfo;
	float	fGrayThreshold = (float)35;
	CUR_CAMERA_STATUS strCurCamSta;
	static unsigned char s_ucDayToNight = 0;
	static unsigned char s_ucNightToDay = 0;

	PASSPORT_OVERALL_CONF_INFO strOACfgInfoGet;

	memset(&strCurCamSta, 0, sizeof(CUR_CAMERA_STATUS));
	strCurCamSta.cbSize = sizeof(CUR_CAMERA_STATUS);

	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strOACfgInfoGet, 0);
	if (0 == ret)
	{
		ret = cameraRunStaParamGet(0, &strCurCamSta);
		if (strOACfgInfoGet.ucSmartSwitchEn > 0)
		{

			if (cMode == CAM_PARAM_MODE_MANUAL)
			{
				g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
				s_ucNightToDay = 0;
				s_ucDayToNight = 0;

				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					/* 调用相关接口进行频闪灯到闪光灯的切换 */
					dctlPPFlashConf(g_iPreSta, uiLane);
				}
				syslog(LOG_ERR | LOG_USER, "pp smart switch,manual switch night to day!\n");
				DSP_INFO("pp smart switch,manual switch night to day!\n");
			}

			DSP_INFO("float val:%f.\n", fGrayThreshold);
			cGroup = camTimeRangeJudge();
			/* 增益控制设置 */
			memset(&strCamInfo, 0, sizeof(strCamInfo));
			ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamInfo, EGAIN + cGroup * MAX_CAM_CTRL_NUM);
			if (0 != ret)
			{
				DSP_ERROR("pp smart get camara gain conf error\n");
				return -1;
			}
			else
			{
				if (FIX == strCamInfo.cCtlType)
				{
					fMaxGain = strCamInfo.fFixVal - 1;
					fMinGain = strCamInfo.fFixVal + 1;
				}
				else
				{
					fMaxGain = strCamInfo.fMaxVal - 4;
					if (fMaxGain < strCamInfo.fMinVal)
					{
						fMaxGain = strCamInfo.fMinVal;
					}

					fMinGain = strCamInfo.fMinVal + 4;
					if (fMinGain > fMaxGain)
					{
						fMinGain = fMaxGain;
					}
				}
			}

			DSP_INFO("pp smart switch,confMaxGain:%f,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					 fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);

			if ((strCurCamSta.fCurGain > fMaxGain) && (strCurCamSta.fCurGrey < fGrayThreshold))
			{
				s_ucNightToDay = 0;

				if (g_iPreSta != FLASH_FILLIN_DAY_TO_NIGHT)
				{
					s_ucDayToNight++;
				}

				if (cMode == CAM_PARAM_MODE_MANUAL)
				{
					/* 手动模式下需及时进行一次开灯切换操作，关灯可不需要，因为已经在默认状态下关过 */
					s_ucDayToNight = MAX_CHECK_SWITCH_SUM + 1;
				}

				if (s_ucDayToNight > MAX_CHECK_SWITCH_SUM)
				{
					g_iPreSta = FLASH_FILLIN_DAY_TO_NIGHT;
					s_ucDayToNight = 0;

					for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
					{
						/* 调用相关接口进行闪光灯到频闪灯的切换 */
						dctlPPFlashConf(g_iPreSta, uiLane);
					}
					DSP_INFO("pp smart switch Day_to_night!\n");
					syslog(LOG_ERR | LOG_USER, "pp smart switch day to night,preSta:%d,confMaxGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
						   g_iPreSta, fMaxGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				}
				else
				{
					DSP_INFO("pp smart switch Day_to_night check,time is %d!\n", s_ucDayToNight);
				}
			}
			else if ((strCurCamSta.fCurGain <= fMinGain) && (strCurCamSta.fCurGrey >= fGrayThreshold))
			{
				s_ucDayToNight = 0;

				if (g_iPreSta != FLASH_FILLIN_NIGHT_TO_DAY)
				{
					s_ucNightToDay++;
				}

				if (s_ucNightToDay > MAX_CHECK_SWITCH_SUM)
				{
					if (s_ucNightToDay % (MAX_CHECK_SWITCH_SUM + 1) == 0)
					{
						if (g_iPreSta != FLASH_FILLIN_NIGHT_TO_DAY)
						{
							syslog(LOG_ERR | LOG_USER, "pp smart switch night to day,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
								   g_iPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
						}

						g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
						for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
						{
							/* 调用相关接口进行频闪灯到闪光灯的切换 */
							dctlPPFlashConf(g_iPreSta, uiLane);
						}
					}
					DSP_INFO("pp smart switch night_to_day is here!\n");

					s_ucNightToDay++;
				}
				else
				{
					DSP_INFO("pp smart switch, night_to_day check,time is %d!\n", s_ucNightToDay);
				}
			}
			else
			{
				s_ucNightToDay = 0;
				s_ucDayToNight = 0;
				syslog(LOG_ERR | LOG_USER, "warning:pp smart,can't switch,clean switch time.confMaxGain:%f,confMinGain:%f,curGain:%f;confGrey:%f,curGrey:%f.\n", \
					   fMaxGain, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
			}

		}
		else
		{
			DSP_INFO("pp FillIn light ucSmartSwitchEn is disable!\n");
			if (strOACfgInfoGet.ucTimerSwitchEn <= 0)
			{
				g_iPreSta = 0;
				s_ucNightToDay = 0;
				s_ucDayToNight = 0;

				/* 切换回正常工作模式 */
				for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
				{
					dctlPPFlashConf(g_iPreSta, uiLane);
				}

				syslog(LOG_ERR | LOG_USER, "pp light cut to normal sta,preSta:%d,confMinGain:%f,curGain:%f,confGrey:%f,curGrey:%f.\n", \
					   g_iPreSta, fMinGain, strCurCamSta.fCurGain, fGrayThreshold, strCurCamSta.fCurGrey);
				DSP_INFO("pp FillIn light cut to normal sta!\n");
			}
		}

		if (cMode == CAM_PARAM_MODE_FORCE)
		{
			g_iPreSta = FLASH_FILLIN_NIGHT_TO_DAY;
			s_ucNightToDay = 0;
			s_ucDayToNight = 0;

			for (uiLane = 0; uiLane < g_uiClientLaneSum; uiLane++)
			{
				/* 调用相关接口进行频闪灯到闪光灯的切换 */
				dctlPPFlashConf(g_iPreSta, uiLane);
			}
			syslog(LOG_ERR | LOG_USER, "pp force light cut night to day!\n");
			DSP_INFO("pp force FillIn light night_to_day!\n");
		}

	}
	else
	{
		DSP_ERROR("SysConfGet get e_PASSPORT_OVERALL_CONF failed!\n");
	}
	return ret;

}
#endif

int dspPpJpgOsdConfSet(int iChanId)
{
#if USE_NEW_OSD_SET
	int ret = 0;
	BAYONET_JPG_OSD_CONF strSysPpJpgOsdConfGet;
	DSP_PP_JPG_OSD_CONF	 strPpJpgOsdConf;

	ret = SysConfGet(e_PP_JPG_OSD_CONF, &strSysPpJpgOsdConfGet, 0);
	if (0 != ret)
	{
		SYSHANDLE_ERROR("read pp jpg osd conf failed\n");
		return -1;
	}
	memset(&strPpJpgOsdConf, 0, sizeof(strPpJpgOsdConf));
	memcpy((void*)(&strPpJpgOsdConf) + sizeof(unsigned int), &strSysPpJpgOsdConfGet, sizeof(strSysPpJpgOsdConfGet));

	strPpJpgOsdConf.chan_id = iChanId;
	//sem_wait(&sem_dsp);
	ret = CALL_CYC_CMD(NET_DEV_SET_PP_JPG_OSD_CONF, strPpJpgOsdConf);
	//sem_post(&sem_dsp);
	if (0 != ret)
	{
		DSP_ERROR("%s %d ret=%d error .\n", __FUNCTION__, __LINE__, ret);
	}
	else
	{
		DSP_ERROR("%s %d ret=%d OK.\n", __FUNCTION__, __LINE__, ret);
	}
#endif
	return 0;
}

int dspPPIrregNameInfoSet(void)
{
#if USE_NEW_OSD_SET
	int ret = 0;
	char szPpDevCodeTmp[EXT_DEV_CODE_LEN + MAX_DEV_CODE_LEN];

	PASSPORT_OVERALL_CONF_INFO strPPOverAllConf;
	DSP_PP_IRREG_INFO strDspPpIrregInfo;
	PP_IRREG_CONF_INFO strPPIrregConfGet;
	ret = SysConfGet(e_PP_IRREG_INFO_CONF, &strPPIrregConfGet, 0);
	if (0 != ret)
	{
		DSP_ERROR("read pp overall conf failed\n");
		return -1;
	}

	ret = SysConfGet(e_PASSPORT_OVERALL_CONF, &strPPOverAllConf, 0);
	if (0 != ret)
	{
		SYSHANDLE_ERROR("read pp overall conf failed\n");
		return -1;
	}

	//sem_wait(&sem_dsp);

	strDspPpIrregInfo.epIrregCode = e_OverSpeedSmallPercent50Code;
	memcpy(strDspPpIrregInfo.szIrregCode, strPPIrregConfGet.szOverSpeedSmallPercent50Code, MAX_IRREG_CODE_LEN);
	memcpy(strDspPpIrregInfo.szIrregName, strPPIrregConfGet.szOverSpeedSmallPercent50Name, MAX_IRREG_NAME_LEN);
	ret = CALL_CYC_CMD(NET_DEV_SET_PP_IRREG_INFO, strDspPpIrregInfo);

	strDspPpIrregInfo.epIrregCode = e_OverSpeedLargePercent50Code;
	memcpy(strDspPpIrregInfo.szIrregCode, strPPIrregConfGet.szOverSpeedLargePercent50Code, MAX_IRREG_CODE_LEN);
	memcpy(strDspPpIrregInfo.szIrregName, strPPIrregConfGet.szOverSpeedLargePercent50Name, MAX_IRREG_NAME_LEN);
	ret = CALL_CYC_CMD(NET_DEV_SET_PP_IRREG_INFO, strDspPpIrregInfo);

	strDspPpIrregInfo.epIrregCode = e_ReverseRunCode;
	memcpy(strDspPpIrregInfo.szIrregCode, strPPIrregConfGet.szReverseRunCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspPpIrregInfo.szIrregName, strPPIrregConfGet.szReverseRunName, MAX_IRREG_NAME_LEN);
	ret = CALL_CYC_CMD(NET_DEV_SET_PP_IRREG_INFO, strDspPpIrregInfo);

	strDspPpIrregInfo.epIrregCode = e_InfringeProhibCode;
	memcpy(strDspPpIrregInfo.szIrregCode, strPPIrregConfGet.szInfringeProhibCode, MAX_IRREG_CODE_LEN);
	memcpy(strDspPpIrregInfo.szIrregName, strPPIrregConfGet.szInfringeProhibName, MAX_IRREG_NAME_LEN);
	ret = CALL_CYC_CMD(NET_DEV_SET_PP_IRREG_INFO, strDspPpIrregInfo);


	memset(szPpDevCodeTmp, 0, sizeof(szPpDevCodeTmp));
	strcat(szPpDevCodeTmp, strPPOverAllConf.szPpDevCode);
	szPpDevCodeTmp[MAX_DEV_CODE_LEN - 1] = 0;

	strcat(szPpDevCodeTmp, strPPOverAllConf.szExPpDevCode);
	szPpDevCodeTmp[MAX_DEV_CODE_LEN + EXT_DEV_CODE_LEN - 1] = 0;

	ret = NET_CYC_SetDEVConfig(NET_DEV_SET_PP_SN, szPpDevCodeTmp, EXT_DEV_CODE_LEN + MAX_DEV_CODE_LEN);
	if (0 != ret)
	{
		DSP_ERROR("%s %d set pp sn name failed, error:%d error.len:%d.\n", __FUNCTION__, __LINE__, NET_DVE_GetLastError(), strlen(szPpDevCodeTmp));
	}
	else
	{
		DSP_INFO("%s %d set pp sn name OK.len:%d.\n", __FUNCTION__, __LINE__, strlen(szPpDevCodeTmp));
	}

	//sem_post(&sem_dsp);

	return ret;
#else
	return 0;
#endif
}

/* 封装查询摄像机状态接口，以转换新isp上的快门和增益 */
int cameraRunStaParamGet(int chan_id, CUR_CAMERA_STATUS* param)
{
	int ret = 0;
	int iTotalLine = 0;

	//ret = dsp_camera274_param_get(0, param);
	ret = ccd_Get_Cur_Cam_Param(param);
#if OLD_ISP_SET

#else
	//iTotalLine = newIspTotalLinesGet();
	//param->fCurShutter = (1 / param->fCurShutter) / iTotalLine;
	param->fCurShutter = param->fCurShutter / 1000000;
	//param->fCurGain = param->fCurGain * 35 / 1000;
	DSP_INFO("new isp gain val is %f \n", param->fCurGain);
#endif
	return 1;
}

int timerRebootProcess(void)
{
	int ret =0;
	char cEnFlag =0;
    struct tm tmPlanStart;
    struct tm tmTimerVal;
    unsigned long ulPlanStart = time(NULL) + 8*3600;
    unsigned long ulTimerRestart =0;

    SYS_FUNC_ENABLE_INFO strSysFuncCfg;
    memset(&strSysFuncCfg, 0, sizeof(strSysFuncCfg));
    
    ret = SysConfGet(e_SYS_FUNC_ENABLE_CONF, &strSysFuncCfg, 0);
	cEnFlag = strSysFuncCfg.cRestartTimeEn;
	if(cEnFlag > 0)
	{
		ulTimerRestart = strSysFuncCfg.uiRestartTime;
		gmtime_r((time_t *)&ulTimerRestart, &tmTimerVal);
		
		gmtime_r((time_t *)&ulPlanStart, &tmPlanStart);
	    if( (tmPlanStart.tm_hour == tmTimerVal.tm_hour) && (tmPlanStart.tm_min == tmTimerVal.tm_min) )
	    {
	        SYSHANDLE_ERROR("timer reboot dev,date:hour:%d,min:%d!\n", tmPlanStart.tm_hour, tmPlanStart.tm_min);
	        mcLog(LMT_SYSCALL,LT_ERROR,"cur date:hour:%d,min:%d,timer reboot dev!\n", tmPlanStart.tm_hour, tmPlanStart.tm_min);
			system("wd_reboot");
	    }
	}
}


