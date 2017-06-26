/******************************************************************************
 * vehicleDcReport.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ��Ҫ���ڶԵشų�����Э����н��������ΪDSP������ץ�ĵ�ͨ���š�
 * �����š�����
 *     
 * modification history
 * --------------------
 * 01a, 2011.10.18, wangbin Create
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

extern unsigned long g_ulOrgTime;

/******************************************************************************
 * dcEPRealLaneNumGet() :
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
int dcEPRealLaneNumGet(unsigned char ucVHLaneId)
{
    unsigned int i =0;
    for(i =0; i< MAX_VEHICLE_NUM; i++)
    {
        if( ucVHLaneId == g_aEPConfInfo[i].cModuleId -1 )
        {
            return i;
        }
    }
    DSP_ERROR("EP no find ok client lane id =%d\n",ucVHLaneId);
    return -1;
}

int dcEPppSnapSeq(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime)
{
    int iTmpChannel =0;
    
    /* ͨ�����ҵ��Ӿ������ó�����Ϣ�еĳ�����������Ϣ���ҵ�ƥ�������ʵ�ʳ��� */
    iTmpChannel = dcEPRealLaneNumGet(ucCarChannel);
    if(-1 == iTmpChannel)
    {
        DSP_ERROR("ep config vehicle lane num %d is error\n",ucCarChannel);
        return -1;
    }
    DSP_INFO("pp snap vehicle lane id:%d,real lane id:%d\n", ucCarChannel, iTmpChannel);
	if(epPPPicSendFlag() == 1)
    {
        if ( COIL_FRONT_POS == ucCoilPos )// ǰ��Ȧ
        {
            if ( CAR_LEAVE_COIL == ucCoilSta )// ������ץ�ĵڶ���ͼƬ
            {
                dctlEPFlashNormalSnap(iTmpChannel, 1, SNAP_TYPE_CLIP);
            }
            return 0;
        }
	}
    return 0;
}

/******************************************************************************
 * dcEPSnapSeqQuery() :���ݳ�������Ϣ�����е��Ӿ���ץ�Ĳ���
 * 
 * DESCRIPTION:  6+1��ʱ��ÿһ����Ӧ���������أ���Ȼ�����456��
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.19, wangbin Create
 * --------------------
 ******************************************************************************/
