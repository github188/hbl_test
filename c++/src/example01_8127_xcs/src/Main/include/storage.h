#ifndef __SRORAGE_H_
#define __SRORAGE_H_

#define UTC_TIME_ZONE 8*3600

/* 正在从NRM获取NRU IP,获取过程中，还没获取成功 */
#define  GETTING_NRU_IP_FROM_NRM           (0)    

/* 从NRM获取到NRU的IP,并在录制的过程中 */
#define  RECORDING_TO_NRU_GET_FROM_NRM     (1)   

/* 正在从最近使用的NRU IP+NRM获取的管理端口获取NRU IP */
#define  GETTING_NRU_IP_FROM_NRU_HTTP      (2) 

/* NRU IP从最近使用的NRU的管理端口获取成功并在录制 */
#define  RECORDING_TO_NRU_FROM_NRU_HTTP    (3)      

/* 开机无法连接到NRM，最近也未使用过NRU,系统未保存最近使用的NRU IP */
#define  RECORD_DIRECT_TO_NAS              (4)     

int StorageInit(void);
int StorageUninit(void);
int StorageDataWrite(int chanId, void *shareBuff, int nLen);
int StorageDebugSwitch(int nLevel);
int nasChannelStorageGetRecordPositon(unsigned int nChannel);

#endif

