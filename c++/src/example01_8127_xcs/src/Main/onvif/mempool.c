#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <strings.h>
#include <sys/mman.h>
#include <syslog.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mempool.h"
#include "shmAlloc.h"



volatile unsigned char *pmapmem = NULL;

volatile int __g_sem_alloc_lock = -1;

volatile shm_alloc_handle __g_shm_allocator;


/*ª•≥‚–≈∫≈¡ø,sem*/
typedef union _semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
}semun;

int  sem_get(key_t key)
{
    int status = 0;
    int semid;

    semid = semget(key,1,0660|IPC_CREAT|IPC_EXCL);
    if(-1 == semid)
    {
        if(errno == EEXIST)
        {
            semid = semget(key,1,0);
        }
    }
    else
    {
        /*if created...*/
        semun arg;
        arg.val = 1;
        status  = semctl(semid,0,SETVAL,arg);
    }
    if( (-1 == semid) || (-1 == status) )
    {
        return -1;
    }
    return semid;
}
int sem_delete(int semid)
{
    return semctl(semid,0,IPC_RMID,NULL);
}

/*P*/
int sem_take(int semid)
{
    struct sembuf p_buf;
    p_buf.sem_num = 0;
    p_buf.sem_op = -1;
    p_buf.sem_flg = SEM_UNDO;

    if(semop(semid,&p_buf,1)==-1)
    {
        perror("p (semid) failed\n");
        syslog(LOG_ERR, "p (semid) failed %d %s\n",errno, strerror(errno));
        exit(1);
    }
    return 0;
}

/*V*/
int sem_give(int semid)
{
    struct sembuf v_buf;
    v_buf.sem_num = 0;
    v_buf.sem_op = 1;
    v_buf.sem_flg = SEM_UNDO;

    if(semop(semid,&v_buf,1)==-1)
    {
        perror("v (semid) failed\n");
        syslog(LOG_ERR,"v (semid) failed %d %s\n",errno, strerror(errno));
        exit(1);
    }
    return 0;
}



int  comm_shm_init()
{
	int ret =0;
    int fd_sharemem;
    if ((fd_sharemem=open(DEV_SHAREMEM, O_CREAT|O_RDWR|O_TRUNC, 00777))<0)
    {
      shell_print("ERROR:can not open sharemem device %s.\n", DEV_SHAREMEM);
      return -1;
    }
    else
    {
    	shell_print("ok:open sharemem device %s ok.\n", DEV_SHAREMEM);
    }

#if 0
	else
	{
		ret = lseek(fd_sharemem, MEMPOOL_SHARE_MEM_LEN, SEEK_SET);
		if(ret < 0)
		{
			shell_print("ERROR:can not lseek to end.\n");
		    return -1;
		}
	}

	write(fd_sharemem,' ', 1);
#endif

    pmapmem = (unsigned char*)mmap(NULL, MEMPOOL_SHARE_MEM_LEN, PROT_READ | PROT_WRITE, MAP_SHARED|MAP_NORESERVE, fd_sharemem, 0x0);
    if (pmapmem==MAP_FAILED)
    {
        close(fd_sharemem);
        shell_print(" mapped failed pdata = 0x%x  %d %s\n",(int)pmapmem,errno,strerror(errno));
        return -1;
    }

    close(fd_sharemem);

    bzero((void*)pmapmem,MEMPOOL_SHARE_MEM_LEN);
    msync((void*)pmapmem,MEMPOOL_SHARE_MEM_LEN,MS_SYNC|MS_INVALIDATE);

    shell_print("pdata suceessfully mapped pdata = 0x%x MEMPOOL_SHARE_MEM_LEN:%d\n",(int)pmapmem,MEMPOOL_SHARE_MEM_LEN);

    return ret;

}
static volatile void *mmap_start = MAP_FAILED;
uint8_t *comm_shm_attach(int length,int offset)
{
    uint8_t   *p_shared_mem=NULL;

    if(mmap_start == MAP_FAILED)
    {
        int fd=open(DEV_SHAREMEM,O_RDWR);
        if (fd < 0)
        {
        	shell_print("open %s failed.\n", DEV_SHAREMEM);
            return NULL;
        }
        else
        {
        	shell_print("open %s ok.\n", DEV_SHAREMEM);
        }
        mmap_start = mmap(NULL, MEMPOOL_SHARE_MEM_LEN, PROT_WRITE|PROT_READ, MAP_FILE|MAP_SHARED, fd, 0);
        if (mmap_start==MAP_FAILED)
        {
        	shell_print("mmap failed.size is 0x%x.\n", MEMPOOL_SHARE_MEM_LEN);
            close(fd);
            return NULL;
        }
        else
        {
        	shell_print("mmap ok.size is 0x%x.\n", MEMPOOL_SHARE_MEM_LEN);
        }
        close(fd);
    }
    p_shared_mem = (uint8_t*)mmap_start + offset;
    return p_shared_mem;
}
void comm_shm_invalidate(void *addr, int size)
{
    msync(addr,size,MS_INVALIDATE);
}

