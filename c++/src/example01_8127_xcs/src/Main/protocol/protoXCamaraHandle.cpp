/******************************************************************************
 * bayonetProHandle.cpp:
 *
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 *
 * DESCRIPTION:
 *
 * modification history
 * --------------------
 * 01a, 2009.12.04, kj Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include "SysConf.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"
#include <time.h>
#include <sys/time.h>
#include "Record.h"
#include "rtc_util.h"

extern "C"
{
#include "dsp.h"
}

#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"

#include "SysHandle.h"

#if defined(CYC200XZD)||defined(CYC500XZD)
#include "zebraCtl.h"
#include "ccdCtl.h"
#else
#include "icx274CCDCtl.h"
#endif

extern cam_info_t HDConf;
extern CAMERA_CTRL_CONF_INFO  DefaultStrCameraConfVal[MAX_CAM_GROUP_NUM][MAX_CAM_CTRL_NUM];

int CHandleXcsPtl::hdCamaraAttribQuery(XCS_CLIENT* pClintSocket)
{
	int nRet = 0;
	PSTR_CAMARA_ATTRIB_PC_QUERY pPcQueryInfo = (PSTR_CAMARA_ATTRIB_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
	PSTR_CAMARA_ATTRIB_QUERY pStrSend = (PSTR_CAMARA_ATTRIB_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);

	nRet = getCameraInfo(reinterpret_cast<int*>(&pStrSend->uiWidth), \
						 reinterpret_cast<int*>(&pStrSend->uiHeight), reinterpret_cast<int*>(&pStrSend->uiPixels));
	XCSPTL_INFO("%s %d Query HD Info %s.\n", __FUNCTION__, __LINE__, nRet == 0 ? "sucess" : "failed");

	if (nRet)
	{
		XCSPTL_ERROR("query cama attri Info failed.\n");
		SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
	}
	else
	{
		XCSPTL_INFO("%s %d cam width:0x%x,geight:0x%x,pix:0x%x\n", __FUNCTION__, __LINE__, \
					pStrSend->uiWidth, pStrSend->uiHeight, pStrSend->uiPixels);
		pStrSend->uiWidth = ntohl(pStrSend->uiWidth);
		pStrSend->uiHeight = ntohl(pStrSend->uiHeight);
		pStrSend->uiPixels = ntohl(pStrSend->uiPixels);
		pStrSend->uiHdCameraId = pPcQueryInfo->uiHdCameraId;
	}

	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_CAMARA_ATTRIB_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_ATTRIB_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

	return 0;
}

/******************************************************************************
 * CHandleXcsPtl.hdCamaraParamQuery() :
 *
 * DESCRIPTION:  协议的序号均是从1开始的，所以需要减一或加一
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 *01a, 2010.08.10, wangbin Create
 * --------------------
 ******************************************************************************/
