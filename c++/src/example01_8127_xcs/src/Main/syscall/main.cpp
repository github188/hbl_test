#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <sys/sem.h>

#include <execinfo.h>

#include "TcpServer.h"

#include "McLog.h"

#include "rtc_util.h"
#include "dbgServer.h"
#include "MmsSend.h"

#include "xcsSysMsg.h"
#include "SysHandle.h"
#include "protoXcs.h"
#include "sysHandle_define.h"

#include "Record.h"

#include "dspParamManage.h"
#include "NetFsCheck.h"
#include "rtc_util.h"

#include "DataUpLoad.h"

#include "NtpConf.h"
#include "Ntp.h"

#include "RawRec.h"

#if defined(CYC200XZD)||defined(CYC500XZD)
#include "zebraCtl.h"
#include "ccdCtl.h"
#else
#include "icx274CCDCtl.h"
#endif

#include "nMCtlApi.h"
#include "epRecApi.h"
#include "dsp_service.h"

#include "hardInfoRw.h"

#include "arpDep.h"

#include "netDctl.h"
//#include "dm6437_service.h"
//#include "dm648_service.h"

#ifdef MODULE_ONVIF

#include "onvifSend.h"
#include "memShm.h"

#endif

#ifdef MODULE_NETREC
#include "memShm.h"
#include "storage.h"
#endif

#define DEBUG_DM8127 1

/* 硬件版本号全局变量 */
unsigned int g_uiHardWVer =0x05;

int SysHandleDebugSwitch(int nLevel);

extern char version_string[];

extern FtpPpSend *g_CPPFtp;

#if defined(CYC500XKW4C)||defined(CYC500XAW)
extern FtpEpSend *g_CEPFtp;
#endif

extern char g_cOnvifVideoSta;
extern char g_cNetRecVideoSta;

extern char g_cRecInitSta;
extern char g_cMmsInitSta;
extern char g_cFtpInitSta;
extern char g_cXcsInitSta;
extern char g_cCCDCtlSta;
extern char g_cAlarmRecSta;

extern char    g_bForceRed[MAX_VEHICLE_NUM];
extern LANE_SWITCH_STA_INFO g_EPLaneSwitchInfo[MAX_VEHICLE_NUM];
extern PP_LANE_SWITCH_STA_INFO g_PPLaneSwitchInfo[MAX_VEHICLE_NUM];

/* 定时查询车检器心跳 */
extern int g_iMonitorVehicleSum;
extern int g_iVehicleHeartSum;

extern int g_aiVehicleType[MAX_COM_DEV];
/* 抓拍图片路径统计信息结构 */
#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)
STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[1];
#elif defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[MAX_VEHICLE_NUM];
#elif defined(CYC500XKW4C)||defined(CYC500XAW)
STR_JPEG_SNAP_COUNT_INFO g_jpegSnapCountInfo[MAX_VEHICLE_NUM];
#endif

strCXProInfo g_strCXProInfo;

MSG_Q_ID pSysManageQueue = NULL;
CDataUpLoad  *g_DataReport = NULL;
int           g_bTcpDataOk = 0;
CNtp         *g_NtpCheck   = NULL;  

/* 电警最新小协议版本号 */
unsigned short g_usEpXcsProVer = XCS_PRO_EPG_VER_2;

/* 控制主线程运行标志，以便于线程退出时进行反初始化 */
static char s_cAppMsgRunSta =1;


/* 为了保证电子警察抓拍的完整性和准确性，现定义手动抓拍和地感线圈抓拍互斥标志位
   只有在地感线圈无抓拍时才能进行手动抓拍,或没有手动抓拍时才能进行地感抓拍，在第一步时
   置上，其他步数在该模块不为1时直接丢弃*/
unsigned char g_ucEPComSnapFlag =0;
/* 地感线圈一组抓拍完成后，需要把串口缓冲全部清空，以便于准确和完整地进行下一次抓拍，即每一组
信令的存在是有其时限性的，一旦超过了其时效，再进行抓拍也是无效的 */ 
unsigned char g_ucEPComBufCleanFlag =0;

/* 电子警察手动抓拍标志，若在手动抓拍则不能进行地感线圈抓拍 */
unsigned char g_ucEPHandSnapFlag =0;

/* 外部摄像机同步中断信号是否正常，若正常则不进行切换，若不正常则需要进行切换  */
char g_cFlashSwichSta =0;

/* 系统视频状态和录像状态全局变量 */
extern STR_VIDEO_RECSTA g_strVRSta;

extern int g_aiCoilSta[MAX_VEHICLE_NUM];
extern int g_aiLampSta[MAX_VEHICLE_NUM];
extern int g_aiVirtualSta[MAX_VEHICLE_NUM];

extern void* g_pRecData;

#define ONE_DAY_SECS (24*60*60)
#define EP_LED_CHECK_CYCLE		5	/* 电警频闪切换检测周期，单位:分钟 */
#define POWER_ON_CHECK_CYCLE	3	/* 上电时频闪切换强制检测周期，单位:分钟 */

#if defined(CYC200XZD)||defined(CYC500XZD)
extern sem_t s_zebraSem;
extern sem_t s_zebraRegCtlSem;
extern int g_iSpiBusSta;
#endif

/*设备温度*/
char g_cTemperature = 28;

int hardWareVerGet(void)
{
    int ret =0;
    CHardInfoRW cHWVerInfo(NULL);
    ret = cHWVerInfo.hardInfoRead(&g_uiHardWVer);
    if( 0 != ret)
    {
        g_uiHardWVer = 0x01;
    }
    return 0;
}

int writeThreadInfo(char *pcThreadInfo)
{
    int fd =0;
    char pid_char[256];
    int pid = getpid();
    
    pthread_t tid;
    
    tid = pthread_self();
    fd = open(MAIN_PID_FILE_NAME, O_WRONLY | O_CREAT | O_APPEND);// | O_EXCL);
    if (fd < 0)
    {
        if (errno == EEXIST)
            SYSHANDLE_ERROR("process already running\n");
        else
            SYSHANDLE_ERROR("open pid file failed\n");

        return errno;
    }

    memset(pid_char, 0, sizeof(pid_char));
    snprintf(pid_char, sizeof(pid_char)-1, "%d tid:%lu %s\n", pid, tid,pcThreadInfo);
    
    write(fd, pid_char, strlen(pid_char));
    close(fd);
    return 0;
}

/******************************************************************************
 * SIGSEGVHandle() :
 * 
 * DESCRIPTION:  可以通过该函数定位到出错的地方
 *  
 * Input: 
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2009.12.16, wangbin Create
 * --------------------
 ******************************************************************************/
void SIGSEGVHandle(int signo)
{
    void *array[20];
    size_t size;
    char **strings;
    size_t i;

    int pid = getpid();
    int fd =0;
    char pid_char[256]={0};

    pthread_t tid;
    tid = pthread_self();

    size = backtrace (array, 20);
    strings = backtrace_symbols (array, size);

    SYSHANDLE_ERROR("threadId:%d tid:%lu Obtained %zd stack frames.\n", pid, tid, size);
    syslog(LOG_ERR|LOG_USER, "threadId:%d tid:%lu Obtained %zd stack frames.\n", pid, tid,size);


    for (i = 0; i < size; i++)
    {
        syslog(LOG_ERR|LOG_USER, "%s\n", strings[i]);
        SYSHANDLE_ERROR ("%s\n", strings[i]);
    }

    free (strings);

    fd = open(MAIN_PID_FILE_NAME, O_RDONLY);
    if (fd < 0)
    {
        syslog(LOG_ERR|LOG_USER, "open pid file failed!\n");
        SYSHANDLE_ERROR("open pid file failed\n");
    }
    read(fd, pid_char, sizeof(pid_char));
    close(fd);

    SYSHANDLE_ERROR("xcs.pid file:%s\n", pid_char);
    syslog(LOG_ERR|LOG_USER, "xcs.pid file:%s\n", pid_char);
    sleep(10);
    exit(0);
}


void exitSignalHandle(int sig)
{
    #if 0
    icx274RegCtlUnInit();
    dspUnInit();
    SYSHANDLE_ERROR("dsp uninit ok\n");
    NetFsUninit();
    RecUnInit();
    SYSHANDLE_ERROR("rec uninit ok\n");
    MmsSendUninit();
    DataReportUninit();
    SYSHANDLE_ERROR("data report uninit ok\n");
    
    //SysConfUninit();
    mcLogUnInit();
    NtpUninit();
    //usleep(20*1000);
    s_cAppMsgRunSta =0;
    syslog(LOG_ERR|LOG_USER, "pid: %d xcs exit, recv a signal : 0x%x", getpid(), sig);
    #endif
    SYSHANDLE_ERROR("xcs main process is killed\n");
    printf("xcs main process is killed\n");
    //NET_CYC_Stop();
	//NET_CYC_Cleanup();
    exit(0);
}


void fpeSignalHandle(int sig)
{
    int fd =0;
    char pid_char[256]={0};
    
    s_cAppMsgRunSta =0;
    SYSHANDLE_ERROR("div 0 xcs main process is killed\n");
    syslog(LOG_ERR|LOG_USER, "div 0 error,xcs pid:%d exit, recv a signal : 0x%x", getpid(), sig);
    
    fd = open(MAIN_PID_FILE_NAME, O_RDONLY);
    if (fd < 0)
    {
        syslog(LOG_ERR|LOG_USER, "open pid file failed!\n");
        SYSHANDLE_ERROR("open pid file failed\n");
    }
    read(fd, pid_char, sizeof(pid_char));
    close(fd);
    
    syslog(LOG_ERR|LOG_USER, "xcs.pid file: %s\n", pid_char);
    sleep(10);
    exit(0);
}

int appQuitCtrlC(void) 
{
    struct sigaction act; 

    act.sa_handler = exitSignalHandle; 
    sigemptyset(&act.sa_mask); 
    act.sa_flags=0; 
    if( sigaction(SIGINT, &act, NULL) < 0 ) 
    { 
        fprintf(stderr, "Install Signal Action Error:%s\n\a",strerror(errno));
        exit(1); 
    }
    return 0;
}

