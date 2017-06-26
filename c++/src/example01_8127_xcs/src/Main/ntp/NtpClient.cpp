#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>     /* gethostbyname */
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include "msgQLib.h"
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <syslog.h>

#include "NtpClient.h"
#include "xcsSysMsg.h"


struct ntptime {	
unsigned int coarse;	
unsigned int fine;
};
#define JAN_1970        (0x83aa7e80)     /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT        (123)
#define NTPFRAC(x)      ( 4294*(x) + ( (1981*(x))>>11 ) )
#define USEC(x)         ( ( (x) >> 12 ) - 759 * ( ( ( (x) >> 10 ) + 32768 ) >> 16 ) )

/* Converts NTP delay and dispersion, apparently in seconds scaled * by 65536, to microseconds. 
RFC-1305 states this time is in seconds, * doesn'
t mention the scaling. * Should somehow be the same as 1000000 * x / 65536 */
#define sec2u(x) ( (x) * 15.2587890625 )


static bool bServerChanged = false;

static void StuffNetAddr(struct in_addr *p, char *HostName)
{   
    struct hostent *ntpserver;  
    ntpserver=gethostbyname(HostName); 
    if (ntpserver == NULL) 
    {        
        herror(HostName);       
        return ;    
    }   
    if (ntpserver->h_length != 4) 
    {     /* IPv4 only, until I get a chance to test IPv6 */      
        fprintf(stderr,"oops %d\n",ntpserver->h_length);        
        return ; 
    }   
    memcpy(&(p->s_addr),ntpserver->h_addr_list[0],4);
}

static void NtpcGetTime(unsigned int *TimCoarse,unsigned int *TimeFine)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME,&now);
    *TimCoarse = now.tv_sec+JAN_1970;
    *TimeFine   = NTPFRAC(now.tv_nsec/1000);
    
}
static void GetPacketTimestamp(int usd, struct ntptime *udp_arrival_ntp)
{
    NtpcGetTime(&udp_arrival_ntp->coarse, &udp_arrival_ntp->fine);
}

#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4
#define PREC -6    

static void SendPacket(int usd, unsigned int  time_sent[2])
{   
    unsigned int  data[12];   
    int ret =0;
    
    if( sizeof(data) != 48 ) 
    {
        fprintf(stderr,"size error\n");     
        return; 
    }   
    memset(data, 0, sizeof(data)); 
    data[0] = htonl (       
            ( LI << 30 ) | ( VN << 27 ) | ( MODE << 24 ) |      
            ( STRATUM << 16) | ( POLL << 8 ) | ( PREC & 0xff ) );   
    data[1] = htonl(1<<16);  /* Root Delay (seconds) */ 
    data[2] = htonl(1<<16);  /* Root Dispersion (seconds) */    
    NtpcGetTime(time_sent, time_sent+1);   
    data[10] = htonl(time_sent[0]); /* Transmit Timestamp coarse */ 
    data[11] = htonl(time_sent[1]); /* Transmit Timestamp fine   */ 
    ret  = send(usd,data,48,0); 
}

static double NtpDiff( struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = stop->coarse - start->coarse;
	if (stop->fine >= start->fine)
	{
		b = stop->fine - start->fine;
	}
	else
	{
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}
	return a*1.e6 + b * (1.e6/4294967296.0);
}



CNtpClient::CNtpClient(MSG_Q_ID  MsgId)
{
  
    m_strNtpClient.bEnable         = 0;
    m_strNtpClient.uiNtpServerIp   = DEFAULT_NTP_SERVERIP;
    m_strNtpClient.usNtpPort       = DEFAULT_NTP_PORT;
    m_strNtpClient.uiInterval      = DEFAULT_NTP_INTERVAL;
    m_strNtpClient.uiTimeLag       = DEFAULT_NTP_TIMELAG;

    m_strSendMsgId  = MsgId;
    m_siDbgLevel    = DBG_ALL;
    
}
CNtpClient::~ CNtpClient()
{
    if (m_siSockFd>0)
    {
        close(m_siSockFd);
    }
}
bool CNtpClient::NtpSetupRecive()
{
    struct sockaddr_in sa_rcvr;

    memset(&sa_rcvr,0,sizeof(sa_rcvr));

    sa_rcvr.sin_family = AF_INET;
    sa_rcvr.sin_addr.s_addr =htonl(INADDR_ANY);
    sa_rcvr.sin_port=htons(m_strNtpClient.usLocalPort);
   
    if (bind (m_siSockFd,(struct  sockaddr *)&sa_rcvr,sizeof(sa_rcvr)) == -1)
    {
        return false;
    }
    return true;
}

