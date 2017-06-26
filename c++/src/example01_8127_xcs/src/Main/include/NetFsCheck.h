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

#define NETFS_STA_BAD    0      /* ʧ��*/
#define NETFS_STA_OK     1      /* ״̬����*/
#define NETFS_OK2BAD     2      /* ״̬�ɺñ仵,�������� */
#define NETFS_BAD2OK     3      /* ״̬�ɻ����*/
#define NETFS_MOUNTBAD   4      /* CIFS ping OK,mount failed */
#define NETFS_STA_INIT   5      /* ��ʼ״̬ */


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
