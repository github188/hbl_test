#ifndef __CISCHECK_H__
#define __CISCHECK_H__




typedef enum tagcifsStat
{
    CIFS_WAIT_MOUNT = 0,
    CIFS_PRE_QUERY  = 1,
    CIFS_QUERY      = 2,
    CIFS_CHK_MOUNT  = 3,
    CIFS_CHK_UMOUNT = 4,
    CIFS_PING       = 5,
    CIFS_WAIT_PING  = 6,
    CIFS_CHK_PING   = 7,
    CIFS_CHK_OFF    = 255
}cifs_stat_t;

typedef struct
{
    const char *strPath; // share folder path
    const char *strAddr;  // IP address
    cifs_stat_t tStat;
    int iFailCnt;
    FILE *pipe;
}check_cifs_t;


#ifdef __cplusplus
extern "C"
{
#endif

 


#ifdef __cplusplus
};
#endif


#endif
