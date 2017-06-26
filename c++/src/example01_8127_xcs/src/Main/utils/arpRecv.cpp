#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <syslog.h>

#include "arpDep.h"

#include "SysConf.h"
#include "McLog.h"

extern int writeThreadInfo(char *pcThreadInfo);

#define ARPOP_REQUEST_          (0x0005)
#define ARPOP_REPLY_            (0x0006)

typedef enum _op_status
{
    OP_STATUS_NONE=0,
    OP_STATUS_DISCOVERY,
    OP_STATUS_READY,
    OP_STATUS_SETIP
}OP_STATUS_E;


static ARPRCV_MODULE            *g_ArpRcvModule = NULL;
static int                      reqfd = -1;
static SET_IP_FUNC              hook_set_ip_process = NULL;
static char                     arp_sha[ETH_ALEN];

static unsigned short g_sys_verno_val = 0;
static int get_ifi(char *dev, char * macAddr, int macLen, struct in_addr *lc_addr, int ipLen);

extern int xcsSysReboot(void);

extern char version_string[];
extern const unsigned int version_int;

static int setipcallback(INADDR *addr)
{
    NET_CONF_INFO strNetInfo;
    unsigned int uiSetIp =0;
    unsigned int uiTmpSetIp =0;

    uiTmpSetIp = addr->byte[0];
    uiTmpSetIp = uiTmpSetIp << 24;
    uiSetIp |= uiTmpSetIp;

    uiTmpSetIp = addr->byte[1];
    uiTmpSetIp = uiTmpSetIp << 16;
    uiSetIp |= uiTmpSetIp;

    uiTmpSetIp = addr->byte[2];
    uiTmpSetIp = uiTmpSetIp << 8;
    uiSetIp |= uiTmpSetIp;

    uiTmpSetIp = addr->byte[3];
    uiSetIp |= uiTmpSetIp;

    shell_print("byte0:%d,byte1:%d,byte2:%d,byte3:%d  netset ip:0x%x.\n",\
    addr->byte[0],addr->byte[1],addr->byte[2],addr->byte[3], uiSetIp);
    
    int ret = SysConfGet(e_NET_CONF, &strNetInfo, 0);
    if(ret == 0)
    {
        strNetInfo.uiIPAddr = uiSetIp;
        shell_print("netset ip:0x%x.\n",  strNetInfo.uiIPAddr);

        ret = SysConfSet(e_NET_CONF, &strNetInfo, 0);
        if( 0 > ret)
        {
            shell_print("netinfo set net Info Conf failed.\n");
            return -1;
        }
        else
        {
            xcsSysReboot();
        }
    }
    return 0;
}


