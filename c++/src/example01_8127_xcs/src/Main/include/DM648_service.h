#ifndef _DM648_INTERFACE_H
#define _DM648_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#define DM648_SUCCESS     (0)
#define DM648_FAILURE    (-1)

extern int shell_print(const char* format, ...);
extern int *g_DM648_debug_level;
#define DM648_DEBUG(level, format, args...) do{\
    if (g_DM648_debug_level != NULL)\
    {\
        if (level & *g_DM648_debug_level)\
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


/*

*/
int DM648Init(int (*func)());
int DM648Exit();
int DM648SetDbgLevel(int level);
char * DM648GetVersion();

#ifdef __cplusplus
}
#endif

#endif

