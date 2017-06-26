#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include "xcsGlobalDef.h"
#include "dsp_define.h"
#include "vehicleReport.h"
#include "dspParamManage.h"

extern "C" {
#include "dsp.h"
}

#include "SysHandle.h"

extern CARROAD_DECT_CONF g_aEPConfInfo[MAX_VEHICLE_NUM];
extern CARROAD_DECT_CONF g_aPassPortChanInfo[MAX_VEHICLE_NUM];


/******************************************************************************
 * hgdPassPortSnapBegin() :哈工大车检器用于卡口时抓拍的第一张图片
 * 
 * DESCRIPTION:  运用于卡口第一张图片，即车刚进线圈时通知DSP进行抓拍
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.18, wangbin Create
 * --------------------
 ******************************************************************************/
int hgdPassPortSnapBegin(unsigned int iComId, int moduleId, unsigned char ucCarDir)
{
    int i =0;
    int ret = -1;
    for(i=0; i < MAX_VEHICLE_NUM; i++)
    {
        if( g_aPassPortChanInfo[i].cModuleId == moduleId )
        /*&&(g_aPassPortChanInfo[i].cBoardId == nBoardId)) */
        {
               ret = dctlPPFlashNormalSnap(iComId, g_aPassPortChanInfo[i].uiCarRoadId, 1, 0, ucCarDir);
            if(ret!=0)
            {
                DSP_ERROR("%s,%s,%d,车道%d地感线圈抓拍第一张图片失败\n",__FILE__,__FUNCTION__,__LINE__,g_aPassPortChanInfo[i].uiCarRoadId);
            }
        }        
    }
    return ret;
}


/******************************************************************************
 * hgdPassPortSnapEnd() :哈工大车检器用于卡口时抓拍的第二张图片
 * 
 * DESCRIPTION:  当车进入第二线圈时，车检器会上报此时车速，把车速传送给DSP，DSP
 * 进行速度判断，若为超速则DSP会抓拍第二张图片，并上报；若不超速则此次抓拍过程结束，
 * 不上报图片。
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.18, wangbin Create
 * --------------------
 ******************************************************************************/
int hgdPassPortSnapEnd(unsigned int iComId, int moduleId, int speed, unsigned char ucCarDir)
{
    int i=0;
    int ret = 0;
    for(i=0; i < MAX_VEHICLE_NUM; i++)
    {
        DSP_ERROR("dev id:%d,vehicle id:%d\n",g_aPassPortChanInfo[i].cModuleId, moduleId);
        if( g_aPassPortChanInfo[i].cModuleId == moduleId )
        /* &&(g_aPassPortChanInfo[i].cBoardId == nBoardId)) */
        {
            ret =dctlPPFlashNormalSnap(iComId, g_aPassPortChanInfo[i].uiCarRoadId, 2, speed, ucCarDir);
        }
    }

    return ret;
}

/******************************************************************************
 * hgdEPSnapCtl() :哈工大车检器用于电子警察抓拍控制
 * 
 * DESCRIPTION:  车进第一线圈时拍一张，出第一线圈时再拍一张，车出第二线圈时再拍一张，
 * DSP把拍到的三张合成一张图片及一张特写图片上报。电子警察必须配合交通灯红灯状态，若在拍
 * 三张图片的过程中交通灯变非红灯了，则后续操作停止，不再通知DSP进行抓拍
 * 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.18, wangbin Create
 * --------------------
 ******************************************************************************/
int hgdEPSnapCtl(int comid,int nBoardId,int moduleId,int status)
{
    int ret =0;
    DSP_INFO("com %d board %d module %d status",comid,nBoardId,moduleId,status==1?"进车":"出车");
    int iPicMixType =0;

    iPicMixType = dspEpSnapTypeGet(0);
    for (int i=0; i<MAX_VEHICLE_NUM; ++i)
    {
        ret = getTrafficLightSta(i);
        if( RED_LIGHT_ON != ret)
        {
            DSP_INFO("red light is green\n");
            /* 当为绿灯时需要进行绿灯抓拍 */
#if defined(CYC500XKW4C)||defined(CYC500XAW)
            if ( (g_aEPConfInfo[i].cBoardId== nBoardId) && (comid == g_aEPConfInfo[i].cComId) )
            {
                if ( g_aEPConfInfo[i].cFrontCoilId == moduleId )// 前线圈
                {
                    if ( 0 == status )// 出车，抓拍第二张图片
                    {
                        dctlEPFlashNormalSnap(i, 1, SNAP_TYPE_CLIP);
                        return 0;
                    }
                }
            }
            continue;
#endif
        }
        
        //DSP_INFO("conf com %d board %d front %d back %d .\n",g_aEPConfInfo[i].nComId,g_aEPConfInfo[i].nBoardId,g_aEPConfInfo[i].nFrontId,g_aEPConfInfo[i].nBackId);
        if ( (g_aEPConfInfo[i].cBoardId== nBoardId) && (comid == g_aEPConfInfo[i].cComId) )
        {
            if ( g_aEPConfInfo[i].cFrontCoilId == moduleId )// 前线圈
            {
                if ( 1 == status ) //进车，抓拍第一张图片
                {
                    dctlEPFlashNormalSnap(i, 1, iPicMixType);
                }
                else if ( 0 == status )// 出车，抓拍第二张图片
                {
                    dctlEPFlashNormalSnap(i, 2, iPicMixType);
                }
                goto gotoFlag;
            }
            else if (g_aEPConfInfo[i].cBehindCoilId == moduleId)// 后线圈
            {
                if ( 0 == status )// 出车，抓拍第三张图片
                {
                    dctlEPFlashNormalSnap(i, 3, iPicMixType);
                }
                else
                {
                    goto gotoFlag;                    
                }
            }
        }
        if(SNAP_TYPE_6_PLUS_1 == iPicMixType)
        {
            sleep(1);
            dctlEPFlashNormalSnap(i, 4, iPicMixType);
            usleep(1000);
            dctlEPFlashNormalSnap(i, 5, iPicMixType);
            usleep(1000);
            dctlEPFlashNormalSnap(i, 6, iPicMixType);
        }
gotoFlag:
        continue;
    }
    return 0;
}


