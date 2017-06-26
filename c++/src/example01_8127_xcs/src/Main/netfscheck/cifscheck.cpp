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

//#include "work_queue.h"
#include "NetFsCheck.h"

#include "cifscheck.h"
#include "xcsSysMsg.h"
#include "SysHandle.h"
#include "SysConf.h"
#include "nMCtlApi.h"
#include "McLog.h"
//#include "fw_env.h"
//#include "epCore.h"
//#include "env.h"

#include "xcsGlobalDef.h"

static check_cifs_t cifs_check;

static STR_NETFS_INFO  *g_strCifsInfo = NULL;
static STR_NETFS_STA   g_strCifsSta;
static int s_CifsChecking = 0;

static pthread_t cifs_pid;
static int g_quit= 0;

static int ChkCifsMount(const char *cifsip, const char* cifsdir)
{
    int flag = 0;
    FILE *fp =NULL;

    char buffer[512] = {0};
    char cifspath[512] = {0};

    memset(buffer, 0, sizeof(buffer));
    memset(cifspath, 0, sizeof(cifspath));

    snprintf(cifspath, sizeof(cifspath), "//%s%s", cifsip, cifsdir);
    fp = fopen("/proc/mounts", "r");
    if(fp == NULL)
    {
        syslog(LOG_ERR|LOG_USER, " ChkCifsMount open file failed\n");
        return -1;
    }
    for(;;)
    {
        fgets(buffer, sizeof(buffer), fp);
        if (feof(fp))
        {
            break;
        }

        if (0 == strncmp(buffer, cifspath, strlen(cifspath)))
        {
            flag = 1;
            break;
        }
    }

    fclose(fp);
    return (flag)? 0 : -1;
}

static int uMountCifs(void)
{
    int ret =0;
    ret = ChkCifsMount(cifs_check.strAddr, cifs_check.strPath);
    if (0 != ret)
    {
        g_strCifsSta.uistatus = NETFS_STA_BAD;
    }
    else
    {
        ret = system("umount -f /cifs");
        ret >>= 8 ;
        if ( 0 == ret )
        {
            g_strCifsSta.uistatus       = NETFS_STA_INIT; 
            g_strCifsSta.uiNetFsType    = NETFS_CIFS;

            syslog(LOG_ERR|LOG_USER, " CIFS service recover,umount ok\n");
        }
        else
        {
            g_strCifsSta.uistatus       = NETFS_STA_BAD; 
            g_strCifsSta.uiNetFsType    = NETFS_CIFS;
            syslog(LOG_ERR|LOG_USER, " CIFS service recover,umount failed\n");
        }
    }
    return ret;
}

// 适合 hardlink mount

