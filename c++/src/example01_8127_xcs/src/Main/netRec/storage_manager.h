#ifndef NAS_STORAGE_H
#define NAS_STORAGE_H

#include <stdint.h>
#include <msgQlib.h>
#include "storage.h"

#define  NAS_TRANSFER_TYPE_RTP   "RTP"
#define  NAS_TRANSFER_TYPE_TCP   "TCP"

#define  NAS_UDP        (1)
#define  NAS_TCP        (3)

int StorageCfgInit(void);
int nasReportPuid(const char *puid1, const char *puid2);
//int nasChannelStorageStart(int nChannel,uint32_t *pActiveData, uint32_t *pPort, uint32_t*pTransferType);
int nasChannelStorageStart(unsigned int nChannel);

int nasChannelStorageStop(unsigned int nChannel);



#if 0
//开启直写通道    (每隔录像通道都对应唯一的GUID,由存储服务器分配给前端设备)
int nasTrackOpen(const char *Mac, const char * Guid);


//关闭直写通道协议
int nasTrackClose(const char *Mac, const char * Guid);
#endif

//获取直写通道配额
int nasQuotaGet(const char *Mac, const char * Guid,
                uint32_t *pSizeLimit,      //容量限制
                 uint32_t *pTimeLimit);     //时间限制

//设置直写通道配额
int nasQuotaSet(const char *Mac, const char * Guid,
                 uint32_t SizeLimit, //容量限制
                uint32_t TimeLimit); //时间限制

int nasTrackRedirect(const char *xml_buf, int nlen);
int nasRecordRedirectResp(int sockfd ,int nErrorCode);
MSG_Q_ID nasRedirectMsgQGet(void);
int nasChannelStorageGetStartTimes(unsigned int nChannel);
int nasChannelStorageGetNruDeadTimes(unsigned int nChannel);
int nasChannelStorageNruScan(unsigned int nChannel);
int nasChannelStorageGetRecordPositon(unsigned int nChannel);
int nasChannelStorageGetNruStatus(unsigned int nChannel);
int nasChannelStorageDealWithGettingNruError(unsigned int nChannel);
int nasChannelStorageDealWithGettingNruSuccess(unsigned int nChannel, char *szIp,char *szHttpPort);
int nasChannelStorageNrmManageIpPortSet(unsigned int nChannel);

#endif