void pipeSignalHandle(int sig)
{
    int fd =0;
    char pid_char[256]={0};
    
    SYSHANDLE_ERROR("sigpipe error,xcs main process is killed\n");
    syslog(LOG_ERR|LOG_USER, "sigpipe error,xcs pid:%d exit, recv a signal : 0x%x", getpid(), sig);
    
    fd = open(MAIN_PID_FILE_NAME, O_RDONLY);
    if (fd < 0)
    {
        syslog(LOG_ERR|LOG_USER, "open pid file failed!\n");
        SYSHANDLE_ERROR("open pid file failed\n");
    }
    read(fd, pid_char, sizeof(pid_char));
    close(fd);
    
    syslog(LOG_ERR|LOG_USER, "xcs.pid file: %s\n", pid_char);
    sleep(2);
    syslog(LOG_ERR|LOG_USER, "pipe error,killall xcs\n");
    system("killall xcs");
}


static void extSignalHand()
{
    /* socket send异常后会发送sigpipe信号导致线程退出 */
    //signal(SIGPIPE, SIG_IGN);

    #if 1
    struct sigaction action;
    action.sa_handler = pipeSignalHandle;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);
    #endif


    /* 进程退出或被杀死信号处理 */
    signal(SIGKILL, exitSignalHandle);
    signal(SIGTERM, exitSignalHandle);
    /* 除0错误或计算异常处理 */
    signal(SIGFPE, fpeSignalHandle);
    
    /* 段错误等异常信号处理 */
    signal(SIGSEGV, SIGSEGVHandle);
    
    /* 用户按下CTRL+C时进行程序的反初始化 */
    appQuitCtrlC();
    signal(SIGINT, exitSignalHandle);

    /* 防止进程产生僵尸线程 */
    //signal(SIGCHLD,SIG_IGN);
    #if 0
    action.sa_handler = SIG_IGN;
    sigaction(SIGCHLD, &action, NULL);
    #endif
}

/******************************************************************************
 * secSwitchToDateString() :
 * 
 * DESCRIPTION:  秒数转换成真实世界所使用的时间日期并显示
 * Input: iTime,公元1970年1月1日的UTC时间从0时0分0秒算起到现在所经过的秒数
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.12.23, zy Create
 * --------------------
 ******************************************************************************/
void secSwitchToDateString(unsigned long ulSecs, char *pcBuf)
{
    struct tm tmPlanStart;

    gmtime_r((time_t *)&ulSecs, &tmPlanStart);
    sprintf(pcBuf,"%04d-%02d-%02d %02d:%02d:%02d",\
        tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
        tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec);
    //shell_print("seconds is %d, date is %s\n", ulSecs, pcBuf);
}

void mainRebootAtTime(void)
{
    struct tm tmPlanStart;
    unsigned long ulPlanStart = time(NULL) + 8*3600;

	gmtime_r((time_t *)&ulPlanStart, &tmPlanStart);
    if( (tmPlanStart.tm_hour == 23) && (tmPlanStart.tm_min == 59) )
    {
        SYSHANDLE_ERROR("cur date:hour:%d,min:%d,reboot sipsdk!\n", tmPlanStart.tm_hour, tmPlanStart.tm_min);
        mcLog(LMT_SYSCALL,LT_ERROR,"cur date:hour:%d,min:%d,reboot sipsdk!\n", tmPlanStart.tm_hour, tmPlanStart.tm_min);
		system("killall sipsdk");
    }
}

void mainAppShowDate()
{
    int nRet =0;
    unsigned int ulRtc = 0;
    static unsigned int s_uiLastTimeSec =0;
    unsigned long ulPlanStart = time(NULL) + 8*3600;
    char szDefault[256];

    memset(szDefault,0x00,sizeof(szDefault));
    secSwitchToDateString(ulPlanStart, szDefault);
    SYSHANDLE_ERROR("cur date:%s\n", szDefault);
    /* 每隔一天就通过读RTC对系统进行对时 */
    if((ulPlanStart - s_uiLastTimeSec) > ONE_DAY_SECS)
    {   
        s_uiLastTimeSec = ulPlanStart;
        nRet = GetRtc(&ulRtc);
        if(nRet < 0)
        {
            syslog(LOG_ERR|LOG_USER, "RTC sync timer:read rtc time failed!\n");
            return ;
        }
        memset(szDefault,0x00,sizeof(szDefault));
        secSwitchToDateString(ulRtc+8*3600, szDefault);
        syslog(LOG_ERR|LOG_USER, "RTC sync timer:RTC time is %s!\n", szDefault);
        syslog(LOG_ERR|LOG_USER, "RTC sync timer:xcs cur time is %s!\n", szDefault);
        SysSetTime(ulRtc);

        /* 隔一天就把打印输出配置成error模式，以减小打印输出 */
        DebugSwitch("ALL", 0x01);
    }
}

static int camTemperatureSend(void)
{
    unsigned int iValue =0;
    int iSta[MAX_VEHICLE_NUM] ={0};
    
    int fd,size;
	char buffer[10];

	fd=open("/sys/bus/i2c/devices/1-0048/temp1_input", O_RDONLY);
	size=read(fd,buffer,sizeof(buffer));
	close(fd);

	//printf("%s",buffer);

    iValue = strtoul(buffer, NULL, 10);
    SYSHANDLE_INFO("kodar cam temperature is %s,val:%d!\n", buffer, iValue);

    iSta[0] = (iValue)/1000;	
	g_cTemperature = iSta[0];
    nMMsgSend(NM_CAM_TEMPERATURE_STA, 0, 0, iSta);
    return 0;
}

/******************************************************************************
 * flashLampStaMonitor() :
 * 
 * DESCRIPTION:  动态检测外部摄像机同步中断信号，若不来，则由闪光灯模式切换到卤光
   灯模式
 * Input: cMode，0：获取初始同步中断数; 1:动态检测同步中断数有没有变化
 * Output: 
 * Returns: 
 * 
 * modification history
 * --------------------
 *01a, 2010.01.22, wangbin Create
 * --------------------
 ******************************************************************************/
static int flashLampStaMonitor(char cMode)
{
    unsigned int uiTmpSum =0;
    static unsigned int s_uiFlashIntSum =0;
    int iSta[MAX_VEHICLE_NUM] ={0};
    dctlModuleFlashIntNumGet(&uiTmpSum);
    SYSHANDLE_INFO("flash lamp current num:0x%x, last num:0x%x\n", uiTmpSum, s_uiFlashIntSum);
    if(0 == cMode)
    {
        /* 系统启动时先进行闪光灯初始状态的读取 */
        s_uiFlashIntSum = uiTmpSum;
        g_cFlashSwichSta =0;
        if(0 == s_uiFlashIntSum)
        {
            iSta[0] = MODE_ERROR_STA;
            nMMsgSend(NM_CAM_SYNC_INT_STA, 0, 0, iSta);
        }
        else
        {
            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_CAM_SYNC_INT_STA, 0, 0, iSta);
        }
        return 0;
    }
    else
    {
        /* 若外部摄像机中断信号不来了，则自动切换到卤光灯模式 */
        if(s_uiFlashIntSum == uiTmpSum)
        {
            if(0 == g_cFlashSwichSta)
            {
                SYSHANDLE_ERROR("flash lamp int is error,switch to Halogen mode.\n");
                g_cFlashSwichSta =1;
                syslog(LOG_ERR|LOG_USER, "flash lamp int is error,switch to Halogen mode\n");

                #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                    dctlPPFlashConf(FLASH_DYNA_SET_MODE, LIGHT_TYPE_HALGON);
                #else
                    dctlEPFlashConf(FLASH_DYNA_SET_MODE, LIGHT_TYPE_HALGON);
                #endif
                dspVLFlashModeSet();
                iSta[0] = MODE_ERROR_STA;
                nMMsgSend(NM_CAM_SYNC_INT_STA, 0, 0, iSta);
            }
            else
            {
                SYSHANDLE_INFO("flash lamp int is error,work at Halogen mode.\n");
            }
        }
        else
        {
            if(1 == g_cFlashSwichSta)
            {
                SYSHANDLE_ERROR("flash lamp int is ok,switch to flash mode.\n");
                /* 若恢复了，则切换到闪光灯模式 */
                s_uiFlashIntSum = uiTmpSum;
                g_cFlashSwichSta =0;
                syslog(LOG_ERR|LOG_USER, "flash lamp int is ok,switch to flash lamp mode\n");
                
                #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                    dctlPPFlashConf(FLASH_DYNA_SET_MODE, LIGHT_TYPE_FLASH);
                #else
                    dctlEPFlashConf(FLASH_DYNA_SET_MODE, LIGHT_TYPE_FLASH);
                #endif
                dspVLFlashModeSet();
                iSta[0] = MODE_OK_STA;
                nMMsgSend(NM_CAM_SYNC_INT_STA, 0, 0, iSta);
            }
            else
            {
                s_uiFlashIntSum = uiTmpSum;
                SYSHANDLE_INFO("flash lamp int is ok,work flash mode.\n");
            }
        }
    }
    return 0;
}

/* 系统刚上电时，对车检器掉电前运行状态做一次判断，若上次为异常则要做相应的切换工作 */
int vehicleIniStaSwitch(void)
{
    int i =0;
    if( dctlVehilceStaGet() == 0 )
    {
        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            SYSHANDLE_ERROR("vehicle start is stop,switch to virtual coil\n");
            for(i=0; i< MAX_COM_DEV; i++)
            {
                switch(g_aiVehicleType[i])
                {
                	case DEV_TYPE_CY:
                    case DEV_TYPE_KJ:
                    case DEV_TYPE_BEST:
                        kjPPCoilStaSwitch(BREAKDOWN_STA_STOP, 0);
                        break;
                    case DEV_TYPE_SJ:
                        sjPPCoilStaSwitch(0, BREAKDOWN_STA_STOP);
                        break;
                    default:
                        break;
                }
            }
            
        #elif defined(CYC500XKW4C)||defined(CYC500XAW)
            SYSHANDLE_ERROR("vehicle start is stop,switch to virtual coil\n");
            for(i=0; i< MAX_COM_DEV; i++)
            {
                switch(g_aiVehicleType[i])
                {
                	case DEV_TYPE_CY:
                    case DEV_TYPE_KJ:
                    case DEV_TYPE_BEST:
                        kjEPCoilStaSwitch(BREAKDOWN_STA_STOP, 0, 0);
                        break;
                    default:
                        break;
                }
            }
        #endif
    }
    return 0;
}


