/******************************************************************************
 * dCtlGpioOut.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2011.09.06, wangbin Create
 * --------------------
 ******************************************************************************/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>

#include "sysHandle_define.h"
#include "hardInfoRw.h"

CHardInfoRW::CHardInfoRW(char *pcDevName)
{
    if(NULL != pcDevName)
    {
        memcpy(m_devName, pcDevName, sizeof(m_devName));
        m_iFd = open(m_devName, O_RDWR);
        if(-1 == m_iFd)
        {
            SYSHANDLE_ERROR("open %s dev error!\n", m_devName);
        }
        else
        {
            SYSHANDLE_DEBUG("open %s ok\n", m_devName);
        }
    }
    else
    {
        memcpy(m_devName, HARD_VER_DEV_NAME, sizeof(m_devName));
        m_iFd = open(m_devName, O_RDWR);
        if(-1 == m_iFd)
        {
            SYSHANDLE_ERROR("open %s dev error!\n", m_devName);
        }
        else
        {
            SYSHANDLE_DEBUG("open %s ok\n", m_devName);
        }
    }
}

CHardInfoRW::~CHardInfoRW()
{
    if(m_iFd > 0)
        close(m_iFd);
}

int CHardInfoRW::hardInfoRead(unsigned int *puiVer)
{
    if(m_iFd > 0)
    {
        ioctl(m_iFd, MISC_HARDWARE_READ, puiVer);//ver»ñÈ¡°æ±¾ºÅ
        SYSHANDLE_ERROR("%s get hardware ver ok,val:0x%x!\n", m_devName, *puiVer);
        return 0;
    }
    else
    {
        SYSHANDLE_ERROR("%s dev not opened!\n", m_devName);
    }
    return -1;
}

int CHardInfoRW::hardInfoDevClose()
{
    if(m_iFd > 0)
        close(m_iFd);
    else
    {
        SYSHANDLE_ERROR("gpio out fd not opened\n");
        return -1;
    }
    return 0;
}


