#ifndef __MEMSHM_H__
#define __MEMSHM_H__

int                 memShmAllocator(void);
int                 memShmUnallocator(void);
int                 memShmAllFree(void);
int                 memShmFree(void *shareBuff);
void*               memShmMalloc(int nSize);
void*               memShmPointer(unsigned int offset);
unsigned int        memShmOffset(void *mem);

#endif
