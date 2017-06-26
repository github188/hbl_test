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

#include "SysConfCheck.h"
#include "SysConf.h"
#include "SysHandle.h"
#include "protoXHandle.h"
#include "protoXHandleDebug.h"
#include "McLog.h"
#include <time.h>
#include <sys/time.h>
#include "Record.h"
#include "rtc_util.h"
     extern "C" {
#include "dsp.h"
     }
#include "Record.h"
#include "Reboot.h"
#include "Update_delay.h"
#include "MmsSendDefine.h"
#include "MmsSend.h"
#include "cfgSock.h"
#if 0

int CHandleXcsPtl::CmdRecordPeriodQuery(XCS_CLIENT * pClintSocket)
{
	if(NULL==pClintSocket)
		return (-1);

	int nRet = 0;
	unsigned short usChannel = 0;
	int nChannel = 0;
	LPTRecordPalnHead pRecordPlanHead = (LPTRecordPalnHead)(pCmdPack->pSendBuf + NT_PACK_HEAD_LENGTH);
	LPTPeriodRecordPlan pPeriodRecordPlan = (LPTPeriodRecordPlan)(pCmdPack->pSendBuf + NT_PACK_HEAD_LENGTH+sizeof(TRecordPalnHead));
	REC_CONF_INFO *pRecConfInfo = NULL;
	int nPlanCount = 0;
	
	memcpy(&usChannel,(pCmdPack->pRecvBuf + NT_PACK_HEAD_LENGTH),sizeof(unsigned short));
	usChannel = ntohs(usChannel);

	nRet = confGet(e_REC_CONF,(void **)&pRecConfInfo);
	if(0==nRet)
	{
		if(usChannel==0xffff)
		{
			for(nChannel=0;nChannel<TOTAL_CHANNEL_NUM;nChannel++)
			{
				if(IS_P_CHANNEL_VALID(nChannel))
				{
					if(pRecConfInfo[nChannel].nChannelID==nChannel+1)
					{
						for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
						{
							if(pRecConfInfo[nChannel].ptdDayPlanRecTime.bIsValid)
							{
								if((pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime==0)&&\
									(pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime==0))
								{
								}
								else
								{
									pPeriodRecordPlan->nVideoChannel = htons(nChannel+1);
									pPeriodRecordPlan->nWeekDay = 0;
									pPeriodRecordPlan->nBeginHour = (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime/3600);
									pPeriodRecordPlan->nBeginMinute  =  (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime%3600)/60;
									pPeriodRecordPlan->nEndHour = (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime/3600);
									pPeriodRecordPlan->nEndMinute  =  (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime%3600)/60;
									pPeriodRecordPlan->nReserved = 0;
									pPeriodRecordPlan->nRandom[0] =  htonl(zone_rand(0x00,0x7fffffff));
									pPeriodRecordPlan->nRandom[1] =  htonl(zone_rand(0x00,0x7fffffff));
									pPeriodRecordPlan->nRandom[2] =  htonl(zone_rand(0x00,0x7fffffff));
									pPeriodRecordPlan->nRandom[3] =  htonl(zone_rand(0x00,0x7fffffff));
									
									pPeriodRecordPlan++;
									nPlanCount++;
								}
							}
						}

						for(int nWeekDay=0;nWeekDay<7;nWeekDay++)
						{
							if(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].bIsValid)
							{
								for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
								{
									if((pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiStartTime==0)&&\
										(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiEndTime==0))
									{
									}
									else
									{
										pPeriodRecordPlan->nVideoChannel = htons(nChannel+1);
										pPeriodRecordPlan->nWeekDay = nWeekDay+1;
										pPeriodRecordPlan->nBeginHour = (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiStartTime/3600);
										pPeriodRecordPlan->nBeginMinute  =  (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiStartTime%3600)/60;
										pPeriodRecordPlan->nEndHour = (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiEndTime/3600);
										pPeriodRecordPlan->nEndMinute  =  (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiEndTime%3600)/60;
										pPeriodRecordPlan->nReserved = 0;
										pPeriodRecordPlan->nRandom[0] =  htonl(zone_rand(0x00,0x7fffffff));
										pPeriodRecordPlan->nRandom[1] =  htonl(zone_rand(0x00,0x7fffffff));
										pPeriodRecordPlan->nRandom[2] =  htonl(zone_rand(0x00,0x7fffffff));
										pPeriodRecordPlan->nRandom[3] =  htonl(zone_rand(0x00,0x7fffffff));
										
										pPeriodRecordPlan++;
										nPlanCount++;
									}
								}
							}
						}
					}
				}
			}

			pRecordPlanHead->nVideoChannel = htons(0xffff);
			pRecordPlanHead->nRecordPlanNum = htons(nPlanCount);
		
		}
		else
		{
			nChannel = usChannel-1;

			if(IS_P_CHANNEL_VALID(nChannel))
			{
				if(pRecConfInfo[nChannel].nChannelID==nChannel+1)
				{
					if(pRecConfInfo[nChannel].ptdDayPlanRecTime.bIsValid)
					{
						for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
						{
							if((pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime==0)&&\
								(pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime==0))
							{
							}
							else
							{
								pPeriodRecordPlan->nVideoChannel = htons(nChannel+1);
								pPeriodRecordPlan->nWeekDay = 0;
								pPeriodRecordPlan->nBeginHour = (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime/3600);
								pPeriodRecordPlan->nBeginMinute  =  (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime%3600)/60;
								pPeriodRecordPlan->nEndHour = (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime/3600);
								pPeriodRecordPlan->nEndMinute  =  (pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime%3600)/60;
								pPeriodRecordPlan->nReserved = 0;
								pPeriodRecordPlan->nRandom[0] =  htonl(zone_rand(0x00,0x7fffffff));
								pPeriodRecordPlan->nRandom[1] =  htonl(zone_rand(0x00,0x7fffffff));
								pPeriodRecordPlan->nRandom[2] =  htonl(zone_rand(0x00,0x7fffffff));
								pPeriodRecordPlan->nRandom[3] =  htonl(zone_rand(0x00,0x7fffffff));
								
								pPeriodRecordPlan++;
								nPlanCount++;
							}
						}
					}

					for(int nWeekDay=0;nWeekDay<7;nWeekDay++)
					{
						if(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].bIsValid)
						{
							for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
							{
								if((pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiStartTime==0)&&\
									(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiEndTime==0))
								{
								}
								else
								{
									pPeriodRecordPlan->nVideoChannel = htons(nChannel+1);
									pPeriodRecordPlan->nWeekDay = nWeekDay+1;
									pPeriodRecordPlan->nBeginHour = (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiStartTime/3600);
									pPeriodRecordPlan->nBeginMinute  =  (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiStartTime%3600)/60;
									pPeriodRecordPlan->nEndHour = (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiEndTime/3600);
									pPeriodRecordPlan->nEndMinute  =  (pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay].ts[i].uiEndTime%3600)/60;
									pPeriodRecordPlan->nReserved = 0;
									pPeriodRecordPlan->nRandom[0] =  htonl(zone_rand(0x00,0x7fffffff));
									pPeriodRecordPlan->nRandom[1] =  htonl(zone_rand(0x00,0x7fffffff));
									pPeriodRecordPlan->nRandom[2] =  htonl(zone_rand(0x00,0x7fffffff));
									pPeriodRecordPlan->nRandom[3] =  htonl(zone_rand(0x00,0x7fffffff));
									
									pPeriodRecordPlan++;
									nPlanCount++;
								}
							}
						}
					}
				}
			}

			pRecordPlanHead->nVideoChannel = htons(nChannel+1);
			pRecordPlanHead->nRecordPlanNum = htons(nPlanCount);
		}
	}
	else
	{
		pRecordPlanHead->nVideoChannel = htons(0);
		pRecordPlanHead->nRecordPlanNum = htons(0);
	}

	SetSimpleAckHead((PACKETHEADER*)pCmdPack->pSendBuf, CPH_RECORD, CPE_RECORD_PERIOD_QUERY,sizeof(TRecordPalnHead)+sizeof(TPeriodRecordPlan)*nPlanCount);
	pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(TRecordPalnHead)+sizeof(TPeriodRecordPlan)*nPlanCount;
	
	return 0;
}