bool CNtpClient::NtpSetupTransmit(void)
{
   struct sockaddr_in sa_dest;	
   char host[128];
   struct in_addr in;
   
   memset(&sa_dest,0,sizeof sa_dest);	
   sa_dest.sin_family=AF_INET;	
   in.s_addr = m_strNtpClient.uiNtpServerIp;
   in.s_addr = ntohl(in.s_addr);
   sprintf(host,"%s",inet_ntoa(in));
   NTP_DEBUG(DBG_INFO,"adjust time from server %s \n",host);
   
   StuffNetAddr(&(sa_dest.sin_addr),host);	
   sa_dest.sin_port=htons(m_strNtpClient.usNtpPort);	
   if (connect(m_siSockFd,(struct sockaddr *)&sa_dest,sizeof(sa_dest))==-1)		
   {
        return false;
   }
   return true;
}



#define Data(i) ntohl(((unsigned int *)data)[i])

/* Does more than print, so this name is bogus.
 * It also makes time adjustments, both sudden (-s)
 * and phase-locking (-l).
 * sets *error to the number of microseconds uncertainty in answer
 * returns 0 normally, 1 if the message fails sanity checks
 */
int CNtpClient::ParseData(unsigned int *data, struct ntptime *arrival, 
                              unsigned int *TimeOfSend)
{
/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double el_time,st_time,skew1,skew2;
	int freq = 0;

	li      = Data(0) >> 30 & 0x03;
	vn      = Data(0) >> 27 & 0x07;
	mode    = Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll    = Data(0) >>  8 & 0xff;
	prec    = Data(0)       & 0xff;
	if (prec & 0x80)
	    prec|=0xffffff00;
	delay   = Data(1);
	disp    = Data(2);
	refid   = Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);
	xmttime.fine   = Data(11);

	if (1)
	{
    	NTP_DEBUG(DBG_ALL,"LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d\n",
    		li, vn, mode, stratum, poll, prec);
    	NTP_DEBUG(DBG_ALL,"Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u\n",
    		sec2u(delay),sec2u(disp),
    		refid>>24&0xff, refid>>16&0xff, refid>>8&0xff, refid&0xff);
    	NTP_DEBUG(DBG_ALL,"Reference %u.%.6u\n", reftime.coarse,    USEC(reftime.fine));
    	NTP_DEBUG(DBG_ALL,"(sent)    %u.%.6u\n", TimeOfSend[0],    USEC(TimeOfSend[1]));
    	NTP_DEBUG(DBG_ALL,"Originate %u.%.6u\n", orgtime.coarse,    USEC(orgtime.fine));
    	NTP_DEBUG(DBG_ALL,"Receive   %u.%.6u\n", rectime.coarse,    USEC(rectime.fine));
    	NTP_DEBUG(DBG_ALL,"Transmit  %u.%.6u\n", xmttime.coarse,    USEC(xmttime.fine));
    	NTP_DEBUG(DBG_ALL,"Our recv  %u.%.6u\n", arrival->coarse,  USEC(arrival->fine));
	}
	el_time =   NtpDiff(&orgtime,arrival);   /* elapsed */
	st_time =   NtpDiff(&rectime,&xmttime);   /* stall */
	skew1   =   NtpDiff(&orgtime,&rectime);
	skew2   =   NtpDiff(&xmttime,arrival);
	if (1)
	{
    	NTP_DEBUG(DBG_ALL,"Total elapsed: %9.2f\n"
    	       "Server stall:  %9.2f\n"
    	       "Slop:          %9.2f\n",
    		el_time, st_time, el_time-st_time);
    	NTP_DEBUG(DBG_ALL,"Skew:          %9.2f\n"
    	       " day   second     elapsed    stall     skew  dispersion  freq\n",
    		    (skew1-skew2)/2);
	}

	
    NTP_DEBUG(DBG_ALL,"%d %.5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d\n",
	arrival->coarse/86400, arrival->coarse%86400,
	arrival->fine/4294967, el_time, st_time,
	(skew1-skew2)/2, sec2u(disp), freq);
 
    m_strSetTv.tv_sec = xmttime.coarse-JAN_1970;
	m_strSetTv.tv_usec = USEC(xmttime.fine);

    if ((abs(xmttime.coarse - arrival->coarse) >= m_strNtpClient.uiTimeLag)&&(m_strNtpClient.bEnable))
    {
        if ( NULL != m_strSendMsgId)
        {
            NTP_DEBUG(DBG_ALL,"send time to the main process thread\n");
            
            syslog(LOG_ERR|LOG_USER, "ntp client rcv LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d.\n",
    		                li, vn, mode, stratum, poll, prec);
    		                
            if( (1360115525 > m_strSetTv.tv_sec) || (2054339963 < m_strSetTv.tv_sec))
            {
                syslog(LOG_ERR|LOG_USER, "ntp sync time,secs is %d,secs is error,not sync timer!\n", (unsigned int)m_strSetTv.tv_sec);
                return 0;
            }

            syslog(LOG_ERR|LOG_USER, "ntp thread send sync time to main thread,secs is %d!\n", (unsigned int)m_strSetTv.tv_sec);
            STR_SYS_APP_MSG  strSysAppMsg;
            strSysAppMsg.uiModuleId = SYS_MSG_NTP_MODULE_ID;
            strSysAppMsg.uiMsgType  = MSG_NTP_TIME_REPORT;
            memcpy(strSysAppMsg.aucData,(void *)&m_strSetTv,sizeof(struct timeval));
            msgQSend(m_strSendMsgId,(char *)&strSysAppMsg,sizeof(STR_SYS_APP_MSG),0);
        }
    }
        
	return 0;
}

