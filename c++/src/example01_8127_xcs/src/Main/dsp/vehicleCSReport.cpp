/******************************************************************************
 * csVehicleReport.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  主要用于对川速雷达协议进行解析，结果为DSP可用于抓拍的通道号、
 * 车道号、步骤
 *     
 * modification history
 * --------------------
 * 01a, 2011.09.05, wangbin Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include "xcsGlobalDef.h"
#include "dsp_define.h"
#include "vehicleReport.h"
#include "vehicleBasePro.h"
#include "dspParamManage.h"

extern "C" {
#include "dsp.h"
}

#include "SysHandle.h"

extern CARROAD_DECT_CONF g_aPassPortChanInfo[MAX_VEHICLE_NUM];
extern PP_LANE_SWITCH_STA_INFO g_PPLaneSwitchInfo[MAX_VEHICLE_NUM];

/******************************************************************************
 * csPPRealLaneNumGet() :
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2011.09.05, wangbin Create
 * --------------------
 ******************************************************************************/
int csPPRealLaneNumGet(unsigned char ucVHLaneId)
{
    unsigned int i =0;
    for(i =0; i< g_uiClientLaneSum; i++)
    {
        if( ucVHLaneId == g_aPassPortChanInfo[i].cModuleId -1 )
        {
            return i;
        }
    }
    DSP_INFO("no find ok cs vehilce lane id\n");
    return -1;
}


/******************************************************************************
 * csPPSnapSeqQuery() :根据车检器信息，进行出卡口抓拍
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 注意:抓拍步骤是从1开始的，而老的抓拍步骤是从0开始的
 * modification history
 * --------------------
 *01a, 2011.09.05, wangbin Create
 * --------------------
 ******************************************************************************/
int csPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCarSpeed, unsigned char ucCarDir)
{
    int ret =0;
    int iTmpChannel =0;
    unsigned int uiCarInfo =0;
    unsigned short usSnapInter = 75;
    
    if(MAX_VEHICLE_NUM <= ucCarChannel)
    {
        DSP_ERROR("in param carChannel error\n");
        return -1;
    }
    
    if( ucCarSpeed == 0 )
    {
        /* 由于川速雷达在空闲时也一直发速度0，所以当速度为0时不进行处理 */
        return 0;
    }

    usSnapInter = dspPPSnapInterValGet(0, iTmpChannel);
    
    if((g_PPLaneSwitchInfo[iTmpChannel].cCoilDecType == GROUD_LOOP) \
    && (g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == CS_RADAR_SPEED) )
    {
        uiCarInfo = 0;
        uiCarInfo = (uiCarInfo << 16);
        uiCarInfo |= ucCarSpeed;
        ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, CAR_RUN_CORRECT_DIR);
        usleep(usSnapInter*1000);
        ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarInfo, CAR_RUN_CORRECT_DIR);
    }
    else if( (g_PPLaneSwitchInfo[iTmpChannel].cCoilDecType == VIRTUAL_LOOP) \
    && (g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == CS_RADAR_SPEED) )
    {
        uiCarInfo = 0;
        uiCarInfo = (uiCarInfo << 16);
        uiCarInfo |= ucCarSpeed;

        DSP_INFO("csPPSnapSeqQuery uiCarInfo:0x%x\n", uiCarInfo);
        ret = dctlRadarNormalSnap(iComId, iTmpChannel, 1, uiCarInfo, ucCarDir);
        if( 0 != ret)
    	{
    	    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    	}
    	else
    	{
    	    DSP_INFO("cs radar pp Snap ok,carInfo:0x%x!\n", uiCarInfo);
    	}
    }
    else
    {
        DSP_ERROR("csPPSnapSeqQuery coil type:%d coil dec type:%d is error!\n", \
        g_PPLaneSwitchInfo[iTmpChannel].cCoilDecType,\
        g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType);
    }
	return 0;

}