int SendMsgToApp(MSG_Q_ID  strMsgId,STR_NETFS_STA *pStrNetFsSta)
{
    STR_SYS_APP_MSG strSysAppMsg;
     
    strSysAppMsg.uiModuleId = SYS_MSG_NETFS_MODULE_ID;
    strSysAppMsg.ucCarRoadId =0;
    strSysAppMsg.uiMsgType  = NETFS_STA_CHANGE;
    memcpy(strSysAppMsg.aucData,pStrNetFsSta,sizeof(STR_NETFS_STA));
    
    
    if ( NULL != strMsgId )
    {
        msgQSend(strMsgId, (char *)&strSysAppMsg, sizeof(strSysAppMsg), 0);
    }
    return 0;
    
}
int MonCifsStat3(check_cifs_t  *ptCifsInfo)
{
    int ret;
    char cifsping[32]={0};
    memset(cifsping, 0, sizeof(cifsping));


    if (CIFS_PING == ptCifsInfo->tStat)
    {
        snprintf(cifsping, sizeof(cifsping), "/bin/cifsping 10 %s", ptCifsInfo->strAddr);
        SYSHANDLE_INFO("cifs ready ping string is %s!\n", cifsping);
      	ptCifsInfo->pipe = popen(cifsping, "r");
      	if(ptCifsInfo->pipe == NULL)
      	{
            shell_print("cifs popen failed,reboot sys!\n");
            syslog(LOG_ERR|LOG_USER, "cifs popen failed,reboot system!\n");
            g_strCifsSta.uistatus = NETFS_OK2BAD;
            g_strCifsSta.uiNetFsType    = NETFS_CIFS;
            SendMsgToApp(g_strCifsInfo->strMsgId,&g_strCifsSta);
            g_strCifsSta.uistatus = NETFS_STA_BAD;
            ptCifsInfo->tStat = CIFS_PING;
            s_CifsChecking = 0;
            g_quit =1;
      	}
      	else
      	{
            ptCifsInfo->tStat = CIFS_WAIT_PING;
            ptCifsInfo->iFailCnt = 2;
        }
    }
    else if (CIFS_WAIT_PING == ptCifsInfo->tStat)
    {
        if (ptCifsInfo->iFailCnt <= 0)
        {
            ptCifsInfo->tStat = CIFS_CHK_PING;
            ptCifsInfo->iFailCnt = 0;
        }
        else
        {
            ptCifsInfo->iFailCnt--;
        }
    }
    else if (CIFS_CHK_PING == ptCifsInfo->tStat)
    {   
        ret = fread(cifsping,1,sizeof(cifsping),ptCifsInfo->pipe);
        pclose(ptCifsInfo->pipe);
        SYSHANDLE_INFO("cifs chk ping string is %s,ret is %d!\n", cifsping, ret);
        if (ret >= 7 && 0 == strncmp(cifsping, "SUCCESS", 7))
		{
            ret = ChkCifsMount(ptCifsInfo->strAddr, ptCifsInfo->strPath);
            if (0 != ret)
            {
                ret = system("/bin/mount_cifs.sh");
                ptCifsInfo->tStat = CIFS_CHK_MOUNT;
            }
            else
            {
                if ((NETFS_STA_BAD == g_strCifsSta.uistatus) || (NETFS_STA_INIT == g_strCifsSta.uistatus))
                {
                    shell_print("cifs mount org status is ok\n");
                    g_strCifsSta.uistatus = NETFS_STA_OK;
                    g_strCifsSta.uiNetFsType    = NETFS_CIFS;
                    SendMsgToApp(g_strCifsInfo->strMsgId,&g_strCifsSta);
                }
                ptCifsInfo->tStat = CIFS_PING;
                s_CifsChecking = 0;
            }
	    }
        else
        {
            ptCifsInfo->tStat = CIFS_PING;
            
            if ( NETFS_STA_OK ==g_strCifsSta.uistatus )//当状态由好变坏时
            {
                shell_print("cifs ping status changed to BAD\n");
                syslog(LOG_ERR|LOG_USER, "cifs ping failed,lose CIFS service,reboot system!\n");
                g_strCifsSta.uistatus = NETFS_OK2BAD;
                g_strCifsSta.uiNetFsType    = NETFS_CIFS;
                SendMsgToApp(g_strCifsInfo->strMsgId,&g_strCifsSta);
                g_strCifsSta.uistatus = NETFS_STA_BAD;
                g_quit =1;
            }
            else if( NETFS_STA_INIT ==g_strCifsSta.uistatus )
            {
                g_strCifsSta.uistatus = NETFS_STA_INIT;
            }
            else
            {
                g_strCifsSta.uistatus = NETFS_STA_BAD;
            }
         }  
    }
    else if (CIFS_CHK_MOUNT == ptCifsInfo->tStat)
    {
        ret = ChkCifsMount(ptCifsInfo->strAddr, ptCifsInfo->strPath);
        if (0 != ret)
        {
            ptCifsInfo->tStat = CIFS_PING;
            ptCifsInfo->iFailCnt = 10;  

            if ( NETFS_STA_OK ==g_strCifsSta.uistatus )//当状态由好变坏时
            {
                shell_print("cifs mount status changed to BAD\n");
                syslog(LOG_ERR|LOG_USER, "cifs mount failed,lose CIFS service,reboot system!\n");
                g_strCifsSta.uistatus = NETFS_OK2BAD;
                g_strCifsSta.uiNetFsType    = NETFS_CIFS;
                SendMsgToApp(g_strCifsInfo->strMsgId,&g_strCifsSta);
                g_strCifsSta.uistatus = NETFS_STA_BAD;
                g_quit =1;
            }
            else if( NETFS_STA_INIT ==g_strCifsSta.uistatus )
            {
                g_strCifsSta.uistatus = NETFS_STA_INIT;
            }
            else
            {
                g_strCifsSta.uistatus = NETFS_STA_BAD;
            }
        }
        else
        {
            ptCifsInfo->tStat = CIFS_PING;
            ptCifsInfo->iFailCnt = 10;  
            if ((NETFS_STA_BAD == g_strCifsSta.uistatus) || (NETFS_STA_INIT == g_strCifsSta.uistatus))
            {
                shell_print("cifs mount sta is OK!\n");
                g_strCifsSta.uistatus       = NETFS_BAD2OK;
                g_strCifsSta.uiNetFsType    = NETFS_CIFS;
                syslog(LOG_ERR|LOG_USER, " CIFS service is mount ok!\n");
                SendMsgToApp(g_strCifsInfo->strMsgId, &g_strCifsSta);
                g_strCifsSta.uistatus       = NETFS_STA_OK;
            }
            else
            {
                g_strCifsSta.uistatus = NETFS_STA_OK;
            }
            s_CifsChecking = 0;
        }
    }
    
    return 0;
}

