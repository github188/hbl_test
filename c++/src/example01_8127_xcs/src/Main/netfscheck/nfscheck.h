#ifndef __NFSCHECK_H__
#define __NFSCHECK_H__

#include <rpc/rpc.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>

typedef enum tagNfsStat
{
    NFS_WAIT_MOUNT = 0,
    NFS_PRE_QUERY  = 1,
    NFS_QUERY      = 2,
    NFS_CHK_MOUNT  = 3,
    NFS_CHK_UMOUNT = 4,
    NFS_PING       = 5,
    NFS_WAIT_PING  = 6,
    NFS_CHK_PING   = 7,
    NFS_CHK_OFF    = 255
}nfs_stat_t;

typedef struct
{
//    char strPath[256]; // share folder path
//    char strAddr[16];  // IP address
    const char *strPath; // share folder path
    const char *strAddr;  // IP address

    nfs_stat_t tStat;
    int iFailCnt;
    FILE *pipe;
	CLIENT *client;
}check_nfs_t;


#ifdef __cplusplus
extern "C"
{
#endif

#ifdef __cplusplus
};
#endif


#endif
