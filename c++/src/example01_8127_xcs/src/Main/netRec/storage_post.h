#ifndef STORAGE_POST_H
#define STORAGE_POST_H

#include <stdint.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include "storage_define.h"

#define nasPostDebug(x...)    STG_DEBUG(x); 
#define TRACE()               printf("[*** %s %s %d  ***] \n",__FILE__, __FUNCTION__, __LINE__)
#define NRU

#if defined(NRU)
#define NAS_TRACK_OPEN          "/StartRecord"
#define NAS_TRACK_CLOSE         "/StopTrack"
#define NAS_QUOTA_GET           "/GetQuota"
#define NAS_QUOTA_SET           "/SetQuota"
#define NAS_MAP_TRACK           "/MapTrack"

#else
#define NAS_TRACK_OPEN          "/OpenTrack"
#define NAS_TRACK_CLOSE         "/StopTrack"
#define NAS_QUOTA_GET           "/GetQuota"
#define NAS_QUOTA_SET           "/SetQuota"
#define NAS_MAP_TRACK           "/MapTrack"
#endif
//nas http server 连接信息
//int  nasHttpSet(char *pszIp, int nPort);
int  nasHttpInfoSet(int Ip, int nPort);

int nasHttp_Post(const char *pszURI, const char *content, uint32_t len,
                            char *pszResult,uint32_t *pResultLen);
int nasHttp_Listen(int port);

int nasUdpHeartBeatSend(sockaddr_in server, unsigned short sequence_number, unsigned long long session);

unsigned int uNetAddrStr2IntHost(char szAddr[]);
#endif
