#ifndef __NETFS_CHECK_H
#define __NETFS_CHECK_H

#include "msgQLib.h"

#define _PACKED_ __attribute__((packed))

typedef struct
{
    int  bEnable;
    char  szIpAddr[32];
    char  szRootPath[32];
    char  szMountPath[32];
    int   iCheckInteval;
    MSG_Q_ID  strMsgId;
}_PACKED_ STR_NETFS_INFO;

#define NETFS_STA_CHANGE    		(0x55)

#define NETFS_STA_BAD    0      /* 失败*/
#define NETFS_STA_OK     1      /* 状态良好*/
#define NETFS_OK2BAD     2      /* 状态由好变坏,必须重启 */
#define NETFS_BAD2OK     3      /* 状态由坏变好*/
#define NETFS_MOUNTBAD   4      /* CIFS ping OK,mount failed */
#define NETFS_STA_INIT   5      /* 初始状态 */


#define NETFS_NFS         (1)
#define NETFS_CIFS        (2)


typedef struct 
{
    unsigned int uiNetFsType;
    unsigned int uistatus;
}_PACKED_ STR_NETFS_STA;

#ifdef __cplusplus
extern "C" {
#endif

extern int SendMsgToApp(MSG_Q_ID  strMsgId,STR_NETFS_STA *pStrNetFsSta);

extern int CifsStaCheckInit(STR_NETFS_INFO * pNetFsInfo);
extern int CifsStaCheckUninit(void);

extern int NfsStaCheckInit(STR_NETFS_INFO * pNetFsInfo);
extern int NfsStaCheckUninit(void);


#ifdef __cplusplus
}
#endif

#endif
