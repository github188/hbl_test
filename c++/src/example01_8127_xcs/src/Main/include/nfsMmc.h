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
�ж��豸û��mount��

* ����:            �ɹ�mount����0 ��ʧ�ܷ���-1
*/
int checkmount(eMountType type);


#ifdef __cplusplus
}
#endif



#endif
