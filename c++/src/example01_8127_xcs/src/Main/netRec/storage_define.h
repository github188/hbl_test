

#ifndef __SRORAGE_DEFINE_H_
#define __SRORAGE_DEFINE_H_

extern int g_Storage_Debug;

#include "McLog.h"
#include <syslog.h>

//#define STORAGE_MODULE_DEBUG
#ifdef STORAGE_MODULE_DEBUG
#define STG_DEBUG(x...)  shell_print(x);
#else
    #define STG_DEBUG(x...)\
    do{\
        if(g_Storage_Debug)\
            shell_print(x);\
    }while(0);
#endif

//用于打印模块错误信息，可以用全局变量g_Storage_Debug单独控制
#ifdef STORAGE_MODULE_DEBUG
#define STG_ERROR(x...)  shell_print(x);
#else
#define STG_ERROR(x...)\
do{\
    if(g_Storage_Debug&0x0001)\
        shell_print(x);\
}while(0);
#endif

//用于打印模块警告信息，可以用全局变量g_Storage_Debug单独控制
#ifdef STORAGE_MODULE_DEBUG
#define STG_WARNING(x...)  shell_print(x);
#else
#define STG_WARNING(x...)\
do{\
    if(g_Storage_Debug&0x0002)\
        shell_print(x);\
}while(0);
#endif

//用于打印模块提示信息，可以用全局变量g_Storage_Debug单独控制
#ifdef STORAGE_MODULE_DEBUG
#define STG_INFO(x...)  shell_print(x);
#else
#define STG_INFO(x...)\
do{\
    if(g_Storage_Debug&0x0004)\
        shell_print(x);\
}while(0);
#endif

int StorageCfgInit(void);

#endif

