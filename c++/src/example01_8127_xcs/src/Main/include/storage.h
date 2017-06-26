#ifndef __SRORAGE_H_
#define __SRORAGE_H_

#define UTC_TIME_ZONE 8*3600

/* ���ڴ�NRM��ȡNRU IP,��ȡ�����У���û��ȡ�ɹ� */
#define  GETTING_NRU_IP_FROM_NRM           (0)    

/* ��NRM��ȡ��NRU��IP,����¼�ƵĹ����� */
#define  RECORDING_TO_NRU_GET_FROM_NRM     (1)   

/* ���ڴ����ʹ�õ�NRU IP+NRM��ȡ�Ĺ���˿ڻ�ȡNRU IP */
#define  GETTING_NRU_IP_FROM_NRU_HTTP      (2) 

/* NRU IP�����ʹ�õ�NRU�Ĺ���˿ڻ�ȡ�ɹ�����¼�� */
#define  RECORDING_TO_NRU_FROM_NRU_HTTP    (3)      

/* �����޷����ӵ�NRM�����Ҳδʹ�ù�NRU,ϵͳδ�������ʹ�õ�NRU IP */
#define  RECORD_DIRECT_TO_NAS              (4)     

int StorageInit(void);
int StorageUninit(void);
int StorageDataWrite(int chanId, void *shareBuff, int nLen);
int StorageDebugSwitch(int nLevel);
int nasChannelStorageGetRecordPositon(unsigned int nChannel);

#endif

