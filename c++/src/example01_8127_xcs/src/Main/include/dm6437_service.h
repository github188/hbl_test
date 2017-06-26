#ifndef _DM6437_INTERFACE_H
#define _DM6437_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dsp_service.h"

#define DM6437_SUCCESS     (0)
#define DM6437_FAILURE    (-1)

extern int shell_print(const char* format, ...);
extern int *g_dm6437_debug_level;
#define DM6437_DEBUG(level, format, args...) do{\
    if (g_dm6437_debug_level != NULL)\
    {\
        if (level & *g_dm6437_debug_level)\
        {\
            shell_print("%s(%d) : "format"\n",__FUNCTION__, __LINE__,  ##args);\
        }\
    }\
    else\
    {\
        if (level & LV_ERROR)\
        {\
            shell_print("%s(%d) : "format"\n",__FUNCTION__, __LINE__,  ##args);\
        }\
    }\
    }while(0)


int DM6437Init(int (*func)());
int DM6437Exit();
int DM6437SetDbgLevel(int level);
char * DM6437GetVersion();

#ifdef __cplusplus
}
#endif

#endif


