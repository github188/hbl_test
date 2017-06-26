#ifndef __MMSSEND_LIMIT_H
#define __MMSSEND_LIMIT_H

#ifdef __cplusplus
extern "C"{
#endif


extern int g_MMSSEND_AUDIO_CHANNEL ;			//表示视频通道号，该号=g_MMSSEND_MAX_CHANNEL_NUM
extern int g_MMSSEND_MAX_CHANNEL_NUM ;			//表示通道数(视频通道数+1路音频对讲)，通道数=g_MMSSEND_MAX_CHANNEL_NUM+1
extern int g_MMSSEND_BANDWIDTH ;			//sendbox 带宽


int IS_P_MMS_CHANNEL_VALID(int x);



#ifdef __cplusplus
}
#endif

#endif