/*
 * Function:       
 * Description:    主要的作用是用于检视车检器是否在工作，若掉电则红灯检测切换到视频检测模式，若上电
 了则切回到指定检测模式
 * Calls:          
 * Called By:      
 * Table Accessed: 
 * Table Updated:  
 * Input:          
 * Output:         
 * Return:         
 * Others:         
 */
int vehicleStaMonitor(void)
{
    int i =0;
    SYSHANDLE_INFO("vehicle monitor,monitorSum:0x%x,gHeart:0x%x\n", g_iMonitorVehicleSum, g_iVehicleHeartSum);
    if( g_iMonitorVehicleSum == g_iVehicleHeartSum )
    {
        if( dctlVehilceStaGet() == 1 )
        {
            dctlVehilceStaSet(0);
            #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                SYSHANDLE_ERROR("vehicle is stop,switch to virtual coil\n");
                for(i=0; i< MAX_COM_DEV; i++)
                {
                    switch(g_aiVehicleType[i])
                    {
                    	case DEV_TYPE_CY:
                        case DEV_TYPE_KJ:
                        case DEV_TYPE_BEST:
                            kjPPCoilStaSwitch(BREAKDOWN_STA_STOP, 0);
                            break;
                        case DEV_TYPE_SJ:
                            sjPPCoilStaSwitch(0, BREAKDOWN_STA_STOP);
                            break;
                        default:
                            break;
                    }
                }
                
            #elif defined(CYC500XKW4C)||defined(CYC500XAW)
                for(i=0; i< MAX_COM_DEV; i++)
                {
                    switch(g_aiVehicleType[i])
                    {
                    	case DEV_TYPE_CY:
                        case DEV_TYPE_KJ:
                        case DEV_TYPE_BEST:
                            kjEPCoilStaSwitch(BREAKDOWN_STA_STOP, 0, 0);
                            break;
                        default:
                            break;
                    }
                }
            #endif
        }
        else
        {
            SYSHANDLE_INFO("vehicle is failed,work at virtual coil\n");
        }
    }
    else
    {
        if( dctlVehilceStaGet() == 0 )
        {
            dctlVehilceStaSet(1);
        #if defined(CYC500XKW4C)||defined(CYC500XAW)
            for(i=0; i< MAX_COM_DEV; i++)
            {
                switch(g_aiVehicleType[i])
                {
                	case DEV_TYPE_CY:
                    case DEV_TYPE_KJ:
                    case DEV_TYPE_BEST:
                        kjEPCoilStaSwitch(BREAKDOWN_STA_WORK, 0, 0);
                        break;
                    default:
                        break;
                }
            }
        #elif defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
            for(i=0; i< MAX_COM_DEV; i++)
            {
                switch(g_aiVehicleType[i])
                {
                	case DEV_TYPE_CY:
                    case DEV_TYPE_KJ:
                    case DEV_TYPE_BEST:
                        kjPPCoilStaSwitch(BREAKDOWN_STA_WORK, 0);
                        break;
                    default:
                        break;
                }
            }
        #endif
        }
        else
        {
            SYSHANDLE_INFO("vehicle is ok,work at group coil\n");
        }
    }
    return 0;
}


int recStaNMProc(unsigned char cChan)
{
    static STR_VIDEO_RECSTA s_strTmpData={0};
    int iSta[MAX_VEHICLE_NUM] ={0};
    if( g_strVRSta.iRecEnable )
    {
        if( g_strVRSta.iRecSum[cChan] == s_strTmpData.iRecSum[cChan])
        {
            iSta[0] = MODE_ERROR_STA;
            nMMsgSend(NM_RECORD_STA, cChan, 0, iSta);
        }
        else
        {
            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_RECORD_STA, cChan, 0, iSta);
        }
    }
    else
    {
        iSta[0] = MODE_SHUTDOWN_STA;
        nMMsgSend(NM_RECORD_STA, cChan, 0, iSta);
    }
    
    if( g_strVRSta.iVideoEnable )
    {
        if( g_strVRSta.iVideoSum[cChan] == s_strTmpData.iVideoSum[cChan])
        {
            iSta[0] = MODE_ERROR_STA;
            nMMsgSend(NM_VIDEO_STA, 0, 0, iSta);
        }
        else
        {
            iSta[0] = MODE_OK_STA;
            nMMsgSend(NM_VIDEO_STA, 0, 0, iSta);
        }
    }
    else
    {
        iSta[0] = MODE_SHUTDOWN_STA;
        nMMsgSend(NM_VIDEO_STA, 0, 0, iSta);
    }
    s_strTmpData = g_strVRSta;
    return 0;
}


int xcsAlarmRecInit(void)
{
    int ret = 0;
#ifdef MODULE_ALARMREC

#if defined(CYC500XKW4C)||defined(CYC500XAW)
    FTP_CONF_INFO strPpFtpConfInfo;
    ret = epAlarmRecFlagGet();
    if(ret > 0)
    {
        memset(&strPpFtpConfInfo, 0, sizeof(strPpFtpConfInfo));
        ret = SysConfGet(e_PASSPORT_FTP_CONF, &strPpFtpConfInfo, 0);
          if( 0 == ret)
        {
            ret = epRecModuleInit();
            if( 0 != ret )
            {
                printf("alarmRecInit failed.\n");
                SYSHANDLE_ERROR("alarmRecInit failed.\n");
                return -1;
            }
            else
            {
                printf("alarmRecInit ok.\n");
                //SYSHANDLE_INFO("time rec interval:%d,last:%d.\n", strPpFtpConfInfo.usTRecInterval, strPpFtpConfInfo.ucTRecLast);
                //epRecModuleTimerRecConfSet(strPpFtpConfInfo.usTRecInterval, strPpFtpConfInfo.ucTRecLast, strPpFtpConfInfo.cTRecEnable);
                //epRecModuleFileNameSet(TIME_REC_TYPE, "/alarmRec/timerRec.asf");
                //epRecModuleTimerRecCallBackFunSet(TimerRecFileSend);

                epRecModuleFileNameSet(EP_REC_TYPE, "/alarmRec/epRec.asf");
                epRecModuleAlarmRecCallBackFunSet(alarmRecFileSend);
                SYSHANDLE_INFO("alarmRecInit ok.\n");
                g_cAlarmRecSta = 1;
                return 0;
            }
        }
        else
        {
            SYSHANDLE_ERROR("get ep rec conf info failed.\n");
            return -1;
        }
    }
    else
    {
        SYSHANDLE_ERROR("get ep alarm rec enable flag failed or not enable.\n");
        g_cAlarmRecSta = 0;
        return -1;
    }
#endif

#endif
    return ret;
}


int timerRecDyncConf(void)
{
    int ret = 0;
#ifdef MODULE_ALARMREC

#if defined(CYC500XKW4C)||defined(CYC500XAW)
    FTP_CONF_INFO strPpFtpConfInfo;
    memset(&strPpFtpConfInfo, 0, sizeof(strPpFtpConfInfo));
    ret = SysConfGet(e_PASSPORT_FTP_CONF, &strPpFtpConfInfo, 0);
      if( 0 == ret)
    {
        SYSHANDLE_INFO("time rec interval:%d,last:%d.\n", strPpFtpConfInfo.usTRecInterval, strPpFtpConfInfo.ucTRecLast);
        //epRecModuleTimerRecConfSet(strPpFtpConfInfo.usTRecInterval, strPpFtpConfInfo.ucTRecLast, strPpFtpConfInfo.cTRecEnable);
        return 0;
    }
    else
    {
        SYSHANDLE_ERROR("get pp ftp conf info failed.\n");
        return -1;
    }
#endif

#endif

    return ret;
}

#ifdef MODULE_ONVIF
int shamemTest(void )
{
    static int s_i=0;
    int ret =0;
    long data_len = 0x200000;
    void *shareBuff = NULL;

    shareBuff =  memShmMalloc(data_len);
    if (!shareBuff)
    {
        shell_print("%s %d DSPCOMM: memShmMalloc failed data_len:%d.\n", __FUNCTION__, __LINE__, data_len);
        return -1;
    }
    else
    {
        shell_print("%s %d DSPCOMM: memShmMalloc ok data_len:%d.\n", __FUNCTION__, __LINE__, data_len);
    }

    s_i++;

    memset(shareBuff, s_i, data_len);

    int offset = memShmOffset(shareBuff);

    STREAMDATA msg = {0, 0, 0};

	msg.nChannel = 0;
	msg.offset = offset;
	msg.nLen = data_len;

    ret = args_send( (const char *)&msg, sizeof(msg));
    if(ret < 0)
    {
        memShmFree(shareBuff);
        shell_print("args_send failed len=0x%x\n", sizeof(msg));
    }
    else
    {
        shell_print("args_send ok. offset:0x%x,nLen:0x%x.\n", msg.offset, msg.nLen);
    }
    return 0;
}
#endif