int CHandleXcsPtl::CmdRecordPeriodAdd(XCS_CLIENT * pClintSocket)
{
	if(NULL==pCmdPack)
		return (-1);

	if(USER_TYPE_SYSTEM!=pCmdPack->dwUserPrivilege)
	{
		SetNakPack((PACKETHEADER*)pCmdPack->pSendBuf);
		pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH;
	
		return 0;
	}

	int nRet = 0;
	int nChannel = 0;
	LPTPeriodRecordPlan pPeriodRecordPlan = (LPTPeriodRecordPlan)(pCmdPack->pRecvBuf + NT_PACK_HEAD_LENGTH);
	REC_CONF_INFO *pRecConfInfo = NULL;
	int nPlanCount = 0;

	nPlanCount = (pCmdPack->nPackLen - NT_PACK_HEAD_LENGTH)/sizeof(TPeriodRecordPlan);

	nRet = confGet(e_REC_CONF,(void **)&pRecConfInfo);
	if(0==nRet)
	{
		for(int nCount=0;nCount<nPlanCount;nCount++,pPeriodRecordPlan++)
		{
			nChannel = ntohs(pPeriodRecordPlan->nVideoChannel) -1;

			if(IS_P_CHANNEL_VALID(nChannel))
			{
				unsigned int uiStartTime = pPeriodRecordPlan->nBeginHour*3600+pPeriodRecordPlan->nBeginMinute*60+0;
				unsigned int uiEndTime =pPeriodRecordPlan->nEndHour*3600+pPeriodRecordPlan->nEndMinute*60+0;
				unsigned int nWeekDay = pPeriodRecordPlan->nWeekDay;
				if(uiStartTime<=uiEndTime)
				{
					if(nWeekDay==0)
					{
						pRecConfInfo[nChannel].ptdDayPlanRecTime.bIsValid = 1;
					
						for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
						{
							if((pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime==0)&&\
								(pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime==0))
							{
								AddRecCycleTime(nChannel,nWeekDay,uiStartTime,uiEndTime);
								pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime=uiStartTime;
								pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime=uiEndTime;

								break;
							}

							if((pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime==uiStartTime)&&\
								(pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime==uiEndTime))
							{
								break;
							}
						}
					}
					else
					{
						pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].bIsValid = 1;
					
						for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
						{
							if((pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiStartTime==0)&&\
								(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiEndTime==0))
							{
								AddRecCycleTime(nChannel,nWeekDay,uiStartTime,uiEndTime);
								pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiStartTime=uiStartTime;
								pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiEndTime=uiEndTime;

								break;
							}

							if((pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiStartTime==uiStartTime)&&\
								(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiEndTime==uiEndTime))
							{
								break;
							}
						}
					}
				}
			}
		}
	}

	SaveRecConfValue();
	
	SetAckPack((PACKETHEADER*)pCmdPack->pSendBuf);
	pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH;
	
	return 0;
}

