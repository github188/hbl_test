#ifndef __NFS_MMC_H
#define __NFS_MMC_H

typedef enum
{
	e_nfs = 0,
	e_sd = 1,
	e_udisk =2,
	e_cifs	=3,
}eMountType;

#ifdef __cplusplus
extern "C" {
#endif

/*
判断设备没有mount上

* 返回:            成功mount返回0 ，失败返回-1
*/
int checkmount(eMountType type);


#ifdef __cplusplus
}
#endif



#endif