static int mainMsgQProc(MSG_Q_ID pMsgQId)
{

    
    unsigned int i =0;
    int ret =0;
    STR_SYS_APP_MSG strSysAppMsg;
    unsigned int uiClickSun =0;

    int iCheckFlag =0;
    int iCheckCycle =0; /* 分钟为单位 */
    
    if(NULL == pMsgQId)
    {
        SYSHANDLE_ERROR("in pMsgQId is null\n");
        return -1;
    }
  
    SYSHANDLE_ERROR("\n\n #####enter mainMsgQProc is here.\n\n");
	 SYSHANDLE_ERROR("\n\n #####enter mainMsgQProc is here.\n\n");
	  SYSHANDLE_ERROR("\n\n #####enter mainMsgQProc is here.\n\n"); 
	   SYSHANDLE_ERROR("\n\n #####enter mainMsgQProc is here.\n\n");
	   SYSHANDLE_ERROR("\n\n #####enter mainMsgQProc is here.\n\n");
	    SYSHANDLE_ERROR("\n\n #####enter mainMsgQProc is here.\n\n");
	  
#if DEBUG_DM8127
    /* 进入死循环前，先读取第一次摄像机同步中断值 */
    //flashLampStaMonitor(0);
    /* 进入死循环前，先读取一次摄像机当前工作温度 */
    camTemperatureSend();

    /* 进入死循环前，先对车检器进行一次判断 */
    vehicleIniStaSwitch();
#endif

    while(1)
    {
        if( 1 != s_cAppMsgRunSta)
            break;
	
        memset(&strSysAppMsg, 0, sizeof(strSysAppMsg));
        ret = msgQReceive(pMsgQId, (char *)&strSysAppMsg, sizeof(strSysAppMsg), 1*1000);
        if( sizeof(strSysAppMsg) == ret )
        {
            SYSHANDLE_INFO("App rcv moduleId:%d,sysType:%d\n", strSysAppMsg.uiModuleId, strSysAppMsg.uiMsgType);
            if (SYS_MSG_NTP_MODULE_ID == strSysAppMsg.uiModuleId)
            {
                switch( strSysAppMsg.uiMsgType )
                {
                    case MSG_NTP_TIME_REPORT:
                        {
                            struct timeval tv;
                            char szDate[256] ={0};
                            memcpy(&tv,strSysAppMsg.aucData,sizeof(struct timeval));
                            long lUtc =tv.tv_sec;
                            SYSHANDLE_INFO("ntp set timer is here\n");
                            secSwitchToDateString(lUtc+8*3600, szDate);
                       	    syslog(LOG_ERR|LOG_USER, "ntp sync time,secs is %d,date is %s!\n", (unsigned int)lUtc, szDate);
                            SysSetTime((unsigned int)lUtc);
                        }
                        break;
			 case MSG_UART_SIMU_RUN_SNAP:     //串口 抓拍
                    	{
							SYSHANDLE_ERROR("#####uart########\n");
								SYSHANDLE_ERROR("###uart##########\n");
								SYSHANDLE_ERROR("####uart#########\n");
								
			   SYSHANDLE_ERROR("####%d.\n",strSysAppMsg.ucCarRoadId);
			 	  FORCE_SNAP fsp;
				memset(&fsp, 0, sizeof(fsp));
				fsp.chan_id = 0;
				fsp.lane_id = 0;
				fsp.iContinuSnapEn = 0;
				NET_DEV_ManualSnap(&fsp, 2);
                        break;
                    	}
                    default:
                        {
                            syslog(LOG_ERR|LOG_USER, "ntp sync time is here,but msgType:%d is error!\n", strSysAppMsg.uiMsgType);
                        }
                        break;
                }
            }
            else if(SYS_MSG_PROTOCOL_MODULE_ID == strSysAppMsg.uiModuleId)
            {
                switch( strSysAppMsg.uiMsgType )
                {
                    case MSG_SYS_TIME_SET:
                        {
                            unsigned int uiTime =0;
                            char szDate[256]={0};
                            memcpy(&uiTime, strSysAppMsg.aucData, sizeof(uiTime));
                            secSwitchToDateString(uiTime+8*3600, szDate);
                       	    syslog(LOG_ERR|LOG_USER, "kj client or v3 sync timer,secs is %d,date is %s!\n", uiTime, szDate);
                            SYSHANDLE_INFO("kj client or V3 set time,secs is %d,date is %s!\n", uiTime, szDate);
                            SysSetTime(uiTime);
                        }
                        break;
                    case MSG_DEV_CTL_CAP:
                    #ifdef MODULE_DEV_CTL
                        dctlModuleCapDel();
                        devCtlParamConf();
                    #endif
                        break;
                    case MSG_BAYONET_GLOBAL_INFO_SET:
                        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
						{
							ppGetUsedLaneSum();
							VALID_LANE_NUM_PARAM vlnp;
							memset(&vlnp, 0, sizeof(vlnp));
							vlnp.chan_id = 0;
							vlnp.lane_id = g_uiClientLaneSum;
							CALL_CYC_CMD(NET_DEV_SET_VALID_LANE_NUM, vlnp);
						}
                        dspPpTimerSwitchDec(CAM_PARAM_MODE_MANUAL);
                        dspPpFillInLightSwitchDec(CAM_PARAM_MODE_MANUAL);
                        #endif
                        ftpPPChannelNameSet();
                        dspLPRParamSet(0);
                        for(i =0; i< g_uiClientLaneSum; i++)
                        {
                            #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                            dspPPSnapOsdSet(0, i);
                            dspPPSnapAreaSet(0, i);
	                        dspPPCoilTypeSet(0, i);
                            dctlPPFlashConf(FLASH_NORMAL_SET_MODE, i);
                            #else
                            dspEpPpSnapOsdSet(SINGAL_LANE_SET_MODE, i);
                            #endif
                        }
                        dctlLoadPPCoil();
                        dctlPPLoadSwitchInfo();
                        
                        dspPPVSpeedFlagSet();
                        dspPpFunEnSet(0,0);

                        dspPPIrregNameInfoSet();

                        g_iVehicleHeartSum++;
                        vehicleStaMonitor();

                        break;
                    case MSG_BAYONET_CAR_CHANNEL_INFO_SET:
                        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                        dspPPSnapOsdSet(0, strSysAppMsg.ucCarRoadId);
                        dspPPSnapAreaSet(0, strSysAppMsg.ucCarRoadId);
                        dspPPUnidirLaneTimeSet(0, strSysAppMsg.ucCarRoadId);
                        dspPPCoilTypeSet(0, strSysAppMsg.ucCarRoadId);
                        dctlLoadPPCoil();
                        dctlPPLoadSwitchInfo();
                        dctlPPFlashConf(FLASH_NORMAL_SET_MODE, 0);
                        dspPPVLParamSet(0, strSysAppMsg.ucCarRoadId);
                        dspVLFlashModeSet();
                        dspPPLimitSpeedSet(0, strSysAppMsg.ucCarRoadId);
                        g_cFlashSwichSta =0;
                        
                        g_iVehicleHeartSum++;
                        vehicleStaMonitor();
						
						dspPpFunEnSet(0,0);
                        #endif
                        break;
                    case MSG_BAYONET_HIGH_VL_INFO_SET:
                        {
                            dspPPHighVLParamSet(0, strSysAppMsg.ucCarRoadId);
                            break;
                        }
                     case MSG_VIDEO_PARAM_SET:
                        {
                            dspEncParamSet(strSysAppMsg.ucChannelId);
                            #if defined(CYC200XKW)
                            camCtlParamRunJudge(CAM_PARAM_MODE_FORCE);
                            #endif
                        }
                        break;
                    case MSG_BAYONET_MANUAL_SNAP:
                        {
                            PSTR_BAYONET_HAND_SNAP pstrHandSnapInfo = (PSTR_BAYONET_HAND_SNAP)(strSysAppMsg.aucData);
                            dspPPMSnap(pstrHandSnapInfo->uiCarRoadId, pstrHandSnapInfo->uiCarSpeed, CAR_RUN_CORRECT_DIR);
                        }
                        break;
                    case MSG_BAYONET_FTP_INFO_SET:
                    case MSG_EP_FTP_INFO_SET:
					#if defined(CYC500XKW4C)||defined(CYC500XAW)
                        timerRecDyncConf();
                    #endif
                        ftpServerChange();
                        break;
                    case MSG_EP_TIME_RANGE_INFO_SET:
                        epTimeRangeJudge(strSysAppMsg.ucCarRoadId);
                        break;
                    case MSG_CAMARA_CTL_INFO_SET:
                        sysCamTimeUpdate();
                        camCtlParamRunJudge(CAM_PARAM_MODE_FORCE);
                        #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                        kodarDubalShutParamSet(DUBAL_FLASH_FORCE_SWITCH);
                        dspPpFillInLightSwitchDec(CAM_PARAM_MODE_MANUAL);
                        #else
                        dspEpFillInLightSwitchDec(CAM_PARAM_MODE_MANUAL);	// tangzl 0725
                        #endif
                        break;
                    case MSG_CAMARA_TIME_RANGE_SET:
                        sysCamTimeUpdate();
                        break;
                    case MSG_REMOTE_REBOOT:
                        xcsSysReboot();
                        break;
                    case MSG_REMOTE_RESTORE_PARAM:
                    	xcsSysRestoreDevConf();
                    	break;
                    case MSG_NTP_SERVER_SET:
                        ntpServerIpSet();
                        break;
                    case MSG_REMOTE_UPDATE:
                        xcsSysUpdate(strSysAppMsg.aucData);
                        break;
                    case MSG_330_DEV_CONNECT_INFO_SET:
                        ftpPP330PlatFormInfoSet();
                        break;
                    case MSG_EP_330_DEV_CONNECT_INFO_SET:
						#if defined(CYC500XKW4C)||defined(CYC500XAW)
                        ftpEP330PlatFormInfoSet();
                        #endif
                        break;
                    //case MSG_EP_RED_LIGHT_ENFORCE_SET:
                    //    break;
                    case MSG_ELECT_POLICE_SIMU_RUN_SNAP:     //电警察 手动抓拍
                    	{
							SYSHANDLE_ERROR("#############\n");
								SYSHANDLE_ERROR("#############\n");
								SYSHANDLE_ERROR("#############\n");
								SYSHANDLE_ERROR("#############\n");
								
			   SYSHANDLE_ERROR("####%d.\n",strSysAppMsg.ucCarRoadId);
                        dspEPMSnap(strSysAppMsg.ucCarRoadId);
                        break;
                    	}

			 case MSG_UART_SIMU_RUN_SNAP:     //串口 抓拍
                    	{
							SYSHANDLE_ERROR("#####uart########\n");
								SYSHANDLE_ERROR("###uart##########\n");
								SYSHANDLE_ERROR("####uart#########\n");
								
			   SYSHANDLE_ERROR("####%d.\n",strSysAppMsg.ucCarRoadId);
			   FORCE_SNAP fsp;
			memset(&fsp, 0, sizeof(fsp));
			fsp.chan_id = 0;
			fsp.lane_id = 0;
			fsp.iContinuSnapEn = 0;
			NET_DEV_ManualSnap(&fsp, 2);
                        break;
                    	}
                    case MSG_EP_CAR_CHANNEL_INFO_SET:
                        g_iVehicleHeartSum++;
                        vehicleStaMonitor();
                        g_cFlashSwichSta =0;
                        dctlEPFlashConf(FLASH_NORMAL_SET_MODE,0);
                        dspEPCoilTypeSet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        dspEPSnapOsdSet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        dspEPLaneAreaSet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        dspVLFlashModeSet();
                        dCtlLoadEPCoil();
                        dctlEPLoadRedInfo(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        ftpEPRoadRemarksSet();
                        dspEPVLAreaSet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        dspEpFuncEn(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        dspEPRedLightSet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        dspEPSOTNoEntrySet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);

                        dspEpPpSnapOsdSet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);
                        break;
                    case MSG_EP_GLOBAL_INFO_SET:
                        g_iVehicleHeartSum++;
                        vehicleStaMonitor();
                        g_cFlashSwichSta =0;
                        epGetUsedLaneSum();
						{
							VALID_LANE_NUM_PARAM vlnp;
							memset(&vlnp, 0, sizeof(vlnp));
							vlnp.chan_id = 0;
							vlnp.lane_id = g_uiClientLaneSum;
							CALL_CYC_CMD(NET_DEV_SET_VALID_LANE_NUM, vlnp);
						}
                        
                        dspEpTimerSwitchDec(CAM_PARAM_MODE_MANUAL);
                        dspEpFillInLightSwitchDec(CAM_PARAM_MODE_MANUAL);
                        
                        dctlEPFlashConf(FLASH_NORMAL_SET_MODE,0);
                        dspVLFlashModeSet();
                        dCtlLoadEPCoil();
                        dctlEPLoadRedInfo(ALL_LANE_SET_MODE, 0);
                        dspEPSnapOsdSet(ALL_LANE_SET_MODE, 0);
                        dspEpPpSnapOsdSet(ALL_LANE_SET_MODE, 0);
                        dspEPLaneAreaSet(ALL_LANE_SET_MODE, 0);
                        dspEPRedLightAreaSet(RL_NORMAL_SET_MODE, 0);
                        ftpEPChannelNameSet();
                        ftpEPRoadRemarksSet();
                        dspEPVLAreaSet(ALL_LANE_SET_MODE, 0);
                        /* 虚拟线圈下卡口图片是否发送标志设置 */
                        dspEPCoilTypeSet(ALL_LANE_SET_MODE, 0);
                        dspEpFuncEn(ALL_LANE_SET_MODE, 0);
                        dspEPRedLightSet(ALL_LANE_SET_MODE, 0);
                        dspEPSOTNoEntrySet(ALL_LANE_SET_MODE, 0);

                        ftpEPIrregNameInfoSet();
                        dspEPIrregNameInfoSet();

                        /* 电警AC交流同步切换定时检测 */
                        iCheckFlag = dspEPACSwitchParamGet(&iCheckCycle);
                        if( iCheckFlag > 0)
                        {
                            dspEpACPhaseSwitchDec(CAM_PARAM_MODE_FORCE);
                        }
						SYSHANDLE_INFO("call MSG_EP_GLOBAL_INFO_SET ok.\n");
                        break;
                    case MSG_DATA_REPORT_PARAM_SET:
                        STR_DATA_REPORT_SET *pStrDataReportSet ;
                        pStrDataReportSet = (STR_DATA_REPORT_SET *)strSysAppMsg.aucData;
                        SetDataReportParam(pStrDataReportSet);   /*add by forejolly  添加发送到服务器的端口号和IP*/
                        break;
                    case MSG_XCS_EXCEPT_DISCONNECT:
                        {
                            STR_EXCEPT_DISCON_CLIENT *pstrClientInfo;
                            pstrClientInfo = (STR_EXCEPT_DISCON_CLIENT *)strSysAppMsg.aucData;
                
                            g_DataReport->DelServer(pstrClientInfo->uiDataUploadIp, pstrClientInfo->usDataUploadPort);
                            g_bTcpDataOk = 0;

                            MMS_INFO strMmsInfo;
                            ret = mmsSectionGet(&strMmsInfo, ntohl(pstrClientInfo->uiIp));
                            if (ret < 0)
                            {
                                SYSHANDLE_INFO("main pMms is NULL\n");
                                break;
                            }
                            else
                            {
                                ret = MmsStop(strMmsInfo.cChannel, strMmsInfo.nIp, strMmsInfo.usPort,\
                                strMmsInfo.cSplitMode, strMmsInfo.cConnectMode);
                                if(0 != ret )
                                {
                                    SYSHANDLE_ERROR("except stop MmsStop failed,ret:%d\n", ret);
                                }
                                else
                                {
                                    SYSHANDLE_INFO("except stop MmsStop ok,ret:%d\n", ret);
                                }
                                syslog(LOG_ERR|LOG_USER,"xcs except disconnect,del mms section!\n");
                                mmsSectionDel(&strMmsInfo);
                            }
                        }
                        break;
                    case MSG_EP_IRREG_INFO_SET:
                        ftpEPIrregNameInfoSet();
                        dspEPIrregNameInfoSet();
                        break;
                    case MSG_EP_LPR_INFO_SET:
                        dspEpLPRParamSet(0);
                        break;
                    case MSG_EP_SOT_NO_ENTRY_SET:
                        dspEPSOTNoEntrySet(SINGAL_LANE_SET_MODE, strSysAppMsg.ucCarRoadId);                        
                        break;
                    case MSG_EP_JPG_OSD_SET:
                    	dspEpJpgOsdConfSet(0);
                    	break;
                    case MSG_PP_JPG_OSD_SET:
                    	dspPpJpgOsdConfSet(0);
                    	break;
                   	case MSG_PP_IRREG_INFO_SET:
                        ftpPPIrregNameInfoSet();
                        dspPPIrregNameInfoSet();
                        break;
                        
                    case MSG_NRM_INFO_SET:
                    
                    #ifdef MODULE_NETREC
                    	StorageUninit();
                    	StorageInit();
                    #endif
                    	break;

                    default:
                        break;
                }
            }
            else if (SYS_MSG_NETFS_MODULE_ID == strSysAppMsg.uiModuleId)
            {
                switch (strSysAppMsg.uiMsgType)
                {
                    case NETFS_STA_CHANGE:
                        {
                        #ifdef MODULE_CIFS
                            STR_NETFS_STA  *pStrNetFsSta = (STR_NETFS_STA *)strSysAppMsg.aucData;
                            NetFsProcess(pStrNetFsSta);
                        #endif
                        }
                        break;
                    default:
                        break;
                }
            }
        }
        else
        {
//            SYSHANDLE_ERROR("app rcv msg error or timeout\n");
            
        }
        
#if DEBUG_DM8127
        
        /* 十分钟打印一次时间，以便于分析系统什么时间出问题的 */
        uiClickSun++;
 //	dspEPMSnap(0);   //抓拍add 2015-11-12
        if(uiClickSun/(10*60))
        {
            if(0 == uiClickSun%(10*60))
            {
                //mainAppShowDate();  八达系统时间经常不对，去掉该处理，验证一下是哪里的问题
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
                camTemperatureSend();
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
            }
        }

        /* 隔四分钟对车检器进行一次心跳检查，若有一次心跳没来则判定车检器异常，以减小数据丢失 */
        if(uiClickSun/(4*60))
        {
            if(0 == uiClickSun%(4*60))
            {
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
                vehicleStaMonitor();
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
                g_iMonitorVehicleSum = g_iVehicleHeartSum;
            }
        }

        /* 每隔一分钟进行摄像机参数时间段判断，若有更新则重新摄像机参数 */
        if( uiClickSun/60 )
        {
            if(0 == uiClickSun%(60))
            {
                //SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);      //进来 打印      
                camCtlParamRunJudge(CAM_PARAM_MODE_NORMAL);
				//SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);//进来 打印
                #if defined(CYC200XZD)||defined(CYC500XZD)
                if(g_iSpiBusSta != 1)
                {
                    camCtlParamRunJudge(CAM_PARAM_MODE_FORCE);
                }
                #endif

                
                #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
                /* 判断双快门模式是否要切换 */
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);                
                kodarDubalShutParamSet(DUBAL_FLASH_NORMAL_SWITCH);
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
                #endif
                
#if defined(CYC500XLKW)
                //mainRebootAtTime();  //每隔一分钟检测一下是否需要在凌晨重启sipsdk
#else
				/* 定时重启 */
				timerRebootProcess();
#endif                
            }
        }

        /* 每隔半分钟进行电警时间段切换判断 */
        if( uiClickSun/30 )
        {
            if(0 == uiClickSun%(30))
            {
                //SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);  //进来 打印          
                #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
				SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
				iCheckFlag = dspPpTimerSwitchEnGet();
				if(iCheckFlag > 0)
                {
                	dspPpTimerSwitchDec(CAM_PARAM_MODE_NORMAL);
				}
                #else
                //SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);//进来 打印
                iCheckFlag = dspEpTimerSwitchEnGet();
                if(iCheckFlag > 0)
                {
                	dspEpTimerSwitchDec(CAM_PARAM_MODE_NORMAL);
				}
				//SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);//进来 打印
                #endif
            }
        }
        
        /* 隔十秒钟对摄像机闪光中断是否在变化进行判断 */
        if( uiClickSun/10 )
        {
            if(0 == uiClickSun%(10))
            {
                //SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);     //进来 打印       
                flashLampStaMonitor(1);
                recStaNMProc(0);
                vehicleNMStaProc();
				//SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__); //进来 打印
#ifdef MODULE_ONVIF
                //shamemTest();
#endif
		//add by forejolly 2015-8-13
		//printf("@@@@@@@@@@@@@@@@@@@@@@\n");
		//   dspEPMSnap(0);       /*10秒钟一次抓拍*/

            }
        }
#if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
        SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
        /* 卡口的频闪及闪光灯切换定时检测 */
        iCheckFlag = dspPPFillInLightParamGet();
        if( iCheckFlag > 0)
        {
            if( uiClickSun/(60*EP_LED_CHECK_CYCLE) )
            {
                if( 0 == uiClickSun%(60*EP_LED_CHECK_CYCLE) )
                {
                    dspPpFillInLightSwitchDec(CAM_PARAM_MODE_NORMAL);
                }
            }
            /* 由于刚开机时，调光算法不稳定，等待3分钟后，强制进行一次频闪及闪光的切换判断 */
            if( uiClickSun == POWER_ON_CHECK_CYCLE*60 )
            {
                syslog(LOG_ERR|LOG_USER, "开机三分钟后强制进行卡口频闪切换判断!\n");
                dspPpFillInLightSwitchDec(CAM_PARAM_MODE_MANUAL);
            }
        }
        SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
#endif

#if defined(CYC500XKW4C)||defined(CYC500XAW)
        /* 电警AC交流同步切换定时检测 */
       // SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);  //进来 打印        
        iCheckFlag = dspEPACSwitchParamGet(&iCheckCycle);
        if( iCheckFlag > 0)
        {
            if( uiClickSun/(60*iCheckCycle) )
            {
                if( 0 == uiClickSun%(60*iCheckCycle) )
                {
                	SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);                
                    dspEpACPhaseSwitchDec(CAM_PARAM_MODE_NORMAL);
                    SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
                }
            }

            /* 由于刚开机时，调光算法不稳定，等待3分钟后，强制进行一次白天及晚上的切换判断 */
            if( uiClickSun == POWER_ON_CHECK_CYCLE*60 )
            {
                syslog(LOG_ERR|LOG_USER, "开机三分钟后强制进行AC交流同步白天及晚上切换判断!\n");
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);                
                dspEpACPhaseSwitchDec(CAM_PARAM_MODE_NORMAL);
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
            }
        }

        /* 电警频闪及闪光灯切换定时检测 */
        iCheckFlag = dspEPFILightSmartSwitchGet();
        if( iCheckFlag > 0)
        {
            if( uiClickSun/(60*EP_LED_CHECK_CYCLE) )
            {
                if( 0 == uiClickSun%(60*EP_LED_CHECK_CYCLE) )
                {
	                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);                
                    dspEpFillInLightSwitchDec(CAM_PARAM_MODE_NORMAL);
                    SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
                }
            }
            /* 由于刚开机时，调光算法不稳定，等待3分钟后，强制进行一次频闪及闪光的切换判断 */
            if( uiClickSun == POWER_ON_CHECK_CYCLE*60 )
            {
                syslog(LOG_ERR|LOG_USER, "开机三分钟后强制进行电警频闪切换判断!\n");
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);                
                dspEpFillInLightSwitchDec(CAM_PARAM_MODE_MANUAL);
                SYSHANDLE_ERROR("%s %d, main is here!\n", __FUNCTION__,__LINE__);
            }
        }        
