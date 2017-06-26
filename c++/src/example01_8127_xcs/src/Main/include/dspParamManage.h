/******************************************************************************
 * dspParamManage.h:
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
#ifndef __DSP_PARAM_MANAGE_H__
#define __DSP_PARAM_MANAGE_H__

int cameraDirectWriteReg(unsigned int  rag_addr,unsigned int value);
int cameraDirectReadReg(unsigned int reg_addr,unsigned int *pvalue);

void zebraFrameRateSet(void);
void zebraCamStrobeFlashEnable(void);
void zebraCamStrobeFlashDisable(void);

int dspCamParamSet(char cGroup);

int dspEncParamSet(int iChanId);

int newIspTotalLinesGet(void);

int dspEPLaneAreaSet(char cMode, unsigned int uiLane);
int dspVLFlashModeSet();
int dspEpSetRunNotLaneDir(int iLane, int iMode, int iDir);
// 设置红灯偏色校正区域
int dspEPRLDecModeSet(void);
int dspEPRedLightAreaSet(char cMode, char cDecFlag);
int dspEPSnapOsdSet(char cMode, unsigned int uiLane);
int dspEpPpSnapOsdSet(char cMode, unsigned int uiLane);
int dspEPRedLightDetectTypeGet(unsigned char ucChannelNum);
int dspEPCoilTypeSet(char cMode, unsigned int uiLane);
int dspEpSnapTypeGet(char cChanId);
int dspEPCoilTypeGet(char cChanId);
int dspEPVLAreaSet(char cMode, unsigned int uiLane);
int dspEpFuncEn(char cMode, unsigned int uiLane);
int dspEPRedLightSet(char cMode, unsigned int uiLane);
int epTimeRangeJudge(char cLaneId);
int dspEPACSwitchParamGet(int *piDecCycle);
int dspEpACPhaseSwitchDec(char cMode);
int dspEpLPRParamSet(int iChanId);
int dspEPIrregNameInfoSet(void);
int dspEPSOTNoEntrySet(char cMode, unsigned int uiLane);
int dspEpFillInLightSwitchDec(char cMode);
int dspEPFILightSmartSwitchGet(void);
int	dspEpTimerSwitchEnGet(void);
int dspEpTimerSwitchDec(char cMode);
int dspEpJpgOsdConfSet(int iChanId);

/* 以下为卡口函数调用声明 */
int olviaParalWorkModeGet(void);
int dspPpFunEnSet(char cMode, unsigned int uiLane);
int dspPPSnapOsdSet(int iChanId, int iLaneId);
int dspPPVSpeedFlagSet(void);
int dspPPVLParamSet(int iChanel, int iLaneId);
int dspPPCoilTypeSet(int iChanel, int iLaneId);
int dspPPLimitSpeedSet(int iChanId, int iLaneId);
int dspPPSnapAreaSet(int iChanel, int iLaneId);
int dspPPUnidirLaneTimeSet(int iChanel, int iLaneId);
int dspPPCoilTypeGet(int iChanel, int iLaneId);
int dspPPHighVLParamSet(int iChanel, int iLaneId);

int dspLPRParamSet(int iChanId);

int dspPpFuncEn(unsigned int uiLane);
int dspPPFillInLightParamGet(void);
int dspPpFillInLightSwitchDec(char cMode);

unsigned short dspPPLimitSpeedGet(int iChanId, int iLaneId);
unsigned short dspPPSnapInterValGet(int iChanId, int iLaneId);

int dspPpTimerSwitchEnGet(void);
int dspPpTimerSwitchDec(char cMode);
int dspPpJpgOsdConfSet(int iChanId);
int dspPPIrregNameInfoSet(void);

int timerRebootProcess(void);


#endif /* __DSP_PARAM_MANAGE_H__ */