int CHandleXcsPtl::CmdRecordPeriodDel(XCS_CLIENT * pClintSocket)
{
	if(NULL==pCmdPack)
		return (-1);

	if(USER_TYPE_SYSTEM!=pCmdPack->dwUserPrivilege)
	{
		SetNakPack((PACKETHEADER*)pCmdPack->pSendBuf);
		pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH;
	
		return 0;
	}

	int nRet = 0;
	int nChannel = 0;
	LPTPeriodRecordPlan pPeriodRecordPlan = (LPTPeriodRecordPlan)(pCmdPack->pRecvBuf + NT_PACK_HEAD_LENGTH);
	REC_CONF_INFO *pRecConfInfo = NULL;
	int nPlanCount = 0;

	nPlanCount = (pCmdPack->nPackLen - NT_PACK_HEAD_LENGTH)/sizeof(TPeriodRecordPlan);

	nRet = confGet(e_REC_CONF,(void **)&pRecConfInfo);
	if(0==nRet)
	{
		for(int nCount=0;nCount<nPlanCount;nCount++,pPeriodRecordPlan++)
		{
			nChannel = ntohs(pPeriodRecordPlan->nVideoChannel) -1;

			if(IS_P_CHANNEL_VALID(nChannel))
			{
				unsigned int uiStartTime = pPeriodRecordPlan->nBeginHour*3600+pPeriodRecordPlan->nBeginMinute*60+0;
				unsigned int uiEndTime =pPeriodRecordPlan->nEndHour*3600+pPeriodRecordPlan->nEndMinute*60+0;
				unsigned int nWeekDay = pPeriodRecordPlan->nWeekDay;

				if(nWeekDay==0)
				{
					for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
					{
						if((pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime==uiStartTime)&&\
							(pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime==uiEndTime))
						{
							DelRecCycleTime(nChannel,nWeekDay,uiStartTime,uiEndTime);
							pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiStartTime = 0;
							pRecConfInfo[nChannel].ptdDayPlanRecTime.ts[i].uiEndTime = 0;
						}
					}
				}
				else
				{
					for(int i=0;i<DEFAULT_PLANT_TIME_SEGMENT;i++)
					{
						if((pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiStartTime==uiStartTime)&&\
							(pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiEndTime==uiEndTime))
						{
							DelRecCycleTime(nChannel,nWeekDay,uiStartTime,uiEndTime);
							pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiStartTime=0;
							pRecConfInfo[nChannel].ptdWeekPlanRecTime[nWeekDay-1].ts[i].uiEndTime = 0;
						}
					}
				}
			}
		}
	}

	SaveRecConfValue();
	
	SetAckPack((PACKETHEADER*)pCmdPack->pSendBuf);
	pCmdPack->nSendLen = NT_PACK_HEAD_LENGTH;
	
	return 0;
}

