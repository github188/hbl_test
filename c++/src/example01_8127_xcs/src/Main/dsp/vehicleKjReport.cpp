/******************************************************************************
 * kjVehicleReport.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ��Ҫ���ڶԿƿ�������Э����н��������ΪDSP������ץ�ĵ�ͨ���š�
 * �����š�����
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.19, wangbin Create
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

unsigned long g_ulOrgTime =0;

#define ADJUST_SPEED_EN 0

/* �����ٶȼ��㳬ʱ���� */
typedef struct ppProtoReachTime
{
    unsigned int uiReachTimeSec;
    unsigned int uiReachTimeMsec;
    unsigned int uiOkFlag;
    unsigned int uiSpeed1;
    unsigned int uiSpeed2;
    unsigned int uiLastMs2;
}STR_PP_PROTO_REACH_TIME;


/******************************************************************************
 * kjEPRealLaneNumGet() :
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
int kjEPRealLaneNumGet(unsigned char ucVHLaneId)
{
    unsigned int i =0;
    for(i =0; i< MAX_VEHICLE_NUM; i++)
    {
        if( ucVHLaneId == g_aEPConfInfo[i].cModuleId -1 )
        {
            return i;
        }
    }
    DSP_ERROR("no find ok client lane id\n");
    return -1;
}

int kjEPppSnapSeq(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime)
{
    char cCoilPosion =0;
    unsigned char ucResverSnapFlag =0;
    unsigned char ucCoilSetPosition =0;
    int iTmpChannel =0;
    
    if(MAX_VEHICLE_NUM <= ucCarChannel)
    {
        DSP_ERROR("in param carChannel error\n");
        return -1;
    }
    /* ͨ�����ҵ��Ӿ������ó�����Ϣ�еĳ�����������Ϣ���ҵ�ƥ�������ʵ�ʳ��� */
    iTmpChannel = kjEPRealLaneNumGet(ucCarChannel);
    if(-1 == iTmpChannel)
    {
        DSP_ERROR("ep config vehicle lane num is error\n");
        return -1;
    }
    
    if(ucCarDir == CAR_RUN_REVERSE_DIR )
    {
        ucResverSnapFlag = ePReservSnapFlagGet();
        if( ucResverSnapFlag <= 0)
        {
            DSP_INFO("ep resver snap flag is 0,return!\n");
            return -1;
        }
    }
    
    DSP_INFO("pp snap vehicle lane id:%d,real lane id:%d\n", ucCarChannel, iTmpChannel);
	if(epPPPicSendFlag() == 1)
    {
        if ( COIL_FRONT_POS == ucCoilPos )
        {
            if( CAR_ENTER_COIL == ucCoilSta ) //����ǰ��Ȧ
            {
                cCoilPosion =0;
            }
            else if ( CAR_LEAVE_COIL == ucCoilSta )// �뿪ǰ��Ȧ
            {
                cCoilPosion =2;
            }
        }
        else if ( COIL_BEHIND_POS == ucCoilPos )
        {
            if( CAR_ENTER_COIL == ucCoilSta ) //����ǰ��Ȧ
            {
                cCoilPosion =1;
            }
            else if ( CAR_LEAVE_COIL == ucCoilSta )// �뿪ǰ��Ȧ
            {
                cCoilPosion =3;
            }
        }
        ucCoilSetPosition = epPpSnapPositionGet();
        DSP_INFO("pp snap vehicle position:%d,set position:%d\n", cCoilPosion, ucCoilSetPosition);
        
        if(ucCoilSetPosition == cCoilPosion)
        {
            dctlEPFlashNormalSnap(iTmpChannel, 1, SNAP_TYPE_CLIP);
        }
	}
    return 0;
}

