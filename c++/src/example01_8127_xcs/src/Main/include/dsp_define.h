#ifndef DSP_DEFINE_H
#define DSP_DEFINE_H

#include "McLog.h"
extern int g_dsp_debug_module;


#define DSP_DEBUG(x...)\
do{\
	if(g_dsp_debug_module)\
		shell_print("dspApp mode:"x);\
}while(0)


#define DSP_ERROR(x...)\
do{\
	if(g_dsp_debug_module&0x0001)\
		shell_print("dspApp mode:"x);\
}while(0)

#define DSP_WARNING(x...)\
do{\
	if(g_dsp_debug_module&0x0002)\
		shell_print("dspApp mode:"x);\
}while(0)

#define DSP_INFO(x...)\
do{\
	if(g_dsp_debug_module&0x0004)\
		shell_print("dspApp mode:"x);\
}while(0)

#define XXX_INFO(x...)\
do{\
	shell_print("dspXXX mode:"x);\
}while(0)
#endif
