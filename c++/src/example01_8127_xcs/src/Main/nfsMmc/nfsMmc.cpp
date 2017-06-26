#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nfsMmc.h"

#include "SysHandle.h"

int checkmount(eMountType type)// type == 0 nfs  type==1 sd
{
	char buffer[512];
	char mountpath[512];
    unsigned char ucUdiskEn =0;
    unsigned char ucSDEn =0;
	unsigned int flag = 0;
	FILE *fp;

	memset(mountpath,0,sizeof(mountpath));
	if ( type == e_sd )
	{
	    ucSDEn = sdEnFlagGet();
	    if(ucSDEn == 0)
	    {
	        shell_print("%s %d sd En is 0, return -1!", __FUNCTION__, __LINE__);
	        return -1;
	    }
	    strcpy(mountpath, SD_MOUNT_POINT);
	}
	else if ( type == e_nfs )
	{// nfs
		strcpy(mountpath,"/nfs");
	}
	else if (type == e_cifs)
	{
	    strcpy(mountpath,"/pics");
	}
	else if ( e_udisk == type )
	{
	    ucUdiskEn = uDiskEnFlagGet();
	    if(ucUdiskEn == 0)
	    {
	        shell_print("%s %d uDisk En is 0, return -1!", __FUNCTION__, __LINE__);
	        return -1;
	    }
	    strcpy(mountpath, U_DIST_MOUNT_POINT);
	}
	else
	{
	    shell_print("%s %d type:%d is error!", __FUNCTION__, __LINE__, type);
		return -1;
	}
	fp = fopen("/proc/mounts", "r");
    if(fp == NULL)
    {
        //syslog(LOG_ERR|LOG_USER, " checkmount open file failed\n");
        shell_print("%s %d fopen proc mounts file error!", __FUNCTION__, __LINE__);
        return -1;
    }
	for(;;)
	{
		fgets(buffer, sizeof(buffer), fp);
		if (feof(fp))
		{
			break;
		}
		if (NULL != strstr( buffer,mountpath))
		{
			flag = 1;
			break;
		}
	}

	fclose(fp);

	return (flag)? 0 : -1;
}

