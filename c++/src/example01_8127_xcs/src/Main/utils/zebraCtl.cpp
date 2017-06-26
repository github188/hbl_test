#if defined(CYC200XZD)||defined(CYC500XZD)
/******************************************************************************
 * zebraCtl.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2010.05.05, wb Create
 * --------------------
 ******************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <sys/time.h>
#include <syslog.h>

#include "vehicleReport.h"
#include "vehicleBasePro.h"
#include "dctlApi.h"
#include "zebraCtl.h"

int g_zebra_debug_module = 0xffff;
sem_t s_zebraSem;
sem_t s_zebraRegCtlSem;

STRZEBRAREGVAL g_zebraRegVal;

extern int g_iSpiBusSta;

int spiStrInit(spi_param_t * strSpiPro)
{
    strSpiPro->ucStartFlag1 = SPI_MSG_START_FLAG1;
    strSpiPro->ucStartFlag2 = SPI_MSG_START_FLAG2;
    
    strSpiPro->ucEndFlag1 = SPI_MSG_END_FLAG1;
    strSpiPro->ucEndFlag2 = SPI_MSG_END_FLAG2;
    return 0;
}

/* 本函数不应该进行握手信号的判断，它本身就是用于查询spi总线状态的 */
int dm368SpiBusStaQuery(void)
{
    spi_param_t strSpiPro;

    memset(&strSpiPro, 0, sizeof(strSpiPro));

    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM6467_QUERY_SPI_STA;

    sprintf(strSpiPro.ucData, "%x", 1);
    
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));

    return 0;
}

int dm368SysTimeSet(unsigned int uiSecs)
{
    spi_param_t strSpiPro;
    
    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();
        return -1;
    }
    memset(&strSpiPro, 0, sizeof(strSpiPro));

    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM368_SYS_TIME_SET;

    sprintf(strSpiPro.ucData, "%8x", uiSecs);
    
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));

    return 0;
}

int dm368DebugOutModeSet(char cMode)
{
    spi_param_t strSpiPro;

    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();
        return -1;
    }

    memset(&strSpiPro, 0, sizeof(strSpiPro));

    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM368_DEBUG_OUT_MODE;

    sprintf(strSpiPro.ucData, "%x", cMode);
    
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));

    return 0;
}

int dm368DebugLevelSet(char cLevel)
{
    spi_param_t strSpiPro;

    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();    
        return -1;
    }

    memset(&strSpiPro, 0, sizeof(strSpiPro));

    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM368_SET_DEBUG_LEVEL;

    sprintf(strSpiPro.ucData, "%x", cLevel);
    
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));

    return 0;
}

int dm368PGCamReboot(void)
{
    spi_param_t strSpiPro;

    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();    
        return -1;
    }

    memset(&strSpiPro, 0, sizeof(strSpiPro));

    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM6467_REBOOT_PG_CAM;
  
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));

    return 0;
}

int dm368ProgramVerQuery(void)
{
    spi_param_t strSpiPro;

    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();    
        return -1;
    }

    memset(&strSpiPro, 0, sizeof(strSpiPro));

    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM6467_QUERY_VERSION;
  
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));

    return 0;
}

int zebraCtlRegValSet(int iAdd, unsigned int uiVal)
{
    spi_param_t strSpiPro;

    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();
        return -1;
    }

    memset(&strSpiPro, 0, sizeof(strSpiPro));

    sem_wait(&s_zebraRegCtlSem);

    if(iAdd < 0x900)
    {
        uiVal  |=  0x82000000;
    }
    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM6467_SET_ZEBRA_REG_ADDR_VAL;

    sprintf(strSpiPro.ucData,   "%4x", iAdd);
    sprintf(strSpiPro.ucData+4, "%8x", uiVal);
    ZEBRACTL_INFO("zebra set,iadd:0x%x,val:0x%x string is %s.\n", iAdd, uiVal, strSpiPro.ucData);
    
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));
    sem_post(&s_zebraRegCtlSem);
    
    return 0;
}