/******************************************************************************
 * kjEPSnapSeqQuery() :���ݳ�������Ϣ�����е��Ӿ���ץ�Ĳ���
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
int kjEPSnapSeqQuery(unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime)
{
    int ret =0;
    int iPicMixType =0;
    static unsigned int s_uiDelayUS =0;
    unsigned long ulTimeOutVal =0;
    int iTmpChannel =0;
    unsigned long ulIntervalTime =0;
    unsigned long ulSecondTime =0;
    static unsigned int s_uiCarSpeed =0;
    char cCarDistance =20;
    
    /* ����̬������Ϊ0xff��ʾ���¿��飬������ֱͬ�Ӷ��� */
    static unsigned char s_ucCarChan =0xff;

    struct timeval tv;
    struct timezone tz;

    if(ucCarDir == CAR_RUN_REVERSE_DIR )
    {
        DSP_INFO("in param car dir is REVERSE_DIR\n");
        return -1;
    }

    if(MAX_VEHICLE_NUM <= ucCarChannel)
    {
        DSP_ERROR("in param carChannel error\n");
        return -1;
    }
    /* ͨ�����ҵ��Ӿ������ó�����Ϣ�еĳ�����������Ϣ���ҵ�ƥ�������ʵ�ʳ��� */
    iTmpChannel = kjEPRealLaneNumGet(ucCarChannel);
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
	    if ( CAR_ENTER_COIL == ucCoilSta )// ���������㳵��
		{
		    if(0 == usLastTime)
		    {
		        //mcLog(LMT_SYSCALL, LT_INFO, "warning:ep vehicle report speed time is 0,default val is 1000!\n");
		        usLastTime = 1000;
		    }
            s_uiCarSpeed = (g_aEPConfInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
            if(4 >= s_uiCarSpeed)
            {
                #if 0
                mcLog(LMT_SYSCALL, LT_INFO, "warning:ep vehicle speed time is %d, last time is %d,distance is %d!\n", \
                s_uiCarSpeed, usLastTime, g_aEPConfInfo[iTmpChannel].usCoilDistance);
                #endif
                s_uiCarSpeed =4;
            }
            
            /* 2010-6-1 add by wangb for �������������������ʱ */
            if(g_usEpXcsProVer == XCS_PRO_EPG_VER_2)
            {
                cCarDistance= ePLaneCarDistanceGet(iTmpChannel);
            }
            else
            {
                cCarDistance= ePCarDistanceGet();
            }
    	    s_uiDelayUS = cCarDistance*3600*1000/s_uiCarSpeed;
    	    DSP_INFO("lane:%d,ep snap coil distance:%d cm,car speed:%dkm/h,delayTime:%dus,car Distance:%d m\n", \
    	    iTmpChannel,g_aEPConfInfo[iTmpChannel].usCoilDistance, s_uiCarSpeed, s_uiDelayUS, cCarDistance);

            goto gotoRet;
        }
		else if ( CAR_LEAVE_COIL == ucCoilSta )// ������ץ�ĵ�����ͼƬ
		{
		    if((g_ucEPComSnapFlag) && (s_ucCarChan == iTmpChannel))
		    {
                if((SNAP_TYPE_3_PLUS_1 == iPicMixType)||(SNAP_TYPE_3_SEPARATE == iPicMixType )\
                || ( SNAP_TYPE_3_PLUS_1_LEFT == iPicMixType ))
                {
    	            /* Ϊ���ܿ�������·�ں����ʻ�������Լ���һ����ʱ */
    	            if(s_uiDelayUS > 6000000)
    	            {
    	                DSP_INFO("info1:distance:%d cm,car speed:%dkm/h,delayTime:%dus\n", \
    	                g_aEPConfInfo[iTmpChannel].usCoilDistance, s_uiCarSpeed, s_uiDelayUS);
    	                s_uiDelayUS = 6000000;
    	            }
    	            else if(s_uiDelayUS < 800000)
    	            {
    	                DSP_INFO("info2: distance:%d cm,car speed:%dkm/h,delayTime:%dus\n", \
    	                g_aEPConfInfo[iTmpChannel].usCoilDistance, s_uiCarSpeed, s_uiDelayUS);
    	                s_uiDelayUS = 800000;
    	            }
    	            usleep(s_uiDelayUS/2);
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
 * kjPPRealLaneNumGet() :
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
int kjPPRealLaneNumGet(unsigned char ucVHLaneId)
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
 * kjPPSnapSeqQuery() :���ݳ�������Ϣ�����г�����ץ��
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
int kjPPSnapSeqQuery(unsigned int iComId, unsigned char ucCarChannel, unsigned char ucCoilPos, unsigned char ucCoilSta, unsigned char ucCarDir, unsigned short usLastTime, unsigned short usGroupId)
{
    int ret =0;
    int iSnapFlag =0;
    int iTmpChannel =0;
    int iTmpInterlSpeed =0;
    
    unsigned int uiCarDir =0;
    unsigned int uiCarSpeed =0;
    unsigned int uiCarInfo =0;
    unsigned short usMaxSpeed =0;
    static unsigned int s_uiMsTime =0;
    static unsigned int s_uiCarSpeed =0;
    static unsigned int s_uiCarLen =0;
    static int s_iStep0Speed =0;
    static int s_iStep1Speed =0;
    static int s_iStep2Speed =0;
    static int s_iStep3Speed =0;

	/* ����ץ���ٶ��Ƿ���ȷ״̬��־ */
    static int s_iSpeedOkFlag[ MAX_LANE_NUM ] = { 0 };
    
	/* ����ץ���ٶ��Ƿ���ȷ״̬��־ */    
    static int s_iRsvSpeedOkFlag[ MAX_LANE_NUM ] = { 0 };    

	static STR_PP_PROTO_REACH_TIME strLaneProtoTime[MAX_LANE_NUM] = { 0 };

	static STR_PP_PROTO_REACH_TIME strRsvLaneProtoTime[MAX_LANE_NUM] = { 0 };

    if(MAX_VEHICLE_NUM <= ucCarChannel)
    {
        DSP_ERROR("in param carChannel error\n");
        return -1;
    }
    ret = kjPPRealLaneNumGet(ucCarChannel);
    if(-1 == ret)
    {
        DSP_INFO("vehicle lane id is not right\n");
        return 0;
    }
    else
    {
    	if(ret > MAX_LANE_NUM)
    	{
    		ret = MAX_LANE_NUM -1;
    	}
    }

    iTmpChannel = ret;

	/* ����Ȧץ��ģʽ */
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
                if( s_uiCarLen < 0 )
                {
                	s_uiCarLen = 400;
                }
                else if( s_uiCarLen > 1000 )
                {
                	s_uiCarLen = 400;
                }
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
            
            uiCarDir = usGroupId*0x10000+CAR_RUN_NO_DIR;
            
            ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, uiCarInfo, uiCarDir);
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

	/* ˫��Ȧץ��ģʽ */
    if(CAR_RUN_CORRECT_DIR == ucCarDir)
    {
    	if ( COIL_FRONT_POS == ucCoilPos )// ǰ��Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//������ץ�ĵ�һ��ͼƬ
    		{
    			s_iSpeedOkFlag[iTmpChannel] =1;

#if ADJUST_SPEED_EN
				struct timeval tv;
    			struct timezone tz;
	    		gettimeofday(&tv,&tz);

				DSP_INFO("pp Snap 0 orgMsTime:%d,curTime:%d.\n", \
				strLaneProtoTime[iTmpChannel].uiReachTimeMsec,\
				tv.tv_usec/1000);

				if( tv.tv_sec != strLaneProtoTime[iTmpChannel].uiReachTimeSec ) 
				{
					strLaneProtoTime[iTmpChannel].uiOkFlag =1;
				}
				else
				{
					syslog(LOG_ERR|LOG_USER, "pp Snap 0 orgMsTime:%d,curTime:%d.\n", \
					strLaneProtoTime[iTmpChannel].uiReachTimeMsec,\
					tv.tv_usec/1000);
					
					s_uiMsTime = tv.tv_usec/1000 - strLaneProtoTime[iTmpChannel].uiReachTimeMsec;
					if( s_uiMsTime > 500 )
					{
						strLaneProtoTime[iTmpChannel].uiOkFlag =1;
					}
					else
					{
						strLaneProtoTime[iTmpChannel].uiOkFlag =0;
					}
				}

    			strLaneProtoTime[iTmpChannel].uiReachTimeSec = tv.tv_sec;
    			strLaneProtoTime[iTmpChannel].uiReachTimeMsec = tv.tv_usec/1000;
#endif
				DSP_INFO("adjust speed mode en:%d.\n", ADJUST_SPEED_EN);

    			uiCarDir = usGroupId*0x10000+ucCarDir;
    		    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, uiCarDir);
    		    if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap_0 ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp Snap 0 ok,okFlag:%d!\n", strLaneProtoTime[iTmpChannel].uiOkFlag);
    			}
    		}
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//����
    		{
#if ADJUST_SPEED_EN
    			if( s_iSpeedOkFlag[iTmpChannel] == 1 )
    			{
    				s_iSpeedOkFlag[iTmpChannel] =2;
    			}
    			else if( s_iSpeedOkFlag[iTmpChannel] == 3 )
    			{
    				s_iSpeedOkFlag[iTmpChannel] =2;
    			}
    			else
    			{
    				s_iSpeedOkFlag[iTmpChannel] = 0;
    				syslog(LOG_ERR|LOG_USER, "vehicle except,front leave coil not right.\n");
    			}
#endif    			
    			strLaneProtoTime[iTmpChannel].uiLastMs2 = usLastTime;
    			
    			DSP_INFO("12_pp dir snap coil distance:%d cm,lastTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, usLastTime);
    		}
    		else
    		{
    			s_iSpeedOkFlag[iTmpChannel] =0;
    			syslog(LOG_ERR|LOG_USER, "vehicle except,front coil proto error.\n");
    		}
    	}
    	else if (COIL_BEHIND_POS == ucCoilPos)// ����Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )// ���������ó��ٲ�����ץ�ĵڶ���ͼƬ
    		{
    		    if(0 == usLastTime)
    		    {
    		        usLastTime = 1000;
    		    }
    		    
                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("13_pp dir snap coil distance:%d cm,car speed:%dkm/h,lastTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
#if ADJUST_SPEED_EN        	    
        	    if ( s_iSpeedOkFlag[iTmpChannel] == 2 )
        	    {
        	    	s_iSpeedOkFlag[iTmpChannel] =3;
        	    }
        	    else if ( s_iSpeedOkFlag[iTmpChannel] == 1 )
        	    {
        	    	s_iSpeedOkFlag[iTmpChannel] =3;
        	    }
        	    else
        	    {
        	    	s_iSpeedOkFlag[iTmpChannel] =0;
        	    	syslog(LOG_ERR|LOG_USER, "vehicle except,behind enter coil not right.\n");
        	    }
			
    	    	usMaxSpeed = dspPPLimitSpeedGet(0, iTmpChannel);
    	    	if( uiCarSpeed > 2*usMaxSpeed )
    	    	{
    	    		syslog(LOG_ERR|LOG_USER, "11_vehicle speed is too big,org speed:%d,change Speed:%d.okFlag:%d\n",\
    	    		uiCarSpeed, usMaxSpeed,s_iSpeedOkFlag[iTmpChannel]);

    	    		uiCarSpeed = usMaxSpeed-1;
    	    	}
#endif

    	    	uiCarDir = usGroupId*0x10000+ucCarDir;
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarSpeed, uiCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap_1 ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp snap 1 ok,okFlag:%d,speekOkFlag:%d.speed is %d.lastTime:%d.\n",\
    			    strLaneProtoTime[iTmpChannel].uiOkFlag,s_iSpeedOkFlag[iTmpChannel], uiCarSpeed, usLastTime);
    			}
    			
    			strLaneProtoTime[iTmpChannel].uiSpeed1 = uiCarSpeed;
    			
    		}
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//����
    		{
#if ADJUST_SPEED_EN
    			if( s_iSpeedOkFlag[iTmpChannel] ==3 )
        	    {
        	    	s_iSpeedOkFlag[iTmpChannel] =4;
        	    }
        	    else if( s_iSpeedOkFlag[iTmpChannel] == 2 )
        	    {
        	    	s_iSpeedOkFlag[iTmpChannel] =4;
        	    }
        	    else
        	    {
        	    	s_iSpeedOkFlag[iTmpChannel] =0;
        	    	syslog(LOG_ERR|LOG_USER, "vehicle except,behind leave coil not right.\n");
        	    }

				if( strLaneProtoTime[iTmpChannel].uiOkFlag < 1 )
				{
        	    	syslog(LOG_ERR|LOG_USER, "vehicle except,behind leave coil.order is:%d,okFlag is %d.interMs is %d.\n",\
        	    	s_iSpeedOkFlag[iTmpChannel], strLaneProtoTime[iTmpChannel].uiOkFlag, s_uiMsTime);

					s_iSpeedOkFlag[iTmpChannel] =0;
				}

				if(0 == usLastTime)
    		    {
    		        usLastTime = 1000;
    		    }
    		    else
    		    {
    		    	usLastTime -= strLaneProtoTime[iTmpChannel].uiLastMs2;
    		    }
    		    
                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("14_pp dir snap coil distance:%d cm,car speed:%dkm/h,lastTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
        	    
        	    if( s_iSpeedOkFlag[iTmpChannel] != 0 )
        	    {
        	    	usMaxSpeed = dspPPLimitSpeedGet(0, iTmpChannel);
        	    	if( uiCarSpeed > 2*usMaxSpeed )
        	    	{
        	    		syslog(LOG_ERR|LOG_USER, "22_vehicle speed is too big,org speed:%d,change Speed:%d.\n",\
        	    		uiCarSpeed, usMaxSpeed);

        	    		uiCarSpeed = usMaxSpeed-1;
        	    	}
        	    	uiCarDir = usGroupId*0x10000+ucCarDir;
	    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
				}
				else
				{
					uiCarSpeed = 0;
					syslog(LOG_ERR|LOG_USER, "22_vehicle except,speed set %d km/h,time out flag:%d.\n",
					uiCarSpeed, strLaneProtoTime[iTmpChannel].uiOkFlag );
					uiCarDir = usGroupId*0x10000+ucCarDir;
					ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
				}
				
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap_2 ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp snap 2 ok,okFlag:%d,speekOkFlag:%d,speed is %d.lastTime:%d!\n",\
    			    strLaneProtoTime[iTmpChannel].uiOkFlag,s_iSpeedOkFlag[iTmpChannel], uiCarSpeed, usLastTime);
    			}

				strLaneProtoTime[iTmpChannel].uiSpeed2 = uiCarSpeed;

				iTmpInterlSpeed = abs(strLaneProtoTime[iTmpChannel].uiSpeed2 - strLaneProtoTime[iTmpChannel].uiSpeed1);
				if(iTmpInterlSpeed > 20)
				{
					syslog(LOG_ERR|LOG_USER, "22_vehicle except,lane:%d.speed_1 is %d km/h,speed_2 is %d.\n",
					iTmpChannel,strLaneProtoTime[iTmpChannel].uiSpeed1, strLaneProtoTime[iTmpChannel].uiSpeed2 );
				}
#else
				uiCarSpeed = strLaneProtoTime[iTmpChannel].uiSpeed1;
				uiCarDir = usGroupId*0x10000+ucCarDir;
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
#endif
    		}
    		else
    		{
    			s_iSpeedOkFlag[iTmpChannel] =0;
    			syslog(LOG_ERR|LOG_USER, "vehicle except,behind coil proto not right.\n");
    		}
    	}
    	else
    	{
    	    return -1;
    	}
    }
    
    else
    
#if 0    
    {
        if ( COIL_BEHIND_POS == ucCoilPos )// ����Ȧ
    	{
    		if ( CAR_LEAVE_COIL == ucCoilSta )//������ץ�ĵ�һ��ͼƬ
    		{
    			uiCarDir = usGroupId*0x10000+ucCarDir;
    		    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, uiCarDir);
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
    		if ( CAR_LEAVE_COIL == ucCoilSta )// ���������ó��ٲ�����ץ�ĵڶ���ͼƬ
    		{
    		    if(0 == usLastTime)
    		    {
    		        //mcLog(LMT_SYSCALL, LT_INFO, "error1:pp vehicle report speed time is 0,default val is 1000!\n");
    		        usLastTime = 1000;
    		    }
                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("pp redir snap coil distance:%d cm,car speed:%dkm/h,delayTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
        	    uiCarDir = usGroupId*0x10000+ucCarDir;
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarSpeed, uiCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp reverse snap 1 ok!\n");
    			}
				uiCarDir = usGroupId*0x10000+ucCarDir;
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp reverse snap 2 ok!\n");
    			}
    		}
    	}
    	else
    	{
    	    return -1;
    	}
    }
    
