#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <rpc/pmap_prot.h>
#include <rpc/pmap_clnt.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include <ctype.h>
#include <syslog.h>
#include <fcntl.h>
#include "NetFsCheck.h"
#include "msgQLib.h"

#include "nfscheck.h"
#include "SysHandle.h"
#include "nMCtlApi.h"
#include "SysConf.h"
#include "McLog.h"


// assume only one nfs connection
static check_nfs_t nfs_check;
static STR_NETFS_INFO *g_strNfsInfo;
static STR_NETFS_STA   g_strNfsSta;

static pthread_t nfs_pid;
static int s_nfsChecking = 0;
static int s_quit= 0;

static int ChkNfsMount(const char *nfsip, const char* nfsdir)
{
    int flag = 0;
    FILE *fp;

    char buffer[512] = {0};
    char nfspath[512]= {0};

    memset(buffer, 0, sizeof(buffer));
    memset(nfspath, 0, sizeof(nfspath));
    

    snprintf(nfspath, sizeof(nfspath), "%s:%s", nfsip, nfsdir);

    fp = fopen("/proc/mounts", "r");
    if(fp == NULL)
    {
        syslog(LOG_ERR|LOG_USER, " ChkNfsMount open file failed\n");
        return -1;
    }
    for(;;)
    {
        fgets(buffer, sizeof(buffer), fp);
        if (feof(fp))
        {
            break;
        }

        if (0 == strncmp(buffer, nfspath, strlen(nfspath)))
        {
            flag = 1;
            break;
        }
    }

    fclose(fp);

    return (flag)? 0 : -1;
}


static int uMountNfs(void)
{
    int ret =0;
    ret = ChkNfsMount(nfs_check.strAddr, nfs_check.strPath);
    if (0 != ret)
    {
        shell_print("%s,%d,nfs not mount\n", __FUNCTION__, __LINE__);
        g_strNfsSta.uistatus = NETFS_STA_BAD;
    }
    else
    {
        shell_print("%s,%d,nfs mount, need to umount!\n", __FUNCTION__, __LINE__);
        ret = system("umount -f /nfs");
        ret >>= 8 ;
        if ( 0 == ret )
        {
            g_strNfsSta.uistatus       = NETFS_STA_INIT; 
            g_strNfsSta.uiNetFsType    = NETFS_NFS;

            syslog(LOG_ERR|LOG_USER, " NETFS service recover,umount ok\n");
            
        }
        else
        {
            g_strNfsSta.uistatus       = NETFS_STA_BAD; 
            g_strNfsSta.uiNetFsType    = NETFS_NFS;
            syslog(LOG_ERR|LOG_USER, " NETFS service recover,umount failed\n");
        }
    }
    return ret;
}