static void* recvProcess(void *arg)
{
    int salen;
    int bRet = -1;
    char recv_buf[512];
    ARPPKT* rcvPkt =NULL;
    ARPRCV_MODULE* pArpRcv = (ARPRCV_MODULE *)arg;
    ARPPKT* arpPkt = &pArpRcv->arpPkt;
    struct sockaddr_ll reqsa, repsa;
    int sock = reqfd;
    char sha[ETH_ALEN];
    struct in_addr spa;
    unsigned int nRunTime = 0;

    struct timeval tvTimeOut;
    fd_set readFds;
    int nSelect = 0;
    int maxFdNum = 0;

    salen = sizeof(struct sockaddr_ll);
    bzero(&reqsa, salen);
    reqsa.sll_family = PF_PACKET;
    reqsa.sll_ifindex = if_nametoindex(ETH0);

    int op_status = OP_STATUS_NONE;

    writeThreadInfo("arpRecv");
    
    shell_print("%s %s %d pid = %d\n", __FILE__, __FUNCTION__, __LINE__, (int)pthread_self());
    while(1)
    {
        if(pArpRcv->bThreadQuit)
            break;
        FD_ZERO(&readFds);
        FD_SET(sock, &readFds);
        tvTimeOut.tv_sec = 5;
        tvTimeOut.tv_usec = 0;
        maxFdNum = sock + 1;
        nSelect = select(maxFdNum, (fd_set *)&readFds, 0, 0, &tvTimeOut);
        if(nSelect > 0)
        {
            if(!FD_ISSET(sock, &readFds))
                continue;
            memset(recv_buf, 0 , 512);
            get_ifi(ETH0, sha, ETH_ALEN, &spa, INLEN);
            if(recvfrom(sock, recv_buf, sizeof(ARPPKT), 0, (struct sockaddr *)&repsa, (socklen_t *)&salen) <= 0)
            {
                perror("Recvfrom error");
                exit(1);
            }

            if (nRunTime >= (60*1000))
            {
                shell_print("INFO: ARP recvProcess thread is running.\n");
                nRunTime = 0;
            }

            rcvPkt = (ARPPKT *)recv_buf;
            memset(arpPkt->padding, 0x00, sizeof(arpPkt->padding));
            memcpy(arpPkt->eh.ether_dhost, rcvPkt->ea.arp_sha, ETH_ALEN);
            memcpy(arpPkt->eh.ether_shost, sha, ETH_ALEN);
            memcpy(arpPkt->ea.arp_tha, rcvPkt->ea.arp_sha, ETH_ALEN);
            memcpy(arpPkt->ea.arp_tpa, rcvPkt->ea.arp_spa, INLEN);
            memcpy(arpPkt->ea.arp_sha, sha, ETH_ALEN);
            memcpy(arpPkt->ea.arp_spa, &spa, INLEN);

            if((ntohs(rcvPkt->ea.arp_op) == ARPOP_REQUEST_)
                && (rcvPkt->padding[1] == MSG_REQUEST))
            {
                usleep(20*1000);
                nRunTime += 20;
                switch ((unsigned char)rcvPkt->padding[0])
                {
                    case MSG_SNIFFER_DEV_IP:
                        arpPkt->ea.arp_op = htons(ARPOP_REPLY_);

	                    arpPkt->padding[0] = MSG_SNIFFER_DEV_IP;
	                    arpPkt->padding[1] = MSG_SNIFFER_DEV_IP_RESPONSE;
	                    
						memcpy(&arpPkt->padding[2], version_string, 10);
                        memcpy(&arpPkt->padding[12], &g_sys_verno_val, 2);
                        
                        
	                    if(sendto(sock, arpPkt, sizeof(ARPPKT), 0, (struct sockaddr *)&reqsa, sizeof(reqsa)) <= 0)
	                    {
	                        perror("Sendto error");
	                    }
	                    else
	                    {
	                        op_status = OP_STATUS_DISCOVERY;
	                    }
	                    break;
	                case MSG_SET_DEV_ID_READY:
	                    {
	                        arpPkt->ea.arp_op = htons(ARPOP_REPLY_);
	                        arpPkt->padding[0] = MSG_SET_DEV_ID_READY;
	                        arpPkt->padding[1] = MSG_SET_DEV_IP_READY_RESPONSE;
	                        arpPkt->padding[2] = SET_IP_SUCCESS;

                            if(sendto(sock, arpPkt, sizeof(ARPPKT), 0, (struct sockaddr *)&reqsa, sizeof(reqsa)) <= 0)
                            {
                                perror("Sendto error");
                            }
                            else
                            {
                                op_status = OP_STATUS_READY;
                            }

                            break;
                        }
                    case MSG_SET_DEV_IP:
                        {
                            if(OP_STATUS_READY != op_status)
                                break;


                            INADDR *ptr = (INADDR *)&rcvPkt->padding[2];

                            if(pArpRcv->set_ip_callback)
                            {
                                bRet = g_ArpRcvModule->set_ip_callback(ptr);
                            }
                            if(bRet ==0)
                            {
                                //in_addr_t inAddr = inet_addr(ptr->byte);
                                shell_print("%#x\n", *(unsigned int *)ptr->byte);
                                memcpy(arpPkt->ea.arp_spa, ptr->byte, INLEN);
                                arpPkt->padding[2] = SET_IP_SUCCESS;
                            }
                            else
                                arpPkt->padding[2] = SET_IP_FAIL;

                            arpPkt->ea.arp_op = htons(ARPOP_REPLY_);

                            arpPkt->padding[0] = MSG_SET_DEV_IP;
                            arpPkt->padding[1] = MSG_SET_DEV_IP_RESPONSE;

                            memcpy(arpPkt->ea.arp_spa, ptr->byte, INLEN);
                            if(sendto(sock, arpPkt, sizeof(ARPPKT), 0, (struct sockaddr *)&reqsa, sizeof(reqsa)) <= 0)
                            {
                                perror("Sendto error");
                            }
                            else
                            {
                                op_status = OP_STATUS_SETIP;
                            }

                            break;

                        }

                    default:
                        break;
                }
            }

        }
        else if(nSelect == 0)
        {
            nRunTime+=5*1000;
        }
        else
        {
        	syslog(LOG_ERR|LOG_USER, "arpRecv select error,killall xcs!\n");
        	system("killall -9 xcs");
            nRunTime+=20;
            usleep(20*1000);
        }
    }


    pthread_exit(0);
}