#define incoming ((char *)incoming_word)
#define sizeof_incoming ( sizeof(incoming_word) )

static unsigned int incoming_word[325];

void  CNtpClient::PrimaryLoop(int mode )
{
    static int s_iRunSta =0;
    fd_set fds;
    struct sockaddr sa_xmit;
    int i,pack_len,probes_sent;
    unsigned int sa_xmit_len =0;
    struct timeval to;
    struct ntptime udp_arrival_ntp;
    int wait = 0;

    if(m_siSockFd < 0)
    {
        NTP_DEBUG(DBG_ERR,"ntp Listening socket is error.\n");
        return;
    }

    /* 防止多处调用导致线程异常 */
    if(s_iRunSta > 0)
    {
        NTP_DEBUG(DBG_ERR,"ntp other run,return!\n");
        return;
    }
    else
    {
        s_iRunSta =1;
    }
    

    NTP_DEBUG(DBG_ALL,"ntp Listening.....\n");
    probes_sent = 0;
    sa_xmit_len = sizeof(sa_xmit);
    to.tv_sec = 2;
    to.tv_usec = 0;

    if ( 1 == mode )
    {
        wait = 0;
    }
    else
    {
        wait = 1;
    }

    while (1)
    {
    		
        if ( 0 == wait)
        {
            SendPacket(m_siSockFd,m_auiTimeOfSend);
            to.tv_sec = 2;
            to.tv_usec = 1000;
        }
        else
        {
            to.tv_sec = m_strNtpClient.uiInterval;
            to.tv_usec = 1000;
        }

        FD_ZERO(&fds);
        FD_SET(m_siSockFd, &fds);

        i = select (m_siSockFd+1,&fds,NULL,NULL,&to);
        if(i < 0)
        {
        	syslog(LOG_ERR|LOG_USER, "NepClinet select error,killall xcs!\n");
        	system("killall -9 xcs");
        }
        if ((i!=1)||(!FD_ISSET(m_siSockFd,&fds)))
        {
            if (i<0)
            {
                NTP_DEBUG(DBG_ALL,"ntp select ret is %d,break!\n",i);
                if (errno !=EINTR )
                {
                    perror("select");
                }
                break;
            }
            if (to.tv_sec == 0)
            {
                SendPacket(m_siSockFd,m_auiTimeOfSend);
                to.tv_sec = m_strNtpClient.uiInterval;
                to.tv_usec = 0;

                NTP_DEBUG(DBG_ALL,"ntp select timeout!\n");
            }

            if ( 1 == wait )
            {
                NTP_DEBUG(DBG_ALL,"ntp wait is 1,break!\n");
                break;
            }
        }

        if ( i > 0 )
        {
            NTP_DEBUG(DBG_ALL,"ntp rcv server pack!\n");
            pack_len=recvfrom(m_siSockFd,incoming,sizeof_incoming,0,&sa_xmit,&sa_xmit_len);     
        }
        else if ( 0 == wait )
        {
            NTP_DEBUG(DBG_ALL,"ntp wait is 0,break!\n");
            break;
        }

        if (pack_len < 0)
        {
            NTP_DEBUG(DBG_ERR,"received len err %d\n",pack_len);
        }
        else if( ((unsigned int)pack_len > 0) && (((unsigned int)pack_len < sizeof_incoming )&&( (unsigned int)pack_len >= 12*4 )) )
        {
            NTP_DEBUG(DBG_ALL,"ntp get server ok packet!\n");
            GetPacketTimestamp(m_siSockFd,&udp_arrival_ntp);
            ParseData(incoming_word, &udp_arrival_ntp, m_auiTimeOfSend);
        }
        if ( 0 == wait )
        {
            NTP_DEBUG(DBG_ALL,"ntp wait is 0,break2!\n");
            break;
        }
 	}
 	
    s_iRunSta =0;
}
bool CNtpClient::NtpClientInit(STR_NTP_CLIENT *pNtpClient)
{
    int nRet = 0;
    bool ret = true;
    
    if ( NULL != pNtpClient)
    {
        m_strNtpClient.bEnable         = pNtpClient->bEnable;
        m_strNtpClient.uiNtpServerIp   = pNtpClient->uiNtpServerIp;
        m_strNtpClient.usNtpPort       = pNtpClient->usNtpPort;
        m_strNtpClient.uiInterval      = pNtpClient->uiInterval;
        m_strNtpClient.uiTimeLag       = pNtpClient->uiTimeLag;
        m_strNtpClient.bForced         = pNtpClient->bForced;
    }
    else
    {
        return false;
    }

    if (m_strNtpClient.bEnable)
    {
        m_siSockFd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (m_siSockFd == -1)
        {
            NTP_DEBUG(DBG_ALL,"create socket failed\n");
            return false;
        }    
        ret = NtpSetupRecive();
        if(ret == false)
        {
            close(m_siSockFd);
            return false;
        }
       
        ret = NtpSetupTransmit();
        if(ret == false)
        {
            close(m_siSockFd);
            return false;
        }
    }
    
    nRet = pthread_create(&m_strNtpPid, 0, &pNtpThreadFunc, this);
    if ( nRet != 0 )
    {
        NTP_DEBUG(DBG_ERR,"Create pthread faile\n");
        if (m_siSockFd>0)
        {
            close(m_siSockFd);
        }   
        return false;
    }
    return ret ;
}