#else

	{
    	if ( COIL_BEHIND_POS == ucCoilPos )// ����Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )//����������ʼʱ��
    		{
    			s_iRsvSpeedOkFlag[iTmpChannel] =1;
    			
#if ADJUST_SPEED_EN
				struct timeval tv;
    			struct timezone tz;
	    		gettimeofday(&tv,&tz);

				DSP_INFO("rsv pp Snap 0 orgMsTime:%d,curTime:%d.\n", \
				strRsvLaneProtoTime[iTmpChannel].uiReachTimeMsec,\
				tv.tv_usec/1000);

				if( tv.tv_sec != strRsvLaneProtoTime[iTmpChannel].uiReachTimeSec ) 
				{
					strRsvLaneProtoTime[iTmpChannel].uiOkFlag =1;
				}
				else
				{
					syslog(LOG_ERR|LOG_USER, "rsv pp Snap 0 orgMsTime:%d,curTime:%d.\n", \
					strRsvLaneProtoTime[iTmpChannel].uiReachTimeMsec,\
					tv.tv_usec/1000);
					
					s_uiMsTime = tv.tv_usec/1000 - strRsvLaneProtoTime[iTmpChannel].uiReachTimeMsec;
					if( s_uiMsTime > 500 )
					{
						strRsvLaneProtoTime[iTmpChannel].uiOkFlag =1;
					}
					else
					{
						strRsvLaneProtoTime[iTmpChannel].uiOkFlag =0;
					}
				}

    			strRsvLaneProtoTime[iTmpChannel].uiReachTimeSec = tv.tv_sec;
    			strRsvLaneProtoTime[iTmpChannel].uiReachTimeMsec = tv.tv_usec/1000;
#endif    			
    		}
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//ץ�ĵ�һ��ͼƬ
    		{
#if ADJUST_SPEED_EN
    			if( s_iRsvSpeedOkFlag[iTmpChannel] == 1 )
    			{
    				s_iRsvSpeedOkFlag[iTmpChannel] =2;
    			}
    			else if( s_iRsvSpeedOkFlag[iTmpChannel] == 3 )
    			{
    				s_iRsvSpeedOkFlag[iTmpChannel] =2;
    			}
    			else
    			{
    				s_iRsvSpeedOkFlag[iTmpChannel] = 0;
    				syslog(LOG_ERR|LOG_USER, "rsv vehicle except,front leave coil not right.\n");
    			}
#endif    			
    			strRsvLaneProtoTime[iTmpChannel].uiLastMs2 = usLastTime;
    			
    			DSP_INFO("12_rsv_pp dir snap coil distance:%d cm,lastTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, usLastTime);

				uiCarDir = usGroupId*0x10000+ucCarDir;
        	    ret = dctlPPFlashNormalSnap(iComId, iTmpChannel, 1, 0, uiCarDir);
    		    if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap_0 ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp rsv Snap 0 ok\n");
    			}
    		}
    		else
    		{
    			s_iRsvSpeedOkFlag[iTmpChannel] =0;
    			syslog(LOG_ERR|LOG_USER, "rsv vehicle except,behind coil proto error.\n");
    		}
    	}
    	else if ( COIL_FRONT_POS == ucCoilPos)// ǰ��Ȧ
    	{
    		if ( CAR_ENTER_COIL == ucCoilSta )// ���������㳵��
    		{
    		    if(0 == usLastTime)
    		    {
    		        usLastTime = 1000;
    		    }
    		    
                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("13_rsv_pp dir snap coil distance:%d cm,car speed:%dkm/h,lastTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
#if ADJUST_SPEED_EN
        	    if ( s_iRsvSpeedOkFlag[iTmpChannel] == 2 )
        	    {
        	    	s_iRsvSpeedOkFlag[iTmpChannel] =3;
        	    }
        	    else if ( s_iRsvSpeedOkFlag[iTmpChannel] == 1 )
        	    {
        	    	s_iRsvSpeedOkFlag[iTmpChannel] =3;
        	    }
        	    else
        	    {
        	    	s_iRsvSpeedOkFlag[iTmpChannel] =0;
        	    	syslog(LOG_ERR|LOG_USER, "rsv vehicle except,behind enter coil not right.\n");
        	    }
				
    	    	usMaxSpeed = dspPPLimitSpeedGet(0, iTmpChannel);
    	    	if( uiCarSpeed > 2*usMaxSpeed )
    	    	{
    	    		syslog(LOG_ERR|LOG_USER, "rsv 11_vehicle speed is too big,org speed:%d,change Speed:%d.okFlag:%d\n",\
    	    		uiCarSpeed, usMaxSpeed,s_iRsvSpeedOkFlag[iTmpChannel]);

    	    		uiCarSpeed = usMaxSpeed-1;
    	    	}
#endif
    			strRsvLaneProtoTime[iTmpChannel].uiSpeed1 = uiCarSpeed;
    			
    		}
    		else if ( CAR_LEAVE_COIL == ucCoilSta )//����������ץ�ģ��ٶȲ��ý���Ȧʱ��������ٶ�
    		{
#if ADJUST_SPEED_EN    		
    			if( s_iRsvSpeedOkFlag[iTmpChannel] ==3 )
        	    {
        	    	s_iRsvSpeedOkFlag[iTmpChannel] =4;
        	    }
        	    else if( s_iRsvSpeedOkFlag[iTmpChannel] == 2 )
        	    {
        	    	s_iRsvSpeedOkFlag[iTmpChannel] =4;
        	    }
        	    else
        	    {
        	    	s_iRsvSpeedOkFlag[iTmpChannel] =0;
        	    	syslog(LOG_ERR|LOG_USER, "rsv vehicle except,behind leave coil not right.\n");
        	    }

				if( strRsvLaneProtoTime[iTmpChannel].uiOkFlag < 1 )
				{
        	    	syslog(LOG_ERR|LOG_USER, "rsv vehicle except,behind leave coil.order is:%d,okFlag is %d.interMs is %d.\n",\
        	    	s_iRsvSpeedOkFlag[iTmpChannel], strRsvLaneProtoTime[iTmpChannel].uiOkFlag, s_uiMsTime);

					s_iRsvSpeedOkFlag[iTmpChannel] =0;
				}

				if(0 == usLastTime)
    		    {
    		        usLastTime = 1000;
    		    }
    		    else
    		    {
    		    	usLastTime -= strRsvLaneProtoTime[iTmpChannel].uiLastMs2;
    		    }
#endif
				uiCarSpeed = strRsvLaneProtoTime[iTmpChannel].uiSpeed1;
				uiCarDir = usGroupId*0x10000+ucCarDir;
    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 2, uiCarSpeed, uiCarDir);
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap_1 ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp rsv snap 1 ok,okFlag:%d,speekOkFlag:%d.speed is %d.lastTime:%d.\n",\
    			    strRsvLaneProtoTime[iTmpChannel].uiOkFlag,s_iRsvSpeedOkFlag[iTmpChannel], uiCarSpeed, usLastTime);
    			}
    			
#if ADJUST_SPEED_EN

                uiCarSpeed = (g_aPassPortChanInfo[iTmpChannel].usCoilDistance * 36) / usLastTime;//km/h
                DSP_INFO("14_rsv_pp dir snap coil distance:%d cm,car speed:%dkm/h,lastTime:%d ms\n", \
        	    g_aPassPortChanInfo[iTmpChannel].usCoilDistance, uiCarSpeed, usLastTime);
        	    
        	    if( s_iRsvSpeedOkFlag[iTmpChannel] != 0 )
        	    {
        	    	usMaxSpeed = dspPPLimitSpeedGet(0, iTmpChannel);
        	    	if( uiCarSpeed > 2*usMaxSpeed )
        	    	{
        	    		syslog(LOG_ERR|LOG_USER, "rsv 22_vehicle speed is too big,org speed:%d,change Speed:%d.\n",\
        	    		uiCarSpeed, usMaxSpeed);

        	    		uiCarSpeed = usMaxSpeed-1;
        	    	}
        	    	
        	    	uiCarDir = usGroupId*0x10000+ucCarDir;
	    			ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
				}
				else
				{
					uiCarSpeed = 0;
					syslog(LOG_ERR|LOG_USER, "rsv 22_vehicle except,speed set %d km/h,time out flag:%d.\n",
					uiCarSpeed, strRsvLaneProtoTime[iTmpChannel].uiOkFlag );
					
					uiCarDir = usGroupId*0x10000+ucCarDir;
					ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
				}
				
    			if( 0 != ret)
    			{
    			    DSP_ERROR("dctlPPFlashNormalSnap_2 ret:0x%x failed\n", ret);
    			}
    			else
    			{
    			    DSP_INFO("pp rsv snap 2 ok,okFlag:%d,speekOkFlag:%d,speed is %d.lastTime:%d!\n",\
    			    strRsvLaneProtoTime[iTmpChannel].uiOkFlag,s_iRsvSpeedOkFlag[iTmpChannel], uiCarSpeed, usLastTime);
    			}

				strRsvLaneProtoTime[iTmpChannel].uiSpeed2 = uiCarSpeed;

				iTmpInterlSpeed = abs(strRsvLaneProtoTime[iTmpChannel].uiSpeed2 - strRsvLaneProtoTime[iTmpChannel].uiSpeed1);
				if(iTmpInterlSpeed > 20)
				{
					syslog(LOG_ERR|LOG_USER, "rsv 22_vehicle except,lane:%d.speed_1 is %d km/h,speed_2 is %d.\n",
					iTmpChannel,strRsvLaneProtoTime[iTmpChannel].uiSpeed1, strRsvLaneProtoTime[iTmpChannel].uiSpeed2 );
				}
#else
				uiCarDir = usGroupId*0x10000+ucCarDir;
	    		ret =dctlPPFlashNormalSnap(iComId, iTmpChannel, 3, uiCarSpeed, uiCarDir);
#endif
    		}
    		else
    		{
    			s_iRsvSpeedOkFlag[iTmpChannel] =0;
    			syslog(LOG_ERR|LOG_USER, "rsv vehicle except,behind coil proto not right.\n");
    		}
    	}
    	else
    	{
    	    return -1;
    	}
    }
#endif
	return 0;
}

