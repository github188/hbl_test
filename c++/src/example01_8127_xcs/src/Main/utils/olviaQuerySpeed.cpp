#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include "xcsGlobalDef.h"
#include "dsp_define.h"
#include "vehicleReport.h"
#include "vehicleBasePro.h"
#include "dspParamManage.h"

#include "SysConf.h"
#include "McLog.h"

#include "SysHandle.h"

extern CARROAD_DECT_CONF g_aPassPortChanInfo[MAX_VEHICLE_NUM];

static pthread_t olvia_pid;
static int s_quit= 0;

/*
ITIMER_REAL:    ��ϵͳ��ʵ��ʱ�������㣬���ͳ�SIGALRM�źš�
ITIMER_VIRTUAL: �Ըý������û�̬�»��ѵ�ʱ�������㣬���ͳ�SIGVTALRM�źš�
ITIMER_PROF:    �Ըý������û�̬�º��ں�̬�����ѵ�ʱ�������㣬���ͳ�SIGPROF�źš�
*/
void olviaQueryPro(void)
{
	static unsigned int s_uiRadaNo =0;
	char szQueryData[4]={0xFF,0xAA,0x06,00};

    //szQueryData[3] = g_aPassPortChanInfo[s_uiRadaNo].cModuleId;
    szQueryData[3] = s_uiRadaNo+1;

	dCtlModuleCapWirte(DEV_TYPE_OLVIA, szQueryData, sizeof(szQueryData));

    s_uiRadaNo++;
    //if(s_uiRadaNo >= g_uiClientLaneSum)
    if(s_uiRadaNo >= 3)
    {
    	s_uiRadaNo =0;
    }
}


void sigroutine(int signo)
{
    switch (signo)
    {
        case SIGALRM:
        	olviaQueryPro();
            //shell_print("Catch a signal -- SIGALRM /n");
            signal(SIGALRM, sigroutine);
            break;
        default:
            shell_print("Catch a except signal:%d./n", signo);
            break;
    }
    return;
}

static void *pOlviaDataSendThread(void *arg)
{
   
    writeThreadInfo("olviaSendData");

    struct itimerval value, ovalue;
    printf("olviaSendInit process id is %d /n", getpid());
    
    shell_print("olviaSendInit process id is %d /n", getpid());
    
    signal(SIGALRM, sigroutine);
    
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 50000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 50000;
    setitimer(ITIMER_REAL, &value, &ovalue);
    
    while(1)
    {
    	if ( 1 == s_quit)
        {
            syslog(LOG_ERR|LOG_USER, "olvia close,quit send thread!/n");
            break;
        }
        sleep(1);
    }
    pthread_exit(0);
  
}

int olviaSendInit(void)
{
	int ret =0;
	ret = olviaParalWorkModeGet();
	if(ret >= 1)
	{
    	ret = pthread_create(&olvia_pid,0,&pOlviaDataSendThread,NULL);
    	return ret;
    }
    else
    {
    	shell_print("olvia paral work mode is disable,can't creat pthread!/n");
    	return ret;
    }
}

int olviaSendUnIn(void)
{
    struct itimerval value, ovalue;
    
    shell_print("olviaSendUnIn process id is %d/n", getpid());
    
    signal(SIGALRM, sigroutine);
    
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 0;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &value, &ovalue);

    s_quit = 1;
    
    if(olvia_pid)
    {
        pthread_join(olvia_pid, NULL);
    }
    
    return 0;
}

