#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>	/*for socket(),bind(),close,etc*/
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/un.h>
#include <net/if_arp.h>
#include <net/route.h>
#include <netinet/in.h>	/*for sockaddr_in*/
#include <netinet/tcp.h>/*for TCP option*/
#include <arpa/inet.h>

#include "onvifSend.h"

#define ONVIF_LISTEN_NAME   "/tmp/loRecver"
#define ONVIF_LOCAL_NAME    "/tmp/clRecver"

static int s_args_fd        = -1;

static struct sockaddr_un s_args_addr = {0};

int init_onvifSend()
{
	if (-1 == s_args_fd)
	{
		s_args_fd = socket(AF_UNIX, SOCK_DGRAM, 0);         
		
		if (-1 != s_args_fd)
		{
			unlink(ONVIF_LOCAL_NAME);
			
			bzero(&s_args_addr , sizeof(s_args_addr));
			s_args_addr.sun_family      = AF_UNIX;        
			snprintf(s_args_addr.sun_path, sizeof(s_args_addr.sun_path), ONVIF_LOCAL_NAME);
			bind(s_args_fd, (struct sockaddr *)&s_args_addr, sizeof(s_args_addr));
			
			bzero(&s_args_addr , sizeof(s_args_addr));
			s_args_addr.sun_family      = AF_UNIX;
			snprintf(s_args_addr.sun_path, sizeof(s_args_addr.sun_path), ONVIF_LISTEN_NAME);
		}
	}
	
	return 0;
}


int uninit_onvifSend()
{
	if (s_args_fd != -1)
		close(s_args_fd);
	
	s_args_fd = -1;
    
	unlink(ONVIF_LOCAL_NAME);
	
	return 0;
}

/* args is binary-based */
int args_send(const char *buffer, unsigned int len)
{
	int ret = -1 ;
	
	int addrlen = sizeof(s_args_addr) ;
    
	if (-1 == s_args_fd)
		return -1;

	ret = sendto(s_args_fd , buffer, len, 0,
		(struct sockaddr *)&s_args_addr, addrlen);

	if(ret < 0)
	{
		close(s_args_fd);
		s_args_fd = -1;
		
		ret = -2;
		
		DSP_ERROR("onvif send args message failed, ret:%d, errno:%d,errorstr:%s\n", ret, errno, strerror(errno));
		init_onvifSend();
	}
	else if (ret != len)
	{
		ret = sendto(s_args_fd , buffer-ret, len-ret, 0, (struct sockaddr *)&s_args_addr, addrlen);
		DSP_ERROR("onvif send args len is too short, ret:%d,len:%d\n", ret, len);
	}
	return ret ;
}
#if 0
static int args_recv(char * buf, int len)
{
	int ret = -1;
	
	if (-1 == s_args_fd)
		return -1;
    
	// socket is blocking
	ret = recv(s_args_fd , buf, len, 0);
	
	//TODO:添加判断是否特定来源数据的代码
	return ret;
}
#endif