int zebraCtlRegValGet(int iAddr, unsigned int *puiVal)
{
    int ret =0;
    int iTmpAddr =0;
    unsigned char ucTmpProSeq =0;
    char cTmp[16] ={0};
    char cTryTimes =0;
    struct timespec abs_ts;
	struct timeval tv;
	struct timezone tz;
    spi_param_t strSpiPro;
    static unsigned char s_ucQureySeq =0;

    if(g_iSpiBusSta != 1)
    {
        dm368SpiBusStaQuery();
        return -1;
    }

    sem_wait(&s_zebraRegCtlSem);

spi_flag:
    cTryTimes++;
	
    if(cTryTimes > 2)
    {
        ZEBRACTL_ERROR("zebra return reg:0x%x,act reg:0x%x,retry times:%d,direc return!\n",\
                    iTmpAddr, iAddr, cTryTimes);

        sem_post(&s_zebraRegCtlSem);
        
        syslog(LOG_ERR|LOG_USER, "query zebra reg error,return reg:0x%x,act reg:0x%x,retry times:%d,direc return!\n",\
                    iTmpAddr, iAddr, cTryTimes);
        return -1;
    }
    
    s_ucQureySeq++;
    
    memset(&strSpiPro, 0, sizeof(strSpiPro));
    spiStrInit(&strSpiPro);
    strSpiPro.ucProType = CMD_DM6467_QUERY_ZEBRA_REG_ADDR;
    strSpiPro.ucProSeq = s_ucQureySeq;
    
    sprintf(strSpiPro.ucData, "%4x", iAddr);
    ZEBRACTL_INFO("zebra query addr:0x%x,proSeq:%d,string is %s.\n", iAddr, s_ucQureySeq, strSpiPro.ucData);

    while(1)
    {
        gettimeofday(&tv, &tz);
        abs_ts.tv_sec = tv.tv_sec;
    	ret = sem_timedwait(&s_zebraSem, &abs_ts);
        if(ret != 0)
        {
            break;
        }
    }
    
    dCtlModuleCapWirte(DEV_TYPE_SPI, (char *)&strSpiPro, (int)sizeof(spi_param_t));
    
	gettimeofday(&tv, &tz);

    abs_ts.tv_sec = tv.tv_sec+(200 +(tv.tv_usec/1000))/1000;
	abs_ts.tv_nsec = ((200 +(tv.tv_usec/1000))%1000)*1000*1000;
	
	ret = sem_timedwait(&s_zebraSem, &abs_ts);
    if(0 == ret)
    {
        ZEBRACTL_INFO("zebra query return str is %s.return proSeq is %d.\n", (char *)&g_zebraRegVal, g_zebraRegVal.ucProSeq);
    
        memset(cTmp, 0, sizeof(cTmp));
        strncpy(cTmp, g_zebraRegVal.aRegAdd, 4);
        iTmpAddr = strtoul(cTmp, NULL, 16);

        memset(cTmp, 0, sizeof(cTmp));
        strncpy(cTmp, g_zebraRegVal.aRegVal, 8);
        *puiVal = strtoul(cTmp, NULL, 16);

		ucTmpProSeq = g_zebraRegVal.ucProSeq;
		
        memset((char *)&g_zebraRegVal, 0, sizeof(g_zebraRegVal));

        if(iTmpAddr != iAddr)
        {
            ZEBRACTL_ERROR("zebra first return reg:0x%x,act reg:0x%x,go to retry!\n", iTmpAddr, iAddr);
            syslog(LOG_ERR|LOG_USER, "zebra first return error!return reg:0x%x,act reg:0x%x,go to retry!\n", iTmpAddr, iAddr);
            goto spi_flag;
        }
        
		#if 1
        if(s_ucQureySeq != ucTmpProSeq)
        {
        	ZEBRACTL_ERROR("zebra first proSeq error!return seq:0x%x,act seq:0x%x,go to retry!\n", ucTmpProSeq, s_ucQureySeq);
        	syslog(LOG_ERR|LOG_USER, "zebra first proSeq error!return seq:0x%x,act seq:0x%x,go to retry!\n", ucTmpProSeq, s_ucQureySeq);
            goto spi_flag;
        }
        #endif
    }
    else
    {
        ZEBRACTL_ERROR("not wait spi data,retry sem_timedwait!\n");
        gettimeofday(&tv, &tz);

        abs_ts.tv_sec = tv.tv_sec+(1000 +(tv.tv_usec/1000))/1000;
    	abs_ts.tv_nsec = ((1000 +(tv.tv_usec/1000))%1000)*1000*1000;
    	
    	ret = sem_timedwait(&s_zebraSem, &abs_ts);
        if(0 == ret)
        {
            ZEBRACTL_ERROR("zebra second query ok,return str is %s.\n", (char *)&g_zebraRegVal);
        
            memset(cTmp, 0, sizeof(cTmp));
            strncpy(cTmp, g_zebraRegVal.aRegAdd, 4);
            iTmpAddr = strtoul(cTmp, NULL, 16);
            //ZEBRACTL_ERROR("zebra return str:%s,reg add is 0x%x.\n", cTmp, iTmpAddr);

            memset(cTmp, 0, sizeof(cTmp));
            strncpy(cTmp, g_zebraRegVal.aRegVal, 8);
            *puiVal = strtoul(cTmp, NULL, 16);
            //ZEBRACTL_ERROR("zebra return str:%s,reg val is 0x%x.\n", cTmp, *puiVal);

			ucTmpProSeq = g_zebraRegVal.ucProSeq;

            memset((char *)&g_zebraRegVal, 0, sizeof(g_zebraRegVal));

            if(iTmpAddr != iAddr)
            {
                ZEBRACTL_ERROR("zebra second return reg:0x%x,act reg:0x%x,go to retry!\n", iTmpAddr, iAddr);
				syslog(LOG_ERR|LOG_USER, "zebra second return reg:0x%x,act reg:0x%x,go to retry!\n", iTmpAddr, iAddr);
                goto spi_flag;
            }
            
			#if 1
            if(s_ucQureySeq != ucTmpProSeq)
	        {
				ZEBRACTL_ERROR("zebra second proSeq error!return seq:0x%d,act seq:0x%x,go to retry!\n", ucTmpProSeq, s_ucQureySeq);
	        	syslog(LOG_ERR|LOG_USER, "zebra second proSeq error!return seq:0x%d,act seq:0x%x,go to retry!\n", ucTmpProSeq, s_ucQureySeq);
	            goto spi_flag;
	        }
	        #endif
        }
        else
        {
            ZEBRACTL_ERROR("not wait spi data, go to rerty!\n");
            goto spi_flag;
        }
    }
    
    sem_post(&s_zebraRegCtlSem);
    return 0;
}


#endif