#endif

#endif /* DEBUG_DM8127 */
        
    }
    return 0;
}

int xcsRecInit(void)
{
    int nRet = 0;
#ifdef MODULE_REC
    if( g_cRecInitSta <= 0 )
    {
        nRet = recSysConfSet();
        if(0!=nRet)
        {
            printf("recInit failed.\n");
            g_cRecInitSta =0;
            SYSHANDLE_ERROR("recInit failed.\n");
            g_strVRSta.iRecEnable =0;
        }
        else
        {
            g_cRecInitSta =1;
            SYSHANDLE_ERROR("recInit ok.\n");
            g_strVRSta.iRecEnable =1;
        }
    }
    else
    {
        SYSHANDLE_ERROR("rec had Inited,do not need reinit.\n");
    }
#endif
    return nRet;
}

int xcsRecUninit(void)
{
#ifdef MODULE_REC
    if(g_cRecInitSta > 0)
    {
        g_cRecInitSta =0;
        SYSHANDLE_ERROR("recUnInit begin.\n");
        RecUnInit();
        SYSHANDLE_ERROR("recUnInit end.\n");
        if(g_pRecData != NULL)
        {
            free(g_pRecData);
            g_pRecData = NULL;
        }
    }
#endif
    return 0;
}

int tcpRecvDebugSwitch(int nLevel)
{
    g_Recv_Debug = nLevel;
    return 0;
}

    /* 初始化实时视频流模块 */
