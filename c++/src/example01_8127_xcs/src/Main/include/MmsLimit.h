#ifndef __MMSSEND_LIMIT_H
#define __MMSSEND_LIMIT_H

#ifdef __cplusplus
extern "C"{
#endif


extern int g_MMSSEND_AUDIO_CHANNEL ;			//��ʾ��Ƶͨ���ţ��ú�=g_MMSSEND_MAX_CHANNEL_NUM
extern int g_MMSSEND_MAX_CHANNEL_NUM ;			//��ʾͨ����(��Ƶͨ����+1·��Ƶ�Խ�)��ͨ����=g_MMSSEND_MAX_CHANNEL_NUM+1
extern int g_MMSSEND_BANDWIDTH ;			//sendbox ����


int IS_P_MMS_CHANNEL_VALID(int x);



#ifdef __cplusplus
}
#endif

#endif

