#ifndef _MEMPOOL_H_
#define _MEMPOOL_H_

//#include "commoninc.h"

#define SEM_KEY_MEM_LOCK    (0x3110)

#define DEV_SHAREMEM        "/dev/misc/app_shm"
//#define DEV_SHAREMEM        "/exappflash/sharemem.txt"

#define MEMPOOL_SHARE_MEM_LEN       (4*1024*1024) //(20*1024*1024)

#if defined(__cplusplus)
extern "C" {
#endif

int  comm_shm_init();
int mempool_create(void);
void *mempool_malloc(size_t size);
int mempool_free(void * mem);
int mempool_inc_ref(void *mem);
unsigned int mempool_offset(void *mem);
void* mempool_pointer(unsigned int offset);
int mempool_all_free(void);

#if defined(__cplusplus)
}
#endif

#endif
