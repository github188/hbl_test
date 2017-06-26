/******************************************************************************
 * vehicleSpiReport.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  主要用于对川速雷达协议进行解析，结果为DSP可用于抓拍的通道号、
 * 车道号、步骤
 *     
 * modification history
 * --------------------
 * 01a, 2012.04.10, wangbin Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <semaphore.h>

#include "xcsGlobalDef.h"
#include "vehicleReport.h"
#include "vehicleBasePro.h"

#include "zebraCtl.h"
#include "SysHandle.h"

#if defined(CYC500XZD)||defined(CYC200XZD)

extern STRZEBRAREGVAL g_zebraRegVal;
extern sem_t s_zebraSem;

int g_iSpiBusSta =0;

int spiTransInfoQuery(unsigned char *pucData)
{
    spi_param_t *strSpiPro = ( spi_param_t * )pucData;
    if( strSpiPro->ucProType == CMD_DM6467_RESP_ZEBRA_REG_VAL )
    {
        strncpy((char *)&g_zebraRegVal, (char *)(strSpiPro->ucData), 13);
        g_zebraRegVal.ucProSeq = strSpiPro->ucProSeq;
        ZEBRACTL_INFO("spi zebra rev is %s.\n", (char *)(strSpiPro->ucData));
        ZEBRACTL_INFO("spi zebra seq is %d.\n", g_zebraRegVal.ucProSeq);
        sem_post(&s_zebraSem);
    }
    else if(strSpiPro->ucProType == CMD_DM368_SEND_DEBUG_INFO )
    {
        shell_print("dm368 debug:%s", strSpiPro->ucData);
    }
    else if(strSpiPro->ucProType == CMD_DM368_SEND_SPI_STA )
    {
        shell_print("dm368 spi bus sta is 0x%x\n", strSpiPro->ucData);
        g_iSpiBusSta =1;
    }
    else if(strSpiPro->ucProType == CMD_FRONT_REQUEST_SET_CAM)
    {
    	shell_print("dm368  request set cam param.\n");
		syslog(LOG_ERR|LOG_USER, "dm368  request set cam param!\n");

		int ret =0;
		STR_SYS_APP_MSG strTmpSysMsg;
	    memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
	    strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
	    strTmpSysMsg.uiMsgType = MSG_CAMARA_CTL_INFO_SET;
	    ret = sendMsgToMainApp(&strTmpSysMsg);
	    if(-1 == ret)
	    {
	        ZEBRACTL_ERROR("spi send cama ctl info Conf failed.\n");
	    }
	    else
	    {
	        ZEBRACTL_INFO("spi send cama ctl info msg ok.\n");
	    }
    }
    else
    {
        printf("error,spi type:0x%x!\n", strSpiPro->ucProType);
    }
	return 0;
}

#endif