// 适合 hardlink mount
int MonNfsStat3(check_nfs_t  *ptNfsInfo)
{
    int ret;
    char nfsping[32]={0};

    memset(nfsping, 0, sizeof(nfsping));
    
    if (NFS_PING == ptNfsInfo->tStat)
    {
        snprintf(nfsping, sizeof(nfsping), "/bin/nfsping 60 %s", ptNfsInfo->strAddr);
        SYSHANDLE_INFO("nfs ready ping string is %s!\n", nfsping);
        ptNfsInfo->pipe = popen(nfsping, "r");
        if(ptNfsInfo->pipe == NULL)
        {
            shell_print("nfs ping, popen open failed,reboot sys\n");
            syslog(LOG_ERR|LOG_USER, "nfs ping, popen open failed,reboot sys!\n");
            g_strNfsSta.uistatus = NETFS_OK2BAD;
            g_strNfsSta.uiNetFsType    = NETFS_NFS;
            SendMsgToApp(g_strNfsInfo->strMsgId,&g_strNfsSta);
            g_strNfsSta.uistatus    = NETFS_STA_BAD;
			ptNfsInfo->tStat = NFS_PING;
            s_nfsChecking = 0;
            s_quit =1;
        }
        else
        {
            ptNfsInfo->tStat = NFS_WAIT_PING;
            ptNfsInfo->iFailCnt = 2;
        }
    }
    else if (NFS_WAIT_PING == ptNfsInfo->tStat)
    {
        if (ptNfsInfo->iFailCnt <= 0)
        {
            ptNfsInfo->tStat = NFS_CHK_PING;
            ptNfsInfo->iFailCnt = 0;
        }
        else
        {
        		ptNfsInfo->iFailCnt--;        
       	}
    }
    else if (NFS_CHK_PING == ptNfsInfo->tStat)
    {        
        ret = fread(nfsping, 1, sizeof(nfsping), ptNfsInfo->pipe);
        pclose(ptNfsInfo->pipe);
        SYSHANDLE_INFO("nfs chk ping string is %s,ret is %d!\n", nfsping, ret);
        if (ret >= 7 && 0 == strncmp(nfsping, "SUCCESS", 7))
        {
            ret = ChkNfsMount(ptNfsInfo->strAddr, ptNfsInfo->strPath);
            if (0 != ret)
            {   
	            ret = system("/bin/mount_nas.sh");
               	ptNfsInfo->tStat = NFS_CHK_MOUNT;
	        }
	        else
	        {
	           	if (( NETFS_STA_BAD == g_strNfsSta.uistatus) || (NETFS_STA_INIT == g_strNfsSta.uistatus))
                {
    	            g_strNfsSta.uistatus = NETFS_STA_OK;
                    g_strNfsSta.uiNetFsType = NETFS_NFS;
                    shell_print("nfs ping status org is ok!\n");
                    SendMsgToApp(g_strNfsInfo->strMsgId,&g_strNfsSta);
                }
                ptNfsInfo->tStat = NFS_PING;
                s_nfsChecking = 0;
            }
        }
        else            
        {
            ptNfsInfo->tStat = NFS_PING;
            if (NETFS_STA_OK ==g_strNfsSta.uistatus)//当状态由好变坏时
            {
                g_strNfsSta.uistatus = NETFS_OK2BAD;
                g_strNfsSta.uiNetFsType = NETFS_NFS;
                shell_print("nfs ping status changed to BAD,reboot system\n");
                syslog(LOG_ERR|LOG_USER, "nfs ping failed,lose nfs service,reboot system\n");
                SendMsgToApp(g_strNfsInfo->strMsgId,&g_strNfsSta);
                g_strNfsSta.uistatus = NETFS_STA_BAD;
                s_quit =1;
            }
            else if( NETFS_STA_INIT ==g_strNfsSta.uistatus )
            {
                g_strNfsSta.uistatus = NETFS_STA_INIT;
            }
            else
            {
                g_strNfsSta.uistatus = NETFS_STA_BAD;
            }
         }            
    }
    else if (NFS_CHK_MOUNT == ptNfsInfo->tStat)
    {
        ret = ChkNfsMount(ptNfsInfo->strAddr, ptNfsInfo->strPath);
        if (0 != ret)
        {
            ptNfsInfo->tStat = NFS_PING;
            ptNfsInfo->iFailCnt = 10; 
            if ( NETFS_STA_OK ==g_strNfsSta.uistatus )//当状态由好变坏时
            {
                shell_print("nfs mount status changed to BAD\n");
                syslog(LOG_ERR|LOG_USER, "nfs mount failed,lose nfs service,reboot system!\n");
                g_strNfsSta.uistatus = NETFS_OK2BAD;
                g_strNfsSta.uiNetFsType    = NETFS_NFS;
                SendMsgToApp(g_strNfsInfo->strMsgId,&g_strNfsSta);
                g_strNfsSta.uistatus    = NETFS_STA_BAD;
                s_quit =1;
            }
            else if( NETFS_STA_INIT == g_strNfsSta.uistatus )
            {
                g_strNfsSta.uistatus = NETFS_STA_INIT;
            }
            else
            {
                g_strNfsSta.uistatus = NETFS_STA_BAD;
            }
        }
        else
        {
            ptNfsInfo->tStat = NFS_PING;
            ptNfsInfo->iFailCnt = 10;  
            if ((NETFS_STA_BAD == g_strNfsSta.uistatus) || (NETFS_STA_INIT == g_strNfsSta.uistatus))
            {
                shell_print("nfs mount sta is OK!\n");
                g_strNfsSta.uistatus       = NETFS_BAD2OK;
                g_strNfsSta.uiNetFsType    = NETFS_NFS;
                syslog(LOG_ERR|LOG_USER, " NFS service is mount ok!\n");
                SendMsgToApp(g_strNfsInfo->strMsgId, &g_strNfsSta);
                g_strNfsSta.uistatus = NETFS_STA_OK;
            }
            else
            {
                g_strNfsSta.uistatus = NETFS_STA_OK;
            }
            s_nfsChecking = 0;
        }
    }
    return 0;
}
static int NfsHandle(void *param)
{
    s_nfsChecking = 1 ;
	MonNfsStat3(&nfs_check);
    if (NFS_CHK_OFF != nfs_check.tStat)
        ;
    else
        syslog(LOG_ERR|LOG_USER, "nfs keep alive turned off\n");
    return 0;
}

