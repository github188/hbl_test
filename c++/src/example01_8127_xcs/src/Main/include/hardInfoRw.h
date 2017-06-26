/******************************************************************************
 * hardInfoRW.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.20, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_HARD_INFO_RW_H__
#define _DEV_HARD_INFO_RW_H__

#define MISC_HARDWARE_READ _IOR('M', 0x17, unsigned long)  /* Read Alarm sta*/ 
 
#define HARD_VER_DEV_NAME "/dev/misc/hwver"
#define VERDEVNAMELEN 0x30

class CHardInfoRW 
{
public:
    CHardInfoRW(char *pcDevName);
    ~CHardInfoRW ();

    /* 通过ioctl函数设置指定GPIO 输出为1或为0 */
    int hardInfoRead(unsigned int *puiVer);

private:
    int  hardInfoDevClose();

    char    m_devName[VERDEVNAMELEN];
    int     m_iFd;                          //设备号
};

#endif /* _DEV_HARD_INFO_RW_H__ */