int dcEPSnapSeqQuery(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime)
{
    int ret =0;
    int iPicMixType =0;
    static unsigned int s_uiDelayUS =0;
    unsigned long ulTimeOutVal =0;
    int iTmpChannel =0;
    unsigned long ulIntervalTime =0;
    unsigned long ulSecondTime =0;
    
    /* ����̬������Ϊ0xff��ʾ���¿��飬������ֱͬ�Ӷ��� */
    static unsigned char s_ucCarChan =0xff;

    struct timeval tv;
    struct timezone tz;

    if(ucCarDir == CAR_RUN_REVERSE_DIR )
    {
        DSP_INFO("in param car dir is REVERSE_DIR\n");
        return -1;
    }

    /* ͨ�����ҵ��Ӿ������ó�����Ϣ�еĳ�����������Ϣ���ҵ�ƥ�������ʵ�ʳ��� */
    iTmpChannel = dcEPRealLaneNumGet(ucCarChannel);
    if(-1 == iTmpChannel)
    {
        DSP_ERROR("ep config vehicle lane num is error\n");
        return -1;
    }
    /* �����ֶ�ץ�ĵĻ����ظ���Ȧץ��ֱ�Ӷ���,����״̬����Ϊԭ�� */
    if(g_ucEPHandSnapFlag)
    {
        DSP_INFO("ep hand snap is doning,throw lane snap!\n");
        goto quitFlag;
    }
    DSP_INFO("ep vehicle lane id:%d,real lane id:%d\n", ucCarChannel, iTmpChannel);
    ret = getTrafficLightSta(iTmpChannel);
    if( RED_LIGHT_OFF == ret)
    {
        DSP_INFO("red light is green\n");
         /* ���ǵ�ǰ����ץ�ĵ�ͨ����������״̬��ԭ�㣬��������ֱ�ӷ��� */
        if(iTmpChannel == s_ucCarChan)
        {
            /* ��ǰץ��ͨ����Ϊ�̵ƣ�ֱ�Ӷ�����ǰ��ץ�Ĳ��� */
            DSP_INFO("snap lane is green,throw\n");
            goto quitFlag;
        }
        else
        {
            DSP_INFO("current lane is green,throw\n");
            return 0;
        }
    }

    iPicMixType = dspEpSnapTypeGet(0);
	if ( COIL_FRONT_POS == ucCoilPos )// ǰ��Ȧ
	{
		if ( CAR_ENTER_COIL == ucCoilSta )//������ץ�ĵ�һ��ͼƬ
		{
		    if(0 == g_ucEPComSnapFlag)
		    {
		        setSimSnapRedTime(iTmpChannel);
                dctlEPFlashNormalSnap(iTmpChannel, 1, iPicMixType);
                g_ucEPComSnapFlag =1;
                s_ucCarChan = iTmpChannel;

                gettimeofday(&tv,&tz);
                g_ulOrgTime = tv.tv_sec*1000 + tv.tv_usec/1000;
                goto gotoRet;
            }
            else /* �����鵽�������жϵ�ǰץ�����Ƿ�ʱ��������ִ������ */
            {
                gettimeofday(&tv,&tz);
                ulSecondTime = tv.tv_sec*1000 + tv.tv_usec/1000;
                ulIntervalTime = ulSecondTime - g_ulOrgTime;
                
                if(SNAP_TYPE_6_PLUS_1 == iPicMixType)
	            {
	                ulTimeOutVal =6*1000;
	            }
	            else
	            {
	                ulTimeOutVal =3*1000;
	            }
                /* ��ǰ���Ѿ���ʱ�ˣ�ֱ���������� */
                if(ulTimeOutVal < ulIntervalTime)
                {
                    setSimSnapRedTime(iTmpChannel);
                    dctlEPFlashNormalSnap(iTmpChannel, 1, iPicMixType);
                    g_ucEPComSnapFlag =1;
                    s_ucCarChan = iTmpChannel;
                    
                    gettimeofday(&tv,&tz);
                    g_ulOrgTime = tv.tv_sec*1000 + tv.tv_usec/1000;
                    goto gotoRet;
                }
                else/* ��ǰ�黹δ�����ֱ꣬�Ӷ��� */
                {
                    return 0;
                }
            }
		}
		else if ( CAR_LEAVE_COIL == ucCoilSta )// ������ץ�ĵڶ���ͼƬ
		{
		    if((g_ucEPComSnapFlag) && (s_ucCarChan == iTmpChannel))
		    {
                dctlEPFlashNormalSnap(iTmpChannel, 2, iPicMixType);
                goto gotoRet;
            }
		}
		else
		{
		    DSP_ERROR("VH Snap coil status is error\n");
            goto gotoRet;
        }
	}
	else if (COIL_BEHIND_POS == ucCoilPos)// ����Ȧ
	{
        if ( CAR_LEAVE_COIL == ucCoilSta )// ������ץ�ĵ�����ͼƬ
		{
		    if((g_ucEPComSnapFlag) && (s_ucCarChan == iTmpChannel))
		    {
                if((SNAP_TYPE_3_PLUS_1 == iPicMixType)||(SNAP_TYPE_3_SEPARATE == iPicMixType )\
                || (SNAP_TYPE_3_PLUS_1_LEFT == iPicMixType) )
    	        {
    	            /* Ϊ���ܿ�������·�ں����ʻ�������Լ���һ����ʱ */
    	            s_uiDelayUS = 3000000;//force to 3s by yuanjc
    	            usleep(s_uiDelayUS);
                    dctlEPFlashNormalSnap(iTmpChannel, 3, iPicMixType);
    	            goto quitFlag;
    	        }
    	        else
    	        {
    	            dctlEPFlashNormalSnap(iTmpChannel, 3, iPicMixType);
    	        }
    	    }
    	    else
    	    {
    	        DSP_INFO("step 3 static lane id:%d,real lane id:%d\n", s_ucCarChan, iTmpChannel);
    	        return 0;
    	    }
		}
		else
		{
		    goto gotoRet;
		}
	}
	else
	{
	    DSP_ERROR("VH Snap coil pos is error\n");
        goto gotoRet;
	}
	if((SNAP_TYPE_6_PLUS_1 == iPicMixType) && ( s_ucCarChan == iTmpChannel ))
	{
	    if(s_uiDelayUS > 8000000)
	    {
	        //mcLog(LMT_SYSCALL, LT_INFO, "warning:ep delay us is %d,too big change to 8000000!\n", s_uiDelayUS);
	        s_uiDelayUS = 8000000;
	    }
        usleep(s_uiDelayUS/4);
	    dctlEPFlashNormalSnap(iTmpChannel, 4, iPicMixType);
	    usleep(s_uiDelayUS/4);
	    dctlEPFlashNormalSnap(iTmpChannel, 5, iPicMixType);
	    usleep(s_uiDelayUS/2);
	    dctlEPFlashNormalSnap(iTmpChannel, 6, iPicMixType);
	    sleep(1);
	    goto quitFlag;
	}
gotoRet:
	return 0;

quitFlag:
    g_ucEPComSnapFlag =0;
    g_ucEPComBufCleanFlag =1;
    s_ucCarChan = 0xff;
    return 0;
}

