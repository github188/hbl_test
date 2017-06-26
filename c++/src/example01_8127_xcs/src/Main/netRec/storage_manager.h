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
//����ֱдͨ��    (ÿ��¼��ͨ������ӦΨһ��GUID,�ɴ洢�����������ǰ���豸)
int nasTrackOpen(const char *Mac, const char * Guid);


//�ر�ֱдͨ��Э��
int nasTrackClose(const char *Mac, const char * Guid);
#endif

//��ȡֱдͨ�����
int nasQuotaGet(const char *Mac, const char * Guid,
                uint32_t *pSizeLimit,      //��������
                 uint32_t *pTimeLimit);     //ʱ������

//����ֱдͨ�����
int nasQuotaSet(const char *Mac, const char * Guid,
                 uint32_t SizeLimit, //��������
                uint32_t TimeLimit); //ʱ������

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