int InitNfsCheck(char *szIpAddr,char *szPath)
{
	int ret;
    int iSta[MAX_VEHICLE_NUM] ={0};
	NET_HDD_CONF_INFO strNetHddConf;
	
    memset(iSta, 0, sizeof(iSta));
    memset(&strNetHddConf, 0, sizeof(strNetHddConf));
	ret = SysConfGet(e_REC_NET_HDD_CONF,&strNetHddConf,0);
    if ( ret < 0)
    {
        return -1;
    }
    
	if ( 1 == strNetHddConf.uiAvaiable)
	{
	    iSta[0] = MODE_OK_STA;
		nMMsgSend(NM_RECORD_NAS_STA, 0, 0, iSta);
	}
	else
	{
	    iSta[0] = MODE_SHUTDOWN_STA;
		nMMsgSend(NM_RECORD_NAS_STA, 0, 0, iSta);
	}

	if (g_strNfsInfo->bEnable)
	{
	    nfs_check.strAddr = (const char *)szIpAddr;
		nfs_check.strPath = (const char *)szPath;
        nfs_check.tStat = NFS_PING;
        nfs_check.iFailCnt = 0;
	}
    g_strNfsSta.uistatus = NETFS_STA_INIT;
	return 0;
}

static void *pNfsStaCheck(void *arg)
{
    int i = 0;
    writeThreadInfo("nfsCheck");
    while(1)
    {
        if ( 1 == s_quit)
        {
            syslog(LOG_ERR|LOG_USER, "nfs check failed, lease thread!\n");
            break;
        }
        /* 先检测再进入时间延迟 */
        if (g_strNfsInfo->bEnable)
        {
            NfsHandle(NULL);
        }
        if ( 0 == s_nfsChecking)
        {
            for ( i = 0;i<g_strNfsInfo->iCheckInteval;i++)
            {
                if ( 1 == s_quit)
                    break;
                sleep(1);
            }
        }
        else
        {
            sleep(1);
        }
    }
    uMountNfs();
    free(g_strNfsInfo);
    g_strNfsInfo = NULL;
    s_quit =0;
    pthread_exit(0);
}

int NfsStaCheckInit(STR_NETFS_INFO *pNetFsInfo)
{
    int ret = 0;

    if ( NULL == g_strNfsInfo)
    {
        g_strNfsInfo = (STR_NETFS_INFO *)malloc(sizeof(STR_NETFS_INFO));
        if ( NULL == g_strNfsInfo )
        {
            return -1;
        }
    }
    if ( NULL == pNetFsInfo)
    {        
        free(g_strNfsInfo);
        g_strNfsInfo = NULL;
        return -1;
    }
    else
    {
        strncpy(g_strNfsInfo->szIpAddr, pNetFsInfo->szIpAddr,sizeof(g_strNfsInfo->szIpAddr));
        strncpy(g_strNfsInfo->szRootPath, pNetFsInfo->szRootPath,sizeof(g_strNfsInfo->szRootPath));
        strncpy(g_strNfsInfo->szMountPath, pNetFsInfo->szMountPath,sizeof(g_strNfsInfo->szMountPath));
        g_strNfsInfo->bEnable       =  pNetFsInfo->bEnable;
        g_strNfsInfo->iCheckInteval = (pNetFsInfo->iCheckInteval<1) ? 1:pNetFsInfo->iCheckInteval;
        g_strNfsInfo->strMsgId  = pNetFsInfo->strMsgId;
    }
    InitNfsCheck(g_strNfsInfo->szIpAddr,g_strNfsInfo->szRootPath);
    
    ret = pthread_create(&nfs_pid,0,&pNfsStaCheck,NULL);
    return 0;
    
}
int NfsStaCheckUninit(void)
{
    s_quit = 1;
    if(nfs_pid)
    {
        pthread_join(nfs_pid, NULL);
    }
    return 0;
}