static int CifsHandle(void *param)
{
    s_CifsChecking = 1 ;
	MonCifsStat3(&cifs_check);
    if (CIFS_CHK_OFF != cifs_check.tStat)
        ;
    else
        syslog(LOG_ERR|LOG_USER, "cifs keep alive turned off\n");
    return 0;
}

static int InitCifsCheck(char *szIpAddr,char *szPath)
{
	int ret;
	NET_HDD_CONF_INFO strNetHddConf;
    int iSta[MAX_VEHICLE_NUM] ={0};

    memset(iSta, 0, sizeof(iSta));

    memset(&strNetHddConf, 0, sizeof(strNetHddConf));
	ret = SysConfGet(e_PIC_NET_HDD_CONF,&strNetHddConf,0);
    if ( ret < 0)
    {
        return -1;
    }
    
	if ( 1 == strNetHddConf.uiAvaiable)
	{   
	    iSta[0] = MODE_OK_STA;
		nMMsgSend(NM_CIFS_NAS_STA, 0, 0, iSta);
	}
	else
	{
	    iSta[0] = MODE_SHUTDOWN_STA;
		nMMsgSend(NM_CIFS_NAS_STA, 0, 0, iSta);
	}

	if (g_strCifsInfo->bEnable)
	{
	    cifs_check.strAddr = (const char *)szIpAddr;
	    cifs_check.strPath = (const char *)szPath;
        cifs_check.tStat = CIFS_PING;
        cifs_check.iFailCnt = 0;
    }
    g_strCifsSta.uistatus = NETFS_STA_INIT;
	return 0;
}


static void *pCifsStaCheck(void *arg)
{
    int i=0;
    writeThreadInfo("cifsCheck");
    while(1)
    {
        if ( 1 == g_quit)
        {
            syslog(LOG_ERR|LOG_USER, "cifs is failed,quit check!\n");
            break;
        }
        /* 先检测再进入时间延迟 */
        if (g_strCifsInfo->bEnable)
        {
            CifsHandle(NULL);
        }

        if ( 0 == s_CifsChecking)
        {
            for (i = 0;i<g_strCifsInfo->iCheckInteval;i++)
            {
                if ( 1 == g_quit)
                    break;
                sleep(1);
            }
        }
        else
        {
            sleep(1);
        }
    }
    uMountCifs();
    free(g_strCifsInfo);
    g_strCifsInfo = NULL;
    g_quit = 0;
    pthread_exit(0);
    
}

int CifsStaCheckInit(STR_NETFS_INFO  *pNetFsInfo)
{
    int ret = 0;

    if ( NULL ==g_strCifsInfo)
    {
        g_strCifsInfo = (STR_NETFS_INFO *)malloc(sizeof(STR_NETFS_INFO));
        if ( NULL == g_strCifsInfo)
        {
            return -1;
        }
    }
    if ( NULL == pNetFsInfo)
    {
        free(g_strCifsInfo);
        g_strCifsInfo = NULL;
        return -1;
    }
    else
    {
        strncpy(g_strCifsInfo->szIpAddr, pNetFsInfo->szIpAddr,sizeof(g_strCifsInfo->szIpAddr));
        strncpy(g_strCifsInfo->szRootPath, pNetFsInfo->szRootPath,sizeof(g_strCifsInfo->szRootPath));
        strncpy(g_strCifsInfo->szMountPath, pNetFsInfo->szMountPath,sizeof(g_strCifsInfo->szMountPath));
        g_strCifsInfo->bEnable      = pNetFsInfo->bEnable;
        g_strCifsInfo->iCheckInteval= (pNetFsInfo->iCheckInteval <1)?1:pNetFsInfo->iCheckInteval;
        g_strCifsInfo->strMsgId = pNetFsInfo->strMsgId;   
    }
    InitCifsCheck(g_strCifsInfo->szIpAddr,g_strCifsInfo->szRootPath);    

    ret = pthread_create(&cifs_pid,0,&pCifsStaCheck,NULL);
    return ret;
    
}
int CifsStaCheckUninit(void)
{
    g_quit = 1;
    if(cifs_pid)
    {
        pthread_join(cifs_pid, NULL);
    }
    
    return 0;
}


