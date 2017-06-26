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
 * hgdPassPortSnapBegin() :�����󳵼������ڿ���ʱץ�ĵĵ�һ��ͼƬ
 * 
 * DESCRIPTION:  �����ڿ��ڵ�һ��ͼƬ�������ս���Ȧʱ֪ͨDSP����ץ��
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
                DSP_ERROR("%s,%s,%d,����%d�ظ���Ȧץ�ĵ�һ��ͼƬʧ��\n",__FILE__,__FUNCTION__,__LINE__,g_aPassPortChanInfo[i].uiCarRoadId);
            }
        }        
    }
    return ret;
}


/******************************************************************************
 * hgdPassPortSnapEnd() :�����󳵼������ڿ���ʱץ�ĵĵڶ���ͼƬ
 * 
 * DESCRIPTION:  ��������ڶ���Ȧʱ�����������ϱ���ʱ���٣��ѳ��ٴ��͸�DSP��DSP
 * �����ٶ��жϣ���Ϊ������DSP��ץ�ĵڶ���ͼƬ�����ϱ�������������˴�ץ�Ĺ��̽�����
 * ���ϱ�ͼƬ��
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
 * hgdEPSnapCtl() :�����󳵼������ڵ��Ӿ���ץ�Ŀ���
 * 
 * DESCRIPTION:  ������һ��Ȧʱ��һ�ţ�����һ��Ȧʱ����һ�ţ������ڶ���Ȧʱ����һ�ţ�
 * DSP���ĵ������źϳ�һ��ͼƬ��һ����дͼƬ�ϱ������Ӿ��������Ͻ�ͨ�ƺ��״̬��������
 * ����ͼƬ�Ĺ����н�ͨ�Ʊ�Ǻ���ˣ����������ֹͣ������֪ͨDSP����ץ��
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
    DSP_INFO("com %d board %d module %d status",comid,nBoardId,moduleId,status==1?"����":"����");
    int iPicMixType =0;

    iPicMixType = dspEpSnapTypeGet(0);
    for (int i=0; i<MAX_VEHICLE_NUM; ++i)
    {
        ret = getTrafficLightSta(i);
        if( RED_LIGHT_ON != ret)
        {
            DSP_INFO("red light is green\n");
            /* ��Ϊ�̵�ʱ��Ҫ�����̵�ץ�� */
#if defined(CYC500XKW4C)||defined(CYC500XAW)
            if ( (g_aEPConfInfo[i].cBoardId== nBoardId) && (comid == g_aEPConfInfo[i].cComId) )
            {
                if ( g_aEPConfInfo[i].cFrontCoilId == moduleId )// ǰ��Ȧ
                {
                    if ( 0 == status )// ������ץ�ĵڶ���ͼƬ
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
            if ( g_aEPConfInfo[i].cFrontCoilId == moduleId )// ǰ��Ȧ
            {
                if ( 1 == status ) //������ץ�ĵ�һ��ͼƬ
                {
                    dctlEPFlashNormalSnap(i, 1, iPicMixType);
                }
                else if ( 0 == status )// ������ץ�ĵڶ���ͼƬ
                {
                    dctlEPFlashNormalSnap(i, 2, iPicMixType);
                }
                goto gotoFlag;
            }
            else if (g_aEPConfInfo[i].cBehindCoilId == moduleId)// ����Ȧ
            {
                if ( 0 == status )// ������ץ�ĵ�����ͼƬ
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