#ifdef MODULE_MMS
int initMmsSendMode(void)
{
	int nRet =0;
    nRet = MmsSendInit(MAX_SECTION_NUM+1, 8*1024*1024);
    if(0!=nRet)
    {
        g_cMmsInitSta =0;
        printf("MmsInit failed.\n");
        SYSHANDLE_ERROR("MmsInit failed.\n");
    }
    else
    {
        g_cMmsInitSta = 1;
        MmsDebugSwitch(0x0f);
        SYSHANDLE_ERROR("MmsSendInit ok.\n");
        printf("MmsSendInit ok.\n");
    }
    return nRet;
}
#endif

#if 1

int initCmosDev(void)
{
	int fd = 0 ;
	fd = open ("/dev/cmos", O_RDWR);
	if(fd >= 0)
	{
		SYSHANDLE_ERROR("cmos init success.\n");
        printf("cmos init success.\n");
	}
	else
	{
		SYSHANDLE_ERROR("cmos init failed.\n");
        printf("cmos init failed.\n");
	}
	return 1;
}

int main()
{
    int i =0;
    int nRet = 0;
    extSignalHand();

printf("~~~~~~~~~~ddd~~~~mymymyyyyy zf appppppppppppppp\n");
//char *argv[] = "VNF_QUALITY TRISTREAM TRIPLE_H264 TI2A AEWB 1080P_D1 H264 HIGH_SPEED1 H264 HIGH_SPEED2 MJPEG 80";
//char argc = 10;
//main_init(argc,argv);

SYSHANDLE_ERROR("~~~~~~2222~~~~~~~~~~~~~~~~~mymymyyyyy~~~~~~~~~mymymyyyyy~~~zf appppppppppppppp\n");
    hardWareVerGet();
    
    memset(g_EPLaneSwitchInfo, 0, sizeof(g_EPLaneSwitchInfo));
    memset(g_PPLaneSwitchInfo, 0, sizeof(g_PPLaneSwitchInfo));
    memset(g_bForceRed, 0, sizeof(g_bForceRed));
    memset(g_jpegSnapCountInfo, 0, sizeof(g_jpegSnapCountInfo));
    memset(&g_strVRSta, 0, sizeof(g_strVRSta));

    memset(&g_strCXProInfo, 0, sizeof(g_strCXProInfo));
    
    for(i=0; i< MAX_VEHICLE_NUM; i++)
    {
        g_aiCoilSta[i] = MODE_SHUTDOWN_STA;
        g_aiLampSta[i] = MODE_SHUTDOWN_STA;
        g_aiVirtualSta[i] = MODE_SHUTDOWN_STA;
    }

    /* 初始化日志模块 */
#ifdef MODULE_MC_LOG
    nRet = mcLogInit(NULL);
    if(0!=nRet)
    {
        SYSHANDLE_ERROR("mcLogInit failed.\n");
        printf("mcLogInit failed.\n");
    }
    else
    {
        SYSHANDLE_ERROR("mcLogInit success.\n");
        printf("mcLogInit success.\n");
    }
#endif

    /* 启动时创建 pid 文件，以便 monitor 监控 */
    unlink(MAIN_PID_FILE_NAME);
    writeThreadInfo("main");
    
    /* 创建系统消息队列 */
    pSysManageQueue = msgQCreate(60, 1024, 0);
    if (pSysManageQueue == NULL )
    {
        printf("creat sys manage message Queue failed .\n");
        SYSHANDLE_ERROR("creat sys manage message Queue failed .\n");
        syslog(LOG_ERR|LOG_USER, "%s application  msgQCreat failed...", DVS_DEVICE_TYPE_NAME);
        return -1;
    }
    else
    {
        SYSHANDLE_ERROR("sys msgQCreate success.\n");
        printf("sys msgQCreate success.\n");
    }
    mcLog(LMT_SYSCALL, LT_INFO, "%s application start ...,softVer:%s,hardVer:%d", DVS_DEVICE_TYPE_NAME, version_string,g_uiHardWVer);

#if defined(CYC200XZD)||defined(CYC500XZD)
    nRet = sem_init(&s_zebraSem, 0, 0);
    if ( -1 == nRet )
    {
        SYSHANDLE_ERROR("dspInit sem_init");
    }
    
    nRet = sem_init(&s_zebraRegCtlSem, 0, 1);
    if ( -1 == nRet )
    {
        SYSHANDLE_ERROR("dspInit sem_init");
    }

    nRet += devCtlInit();

    dm368SpiBusStaQuery();

    /* 初始化CCD控制模块 */
#ifdef MODULE_CCDCTL
    // DspReset();
    nRet = ccdRegCtlInit();
    if(0!=nRet)
    {
        printf("ccdCtlModule init failed.\n");
        g_cCCDCtlSta = 0;
        SYSHANDLE_ERROR("ccdCtlModule init failed.\n");
        syslog(LOG_ERR|LOG_USER, "ccdCtlModule init failed,exit xcs!\n");
		exit(0);
    }
    else
    {
        g_cCCDCtlSta = 1;
        dspCameraInfoShow();
        SYSHANDLE_ERROR("ccdCtlModule init ok.\n");
        printf("ccdCtlModule init ok.\n");
    }
#endif

#endif /* defined(CYC200XZD)||defined(CYC500XZD) */

#ifdef MODULE_CMOS

	initCmosDev();

#endif

#ifdef MODULE_ICX274_CCD
    // DspReset();
    nRet = icx274RegCtlInit();
    if(0!=nRet)
    {
        printf("icx274CcdCtl init failed.\n");
        g_cCCDCtlSta = 0;
        SYSHANDLE_ERROR("icx274CcdCtl init failed.\n");
        syslog(LOG_ERR|LOG_USER, "icx274ccdCtl init failed,exit xcs!\n");
        exit(0);
    }
    else
    {
        g_cCCDCtlSta = 1;
        dspCameraInfoShow();
        SYSHANDLE_ERROR("sys icx274 ccdCtlModule init ok.\n");
        printf("sys icx274 ccdCtlModule init ok.\n");
    }
#endif

	dspCameraInfoShow();

    /* 初始化系统配置文件模块 */
#ifdef MODULE_SYS_CONF
    nRet = SysConfInit();
    if(0!=nRet)
    {
        printf("confInit failed.\n");
        SYSHANDLE_ERROR("confInit failed.\n");
        syslog(LOG_ERR|LOG_USER, "sysconf init failed,exit xcs!\n");
        exit(0);
    }
    else
    {
        debugShellLogSet();
    
        SYSHANDLE_ERROR("sys confInit success.\n");
        printf("sys confInit success.\n");
    #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
        ppGetUsedLaneSum();
    #elif defined(CYC500XKW4C)||defined(CYC500XAW)
        epGXcsProVerGet();
        epGetUsedLaneSum();
    #endif
        sysCamTimeUpdate();

        redLightDelaySecGet();

        dspEPRLDecModeSet();
    }
#endif

/* 创建交通灯异常处理队列和线程 */
    dctlEPLoadRedInfo(OTHER_SET_MODE, 0);
    trafficLMInit();

    /* 读取实时时钟，并设置系统时间 */
    unsigned int ulRtc = 0;
    nRet = GetRtc(&ulRtc);
    SysSetTime(ulRtc, false);


/* 初始化FTP模块 */
#ifdef MODULE_FTP_SEND 
    nRet = ftpSendInit();
    if(0!=nRet)
    {
        printf("ftpInit failed.\n");
        g_cFtpInitSta =0;
        SYSHANDLE_ERROR("ftpInit failed.\n");
    }
    else
    {
        g_cFtpInitSta =1;
        SYSHANDLE_ERROR("ftpInit ok.\n");
        printf("ftpInit ok.\n");
    }
#endif

    /* 初始化XCS协议处理模块 */
#ifdef MODULE_XCS_PROTOCOL
    CTcpServer *pTcpServer = NULL;
    pTcpServer = (CTcpServer *)(new CTcpServer);
    if(NULL == pTcpServer)
    {
        printf("new pTcpServer failed.\n");
        SYSHANDLE_ERROR("new pTcpServer failed.\n");
    }
    else
    {
        nRet = pTcpServer->TcpServerOpen(5000, MAX_XCS_CLIENT_NUM, pSysManageQueue);
        if(0!=nRet)
        {
            SYSHANDLE_ERROR("TcpServerOpen failed.\n");
            printf("TcpServerOpen failed.\n");
        }
        else
        {
            SYSHANDLE_ERROR("TcpServerOpen ok.\n");
            printf("TcpServerOpen ok.\n");
            nRet = DataReportInit();
            if(0!=nRet)
            {
                g_cXcsInitSta =0;
                SYSHANDLE_ERROR("TcpDataReport failed.\n");
            }
            else
            {
                g_cXcsInitSta =1;
                SYSHANDLE_ERROR("TcpDataReport ok.\n");
            }
        }
    }
#endif    

/* 为了解决rec模块启动时间过久导致monitor进程监控xcs通信状态误判为异常，导致系统重启问题，所以
   该模块放到前面来初始化 */
#ifdef MODULE_CONF_SERVER
    CDbgServer *pDbgServer = NULL;
    pDbgServer = new CDbgServer;
    if (NULL == pDbgServer)
    {
        printf("dbg server Init failed.\n");
        SYSHANDLE_ERROR("dbg server Init failed.\n");
        syslog(LOG_ERR|LOG_USER, "dbgServer malloc failed,can`t print ver!\n");
    }
    else
    {
        SYSHANDLE_ERROR("CDbgServer success.\n");
        pDbgServer->DbgServerOpen(DBG_LISTEN_NAME);
    }
#endif


#ifdef MODULE_NM
    SYS_NM_CONF_INFO strNMConfInfo;
    memset(&strNMConfInfo, 0, sizeof(strNMConfInfo));
    nRet = SysConfGet(e_NM_CONF, &strNMConfInfo, 0);
    if( 0 == nRet)
    {
        struct in_addr in;
        char szTemp[128];

        if( strNMConfInfo.cEnable )
        {
            memset(szTemp, 0, sizeof(szTemp));
            in.s_addr = htonl(strNMConfInfo.uiNMSerIp);
            sprintf(szTemp,"%s", inet_ntoa(in));
            nRet = nMModuleInit(szTemp, strNMConfInfo.usNMPort);
            if(0!=nRet)
            {
                printf("nm init failed.\n");
                SYSHANDLE_ERROR("nm init failed.\n");
            }
            else
            {
                SYSHANDLE_ERROR("nm init ok.ip:%s,port:%d\n",szTemp, strNMConfInfo.usNMPort);
                printf("nm init ok.\n");
            }
        }
        else
        {
            printf("nm not enable.\n");
            SYSHANDLE_ERROR("nm not enable.\n");
        }
        /* 配置默认网关 */
        if( strNMConfInfo.cDefaultGWEn )
        {
            memset(szTemp, 0, sizeof(szTemp));
            in.s_addr = htonl(strNMConfInfo.uiDefaultGWIp);
            sprintf(szTemp,"route add default gw %s", inet_ntoa(in));
            system(szTemp);
        }
    }
    else
    {
        SYSHANDLE_ERROR("get nm conf failed\n");
    }
    
#endif


    /* 初始化实时视频流模块 */
#ifdef MODULE_MMS
	initMmsSendMode();
#endif

    /* 初始化录像模块 */
#ifdef MODULE_REC
    xcsRecInit();
#endif

    /* 初始化电警录像模块 */
#ifdef MODULE_ALARMREC
    xcsAlarmRecInit();
#endif

#ifdef MODULE_ONVIF

    memShmAllocator();
    init_onvifSend();

    g_cOnvifVideoSta =1;
#endif

    /* 初始化DSP模块 */
#ifdef MODULE_DSP
    nRet = dspInit();
    if(0!=nRet)
    {
        g_cDspInitSta =0;
        printf("dspInitAll failed.\n");
        SYSHANDLE_ERROR("dspInitAll failed.\n");
        syslog(LOG_ERR|LOG_USER, "dsp init failed,exit xcs!\n");
        exit(0);
    }
    else
    {
        g_cDspInitSta =1;
        SYSHANDLE_ERROR("dspInit ok.\n");
        printf("dspInit ok.\n");
    }
#endif

	ulRtc = 0;
	nRet = GetRtc(&ulRtc);
	SysSetTime(ulRtc);

    /* 初始化外设控制模块 */
#ifdef MODULE_DEV_CTL
    nRet = dctlThreadProInit();
    #if !defined(CYC200XZD) && !defined(CYC500XZD)
    nRet += devCtlInit();
	#endif
	if(0!=nRet)
    {
        printf("devctlInit failed.\n");
        SYSHANDLE_ERROR("devctlInit failed.\n");
    }
    else
    {
    #if defined(CYC200MK)||defined(CYC200K3)||defined(CYC200K2)||defined(CYC200KT)||defined(CYC500K)||defined(CYC500KD)||defined(CYC500KKW2C)
	    /* 进入死循环前，先把频闪灯关闭一下 */
	    dspPpFillInLightSwitchDec(CAM_PARAM_MODE_FORCE);	

        dctlPPLoadSwitchInfo();
        dctlLoadPPCoil();
        dctlPPFlashConf(FLASH_NORMAL_SET_MODE, 0);
        
        dspVLFlashModeSet();
    #elif defined(CYC500XKW4C)||defined(CYC500XAW)
        /* add by wangb 2010-6-1 for 按照车速计算6合1最后三步时间间隔，
        注意：该函数必须放在dCtlLoadEPCoil()函数之前 */
        
	    /* 进入死循环前，先把频闪灯关闭一下 */
		dspEpFillInLightSwitchDec(CAM_PARAM_MODE_FORCE);
        
        dctlPPLoadSwitchInfo();
        dctlLoadPPCoil();
        dctlEPLoadRedInfo(ALL_LANE_SET_MODE, 0);
        dCtlLoadEPCoil();
        dctlEPFlashConf(FLASH_NORMAL_SET_MODE,0);
        
        dspVLFlashModeSet();

        /* 进行网络交通灯检测器的初始化 */
        _Net_Init();
        unsigned short usNetVehiclePort = netVehiclePortGet();
        initNetSocket("172.16.65.110", 5101, usNetVehiclePort, NET_TYPE_UDP);                                
        
    #endif
        SYSHANDLE_ERROR("devctlInit ok.\n");
        printf("devctlInit ok.\n");
    }
#endif


#ifdef  MODULE_CIFS
    nRet = NetFsCheckInit();
    if(0!=nRet)
    {
        printf("cifs check module init failed.\n");
        SYSHANDLE_ERROR("cifs check module init failed.\n");
    }
    else
    {
        SYSHANDLE_ERROR("cifs check module init ok.\n");
    }
#endif

#ifdef  MODULE_NTP
    nRet = NtpInit();
    if(0!=nRet)
    {
        printf("ntp init failed.\n");
        SYSHANDLE_ERROR("ntp init failed.\n");
    }
    else
    {
        SYSHANDLE_ERROR("ntp init ok.\n");
        printf("ntp init ok.\n");
    }
#endif

    /* 初始化ip地址扫描响应线程 */
    arpFunInit();

#ifdef MODULE_NETREC

    memShmAllocator();

	nRet = StorageInit();
    if(0!=nRet)
    {
        printf("StorageInit failed.\n");
        syslog(LOG_ERR|LOG_USER, "StorageInit failed!");
    }
    else
    {
   		g_cNetRecVideoSta =1;
   		dspEncStaCtl(0, g_cNetRecVideoSta);
        printf("StorageInit ok.\n");
        syslog(LOG_ERR|LOG_USER, "StorageInit ok!");
        
    }
#endif

    //mainAppShowDate();

    /* 在系统程序都起来后，把调试信息改为ERROR级别 */
    //DebugSwitch("ALL", 0x01);
    //nMDebugSwitch(0x0);
	 // DebugSwitch("ALL", 0x01);
    //nMDebugSwitch(0x0);

   // debugShellLogSet();
    
    /* 系统消息队列处理,while(1) */
    mainMsgQProc(pSysManageQueue);

    SYSHANDLE_ERROR("app thread quit begin.\n");

#ifdef MODULE_XCS_PROTOCOL    
    pTcpServer->TcpServerClose();
    delete pTcpServer;
    pTcpServer = NULL;
#endif

    pDbgServer->DbgServerClose();
    delete pDbgServer;
    pDbgServer = NULL;

#ifdef MODULE_CIFS
    NetFsUninit();
#endif

#ifdef MODULE_REC
    RecUnInit();
#endif

#if !defined(CYC500XZD)&&!defined(CYC200XZD)
    icx274RegCtlUnInit();
#endif

    MmsSendUninit();
    DataReportUninit();
    
    SysConfUninit();
    mcLogUnInit();
    NtpUninit();
    trafficLMUninit();
#ifdef MODULE_ALARMREC
    epRecModuleUnInit();
#endif
    exit(0);
}

