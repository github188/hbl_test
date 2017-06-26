#ifndef ONVIF_SEND_H
#define ONVIF_SEND_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "dsp_define.h"

typedef struct _tag_StreamData{
	int nChannel;
	unsigned int offset;
	int nLen;
}STREAMDATA;

int init_onvifSend();
int uninit_onvifSend();

int args_send(const char *buffer, unsigned int len);

#ifdef __cplusplus
};
#endif

#endif