#endif

#ifdef MODULE_REC
int CHandleXcsPtl::RecordRtspQuery(XCS_CLIENT * pClintSocket)
{
    LPSTR_RTSP_CONF_QUERY pRecRtspQuery = (LPSTR_RTSP_CONF_QUERY)(pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH);
    struct in_addr in;
    RTSP_CONF_INFO strSysRtspCfg;
    memset(&strSysRtspCfg, 0, sizeof(RTSP_CONF_INFO));
    int nRet = SysConfGet(e_RTSP_CONF, &strSysRtspCfg, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("record get rtsp Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    else
    {
        pRecRtspQuery->cRtspMode = strSysRtspCfg.nRtspMode;
        in.s_addr = htonl(strSysRtspCfg.dwRtspIpAddr);
	    sprintf(pRecRtspQuery->szRtspIP, "%s", inet_ntoa(in));

	    strSysRtspCfg.szRtspPath[31] =0;
        strcpy(pRecRtspQuery->szRtspPath, strSysRtspCfg.szRtspPath);
        pRecRtspQuery->sHttpPort = htons(strSysRtspCfg.dwRtspDownLoadPort);
        pRecRtspQuery->sRtspPort = htons(strSysRtspCfg.dwRtspPort);

        strSysRtspCfg.szRtspUsername[19] =0;
        strSysRtspCfg.szRtspPassword[19] =0;
        strcpy(pRecRtspQuery->szUserName, strSysRtspCfg.szRtspUsername);
        strcpy(pRecRtspQuery->szPassword, strSysRtspCfg.szRtspPassword);
    }
    XCSPTL_INFO("record get rtsp Conf is here.\n");
    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_RTSP_CONF_QUERY);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_RECORD, CPE_RECORD_DOWNLOAD_PLAYTYPE_QUERY, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::RecordRtspSet(XCS_CLIENT * pClintSocket)
{
    int ret =0;
    STR_RTSP_CONF_SET *pRecRtspSet = (STR_RTSP_CONF_SET *)(pClintSocket->pRecvBuf+NT_PACK_HEAD_LENGTH);
    RTSP_CONF_INFO strSysRtspCfg;

    memset(&strSysRtspCfg, 0, sizeof(strSysRtspCfg));

    strSysRtspCfg.nRtspMode = pRecRtspSet->cRtspMode;
    strSysRtspCfg.dwRtspPort = ntohs(pRecRtspSet->sRtspPort);
    strSysRtspCfg.dwRtspDownLoadPort = ntohs(pRecRtspSet->sHttpPort);
    //XCSPTL_INFO("1------record set rtsp ip is %s\n", pRecRtspSet->szRtspIP);
    strSysRtspCfg.dwRtspIpAddr = ntohl(inet_addr(pRecRtspSet->szRtspIP));
    //XCSPTL_INFO("2------record turn rtsp ip is 0x%x \n", strSysRtspCfg.dwRtspIpAddr);

    pRecRtspSet->szRtspPath[31] =0;
    pRecRtspSet->szUserName[19] =0;
    pRecRtspSet->szPassword[19] =0;
    strcpy(strSysRtspCfg.szRtspPath, pRecRtspSet->szRtspPath);
    strcpy(strSysRtspCfg.szRtspUsername, pRecRtspSet->szUserName);
    strcpy(strSysRtspCfg.szRtspPassword, pRecRtspSet->szPassword);

    ret = SysConfSet(e_RTSP_CONF, &strSysRtspCfg, 0);
    if( 0 > ret)
    {
        XCSPTL_ERROR("record set rtsp Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    SetAckPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf) );
    return ret;
}

#define MAX_SFI_FILE 100

#define LTC_REC_QUERY 1 /* 采用本地时间查询 */

int CHandleXcsPtl::RecordFileQuery(XCS_CLIENT *pClintSocket)
{
    PSTR_RECORD_PC_SEARCH pRecQuery = (STR_RECORD_PC_SEARCH *)(pClintSocket->pRecvBuf + NT_PACK_HEAD_LENGTH);
    PSTR_QUERY_INDEX_HEAD pRecQueryIndexHead = (PSTR_QUERY_INDEX_HEAD)(pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH);
    int iCnt = 0;

    PSTR_QUERY_INDEX pRecQueryIndex = NULL;
    LPTRecDigest pRecDigest= NULL;

    int nChannel = ntohs(pRecQuery->usVideoNum)-1;
    unsigned short Alarmtype=(ntohl(pRecQuery->uiRecordType)&0xffff);
    unsigned short RecType=((ntohl(pRecQuery->uiRecordType)&0xffff0000)>>16);
    unsigned long utcBeginTime= ntohl(pRecQuery->uiBeginTime);
    unsigned long utcEndTime= ntohl(pRecQuery->uiEndTime);
    SFI sfiArray[MAX_SFI_FILE] = {0};

    /* 因为配置客户端下发的是cst时间，但录像索引均是用utc时间记录的，所以需要把时区时间去掉 */
/*  富阳查找时录像时间少8小时，所以暂时去掉 */
	#if LTC_REC_QUERY
    utcBeginTime -= 8*3600;
    utcEndTime 	-= 8*3600;
	#else

	#endif
	
	XCSPTL_INFO("record file search beginTime:%d,endTime:%d.\n", utcBeginTime, utcEndTime);

    if(IS_P_CHANNEL_VALID(nChannel))
    {
        //shell_print("搜索录像类型:  %04x :: %04x\n", RecType, Alarmtype);
        if(0 == RecType)  //普通录像
        {
            iCnt = RecSearchByTime(nChannel,utcBeginTime,utcEndTime,sfiArray,MAX_SFI_FILE);
        }
        else if( 1 == RecType)
        {
            iCnt = RecSearchByEvent(nChannel, Alarmtype, utcBeginTime, utcEndTime, sfiArray, MAX_SFI_FILE);
        }
        else if( 0xffff == RecType)
        {
            iCnt = RecSearchByTime(nChannel,utcBeginTime,utcEndTime,sfiArray,MAX_SFI_FILE);
        }
    }
    if(0 > iCnt)
    {
        XCSPTL_ERROR("record file not searched file.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_WRITE_CONF);
        return -1;
    }
    pRecQueryIndex = (PSTR_QUERY_INDEX)(pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH+sizeof(STR_QUERY_INDEX_HEAD));
    for(int i=0;i<iCnt;i++)
    {
    	memset(pRecQueryIndex,0,sizeof(STR_QUERY_INDEX));
		
        pRecQueryIndex->uiRecordType = pRecQuery->uiRecordType;
        
		#if LTC_REC_QUERY
        pRecQueryIndex->uiStartTime = htonl(sfiArray[i].ulBeginTime + 8*3600);
        pRecQueryIndex->uiEndTime = htonl(sfiArray[i].ulEndTime + 8*3600);
        #else
        pRecQueryIndex->uiStartTime = htonl(sfiArray[i].ulBeginTime);
        pRecQueryIndex->uiEndTime = htonl(sfiArray[i].ulEndTime);
        #endif
        
        pRecQueryIndex->usVideoNum = htons(nChannel+1);
        pRecQueryIndex->uiFileSize = htonl(sfiArray[i].ulSize);
        pRecQueryIndex->nFileOffset =  htonl(0);
        memset(pRecQueryIndex->sFileName,0x00,sizeof(pRecQueryIndex->sFileName));

        sfiArray[i].szFileName[127] =0;
        strcpy(pRecQueryIndex->sFileName,sfiArray[i].szFileName);
        pRecQueryIndex =(PSTR_QUERY_INDEX) ((char *)pRecQueryIndex+sizeof(STR_QUERY_INDEX));
    }
    pRecDigest = (LPTRecDigest)(pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH+sizeof(STR_QUERY_INDEX_HEAD)+sizeof(STR_QUERY_INDEX)*iCnt);

    for(int i=0;i<iCnt;i++)
    {
        pRecDigest->nDigestSize = htons(0);
        pRecDigest->nDigestType = htons(0);

        pRecDigest =(LPTRecDigest) ((char *)pRecDigest+4);
    }
    pRecQueryIndexHead->uiTotalIndexNum = htonl(iCnt);
    pRecQueryIndexHead->uiCurIndexNum = htonl(iCnt);
    pRecQueryIndexHead->uiIndexStrLen = htonl(sizeof(STR_QUERY_INDEX));
    pRecQueryIndexHead->nDigestStructLen = htonl(0);
	

    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_QUERY_INDEX_HEAD)+sizeof(STR_QUERY_INDEX)*iCnt+4*iCnt;
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_RECORD, CPE_XCS_RECORD_SEARCH, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
}

int CHandleXcsPtl::RecordFilePlayQuery(XCS_CLIENT *pClintSocket)
{
    char cDevIp[16] ={0};
    PSTR_RECORD_PC_PLAYBACK pStrPCRecPlayBack = (PSTR_RECORD_PC_PLAYBACK)(pClintSocket->pRecvBuf+NT_PACK_HEAD_LENGTH);
    PSTR_RECORD_PLAYBACK pStrRecPlayBack = (PSTR_RECORD_PLAYBACK)(pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH);
    char *pURL =NULL;
    char *pfilname =NULL;
    struct in_addr in;

    RTSP_CONF_INFO strSysRtspCfg;
    memcpy(cDevIp, cfgSockGetValue("ipaddr", "", "0.0.0.0"), sizeof(cDevIp));
    memset(&strSysRtspCfg, 0, sizeof(RTSP_CONF_INFO));
    int nRet = SysConfGet(e_RTSP_CONF, &strSysRtspCfg, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("playback record get rtsp Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    else
    {
        in.s_addr = strSysRtspCfg.dwRtspIpAddr;
	    
        pURL = pStrRecPlayBack->cFileUrl;
        pfilname = pStrPCRecPlayBack->cFileName;
        if(memcmp(pfilname, "/ide", 4) == 0)
        {
            pfilname += 4;
        }
        if ( strSysRtspCfg.nRtspMode != 2 )
        {
                sprintf(pURL, "http://%s:%d%s", inet_ntoa(in), strSysRtspCfg.dwRtspPort, pfilname);
        }
        else
        {
            char szUserPass[64] = {0};
            if (strlen(strSysRtspCfg.szRtspUsername)>0 && strlen(strSysRtspCfg.szRtspPassword)>0)
            {
                sprintf( szUserPass, "%s:%s@", strSysRtspCfg.szRtspUsername, strSysRtspCfg.szRtspPassword);
            }
            
            sprintf(pURL, "rtsp://%s%s:%d/%s/%s%s",
                    szUserPass, inet_ntoa(in), strSysRtspCfg.dwRtspPort,
                    strSysRtspCfg.szRtspPath, cDevIp,
                    pfilname);
        }
    }
    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_RECORD_PLAYBACK);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_RECORD, CPE_XCS_RECORD_PLAYBACK, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
    

}


int CHandleXcsPtl::RecordFileDownLoadQuery(XCS_CLIENT *pClintSocket)
{
    char cDevIp[16] ={0};
    PSTR_RECORD_PC_FILE_DOWNLOAD pStrPCRecDownLoad = (PSTR_RECORD_PC_FILE_DOWNLOAD)(pClintSocket->pRecvBuf+NT_PACK_HEAD_LENGTH);
    PSTR_RECORD_FILE_DOWNLOAD pStrRecDownLoad = (PSTR_RECORD_FILE_DOWNLOAD)(pClintSocket->pSendBuf+NT_PACK_HEAD_LENGTH);
    char *pURL =NULL;
    char *pfilname =NULL;
    struct in_addr in;

    RTSP_CONF_INFO strSysRtspCfg;
    memcpy(cDevIp, cfgSockGetValue(NULL, "ipaddr", "0.0.0.0"), sizeof(cDevIp));
    memset(&strSysRtspCfg, 0, sizeof(RTSP_CONF_INFO));
    int nRet = SysConfGet(e_RTSP_CONF, &strSysRtspCfg, 0);
    if( 0 > nRet)
    {
        XCSPTL_ERROR("download record get rtsp Conf failed.\n");
        SetNakPack( (PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), ERROR_READ_CONF);
        return -1;
    }
    else
    {
        in.s_addr = strSysRtspCfg.dwRtspIpAddr;
	    
        pURL = pStrRecDownLoad->cFileUrl;
        pfilname = pStrPCRecDownLoad->cFileName;
        if(memcmp(pfilname, "/ide", 4) == 0)
        {
            pfilname += 4;
        }
        if ( strSysRtspCfg.nRtspMode != 2 )
        {
                sprintf(pURL, "http://%s:%d%s", inet_ntoa(in), strSysRtspCfg.dwRtspPort, pfilname);
        }
        else        
        {
            char szUserPass[64] = {0};
            if (strlen(strSysRtspCfg.szRtspUsername)>0 && strlen(strSysRtspCfg.szRtspPassword)>0)
            {
                sprintf( szUserPass, "%s:%s@", strSysRtspCfg.szRtspUsername, strSysRtspCfg.szRtspPassword);
            }
            sprintf(pURL, "http://%s%s:%d/%s/%s%s",
                    szUserPass, inet_ntoa(in), strSysRtspCfg.dwRtspDownLoadPort,
                    strSysRtspCfg.szRtspPath, cDevIp,
                    pfilname);
        }
    }
    /* 3.组织应答包 */
    pClintSocket->nSendLen = NT_PACK_HEAD_LENGTH + sizeof(STR_RECORD_PLAYBACK);
    SetSimpleAckHead((PSTR_PACKET_HEADER)(pClintSocket->pSendBuf), CPH_XCS_RECORD, CPE_RECORD_FILE_DOWNLOAD, \
    pClintSocket->nSendLen - NT_PACK_HEAD_LENGTH);
    return 0;
    

}
#endif