static int get_ifi(char *dev, char * macAddr, int macLen, struct in_addr *lc_addr, int ipLen)
{
    int sock;
    struct ifreq macreq;
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(macreq.ifr_name, dev);

    if(ioctl(sock, SIOCGIFHWADDR, &macreq) != 0)
        return 1;
    memcpy(macAddr, macreq.ifr_hwaddr.sa_data, macLen);

    if(ioctl(sock, SIOCGIFADDR, &macreq) != 0)
        return 1;
    memcpy(lc_addr, &((struct sockaddr_in *)(&macreq.ifr_addr))->sin_addr, ipLen);
    close(sock);

    return 0;
}


static int arpRcvInit(SET_IP_FUNC func)
{
    int nRet;
    ARPPKT *arpPkt;
    struct in_addr arp_spa;

    if(g_ArpRcvModule&&(g_ArpRcvModule->bInit))
        return 0;
    if(func == NULL)
        return 0;
	char *pVer = (char *)&version_int;
	g_sys_verno_val =  (pVer[3] << 12) | (pVer[2] << 8) | (pVer[1] << 4) |pVer[0];
	g_sys_verno_val = htons(g_sys_verno_val);
    g_ArpRcvModule = (ARPRCV_MODULE*)malloc(sizeof(ARPRCV_MODULE));
    memset(g_ArpRcvModule,0x00,sizeof(ARPRCV_MODULE));
    g_ArpRcvModule->set_ip_callback = func;


    if(!g_ArpRcvModule)
    {
        shell_print("%s:%d malloc() failed.\n",__FUNCTION__,__LINE__);
        goto error_ret;
    }
    arpPkt = &g_ArpRcvModule->arpPkt;
    if((reqfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0)
    {
        perror("Socket error");
        goto error_ret;
    }

    if(get_ifi(ETH0, arp_sha, ETH_ALEN, &arp_spa, INLEN))
    {
        fprintf(stderr, "Error: Get host¡¯s information failed\n");
        goto error_ret;
    }


    g_ArpRcvModule->bThreadQuit = 0;


    memcpy(arpPkt->eh.ether_dhost, MAC_BCAST_ADDR, ETH_ALEN);
    memcpy(arpPkt->eh.ether_shost, arp_sha, ETH_ALEN);
    arpPkt->eh.ether_type = htons(ETHERTYPE_ARP);

    arpPkt->ea.arp_hrd = htons(ARPHRD_ETHER);
    arpPkt->ea.arp_pro = htons(ETHERTYPE_IP);
    arpPkt->ea.arp_hln = ETH_ALEN;
    arpPkt->ea.arp_pln = INLEN;

    memcpy(arpPkt->ea.arp_sha, arp_sha, ETH_ALEN);
    memcpy(arpPkt->ea.arp_spa, &arp_spa, INLEN);

    nRet = pthread_create(&g_ArpRcvModule->threadArpRev, NULL, recvProcess , (void*)g_ArpRcvModule);
    if(0 != nRet)
    {
        shell_print("%s - %d: pthread_create failed;\n",  __FUNCTION__, __LINE__);
        goto error_ret;
    }


    g_ArpRcvModule->bInit = 1;

    return 0;
error_ret:
    if(g_ArpRcvModule)
    {
        g_ArpRcvModule->bThreadQuit = 1;
        if(g_ArpRcvModule->threadArpRev)
        {
            void *thread_result = NULL;
            pthread_join(g_ArpRcvModule->threadArpRev,&thread_result);
            g_ArpRcvModule->threadArpRev = 0;
        }
        if(reqfd > 0)
        {
            close(reqfd);
            reqfd = -1;
        }
        free(g_ArpRcvModule);
        g_ArpRcvModule = NULL;
        hook_set_ip_process  = NULL;

    }
    return -1;

}
static int arpRcvUninit()
{
    if(g_ArpRcvModule)
    {
        g_ArpRcvModule->bThreadQuit = 1;
        if(g_ArpRcvModule->threadArpRev)
        {
            void *thread_result = NULL;
            pthread_join(g_ArpRcvModule->threadArpRev,&thread_result);
            g_ArpRcvModule->threadArpRev = 0;
        }
        if(reqfd > 0)
        {
            close(reqfd);
            reqfd = -1;
        }
        free(g_ArpRcvModule);
        g_ArpRcvModule = NULL;
        hook_set_ip_process  = NULL;

    }
    return 0;
}


int arpFunInit(void)
{
    int ret =arpRcvInit(setipcallback);
    if(ret == -1)
        arpRcvUninit();
    return 0;
}

