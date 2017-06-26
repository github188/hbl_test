#include <semaphore.h>
#include <stdio.h>
#include "memShm.h"
#include "mempool.h"

#if 1

#if defined(PARALLEL_TASK)
static sem_t   g_mempool_sem;
#endif
#define MY_PRINTF printf

int memShmAllocator(void)
{
    int ret = -1;

#if defined(PARALLEL_TASK)
    if((ret = sem_init(&g_mempool_sem, 0 ,1)) != 0)
    {
        MY_PRINTF("sem init failed \n");
        goto done;
    }
#endif

    if(comm_shm_init() < 0)
    {
        MY_PRINTF("comm_shm_init failed");
        goto done;
    }
    MY_PRINTF("comm_shm_init\n");

    if(mempool_create() < 0)
    {
        MY_PRINTF("mempool_create failed");
        goto done;
    }
    MY_PRINTF("mempool_create\n");

    ret = 0;

done:
    return ret;

}

int memShmUnallocator(void)
{
#if defined(PARALLEL_TASK)
    sem_destroy(&g_mempool_sem);
#endif
    return 0;
}
int memShmAllFree(void)
{
#if defined(PARALLEL_TASK)
     sem_wait(&g_mempool_sem);
#endif

     mempool_all_free();

#if defined(PARALLEL_TASK)
     sem_post(&g_mempool_sem);
#endif
     return 0;
}
int memShmFree(void *shareBuff)
{
#if defined(PARALLEL_TASK)
     sem_wait(&g_mempool_sem);
#endif

     mempool_free(shareBuff);

#if defined(PARALLEL_TASK)
     sem_post(&g_mempool_sem);
#endif
     return 0;
}

void* memShmMalloc(int nSize)
{

    void *pBuff = NULL;

#if defined(PARALLEL_TASK)
    sem_wait(&g_mempool_sem);
#endif

    pBuff = mempool_malloc(nSize);

#if defined(PARALLEL_TASK)
    sem_post(&g_mempool_sem);
#endif
    return pBuff;
}

void* memShmPointer(unsigned int offset)
{
    void *pBuff = NULL;

#if defined(PARALLEL_TASK)
    sem_wait(&g_mempool_sem);
#endif

    pBuff = mempool_pointer(offset);

#if defined(PARALLEL_TASK)
    sem_post(&g_mempool_sem);
#endif
    return pBuff;
}

unsigned int memShmOffset(void *mem)
{
    unsigned int ret = 0;

#if defined(PARALLEL_TASK)
    sem_wait(&g_mempool_sem);
#endif

    ret = mempool_offset(mem);

#if defined(PARALLEL_TASK)
    sem_post(&g_mempool_sem);
#endif
    return ret;
}

#endif