#else
int main(void)
{
	do
	{
		sleep(1);
		printf("its is here!\n");
	}while(1);
	return 0;
}
#endif

#if 1

int SysHandleDebugSwitch(int nLevel)
{
    g_SysHandle_Debug = nLevel;
    return 0;
}



int ftpDebugSwitch(int nLevel)
{
    int nFlag =0;
    
    if(nLevel==1)
    {
        nFlag = DBG_ERR;//ERROR
    }
    else if(nLevel==2)
    {
        nFlag = DBG_WARN;//WARNING
    }
    else if(nLevel==3)
    {
        nFlag = DBG_INFO;//INFO
    }
    else if(nLevel==4)
    {
        nFlag = DBG_ALL;//ALL
    }
    else
    {
        nFlag = 0x00;//CLEAR
    }
    shell_print("ftp debug level is %d #############\n",nLevel);
    
    if(NULL != g_CPPFtp)
    {
        g_CPPFtp->FtpSwitchDbgLevel(nFlag);	// tangzl 0725
        shell_print("ftp passPort lib version is %s.\n", g_CPPFtp->FtpGetSoftWareVer());
    }
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    if(NULL != g_CEPFtp)
    {
        g_CEPFtp->FtpSwitchDbgLevel(nFlag);	// tangzl 0725
        shell_print("ftp epPort lib version is %s.\n", g_CEPFtp->FtpGetSoftWareVer());
    }
#endif
    return 0;
}

