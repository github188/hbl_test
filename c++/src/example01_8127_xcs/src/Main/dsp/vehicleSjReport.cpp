/******************************************************************************
 * kjVehicleReport.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  主要用于对苏江车检器协议进行解析，结果为DSP可用于抓拍的通道号、
 * 车道号、步骤
 *     
 * modification history
 * --------------------
 * 01a, 2011.01.04, wangbin Create
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

extern CARROAD_DECT_CONF g_aEPConfInfo[MAX_VEHICLE_NUM];
extern CARROAD_DECT_CONF g_aPassPortChanInfo[MAX_VEHICLE_NUM];

extern unsigned char g_ucEPComSnapFlag;
extern unsigned char g_ucEPComBufCleanFlag;
extern unsigned char g_ucEPHandSnapFlag;

extern PP_LANE_SWITCH_STA_INFO g_PPLaneSwitchInfo[MAX_VEHICLE_NUM];

/******************************************************************************
 * sjPPRealLaneNumGet() :
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.27, wangbin Create
 * --------------------
 ******************************************************************************/
int sjPPRealLaneNumGet(unsigned char ucVHLaneId)
{
    unsigned int i =0;
    for(i =0; i< g_uiClientLaneSum; i++)
    {
        if( ucVHLaneId == g_aPassPortChanInfo[i].cModuleId -1 )
        {
            return i;
        }
    }
    DSP_INFO("no find ok sj vehilce lane id\n");
    return -1;
}


/******************************************************************************
 * sjPPSnapSeqQuery() :根据车检器信息，进行出卡口抓拍
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 注意:抓拍步骤是从1开始的，而老的抓拍步骤是从0开始的
 * modification history
 * --------------------
 *01a, 2011.01.04, wangbin Create
 * --------------------
 ******************************************************************************/
int sjPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime)
{
    int ret =0;
    int iTmpChannel =0;
    int iSnapFlag =0;
    unsigned int uiCarInfo =0;
    unsigned int uiCarSpeed =0;
    static unsigned int s_uiCarSpeed =0;
    static unsigned int s_uiCarLen =0;
    static int s_iStep0Speed =0;
    static int s_iStep1Speed =0;
    static int s_iStep2Speed =0;
    static int s_iStep3Speed =0;
    
    if(MAX_VEHICLE_NUM <= ucCarChannel)
    {
        DSP_ERROR("in param carChannel error\n");
        return -1;
    }
    ret = sjPPRealLaneNumGet(ucCarChannel);
    if(-1 ==  ret)
    {
        DSP_INFO("vehicle lane id is not right\n");
        return 0;
    }
    
    iTmpChannel = ret;

    if((g_PPLaneSwitchInfo[iTmpChannel].cCoilDecType == GROUD_LOOP) \
    && ( (g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == GROUD_SINGAL_COIL_NO_SPEED)
    ||(g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == GROUD_SINGAL_COIL_SPEED) ))
    {
        if ( COIL_FRONT_POS == ucCoilPos )// 前线圈
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//进车
    		{
    		    s_uiCarLen = 0;
    		    s_uiCarSpeed =0;
    		    
    		    s_iStep0Speed = usLastTime;
                if(( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilPos == SINGAL_COIL_POS_FRONT) \
                && ( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilSnapType == SINGAL_COIL_ENTER_SNAP ))
                {
                    iSnapFlag =1;
                }
    		}
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//出车
    		{
    		    s_iStep1Speed = usLastTime;
    		    if(( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilPos == SINGAL_COIL_POS_FRONT) \
                && ( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilSnapType == SINGAL_COIL_LEAVE_SNAP ))
                {
                    iSnapFlag =1;
                }
    		}
    	}
    	else if (COIL_BEHIND_POS == ucCoilPos)// 后线圈
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//进车
    		{
    		    s_iStep2Speed = usLastTime;

                if( usLastTime > s_iStep0Speed)
    		    {
    		        usLastTime = usLastTime - s_iStep0Speed;
    		    }
    		    else
    		    {
    		        usLastTime = usLastTime + ( 0xffff - s_iStep0Speed +1);
    		    }
    		    if( usLastTime == 0)
    		    {
    		        DSP_ERROR("sj error1:coil pass time is 0!\n");
    		        usLastTime = 1000;
    		    }
    		    DSP_INFO("singal coil car real pass time is %d!\n", usLastTime);
                s_uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("pp singal coil,snap coil distance:%d cm,car speed:%dkm/h,delayTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, s_uiCarSpeed, usLastTime);

                /* 计算车长 */
                if( s_iStep1Speed > s_iStep0Speed)
    		    {
    		        usLastTime = s_iStep1Speed - s_iStep0Speed;
    		    }
    		    else
    		    {
    		        usLastTime = s_iStep1Speed + ( 0xffff - s_iStep0Speed +1);
    		    }
                s_uiCarLen = (s_uiCarSpeed*usLastTime)/36 - g_aPassPortChanInfo[iTmpChannel].usSingalCoilHigh;
                DSP_INFO("pp singal coil,car len:%d,car speed:%d, last time:%d\n", s_uiCarLen, s_uiCarSpeed, usLastTime);
    		    
                if(( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilPos == SINGAL_COIL_POS_BEHIND) \
                && ( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilSnapType == SINGAL_COIL_ENTER_SNAP ))
                {
                    iSnapFlag =1;
                }
    		}
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//出车
    		{
    		    s_iStep3Speed = usLastTime;
    		    if(( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilPos == SINGAL_COIL_POS_BEHIND) \
                && ( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilSnapType == SINGAL_COIL_LEAVE_SNAP ))
                {
                    iSnapFlag =1;
                }
    		}
    	}

        if(iSnapFlag > 0)
        {
            if(g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == GROUD_SINGAL_COIL_NO_SPEED)
            {
                uiCarInfo = 0;
            }
            else if(g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == GROUD_SINGAL_COIL_SPEED)
            {
                uiCarInfo = s_uiCarLen;
                uiCarInfo = (uiCarInfo << 16);
                uiCarInfo |= s_uiCarSpeed;
            }
            DSP_ERROR("dctlPPFlashNormalSnap uiCarInfo:0x%x\n", uiCarInfo);
            ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, uiCarInfo, CAR_RUN_NO_DIR);
		    if( 0 != ret)
			{
			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
			}
			else
			{
			    DSP_INFO("singal coil pp Snap ok,carInfo:0x%x!\n", ret, uiCarInfo);
			}
        }
        else
        {
            DSP_INFO("%s line:%d:iSnapFlag:%d\n", __FUNCTION__, __LINE__, iSnapFlag);
        }
		return 0;
    }


    if(CAR_RUN_CORRECT_DIR == ucCarDir)
    {
    	if ( COIL_FRONT_POS == ucCoilPos )// 前线圈
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//进车，抓拍第一张图片
    		{
    		    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, ucCarDir);
    		    if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    s_iStep0Speed = usLastTime;
    			    DSP_INFO("sj pp Snap 0 ok, front coil pass time is %d!\n", s_iStep0Speed);
    			}
    		}
    	}
    	else if (COIL_BEHIND_POS == ucCoilPos)// 后线圈
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )// 进车，设置车速并触发抓拍第二张图片
    		{
    		    DSP_INFO("behind coil pass time is %d!\n", usLastTime);
    		    if( usLastTime > s_iStep0Speed)
    		    {
    		        usLastTime = usLastTime - s_iStep0Speed;
    		    }
    		    else
    		    {
    		        usLastTime = usLastTime + ( 0xffff - s_iStep0Speed +1);
    		    }
    		    if( usLastTime == 0)
    		    {
    		        DSP_ERROR("sj error:coil pass time is 0!\n");
    		        usLastTime = 1000;
    		    }
    		    DSP_INFO("car real pass time is %d!\n", usLastTime);
                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("pp dir snap coil distance:%d cm,car speed:%dkm/h,delayTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarSpeed, ucCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("sj pp snap 1 ok!\n");
    			}
    		}
    	}
    	else
    	{
    	    return -1;
    	}
    }
    else if(CAR_RUN_REVERSE_DIR == ucCarDir)
    {
        if ( COIL_BEHIND_POS == ucCoilPos )// 后线圈
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//进车，抓拍第一张图片
    		{
    		    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, ucCarDir);
    		    if( 0 != ret)
    			{
    			    DSP_ERROR("sj dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("sj pp reverse Snap 0 ok!\n", ret);
    			}
    		}
    	}
    	else if ( COIL_FRONT_POS == ucCoilPos)// 前线圈
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )// 进车，设置车速并触发抓拍第二张图片
    		{
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarSpeed, ucCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("sj dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("sj pp reverse snap 1 ok!\n");
    			}
    		}
    	}
    	else
    	{
    	    return -1;
    	}
    }
    else
    {
        DSP_ERROR("sj pp no dir mode,do nothing!\n");
    }
	return 0;
}