/******************************************************************************
 * dcPPRealLaneNumGet() :
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
int dcPPRealLaneNumGet(unsigned char ucVHLaneId)
{
    unsigned int i =0;
    for(i =0; i< g_uiClientLaneSum; i++)
    {
        if( ucVHLaneId == g_aPassPortChanInfo[i].cModuleId -1 )
        {
            return i;
        }
    }
    DSP_INFO("no find ok vehilce lane id\n");
    return -1;
}

/******************************************************************************
 * dcPPSnapSeqQuery() :���ݳ�������Ϣ�����г�����ץ��
 * 
 * DESCRIPTION:  
 *  
 * Input: 
 * Output: 
 * Returns: 
 * ע��:ץ�Ĳ����Ǵ�1��ʼ�ģ����ϵ�ץ�Ĳ����Ǵ�0��ʼ��
 * modification history
 * --------------------
 *01a, 2009.12.19, wangbin Create
 * --------------------
 ******************************************************************************/
int dcPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime)
{
    int ret =0;
    int iSnapFlag =0;
    int iTmpChannel =0;
    unsigned int uiCarSpeed =0;
    unsigned int uiCarInfo =0;
    static unsigned int s_uiCarSpeed =0;
    static unsigned int s_uiCarLen =0;
    static int s_iStep0Speed =0;
    static int s_iStep1Speed =0;
    static int s_iStep2Speed =0;
    static int s_iStep3Speed =0;
    
    ret = dcPPRealLaneNumGet(ucCarChannel);
    if(-1 == ret)
    {
        DSP_INFO("vehicle lane id is not right\n");
        return 0;
    }

    iTmpChannel = ret;


    if((g_PPLaneSwitchInfo[iTmpChannel].cCoilDecType == GROUD_LOOP) \
    && ( (g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == GROUD_SINGAL_COIL_NO_SPEED)
    ||(g_PPLaneSwitchInfo[iTmpChannel].cGroudCoilDecType == GROUD_SINGAL_COIL_SPEED) ))
    {
        if ( COIL_FRONT_POS == ucCoilPos )// ǰ��Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//����
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
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//����
    		{
    		    s_iStep1Speed = usLastTime;
    		    if(( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilPos == SINGAL_COIL_POS_FRONT) \
                && ( g_PPLaneSwitchInfo[iTmpChannel].cSingalCoilSnapType == SINGAL_COIL_LEAVE_SNAP ))
                {
                    iSnapFlag =1;
                }
    		}
    	}
    	else if (COIL_BEHIND_POS == ucCoilPos)// ����Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//����
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
    		        DSP_ERROR("kj error1:coil pass time is 0!\n");
    		        usLastTime = 1000;
    		    }
    		    DSP_INFO("singal coil car real pass time is %d!\n", usLastTime);
                s_uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("pp singal coil,snap coil distance:%d cm,car speed:%dkm/h,delayTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, s_uiCarSpeed, usLastTime);

                /* ���㳵�� */
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
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//����
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
    	if ( COIL_FRONT_POS == ucCoilPos )// ǰ��Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//������ץ�ĵ�һ��ͼƬ
    		{
    		    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, ucCarDir);
    		    if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp Snap 0 ok!\n", ret);
    			}
    		}
    	}
    	else if (COIL_BEHIND_POS == ucCoilPos)// ����Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )// ���������ó��ٲ�����ץ�ĵڶ���ͼƬ
    		{
    		    if(0 == usLastTime)
    		    {
    		        //mcLog(LMT_SYSCALL, LT_INFO, "error:pp vehicle report speed time is 0,default val is 1000!\n");
    		        usLastTime = 1000;
    		    }
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
    			    DSP_INFO("pp snap 1 ok!\n");
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
        if ( COIL_BEHIND_POS == ucCoilPos )// ����Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//������ץ�ĵ�һ��ͼƬ
    		{
    		    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, ucCarDir);
    		    if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp reverse Snap 0 ok!\n", ret);
    			}
    		}
    	}
    	else if ( COIL_FRONT_POS == ucCoilPos)// ǰ��Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )// ���������ó��ٲ�����ץ�ĵڶ���ͼƬ
    		{
    		    if(0 == usLastTime)
    		    {
    		        //mcLog(LMT_SYSCALL, LT_INFO, "error1:pp vehicle report speed time is 0,default val is 1000!\n");
    		        usLastTime = 1000;
    		    }
                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("pp redir snap coil distance:%d cm,car speed:%dkm/h,delayTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
        	    
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarSpeed, ucCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp reverse snap 1 ok!\n");
    			}
    		}
    	}
    	else
    	{
    	    return -1;
    	}
    }
	return 0;
}