void comm_shm_flush(void *addr, int size)
{
    msync(addr,size,MS_SYNC|MS_INVALIDATE);
}

int mem_buf_attach(int initMem)
{
    uint8_t   *p_shared_mem=NULL;

    p_shared_mem = comm_shm_attach(MEMPOOL_SHARE_MEM_LEN, 0);//MM_DATA_BUF_SIZE,MM_DATA_BUF_BASE
    if (p_shared_mem == NULL)
    {
        shell_print("comm_shm_attach failed \n");
        return -1;
    }

    if (initMem)
    {
        memset((void*)p_shared_mem, 0, MEMPOOL_SHARE_MEM_LEN);
    }
    __g_shm_allocator = init_shm_allocator(p_shared_mem, MEMPOOL_SHARE_MEM_LEN); // + MM_DATA_BD_SIZE
    if (!__g_shm_allocator)
    {
        shell_print("init_shm_allocator failed \n");
        return -1;
    }
    __g_sem_alloc_lock = sem_get(SEM_KEY_MEM_LOCK);
   // msync(p_shared_mem, MEMPOOL_SHARE_MEM_LEN, MS_SYNC | MS_INVALIDATE);
    shell_print("mem_buf_attach ok \n");
    return 0;
}


int mem_buf_detach(void)
{
    return 0;
}

int mempool_create(void)
{
    mem_buf_attach(0);
    mem_buf_detach();
    return 0;
}

void *mempool_malloc(size_t size)
{
    void *mem;

    sem_take(__g_sem_alloc_lock);
    mem = shm_malloc(__g_shm_allocator, size);
    if (!mem)
    {
        //shm_alloc_dump(__g_shm_allocator);
    }
//    msync(g_shm.buf_base_addr, MM_DATA_BUF_SIZE, MS_SYNC|MS_INVALIDATE);
    sem_give(__g_sem_alloc_lock);

    return mem;
}

int mempool_free(void * mem)
{
    sem_take(__g_sem_alloc_lock);
    shm_free(__g_shm_allocator, mem);
//    msync(g_shm.buf_base_addr, MM_DATA_BUF_SIZE, MS_SYNC|MS_INVALIDATE);
    sem_give(__g_sem_alloc_lock);
    return 0;
}

int mempool_inc_ref(void *mem)
{
    int ret = -1;
    sem_take(__g_sem_alloc_lock);
    ret = shm_add_reference(__g_shm_allocator, mem);
//    msync(g_shm.buf_base_addr, MM_DATA_BUF_SIZE, MS_SYNC|MS_INVALIDATE);
    sem_give(__g_sem_alloc_lock);

    return ret;
}

int mempool_all_free(void)
{
    sem_take(__g_sem_alloc_lock);
    shm_all_recycle(__g_shm_allocator);
    sem_give(__g_sem_alloc_lock);
    return 0;
}

uint32_t mempool_offset(void *mem)
{
    return shm_offset(__g_shm_allocator, mem);
}

void * mempool_pointer(uint32_t offset)
{
    return shm_pointer(__g_shm_allocator, offset);
}

