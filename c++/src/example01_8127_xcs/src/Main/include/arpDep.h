#ifndef __ARP_DEP_H__
#define __ARP_DEP_H__
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/types.h>
#include <asm/types.h>
#include <netdb.h>
#include <features.h> /* 需要里面的 glibc 版本号 */
#if __GLIBC__ >= 2 && __GLIBC_MINOR >= 1
    #include <netpacket/packet.h>
    #include <net/ethernet.h> /* 链路层（L2）协议 */
#else
    #include <asm/types.h>
    #include <linux/if_packet.h>
    #include <linux/if_ether.h> /* 链路层协议 */
#endif
#include <netinet/if_ether.h>
#include <arpa/inet.h>



#define ETH0			"eth0"

#define MAC_BCAST_ADDR  (char *) "\xff\xff\xff\xff\xff\xff"//255.255.255.255

#define INLEN 4 //ipv4

#define MSG_REQUEST 					0x55
#define MSG_RESPONSE					0x56

#define MSG_SNIFFER_DEV_IP				0xAA/*获取设备ip */
#define MSG_SNIFFER_DEV_IP_REQUEST				MSG_REQUEST
#define MSG_SNIFFER_DEV_IP_RESPONSE				MSG_RESPONSE

#define MSG_SET_DEV_IP				0xCC/*设置设备ip*/
#define MSG_SET_DEV_IP_REQUEST		MSG_REQUEST
#define MSG_SET_DEV_IP_RESPONSE			MSG_RESPONSE

#define MSG_SET_DEV_ID_READY	0xBB
#define MSG_SET_DEV_IP_READY_REQUEST		MSG_REQUEST
#define MSG_SET_DEV_IP_READY_RESPONSE			MSG_RESPONSE


#define SET_IP_SUCCESS	0x7F
#define SET_IP_FAIL		0xFF

#pragma   pack(push,   1)
typedef struct _tag_ARPPKT {
        struct ether_header eh;
        struct ether_arp ea;
        char padding[18];
} ARPPKT;
#pragma   pack(pop)


typedef struct _tag_INADDR
{
	unsigned char byte[4];

}INADDR;
typedef int (*SET_IP_FUNC)( INADDR *addr);

typedef struct _tag_ArpRcvModule
{
	int         bInit;
	int bThreadQuit;
	pthread_t threadArpRev;

	ARPPKT arpPkt;
	SET_IP_FUNC set_ip_callback;

}ARPRCV_MODULE;

#define bzero(s,n) (memset((s), '\0', (n)), (void) 0)

#ifdef __cplusplus
extern "C"
{
#endif

int arpFunInit(void);

#ifdef __cplusplus
};
#endif

#endif