int CHandleXcsPtl::hdCamaraParamQuery(XCS_CLIENT* pClintSocket)
{
	PSTR_CAMARA_CTL_INFO_QUERY pStrSend = (PSTR_CAMARA_CTL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
	pStrSend->cCtrlNum = MAX_CAM_CTRL_NUM;

	memset(pStrSend->pstrCamaraCtl, 0, sizeof(pStrSend->pstrCamaraCtl));
	pStrSend->pstrCamaraCtl[ESHUTTER].cCamaraType = ESHUTTER + 1;
	pStrSend->pstrCamaraCtl[ESHUTTER].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[ESHUTTER].fMaxVal = HDConf.MaxShutterValue;
	pStrSend->pstrCamaraCtl[ESHUTTER].fMinVal = HDConf.MinShutterValue;

	pStrSend->pstrCamaraCtl[EGAIN].cCamaraType = EGAIN + 1;
	pStrSend->pstrCamaraCtl[EGAIN].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[EGAIN].fMaxVal = HDConf.MaxGainValue;
	pStrSend->pstrCamaraCtl[EGAIN].fMinVal = HDConf.MinGainValue;

	pStrSend->pstrCamaraCtl[EEXPOSURE].cCamaraType = EEXPOSURE + 1;
	pStrSend->pstrCamaraCtl[EEXPOSURE].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[EEXPOSURE].fMaxVal = HDConf.MaxExposureValue;
	pStrSend->pstrCamaraCtl[EEXPOSURE].fMinVal = HDConf.MinExposureValue;

	pStrSend->pstrCamaraCtl[ECONTRAST].cCamaraType = ECONTRAST + 1;
	pStrSend->pstrCamaraCtl[ECONTRAST].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[ECONTRAST].fMaxVal = HDConf.MaxContrastValue;
	pStrSend->pstrCamaraCtl[ECONTRAST].fMinVal = HDConf.MinContrastValue;

	pStrSend->pstrCamaraCtl[EBRIGHTNESS].cCamaraType = EBRIGHTNESS + 1;
	pStrSend->pstrCamaraCtl[EBRIGHTNESS].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[EBRIGHTNESS].fMaxVal = HDConf.MaxBrightNessValue;
	pStrSend->pstrCamaraCtl[EBRIGHTNESS].fMinVal = HDConf.MinBrightNessValue;

	pStrSend->pstrCamaraCtl[EGAMMA].cCamaraType = EGAMMA + 1;
	pStrSend->pstrCamaraCtl[EGAMMA].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[EGAMMA].fMaxVal = HDConf.MaxGammaValue;
	pStrSend->pstrCamaraCtl[EGAMMA].fMinVal = HDConf.MinGammaValue;

	pStrSend->pstrCamaraCtl[ESHARPNESS].cCamaraType = ESHARPNESS + 1;
	pStrSend->pstrCamaraCtl[ESHARPNESS].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[ESHARPNESS].fMaxVal = HDConf.MaxSharpNessValue;
	pStrSend->pstrCamaraCtl[ESHARPNESS].fMinVal = HDConf.MinSharpNessValue;

	pStrSend->pstrCamaraCtl[ESATURATION].cCamaraType = ESATURATION + 1;
	pStrSend->pstrCamaraCtl[ESATURATION].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[ESATURATION].fMaxVal = HDConf.MaxSaturationValue;
	pStrSend->pstrCamaraCtl[ESATURATION].fMinVal = HDConf.MinSaturationValue;

	pStrSend->pstrCamaraCtl[EISO].cCamaraType = EISO + 1;
	pStrSend->pstrCamaraCtl[EISO].cCtlType = INTERVAL;
	pStrSend->pstrCamaraCtl[EISO].fMaxVal = HDConf.MaxIsoValue;
	pStrSend->pstrCamaraCtl[EISO].fMinVal = HDConf.MinIsoValue;

	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_CAMARA_CTL_INFO_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_PARAM_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

	return 0;
}

#if defined(CYC500XZD)||defined(CYC200XZD)

int CHandleXcsPtl::hdCamaraStaInfoQuery(XCS_CLIENT* pClintSocket)
{
	PSTR_CAMARA_STA_INFO_QUERY pClinetInfo = (PSTR_CAMARA_STA_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
	float fvalue;
	unsigned int iValue = 0;

	dc1394_feature_get_absolute_value(DC1394_FEATURE_EXPOSURE, &fvalue);
	pClinetInfo->fExposure = fvalue;
	//XCSPTL_INFO("-----------exposure val is %f \n", fvalue);

	dc1394_feature_get_absolute_value(DC1394_FEATURE_SHUTTER, &fvalue);
	pClinetInfo->fShutter = fvalue;
	//XCSPTL_INFO("-----------shutter val is %f \n", fvalue);

	dc1394_feature_get_absolute_value(DC1394_FEATURE_GAIN, &fvalue);
	pClinetInfo->fGain = fvalue;

	//XCSPTL_INFO("-----------gain val is %f \n", fvalue);

	dc1394_feature_get_absolute_value(DC1394_FEATURE_BRIGHTNESS, &fvalue);
	pClinetInfo->fBright = fvalue;

	ccdCSRValGet(REG_CAMERA_SHARPNESS, &iValue);
	pClinetInfo->fSharpness = iValue & 0xfff;

	dc1394_feature_get_absolute_value(DC1394_FEATURE_GAMMA, &fvalue);
	pClinetInfo->fGamma = fvalue;


	dc1394_feature_get_absolute_value(DC1394_FEATURE_SATURATION, &fvalue);
	pClinetInfo->fSaturation = fvalue;


	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_CAMARA_STA_INFO_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_STA_INFO_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
	return 0;
}

#else

int CHandleXcsPtl::hdCamaraStaInfoQuery(XCS_CLIENT* pClintSocket)
{
	char cIndex = 0;
	int ret = 0;
	PSTR_CAMARA_STA_INFO_QUERY pClinetInfo = (PSTR_CAMARA_STA_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
	CUR_CAMERA_STATUS strCurCamSta;
	memset(&strCurCamSta, 0, sizeof(strCurCamSta));
	strCurCamSta.cbSize = sizeof(CUR_CAMERA_STATUS);

	cIndex = camTimeRangeJudge();

	ret = cameraRunStaParamGet(0, &strCurCamSta);
#if 0
	pClinetInfo->fShutter = strCurCamSta.fCurShutter;
	shell_print("-----------shutter val is %f \n", pClinetInfo->fShutter);

	pClinetInfo->fGain = strCurCamSta.fCurGain;
	shell_print("-----------gain val is %f \n", pClinetInfo->fGain);

	pClinetInfo->fExposure = strCurCamSta.fCurGrey;
	shell_print("-----------ture grey val is %f \n", pClinetInfo->fExposure);

	pClinetInfo->fSaturation = strCurCamSta.curluminance;
	shell_print("-----------color adjust val is %f \n", pClinetInfo->fSaturation);

	pClinetInfo->fBright = strCurCamSta.curWdr;
	shell_print("-----------contrast val is %f \n", pClinetInfo->fBright);

	pClinetInfo->fSharpness = strCurCamSta.curContrast;
	shell_print("-----------minscale val is %f \n", pClinetInfo->fSharpness);

	pClinetInfo->fContrast = strCurCamSta.cur3D_noise;
	shell_print("-----------Awb val is %f \n", pClinetInfo->fAWB);

	pClinetInfo->fIso = strCurCamSta.curSaturation;
	shell_print("-----------sharpness val is %f \n", pClinetInfo->fIso);

	pClinetInfo->fGamma = strCurCamSta.curSharpness;
	XCSPTL_INFO("-----------gamma val is %f \n", pClinetInfo->fGamma);

	pClinetInfo->fAWB = strCurCamSta.cur2D_noise;
	shell_print("-----------deNoise val is %f \n", pClinetInfo->fContrast);

#else
	pClinetInfo->fShutter = strCurCamSta.fCurShutter;
	XCSPTL_INFO("-----------shutter val is %f \n", pClinetInfo->fShutter);

	pClinetInfo->fGain = strCurCamSta.fCurGain;
	XCSPTL_INFO("-----------gain val is %f \n", pClinetInfo->fGain);

	pClinetInfo->fExposure = strCurCamSta.fCurGrey;
	XCSPTL_INFO("-----------ture grey val is %f \n", pClinetInfo->fExposure);

	//pClinetInfo->fExposure = kodarGreyValGet(cIndex);
	//XCSPTL_INFO("-----------grey val is %f \n", pClinetInfo->fExposure);

	pClinetInfo->fSaturation = kodarSaturationValGet(cIndex);
	XCSPTL_INFO("-----------color adjust val is %f \n", pClinetInfo->fSaturation);

	pClinetInfo->fBright = kodarContrastValGet(cIndex);
	XCSPTL_INFO("-----------contrast val is %f \n", pClinetInfo->fBright);

	pClinetInfo->fSharpness = kodarSharpValGet(cIndex);
	XCSPTL_INFO("-----------minscale val is %f \n", pClinetInfo->fSharpness);

	pClinetInfo->fContrast = kodarAWBValGet(cIndex);
	XCSPTL_INFO("-----------Awb val is %f \n", pClinetInfo->fAWB);

	pClinetInfo->fIso = kodarIsoValGet(cIndex);
	XCSPTL_INFO("-----------sharpness val is %f \n", pClinetInfo->fIso);

	pClinetInfo->fGamma = kodarGammaValGet(cIndex);
	XCSPTL_INFO("-----------gamma val is %f \n", pClinetInfo->fGamma);

	pClinetInfo->fAWB = kodarDeNoiseValGet(cIndex);
	XCSPTL_INFO("-----------deNoise val is %f \n", pClinetInfo->fContrast);

#endif

	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_CAMARA_STA_INFO_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_STA_INFO_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
	return 0;
}

#endif

/******************************************************************************
 * CHandleXcsPtl.hdCamaraCtlInfoQuery() :
 *
 * DESCRIPTION:  协议的序号均是从1开始的，所以需要减一或加一
 *
 * Input:
 * Output:
 * Returns:
 *
 * modification history
 * --------------------
 *01a, 2010.01.13, wangbin Create
 * --------------------
 ******************************************************************************/
int CHandleXcsPtl::hdCamaraCtlInfoQuery(XCS_CLIENT* pClintSocket)
{
	int i = 0;
	int ret = 0;
	char cIndex = 0;
	CAMERA_CTRL_CONF_INFO strCamaCtlCfgInfo;

	PCAMARA_CTL_INFO_PC_QUERY pPcQueryInfo = (PCAMARA_CTL_INFO_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

	cIndex = pPcQueryInfo->cParamIndex;
	if (cIndex > 0)
	{
		cIndex -= 1;
	}
	if (cIndex > 4)
	{
		XCSPTL_ERROR("q cama ctl index is too big:%d.\n", cIndex);
		cIndex = 0;
	}
	XCSPTL_INFO("q cama ctl index:%d.\n", cIndex);
	PSTR_CAMARA_CTL_INFO_QUERY pStrSend = (PSTR_CAMARA_CTL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
	pStrSend->cCtrlNum = MAX_CAM_CTRL_NUM;
	for (i = 0; i < MAX_CAM_CTRL_NUM; i++)
	{
		ret = SysConfGet(e_CAMERA_CTRL_CONF, &strCamaCtlCfgInfo, (i + cIndex * MAX_CAM_CTRL_NUM));
		if (0 > ret)
		{
			XCSPTL_ERROR("get cama ctl Info Conf failed.\n");
			SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
			return -1;
		}
		pStrSend->pstrCamaraCtl[i].cParamIndex = strCamaCtlCfgInfo.cCamGroup + 1;
		pStrSend->pstrCamaraCtl[i].cCamaraType = strCamaCtlCfgInfo.uiCamaraType + 1;
		pStrSend->pstrCamaraCtl[i].cCtlType = strCamaCtlCfgInfo.cCtlType;
		pStrSend->pstrCamaraCtl[i].fMaxVal = strCamaCtlCfgInfo.fMaxVal;
		pStrSend->pstrCamaraCtl[i].fMinVal = strCamaCtlCfgInfo.fMinVal;
		if (FIX == strCamaCtlCfgInfo.cCtlType)
		{ pStrSend->pstrCamaraCtl[i].fMinVal = strCamaCtlCfgInfo.fFixVal; }

		/* 为了给白平衡配置RGB值所以做了以下映射 */
		if (strCamaCtlCfgInfo.uiCamaraType == ECONTRAST)
		{
			pStrSend->pstrCamaraCtl[i].cResv1[0] = (char)(strCamaCtlCfgInfo.fFixVal);
			pStrSend->pstrCamaraCtl[i].cResv1[1] = (char)(strCamaCtlCfgInfo.fMinVal);
			pStrSend->pstrCamaraCtl[i].cResv1[2] = (char)(strCamaCtlCfgInfo.fMaxVal);

			XCSPTL_INFO("whiteBalance get r:%f\n", strCamaCtlCfgInfo.fFixVal);
			XCSPTL_INFO("whiteBalance get b:%f\n", strCamaCtlCfgInfo.fMinVal);
			XCSPTL_INFO("whiteBalance get g:%f\n", strCamaCtlCfgInfo.fMaxVal);
		}

		/* MinScale最小值限制到2 */
		if (strCamaCtlCfgInfo.uiCamaraType == ESHARPNESS)
		{
			if (pStrSend->pstrCamaraCtl[i].fMinVal <= 2)
			{
				pStrSend->pstrCamaraCtl[i].fMinVal = 2;
			}
		}

#if 1
		XCSPTL_INFO("query cam type:0x%x\n", pStrSend->pstrCamaraCtl[i].cCamaraType);
		XCSPTL_INFO("q ctl type:0x%x\n", pStrSend->pstrCamaraCtl[i].cCtlType);
		XCSPTL_INFO("q maxval:%f\n", pStrSend->pstrCamaraCtl[i].fMaxVal);
		XCSPTL_INFO("q minval:%f\n", pStrSend->pstrCamaraCtl[i].fMinVal);
		XCSPTL_INFO("q fixval:%f\n", pStrSend->pstrCamaraCtl[i].fMinVal);
#endif

	}
	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_CAMARA_CTL_INFO_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_CTL_INFO_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

	return 0;
}

int CHandleXcsPtl::hdCamaraCtlInfoSet(XCS_CLIENT* pClintSocket)
{
	int i = 0;
	int ret = 0;
	char cCtlNum = 0;
	char cGroup = 0;
	STR_CAMARA_CTL_INFO_SET* pClientInfo = (STR_CAMARA_CTL_INFO_SET*)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
	CAMERA_CTRL_CONF_INFO strCamaCtlCfgInfo;

	memset(&strCamaCtlCfgInfo, 0, sizeof(strCamaCtlCfgInfo));
	cCtlNum = pClientInfo->cCtrlNum;
	cGroup = pClientInfo->cParamIndex;
	if (cGroup > 0)
	{
		cGroup -= 1;
	}
	XCSPTL_INFO("set cam ctl info index:%d,ctlNum:%d\n", cGroup, cCtlNum);
	for (i = 0; i < cCtlNum; i++)
	{
		strCamaCtlCfgInfo.uiCamaraType = pClientInfo->pstrCamaraCtl[i].cCamaraType - 1;
		strCamaCtlCfgInfo.cCtlType = pClientInfo->pstrCamaraCtl[i].cCtlType;
		strCamaCtlCfgInfo.fMaxVal = pClientInfo->pstrCamaraCtl[i].fMaxVal;
		strCamaCtlCfgInfo.fMinVal = pClientInfo->pstrCamaraCtl[i].fMinVal;
		strCamaCtlCfgInfo.fFixVal = pClientInfo->pstrCamaraCtl[i].fMinVal;
#if 1
		XCSPTL_INFO("set cam type:0x%x\n", strCamaCtlCfgInfo.uiCamaraType);
		XCSPTL_INFO("set ctl type:0x%x\n", strCamaCtlCfgInfo.cCtlType);
		XCSPTL_INFO("set maxval:%f\n", strCamaCtlCfgInfo.fMaxVal);
		XCSPTL_INFO("set minval:%f\n", strCamaCtlCfgInfo.fMinVal);
		XCSPTL_INFO("set fixval:%f\n", strCamaCtlCfgInfo.fFixVal);
#endif

		if (strCamaCtlCfgInfo.uiCamaraType > EISO)
		{
			//continue;
		}
		/* 为了给白平衡配置RGB值所以做了以下映射 */
		if (strCamaCtlCfgInfo.uiCamaraType == ECONTRAST)
		{
			strCamaCtlCfgInfo.fFixVal = pClientInfo->pstrCamaraCtl[i].cResv1[0];
			strCamaCtlCfgInfo.fMinVal = pClientInfo->pstrCamaraCtl[i].cResv1[1];
			strCamaCtlCfgInfo.fMaxVal = pClientInfo->pstrCamaraCtl[i].cResv1[2];
			XCSPTL_INFO("whiteBalance set r:%f\n", strCamaCtlCfgInfo.fFixVal);
			XCSPTL_INFO("whiteBalance set b:%f\n", strCamaCtlCfgInfo.fMinVal);
			XCSPTL_INFO("whiteBalance set g:%f\n", strCamaCtlCfgInfo.fMaxVal);
		}
		ret = SysConfSet(e_CAMERA_CTRL_CONF, &strCamaCtlCfgInfo, cGroup * MAX_CAM_CTRL_NUM + strCamaCtlCfgInfo.uiCamaraType);
		if (0 > ret)
		{
			XCSPTL_ERROR("set cama ctl Info Conf failed.\n");
			SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
			return -1;
		}
	}
	SaveCamCtrlConf();
	STR_SYS_APP_MSG strTmpSysMsg;
	memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
	strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
	strTmpSysMsg.uiMsgType = MSG_CAMARA_CTL_INFO_SET;
	ret = sendMsgToMainApp(&strTmpSysMsg);
	if (-1 == ret)
	{
		XCSPTL_ERROR("send cama ctl info Conf failed.\n");
	}
	else
	{
		XCSPTL_INFO("send cama ctl info msg ok.\n");
	}
	SetAckPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
	return 0;
}

int CHandleXcsPtl::hdCamaraTimeRQuery(XCS_CLIENT* pClintSocket)
{
	int ret = 0;
	char cIndex = 0;

	CAMERA_TIME_RANGE_INFO strSysCamaTimeRange;

	PCAMARA_TIME_RANGE_PC_QUERY pPcQueryInfo = (PCAMARA_TIME_RANGE_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

	cIndex = pPcQueryInfo->cParamIndex;
	if (cIndex > 0)
	{
		cIndex -= 1;
	}
	XCSPTL_INFO("q cam time range index:%d\n", cIndex);
	PXCS_CAMERA_TIME_RANGE_INFO_QUERY pStrSend = (PXCS_CAMERA_TIME_RANGE_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
	ret = SysConfGet(e_CAMERA_TIME_RANGE, &strSysCamaTimeRange, cIndex);
	if (0 > ret)
	{
		XCSPTL_ERROR("get cama time range Conf failed.\n");
		SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
	}
	pStrSend->cEnable = strSysCamaTimeRange.cEnable;
	pStrSend->cGroupId = strSysCamaTimeRange.cGroupId + 1;

	strcpy(pStrSend->szSceneName, strSysCamaTimeRange.szSceneName);
	strSysCamaTimeRange.szSceneName[29] = 0;
	pStrSend->iBeginTime = htonl(strSysCamaTimeRange.iBeginTime);
	pStrSend->iLastTime = htonl(strSysCamaTimeRange.iLastTime);

	pStrSend->cIrisPos = strSysCamaTimeRange.cIrisPos;

	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(XCS_CAMERA_TIME_RANGE_INFO_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_TIME_RANGE_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

	return 0;
}

int CHandleXcsPtl::hdCamaraTimeRSet(XCS_CLIENT* pClintSocket)
{
	int ret = 0;
	char cIndex = 0;
	XCS_CAMERA_TIME_RANGE_INFO_SET* pClientInfo = (XCS_CAMERA_TIME_RANGE_INFO_SET*)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
	CAMERA_TIME_RANGE_INFO strSysCamaTimeRange;

	memset(&strSysCamaTimeRange, 0, sizeof(strSysCamaTimeRange));
	cIndex = pClientInfo->cGroupId;
	if (cIndex > 0)
	{
		cIndex -= 1;
	}

	ret = SysConfGet(e_CAMERA_TIME_RANGE, &strSysCamaTimeRange, cIndex);
	if (0 > ret)
	{
		XCSPTL_ERROR("set get cama time range Conf failed.\n");
		SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
	}

	strSysCamaTimeRange.cEnable = pClientInfo->cEnable;
	if (0 == cIndex)
	{
		strSysCamaTimeRange.cEnable = 0;
		strSysCamaTimeRange.iBeginTime = 0;
		strSysCamaTimeRange.iLastTime = 23 * 3600 + 59 * 60;
	}
	strSysCamaTimeRange.cGroupId = cIndex;
	strSysCamaTimeRange.cIrisPos = pClientInfo->cIrisPos;

	if (strSysCamaTimeRange.cEnable > 0)
	{
		strcpy(strSysCamaTimeRange.szSceneName, pClientInfo->szSceneName);
		pClientInfo->szSceneName[29] = 0;
		strSysCamaTimeRange.iBeginTime = ntohl(pClientInfo->iBeginTime);
		strSysCamaTimeRange.iLastTime = ntohl(pClientInfo->iLastTime);

		XCSPTL_ERROR("index_%d,enable:%d,bT:%d,eT:%d.IrisPos\n", cIndex, strSysCamaTimeRange.cEnable, \
					 strSysCamaTimeRange.iBeginTime, strSysCamaTimeRange.iLastTime, strSysCamaTimeRange.cIrisPos);

		ret = camTimeRangeParamJudge(cIndex, strSysCamaTimeRange.iBeginTime, strSysCamaTimeRange.iLastTime);
		if (0 != ret)
		{
			XCSPTL_ERROR("failed:cama time range in other range.\n");
			SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
			return -1;
		}
	}
	ret = SysConfSet(e_CAMERA_TIME_RANGE, &strSysCamaTimeRange, cIndex);
	if (0 > ret)
	{
		XCSPTL_ERROR("set cama time range Conf failed.\n");
		SetNakPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
		return -1;
	}

	STR_SYS_APP_MSG strTmpSysMsg;
	memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
	strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
	strTmpSysMsg.uiMsgType = MSG_CAMARA_TIME_RANGE_SET;
	ret = sendMsgToMainApp(&strTmpSysMsg);
	if (-1 == ret)
	{
		XCSPTL_ERROR("send cama time range Conf failed.\n");
	}
	else
	{
		XCSPTL_INFO("send cama time range info msg ok.\n");
	}
	SetAckPack((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf));
	return ret;
}

int CHandleXcsPtl::hdCamaraDefaultParamQuery(XCS_CLIENT* pClintSocket)
{
	unsigned int i = 0;
	unsigned char cIndex = 0;
	CAMERA_CTRL_CONF_INFO* pstrCamaCtlCfgInfo;

	PCAMARA_CTL_INFO_PC_QUERY pPcQueryInfo = (PCAMARA_CTL_INFO_PC_QUERY)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);

	cIndex = pPcQueryInfo->cParamIndex;
	if (cIndex > 0)
	{
		cIndex -= 1;
	}
	if (cIndex > 4)
	{
		XCSPTL_ERROR("q cama default ctl index is too big:%d.\n", cIndex);
		cIndex = 0;
	}
	XCSPTL_INFO("q cama default ctl index:%d.\n", cIndex);
	PSTR_CAMARA_CTL_INFO_QUERY pStrSend = (PSTR_CAMARA_CTL_INFO_QUERY)(pClintSocket->pSendBuf + NT_PACK_HEAD_LENGTH);
	pStrSend->cCtrlNum = MAX_CAM_CTRL_NUM;
	pstrCamaCtlCfgInfo = &DefaultStrCameraConfVal[cIndex][0];
	for (i = 0; i < MAX_CAM_CTRL_NUM; i++)
	{
		pStrSend->pstrCamaraCtl[i].cParamIndex = pstrCamaCtlCfgInfo->cCamGroup;
		pStrSend->pstrCamaraCtl[i].cCamaraType = pstrCamaCtlCfgInfo->uiCamaraType + 1;
		pStrSend->pstrCamaraCtl[i].cCtlType = pstrCamaCtlCfgInfo->cCtlType;
		pStrSend->pstrCamaraCtl[i].fMaxVal = pstrCamaCtlCfgInfo->fMaxVal;
		pStrSend->pstrCamaraCtl[i].fMinVal = pstrCamaCtlCfgInfo->fMinVal;
		if (FIX == pstrCamaCtlCfgInfo->cCtlType)
		{ pStrSend->pstrCamaraCtl[i].fMinVal = pstrCamaCtlCfgInfo->fFixVal; }

		if (pstrCamaCtlCfgInfo->uiCamaraType == ECONTRAST)
		{
			pStrSend->pstrCamaraCtl[i].cResv1[0] = (char)(pstrCamaCtlCfgInfo->fFixVal);
			pStrSend->pstrCamaraCtl[i].cResv1[1] = (char)(pstrCamaCtlCfgInfo->fMinVal);
			pStrSend->pstrCamaraCtl[i].cResv1[2] = (char)(pstrCamaCtlCfgInfo->fMaxVal);

			XCSPTL_INFO("default whiteBalance get r:%f\n", pstrCamaCtlCfgInfo->fFixVal);
			XCSPTL_INFO("default whiteBalance get b:%f\n", pstrCamaCtlCfgInfo->fMinVal);
			XCSPTL_INFO("default whiteBalance get g:%f\n", pstrCamaCtlCfgInfo->fMaxVal);
		}

		pstrCamaCtlCfgInfo++;
	}
	pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_CAMARA_CTL_INFO_QUERY);
	SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_CAMARA, CPE_CAMARA_DEFAULT_CONF_QUERY, \
					 pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);

	return 0;
}

