/******************************************************************************
 * netDctl.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  ���ڽ�ͨ�Ƽ�����ӿں���ͷ�ļ�
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
#define NET_TYPE_MULTICAST          0           // �鲥
#define NET_TYPE_UDP                1           // UDPֱ��
#define NET_TYPE_TCP                2           // TCPֱ��
#define NET_TYPE_TCP_PROXY          3           // TCP����
#else //����DVSЭ�� Ӧ�������¶���
#define NET_TYPE_DEFAULT            0           // DVSĬ��
#define NET_TYPE_UDP                1           // UDPֱ��
#define NET_TYPE_MULTICAST          2           // UDP�鲥
#define NET_TYPE_TCP                3           // TCP
#define NET_TYPE_TCP_PROXY          4           // TCP����(����д4  DVSЭ����û�ж�Ӧ)
#endif

int initNetSocket(char * szRemoteIP, unsigned short nRemotePort, unsigned short nLocalPort, int nNetType);

int _Net_Init();
int _Net_Uninit();

int netSocketDebugSwitch(int nLevel);
int netDctlDebugSwitch(int nLevel);

#endif /* _DEV_NET_DCTL_H */
