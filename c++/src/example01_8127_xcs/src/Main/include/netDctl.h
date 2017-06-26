/******************************************************************************
 * netDctl.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  网口交通灯检测器接口函数头文件
 *     
 * modification history
 * --------------------
 * 01a, 2012.08.29, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_NET_DCTL_H
#define _DEV_NET_DCTL_H

//net transport type
#if 0
#define NET_TYPE_MULTICAST          0           // 组播
#define NET_TYPE_UDP                1           // UDP直连
#define NET_TYPE_TCP                2           // TCP直连
#define NET_TYPE_TCP_PROXY          3           // TCP代理
#else //根据DVS协议 应采用如下定义
#define NET_TYPE_DEFAULT            0           // DVS默认
#define NET_TYPE_UDP                1           // UDP直连
#define NET_TYPE_MULTICAST          2           // UDP组播
#define NET_TYPE_TCP                3           // TCP
#define NET_TYPE_TCP_PROXY          4           // TCP代理(暂且写4  DVS协议中没有对应)
#endif

int initNetSocket(char * szRemoteIP, unsigned short nRemotePort, unsigned short nLocalPort, int nNetType);

int _Net_Init();
int _Net_Uninit();

int netSocketDebugSwitch(int nLevel);
int netDctlDebugSwitch(int nLevel);

#endif /* _DEV_NET_DCTL_H */