extern int writeThreadInfo(char *pcThreadInfo);

void *CNtpClient::pNtpThreadFunc(void *arg)
{
    CNtpClient  *pNtpClient = (CNtpClient *)arg;
    writeThreadInfo("ntpClient");
    while (1)
    {
        if (pNtpClient->m_strNtpClient.bEnable)
        {
            pNtpClient->PrimaryLoop(1);
            sleep(pNtpClient->m_strNtpClient.uiInterval);
            pNtpClient->DoSetServerIP(&(pNtpClient->m_strNtpClient), 0);
        }
        else
        {
            sleep(10);
        }
    }
    pthread_exit(0);
}

bool CNtpClient::DoSetServerIP(STR_NTP_CLIENT *pNtpClient, int iMode)
{
    bool ret = true;
    
    if(iMode > 0)
    {
        m_strNtpClient.bEnable         = pNtpClient->bEnable;
        m_strNtpClient.uiNtpServerIp   = pNtpClient->uiNtpServerIp;
        m_strNtpClient.usNtpPort       = pNtpClient->usNtpPort;
        m_strNtpClient.uiInterval      = pNtpClient->uiInterval;
        m_strNtpClient.uiTimeLag       = pNtpClient->uiTimeLag;
    }

    if (m_strNtpClient.bEnable)
    {
        if (m_siSockFd > 0)
        {
            close(m_siSockFd);
            m_siSockFd = -1;
        }

        m_siSockFd = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if (m_siSockFd == -1)
        {
            return false;
        }

        ret = NtpSetupRecive();
        if(ret == false)
        {
            close(m_siSockFd);
            return false;
        }
        
        ret = NtpSetupTransmit();
        if(ret == false)
        {
            close(m_siSockFd);
            return false;
        }
        PrimaryLoop(1);
    }
    else
    {
        if (m_siSockFd > 0)
        {
            close(m_siSockFd);
            m_siSockFd = -1;
        }

    }

    return ret ;
}
bool CNtpClient::NtpClientSetServer(STR_NTP_CLIENT * pNtpClient)
{
    if ( NULL == pNtpClient)
    {
        return false;
    }

    NTP_DEBUG(DBG_ALL,"NtpClientSetServer\n");

    bServerChanged = true; 
    return DoSetServerIP(pNtpClient, 1);

}
bool CNtpClient::NtpClientSwitchDbg(int level)
{
    m_siDbgLevel = level;
    return true;
}