#if defined(CYC200XZD)||defined(CYC500XZD)
int ZebraDebugSwitch(int nLevel)
{
    g_zebra_debug_module = nLevel;
    return 0;
}
#endif

int DebugSwitch(char *szModule,int nLevel)
{
    int nFlag = 0;

    if(nLevel==1)
    {
        nFlag = 0x01;//ERROR
    }
    else if(nLevel==2)
    {
        nFlag = 0x02;//WARNING
    }
    else if(nLevel==3)
    {
        nFlag = 0x04;//INFO
    }
    else if(nLevel==4)
    {
        nFlag = 0xffff;//ALL
    }
    else
    {
        nFlag = 0x00;//CLEAR
    }
            
    if(0==strcmp(szModule,"ALL"))
    {
        mcDebugSwitch(nFlag);
        SysConfDebugSwitch(nFlag);
        
#ifdef MODULE_DEV_CTL
        DctlDebugSwitch(nFlag);
#endif

#ifdef MODULE_REC
        RecDebugSwitch(nFlag);  //wangb 会导致程序挂掉
#endif
#ifdef MODULE_ALARMREC
        epRecDebugSwitch(nFlag);
#endif
        DspAppDebugSwitch(nFlag);
        kjXcsDebugSwitch(nFlag);
        SysHandleDebugSwitch(nFlag);
        tcpRecvDebugSwitch(nFlag);
        nMDebugSwitch(nFlag);
        MmsDebugSwitch(nFlag);
#if defined(CYC500XKW4C)||defined(CYC500XAW)
        netSocketDebugSwitch(nFlag);
        netDctlDebugSwitch(nFlag);
#endif

#if !defined(CYC500XZD)&&!defined(CYC200XZD)
        icx274CtlDebugSwitch(nFlag);
#else
		ZebraDebugSwitch(nFlag);
#endif

#ifdef MODULE_NETREC
		StorageDebugSwitch(nFlag);
#endif

		if ( nFlag > DBG_ALL)
	    {
	        nFlag = DBG_ALL;
	    }
		ftpDebugSwitch(nFlag);
#ifdef MODULE_NTP
		ntpDebugLevelSet(nFlag);
#endif
#if 1
	    NET_DEV_GET_PRINT_LEVEL(0, nLevel);
	    NET_DEV_GET_PRINT_LEVEL(1, nLevel);
	    NET_DEV_GET_PRINT_LEVEL(2, nLevel);
	    NET_DEV_GET_PRINT_LEVEL(3, nLevel);
#endif
	}
	else if(0==strcmp(szModule,"LOG"))
	{
		mcDebugSwitch(nFlag);
	}
	else if(0==strcmp(szModule,"PROTO"))
	{
        kjXcsDebugSwitch(nFlag);
    }
    else if(0==strcmp(szModule,"CONF"))
    {
        SysConfDebugSwitch(nFlag);
    }
    else if(0==strcmp(szModule,"DCTL"))
    {
        DctlDebugSwitch(nFlag);
    }
#if defined(CYC500XKW4C)||defined(CYC500XAW)
    else if(0==strcmp(szModule,"NETSOCKET"))
    {
        netSocketDebugSwitch(nFlag);
    }
    else if(0==strcmp(szModule,"NETDCTL"))
    {
        netDctlDebugSwitch(nFlag);
    }
#endif
    else if(0==strcmp(szModule,"REC"))
    {
#ifdef MODULE_REC
        RecDebugSwitch(nFlag);
#endif
    }
    else if(0==strcmp(szModule,"ALARMREC"))
    {
#ifdef MODULE_ALARMREC
        epRecDebugSwitch(nFlag);
#endif
    }
    else if( 0== strcmp(szModule, "NETREC") )
    {
#ifdef MODULE_NETREC
		StorageDebugSwitch(nFlag);
#endif
    }
	else if(0==strcmp(szModule,"MMS"))
	{
		MmsDebugSwitch(nFlag);
	}
	else if(0==strcmp(szModule,"DSP"))
	{
		DspAppDebugSwitch(nFlag);
	}
	else if(0==strcmp(szModule,"SYS"))
	{
	    SysHandleDebugSwitch(nFlag);
	}
	else if(0==strcmp(szModule,"TCPRCV"))
	{
	    tcpRecvDebugSwitch(nFlag);
	}
	else if(0==strcmp(szModule,"NM"))
	{
	    nMDebugSwitch(nFlag);
	}
	else if(0==strcmp(szModule,"DSPFIRM"))
	{
	    NET_DEV_GET_PRINT_LEVEL(0, nFlag);
	}
	else if(0==strcmp(szModule,"M3VIDEO"))
	{
	    NET_DEV_GET_PRINT_LEVEL(1, nFlag);
	}
	else if(0==strcmp(szModule,"M3VPSS"))
	{
	    NET_DEV_GET_PRINT_LEVEL(2, nFlag);
	}
	else if(0==strcmp(szModule,"A8"))
	{
	    NET_DEV_GET_PRINT_LEVEL(3, nFlag);
	}
	else if(0==strcmp(szModule,"M3LogS"))
	{
	    NET_DEV_GET_PRINT_LEVEL(4, nFlag);
	}
#if MODULE_NTP
	else if(0==strcmp(szModule,"NTP"))
	{
		ntpDebugLevelSet(nFlag);
	}
#endif 

	else if(0==strcmp(szModule,"FTP"))
	{
	    if ( nFlag > DBG_ALL)
	    {
	        nFlag = DBG_ALL;
	    }
	    ftpDebugSwitch(nFlag);
	}
    else if(0==strcmp(szModule,"CCDCTL"))
    {

#if !defined(CYC500XZD)&&!defined(CYC200XZD)
        //icx274CtlDebugSwitch(nFlag);
#endif

    }
    else
    {
        SYSHANDLE_ERROR("usage : debug <module> [level] \n");
        SYSHANDLE_ERROR("\n");
        SYSHANDLE_ERROR("    module: ALL LOG CONF ALARM DCTL TCPRCV SEND DVSPH VRP REC MMS DSP SYS DSPLIB COM PROTO M3LogS A8 M3VIDEO M3VPSS DSPFIRM\n");            
        SYSHANDLE_ERROR("    level: 0 - NONE, 1 - ERROR, 2 - WARN, 3 - INFO, 4 - ALL\n");
        SYSHANDLE_ERROR("\n");
    }
    
    return 0;
}
#endif






