/******************************************************************************
 * SysConf.cpp:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.12, xcsworker Create
 * --------------------
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <errno.h> 

#include "SysConf.h"
#include "SysConfCheck.h"
#include "sysconf_define.h"

#include "SysHandle.h"
#include "cfgSock.h"

#if defined(CYC200XZD)||defined(CYC500XZD)
#include "zebraCtl.h"
#include "ccdCtl.h"
#else
#include "icx274CCDCtl.h"
#endif

extern cam_info_t HDConf;

#include "dsp.h"
 
// to concatenate a string
#define CONF_GET_STR(sect, key, defvalue)         "r " key "." sect " " defvalue
#define CONF_SET_STR(sect, key, setvalue)         "w " key "." sect " " setvalue

#define INI_FILE_EXIT_SEC "INI_FILE_EXIT"
#define INI_FILE_EXIT_KEY "ini_file_flag"
#define INI_FILE_EXIT_FLAG "OK"

typedef int (*cbParse)(char *string, int count, void *data);
int g_Sysconf_Debug = 0x0;//用于程序运行时动态打开关闭程序调试打印
static SYS_CONF_MODULE *g_sysConfModule = NULL;//全局配置结构指针

#if defined(CYC500XZD)||defined(CYC200XZD)

CAMERA_CTRL_CONF_INFO  DefaultStrCameraConfVal[MAX_CAM_GROUP_NUM][MAX_CAM_CTRL_NUM] = 
{
        {
        {0, 0.0050,  0.00005,  0.03,   2, 0, 0xff, 0xff},
        {1, 10.000,  0.0000,   0.00,   2, 0, 0xff, 0xff},
        {2, 1.6000,  -4.000,   1.200,  2, 0, 0xff, 0xff},
        {3, 4000.0,    0.00,   4000.0, 4, 0, 0xff, 0xff},
        {4, 6.2000,    0.00,   6.2000, 4, 0, 0xff, 0xff},
        {5, 0.9000,    0.00,   0.9000, 4, 0, 0xff, 0xff},
        {6, 2500.0,    0.00,   2500.0, 4, 0, 0xff, 0xff},
        {7, 110.00,    0.00,   100.00, 4, 0, 0xff, 0xff}
        },

        {
        {0, 0.0050,  0.00005,  0.03,   2, 1, 0xff, 0xff},
        {1, 10.000,  0.0000,   0.00,   2, 1, 0xff, 0xff},
        {2, 1.6000,  -4.000,   1.200,  2, 1, 0xff, 0xff},
        {3, 4000.0,    0.00,   4000.0, 4, 1, 0xff, 0xff},
        {4, 0.0000,    0.00,   0.0000, 4, 1, 0xff, 0xff},
        {5, 1.6000,    0.00,   1.6000, 4, 1, 0xff, 0xff},
        {6, 2500.0,    0.00,   2500.0, 4, 1, 0xff, 0xff},
        {7, 180.00,    0.00,   180.00, 4, 1, 0xff, 0xff}
        },

        {
        {0, 0.0050,  0.00005,  0.03,   2, 2, 0xff, 0xff},
        {1, 1.000,   0.0000,   0.00,   2, 2, 0xff, 0xff},
        {2, 0.8000,  -4.000,   1.200,  2, 2, 0xff, 0xff},
        {3, 4000.0,    0.00,   4000.0, 4, 2, 0xff, 0xff},
        {4, 0.0000,    0.00,   0.0000, 4, 2, 0xff, 0xff},
        {5, 0.9000,    0.00,   1.0000, 4, 2, 0xff, 0xff},
        {6, 2500.0,    0.00,   2500.0, 4, 2, 0xff, 0xff},
        {7, 110.00,    0.00,   110.00, 4, 2, 0xff, 0xff}
        },

        {
        {0, 0.0050,  0.00005,  0.03,   2, 3, 0xff, 0xff},
        {1, 10.000,  0.0000,   0.00,   2, 3, 0xff, 0xff},
        {2, 0.8000,  -4.000,   1.200,  3, 3, 0xff, 0xff},
        {3, 4000.0,    0.00,   4000.0, 4, 3, 0xff, 0xff},
        {4, 6.2000,    0.00,   2.0000, 4, 3, 0xff, 0xff},
        {5, 1.1000,    0.00,   1.0000, 4, 3, 0xff, 0xff},
        {6, 2500.0,    0.00,   2500.0, 4, 3, 0xff, 0xff},
        {7, 120.00,    0.00,   120.00, 4, 3, 0xff, 0xff}
        }

};

#else

#if NEW_PIC_ARITHC

CAMERA_CTRL_CONF_INFO  DefaultStrCameraConfVal[MAX_CAM_GROUP_NUM][MAX_CAM_CTRL_NUM] = 
{
        {
        {0, 0.005,    0.00001,  0.005, 2,0}, //日常模式 快门
        {1, 15.00,    6.0000,   6,     2,0}, //增益
        {2, 60,      60,       60,    4,0}, //平均灰度
        {3, 5,        0,        3,     3,0}, //自动白平衡
        {4, 5,        3,        3,     4,0}, //对比度
        {5, 5,        0,        3,     4,0}, //gamma
        {6, 4.0,      2,        4,     4,0}, //minScal
        {7, 5.0,      1,        3,     3,0}, //颜色校正
        {8, 5,        0.00,     3,     4,0}, //锐度     
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        0,     4,0}  //降噪
        },

        {
        {0, 0.005,    0.00001,  0.005, 2,0}, //背光模式 快门
        {1, 15.00,    6.0000,   6,     2,0}, //增益
        {2, 60,      60,       60,    4,0}, //平均灰度
        {3, 5,        0,        3,     3,0}, //自动白平衡
        {4, 5,        3,        3,     4,0}, //对比度
        {5, 5,        0,        3,     4,0}, //gamma
        {6, 4.0,      2.00,     4,     4,0}, //minScal
        {7, 5.0,      1.000,    3,     3,0}, //颜色校正
        {8, 5,        0.00,     3,     4,0}, //锐度   
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,      1.00,    4,     4,0}, //手动对焦
        {11, 4.0,      0,       0,     4,0} //降噪
        },

        {
        {0, 0.005,    0.00001,  0.005, 2,0}, //顺光模式 快门
        {1, 15.00,    6.0000,   6,     2,0}, //增益
        {2, 60,      60,       60,    4,0}, //平均灰度
        {3, 5,        0,        3,     3,0}, //自动白平衡
        {4, 5,        3,        3,     4,0}, //对比度
        {5, 5,        0,        5,     4,0}, //gamma
        {6, 4.0,      2.00,     4,     4,0}, //minScal
        {7, 5.0,      1.000,    3,     3,0}, //颜色校正
        {8, 5,        0.00,     3,     4,0},  //锐度   
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        0,     4,0} //降噪
        },

        {
        {0, 0.005,    0.00001,  0.005, 2,0}, //黄昏模式 快门
        {1, 15.00,    6.0000,   6,     2,0}, //增益
        {2, 60,      60,       60,    4,0}, //平均灰度
        {3, 125,      100,      134,   4,0}, //自动白平衡
        {4, 5,        3,        3,     4,0}, //对比度
        {5, 5,        0,        3,     4,0}, //gamma
        {6, 4.0,      2.00,     4,     4,0}, //minScal
        {7, 5.0,      1.000,    3,     3,0}, //颜色校正
        {8, 5,        0.00,     3,     4,0}, //锐度   
        {9,  2.600,    0.00,    1.8,   4,0}, //缩放
        {10, 4.0,      1.00,    4,     4,0}, //手动对焦
        {11, 4.0,      0,       0,     4,0}  //降噪
        }
};

#else

#if defined(CYC500XKW4C)||defined(CYC500XAW)
CAMERA_CTRL_CONF_INFO  DefaultStrCameraConfVal[MAX_CAM_GROUP_NUM][MAX_CAM_CTRL_NUM] = 
{
        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 2,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       100,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        },

        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 3,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       100,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        },

        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 3,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       100,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        },

        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 3,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       100,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        }

};

#else

CAMERA_CTRL_CONF_INFO  DefaultStrCameraConfVal[MAX_CAM_GROUP_NUM][MAX_CAM_CTRL_NUM] = 
{
        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 2,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       128,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        },

        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 3,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       128,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        },

        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 3,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       128,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        },

        {
        {0, 0.005,    0.00007,  0.005, 2,0}, //日常模式 快门
        {1, 6.00,     0.00,     6,     2,0}, //增益
        {2, 255,      0,       112,    4,0}, //平均灰度
        {3, 3,        0,        2,     4,0}, //降噪3D
        {4, 5,        3,        1,     4,0}, //宽动态
        {5, 5,        0,        2,     4,0}, //清晰度
        {6, 4.0,      2,        128,   4,0}, //对比度
        {7, 128,      80,       128,   4,0}, //亮度
        {8, 5,        0.00,     128,   4,0}, //饱和度
        {9,  2.600,   0.00,     1.8,   4,0}, //缩放
        {10, 4.0,     1.00,     4,     4,0}, //手动对焦
        {11, 4.0,     0,        3,     4,0}  //降噪2D
        }

};

#endif /* cyc500kkw2c */



#endif

#endif

static  int  LoadRtspConf(void);
static  int  LoadSysDefConf(void);
static  int  LoadRecConf(void);
static  int  LoadVideoConf(void);
static  int  LoadNetConf(void);
static  int  LoadPassPortOverallConf(void);
static  int  LoadPassPortVehicleConf(void);
static  int  LoadElecPolliceOverallConf(void);
static  int  LoadElecPolliceVehicleConf(void);
static  int  LoadVLConf(void);
static  int  LoadPassPortFtpConf(void);
static  int  LoadPassPortBackupsFtpConf(void);
static  int  LoadElecPoliceFtpConf(void);
static  int  LoadEpBackupsFtpConf(void);
static  int  LoadNtpConf(void);
static  int  LoadCamTimeRangeConf(void);
static  int  LoadEpTimeRangeConf(void);
static  int  LoadCamCtrlConf(void);
static  int  LoadPicNetHddConf(void);
static  int  LoadRecNetHddConf(void);
static  int  LoadDctlConf(void);
static  int  LoadPlatformConf(void);
static  int  LoadEp330Conf(void);
static  int  LoadAppModeConf(void);
static  int  LoadEpAssistRedLightConf(void);
static  int  LoadEpSOTNoEntryConf(void);
static 	int	 LoadNRMConf(void);
static  int  LoadConf(void);

static	int	 SaveNetConf(void);	
static	int	 SaveRtspConf(void);
static	int	 SaveSysDefConf(void);
static	int	 SaveVideoConf(void);		
static	int	 SaveRecConf(void);
static  int  SavePassPortOverallConf(void);
static  int  SavePassPortVehicleConf(void);
static  int  SaveElecPolliceOverallConf(void);
static  int  SaveElecPolliceVehicleConf(void);
static  int  SaveVLConf(void);
static  int  SavePassPortFtpConf(void);
static  int  SaveElecPoliceFtpConf(void);
static  int	 SaveEpBackupsFtpConf(void);
static  int  SaveNtpConf(void);
static  int  SaveCamTimeRangeConf(void);
static  int  SaveEPTimeRangeConf(void);
/* 由于摄像机配置有十几项，若每一项都保存flash将导致配置很慢，所以修改为
只有全部配置修改完成后才调用该函数保存到flash */
int  SaveCamCtrlConf(void);
static  int  SavePicNetHddConf(void);
static  int  SaveRecNetHddConf(void);
static  int  SaveDctlConf(void);
static  int  SavePlatformConf(void);
static  int  SaveEp330Conf(void);
static  int  SaveV3Conf(void);
static  int  SaveV2Conf(void);
static  int  SaveNMConf(void);
static  int  SaveAppModeConf(void);
static  int  SaveEPIrregInfoConf(void);
static  int  SaveFuncEnableConf(void);
static int   SaveEpVLoopStepIntervalConf(void);
static int   SaveEPLPRInfoConf(void);
static int   SaveEPAssistRedLightConf(void);
static int   SaveEPSOTNoEntryConf(void);
static int 	 SaveEpJpgOsdConf(void);
static int 	 SavePpJpgOsdConf(void);
static int   SavePPIrregInfoConf(void);
static int  SavePassPortBackupsFtpConf(void);
static int	SaveEPNewAddIrregInfoConf(void);

static int MakeDefaultConfFile(void);
static int DefaultSysConfValue(void);
static int SaveConfFile(void);
static int SavePpFuncEnConf(void);
static int SaveNRMConf(void);
static int SaveGB28181Conf(void);

/* 绿灯抓拍车道范围宽高必须定死 */
#define GREEN_LIGHT_WIDTH   1024
#define GREEN_LIGHT_HEIGHT  672


int SysConfDebugSwitch(int nLevel)
{
	g_Sysconf_Debug = nLevel;
	
	return 0;
}

int SysConfInit(void)
{
	int nRet = -1;
	
    int i =0;
    const char *pcEpGVer = NULL;
    char szDefault[256]={0};
    int iEPGVer =0;
    
	if(g_sysConfModule&&g_sysConfModule->bSysConfInit)
	{
		return 0;
	}
	g_sysConfModule = (SYS_CONF_MODULE *)(new int[sizeof(SYS_CONF_MODULE)/4 +1]);
	if(!g_sysConfModule)
	{
		CONF_DEBUG("%s:%d malloc() failed.\n",__FUNCTION__,__LINE__);
		syslog(LOG_ERR|LOG_USER, "%s,line:%d, malloc failed!\n", __FUNCTION__, __LINE__);
		goto error_ret;
	}

	memset(g_sysConfModule,0x00,sizeof(SYS_CONF_MODULE));
	
	g_sysConfModule->bSysConfInit = 0;

	nRet = sem_init(&(g_sysConfModule->semSaveOP),0,1) ;
	if(nRet!=0)
	{
		CONF_DEBUG("%s:%d sem_init() failed.\n",__FUNCTION__,__LINE__);
		goto error_ret;
	}
    nRet = cfgSockInit();
    if ( 0 != nRet  )
    {
		syslog(LOG_ERR|LOG_USER, "%s,line:%d, cfgSockInit ret:%d failed!\n", __FUNCTION__, __LINE__, nRet);    
        goto error_ret;
    }
    const char *cfgRecv;

    cfgRecv = cfgSockGetValue(INI_FILE_EXIT_SEC, INI_FILE_EXIT_KEY, "NO");
    if ( NULL == cfgRecv )
    {
        syslog(LOG_ERR|LOG_USER, "%s,line:%d, cfgSockGetValue return failed!\n", __FUNCTION__, __LINE__);
        goto error_ret;
    }

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", XCS_PRO_EPG_VER);
    pcEpGVer =cfgSockGetValue(sys_ini_section[e_ELEC_POLICE_OVERALL_CONF],\
                            ElecPolicOverallConfInfoKey[e_eaXcsProVer],\
                            szDefault);
                            
    iEPGVer = atoi(pcEpGVer);
    
	if (0 == strncmp(cfgRecv, INI_FILE_EXIT_FLAG, 2))
	{
		LoadConf();

		/*Start of ITS on 2011-6-20 14:35 2.0.0.1，必须在loadconf后进行以下函数调用 */
		if( iEPGVer <= XCS_PRO_EPG_VER )
		{
		    for(i =0; i< MAX_VEHICLE_NUM; i++)
		    {
		        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cDetectType = \
		        g_sysConfModule->s_ElecPoliceOverallConfInfo.cDetectType;

		        if( GROUD_LOOP == g_sysConfModule->s_ElecPoliceOverallConfInfo.cDetectType)
		        {
		            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchFlag = 1;
		        }
		        else
		        {
		            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchFlag = 0;
		        }

		        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usFlashLightType = \
		        g_sysConfModule->s_ElecPoliceOverallConfInfo.cFlashCtlMode;

		        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarDistance = \
		        g_sysConfModule->s_ElecPoliceOverallConfInfo.cCarDistance;

		        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cFrameInterval = \
		        g_sysConfModule->s_ElecPoliceOverallConfInfo.cFrameInterval;
		    }
		}
		/*End of ITS on 2011-6-20 14:35 2.0.0.1*/
		
		/* 为了防止新加的配置在debugShell里面读不到，所以先存一下 */
		nRet = SaveConfFile();
		sleep(2);
    }
    else
    {
        sleep(10);
		/* 延迟5秒后再尝试一次 */
        cfgRecv = cfgSockGetValue(INI_FILE_EXIT_SEC, INI_FILE_EXIT_KEY, "NO");
    	if (0 != strncmp(cfgRecv, INI_FILE_EXIT_FLAG, 2))
    	{
    		nRet = MakeDefaultConfFile();
    		if (0 != nRet)
    		{
    		    syslog(LOG_ERR|LOG_USER, "%s,line:%d, make default conf ini file failed!\n", __FUNCTION__, __LINE__);
    			CONF_ERROR("ERROR:默认配置文件生成失败.\n");
    			goto error_ret;
    		}
    		else
    		{
    		    syslog(LOG_ERR|LOG_USER, "%s,line:%d, make default conf ini file ok!\n", __FUNCTION__, __LINE__);
    		}
        }
		else
		{
			LoadConf();

        	/*Start of ITS on 2011-6-20 14:35 2.0.0.1，必须在loadconf后进行以下函数调用 */
        	if( iEPGVer <= XCS_PRO_EPG_VER )
        	{
        	    for(i =0; i< MAX_VEHICLE_NUM; i++)
        	    {
        	        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cDetectType = \
        	        g_sysConfModule->s_ElecPoliceOverallConfInfo.cDetectType;

        	        if( GROUD_LOOP == g_sysConfModule->s_ElecPoliceOverallConfInfo.cDetectType)
        	        {
        	            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchFlag = 1;
        	        }
        	        else
        	        {
        	            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchFlag = 0;
        	        }

        	        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usFlashLightType = \
        	        g_sysConfModule->s_ElecPoliceOverallConfInfo.cFlashCtlMode;

        	        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarDistance = \
        	        g_sysConfModule->s_ElecPoliceOverallConfInfo.cCarDistance;

        	        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cFrameInterval = \
        	        g_sysConfModule->s_ElecPoliceOverallConfInfo.cFrameInterval;
        	    }
        	}
        	/*End of ITS on 2011-6-20 14:35 2.0.0.1*/
        	
			/* 为了防止新加的配置在debugShell里面读不到，所以先存一下 */
			nRet = SaveConfFile();
		}
    }
	g_sysConfModule->bSysConfInit = 1;
	return 0;

error_ret:

	if(g_sysConfModule)
	{	
//		delete g_sysConfModule->psystemIni;
//		g_sysConfModule->psystemIni = NULL;
//		delete g_sysConfModule->pdvsIni;
//		g_sysConfModule->pdvsIni = NULL;

        CONF_ERROR("ERROR:配置模块初始化失败.\n");
		sem_destroy(&(g_sysConfModule->semSaveOP));
		free(g_sysConfModule);
		g_sysConfModule = NULL;
	}

	CONF_ERROR("ERROR:系统配置模块初始化失败.\n");
	
	return (-1);
	
	return 0;
}

int SysConfUninit(void)
{
    int nRet =0;
    nRet = SaveConfFile();
	if(g_sysConfModule)
	{	
		sem_wait(&(g_sysConfModule->semSaveOP));
		sem_post(&(g_sysConfModule->semSaveOP));
		sem_destroy(&(g_sysConfModule->semSaveOP));
		free(g_sysConfModule);
		g_sysConfModule = NULL;
	}

	CONF_INFO("INFO:系统配置模块反初始化成功.\n");
	
	return nRet;
}

const char * GetProductSerialNo()
{
    return cfgSockGetValue("sn", NULL, "noSetSn");
}



int SysConfGet(CONF_TYPE nConfType,void *pConf,int index)
{
    unsigned char cGroup =0;
	int nRet = 0;
    void *ptr;

	if(NULL==pConf)
	{
		return (-1);
	}
	
	if((NULL==g_sysConfModule)||(0==g_sysConfModule->bSysConfInit))
    {   
		return (-1);
    }
	
	switch(nConfType)
	{
	    case e_APPMODE_CONF:
	        ptr = (int *)(pConf);
	        memcpy(ptr,&g_sysConfModule->s_AppMode,sizeof(int));
	        break;	    
		case e_NET_CONF:
            ptr = (NET_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_NetConfInfo,sizeof(NET_CONF_INFO));
			break;	
            
		case e_RTSP_CONF:
			ptr = (RTSP_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_RtspConfInfo,sizeof(RTSP_CONF_INFO));
			break;
            
		case e_SYS_DEF_CONF:
			ptr = (SYS_DEF_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_SysConfInfo,sizeof(SYS_DEF_CONF_INFO));
			break;	
            
		case e_VIDEO_CONF:
            SYS_ASSERT(0,TOTAL_CHANNEL_NUM);
			ptr = (VIDEO_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_VideoConfInfo[index],sizeof(VIDEO_CONF_INFO));
			break;	
            
		case e_REC_CONF:
            SYS_ASSERT(0,TOTAL_CHANNEL_NUM);
			ptr = (REC_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_RecConfInfo[index],sizeof(REC_CONF_INFO));
			break;
        case e_PASSPORT_OVERALL_CONF:
            ptr = (PASSPORT_OVERALL_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_PassPortOverallConfInfo,sizeof(PASSPORT_OVERALL_CONF_INFO));
            break;
            
        case e_PASSPORT_VEHICLE_CONF:
            SYS_ASSERT(0,MAX_VEHICLE_NUM);
            ptr = (PORT_VEHICLE_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_PortVehicleConfInfo[index],sizeof(PORT_VEHICLE_CONF_INFO));
            break;
            
        case e_ELEC_POLICE_OVERALL_CONF:
            ptr = (ELEC_POLICE_OVERALL_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_ElecPoliceOverallConfInfo,sizeof(ELEC_POLICE_OVERALL_CONF_INFO));
            break;
            
        case e_ELEC_POLICE_VEHICLE_CONF:
            SYS_ASSERT(0,MAX_VEHICLE_NUM);
            ptr = (ELEC_POLICE_VEHICLE_CONF_INFO *)(pConf);
			memcpy(ptr,&g_sysConfModule->s_ElecPoliceVehicleConfInfo[index],sizeof(ELEC_POLICE_VEHICLE_CONF_INFO));
            break;
            
        case e_VL_CONF:
            SYS_ASSERT(0,MAX_VEHICLE_NUM);
            ptr = (VL_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_VlConfInfo[index],sizeof(VL_CONF_INFO));
            break;
            
        case e_PASSPORT_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_PortFtpConfInfo,sizeof(FTP_CONF_INFO));
            break;
            
        case e_ELEC_POLICE_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_ElecPoliceFtpConfInfo,sizeof(FTP_CONF_INFO));
            break;
            
        case e_NTP_CONF:
             ptr = (NTP_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_NtpConfInfo,sizeof(NTP_CONF_INFO));
            break;
            
        case e_PIC_NET_HDD_CONF:
            ptr = (NET_HDD_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_PicNetHddConfInfo,sizeof(NET_HDD_CONF_INFO));
            break;
            
        case e_REC_NET_HDD_CONF:
            ptr = (NET_HDD_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_RecNetHddConfInfo,sizeof(NET_HDD_CONF_INFO));
            break;
        case e_CAMERA_TIME_RANGE:
            SYS_ASSERT(0,MAX_CAM_GROUP_NUM);
            ptr = (CAMERA_TIME_RANGE_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_CamTimeRange[index],sizeof(CAMERA_TIME_RANGE_INFO));
            break;
        case e_CAMERA_CTRL_CONF:
            cGroup = index/MAX_CAM_CTRL_NUM;
            index %= MAX_CAM_CTRL_NUM;
            ptr = (CAMERA_CTRL_CONF_INFO*)(pConf);
			memcpy(ptr, &g_sysConfModule->s_CameraCtrlConfInfo[cGroup][index],sizeof(CAMERA_CTRL_CONF_INFO));
            //dspShowFeature( (CAMERA_CTRL_CONF_INFO*)ptr );
            break;
            
        case e_DCTL_CONF:
            SYS_ASSERT(0,MAX_DCTL_NUM);
            ptr = (DCTL_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_DctlConfInfo[index],sizeof(DCTL_CONF_INFO));
            break;
        case e_PLATFORM_CONF:
            ptr = (PLATFORM_330_CONF_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_Platform330ConfInfo,sizeof(PLATFORM_330_CONF_INFO));
            break;
        case e_V3_CONF:
            ptr = (PLATFORM_V3_CONF_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_PlatformV3ConfInfo, sizeof(PLATFORM_V3_CONF_INFO));
            break;
        case e_NM_CONF:
            ptr = (SYS_NM_CONF_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_sysNMConfInfo, sizeof(SYS_NM_CONF_INFO));
            break;
        case e_SYS_FUNC_ENABLE_CONF:
            ptr = (SYS_FUNC_ENABLE_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_FuncEnableInfo, sizeof(SYS_FUNC_ENABLE_INFO));
            break;
        case e_EP_RANGE_TIME_CONF:
            ptr = (EP_TIME_RANGE_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_EpTimeRange[index][0], EP_FORBID_MAX_DIR*sizeof(EP_TIME_RANGE_INFO));
            break;
        case e_EP_V_LOOP_STEP_INVER:
            ptr = (EP_V_LOOP_STEP__INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_EpVLoopStepInfo, sizeof(EP_V_LOOP_STEP__INFO));
            break;
        case e_EP_330_CONF:
            ptr = (PLATFORM_330_CONF_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_EpPlatform330ConfInfo, sizeof(PLATFORM_330_CONF_INFO));
            break;
        case e_V2_CONF:
            ptr = (PLATFORM_V3_CONF_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_PlatformV2ConfInfo, sizeof(PLATFORM_V3_CONF_INFO));
            break;
		case e_EP_ASSIST_REDLIGHT_CONF:
            ptr = (EP_ASSIST_RED_LIGHT_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_EpAssistRedLight[index][0], EP_ASSIST_RED_LIGHT_NUM*sizeof(EP_ASSIST_RED_LIGHT_INFO));
			break;
        case e_EP_IRREG_INFO_CONF:
            ptr = (EP_IRREG_CONF_INFO *)(pConf);
            memcpy(ptr, &g_sysConfModule->s_EpIrregConfInfo, sizeof(EP_IRREG_CONF_INFO));
            break;
        case e_EP_LPR_INFO_CONF:
            ptr = (EP_LPR_CONF_INFO *)(pConf);
            memcpy(ptr, &g_sysConfModule->s_EpLprConfInfo, sizeof(EP_LPR_CONF_INFO));
            break;
        case e_PP_FUNC_ENABLE_CONF:
            ptr = (PP_FUNC_ENABLE_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_PpFuncConfInfo, sizeof(PP_FUNC_ENABLE_INFO));
            break;
		case e_EP_SOT_NOENTRY_CONF:
            ptr = (EP_STOP_ON_TRAFFIC_NO_ENTRY*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_EpSOTNoEntry[index], sizeof(EP_STOP_ON_TRAFFIC_NO_ENTRY));
			break;
		case e_PP_JPG_OSD_CONF:
            ptr = (BAYONET_JPG_OSD_CONF *)(pConf);
			memcpy(ptr, &g_sysConfModule->s_PpJpgOsdConf, sizeof(BAYONET_JPG_OSD_CONF));
			break;
		case e_EP_JPG_OSD_CONF:
            ptr = (EP_JPG_OSD_CONF *)(pConf);
			memcpy(ptr, &g_sysConfModule->s_EpJpgOsdConf, sizeof(EP_JPG_OSD_CONF));
			break;
        case e_PP_IRREG_INFO_CONF:
            ptr = (PP_IRREG_CONF_INFO *)(pConf);
            memcpy(ptr, &g_sysConfModule->s_PpIrregConfInfo, sizeof(PP_IRREG_CONF_INFO));
            break;
        case e_PASSPORT_BACKUPS_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_PortBackupsFtpConfInfo,sizeof(FTP_CONF_INFO));
            break; 			
		case e_NET_REC_MANAGE_CONF:
            ptr = (NET_REC_MANAGE_INFO *)(pConf);
            memcpy(ptr, &g_sysConfModule->s_NrmConfInfo, sizeof(NET_REC_MANAGE_INFO));
            break;  
        case e_EP_BACKUPS_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(ptr,&g_sysConfModule->s_epBackupsFtpConfInfo,sizeof(FTP_CONF_INFO));
            break;
        case e_EP_NEWADD_IRREG_INFO_CONF:
            ptr = (EP_NEW_ADD_IRREG_CONF_INFO *)(pConf);
            memcpy(ptr, &g_sysConfModule->s_EpNewAddIrregConfInfo, sizeof(EP_NEW_ADD_IRREG_CONF_INFO));
            break;
        case e_GB28181_CONF:
            ptr = (GB28181_CONF_INFO *)(pConf);
            memcpy(ptr,&g_sysConfModule->s_Gb28181ConfInfo, sizeof(GB28181_CONF_INFO));
            break;
		default:
			nRet= -1;
			break;
	}
	
	return nRet;
}


int SysConfSet(CONF_TYPE nConfType,void *pConf,int index)
{
    unsigned char cGroup =0;
	int nRet = 0;
    void *ptr;

   // sem_wait(&(g_sysConfModule->semSaveOP));
    switch(nConfType)
	{
	    case e_APPMODE_CONF:
	        ptr = (int *)(pConf);
	        memcpy(&g_sysConfModule->s_AppMode,ptr,sizeof(int));
	        SaveAppModeConf();
	        break;	    
	        
		case e_NET_CONF:
            ptr = (NET_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_NetConfInfo,ptr,sizeof(NET_CONF_INFO));
			SaveNetConf();
			break;	
            
		case e_RTSP_CONF:
            ptr = (RTSP_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_RtspConfInfo,ptr,sizeof(RTSP_CONF_INFO));
			SaveRtspConf();
			break;
            
		case e_SYS_DEF_CONF:
            ptr = (SYS_DEF_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_SysConfInfo,ptr,sizeof(SYS_DEF_CONF_INFO));
			SaveSysDefConf();
			break;	
            
		case e_VIDEO_CONF:
            SYS_ASSERT(0, TOTAL_CHANNEL_NUM);
            ptr = (VIDEO_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_VideoConfInfo[index],ptr,sizeof(VIDEO_CONF_INFO));
			SaveVideoConf();
			break;	
            
		case e_REC_CONF:
            SYS_ASSERT(0, TOTAL_CHANNEL_NUM);
            ptr = (REC_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_RecConfInfo[index],ptr,sizeof(REC_CONF_INFO));
			SaveRecConf();
			break;
            
        case e_PASSPORT_OVERALL_CONF:
            ptr = (PASSPORT_OVERALL_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_PassPortOverallConfInfo,ptr,sizeof(PASSPORT_OVERALL_CONF_INFO));
            SavePassPortOverallConf();
            break;
            
        case e_PASSPORT_VEHICLE_CONF:
            SYS_ASSERT(0, MAX_VEHICLE_NUM);
            ptr = (PORT_VEHICLE_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_PortVehicleConfInfo[index],ptr,sizeof(PORT_VEHICLE_CONF_INFO));
            SavePassPortVehicleConf();
            break;
            
        case e_ELEC_POLICE_OVERALL_CONF:
            ptr = (ELEC_POLICE_OVERALL_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_ElecPoliceOverallConfInfo,ptr,sizeof(ELEC_POLICE_OVERALL_CONF_INFO));
            SaveElecPolliceOverallConf();
            break;
            
        case e_ELEC_POLICE_VEHICLE_CONF:
            SYS_ASSERT(0, MAX_VEHICLE_NUM);
            ptr = (ELEC_POLICE_VEHICLE_CONF_INFO *)(pConf);
			memcpy(&g_sysConfModule->s_ElecPoliceVehicleConfInfo[index],ptr,sizeof(ELEC_POLICE_VEHICLE_CONF_INFO));
            SaveElecPolliceVehicleConf();
            break;
            
        case e_VL_CONF:
            SYS_ASSERT(0, MAX_VEHICLE_NUM);
            ptr = (VL_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_VlConfInfo[index],ptr,sizeof(VL_CONF_INFO));
            SaveVLConf();
            break;
            
        case e_PASSPORT_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_PortFtpConfInfo,ptr,sizeof(FTP_CONF_INFO));
            SavePassPortFtpConf();
            break;
            
        case e_ELEC_POLICE_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_ElecPoliceFtpConfInfo,ptr,sizeof(FTP_CONF_INFO));
            SaveElecPoliceFtpConf();
            break;

        case e_EP_BACKUPS_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_epBackupsFtpConfInfo,ptr,sizeof(FTP_CONF_INFO));
            SaveEpBackupsFtpConf();
            break;
            
        case e_NTP_CONF:
            ptr = (NTP_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_NtpConfInfo,ptr,sizeof(NTP_CONF_INFO));
            SaveNtpConf();
            break;
            
        case e_PIC_NET_HDD_CONF:
            ptr = (NET_HDD_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_PicNetHddConfInfo,ptr,sizeof(NET_HDD_CONF_INFO));
            SavePicNetHddConf();
            break;
            
        case e_REC_NET_HDD_CONF:
             ptr = (NET_HDD_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_RecNetHddConfInfo,ptr,sizeof(NET_HDD_CONF_INFO));
            SaveRecNetHddConf();
            break;
        case e_CAMERA_TIME_RANGE:
            SYS_ASSERT(0,MAX_CAM_GROUP_NUM);
            ptr = (CAMERA_TIME_RANGE_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_CamTimeRange[index],ptr,sizeof(CAMERA_TIME_RANGE_INFO));
            SaveCamTimeRangeConf();
            break;
        case e_CAMERA_CTRL_CONF:
            cGroup = index/MAX_CAM_CTRL_NUM;
            index %= MAX_CAM_CTRL_NUM;

            ptr = (CAMERA_CTRL_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_CameraCtrlConfInfo[cGroup][index],ptr,sizeof(CAMERA_CTRL_CONF_INFO));
            //SaveCamCtrlConf();
            break;
            
        case e_DCTL_CONF:
            SYS_ASSERT(0, MAX_DCTL_NUM);
            ptr = (DCTL_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_DctlConfInfo[index],ptr,sizeof(DCTL_CONF_INFO));
            SaveDctlConf();
            break;
        case e_PLATFORM_CONF:
            ptr = (PLATFORM_330_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_Platform330ConfInfo,ptr,sizeof(PLATFORM_330_CONF_INFO));
            SavePlatformConf();
            break;
        case e_V3_CONF:
            ptr = (PLATFORM_V3_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_PlatformV3ConfInfo, ptr, sizeof(PLATFORM_V3_CONF_INFO));
            SaveV3Conf();
            break;
        case e_NM_CONF:
            ptr = (SYS_NM_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_sysNMConfInfo, ptr, sizeof(SYS_NM_CONF_INFO));
            SaveNMConf();
            break;
        case e_SYS_FUNC_ENABLE_CONF:
            ptr = (SYS_FUNC_ENABLE_INFO *)(pConf);
            memcpy(&g_sysConfModule->s_FuncEnableInfo, ptr, sizeof(SYS_FUNC_ENABLE_INFO));
            SaveFuncEnableConf();
            break;
        case e_EP_RANGE_TIME_CONF:
            ptr = (EP_TIME_RANGE_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_EpTimeRange[index][0], ptr, EP_FORBID_MAX_DIR*sizeof(EP_TIME_RANGE_INFO));
            SaveEPTimeRangeConf();
            break;
        case e_EP_V_LOOP_STEP_INVER:
            ptr = (EP_V_LOOP_STEP__INFO *)(pConf);
            memcpy(&g_sysConfModule->s_EpVLoopStepInfo, ptr, sizeof(EP_V_LOOP_STEP__INFO));
            SaveEpVLoopStepIntervalConf();
            break;
        case e_EP_330_CONF:
            ptr = (PLATFORM_330_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_EpPlatform330ConfInfo, ptr, sizeof(PLATFORM_330_CONF_INFO));
            SaveEp330Conf();
            break;
        case e_V2_CONF:
            ptr = (PLATFORM_V3_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_PlatformV2ConfInfo, ptr, sizeof(PLATFORM_V3_CONF_INFO));
            SaveV2Conf();
            break;
		case e_EP_ASSIST_REDLIGHT_CONF:
            ptr = (EP_ASSIST_RED_LIGHT_INFO*)(pConf);
            memcpy(&g_sysConfModule->s_EpAssistRedLight[index][0], ptr, EP_ASSIST_RED_LIGHT_NUM*sizeof(EP_ASSIST_RED_LIGHT_INFO));
            SaveEPAssistRedLightConf();
            break;
        case e_EP_IRREG_INFO_CONF:
            ptr = (EP_IRREG_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_EpIrregConfInfo, ptr, sizeof(EP_IRREG_CONF_INFO));
            SaveEPIrregInfoConf();
            break;
        case e_EP_LPR_INFO_CONF:
            ptr = (EP_LPR_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_EpLprConfInfo, ptr, sizeof(EP_LPR_CONF_INFO));
            SaveEPLPRInfoConf();
            break;
        case e_PP_FUNC_ENABLE_CONF:
            ptr = (PP_FUNC_ENABLE_INFO *)(pConf);
            memcpy(&g_sysConfModule->s_PpFuncConfInfo, ptr, sizeof(PP_FUNC_ENABLE_INFO));
            SavePpFuncEnConf();
            break;
		case e_EP_SOT_NOENTRY_CONF:
            ptr = (EP_STOP_ON_TRAFFIC_NO_ENTRY*)(pConf);
            memcpy(&g_sysConfModule->s_EpSOTNoEntry[index], ptr, sizeof(EP_STOP_ON_TRAFFIC_NO_ENTRY));
            SaveEPSOTNoEntryConf();
            break;
        case e_PP_JPG_OSD_CONF:
        	ptr = (BAYONET_JPG_OSD_CONF *)(pConf);
            memcpy(&g_sysConfModule->s_PpJpgOsdConf, ptr, sizeof(BAYONET_JPG_OSD_CONF));
            SavePpJpgOsdConf();
        	break;
        case e_EP_JPG_OSD_CONF:
        	ptr = (EP_JPG_OSD_CONF *)(pConf);
            memcpy(&g_sysConfModule->s_EpJpgOsdConf, ptr, sizeof(EP_JPG_OSD_CONF));
            SaveEpJpgOsdConf();
        	break;
        case e_PP_IRREG_INFO_CONF:
        	ptr = (PP_IRREG_CONF_INFO *)(pConf);
            memcpy(&g_sysConfModule->s_PpIrregConfInfo, ptr, sizeof(PP_IRREG_CONF_INFO));
            SavePPIrregInfoConf();
        	break;
		case e_PASSPORT_BACKUPS_FTP_CONF:
            ptr = (FTP_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_PortBackupsFtpConfInfo,ptr,sizeof(FTP_CONF_INFO));
            SavePassPortBackupsFtpConf();
            break; 
        case e_NET_REC_MANAGE_CONF:
            ptr = (NET_REC_MANAGE_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_NrmConfInfo,ptr,sizeof(NET_REC_MANAGE_INFO));
            SaveNRMConf();
            break;
        case e_EP_NEWADD_IRREG_INFO_CONF:
            ptr = (EP_NEW_ADD_IRREG_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_EpNewAddIrregConfInfo, ptr, sizeof(EP_NEW_ADD_IRREG_CONF_INFO));
            SaveEPNewAddIrregInfoConf();
            break;
        case e_GB28181_CONF:
            ptr = (GB28181_CONF_INFO*)(pConf);
			memcpy(&g_sysConfModule->s_Gb28181ConfInfo, ptr, sizeof(GB28181_CONF_INFO));
            SaveGB28181Conf();
            break;
		default:
			nRet= -1;
			break;
	}
   // sem_post(&(g_sysConfModule->semSaveOP));
	return nRet;
}


static int LoadNetConf(void)
{
    char szDefault[256];
    const char *pszVal = 0;
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","0.0.0.0");
    pszVal =cfgSockGetValue("ipaddr", NULL, szDefault);
    g_sysConfModule->s_NetConfInfo.uiIPAddr = inet_addr(pszVal);
    g_sysConfModule->s_NetConfInfo.uiIPAddr = ntohl(g_sysConfModule->s_NetConfInfo.uiIPAddr);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","0.0.0.0");
    pszVal =cfgSockGetValue("netmask",NULL,szDefault);
    g_sysConfModule->s_NetConfInfo.uiNetMask= inet_addr(pszVal);
    g_sysConfModule->s_NetConfInfo.uiNetMask = ntohl(g_sysConfModule->s_NetConfInfo.uiNetMask);


    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","0.0.0.0");
    pszVal =cfgSockGetValue("gatewayip",NULL,szDefault);
    g_sysConfModule->s_NetConfInfo.uiGateWay= inet_addr(pszVal);
    g_sysConfModule->s_NetConfInfo.uiGateWay = ntohl(g_sysConfModule->s_NetConfInfo.uiGateWay);

    
    return 0;

}


#if 0
static int LoadRtspConf(void)
{
	
	char szDefault[256];
    char szSection[256];
    
	const char *pszVal = 0;
	if(NULL==g_sysConfModule)
		return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_RTSP_CONF_INFO]);
    
	memset(szDefault,0x00,sizeof(szDefault));
	pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_SupportDownLoad],szDefault);
	g_sysConfModule->s_RtspConfInfo.cSupportDownLoad = atoi(pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_DownLoadType],szDefault);
	g_sysConfModule->s_RtspConfInfo.cDownLoadType = atoi(pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_DownLoadIp],szDefault);
	g_sysConfModule->s_RtspConfInfo.uiProxyDownLoadIp = inet_addr(pszVal);
	g_sysConfModule->s_RtspConfInfo.uiProxyDownLoadIp = ntohl(g_sysConfModule->s_RtspConfInfo.uiProxyDownLoadIp);

	memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",554);
	pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_DownLoadPort],szDefault);
    g_sysConfModule->s_RtspConfInfo.usProxyDownLoadPort = atoi(pszVal);



    
    pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_SupportReplay],szDefault);
    g_sysConfModule->s_RtspConfInfo.cSupportReplay= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_ReplayType],szDefault);
    g_sysConfModule->s_RtspConfInfo.cReplayType= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_ReplayIP],szDefault);
    g_sysConfModule->s_RtspConfInfo.uiProxyReplayIp= inet_addr(pszVal);
	g_sysConfModule->s_RtspConfInfo.uiProxyReplayIp = ntohl(g_sysConfModule->s_RtspConfInfo.uiProxyReplayIp);    

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,RtspConfInfoKey[e_ReplayPort],szDefault);
    g_sysConfModule->s_RtspConfInfo.usProxyReplayPort= atoi(pszVal);
    
	


	return 0;
	
}
#else
static int LoadRtspConf()
{
	char szDefault[256];
	char szSection[256];
    
	const char *pszVal = 0;
	if(NULL==g_sysConfModule)
		return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_RTSP_CONF_INFO]);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%d",0);
	pszVal =cfgSockGetValue(szSection, "rtsp-mode", szDefault);
	g_sysConfModule->s_RtspConfInfo.nRtspMode= atoi(pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%s","0.0.0.0");
	pszVal =cfgSockGetValue(szSection, "rtsp-ip", szDefault);
	g_sysConfModule->s_RtspConfInfo.dwRtspIpAddr= inet_addr(pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%d",554);
	pszVal =cfgSockGetValue(szSection, "rtsp-port", szDefault);
	g_sysConfModule->s_RtspConfInfo.dwRtspPort= atoi(pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%d",80);
	pszVal =cfgSockGetValue(szSection, "rtsp-download-port", szDefault);
	g_sysConfModule->s_RtspConfInfo.dwRtspDownLoadPort= atoi(pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%s","");
	pszVal =cfgSockGetValue(szSection, "rtsp-path", szDefault);
	strcpy(g_sysConfModule->s_RtspConfInfo.szRtspPath,pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%s","");
	pszVal =cfgSockGetValue(szSection, "rtsp-user", szDefault);
	strcpy(g_sysConfModule->s_RtspConfInfo.szRtspUsername,pszVal);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%s","");
	pszVal =cfgSockGetValue(szSection, "rtsp-pass", szDefault);
	strcpy(g_sysConfModule->s_RtspConfInfo.szRtspPassword,pszVal);

	return 0;
	
}
#endif

static int LoadSysDefConf(void)
{
	char szDefault[256];
	const char *pszVal = 0;
	if(NULL==g_sysConfModule)
		return (-1);

	memset(szDefault,0x00,sizeof(szDefault));
	sprintf(szDefault,"%d", 1);
	pszVal = cfgSockGetValue("VEHILCE_WORK_STA", "vehWorkSta", szDefault);
	g_sysConfModule->s_SysConfInfo.iVehiSta = atoi(pszVal);
	return 0;
}

static int LoadPassPortOverallConf(void)
{
    char szSection[64];
    char szDefault[256];
    const char *pszVal = 0;
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PASSPORT_OVERALL_CONF]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",PP_DEFAULT_PORT_NAME);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paPassPortName],\
                            szDefault);
    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szPassPortName,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szPassPortName)\
            );
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",PP_DEFAULT_DIRECTION);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paDirection],\
                            szDefault);

    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szDirection,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szDirection)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%s", "0");
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paPassPortId],\
                            szDefault);
                            
    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szPassPortId,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szPassPortId)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%s", "0");
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paDirId],\
                            szDefault);
                            
    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szDirId,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szDirId)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paOSDClolr],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.cOSDColor = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", DEFAULT_PP_LANES);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paValidLanes],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.cValidLanes = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paVLWorkMode],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.cVLWorkMode = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paLPREn],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucLPREn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "浙");
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paLPRString],\
                            szDefault);

    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szLPRFirstChar,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szLPRFirstChar)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 85);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paLPRCConfiden],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucLPRCConfidence = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paVSMeaFlag],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucVSpeedMeaFlag = atoi(pszVal);

    memset(szDefault,500,sizeof(szDefault));
    sprintf(szDefault, "%d", 200);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paVSMeaLen],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.usVSpeedMeaLen = atoi(pszVal);
    if( g_sysConfModule->s_PassPortOverallConfInfo.usVSpeedMeaLen < 200 )
    {
        g_sysConfModule->s_PassPortOverallConfInfo.usVSpeedMeaLen = 200;
    }


    for (int j =0; j < MAX_FLASH_ADDR_SUM; j++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0x01);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                PassPortOverallConfInfoKey[e_paFlashTrigMode],
                j
                );
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_PassPortOverallConfInfo.aucFlashTrigMode[j]=atoi(pszVal);
    }

    for (int j =0; j < MAX_FLASH_ADDR_SUM; j++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0x0);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                PassPortOverallConfInfoKey[e_paFreqFlashEn],
                j
                );
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_PassPortOverallConfInfo.aucFreqFlashEn[j]=atoi(pszVal);
    }

    for (int j =0; j < MAX_FLASH_ADDR_SUM; j++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_STROBE_FLASH_FRE);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                PassPortOverallConfInfoKey[e_paStrobeFlashFreq],
                j
                );
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_PassPortOverallConfInfo.aucStrobeFlashFre[j]=atoi(pszVal);
    }

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",PP_DEFAULT_DEV_CODE);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paPpDevCode],\
                            szDefault);

    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szPpDevCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szPpDevCode)\
            );

    g_sysConfModule->s_PassPortOverallConfInfo.szPpDevCode[MAX_DEV_CODE_LEN-1]=0;

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paFillInLSwitchEn],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucFillInLSwitchEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 30);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paGrayThresold],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucGrayThresholdVal = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 15);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paCheckGrayCycle],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucCheckSwitchCycle = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",PP_DEFAULT_DEV_CODE);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paExPpDevCode],\
                            szDefault);

    strncpy(g_sysConfModule->s_PassPortOverallConfInfo.szExPpDevCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_PassPortOverallConfInfo.szExPpDevCode)\
            );

    g_sysConfModule->s_PassPortOverallConfInfo.szExPpDevCode[EXT_DEV_CODE_LEN-1]=0;

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paSmartSwitchEn],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucSmartSwitchEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paTimerSwitchEn],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.ucTimerSwitchEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 68400);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paFILightBeginTime],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.uiFillInLightBeginTime = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 43200);
    pszVal =cfgSockGetValue(szSection,\
                            PassPortOverallConfInfoKey[e_paFILightLastTime],\
                            szDefault);
    g_sysConfModule->s_PassPortOverallConfInfo.uiFillInLightLastTime = atoi(pszVal);

    
    return 0;
}

static int LoadPassPortVehicleConf(void)
{
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);

    for (int i =0 ; i< MAX_VEHICLE_NUM; i++ )
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_PASSPORT_VEHICLE_CONF],i);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",i);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVehicleId],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].uiCarRoadId= atoi(pszVal);

        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_CAPTIONX);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvCaptionX],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosX= atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosX >= HDConf.image_width )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosX = HDConf.image_width-500;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_CAPTIONY);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvCaptionY],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosY= atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosY >= HDConf.image_height)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosY = HDConf.image_height-200;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"直行%d",i);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVehicleName],\
                                szDefault);
        
        strncpy(g_sysConfModule->s_PortVehicleConfInfo[i].szOSDContent,
                 pszVal,
                 sizeof(g_sysConfModule->s_PortVehicleConfInfo[i].szOSDContent)
                 );

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_AREASTARTX + i*DEFAULT_PP_AREAWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvAreaStartX],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosX = atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosX > HDConf.image_width)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosX = HDConf.image_width;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_AREASTARTY);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvAreaStartY],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosY = atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosY > HDConf.image_height)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosY = HDConf.image_height;
        }
        

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_AREAWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvAreaWidth],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaWidth = atoi(pszVal);

        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaWidth > HDConf.image_width)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaWidth = HDConf.image_width;
        }
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_AREAHEIGHT);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvAreaHeight],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaHeight = atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaHeight > HDConf.image_height)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaHeight = HDConf.image_height;
        }

        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_LIGHT_TYPE);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvLightType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usFlashLightType = atoi(pszVal);

        
        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_CONNECT_TYPE);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvFlashControlType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usFlashLightCtlType = atoi(pszVal);


        
        memset(szDefault,0x00,sizeof(szDefault));
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvComId],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cComId = atoi(pszVal);

        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_FLASH_TYPE);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvFlashDevType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cFlashDevType = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",FLASH_CTL_MODE_SPECIFY);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvFlashCtlMode],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cFlashCtlMode = atoi(pszVal);

        #if defined(CYC500KD)
        
        for (int j =0; j < MAX_FLASH_ADDR_SUM; j++)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s%02d",
                    PassPortVehicleConfInfoKey[e_pvFlashAddr],
                    j
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].ausFlashAddr[j]=atoi(pszVal);
        }

        #else

        for (int j =0 ; j < MAX_FLASH_ADDR_SUM; j++)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", j+1);
            
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s%02d",
                    PassPortVehicleConfInfoKey[e_pvFlashAddr],
                    j
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].ausFlashAddr[j]=atoi(pszVal);
        }
        #endif
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_LOOP);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvDetectType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usDetectType = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_VLSTARTX + i*DEFAULT_PP_VLWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLStartX],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaPosX = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_VLSTARTY);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLStartY],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaPosY = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_VLWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLWidth],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaWidth = atoi(pszVal);



        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_VLHEIGHT);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLHeight],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaHeight = atoi(pszVal);



        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_COIL_TYPE);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVehicleDetectType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleType = atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",CONNECT_TYPE_COM);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVehicleDetectConnectMode],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleLinkMode = atoi(pszVal);

        

        if ( CONNECT_TYPE_COM == g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleLinkMode )
        {
            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", i+1);
            
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "ComId"
                    );/**CoilDesp.ComC.cComId**/
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cComId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", i+1);
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "BoardId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBoardId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "FrontCoilId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cFrontCoilId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szDefault,"%d",1);
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "BehindCoilId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBehindCoilId = atoi(pszVal);
            
            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", i+1);
            
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "GroupId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            /* begin add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            if( 0 == atoi(pszVal) )
            {
                g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId = 1;
            }
            else
            {
                g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId = atoi(pszVal);
            }
            /* end add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            
        }
        else if (CONNECT_TYPE_GPIO == g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleLinkMode)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilGpio",
                    "BoardId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBoardId = atoi(pszVal);
            
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilGpio",
                    "FrontGpioId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cFrontCoilId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szDefault,"%d",1);
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilGpio",
                    "BehindGpioId"
                    ); 
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBehindCoilId = atoi(pszVal);
              
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_MAXSPEED);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvMaxSpeed],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usMaxSpeed = atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_MAXSPEED+10);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvIrreSpeed],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usIrreSpeed = atoi(pszVal);
        if(g_sysConfModule->s_PortVehicleConfInfo[i].usIrreSpeed \
        < g_sysConfModule->s_PortVehicleConfInfo[i].usMaxSpeed)
        {
           g_sysConfModule->s_PortVehicleConfInfo[i].usIrreSpeed \
           = g_sysConfModule->s_PortVehicleConfInfo[i].usMaxSpeed;
        };


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_MINSPEED);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvMinSpeed],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usMinSpeed = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_COILDIST);
        
        pszVal =cfgSockGetValue(szSection,\
                                        PassPortVehicleConfInfoKey[e_pvCoilDist],\
                                        szDefault);
                g_sysConfModule->s_PortVehicleConfInfo[i].uiCoilDist = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLDispFlag],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cVLDispFlag = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvLaneDispFlag],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cLaneDispFlag = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%s", "0");
        pszVal = cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvLaneId],\
                                szDefault);
                                
        strncpy(g_sysConfModule->s_PortVehicleConfInfo[i].szLaneId,\
            pszVal,\
            sizeof(g_sysConfModule->s_PortVehicleConfInfo[i].szLaneId)\
            );

        /*Start of ITS on 2011-5-3 11:24 2.0.0.1*/
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSingalCoilPos],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cSingalCoilPos = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvGroudCoilDecType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cGroudCoilDecType = atoi(pszVal);        

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSingalCoilSnapType],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cSingalCoilSnapType = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvCoilSwitchFlag],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cCoilSwitchFlag = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", SWITCH_DEFAULT_VAL);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvInLightSta],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cInLightSta = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", SWITCH_DEFAULT_VAL);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvCoilSwitchSta],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cCoilSwitchSta = atoi(pszVal);
        /*End of ITS on 2011-5-3 11:24 2.0.0.1*/

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLSnapDir],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cVLSnapDir = atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvVLDecEn],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cVLDecEn = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 200);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSingalCoilHigh],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usSingalCoilHigh = atoi(pszVal);

        memset(szDefault, 0x00, sizeof(szDefault));
        sprintf(szDefault, "%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvDaySnapObj],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].ucDaySnapObj = atoi(pszVal);

        memset(szDefault, 0x00, sizeof(szDefault));
        sprintf(szDefault, "%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvNightSnapObj],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].ucNightSnapObj = atoi(pszVal);

        memset(szDefault, 0x00, sizeof(szDefault));
        sprintf(szDefault, "%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSnapPicNum],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum= atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum >2 )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum = 2;
        }
        if( g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum <= 0 )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum =1;
        }

        memset(szDefault, 0x00, sizeof(szDefault));
        sprintf(szDefault, "%d", 100);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSnapPicInte],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].usSnapPicInter = atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usSnapPicInter < 40 )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].usSnapPicInter = 40;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvInLightEn],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].ucInLightEn= atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvBeginTimeSec],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.uiBeginTimeSecs = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 86399);/* 结束时间为23:59:59 */
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvLastTimeSec],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.uiLastTimeSecs= atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvTimeSlotObj],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.ucSnapObj= atoi(pszVal);
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.ucSnapObj <= 0 )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.ucSnapObj =1;
        }

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSnapDir],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cSnapDir = atoi(pszVal);

        memset(szDefault, 0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_SD_SNAP_EN);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDEnFlag],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cSDEnFlag = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDBeginTime1],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].iSDBeginTime1 = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDLastTime1],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].iSDLastTime1 = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDBeginTime2],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].iSDBeginTime2 = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDLastTime2],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].iSDLastTime2 = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDBeginTime3],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].iSDBeginTime3 = atoi(pszVal);

        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvSDLastTime3],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].iSDLastTime3 = atoi(pszVal);
        
        memset(szDefault,0x00, sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                PassPortVehicleConfInfoKey[e_pvTruckEnFlag],\
                                szDefault);
        g_sysConfModule->s_PortVehicleConfInfo[i].cTruckEnFlag = atoi(pszVal);

    }
    return 0;
}


static int LoadElecPolliceOverallConf(void)
{
    char szSection[64];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_ELEC_POLICE_OVERALL_CONF]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_PORT_NAME);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaElecPoliceName],\
                            szDefault);
    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szCarRoadName,\
            pszVal,\
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szCarRoadName)\
            );
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_DIRECTION);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaDirection],\
                            szDefault);

    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szDirection,
            pszVal,
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szDirection)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",PIC_SEPARATE_THREE);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaPicStyle],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.usPicStyle = atoi(pszVal);


    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", LIGHT_TYPE_HALGON);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaLightType],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.usFlashLightType= atoi(pszVal);

    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",DEFAULT_CONNECT_TYPE);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaFlashControlType],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.usFlashLightCtlType= atoi(pszVal);
    


    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaComId],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cComId= atoi(pszVal);



    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",DEFAULT_FLASH_TYPE);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaFlashDevType],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cFlashDevType= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",FLASH_CTL_MODE_SPECIFY);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaFlashCtlMode],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cFlashCtlMode = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",XCS_PRO_EPG_VER_2);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaXcsProVer],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.usXcsProVer = atoi(pszVal);



    for (int i =0 ; i < MAX_FLASH_ADDR_SUM; i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", i+1);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaFlashAddr],
                i
                );
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceOverallConfInfo.ausFlashAddr[i]=atoi(pszVal);
    }

    for (int i =0 ;i < MAX_AREA_GROUP_NUM; i++)
    {
        for (int j =0 ;j < MAX_AREA_NUM; j++)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_REDSTARTX);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "StartX");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            
            g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaPosX = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_REDSTARTY);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "StartY");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaPosY = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_REDWIDTH);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "Width");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaWidth = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szDefault,"%d",DEFAULT_EP_REDHEIGHT);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "Height");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaHeight = atoi(pszVal);
        }
        
    }

    /*红灯阈值下限参数*/
    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_RED_LIGHT_R_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedLow],
            KEY_RED
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLow.ucRed = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_RED_LIGHT_G_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedLow],
            KEY_GREEN
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLow.ucGreen = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_RED_LIGHT_B_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedLow],
            KEY_BLUE
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLow.ucBlue = atoi(pszVal);

    /*红灯阈值上限参数*/
    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_RED_LIGHT_R_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedHigh],
            KEY_RED
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedHigh.ucRed = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_RED_LIGHT_G_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedHigh],
            KEY_GREEN
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedHigh.ucGreen = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_RED_LIGHT_B_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedHigh],
            KEY_BLUE
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedHigh.ucBlue = atoi(pszVal);

    /*绿灯阈值下限参数*/
    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_GREEN_LIGHT_R_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenLow],
            KEY_RED
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenLow.ucRed = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_GREEN_LIGHT_G_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenLow],
            KEY_GREEN
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenLow.ucGreen = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_GREEN_LIGHT_B_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenLow],
            KEY_BLUE
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenLow.ucBlue = atoi(pszVal);

    /*绿灯阈值上限参数*/
    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_GREEN_LIGHT_R_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenHigh],
            KEY_RED
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenHigh.ucRed = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_GREEN_LIGHT_G_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenHigh],
            KEY_GREEN
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenHigh.ucGreen = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_GREEN_LIGHT_B_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenHigh],
            KEY_BLUE
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenHigh.ucBlue = atoi(pszVal);


    /*黄灯阈值下限参数*/
    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_YELLOW_LIGHT_R_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowLow],
            KEY_RED
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowLow.ucRed = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_YELLOW_LIGHT_G_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowLow],
            KEY_GREEN
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowLow.ucGreen = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_YELLOW_LIGHT_B_LOW_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowLow],
            KEY_BLUE
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowLow.ucBlue = atoi(pszVal);

    /*黄灯阈值上限参数*/
    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_YELLOW_LIGHT_R_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowigh],
            KEY_RED
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowHigh.ucRed = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_YELLOW_LIGHT_G_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowigh],
            KEY_GREEN
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowHigh.ucGreen = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    memset(szInfoKey,0x00,sizeof(szInfoKey));

    sprintf(szDefault,"%d",DEFAULT_YELLOW_LIGHT_B_HIGH_VALUE);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowigh],
            KEY_BLUE
            );
    pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowHigh.ucBlue = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_ROAD_REMARKS);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_roadRemarks],\
                            szDefault);
    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szRemarks,\
            pszVal,\
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szRemarks)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",VIRTUAL_LOOP);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaVLType],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cDetectType = atoi(pszVal);  

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",MAX_VALID_CHAN_SUM);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaValidChanSum],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum = atoi(pszVal);  

    if(g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum > g_uiCamLaneSum)
    {
        g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum = g_uiCamLaneSum;
    }

    if(g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum <= 0)
    {
        g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum = 1;
    }

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", DEFAULT_CAR_DISTANCE);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaCarDistance],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cCarDistance = atoi(pszVal);  

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", DEFAULT_FRAME_INTERVAL);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaSnapInterval],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cFrameInterval = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaVLDisFlag],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cLightAmendDisFlag = atoi(pszVal);
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "0");
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaRoadId],\
                            szDefault);

    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szRoadId,
            pszVal,
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szRoadId)\
            );    

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "0");
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaDirId],\
                            szDefault);
                            
    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szDirId,
            pszVal,
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szDirId)\
            );  

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaRLAmendEn],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cLightAmendEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", RED_DETECT_SINGAL);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaRLDectType],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cRLDectType = atoi(pszVal);
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaOSDColour],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cOSDColour= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 3);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaRLAmendLevel],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cRLAmendLevel = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaPPSendEn],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cPPSendEn = atoi(pszVal);

    for (int i =0 ; i < 4 ; i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",0x0);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaFreqFlashEn], i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFreqFlashEn[i]=atoi(pszVal);
        if(0xff == g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFreqFlashEn[i])
        {
            g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFreqFlashEn[i] =0;
        }
    }


    for (int i =0; i < MAX_AREA_GROUP_NUM; i++)
    {
        memset(szDefault, 0x00, sizeof(szDefault));
        memset(szInfoKey, 0x00, sizeof(szInfoKey));

        sprintf(szDefault,"%d",1);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "lightStrobeEn");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        
        g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucLightStrobeEn = atoi(pszVal);

        memset(szDefault, 0x00, sizeof(szDefault));
        memset(szInfoKey, 0x00, sizeof(szInfoKey));

        sprintf(szDefault,"%d",10);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "lightStrobeFrames");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        
        g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucLightStrobeFrame = atoi(pszVal);


        memset(szDefault, 0x00, sizeof(szDefault));
        memset(szInfoKey, 0x00, sizeof(szInfoKey));

        sprintf(szDefault,"%d",0);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "singalColLight");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        
        g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucSingalColLight = atoi(pszVal);

        memset(szDefault, 0x00, sizeof(szDefault));
        memset(szInfoKey, 0x00, sizeof(szInfoKey));

        sprintf(szDefault,"%d",0);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "mapRDNo");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        
        g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucMapRDNo = atoi(pszVal);
        
    }

    for (int i =0 ; i < 4 ; i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0x01);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaFlashTrigMode], i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFlashTrigMode[i]=atoi(pszVal);
        if(0xff == g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFlashTrigMode[i])
        {
            g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFlashTrigMode[i] = 0x01;
        }
    }

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaPPFlashEn],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cPPFlashEn = atoi(pszVal);

    for (int i =0 ; i < 4 ; i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_STROBE_FLASH_FRE);
        
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaStrobeFlashFreq], i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceOverallConfInfo.aucStrobeFlashFre[i]=atoi(pszVal);
        if(0xff == g_sysConfModule->s_ElecPoliceOverallConfInfo.aucStrobeFlashFre[i])
        {
            g_sysConfModule->s_ElecPoliceOverallConfInfo.aucStrobeFlashFre[i] = 0x04;
        }
    }

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",EP_DEFAULT_DEV_CODE);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaEpDevCode],\
                            szDefault);
                            
    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szEpDevCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szEpDevCode)\
            );
            
    g_sysConfModule->s_ElecPoliceOverallConfInfo.szEpDevCode[MAX_DEV_CODE_LEN-1]=0;

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaACSyncSwitchEn],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cACSyncSwitchEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 50);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaACSwitchThreshold],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.ucACSwitchThreshold = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 30);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaACDecCycle],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.ucACDecCycle = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 500);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaDayACPhase],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.usDayACPhase = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 200);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaNightACPhase],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.usNightACPhase = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 4);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaRedModifyMode],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.cRedModifyMode = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",EP_DEFAULT_DEV_CODE);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaExEpDevCode],\
                            szDefault);
                            
    strncpy(g_sysConfModule->s_ElecPoliceOverallConfInfo.szExEpDevCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_ElecPoliceOverallConfInfo.szExEpDevCode)\
            );
            
    g_sysConfModule->s_ElecPoliceOverallConfInfo.szExEpDevCode[EXT_DEV_CODE_LEN-1]=0;

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaSmartSwitchEn],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.ucSmartSwitchEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaTimerSwitchEn],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.ucTimerSwitchEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 68400);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaFILightBeginTime],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.uiFillInLightBeginTime = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 43200);
    pszVal =cfgSockGetValue(szSection,\
                            ElecPolicOverallConfInfoKey[e_eaFILightLastTime],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceOverallConfInfo.uiFillInLightLastTime = atoi(pszVal);


    return 0;
}



static int LoadElecPolliceVehicleConf(void)
{
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for (int i =0 ; i< MAX_VEHICLE_NUM; i++ )
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_ELEC_POLICE_VEHICLE_CONF_INFO],i);
            
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",i);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evVehicleId],\
                                szDefault);

        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].uiCarRoadId= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_CAPTIONX);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evCaptionX],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosX= atoi(pszVal);
        if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosX >= HDConf.image_width )
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosX = HDConf.image_width -500;
        }

        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_CAPTIONY);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evCaptionY],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosY= atoi(pszVal);
        if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosY >= HDConf.image_height)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosY = HDConf.image_height -200;
        }
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"直行%d",i+1);
        pszVal =cfgSockGetValue(szSection,\
                                  ElecPolicVehicleConfInfoKey[e_evVehicleName],\
                                  szDefault);
        strncpy(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].szOSDContent,
                pszVal,
                sizeof(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].szOSDContent)
                );
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", i*DEFAULT_EP_GROUD_X);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evAreaStartX],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX = atoi(pszVal);
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX > HDConf.image_width)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX = HDConf.image_width;
        }


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_EP_GROUD_Y-400);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evAreaStartY],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY= atoi(pszVal);
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY > HDConf.image_height)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY = HDConf.image_height;
        }


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PP_AREAWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evAreadWidth],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth= atoi(pszVal);
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth > HDConf.image_width)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth = HDConf.image_width;
        }

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_PP_AREAHEIGHT);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evAreadHeight],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight= atoi(pszVal);
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight > HDConf.image_height)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight = HDConf.image_height;
        }


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_RED_DETECT);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evRedDetectType],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLDetectType= atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", CONNECT_TYPE_COM);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evRedDetectConnectMode],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLConnectMode= atoi(pszVal);


        if ( CONNECT_TYPE_COM  == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLConnectMode )
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evRedDetectDesp],
                    "RedCom",
                    "ComId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cComId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",i+1);
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evRedDetectDesp],
                    "RedCom",
                    "CarRoadId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            /* begin add by wangb 2010-4-28 for 红灯车道号固定从1开始 */
            if( 0 == atoi(pszVal) )
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cCarRoadId = 1;
            }
            else
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cCarRoadId = atoi(pszVal);  
            }
            /* end add by wangb 2010-4-28 for 红灯车道号固定从1开始 */
        }
        else if ( CONNECT_TYPE_GPIO == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLConnectMode )
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",i+1);
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evRedDetectDesp],
                    "RedGpio",
                    "GpioId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPGpioRedLightDesp.cGpioId = atoi(pszVal);
        }

        memset(szDefault,0x00,sizeof(szDefault));
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evCoilType],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleType= atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",CONNECT_TYPE_COM);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evVehicleDetectConnectMode],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleLinkMode= atoi(pszVal);

        
        if ( CONNECT_TYPE_COM  == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleLinkMode )
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "ComId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cComId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "BoardId"
                    );
            
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBoardId = atoi(pszVal);
            
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "FrontCoilId"
                    );
            
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cFrontCoilId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szDefault,"%d",1);
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "BehindCoilId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBehindCoilId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",i+1);
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "GroupId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            /* begin add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            if( 0 == atoi(pszVal) )
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId = 1;
            }
            else
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId = atoi(pszVal);
            }
            /* end add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
        }
        else if (CONNECT_TYPE_GPIO == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleLinkMode)
        {

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilGpio",
                    "BoardId"
                    );
            
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBoardId = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilGpio",
                    "FrontGpioId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cFrontCoilId = atoi(pszVal);

            sprintf(szDefault,"%s.%s.","CoilGpio","BehindGpioId");/**CoilDesp.GpioC.nBehindGpioId**/
            
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szDefault,"%d",1);
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilGpio",
                    "BehindGpioId"
                    );
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBehindCoilId = atoi(pszVal);
              
        }
        memset(szDefault,0x00,sizeof(szDefault));
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evForceRedLight],\
                                szDefault);
       g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].uiForcedRedLight = atoi(pszVal);

       memset(szDefault,0x00,sizeof(szDefault));
       sprintf(szDefault,"%d", 0xff);
       pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evLaneDir],\
                               szDefault);
       if( atoi(pszVal) == 0xff )
       {
            pszVal =cfgSockGetValue(szSection,\
                               "evLaneDir",\
                               szDefault);
       }
       if( atoi(pszVal) == 0xff )
       {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cLaneDir= DEFAULT_LANE_DIR; 
       }
       else
       {
           g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cLaneDir= atoi(pszVal); 
       }
       for (int j =0 ;j < MAX_LIGHT_TYPE; j++)
       {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d", 0xff);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evLightType],
                    j,
                    "Enable");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            if(atoi(pszVal) == 0xff)
            {
                sprintf(szInfoKey,"%s.Seq%02d.%s",
                    "evLightType",
                    j,
                    "Enable");
                pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            }
            if(atoi(pszVal) == 0xff)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cEnable = 0;
            }
            else
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cEnable = atoi(pszVal);
            }
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d", 0xff);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evLightType],
                    j,
                    "Type");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            if( atoi(pszVal) == 0xff )
            {
                sprintf(szInfoKey,"%s.Seq%02d.%s",
                    "evLightType",
                    j,
                    "Type");
                    
                pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            }
            if( atoi(pszVal) == 0xff )
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cLightType= DEFAULT_LIGHT_DIR;
            }
            else
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cLightType= atoi(pszVal);
            }
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d", 0xff);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evLightType],
                    j,
                    "VehicleNum");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            if( atoi(pszVal) == 0xff )
            {
                sprintf(szInfoKey,"%s.Seq%02d.%s",
                    "evLightType",
                    j,
                    "VehicleNum");
                pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            }
            if( atoi(pszVal) == 0xff )
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cVehicleNum= 1;
            }
            else
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cVehicleNum= atoi(pszVal);
            }
        }

        memset(szDefault,0x00,sizeof(szDefault));
        memset(szInfoKey,0x00,sizeof(szInfoKey));

        sprintf(szDefault,"%d", DEFAULT_EP_REDSTARTX);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "StartX");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaPosX= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        memset(szInfoKey,0x00,sizeof(szInfoKey));

        sprintf(szDefault,"%d",DEFAULT_EP_REDSTARTY);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "StartY");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaPosY = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        memset(szInfoKey,0x00,sizeof(szInfoKey));

        sprintf(szDefault,"%d", DEFAULT_EP_REDWIDTH);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "Width");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaWidth = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szDefault,"%d", DEFAULT_EP_REDHEIGHT);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "Height");
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaHeight = atoi(pszVal);

        for (int j =0; j < 2; j++)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_VL_X1 + (j/1)*DEFAULT_EP_VL_X2 + i*DEFAULT_EP_VL_X2);
            
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartX");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1= atoi(pszVal);

            if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1 > HDConf.image_width)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1 = HDConf.image_width; 
            }

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_VL_Y1);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartY");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1= atoi(pszVal);

            if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1 > HDConf.image_height)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1 = HDConf.image_height; 
            }

        }

        for (int j =2 ;j < MAX_AREA_NUM; j++)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_VL_X1 + (j%2)*DEFAULT_EP_VL_X2 + i*DEFAULT_EP_VL_X2);
            
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartX");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1= atoi(pszVal);
            
            if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1 > HDConf.image_width)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1 = HDConf.image_width; 
            }

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DEFAULT_EP_VL_Y2);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartY");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1= atoi(pszVal);
            
            if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1 > HDConf.image_height)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1 = HDConf.image_height; 
            }

        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evVLDisFlag],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cVLDispFlag= atoi(pszVal); 
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evLaneDisFlag],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cLaneLoopDispFlag = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%s", "0");
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evLaneId],\
                               szDefault);
        strncpy(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].szLaneId,
            pszVal,
            sizeof(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].szLaneId)\
            ); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_CAR_DISTANCE);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evCarDistance],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarDistance = atoi(pszVal);  

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_FRAME_INTERVAL);
        pszVal =cfgSockGetValue(szSection,\
                                ElecPolicVehicleConfInfoKey[e_evSnapInterval],\
                                szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cFrameInterval = atoi(pszVal);

        /*Start of ITS on 2011-4-2 14:23 2.0.0.1*/
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", VIRTUAL_LOOP);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evCoilDecType],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cDetectType = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evCoilSwitchFlag],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchFlag = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", SWITCH_DEFAULT_VAL);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evCoilSwitchSta],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchSta = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", SWITCH_DEFAULT_VAL);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evRedLightDecSta],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cRedLightDectSta = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_LIGHT_TYPE);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evFlashType],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usFlashLightType = atoi(pszVal); 
        /*End of ITS on 2011-4-2 14:23 2.0.0.1*/
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evInLightSta],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cInLightSta = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evVLDecEn],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cVLDecEn = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evNoDir],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarNoDir = atoi(pszVal); 

        for (int j =4 ;j < 6; j++)
        {
            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d", DEFAULT_EP_VL_X3 + (j%4)*DEFAULT_EP_VL_X2 + i*DEFAULT_EP_VL_X2);
            
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartX");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strNoLaneVLPoint[j-4].usVLX1= atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d", DEFAULT_EP_VL_Y3);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartY");
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strNoLaneVLPoint[j-4].usVLY1= atoi(pszVal);

        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evCarTrackEn],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarTrackEn = atoi(pszVal); 


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 1);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evCarSnapDir],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarSnapDir = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_MIN_PLATE_SIZE);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evMinPlatePix],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucMinPlatePixelSize = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_MAX_PLATE_SIZE);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evMaxPlatePix],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucMaxPlatePixelSize = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 70);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evEveningThresh],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucCorrEveningThresh = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 200);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evCoilDistance],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usCoilDist = atoi(pszVal); 
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usCoilDist < 100 )
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usCoilDist = 100;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evPPDelaySnapFrames],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucPPDelaySnapFrames = atoi(pszVal); 

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evSwitchLaneEn],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cSwitchLaneEn = atoi(pszVal);

		memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 9);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evSwitchLaneLeftNo],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cSwitchLaneLeftNo = atoi(pszVal);         

		memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 10);
        pszVal =cfgSockGetValue(szSection,\
                               ElecPolicVehicleConfInfoKey[e_evSwitchLaneRightNo],\
                               szDefault);
        g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cSwitchLaneStraightNo = atoi(pszVal);         
        
    }

   
    return 0;
}
static int LoadVideoConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;
    if(NULL==g_sysConfModule)
        return (-1);
    for(int i=0;i<TOTAL_CHANNEL_NUM;i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_VIDEO_CONF_INFO],i);
            
       
        g_sysConfModule->s_VideoConfInfo[i].usChannel= i;

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_IMAGE_QUALITY);
        pszVal = cfgSockGetValue(szSection, \
                                 VideoConfInfoKey[e_vVQuality],\
                                 szDefault);
        g_sysConfModule->s_VideoConfInfo[i].cVQuality = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENC_FRAMERATE);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vFrameRate],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].cFrameRate = atoi(pszVal);

        #if defined(CYC500XKW)||defined(CYC500XLKW)||defined(CYC500KX2)||defined(CYC500K)||defined(CYC500KD)||defined(CYC800JX)||defined(CYC500JX)

        if(g_sysConfModule->s_VideoConfInfo[i].cFrameRate > 9)
        {
            g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 9;
        }
        
		#elif defined(CYC500XKW4C)||defined(CYC500XAW)
		
		//g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 17;//帧率放开
		#elif defined(CYC800XKW4C)

		if(g_sysConfModule->s_VideoConfInfo[i].cFrameRate > DEFAULT_ENC_FRAMERATE)
        {
            g_sysConfModule->s_VideoConfInfo[i].cFrameRate = DEFAULT_ENC_FRAMERATE;
        }
		
        #else

        if(g_sysConfModule->s_VideoConfInfo[i].cFrameRate > 17)
        {
            g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 17;
        }
        #endif
        
#if defined(CYT200SDI)
        g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 25;
#endif


       
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENC_BITRATE);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vBitRate],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].uiBitRate = atoi(pszVal);

        /* 为了保证红灯报警录像文件大小，所以码流规定最小为4M */
        if(g_sysConfModule->s_VideoConfInfo[i].uiBitRate > 5120)
        {
            g_sysConfModule->s_VideoConfInfo[i].uiBitRate = 5120;
        }
       

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENC_IFRAMEINTV+i*5);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vKeyFinterval],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].cKeyFInterval = atoi(pszVal);

        /* 为了保证红灯报警录像能够在播放时有I帧，所以规定I帧间隔最大不超过25 */
        if(g_sysConfModule->s_VideoConfInfo[i].cKeyFInterval > 25)
        {
            g_sysConfModule->s_VideoConfInfo[i].cKeyFInterval = 25;
        }

        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENC_FORMAT);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vImageResol],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].cImageResol = atoi(pszVal);
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%s",VENC_TYPE_STR(DEFAULT_ENC_TYPE));
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vEncType],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].cEncodeType = VENC_TYPE_NUM(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENC_CBRVBR);
        pszVal = cfgSockGetValue(szSection,\
                                 VideoConfInfoKey[e_vVBR],\
                                 szDefault);
        g_sysConfModule->s_VideoConfInfo[i].cVBR = atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENV_OSD_X);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vXPos],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].usXPos = atoi(pszVal);
        if( g_sysConfModule->s_VideoConfInfo[i].usXPos >= HDConf.image_width )
        {
            g_sysConfModule->s_VideoConfInfo[i].usXPos = HDConf.image_width -500;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_ENV_OSD_Y);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vYPos],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].usYPos = atoi(pszVal);
        if( g_sysConfModule->s_VideoConfInfo[i].usYPos >= HDConf.image_height )
        {
            g_sysConfModule->s_VideoConfInfo[i].usYPos = HDConf.image_height -200;
        }

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%s","HD_VIDEO");
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vContent],\
                                szDefault);
        strncpy(g_sysConfModule->s_VideoConfInfo[i].szContent,pszVal,
                    sizeof(g_sysConfModule->s_VideoConfInfo[i].szContent));

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",704);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vDebugFrameX],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].usDebugFrameX = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",576);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vDebugFrameY],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].usDebugFrameY = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",0);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vDebugFrameEn],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].ucDebugFrameEn = atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",1);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vTimeDisMode],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].ucTimeDisMode = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",64);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vTimeXPos],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].usTimeXPos = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",96);
        pszVal =cfgSockGetValue(szSection,\
                                VideoConfInfoKey[e_vTimeYPos],\
                                szDefault);
        g_sysConfModule->s_VideoConfInfo[i].usTimeYPos = atoi(pszVal);

    }
    return 0;
}



static int LoadRecConf(void)
{
	char szSection[32];
	char szDefault[256];
	const char *pszVal = 0;
	if(NULL==g_sysConfModule)
		return (-1);
	
	for(int i=0;i<TOTAL_CHANNEL_NUM;i++)
	{
		memset(szSection,0x00,sizeof(szSection));
		sprintf(szSection,"%s%02d",sys_ini_section[e_REC_CONF_INFO],i+1);
			
		memset(szDefault,0x00,sizeof(szDefault));
		sprintf(szDefault,"%d",i);
		pszVal =cfgSockGetValue(szSection,\
								RecConfInfoKey[e_rRecChannelID],\
								szDefault);
		g_sysConfModule->s_RecConfInfo[i].nChannelID = atoi(pszVal);

		memset(szDefault,0x00,sizeof(szDefault));
		sprintf(szDefault,"%d",DEFAULT_REC_MODE);
		pszVal =cfgSockGetValue(szSection,\
								RecConfInfoKey[e_rSysRecMode],\
								szDefault);
		g_sysConfModule->s_RecConfInfo[i].nSysRecMode= atoi(pszVal);

		memset(szDefault,0x00,sizeof(szDefault));
		sprintf(szDefault,"%d",DEFAULT_STORAGE_MODE);
		pszVal =cfgSockGetValue(szSection,\
								RecConfInfoKey[e_rLoopRecFlag],\
								szDefault);
		g_sysConfModule->s_RecConfInfo[i].nLoopRecFlag= atoi(pszVal);

		memset(szDefault,0x00,sizeof(szDefault));
		sprintf(szDefault,"%d",DEFAULT_STORAGE_REC_DAYS);
		pszVal =cfgSockGetValue(szSection,\
    							RecConfInfoKey[e_rStorageRecDays],\
    							szDefault);
		g_sysConfModule->s_RecConfInfo[i].nStorageRecDays= atoi(pszVal);

		{
			memset(szDefault,0x00,sizeof(szDefault));
			sprintf(szDefault,"/ide/disk%d,",i+1);
			pszVal =cfgSockGetValue(szSection,\
									RecConfInfoKey[e_rRecPath],\
									szDefault);

			int j=0;
			char szVal[256] = {0};
			char *pDest = NULL;

			strcpy(szVal,pszVal);
			pDest = szVal;
			do{	
				char *pPath = pDest;

				if(pDest )
					pDest = strchr(pDest,',');
				
				if(NULL==pDest)
				{
					break;
				}
				else
				{
					*pDest = '\0';
					pDest++;

					if(strlen(pPath))
						strcpy(g_sysConfModule->s_RecConfInfo[i].szRecPath[j],pPath);
				}

			}while((++j)<DEFAULT_REC_DISK_PER_CHANNEL);
		}

		{// 一次性计划
			int j = 0;
			char szVal[512] = {0};
			char *pDest = NULL;
			char *pSegment = NULL;
			unsigned long ulPlanStart = time(NULL)+8*3600;
			struct tm tmPlanStart;
			unsigned long ulPlanEnd = ulPlanStart+30*3600*24;
			struct tm tmPlanEnd;
			
			memset(szDefault,0x00,sizeof(szDefault));
			localtime_r((time_t *)&ulPlanStart,&tmPlanStart);
			localtime_r((time_t *)&ulPlanEnd,&tmPlanEnd);
			// 修改默认参数
			sprintf(szDefault,"%01d,%04d-%02d-%02d %02d:%02d:%02d<->%04d-%02d-%02d %02d:%02d:%02d,",\
				0,tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
				tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec,\
				tmPlanEnd.tm_year+1900,tmPlanEnd.tm_mon+1,tmPlanEnd.tm_mday,\
				tmPlanEnd.tm_hour,tmPlanEnd.tm_min,tmPlanEnd.tm_sec);
			pszVal =cfgSockGetValue(szSection,\
													RecConfInfoKey[e_rOncePlanRecTime],\
													szDefault);

			strcpy(szVal,pszVal);
			pSegment = szVal;
			pDest = strchr(szVal,',');

			g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.bIsValid = 0;
			if(pDest)
			{
				*pDest = '\0';
				pDest++;

				if(strlen(pSegment)==1)
				{
					g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.bIsValid = (atoi(pSegment)>0) ? 1:0;
				}
				else
				{
					g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.bIsValid = 0;
				}
			}

			do{
				pSegment = pDest;

				g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiStartTime =0;
				g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiEndTime = 0;

				if(pDest)
					pDest = strchr(pDest,',');
				
				if(pDest)
				{
					*pDest = '\0';
					pDest++;

					if(strlen(pSegment)==(4+1+2+1+2+1+2+1+2+1+2+3+4+1+2+1+2+1+2+1+2+1+2))//41
					{
						int year1,month1,day1,hour1,minute1,second1;
						int year2,month2,day2,hour2,minute2,second2;

						sscanf(pSegment,"%04d-%02d-%02d %02d:%02d:%02d<->%04d-%02d-%02d %02d:%02d:%02d",\
							&year1,&month1,&day1,&hour1,&minute1,&second1,\
							&year2,&month2,&day2,&hour2,&minute2,&second2);

						tmPlanStart.tm_year = year1-1900;
						tmPlanStart.tm_mon = month1-1;
						tmPlanStart.tm_mday = day1;
						tmPlanStart.tm_hour = hour1;
						tmPlanStart.tm_min = minute1;
						tmPlanStart.tm_sec = second1;
						ulPlanStart = mktime(&tmPlanStart);
						tmPlanEnd.tm_year = year2-1900;
						tmPlanEnd.tm_mon = month2-1;
						tmPlanEnd.tm_mday = day2;
						tmPlanEnd.tm_hour = hour2;
						tmPlanEnd.tm_min = minute2;
						tmPlanEnd.tm_sec = second2;
						ulPlanEnd =  mktime(&tmPlanEnd);

						if(IS_VALID_DATE(year1, month1, day1)&&IS_VALID_TIME(hour1, minute1, second1)&&\
							IS_VALID_DATE(year2, month2, day2)&&IS_VALID_TIME(hour2, minute2, second2)&&\
							(ulPlanEnd>ulPlanStart))
						{
							g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiStartTime =ulPlanStart;
							g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiEndTime = ulPlanEnd;
						}
					}
				}				
			}while((++j)<DEFAULT_PLANT_TIME_SEGMENT);
		}

		{//  天计划 
			int j = 0;
			char szVal[256] = {0};
			char *pDest = NULL;
			char *pSegment = NULL;
			
			memset(szDefault,0x00,sizeof(szDefault));
			sprintf(szDefault,"%01d,%02d:%02d:%02d<->%02d:%02d:%02d,",\
				1,0,0,0,23,59,59);// 修改 天计划，变成每天都录
			pszVal =cfgSockGetValue(szSection,\
													RecConfInfoKey[e_rDayPlanRecTime],\
													szDefault);
			strcpy(szVal,pszVal);
			pSegment = szVal;
			pDest = strchr(szVal,',');

			g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.bIsValid = 0;
			if(pDest)
			{
				*pDest = '\0';
				pDest++;

				if(strlen(pSegment)==1)
				{
					g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.bIsValid = (atoi(pSegment)>0) ? 1:0;
				}
				else
				{
					g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.bIsValid = 0;
				}
			}
			
			do{
				pSegment = pDest;

				g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiStartTime =0;
				g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiEndTime = 0;

				if(pDest)
					pDest = strchr(pDest,',');
				
				if(pDest)
				{
					*pDest = '\0';
					pDest++;

					if(strlen(pSegment)==(2+1+2+1+2+3+2+1+2+1+2))
					{
						int hour1,minute1,second1;
						int hour2,minute2,second2;
						unsigned int ulPlanStart;
						unsigned int ulPlanEnd;

						sscanf(pSegment,"%02d:%02d:%02d<->%02d:%02d:%02d",\
							&hour1,&minute1,&second1,\
							&hour2,&minute2,&second2);

						ulPlanStart = hour1*60*60+minute1*60+second1;
						ulPlanEnd = hour2*60*60+minute2*60+second2;

						if(IS_VALID_TIME(hour1, minute1, second1)&&\
							IS_VALID_TIME(hour2, minute2, second2)&&\
							(ulPlanEnd>ulPlanStart))
						{
							g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiStartTime =ulPlanStart;
							g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiEndTime = ulPlanEnd;
						}
					}
				}
			}while((++j)<DEFAULT_PLANT_TIME_SEGMENT);
		}

		//周计划
		for(int j=0;j<7;j++)
		{
			int k = 0;
			char szVal[256] = {0};
			char *pDest = NULL;
			char *pSegment = NULL;
			
			memset(szDefault,0x00,sizeof(szDefault));
			sprintf(szDefault,"%01d,%02d:%02d:%02d<->%02d:%02d:%02d,",\
				0,0,0,0,0,0,0);
			pszVal =cfgSockGetValue(szSection,\
													RecConfInfoKey[e_rMondayPlanRecTime+j],\
													szDefault);
			strcpy(szVal,pszVal);
			pSegment = szVal;
			pDest = strchr(szVal,',');

			g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].bIsValid = 0;
			
			if(pDest)
			{
				*pDest = '\0';
				pDest++;

				if(strlen(pSegment)==1)
				{
					g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].bIsValid = (atoi(pSegment)>0) ? 1:0;
				}
				else
				{
					g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].bIsValid = 0;
				}
			}
			

			do{
				pSegment = pDest;

				g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiStartTime =0;
				g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiEndTime = 0;

				if(pDest)
					pDest = strchr(pDest,',');
				
				if(pDest)
				{
					*pDest = '\0';
					pDest++;

					if(strlen(pSegment)==(2+1+2+1+2+3+2+1+2+1+2))
					{
						int hour1,minute1,second1;
						int hour2,minute2,second2;
						unsigned int ulPlanStart;
						unsigned int ulPlanEnd;

						sscanf(pSegment,"%02d:%02d:%02d<->%02d:%02d:%02d",\
							&hour1,&minute1,&second1,\
							&hour2,&minute2,&second2);

						ulPlanStart = hour1*60*60+minute1*60+second1;
						ulPlanEnd = hour2*60*60+minute2*60+second2;

						if(IS_VALID_TIME(hour1, minute1, second1)&&\
							IS_VALID_TIME(hour2, minute2, second2)&&\
							(ulPlanEnd>ulPlanStart))
						{
							g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiStartTime =ulPlanStart;
							g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiEndTime = ulPlanEnd;
						}
					}
				}
			}while((++k)<DEFAULT_PLANT_TIME_SEGMENT);
		}

		memset(szDefault,0x00,sizeof(szDefault));
		sprintf(szDefault,"%s",REC_FORMAT_STR(DEFAULT_REC_FORMAT));
		pszVal =cfgSockGetValue(szSection,\
												RecConfInfoKey[e_rRecFileFormat],\
												szDefault);
		g_sysConfModule->s_RecConfInfo[i].nRecFileFormat= REC_FORMAT_NUM(pszVal);
	}
	
	return 0;	
}


static int LoadVLConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    if(NULL==g_sysConfModule)
        return (-1);
    for (int i =0 ; i< MAX_VEHICLE_NUM; i++ )
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_VL_CONF_INFO],i);
            
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",i);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlVehicleId],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiCarRoadId= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_VL_DAY2NIGHT);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlThrDayToNight],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].usThrDayToNight= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_NIGHT2DAY);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlThrNightToDay],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].usThrNightToDay= atoi(pszVal);

        
        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_NIGHTTHRESH);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlCorrNightThresh],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiCorrNightThresh= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_DAYTHRESH);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlCorrDayThresh],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiCorrDayThresh= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_MOTOTHRESH);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlCorrMotorThresh],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiCorrMotorThresh= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_EVENTHRESH);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlCorrEvenThresh],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiCorrEveningThresh= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_DAYMOTOWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlDayMotorWidth],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiDayMotorWidth= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
         sprintf(szDefault,"%d",DEFAULT_VL_NIGHTMOTOWIDTH);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlNightMotorWidth],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].uiNightMotorWidth= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_VL_CAPDELAYI);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlCapDelayInterval],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].ucCapDelayInterval = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_VL_CAPDELAYI);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlCapDelayNum],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].cCapDelayNum = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_MIN_PLATE_SIZE);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlMinPlatePixelSize],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].cMinPlatePixelSize = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_MAX_PLATE_SIZE);
        pszVal =cfgSockGetValue(szSection,\
                                VLConfInfoKey[e_vlMaxPlatePixelSize],\
                                szDefault);
        g_sysConfModule->s_VlConfInfo[i].cMaxPlatePixelSize = atoi(pszVal);
    }
      
    return 0;
}
static  int  LoadPassPortFtpConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PASSPORT_FTP_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fEnable],\
                            szDefault);
    g_sysConfModule->s_PortFtpConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_SERVERIP);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpIp],\
                            szDefault);
    g_sysConfModule->s_PortFtpConfInfo.uiFtpIP= inet_addr(pszVal);
    g_sysConfModule->s_PortFtpConfInfo.uiFtpIP = ntohl(g_sysConfModule->s_PortFtpConfInfo.uiFtpIP);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",DEFAULT_FTP_PORT);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpPort],\
                            szDefault);
    g_sysConfModule->s_PortFtpConfInfo.usFtpPort= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_USER);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserName],\
                            szDefault);
    strncpy(g_sysConfModule->s_PortFtpConfInfo.szUserName,
            pszVal,
            sizeof(g_sysConfModule->s_PortFtpConfInfo.szUserName)
            );
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_PASSWD);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserPasswd],\
                            szDefault);
    strncpy(g_sysConfModule->s_PortFtpConfInfo.szUserPasswd,
               pszVal,
               sizeof(g_sysConfModule->s_PortFtpConfInfo.szUserPasswd)
               );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection, FtpConfInfoKey[e_nEpTimerEnable], szDefault);
    g_sysConfModule->s_PortFtpConfInfo.cTRecEnable = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 300);
    pszVal =cfgSockGetValue(szSection, FtpConfInfoKey[e_nEpTimerRecInt], szDefault);
    g_sysConfModule->s_PortFtpConfInfo.usTRecInterval = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_nEpTimerRecLast],\
                            szDefault);
    g_sysConfModule->s_PortFtpConfInfo.ucTRecLast = atoi(pszVal);
    
    return 0;
}

static  int  LoadPassPortBackupsFtpConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PASSPORT_BACKUPS_FTP_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fEnable],\
                            szDefault);
    g_sysConfModule->s_PortBackupsFtpConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_SERVERIP);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpIp],\
                            szDefault);
    g_sysConfModule->s_PortBackupsFtpConfInfo.uiFtpIP= inet_addr(pszVal);
    g_sysConfModule->s_PortBackupsFtpConfInfo.uiFtpIP = ntohl(g_sysConfModule->s_PortBackupsFtpConfInfo.uiFtpIP);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",DEFAULT_FTP_PORT);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpPort],\
                            szDefault);
    g_sysConfModule->s_PortBackupsFtpConfInfo.usFtpPort= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_USER);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserName],\
                            szDefault);
    strncpy(g_sysConfModule->s_PortBackupsFtpConfInfo.szUserName,
            pszVal,
            sizeof(g_sysConfModule->s_PortBackupsFtpConfInfo.szUserName)
            );
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_PASSWD);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserPasswd],\
                            szDefault);
    strncpy(g_sysConfModule->s_PortBackupsFtpConfInfo.szUserPasswd,
               pszVal,
               sizeof(g_sysConfModule->s_PortBackupsFtpConfInfo.szUserPasswd)
               );

    return 0;
}

static  int  LoadElecPoliceFtpConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_ELEC_POLICE_FTP_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fEnable],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceFtpConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_SERVERIP);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpIp],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceFtpConfInfo.uiFtpIP= inet_addr(pszVal);
    g_sysConfModule->s_ElecPoliceFtpConfInfo.uiFtpIP = ntohl(g_sysConfModule->s_ElecPoliceFtpConfInfo.uiFtpIP);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",DEFAULT_FTP_PORT);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpPort],\
                            szDefault);
    g_sysConfModule->s_ElecPoliceFtpConfInfo.usFtpPort= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_USER);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserName],\
                            szDefault);
    strncpy(g_sysConfModule->s_ElecPoliceFtpConfInfo.szUserName,
            pszVal,
            sizeof(g_sysConfModule->s_ElecPoliceFtpConfInfo.szUserName)
            );
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_PASSWD);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserPasswd],\
                            szDefault);
    strncpy(g_sysConfModule->s_ElecPoliceFtpConfInfo.szUserPasswd,
             pszVal,
             sizeof(g_sysConfModule->s_ElecPoliceFtpConfInfo.szUserPasswd)
           );

    return 0;
}

static  int  LoadEpBackupsFtpConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_BACKUPS_FTP_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fEnable],\
                            szDefault);
    g_sysConfModule->s_epBackupsFtpConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_SERVERIP);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpIp],\
                            szDefault);
    g_sysConfModule->s_epBackupsFtpConfInfo.uiFtpIP= inet_addr(pszVal);
    g_sysConfModule->s_epBackupsFtpConfInfo.uiFtpIP = ntohl(g_sysConfModule->s_epBackupsFtpConfInfo.uiFtpIP);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d",DEFAULT_FTP_PORT);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fFtpPort],\
                            szDefault);
    g_sysConfModule->s_epBackupsFtpConfInfo.usFtpPort= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_USER);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserName],\
                            szDefault);
    strncpy(g_sysConfModule->s_epBackupsFtpConfInfo.szUserName,
            pszVal,
            sizeof(g_sysConfModule->s_epBackupsFtpConfInfo.szUserName)
            );
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_FTP_PASSWD);
    pszVal =cfgSockGetValue(szSection,\
                            FtpConfInfoKey[e_fUserPasswd],\
                            szDefault);
    strncpy(g_sysConfModule->s_epBackupsFtpConfInfo.szUserPasswd,
               pszVal,
               sizeof(g_sysConfModule->s_epBackupsFtpConfInfo.szUserPasswd)
               );

    return 0;
}

static  int  LoadNtpConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_NTP_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NtpConfInfoKey[e_ntEnable],\
                            szDefault);
    g_sysConfModule->s_NtpConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s",DEFAULT_NTP_SERVER);
    pszVal =cfgSockGetValue(szSection,\
                            NtpConfInfoKey[e_ntNtpServer],\
                            szDefault);
    g_sysConfModule->s_NtpConfInfo.uiServerIP= inet_addr(pszVal);
    g_sysConfModule->s_NtpConfInfo.uiServerIP = ntohl(g_sysConfModule->s_NtpConfInfo.uiServerIP);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 24*60);
    pszVal =cfgSockGetValue(szSection,\
                            NtpConfInfoKey[e_ntpJudgeCycle],\
                            szDefault);
    g_sysConfModule->s_NtpConfInfo.usJudgeCycle= atoi(pszVal);
    if( g_sysConfModule->s_NtpConfInfo.usJudgeCycle <= 0 )
    {
        g_sysConfModule->s_NtpConfInfo.usJudgeCycle = 1440;
    }

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            NtpConfInfoKey[e_ntpJudegPrecision],\
                            szDefault);
    g_sysConfModule->s_NtpConfInfo.ucJudegPrecision = atoi(pszVal);

    return 0;
}

static  int  LoadCamTimeRangeConf(void)
{
    int i =0;
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for(i =0; i < MAX_CAM_GROUP_NUM; i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_CAM_TIME_RANGE_INFO], i);
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", i);
        pszVal =cfgSockGetValue(szSection,\
                                CamTimeRangeKey[e_camTGroup],\
                                szDefault);
        g_sysConfModule->s_CamTimeRange[i].cGroupId = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                CamTimeRangeKey[e_camTEnable],\
                                szDefault);
        g_sysConfModule->s_CamTimeRange[i].cEnable = atoi(pszVal);
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%s", camNameInfoKey[i]);
        pszVal =cfgSockGetValue(szSection,\
                                CamTimeRangeKey[e_camTNmae],\
                                szDefault);
        strncpy(g_sysConfModule->s_CamTimeRange[i].szSceneName, pszVal,
                    sizeof(g_sysConfModule->s_CamTimeRange[i].szSceneName));

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                CamTimeRangeKey[e_camTBeginTime],\
                                szDefault);
        g_sysConfModule->s_CamTimeRange[i].iBeginTime = atoi(pszVal);


        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 24*3600-1);
        pszVal =cfgSockGetValue(szSection,\
                                CamTimeRangeKey[e_camTEndTime],\
                                szDefault);
        g_sysConfModule->s_CamTimeRange[i].iLastTime = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        pszVal =cfgSockGetValue(szSection,\
                                CamTimeRangeKey[e_irisPos],\
                                szDefault);
        g_sysConfModule->s_CamTimeRange[i].cIrisPos = atoi(pszVal);

    }
    return 0;
}

static  int  LoadEpAssistRedLightConf(void)
{
    int i =0;
    int j =0;
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for(i =0; i < MAX_VEHICLE_NUM; i++)
    {
        for(j =0; j< EP_ASSIST_RED_LIGHT_NUM; j++)
        {
            memset(szSection,0x00,sizeof(szSection));
            sprintf(szSection,"%s%02d",sys_ini_section[e_EP_ASSIST_REDLIGHT_INFO], i);
            
            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cEpAssistRedLightNum]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].cRedLightNum= atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cEpARLEnable]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].cEnableFlag= atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLBeginTime1]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].iBeginTime1 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLEndTime1]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].iLastTime1 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLBeginTime2]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].iBeginTime2 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLEndTime2]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].iLastTime2 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLBeginTime3]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].iBeginTime3 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLEndTime3]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpAssistRedLight[i][j].iLastTime3 = atoi(pszVal);
            
        }
    }
    return 0;
}

static  int  LoadEpSOTNoEntryConf(void)
{
    int i =0;
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for(i =0; i < MAX_VEHICLE_NUM; i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_EP_SOT_NO_ENTRY_INFO], i);
        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cEpSOTNEEnable]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].cEnableFlag= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEBeginTime1]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].iBeginTime1 = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEEndTime1]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].iLastTime1 = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEBeginTime2]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].iBeginTime2 = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEEndTime2]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].iLastTime2 = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEBeginTime3]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].iBeginTime3 = atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", 0);
        memset(szInfoKey, 0x00, sizeof(szInfoKey));
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEEndTime3]
                );
        pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
        g_sysConfModule->s_EpSOTNoEntry[i].iLastTime3 = atoi(pszVal);

    }
    return 0;
}

static  int  LoadEpTimeRangeConf(void)
{
    int i =0;
    int j =0;
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for(i =0; i < MAX_VEHICLE_NUM; i++)
    {
        for(j =0; j< EP_FORBID_MAX_DIR; j++)
        {
            memset(szSection,0x00,sizeof(szSection));
            sprintf(szSection,"%s%02d",sys_ini_section[e_EP_TIME_RANGE_INFO], i);
            
            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", j+1);
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    "Type"
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].cForbidType = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpRange1Enable]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].cRange1Enable = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpRange2Enable]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].cRange2Enable = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpBeginTime1]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].iBeginTime1 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpLastTime1]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].iLastTime1 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpBeginTime2]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].iBeginTime2 = atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            sprintf(szDefault,"%d", 0);
            memset(szInfoKey, 0x00, sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpLastTime2]
                    );
            pszVal =cfgSockGetValue(szSection, szInfoKey, szDefault);
            g_sysConfModule->s_EpTimeRange[i][j].iLastTime2 = atoi(pszVal);
        }
    }
    return 0;
}

static  int  LoadEPVLoopStepInterverConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_VLOOP_STEP_INFO]);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane0Stp01],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane0Step01 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane0Stp12],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane0Step12 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane1Stp01],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane1Step01 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane1Stp12],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane1Step12 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane2Stp01],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane2Step01 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane2Stp12],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane2Step12 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane3Stp01],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane3Step01 = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysVEpStepInterKey[e_lane3Stp12],\
                            szDefault);
    g_sysConfModule->s_EpVLoopStepInfo.uiLane3Step12 = atoi(pszVal);
    
    return 0;
}

static  int  LoadPpFuncEnConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PP_FUNC_CONF_INFO]);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_OsdPlateScopeEn],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucOsdPlateScopeEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 50);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_PlateScopeIncSize],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.usPlateScopeIncSize = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 10);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_RadarSpeedGap],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucRadarSpeedGap = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 32);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_RadarForwardLim],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucRadarForwardLim = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 10);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_RadarBackwardLim],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucRadarBackwardLim = atoi(pszVal);    

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_isShieldReverse],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucisShieldReverse = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_isDecTaillight],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucIsDecTaillight = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_faceIdenEn],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucFaceIdenEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_OlviaRadaParalEn],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucOlviaRadaParalEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_faceOverlayEn],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.ucFaceOvenlayEn = atoi(pszVal);
	
	memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            PpFuncEnableKey[e_isReverseSnapNorm],\
                            szDefault);
    g_sysConfModule->s_PpFuncConfInfo.usIsReverseSnapNorm = atoi(pszVal);
    
    return 0;
}

static  int  LoadFuncEnableConf(void)
{
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];        
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_FUNC_ENABLE_FLAG_INFO]);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_flowRate],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cFlowRateFlag = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_alarmRec],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cAlarmRecFlag = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_speedAmend],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cSpeedAmendFlag = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 5);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_flowRateInter],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cFlowRateInter = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_xmlFileSendFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cXmlFileSendFlag = atoi(pszVal);

#if defined(CYC500XKW)||defined(CYC500XLKW)||defined(CYC500KX2)||defined(CYC500JX)
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1726);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_flashStartLine],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFlashStartLine = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1726);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_freqFlashStartLine],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFreqFlashStartLine = atoi(pszVal);
#elif defined(CYC800JX)||defined(CYC800XKW4C)
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 3096);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_flashStartLine],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFlashStartLine = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 3096);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_freqFlashStartLine],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFreqFlashStartLine = atoi(pszVal);
#else
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1210);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_flashStartLine],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFlashStartLine = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1210);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_freqFlashStartLine],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFreqFlashStartLine = atoi(pszVal);
#endif

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_dspFrmDiffEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucDspFramDifEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 5);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_dspStopDistance],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucDspStopDistan = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_dspOneLampEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucDspOneLampEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_redLightDelay],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucRedLightDelay = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_redTurnGreenSnapEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucRedTurnGreenSnapEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_noDirReportSnapEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucNoDirReportSnapEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 3000);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_freqFlashDelaySec],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usFreqFlashDelayUSec= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 3000);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_ppFlashLightDelaySec],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usppFLightDelayUSec = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 3000);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_epFlashLightDelaySec],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usepFLightDelayUSec = atoi(pszVal);


    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_debugShellLogSaveFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucDebugLogSaveFlag = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_arithDebugShowFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucArithDebugShowFlag = atoi(pszVal);


    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_vdlGreenDectFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucVDLGreenMode = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_pressLineSanpFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucPressLineSanpFlag= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_backWordsSanpFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucBackWordsSanpFlag = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_epPpSnap0Or1],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucEpPpSnap0Or1= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_UdiskWeldSta],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucUdiskWeldSta = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_epCarSpeedOsdEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucEpCarSpeedOsdEn= atoi(pszVal);
	
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_uDiskEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucUdiskEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 2);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_EpPpSnapPosition],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucEpPpSnapPosition= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 200);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_RedLReviseShutter],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usRedLReviseShutter = atoi(pszVal);

	memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 5004);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_NetVehiclePort],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usNetVehiclePort = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_UFdiskEnFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucUFdiskEn = atoi(pszVal);
	
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 5);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_GammaVal],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cGammaVal = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_flashLinesMode],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cFlashLineMode = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_DyncJudgeFlashLines],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cDyncJudFlashLines = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 5);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_speedAmendX],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cSpeedAmendX= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 5);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_speedAmendY],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cSpeedAmendY= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1000);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_ppCsSpeedAmend],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usCsSpeedAmendNumerator = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_epPpPicMode],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucEpPpPicMode = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 256);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_fpgaSatuPam],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usSaturation = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 3300);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_fpgaGPixThresh],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.usGPixsThresh = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_dubalFModeEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cDubalFEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%f", 0.004);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_dubalFShutVal],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.fDubalFShut = atof(pszVal);
    
    if( g_sysConfModule->s_FuncEnableInfo.fDubalFShut > 0.005 )
    {
        g_sysConfModule->s_FuncEnableInfo.fDubalFShut = 0.005;
    }
    
    if( g_sysConfModule->s_FuncEnableInfo.fDubalFShut < 0.00001 )
    {
        g_sysConfModule->s_FuncEnableInfo.fDubalFShut = 0.00001;
    }

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%f", 10.0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_dubalFGainVal],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.fDubalFGain = atof(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 100);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_redDectSaturation],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cRedDecSaturation = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 60);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_redDectBright],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cRedDecBright = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_PlateSegFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucPlateSegFlag = atoi(pszVal);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 70);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_PicQuality],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucPicQuality = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 4);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_RedLReviseDegree],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucRedLReviseDegree = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_ChangeRoadSnapFlag],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucChangeRoad = atoi(pszVal);

    for ( int i=0; i<4; i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",0);
        sprintf(szInfoKey,"%s%02d",
                SysFuncEnableKey[e_crSnapMode],
                i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_FuncEnableInfo.cLaneChgType[i] = atoi(pszVal);
    }

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_PicAssistDebug],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucPicAssistDebug = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_CrossLineParkEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucCrossLineParkEn = atoi(pszVal);

    for ( int i=0; i<5; i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",1);
        sprintf(szInfoKey,"%s%02d",
                SysFuncEnableKey[e_cPressLine],
                i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_FuncEnableInfo.aucPressLine[i] = atoi(pszVal);
    }

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_SDEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucSDEn= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_SDFdiskEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.ucSDFdiskEn = atoi(pszVal);
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_cOnvifEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cOnvifEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_cSipSdkEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cSipSdkEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_cV2En],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cV2En = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_cV3En],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cV3En = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_HaloTakeOutEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cHaloTakeOutEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 32);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_HaloAreaRange],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cHaloAreaRange = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_cGB28181En],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cGB28181En = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_RestartTimeEn],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.cRestartTimeEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 86399);
    pszVal =cfgSockGetValue(szSection,\
                            SysFuncEnableKey[e_RestartTime],\
                            szDefault);
    g_sysConfModule->s_FuncEnableInfo.uiRestartTime = atoi(pszVal);

    return 0;
}

static  int  LoadCamCtrlConf(void)
{
    int i =0;
    int j =0;
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];    
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_CAMERA_CTRL_CONF_INFO]);
    
    for (i =0 ; i < MAX_CAM_GROUP_NUM ; i++ )
    {
        for (j =0 ; j < MAX_CAM_CTRL_NUM ; j++ )
        {

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DefaultStrCameraConfVal[i][j].cCamGroup);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camGroup]);
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].cCamGroup= atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DefaultStrCameraConfVal[i][j].uiCamaraType);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camType]);
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].uiCamaraType= atoi(pszVal);

            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%d",DefaultStrCameraConfVal[i][j].cCtlType);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camManner]);
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].cCtlType= atoi(pszVal);


            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%f",DefaultStrCameraConfVal[i][j].fMaxVal);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camMaxVal]);
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMaxVal= atof(pszVal);


            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%f",DefaultStrCameraConfVal[i][j].fMinVal);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camMinVal]);
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMinVal= atof(pszVal);


            memset(szDefault,0x00,sizeof(szDefault));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            sprintf(szDefault,"%f",DefaultStrCameraConfVal[i][j].fFixVal);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camFixVal]);
            pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fFixVal= atof(pszVal);

        }
    }
    return 0;

}


static  int  LoadPicNetHddConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PIC_NET_HDD_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdEnable],\
                            szDefault);
    g_sysConfModule->s_PicNetHddConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdType],\
                            szDefault);
    g_sysConfModule->s_PicNetHddConfInfo.uiStorageType= atoi(pszVal);

    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdIpAddr],\
                            szDefault);
    g_sysConfModule->s_PicNetHddConfInfo.uiIpAddr = inet_addr(pszVal);
    g_sysConfModule->s_PicNetHddConfInfo.uiIpAddr = ntohl(g_sysConfModule->s_PicNetHddConfInfo.uiIpAddr);
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdFolder],\
                            szDefault);
   strncpy(g_sysConfModule->s_PicNetHddConfInfo.szFolderName,
            pszVal,
            sizeof(g_sysConfModule->s_PicNetHddConfInfo.szFolderName)
            );
   memset(szDefault,0x00,sizeof(szDefault));
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdUserName],\
                            szDefault);
   strncpy(g_sysConfModule->s_PicNetHddConfInfo.szUserName,
            pszVal,
            sizeof(g_sysConfModule->s_PicNetHddConfInfo.szUserName)
            );

   memset(szDefault,0x00,sizeof(szDefault));
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdPassWord],\
                            szDefault);
   strncpy(g_sysConfModule->s_PicNetHddConfInfo.szPassWord,
            pszVal,
            sizeof(g_sysConfModule->s_PicNetHddConfInfo.szPassWord)
            );

   memset(szDefault,0x00,sizeof(szDefault));
   sprintf(szDefault,"%d",0);
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdAvaiable],\
                            szDefault);
    g_sysConfModule->s_PicNetHddConfInfo.uiAvaiable = atoi(pszVal);

   memset(szDefault,0x00,sizeof(szDefault));
   sprintf(szDefault,"%d", 1);
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_uDiskCheckSum],\
                            szDefault);
    g_sysConfModule->s_PicNetHddConfInfo.uiUDiskSum = atoi(pszVal);
    
    return 0;

}
static  int  LoadRecNetHddConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_REC_NET_HDD_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdEnable],\
                            szDefault);
    g_sysConfModule->s_RecNetHddConfInfo.uiEnable= atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdType],\
                            szDefault);
    g_sysConfModule->s_RecNetHddConfInfo.uiStorageType= atoi(pszVal);

    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdIpAddr],\
                            szDefault);
     g_sysConfModule->s_RecNetHddConfInfo.uiIpAddr = inet_addr(pszVal);
     g_sysConfModule->s_RecNetHddConfInfo.uiIpAddr = ntohl(g_sysConfModule->s_RecNetHddConfInfo.uiIpAddr);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdFolder],\
                            szDefault);
    strncpy(g_sysConfModule->s_RecNetHddConfInfo.szFolderName,
            pszVal,
            sizeof(g_sysConfModule->s_RecNetHddConfInfo.szFolderName)
            );
   memset(szDefault,0x00,sizeof(szDefault));
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdUserName],\
                            szDefault);
   strncpy(g_sysConfModule->s_RecNetHddConfInfo.szUserName,
            pszVal,
            sizeof(g_sysConfModule->s_RecNetHddConfInfo.szUserName)
            );
   memset(szDefault,0x00,sizeof(szDefault));
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdPassWord],\
                            szDefault);
   strncpy(g_sysConfModule->s_RecNetHddConfInfo.szPassWord,
            pszVal,
            sizeof(g_sysConfModule->s_RecNetHddConfInfo.szPassWord)
            );
            
   memset(szDefault,0x00,sizeof(szDefault));
   sprintf(szDefault,"%d",0);
   pszVal =cfgSockGetValue(szSection,\
                            NetHddConfInfoKey[e_hdAvaiable],\
                            szDefault);
    g_sysConfModule->s_RecNetHddConfInfo.uiAvaiable = atoi(pszVal);
    return 0;

}

static  int  LoadNRMConf(void)
{
	int i =0;
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_NRM_CONF_INFO]);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NrmConfInfoKey[e_NRMEn],\
                            szDefault);
    g_sysConfModule->s_NrmConfInfo.cEnable = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NrmConfInfoKey[e_MainNRMIP],\
                            szDefault);
    g_sysConfModule->s_NrmConfInfo.uiMainNRMIp = inet_addr(pszVal);
    g_sysConfModule->s_NrmConfInfo.uiMainNRMIp = ntohl(g_sysConfModule->s_NrmConfInfo.uiMainNRMIp);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            NrmConfInfoKey[e_BakNRMIP],\
                            szDefault);
    g_sysConfModule->s_NrmConfInfo.uiBakNRMIp = inet_addr(pszVal);
    g_sysConfModule->s_NrmConfInfo.uiBakNRMIp = ntohl(g_sysConfModule->s_NrmConfInfo.uiBakNRMIp);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 7776);
    pszVal =cfgSockGetValue(szSection,\
                            NrmConfInfoKey[e_MainNRMPort],\
                            szDefault);
    g_sysConfModule->s_NrmConfInfo.usMainNRMPort = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 7776);
    pszVal =cfgSockGetValue(szSection,\
                            NrmConfInfoKey[e_BakNRMPort],\
                            szDefault);
    g_sysConfModule->s_NrmConfInfo.usBakNRMPort = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","120001000100000004");
    pszVal =cfgSockGetValue(szSection,\
                            NrmConfInfoKey[e_NRUPUId],\
                            szDefault);
    strncpy(g_sysConfModule->s_NrmConfInfo.szPuID,
            pszVal,
            sizeof(g_sysConfModule->s_NrmConfInfo.szPuID)
            );

	for (i =0 ; i< MAX_CHANNEL_NUM; i++ )
    {
	    memset(szDefault,0x00,sizeof(szDefault));
	    memset(szInfoKey,0x00,sizeof(szInfoKey));
	    sprintf(szInfoKey,"%s_%d", NrmConfInfoKey[e_LastNRUIp], i);
	    pszVal =cfgSockGetValue(szSection,\
	                            szInfoKey,\
	                            szDefault);
	    g_sysConfModule->s_NrmConfInfo.uiLastNRUIp[i] = inet_addr(pszVal);
	    g_sysConfModule->s_NrmConfInfo.uiLastNRUIp[i] = ntohl(g_sysConfModule->s_NrmConfInfo.uiLastNRUIp[i]);

	    memset(szDefault,0x00,sizeof(szDefault));
	    memset(szInfoKey,0x00,sizeof(szInfoKey));
	    sprintf(szInfoKey,"%s_%d", NrmConfInfoKey[e_LastNRUHttpPort], i);
	    sprintf(szDefault,"%d", 7776);
	    pszVal =cfgSockGetValue(szSection,\
	                            szInfoKey,\
	                            szDefault);
	    g_sysConfModule->s_NrmConfInfo.usLastNRUHttpPort[i] = atoi(pszVal);
	}
    return 0;

}

static  int  LoadDctlConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for ( int i=0;i<MAX_DCTL_NUM;i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_DCTL_CONF_INFO],i);

        memset(szDefault,0x00,sizeof(szDefault));
        if(i == 0)
        {
            sprintf(szDefault,"%d", 1);
        }
        else
        {
            sprintf(szDefault,"%d", 0);
        }
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dEnable],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].cEnable= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_COMID);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dComId],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].cComId= atoi(pszVal);

        
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d", DEFAULT_COIL_TYPE);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dVehDect],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].uiVehicleType= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_DATABITS);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dDatabits],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].cDatabits= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_STOPBITS);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dStopbits],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].cStopbits= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_PARITY);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dParity],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].cParity= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",DEFAULT_BAUDRATE);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dBaudrate],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].uiBaudrate= atoi(pszVal);

        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",200);
        pszVal =cfgSockGetValue(szSection,\
                                DctlConfInfoKey[e_dMaxMeasureSpeed],\
                                szDefault);
        g_sysConfModule->s_DctlConfInfo[i].usMaxMeasureSpeed = atoi(pszVal);
    }

   return 0;
}

static  int  LoadPlatformConf(void)
{
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

   
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PLATFORM_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            PlatformConfInfoKey[e_p330Enable],\
                            szDefault);
    g_sysConfModule->s_Platform330ConfInfo.cEnable = atoi(pszVal);
    
    for ( int i=0;i<4;i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",1);
        sprintf(szInfoKey,"%s%02d",
                PlatformConfInfoKey[e_p330DriveWayId],
                i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_Platform330ConfInfo.sz330DriveWayId[i] = atoi(pszVal);
    }
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            PlatformConfInfoKey[e_p330DevId],\
                            szDefault);
    strncpy(g_sysConfModule->s_Platform330ConfInfo.sz330DevId,
            pszVal,
            sizeof(g_sysConfModule->s_Platform330ConfInfo.sz330DevId));

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            PlatformConfInfoKey[e_p330DirId],\
                            szDefault);
    strncpy(g_sysConfModule->s_Platform330ConfInfo.sz330DirId,
               pszVal,
               sizeof(g_sysConfModule->s_Platform330ConfInfo.sz330DirId));
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            PlatformConfInfoKey[e_p330Key],\
                            szDefault);
   
   strncpy(g_sysConfModule->s_Platform330ConfInfo.sz330Key,
              pszVal,
              sizeof(g_sysConfModule->s_Platform330ConfInfo.sz330Key));
   return 0;
   
}

static  int  LoadEp330Conf(void)
{
    char szSection[32];
    char szDefault[256];
    char szInfoKey[256];
    const char *pszVal = 0;

   
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_330_CONF_INFO]);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            ep330ConfInfoKey[e_epP330Enable],\
                            szDefault);
    g_sysConfModule->s_EpPlatform330ConfInfo.cEnable = atoi(pszVal);
    
    for ( int i=0;i<4;i++)
    {
        memset(szDefault,0x00,sizeof(szDefault));
        sprintf(szDefault,"%d",1);
        sprintf(szInfoKey,"%s%02d",
                ep330ConfInfoKey[e_epP330DriveWayId],
                i);
        pszVal =cfgSockGetValue(szSection,szInfoKey,szDefault);
        g_sysConfModule->s_EpPlatform330ConfInfo.sz330DriveWayId[i] = atoi(pszVal);
    }
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            ep330ConfInfoKey[e_epP330DevId],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpPlatform330ConfInfo.sz330DevId,
            pszVal,
            sizeof(g_sysConfModule->s_EpPlatform330ConfInfo.sz330DevId));

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            ep330ConfInfoKey[e_epP330DirId],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpPlatform330ConfInfo.sz330DirId,
               pszVal,
               sizeof(g_sysConfModule->s_EpPlatform330ConfInfo.sz330DirId));
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            ep330ConfInfoKey[e_epP330Key],\
                            szDefault);
   
   strncpy(g_sysConfModule->s_EpPlatform330ConfInfo.sz330Key,
              pszVal,
              sizeof(g_sysConfModule->s_EpPlatform330ConfInfo.sz330Key));
   return 0;
   
}

/* begin add by wangb 2010-3-22 for v3 platform */
static  int  LoadV3Conf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

   
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_V3_CONF_INFO]);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", DEFAULT_V3_SERVERIP);
    pszVal =cfgSockGetValue(szSection, v3ConfInfoKey[e_v3ServerIp], szDefault);
    g_sysConfModule->s_PlatformV3ConfInfo.uiV3SerIp = inet_addr(pszVal);
    g_sysConfModule->s_PlatformV3ConfInfo.uiV3SerIp = ntohl(g_sysConfModule->s_PlatformV3ConfInfo.uiV3SerIp);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", DEFAULT_V3_SERVERPORT);
    pszVal =cfgSockGetValue(szSection,\
                            v3ConfInfoKey[e_v3ServerPort],\
                            szDefault);
    g_sysConfModule->s_PlatformV3ConfInfo.usV3Port = atoi(pszVal);
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v3ConfInfoKey[e_v3DevNo],\
                            szDefault);
    strncpy(g_sysConfModule->s_PlatformV3ConfInfo.szDevNo,
               pszVal,
               sizeof(g_sysConfModule->s_PlatformV3ConfInfo.szDevNo));
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v3ConfInfoKey[e_v3DevPwd],\
                            szDefault);
    strncpy(g_sysConfModule->s_PlatformV3ConfInfo.szDevPwd,
               pszVal,
               sizeof(g_sysConfModule->s_PlatformV3ConfInfo.szDevPwd));

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v3ConfInfoKey[e_v3ServerSeq],\
                            szDefault);
    g_sysConfModule->s_PlatformV3ConfInfo.cV3ServerSeq = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v3ConfInfoKey[e_v3TimeSyn],\
                            szDefault);
    g_sysConfModule->s_PlatformV3ConfInfo.cV3TimeSynFlag = atoi(pszVal);
    

   memset(szDefault,0x00,sizeof(szDefault));
   pszVal =cfgSockGetValue(szSection,\
                           v3ConfInfoKey[e_v3Enable],\
                           szDefault);
   g_sysConfModule->s_PlatformV3ConfInfo.cV3Enable= atoi(pszVal);

   return 0;
   
}
/* end add by wangb 2010-3-22 for v3 platform */

/* begin add by wangb 2012-1-10 for v2 platform */
static  int  LoadV2Conf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

   
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_V2_CONF_INFO]);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", DEFAULT_V2_SERVERIP);
    pszVal =cfgSockGetValue(szSection, v2ConfInfoKey[e_v2ServerIp], szDefault);
    g_sysConfModule->s_PlatformV2ConfInfo.uiV3SerIp = inet_addr(pszVal);
    g_sysConfModule->s_PlatformV2ConfInfo.uiV3SerIp = ntohl(g_sysConfModule->s_PlatformV2ConfInfo.uiV3SerIp);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", DEFAULT_V2_SERVERPORT);
    pszVal =cfgSockGetValue(szSection,\
                            v2ConfInfoKey[e_v2ServerPort],\
                            szDefault);
    g_sysConfModule->s_PlatformV2ConfInfo.usV3Port = atoi(pszVal);
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v2ConfInfoKey[e_v2DevNo],\
                            szDefault);
    strncpy(g_sysConfModule->s_PlatformV2ConfInfo.szDevNo,
               pszVal,
               sizeof(g_sysConfModule->s_PlatformV2ConfInfo.szDevNo));
    
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v2ConfInfoKey[e_v2DevPwd],\
                            szDefault);
    strncpy(g_sysConfModule->s_PlatformV2ConfInfo.szDevPwd,
               pszVal,
               sizeof(g_sysConfModule->s_PlatformV2ConfInfo.szDevPwd));

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v2ConfInfoKey[e_v2ServerSeq],\
                            szDefault);
    g_sysConfModule->s_PlatformV2ConfInfo.cV3ServerSeq = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            v2ConfInfoKey[e_v2TimeSyn],\
                            szDefault);
    g_sysConfModule->s_PlatformV2ConfInfo.cV3TimeSynFlag = atoi(pszVal);
    

   memset(szDefault,0x00,sizeof(szDefault));
   pszVal =cfgSockGetValue(szSection,\
                           v2ConfInfoKey[e_v2Enable],\
                           szDefault);
   g_sysConfModule->s_PlatformV2ConfInfo.cV3Enable= atoi(pszVal);

   return 0;
   
}
/* end add by wangb 2010-3-22 for v2 platform */

static  int  LoadGb28181Conf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

   
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_GB28181_CONF_INFO]);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "34040000002000000001");
    pszVal =cfgSockGetValue(szSection, gb28181ConfInfoKey[e_Gb28181ServerId], szDefault);
    strncpy(g_sysConfModule->s_Gb28181ConfInfo.szServerId,
               pszVal,
               sizeof(g_sysConfModule->s_Gb28181ConfInfo.szServerId));

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "34040000002000000001");
    pszVal =cfgSockGetValue(szSection, gb28181ConfInfoKey[e_Gb28181ServerDomain], szDefault);
    strncpy(g_sysConfModule->s_Gb28181ConfInfo.szServerDomain,
               pszVal,
               sizeof(g_sysConfModule->s_Gb28181ConfInfo.szServerDomain));


    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "255.255.255.0");
    pszVal =cfgSockGetValue(szSection, gb28181ConfInfoKey[e_Gb28181ServerIp], szDefault);
    strncpy(g_sysConfModule->s_Gb28181ConfInfo.szServerIp,
               pszVal,
               sizeof(g_sysConfModule->s_Gb28181ConfInfo.szServerIp));

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 5160);
    pszVal =cfgSockGetValue(szSection,\
                            gb28181ConfInfoKey[e_Gb28181ServerPort],\
                            szDefault);
    g_sysConfModule->s_Gb28181ConfInfo.uiServerPort = atoi(pszVal);
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 7200);
    pszVal =cfgSockGetValue(szSection,\
                            gb28181ConfInfoKey[e_Gb28181RegisterExpiry],\
                            szDefault);
    g_sysConfModule->s_Gb28181ConfInfo.uiRegisterExpiry = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 20);
    pszVal =cfgSockGetValue(szSection,\
                            gb28181ConfInfoKey[e_Gb28181KeepLiveTime],\
                            szDefault);
    g_sysConfModule->s_Gb28181ConfInfo.uiKeepLiveTime = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            gb28181ConfInfoKey[e_Gb28181ConMode],\
                            szDefault);
    g_sysConfModule->s_Gb28181ConfInfo.uiConMode = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "34010000001320000100");
    pszVal =cfgSockGetValue(szSection, gb28181ConfInfoKey[e_Gb28181ClientId], szDefault);
    strncpy(g_sysConfModule->s_Gb28181ConfInfo.szClientId,
               pszVal,
               sizeof(g_sysConfModule->s_Gb28181ConfInfo.szClientId));

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "34010000001320000100");
    pszVal =cfgSockGetValue(szSection, gb28181ConfInfoKey[e_Gb28181ClientUserName], szDefault);
    strncpy(g_sysConfModule->s_Gb28181ConfInfo.szClientUserName,
               pszVal,
               sizeof(g_sysConfModule->s_Gb28181ConfInfo.szClientUserName));

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "123456");
    pszVal =cfgSockGetValue(szSection, gb28181ConfInfoKey[e_Gb28181ClientPasswd], szDefault);
    strncpy(g_sysConfModule->s_Gb28181ConfInfo.szClientPasswd,
               pszVal,
               sizeof(g_sysConfModule->s_Gb28181ConfInfo.szClientPasswd));

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            gb28181ConfInfoKey[e_Gb28181PackMode],\
                            szDefault);
    g_sysConfModule->s_Gb28181ConfInfo.uiPackMode = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            gb28181ConfInfoKey[e_Gb28181EnFlag],\
                            szDefault);
    g_sysConfModule->s_Gb28181ConfInfo.cEnFlag = atoi(pszVal);


   return 0;
   
}
/* end add by wangb 2010-3-22 for v2 platform */

/* begin add by wangb 2010-8-6 for nm platform */
static  int  LoadNMConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

   
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_NM_CONF_INFO]);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", DEFAULT_NM_SERVERIP);
    pszVal =cfgSockGetValue(szSection, nmConfInfoKey[e_nMServerIp], szDefault);
    g_sysConfModule->s_sysNMConfInfo.uiNMSerIp = inet_addr(pszVal);
    g_sysConfModule->s_sysNMConfInfo.uiNMSerIp = ntohl(g_sysConfModule->s_sysNMConfInfo.uiNMSerIp);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 9090);
    pszVal =cfgSockGetValue(szSection,\
                            nmConfInfoKey[e_nMServerPort],\
                            szDefault);
    g_sysConfModule->s_sysNMConfInfo.usNMPort = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            nmConfInfoKey[e_nMEnable],\
                            szDefault);
    g_sysConfModule->s_sysNMConfInfo.cEnable = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            nmConfInfoKey[e_defaultGWEn],\
                            szDefault);
    g_sysConfModule->s_sysNMConfInfo.cDefaultGWEn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", DEFAULT_NM_SERVERIP);
    pszVal =cfgSockGetValue(szSection, nmConfInfoKey[e_defaultGWIp], szDefault);
    g_sysConfModule->s_sysNMConfInfo.uiDefaultGWIp = inet_addr(pszVal);
    g_sysConfModule->s_sysNMConfInfo.uiDefaultGWIp = ntohl(g_sysConfModule->s_sysNMConfInfo.uiDefaultGWIp);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%d", 80);
    pszVal =cfgSockGetValue(szSection,\
                            nmConfInfoKey[e_defaultHttpPort],\
                            szDefault);
    g_sysConfModule->s_sysNMConfInfo.uiDefaultHttpdPort = atoi(pszVal);
    if( g_sysConfModule->s_sysNMConfInfo.uiDefaultHttpdPort > 65535 )
    {
        g_sysConfModule->s_sysNMConfInfo.uiDefaultHttpdPort = 65535;
    }

   return 0;
}
/* end add by wangb 2010-8-6 for nm platform */

/* 添加卡口违章类型代码及描述信息配置 */
static  int  LoadPPIrregInfoConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PP_IRREG_CONF_INFO]);
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1303");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppOverSpeedSmallPercent50Code],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedSmallPercent50Code,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedSmallPercent50Code)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车行驶超过规定时速50%以下的");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppOverSpeedSmallPercent50Name],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedSmallPercent50Name,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedSmallPercent50Name)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1603");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppOverSpeedLargePercent50Code],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedLargePercent50Code,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedLargePercent50Code)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车行驶超过规定时速50%的");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppOverSpeedLargePercent50Name],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedLargePercent50Name,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szOverSpeedLargePercent50Name)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1301");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppReverseRunCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szReverseRunCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szReverseRunCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车逆向行驶的");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppReverseRunName],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szReverseRunName,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szReverseRunName)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1344");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppInfringeProhibCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szInfringeProhibCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szInfringeProhibCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车违反禁令标志指示的");
    pszVal =cfgSockGetValue(szSection,\
                            SysPpIrregInfoKey[e_ppInfringeProhibName],\
                            szDefault);
    strncpy(g_sysConfModule->s_PpIrregConfInfo.szInfringeProhibName,\
            pszVal,\
            sizeof(g_sysConfModule->s_PpIrregConfInfo.szInfringeProhibName)\
            );            
    
    return 0;
}
/*End of wb on 2012-5-17 9:57 2.0.0.1*/

/*Start of wb on 2012-5-17 9:57 2.0.0.1*/
/* 添加电警违章类型代码及描述信息配置 */
static  int  LoadEPIrregInfoConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_IRREG_CONF_INFO]);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epIrregDispFlag],\
                            szDefault);
    g_sysConfModule->s_EpIrregConfInfo.cDispFlag = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1625");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epRunRDCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szRunRedCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szRunRedCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","驾驶机动车违反道路交通信号灯通行的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epRunRDName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szRunRedName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szRunRedName)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1301");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epReverseRunCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szReverseRunCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szReverseRunCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车逆向行驶的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epReverseRunName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szReverseRunName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szReverseRunName)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1230");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epInfringForbidLineCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szInfringForbidLineCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szInfringForbidLineCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车违反禁止标线指示的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epInfringForbidLineName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szInfringForbidLineName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szInfringForbidLineName)\
            );
            
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1208");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epInfringDirLaneRunCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szInfringDirLaneRunCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szInfringDirLaneRunCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","机动车不按导向车道行驶");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epInfringDirLaneRunName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szInfringDirLaneRunName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szInfringDirLaneRunName)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1228");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epStopOnTrafficNoEntryCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szStopOnTrafficNoEntryCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szStopOnTrafficNoEntryCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","路口遇有交通阻塞时未依次等候的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_epStopOnTrafficNoEntryName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szStopOnTrafficNoEntryName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szStopOnTrafficNoEntryName)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","4312");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_HighwayInfringDirLaneCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szHighwayInfringDirLaneCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szHighwayInfringDirLaneCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","高速公路上不按规定车道行驶的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_HighwayInfringDirLaneName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szHighwayInfringDirLaneName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szHighwayInfringDirLaneName)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","1039");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_StoppedVehicleCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szStoppedVehicleCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szStoppedVehicleCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","违反规定停放、临时停车，妨碍其它车辆、行人通行的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpIrregInfoKey[e_StoppedVehicleName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpIrregConfInfo.szStoppedVehicleName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpIrregConfInfo.szStoppedVehicleName)\
            );
    
    return 0;
}
/*End of wb on 2012-5-17 9:57 2.0.0.1*/

/* 添加电警新增违章类型代码及描述信息配置 */
static  int  LoadEPNewAddIrregInfoConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_NEWADD_IRREG_CONF_INFO]);
    
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","8052");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpNewAddIrregInfoKey[e_epRunInNoLaneCode],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpNewAddIrregConfInfo.szRunInNoLaneCode,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpNewAddIrregConfInfo.szRunInNoLaneCode)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s","驾驶机动车辆在非机动车道或者人行道的");
    pszVal =cfgSockGetValue(szSection,\
                            SysEpNewAddIrregInfoKey[e_epRunInNoLaneName],\
                            szDefault);
    strncpy(g_sysConfModule->s_EpNewAddIrregConfInfo.szRunInNoLaneName,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpNewAddIrregConfInfo.szRunInNoLaneName)\
            );
    
    return 0;
}

static int LoadEPLPRInfoConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;
    //char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_LPR_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 0);
    pszVal =cfgSockGetValue(szSection,\
                            epLPRConfInfoKey[e_evLPREn],\
                            szDefault);
    g_sysConfModule->s_EpLprConfInfo.ucLPREn = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 85);
    pszVal =cfgSockGetValue(szSection,\
                            epLPRConfInfoKey[e_evLPRCConfiden],\
                            szDefault);
    g_sysConfModule->s_EpLprConfInfo.ucLPRCConfidence = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault,"%s", "浙");
    pszVal =cfgSockGetValue(szSection,\
                            epLPRConfInfoKey[e_evLPRFirstCode],\
                            szDefault);

    strncpy(g_sysConfModule->s_EpLprConfInfo.szLPRFirstChar,\
            pszVal,\
            sizeof(g_sysConfModule->s_EpLprConfInfo.szLPRFirstChar)\
            );

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 1024);
    pszVal =cfgSockGetValue(szSection,\
                            epLPRConfInfoKey[e_evLPRWidth],\
                            szDefault);
    g_sysConfModule->s_EpLprConfInfo.usLPRWidth = atoi(pszVal);

    memset(szDefault,0x00,sizeof(szDefault));
    sprintf(szDefault, "%d", 768);
    pszVal =cfgSockGetValue(szSection,\
                            epLPRConfInfoKey[e_evLPRHeight],\
                            szDefault);
    g_sysConfModule->s_EpLprConfInfo.usLPRHeight = atoi(pszVal);
            
    return 0;
}

static  int  LoadPpJpgOsdConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PP_JPG_OSD_CONF_INFO]);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paBusNumOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucBusNumOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paCarColorOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucCarColorOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paCarDirOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucCarDirOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paCarTypeOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucCarTypeOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paCurSpeedOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucCurSpeedOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paDevSnOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucDevSnOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paDirNameOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucDirNameOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paIntelReguDesOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucIntelReguDesOsdEn= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paLaneNameOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucLaneNameOsdEn= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paLPColorOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucLPColorOsdEn= atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paMarkSpeedOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucMarkSpeedOsdEn = atoi(pszVal);


    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paPpNameOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucPpNameOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paPunishSpeedOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucPunishSpeedOsdEn= atoi(pszVal);
    
	memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paReguCodeOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucReguCodeOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paSnapTimeOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucSnapTimeOsdEn = atoi(pszVal);

	memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paSpeedRatioOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucSpeedRatioOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            paJpgOsdConfInfoKey[e_paStandReguDesOsdEn],\
                            szDefault);
    g_sysConfModule->s_PpJpgOsdConf.ucStandReguDesOsdEn = atoi(pszVal);    
    return 0;
}

static  int  LoadEPJpgOsdConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_JPG_OSD_CONF_INFO]);
    
    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epDevSnOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucDevSnOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epRoadNameOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucRoadNameOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epDirNameOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucDirNameOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epLaneNameOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucLaneNameOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epSnapTimeOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucSnapTimeOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epBusNumOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucBusNumOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epLPColorOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucLPColorOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epCarTypeOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucCarTypeOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epCarColorOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucCarColorOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epReguCodeOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucReguCodeOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epStandReguDesOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucStandReguDesOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epIntelReguDesOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucIntelReguDesOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epRDBeginTimeOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucRDBeginTimeOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epRDCycleOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucRDCycleOsdEn = atoi(pszVal);

    memset(szDefault, 0x00, sizeof(szDefault));
    sprintf(szDefault,"%d", 1);
    pszVal =cfgSockGetValue(szSection,\
                            epJpgOsdConfInfoKey[e_epRDDespOsdEn],\
                            szDefault);
    g_sysConfModule->s_EpJpgOsdConf.ucRoadDespOsdEn = atoi(pszVal);
    
    return 0;
}


static  int  LoadAppModeConf(void)
{
    char szSection[32];
    char szDefault[256];
    const char *pszVal = 0;

    
    if(NULL==g_sysConfModule)
        return (-1);
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_APPMODE_CONF_INFO]);
        
    memset(szDefault,0x00,sizeof(szDefault));
    pszVal =cfgSockGetValue(szSection,\
                            AppModeConfInfoKey[e_appmode],\
                            szDefault);
    g_sysConfModule->s_AppMode = atoi(pszVal);
    return 0;
}


static int LoadConf(void)
{
	int nRet =0;
  
	nRet =  LoadNetConf();
	nRet += LoadRtspConf();
	nRet += LoadVideoConf();
	nRet += LoadRecConf();
    nRet += LoadPassPortOverallConf();
    nRet += LoadPassPortVehicleConf();
    nRet += LoadElecPolliceOverallConf();
    nRet += LoadElecPolliceVehicleConf();
    nRet += LoadVLConf();
    nRet += LoadPassPortFtpConf();
    nRet += LoadElecPoliceFtpConf();
    nRet += LoadNtpConf();
    nRet += LoadCamCtrlConf();
    nRet += LoadPicNetHddConf();
    nRet += LoadRecNetHddConf();
    nRet += LoadDctlConf();
	nRet += LoadSysDefConf();
	nRet += LoadPlatformConf();
	nRet += LoadV3Conf();
	nRet += LoadAppModeConf();
    nRet += LoadNMConf();
    nRet += LoadCamTimeRangeConf();
    nRet += LoadFuncEnableConf();
    nRet += LoadEpTimeRangeConf();
    nRet += LoadEPVLoopStepInterverConf();
    nRet += LoadEp330Conf();
    nRet += LoadV2Conf();
    nRet += LoadEpAssistRedLightConf();
    nRet += LoadEPIrregInfoConf();
    nRet += LoadEPLPRInfoConf();
    nRet += LoadPpFuncEnConf();
    nRet += LoadEpSOTNoEntryConf();
    nRet += LoadEPJpgOsdConf();
    nRet += LoadPpJpgOsdConf();
    nRet += LoadPPIrregInfoConf();
	nRet += LoadPassPortBackupsFtpConf();
    nRet += LoadNRMConf();
    nRet += LoadEpBackupsFtpConf();
    nRet += LoadEPNewAddIrregInfoConf();
    nRet += LoadGb28181Conf();
	return nRet;
}

static int SaveNetConf(void)
{
    int ret =0;
	char szTemp[256];
	struct in_addr in;
	if(NULL==g_sysConfModule)
		return (-1);
    
	memset(szTemp,0x00,sizeof(szTemp));
	in.s_addr = g_sysConfModule->s_NetConfInfo.uiIPAddr;
	in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
	cfgSockSetValue("ipaddr", NULL,szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	in.s_addr = g_sysConfModule->s_NetConfInfo.uiNetMask;
	in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
	cfgSockSetValue("netmask",NULL,szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	in.s_addr = g_sysConfModule->s_NetConfInfo.uiGateWay;
	in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
	cfgSockSetValue("gatewayip",NULL,szTemp);
    ret = SaveWriteFile();
    if(0 != ret)
    {
        CONF_ERROR("SaveNetConf error!\n")
    }
	return ret;
}
#if 0
static int SaveRtspConf(void)
{

	char szTemp[256];
    char szSection[256];
    
	struct in_addr in;
	
	if(NULL==g_sysConfModule)
		return (-1);

	cfgSockSetValue(INI_FILE_EXIT_SEC,\
											INI_FILE_EXIT_KEY,\
											INI_FILE_EXIT_FLAG);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_RTSP_CONF_INFO]);
    
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.cSupportDownLoad);
	cfgSockSetValue(szSection,RtspConfInfoKey[e_SupportDownLoad],szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.cDownLoadType);
	cfgSockSetValue(szSection,RtspConfInfoKey[e_DownLoadType],szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	in.s_addr = g_sysConfModule->s_RtspConfInfo.uiProxyDownLoadIp;
	in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
	cfgSockSetValue(szSection,RtspConfInfoKey[e_DownLoadIp],szTemp);

	memset(szTemp,0x00,sizeof(szTemp));;
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.usProxyDownLoadPort);
	cfgSockSetValue(szSection,RtspConfInfoKey[e_DownLoadPort],szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.cSupportReplay);
	cfgSockSetValue(szSection,RtspConfInfoKey[e_SupportReplay],szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.cReplayType);
	cfgSockSetValue(szSection,RtspConfInfoKey[e_ReplayType],szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	in.s_addr = g_sysConfModule->s_RtspConfInfo.uiProxyReplayIp;
	in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
	cfgSockSetValue(szSection,RtspConfInfoKey[e_ReplayIP],szTemp);

	memset(szTemp,0x00,sizeof(szTemp));;
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.usProxyReplayPort);
	cfgSockSetValue(szSection,RtspConfInfoKey[e_ReplayPort],szTemp);
	
	SaveWriteFile();
    
	return 0;
}
#else


static int SaveRtspConf()
{
    int ret =0;
	char szTemp[256];
	char szSection[256];
	
	struct in_addr in;
	
	if(NULL==g_sysConfModule)
		return (-1);

	cfgSockSetValue(INI_FILE_EXIT_SEC,\
											INI_FILE_EXIT_KEY,\
											INI_FILE_EXIT_FLAG);
											
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_RTSP_CONF_INFO]);
    
	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.nRtspMode);
	cfgSockSetValue(szSection, "rtsp-mode", szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	in.s_addr = g_sysConfModule->s_RtspConfInfo.dwRtspIpAddr;
	sprintf(szTemp,"%s",inet_ntoa(in));
	cfgSockSetValue(szSection, "rtsp-ip", szTemp);

	memset(szTemp,0x00,sizeof(szTemp));;
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.dwRtspPort);
	cfgSockSetValue(szSection, "rtsp-port", szTemp);

	memset(szTemp,0x00,sizeof(szTemp));;
	sprintf(szTemp,"%d",g_sysConfModule->s_RtspConfInfo.dwRtspDownLoadPort);
	cfgSockSetValue(szSection, "rtsp-download-port", szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%s",g_sysConfModule->s_RtspConfInfo.szRtspPath);
	cfgSockSetValue(szSection, "rtsp-path", szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%s",g_sysConfModule->s_RtspConfInfo.szRtspUsername);
	cfgSockSetValue(szSection, "rtsp-user", szTemp);

	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%s",g_sysConfModule->s_RtspConfInfo.szRtspPassword);
	cfgSockSetValue(szSection, "rtsp-pass", szTemp);
	
	ret = SaveWriteFile();
	if(0 != ret)
    {
        CONF_ERROR("SaveRtspConf error!\n")
    }
	return ret;
}


#endif
static int SaveSysDefConf(void)
{
	
	char szTemp[256];
	
	if(NULL==g_sysConfModule)
		return (-1);

	cfgSockSetValue(INI_FILE_EXIT_SEC,\
											INI_FILE_EXIT_KEY,\
											INI_FILE_EXIT_FLAG);

	memset(szTemp,0x00,sizeof(szTemp));
	sprintf(szTemp,"%d",g_sysConfModule->s_SysConfInfo.iVehiSta);
	cfgSockSetValue("VEHILCE_WORK_STA", "vehWorkSta", szTemp);

    SaveWriteFile();
	return 0;
}




int SaveWriteFile(void)
{
	int nRet = 0;
	
	if(NULL==g_sysConfModule)
		return (-1);

	cfgSockSetValue(INI_FILE_EXIT_SEC,\
											INI_FILE_EXIT_KEY,\
											INI_FILE_EXIT_FLAG);

	sem_wait(&(g_sysConfModule->semSaveOP));

    nRet = cfgSockSaveFiles();
	if(nRet<0)
	{
		CONF_DEBUG("%s:%d SaveFile() failed.\n",__FUNCTION__,__LINE__);
		goto error_ret;
	}
	
	sem_post(&(g_sysConfModule->semSaveOP));

	return 0;

error_ret:

	sem_post(&(g_sysConfModule->semSaveOP));

	return (-1);
}



static	int	 SaveVideoConf(void)
{
    int ret =0;
    char szTemp[256];
    char szSection[32];

    if(NULL==g_sysConfModule)
        return (-1);
    
    for(int i=0;i<TOTAL_CHANNEL_NUM;i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_VIDEO_CONF_INFO],i);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usChannel);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vVideoChannelID],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp)); 
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].cVQuality);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vVQuality],\
                        szTemp);
        #if defined(CYC500XKW)||defined(CYC500XLKW)||defined(CYC500KX2)||defined(CYC500K)||defined(CYC500KD)||defined(CYC800JX)||defined(CYC800XKW4C)||defined(CYC500JX)

        if(g_sysConfModule->s_VideoConfInfo[i].cFrameRate > 9)
        {
            g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 9;
        }
        
        #elif defined(CYC500XKW4C)||defined(CYC500XAW)
        
        //g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 17;//帧率放开
        
		#elif defined(CYC800XKW4C)   
		
		if(g_sysConfModule->s_VideoConfInfo[i].cFrameRate > DEFAULT_ENC_FRAMERATE)
        {
            g_sysConfModule->s_VideoConfInfo[i].cFrameRate = DEFAULT_ENC_FRAMERATE;
        }

        #else
        
        if(g_sysConfModule->s_VideoConfInfo[i].cFrameRate > 17)
        {
            g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 17;
        }
        
        #endif
#if defined(CYT200SDI)
        g_sysConfModule->s_VideoConfInfo[i].cFrameRate = 25;
#endif

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].cFrameRate);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vFrameRate],\
                        szTemp);

        /* 为了保证红灯报警录像文件大小，所以码流规定最小为4M */
        if(g_sysConfModule->s_VideoConfInfo[i].uiBitRate > 5120)
        {
            g_sysConfModule->s_VideoConfInfo[i].uiBitRate = 5120;
        }
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].uiBitRate);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vBitRate],\
                        szTemp);

        /* 为了保证红灯报警录像能够在播放时有I帧，所以规定I帧间隔最大不超过25 */
        if(g_sysConfModule->s_VideoConfInfo[i].cKeyFInterval > 25)
        {
            g_sysConfModule->s_VideoConfInfo[i].cKeyFInterval = 25;
        }
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].cKeyFInterval);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vKeyFinterval],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].cImageResol);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vImageResol],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%s",VENC_TYPE_STR(g_sysConfModule->s_VideoConfInfo[i].cEncodeType));
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vEncType],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].cVBR);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vVBR],\
                        szTemp);
       
        memset(szTemp, 0x00, sizeof(szTemp));
        if( g_sysConfModule->s_VideoConfInfo[i].usXPos >= HDConf.image_width )
        {
            g_sysConfModule->s_VideoConfInfo[i].usXPos = HDConf.image_width -500;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usXPos);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vXPos],\
                        szTemp);
        
        memset(szTemp, 0x00, sizeof(szTemp));
        if( g_sysConfModule->s_VideoConfInfo[i].usYPos >= HDConf.image_height)
        {
            g_sysConfModule->s_VideoConfInfo[i].usYPos = HDConf.image_height -200;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usYPos);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vYPos],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%s",g_sysConfModule->s_VideoConfInfo[i].szContent);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vContent],\
                        szTemp);   
                        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usDebugFrameX);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vDebugFrameX],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usDebugFrameY);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vDebugFrameY],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].ucDebugFrameEn);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vDebugFrameEn],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].ucTimeDisMode);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vTimeDisMode],\
                        szTemp);
                        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usTimeXPos);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vTimeXPos],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VideoConfInfo[i].usTimeYPos);
        cfgSockSetValue(szSection,\
                        VideoConfInfoKey[e_vTimeYPos],\
                        szTemp);

     
    }
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveVideoConf error!\n");
    }
    return ret;
    
}


static int SaveRecConf(void)
{
    int ret =0;
	char szTemp[512];
	char szSection[32];

	if(NULL==g_sysConfModule)
		return (-1);

	for(int i=0;i<TOTAL_CHANNEL_NUM;i++)
	{
		memset(szSection,0x00,sizeof(szSection));
		sprintf(szSection,"%s%02d",sys_ini_section[e_REC_CONF_INFO],i+1);
		
		memset(szTemp,0x00,sizeof(szTemp));
		sprintf(szTemp,"%d",g_sysConfModule->s_RecConfInfo[i].nChannelID);
		cfgSockSetValue(szSection,\
						RecConfInfoKey[e_rRecChannelID],\
						szTemp);

		memset(szTemp,0x00,sizeof(szTemp));
		sprintf(szTemp,"%d",g_sysConfModule->s_RecConfInfo[i].nSysRecMode);
		cfgSockSetValue(szSection,\
						RecConfInfoKey[e_rSysRecMode],\
						szTemp);

		memset(szTemp,0x00,sizeof(szTemp));
		sprintf(szTemp,"%d",g_sysConfModule->s_RecConfInfo[i].nLoopRecFlag);
		cfgSockSetValue(szSection,\
						RecConfInfoKey[e_rLoopRecFlag],\
						szTemp);

		memset(szTemp,0x00,sizeof(szTemp));
		sprintf(szTemp,"%d",g_sysConfModule->s_RecConfInfo[i].nStorageRecDays);
		cfgSockSetValue(szSection,\
						RecConfInfoKey[e_rStorageRecDays],\
						szTemp);

		memset(szTemp,0x00,sizeof(szTemp));
		for(int j=0;j<DEFAULT_REC_DISK_PER_CHANNEL;j++)
		{
			char *pTmp = szTemp+strlen(szTemp);
			//if(strlen(g_sysConfModule->s_RecConfInfo[i].szRecPath[j]))
				sprintf(pTmp,"%s,",g_sysConfModule->s_RecConfInfo[i].szRecPath[j]);
		}
		cfgSockSetValue(szSection,\
						RecConfInfoKey[e_rRecPath],\
						szTemp);

		{
			unsigned long ulPlanStart = 0;
			struct tm tmPlanStart;
			unsigned long ulPlanEnd = 0;
			struct tm tmPlanEnd;
			char *pTmp = NULL;

			memset(szTemp,0x00,sizeof(szTemp));

			pTmp = szTemp+strlen(szTemp);

			sprintf(pTmp,"%01d,",g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.bIsValid);

			for(int j=0;j<DEFAULT_PLANT_TIME_SEGMENT;j++)
			{
				if((g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiStartTime==0)\
					&&(g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiEndTime==0))
				{
				}
				else
				{
					pTmp = szTemp+strlen(szTemp);
					ulPlanStart = g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiStartTime;
					ulPlanEnd = g_sysConfModule->s_RecConfInfo[i].ptdOncePlanRecTime.ts[j].uiEndTime;
					localtime_r((time_t *)&ulPlanStart,&tmPlanStart);
					localtime_r((time_t *)&ulPlanEnd,&tmPlanEnd);

					sprintf(pTmp,"%04d-%02d-%02d %02d:%02d:%02d<->%04d-%02d-%02d %02d:%02d:%02d,",\
					tmPlanStart.tm_year+1900,tmPlanStart.tm_mon+1,tmPlanStart.tm_mday,\
					tmPlanStart.tm_hour,tmPlanStart.tm_min,tmPlanStart.tm_sec,\
					tmPlanEnd.tm_year+1900,tmPlanEnd.tm_mon+1,tmPlanEnd.tm_mday,\
					tmPlanEnd.tm_hour,tmPlanEnd.tm_min,tmPlanEnd.tm_sec);
				}
			}

			cfgSockSetValue(szSection,\
							RecConfInfoKey[e_rOncePlanRecTime],\
							szTemp);
		}

		{
			unsigned long ulPlanStart = 0;
			unsigned long ulPlanEnd = 0;
			char *pTmp = NULL;
			
			int hour1,minute1,second1;
			int hour2,minute2,second2;

			memset(szTemp,0x00,sizeof(szTemp));

			pTmp = szTemp+strlen(szTemp);

			sprintf(pTmp,"%01d,",g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.bIsValid);

			for(int j=0;j<DEFAULT_PLANT_TIME_SEGMENT;j++)
			{
				if((g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiStartTime==0)\
					&&(g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiEndTime==0))
				{
				}
				else
				{
					pTmp = szTemp+strlen(szTemp);
					ulPlanStart = g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiStartTime;
					ulPlanEnd = g_sysConfModule->s_RecConfInfo[i].ptdDayPlanRecTime.ts[j].uiEndTime;

					hour1 = ulPlanStart/3600;
					minute1 = (ulPlanStart%3600)/60;
					second1 = ulPlanStart%60;
					hour2 = ulPlanEnd/3600;
					minute2 = (ulPlanEnd%3600)/60;
					second2 = ulPlanEnd%60;

					sprintf(pTmp,"%02d:%02d:%02d<->%02d:%02d:%02d,",\
						hour1,minute1,second1,hour2,minute2,second2);
				}
			}


			cfgSockSetValue(szSection,\
								RecConfInfoKey[e_rDayPlanRecTime],\
								szTemp);
		}

		for(int j=0;j<7;j++)
		{
			unsigned long ulPlanStart = 0;
			unsigned long ulPlanEnd = 0;
			char *pTmp = NULL;

			int hour1,minute1,second1;
			int hour2,minute2,second2;

			memset(szTemp,0x00,sizeof(szTemp));

			pTmp = szTemp+strlen(szTemp);

			sprintf(pTmp,"%01d,",g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].bIsValid);

			for(int k=0;k<DEFAULT_PLANT_TIME_SEGMENT;k++)
			{
				if((g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiStartTime==0)\
					&&(g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiEndTime==0))
				{
				}
				else
				{
					pTmp = szTemp+strlen(szTemp);

					ulPlanStart = g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiStartTime;
					ulPlanEnd = g_sysConfModule->s_RecConfInfo[i].ptdWeekPlanRecTime[j].ts[k].uiEndTime;

					hour1 = ulPlanStart/3600;
					minute1 = (ulPlanStart%3600)/60;
					second1 = ulPlanStart%60;
					hour2 = ulPlanEnd/3600;
					minute2 = (ulPlanEnd%3600)/60;
					second2 = ulPlanEnd%60;

					sprintf(pTmp,"%02d:%02d:%02d<->%02d:%02d:%02d,",\
						hour1,minute1,second1,hour2,minute2,second2);
				}
			}

			cfgSockSetValue(szSection,\
								RecConfInfoKey[e_rMondayPlanRecTime+j],\
								szTemp);
		}

		memset(szTemp,0x00,sizeof(szTemp));
		sprintf(szTemp,"%s",REC_FORMAT_STR(g_sysConfModule->s_RecConfInfo[i].nRecFileFormat));
		cfgSockSetValue(szSection,\
											RecConfInfoKey[e_rRecFileFormat],\
											szTemp);
		
	}

    ret = SaveWriteFile();
    if(0 != ret)
    {
        CONF_ERROR("SaveRecConf error!\n")
    }
	return ret;
}

static  int  SavePassPortOverallConf(void)
{
    int ret =0;
    char szTemp[256];
    char szSection[256];
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PASSPORT_OVERALL_CONF]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szPassPortName);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paPassPortName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szDirection);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paDirection],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szPassPortId);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paPassPortId],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szDirId);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paDirId],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.cOSDColor);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paOSDClolr],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.cValidLanes);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paValidLanes],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.cVLWorkMode);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paVLWorkMode],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucLPREn);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paLPREn],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szLPRFirstChar);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paLPRString],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucLPRCConfidence);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paLPRCConfiden],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucVSpeedMeaFlag);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paVSMeaFlag],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.usVSpeedMeaLen);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paVSMeaLen],\
                    szTemp);

    for (int j =0;j < 4; j++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.aucFreqFlashEn[j]);
        sprintf(szInfoKey,"%s%02d",
                PassPortOverallConfInfoKey[e_paFreqFlashEn],
                j
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    for (int j =0;j < 4; j++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.aucFlashTrigMode[j]);
        sprintf(szInfoKey,"%s%02d",
                PassPortOverallConfInfoKey[e_paFlashTrigMode],
                j
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    for (int j =0;j < 4; j++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.aucStrobeFlashFre[j]);
        sprintf(szInfoKey,"%s%02d",
                PassPortOverallConfInfoKey[e_paStrobeFlashFreq],
                j
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szPpDevCode);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paPpDevCode],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucFillInLSwitchEn);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paFillInLSwitchEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucGrayThresholdVal);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paGrayThresold],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucCheckSwitchCycle);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paCheckGrayCycle],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PassPortOverallConfInfo.szExPpDevCode);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paExPpDevCode],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucSmartSwitchEn);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paSmartSwitchEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.ucTimerSwitchEn);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paTimerSwitchEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.uiFillInLightBeginTime);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paFILightBeginTime],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PassPortOverallConfInfo.uiFillInLightLastTime);
    cfgSockSetValue(szSection,\
                    PassPortOverallConfInfoKey[e_paFILightLastTime],\
                    szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SavePassPortOverallConf error!\n");
    }
    return ret;
}
static  int  SavePassPortVehicleConf(void)
{
    int ret =0;
    char szSection[256];
    char szTemp[256];
    char szInfoKey[256];
    

    if(NULL==g_sysConfModule)
        return (-1);
    
    for (int i =0 ; i< MAX_VEHICLE_NUM; i++ )
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_PASSPORT_VEHICLE_CONF],i);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].uiCarRoadId);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVehicleId],\
                        szTemp);      
        

        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosX >= HDConf.image_width )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosX = HDConf.image_width-500;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosX);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvCaptionX],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosY >= HDConf.image_height)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosY = HDConf.image_height-200;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usOSDPosY);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvCaptionY],\
                        szTemp);  
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%s",g_sysConfModule->s_PortVehicleConfInfo[i].szOSDContent);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVehicleName],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosX > HDConf.image_width )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosX = HDConf.image_width;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosX);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvAreaStartX],\
                        szTemp);  

        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosY > HDConf.image_height)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosY = HDConf.image_height;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaPosY);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvAreaStartY],\
                        szTemp);  
       
        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaWidth > HDConf.image_width)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaWidth = HDConf.image_width;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaWidth);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvAreaWidth],\
                        szTemp);  

        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaHeight > HDConf.image_height)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaHeight = HDConf.image_height;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strRoadArea.usAreaHeight);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvAreaHeight],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usFlashLightType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvLightType],\
                        szTemp); 
       
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usFlashLightCtlType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvFlashControlType],\
                        szTemp); 
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cComId);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvComId],\
                        szTemp); 
        
     
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cFlashDevType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvFlashDevType],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cFlashCtlMode);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvFlashCtlMode],\
                        szTemp); 

        for (int j =0 ;j < MAX_FLASH_ADDR_SUM; j++)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].ausFlashAddr[j]);
            sprintf(szInfoKey,"%s%02d",
                    PassPortVehicleConfInfoKey[e_pvFlashAddr],
                    j
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
        }

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usDetectType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvDetectType],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaPosX);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLStartX],\
                        szTemp); 
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaPosY);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLStartY],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaWidth);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLWidth],\
                        szTemp); 
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strVLArea.usAreaHeight);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLHeight],\
                        szTemp); 
       

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVehicleDetectType],\
                        szTemp); 
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVehicleDetectType],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleLinkMode);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVehicleDetectConnectMode],\
                        szTemp); 
        
       if ( CONNECT_TYPE_COM  == g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleLinkMode )
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cComId
                    );/**CoilDesp.ComC.cComId**/
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "ComId"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBoardId
                    );/**CoilDesp.ComC.cComId**/
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "BoardId"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
            
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cFrontCoilId
                    );/**CoilDesp.ComC.cComId**/
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "FrontCoilId"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
            
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBehindCoilId
                    );/**CoilDesp.ComC.cComId**/
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "BehindCoilId"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
            
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            /* begin add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            if( 0 == g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId )
            {
                g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId =1;
            }
            /* end add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId
                    );/**CoilDesp.ComC.cComId**/
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilCom",
                    "GroupId"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
        }
        else if ( CONNECT_TYPE_GPIO == g_sysConfModule->s_PortVehicleConfInfo[i].usVehicleLinkMode)
        {
            
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBoardId
                    );
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilGpio",
                    "BoardId"
                    );
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cFrontCoilId
                    );
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilGpio",
                    "FrontGpioId"
                    );

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_PortVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBehindCoilId
                    );/**CoilDesp.ComC.cComId**/
            sprintf(szInfoKey,"%s.%s.%s",
                    PassPortVehicleConfInfoKey[e_pvCoilDesp],
                    "CoilGpio",
                    "BehindGpioId"
                    );
            
            cfgSockSetValue(szSection,szInfoKey,szTemp);
        }

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usMaxSpeed);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvMaxSpeed],\
                        szTemp); 
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usIrreSpeed \
        < g_sysConfModule->s_PortVehicleConfInfo[i].usMaxSpeed)
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].usIrreSpeed \
            = g_sysConfModule->s_PortVehicleConfInfo[i].usMaxSpeed;
        };

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usIrreSpeed);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvIrreSpeed],\
                        szTemp); 
                        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].usMinSpeed);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvMinSpeed],\
                        szTemp); 
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].uiCoilDist);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvCoilDist],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cVLDispFlag);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLDispFlag],\
                        szTemp); 
                        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cLaneDispFlag);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvLaneDispFlag],\
                        szTemp);
                        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%s",g_sysConfModule->s_PortVehicleConfInfo[i].szLaneId);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvLaneId],\
                        szTemp);
                        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cSingalCoilPos);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSingalCoilPos],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cGroudCoilDecType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvGroudCoilDecType],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cSingalCoilSnapType);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSingalCoilSnapType],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cCoilSwitchFlag);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvCoilSwitchFlag],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].cInLightSta);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvInLightSta],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].cCoilSwitchSta);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvCoilSwitchSta],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].cVLSnapDir);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLSnapDir],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].cVLDecEn);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvVLDecEn],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].usSingalCoilHigh);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSingalCoilHigh],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].ucDaySnapObj);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvDaySnapObj],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].ucNightSnapObj);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvNightSnapObj],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum > 2 )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum =2;
        }
        if( g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum <= 0 )
        {
            g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum =1;
        }
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].ucSnapPicNum);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSnapPicNum],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        if( g_sysConfModule->s_PortVehicleConfInfo[i].usSnapPicInter < 40 )
        {
        	g_sysConfModule->s_PortVehicleConfInfo[i].usSnapPicInter =40;
        }
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].usSnapPicInter);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSnapPicInte],\
                        szTemp);

        memset(szTemp, 0x00, sizeof(szTemp));
        sprintf(szTemp,"%d", g_sysConfModule->s_PortVehicleConfInfo[i].ucInLightEn);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvInLightEn],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.uiBeginTimeSecs);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvBeginTimeSec],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.uiLastTimeSecs);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvLastTimeSec],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].strTimeSlotSnapObj.ucSnapObj);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvTimeSlotObj],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cSnapDir);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSnapDir],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cSDEnFlag);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDEnFlag],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].iSDBeginTime1);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDBeginTime1],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].iSDLastTime1);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDLastTime1],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].iSDBeginTime2);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDBeginTime2],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].iSDLastTime2);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDLastTime2],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].iSDBeginTime3);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDBeginTime3],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].iSDLastTime3);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvSDLastTime3],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_PortVehicleConfInfo[i].cTruckEnFlag);
        cfgSockSetValue(szSection,\
                        PassPortVehicleConfInfoKey[e_pvTruckEnFlag],\
                        szTemp);                        

    }
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SavePassPortVehicleConf error!\n");
    }
    return ret;
}
static  int  SaveElecPolliceOverallConf(void)
{
    int ret =0;
    char szSection[256];
    char szTemp[256];
    char szInfoKey[256];
    

  
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_ELEC_POLICE_OVERALL_CONF]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szCarRoadName);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaElecPoliceName],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szDirection);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaDirection],\
                    szTemp); 

    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.usPicStyle);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaPicStyle],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.usFlashLightType);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaLightType],\
                    szTemp); 
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.usFlashLightCtlType);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaFlashControlType],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cComId);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaComId],\
                    szTemp); 
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cFlashDevType);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaFlashDevType],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cFlashCtlMode);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaFlashCtlMode],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.usXcsProVer);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaXcsProVer],\
                    szTemp); 
    
    for (int i =0 ;i < MAX_FLASH_ADDR_SUM; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.ausFlashAddr[i]);
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaFlashAddr],
                i
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    for (int i =0 ;i < MAX_AREA_GROUP_NUM; i++)
    {

        for ( int j =0 ;j<MAX_AREA_NUM;j++)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaPosX);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "StartX"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaPosY);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "StartY"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaWidth);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "Width"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedArea[i][j].strArea.usAreaHeight);
            sprintf(szInfoKey,"%sGroup%02d.Rect%02d.%s",
                    ElecPolicOverallConfInfoKey[e_eaRedArea],
                    i,
                    j,
                    "Height"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 
         }

        
      
    }
    
     /* 保存红灯下限阈值*/
    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLow.ucRed);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedLow],
            KEY_RED
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLow.ucGreen);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedLow],
            KEY_GREEN
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLow.ucBlue);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedLow],
            KEY_BLUE
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    /* 保存红灯上限阈值*/
    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedHigh.ucRed);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedHigh],
            KEY_RED
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedHigh.ucGreen);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedHigh],
            KEY_GREEN
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedHigh.ucBlue);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaRedHigh],
            KEY_BLUE
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

       /* 保存绿灯下限阈值*/
    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenLow.ucRed);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenLow],
            KEY_RED
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenLow.ucGreen);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenLow],
            KEY_GREEN
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenLow.ucBlue);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenLow],
            KEY_BLUE
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    /* 保存绿灯上限阈值*/
    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenHigh.ucRed);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenHigh],
            KEY_RED
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenHigh.ucGreen);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenHigh],
            KEY_GREEN
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strGreenHigh.ucBlue);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaGreenHigh],
            KEY_BLUE
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

       /* 保存黄灯下限阈值*/
    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowLow.ucRed);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowLow],
            KEY_RED
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowLow.ucGreen);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowLow],
            KEY_GREEN
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowLow.ucBlue);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowLow],
            KEY_BLUE
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    /* 保存黄灯上限阈值*/
    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowHigh.ucRed);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowigh],
            KEY_RED
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowHigh.ucGreen);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowigh],
            KEY_GREEN
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    memset(szInfoKey,0x00,sizeof(szInfoKey));
    
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strYellowHigh.ucBlue);
    sprintf(szInfoKey,"%s.%s",
            ElecPolicOverallConfInfoKey[e_eaYellowigh],
            KEY_BLUE
            );
    cfgSockSetValue(szSection,szInfoKey,szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szRemarks);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_roadRemarks],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cDetectType);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaVLType],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cCarDistance);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaCarDistance],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cFrameInterval);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaSnapInterval],\
                    szTemp); 
                    
    memset(szTemp,0x00,sizeof(szTemp));
    if(g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum <= 0)
    {
       g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum = 1;
    }
    
    if(g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum > MAX_VEHICLE_NUM)
    {
       g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum = g_uiCamLaneSum;
    }
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cValidChanSum);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaValidChanSum],\
                    szTemp); 
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cLightAmendDisFlag);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaVLDisFlag],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szRoadId);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaRoadId],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szDirId);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaDirId],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cLightAmendEn);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaRLAmendEn],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cRLDectType);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaRLDectType],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cOSDColour);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaOSDColour],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cRLAmendLevel);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaRLAmendLevel],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cPPSendEn);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaPPSendEn],\
                    szTemp); 

    for (int i =0 ;i < 4 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFreqFlashEn[i]);
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaFreqFlashEn], i);
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    for (int i =0; i < MAX_AREA_GROUP_NUM; i++)
    {

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucLightStrobeEn);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "lightStrobeEn"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucLightStrobeFrame);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "lightStrobeFrames"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 


        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucSingalColLight);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "singalColLight"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.strRedLightMode[i].ucMapRDNo);
        sprintf(szInfoKey,"%sGroup%02d.%s",
                ElecPolicOverallConfInfoKey[e_eaRedLightModify],
                i,
                "mapRDNo"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        
    }

    for (int i =0 ;i < 4 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.aucFlashTrigMode[i]);
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaFlashTrigMode], i);
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cPPFlashEn);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaPPFlashEn],\
                    szTemp); 

    for (int i =0 ;i < 4 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.aucStrobeFlashFre[i]);
        sprintf(szInfoKey,"%s%02d",
                ElecPolicOverallConfInfoKey[e_eaStrobeFlashFreq], i);
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szEpDevCode);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaEpDevCode],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cACSyncSwitchEn);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaACSyncSwitchEn],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.ucACSwitchThreshold);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaACSwitchThreshold],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.ucACDecCycle);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaACDecCycle],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.usDayACPhase);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaDayACPhase],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.usNightACPhase);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaNightACPhase],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.cRedModifyMode);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaRedModifyMode],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceOverallConfInfo.szExEpDevCode);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaExEpDevCode],\
                    szTemp); 

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.ucSmartSwitchEn);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaSmartSwitchEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.ucTimerSwitchEn);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaTimerSwitchEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.uiFillInLightBeginTime);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaFILightBeginTime],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceOverallConfInfo.uiFillInLightLastTime);
    cfgSockSetValue(szSection,\
                    ElecPolicOverallConfInfoKey[e_eaFILightLastTime],\
                    szTemp);


    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveElecPolliceOverallConf error!\n");
    }
    return ret;
}

static  int  SaveElecPolliceVehicleConf(void)
{
    int ret =0;
    char szSection[256];
    char szTemp[256];
    char szInfoKey[256];
    
    int iWidth =0;
    int iHeight =0;
    int iPixels =0;
    getCameraInfo(&iWidth, &iHeight, &iPixels);
    if(NULL==g_sysConfModule)
        return (-1);
    
    for (int i =0 ; i< MAX_VEHICLE_NUM; i++ )
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_ELEC_POLICE_VEHICLE_CONF_INFO],i);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].uiCarRoadId);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evVehicleId],\
                        szTemp); 
        
        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosX >= HDConf.image_width )
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosX = HDConf.image_width -500;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosX);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCaptionX],\
                        szTemp); 

        
        memset(szTemp,0x00,sizeof(szTemp));
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosY >= HDConf.image_height)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosY = HDConf.image_height -200;
        }
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usOSDPosY);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCaptionY],\
                        szTemp); 
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].szOSDContent);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evVehicleName],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        
		#if defined(CYC500XKW4C)||defined(CYC500XAW)
        if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth != GREEN_LIGHT_WIDTH )
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth = GREEN_LIGHT_WIDTH;
        }
        #endif

        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth > HDConf.image_width )
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth = HDConf.image_width;
        }
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaWidth);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evAreadWidth],\
                        szTemp); 


        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight);
        
		#if defined(CYC500XKW4C)||defined(CYC500XAW)
        if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight != GREEN_LIGHT_HEIGHT)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight = GREEN_LIGHT_HEIGHT;
        }
        #endif

        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight > HDConf.image_height)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaHeight = HDConf.image_height;
        }
        
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evAreadHeight],\
                        szTemp); 
                        
        memset(szTemp,0x00,sizeof(szTemp));
        
		#if defined(CYC500XKW4C)||defined(CYC500XAW)
        if( (g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX + GREEN_LIGHT_WIDTH )\
        > iWidth)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX -= \
            ( (g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX + GREEN_LIGHT_WIDTH ) - iWidth );
        }
        #endif

        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX > HDConf.image_width)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX = HDConf.image_width;
        }
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosX);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evAreaStartX],\
                        szTemp); 
        
        memset(szTemp,0x00,sizeof(szTemp));

		#if defined(CYC500XKW4C)||defined(CYC500XAW)
        if( (g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY + GREEN_LIGHT_HEIGHT)\
        > iHeight)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY -= \
            ( (g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY + GREEN_LIGHT_HEIGHT) - iHeight );
        }
        #endif
        if( g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY > HDConf.image_height)
        {
            g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY = HDConf.image_height;
        }
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRoadArea.usAreaPosY);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evAreaStartY],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLDetectType);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evRedDetectType],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLConnectMode);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evRedDetectConnectMode],\
                        szTemp); 
        
        if ( CONNECT_TYPE_COM  == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLConnectMode )
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evRedDetectDesp],
                    "RedCom",
                    "ComId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cComId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evRedDetectDesp],
                    "RedCom",
                    "CarRoadId"
                    );
            /* begin add by wangb 2010-4-28 for 红灯车道号固定从1开始 */
            if( 0 == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cCarRoadId )
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cCarRoadId =1;
            }
            /* end add by wangb 2010-4-28 for 红灯车道号固定从1开始 */
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPComRedLightDesp.cCarRoadId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
            
        }
        else if ( CONNECT_TYPE_GPIO  == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usRedLConnectMode)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evRedDetectDesp],
                    "RedGpio",
                    "GpioId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strRedLDesp.strEPGpioRedLightDesp.cGpioId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
        }
        

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleType);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCoilType],\
                        szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleLinkMode);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evVehicleDetectConnectMode],\
                        szTemp); 

        if ( CONNECT_TYPE_COM  == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleLinkMode )
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "ComId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cComId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "BoardId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBoardId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
            

            
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "FrontId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cFrontCoilId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "BehindId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cBehindCoilId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilCom",
                    "GroupId"
                    );
            /* begin add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            if( 0 == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId )
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId =1;
            }
            /* end add by wangb 2010-04-28 for 避免配置出现0车道导致无法手动抓拍问题 */
            
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strComCoilDesp.cGroupId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);
            
           
        }
        else if ( CONNECT_TYPE_GPIO  == g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usVehicleLinkMode)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilGpio",
                    "BoardId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBoardId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilGpio",
                    "FrontGpioId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cFrontCoilId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            sprintf(szInfoKey,"%s.%s.%s",
                    ElecPolicVehicleConfInfoKey[e_evCoilDesp],
                    "CoilGpio",
                    "BehindGpioId"
                    );
            sprintf(szTemp,"%d",
                    g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strCoilDesp.strGpioCoilDesp.cBehindCoilId
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            
            
        }
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].uiForcedRedLight);
        cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evForceRedLight],\
                        szTemp); 

        for ( int j =0 ;j<MAX_LIGHT_TYPE;j++)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cEnable);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evLightType],
                    j,
                    "Enable"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cLightType);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evLightType],
                    j,
                    "Type"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLight[j].cVehicleNum);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evLightType],
                    j,
                    "VehicleNum"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 
         }

                            
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cLaneDir);
        cfgSockSetValue(szSection,\
                            ElecPolicVehicleConfInfoKey[e_evLaneDir],\
                            szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaPosX);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "StartX"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaPosY);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "StartY"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaWidth);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "Width"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strTrafficLArea.usAreaHeight);
        sprintf(szInfoKey,"%s.%s",
                ElecPolicVehicleConfInfoKey[e_evLaneLightArea],
                "Height"
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);


        for ( int j =0 ;j<MAX_AREA_NUM;j++)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1 > HDConf.image_width)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1 = HDConf.image_width; 
            }
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLX1);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartX"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));

            if(g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1 > HDConf.image_height)
            {
                g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1 = HDConf.image_height; 
            }
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strVLPoint[j].usVLY1);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartY"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 
         }
         
         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cVLDispFlag);
         cfgSockSetValue(szSection,\
                            ElecPolicVehicleConfInfoKey[e_evVLDisFlag],\
                            szTemp); 

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cLaneLoopDispFlag);
         cfgSockSetValue(szSection,\
                            ElecPolicVehicleConfInfoKey[e_evLaneDisFlag],\
                            szTemp); 

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].szLaneId);
         cfgSockSetValue(szSection,\
                            ElecPolicVehicleConfInfoKey[e_evLaneId],\
                            szTemp); 

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarDistance);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCarDistance],\
                        szTemp); 

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cFrameInterval);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evSnapInterval],\
                        szTemp); 

         /*Start of ITS on 2011-4-2 14:29 2.0.0.1*/
         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchFlag);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCoilSwitchFlag],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCoilSwitchSta);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCoilSwitchSta],\
                        szTemp);
         
         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cDetectType);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCoilDecType],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cRedLightDectSta);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evRedLightDecSta],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usFlashLightType);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evFlashType],\
                        szTemp);
         /*End of ITS on 2011-4-2 14:29 2.0.0.1*/

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cInLightSta);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evInLightSta],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cVLDecEn);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evVLDecEn],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarNoDir);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evNoDir],\
                        szTemp);

        for ( int j =4; j<6; j++)
        {
            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strNoLaneVLPoint[j-4].usVLX1);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartX"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].strNoLaneVLPoint[j-4].usVLY1);
            sprintf(szInfoKey,"%s.Rect%02d.%s",
                    ElecPolicVehicleConfInfoKey[e_evVLPoint],
                    j,
                    "StartY"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 
         }

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarTrackEn);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCarTrackEn],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cCarSnapDir);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCarSnapDir],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucMinPlatePixelSize);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evMinPlatePix],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucMaxPlatePixelSize);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evMaxPlatePix],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucCorrEveningThresh);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evEveningThresh],\
                        szTemp);               

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].usCoilDist);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evCoilDistance],\
                        szTemp);               

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].ucPPDelaySnapFrames);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evPPDelaySnapFrames],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cSwitchLaneEn);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evSwitchLaneEn],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cSwitchLaneLeftNo);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evSwitchLaneLeftNo],\
                        szTemp);

         memset(szTemp,0x00,sizeof(szTemp));
         sprintf(szTemp,"%d", g_sysConfModule->s_ElecPoliceVehicleConfInfo[i].cSwitchLaneStraightNo);
         cfgSockSetValue(szSection,\
                        ElecPolicVehicleConfInfoKey[e_evSwitchLaneRightNo],\
                        szTemp);
                        
    }
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveElecPolliceVehicleConf error!\n");
    }
    return ret;
}

static int SaveVLConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
   
    if(NULL==g_sysConfModule)
        return (-1);
    
    for (int i =0 ; i< MAX_VEHICLE_NUM; i++ )
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_VL_CONF_INFO],i);
            
       
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiCarRoadId);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlVehicleId],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].usThrDayToNight);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlThrDayToNight],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].usThrNightToDay);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlThrNightToDay],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiCorrNightThresh);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlCorrNightThresh],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiCorrDayThresh);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlCorrDayThresh],\
                        szTemp);

        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiCorrMotorThresh);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlCorrMotorThresh],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiCorrEveningThresh);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlCorrEvenThresh],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiDayMotorWidth);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlDayMotorWidth],\
                        szTemp);
        

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].uiNightMotorWidth);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlNightMotorWidth],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].ucCapDelayInterval);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlCapDelayInterval],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].cCapDelayNum);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlCapDelayNum],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].cMinPlatePixelSize);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlMinPlatePixelSize],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_VlConfInfo[i].cMaxPlatePixelSize);
        cfgSockSetValue(szSection,\
                        VLConfInfoKey[e_vlMaxPlatePixelSize],\
                        szTemp);

    }
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveVLConf error!\n");
    }
    return ret;
}
static  int  SavePassPortFtpConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PASSPORT_FTP_CONF_INFO]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortFtpConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fEnable],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_PortFtpConfInfo.uiFtpIP;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpIp],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortFtpConfInfo.usFtpPort);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpPort],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PortFtpConfInfo.szUserName);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserName],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PortFtpConfInfo.szUserPasswd);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserPasswd],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortFtpConfInfo.cTRecEnable);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_nEpTimerEnable],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortFtpConfInfo.usTRecInterval);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_nEpTimerRecInt],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortFtpConfInfo.ucTRecLast);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_nEpTimerRecLast],\
                    szTemp);
    
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SavePassPortFtpConf error!\n");
    }
    return ret;
}

static  int  SavePassPortBackupsFtpConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PASSPORT_BACKUPS_FTP_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortBackupsFtpConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fEnable],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_PortBackupsFtpConfInfo.uiFtpIP;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpIp],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PortBackupsFtpConfInfo.usFtpPort);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpPort],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PortBackupsFtpConfInfo.szUserName);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserName],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PortBackupsFtpConfInfo.szUserPasswd);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserPasswd],\
                    szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SavePassPortFtpConf error!\n");
    }
    return ret;
}

static  int  SaveElecPoliceFtpConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_ELEC_POLICE_FTP_CONF_INFO]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceFtpConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fEnable],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_ElecPoliceFtpConfInfo.uiFtpIP;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpIp],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_ElecPoliceFtpConfInfo.usFtpPort);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpPort],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceFtpConfInfo.szUserName);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserName],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_ElecPoliceFtpConfInfo.szUserPasswd);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserPasswd],\
                    szTemp);
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveElecPoliceFtpConf error!\n");
    }
    return ret;
}

static  int  SaveEpBackupsFtpConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_BACKUPS_FTP_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_epBackupsFtpConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fEnable],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_epBackupsFtpConfInfo.uiFtpIP;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpIp],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_epBackupsFtpConfInfo.usFtpPort);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fFtpPort],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_epBackupsFtpConfInfo.szUserName);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserName],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_epBackupsFtpConfInfo.szUserPasswd);
    cfgSockSetValue(szSection,\
                    FtpConfInfoKey[e_fUserPasswd],\
                    szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveEpFtpConf error!\n");
    }
    return ret;
}

static  int  SaveNtpConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_NTP_CONF_INFO]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_NtpConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    NtpConfInfoKey[e_ntEnable],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_NtpConfInfo.uiServerIP;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    NtpConfInfoKey[e_ntNtpServer],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_NtpConfInfo.usJudgeCycle);
    cfgSockSetValue(szSection,\
                    NtpConfInfoKey[e_ntpJudgeCycle],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_NtpConfInfo.ucJudegPrecision);
    cfgSockSetValue(szSection,\
                    NtpConfInfoKey[e_ntpJudegPrecision],\
                    szTemp);

                    
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveNtpConf error!\n");
    }
    return ret;
}


static  int  SaveCamTimeRangeConf(void)
{
    int i =0;
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    for(i =0; i < MAX_CAM_GROUP_NUM; i++)
    {
    
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_CAM_TIME_RANGE_INFO], i);
    
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_CamTimeRange[i].cGroupId);
        cfgSockSetValue(szSection,\
                        CamTimeRangeKey[e_camTGroup],\
                        szTemp);
        
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_CamTimeRange[i].cEnable);
        cfgSockSetValue(szSection,\
                        CamTimeRangeKey[e_camTEnable],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%s",g_sysConfModule->s_CamTimeRange[i].szSceneName);
        cfgSockSetValue(szSection,\
                        CamTimeRangeKey[e_camTNmae],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_CamTimeRange[i].iBeginTime);
        cfgSockSetValue(szSection,\
                        CamTimeRangeKey[e_camTBeginTime],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_CamTimeRange[i].iLastTime);
        cfgSockSetValue(szSection,\
                        CamTimeRangeKey[e_camTEndTime],\
                        szTemp);

		memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_CamTimeRange[i].cIrisPos);
        cfgSockSetValue(szSection,\
                        CamTimeRangeKey[e_irisPos],\
                        szTemp);
    }
    
    SaveWriteFile();
    return 0;
}

/* 电警辅助红灯配置参数保存 */
static  int  SaveEPAssistRedLightConf(void)
{
    int i =0;
    int j =0;
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    for(i =0; i < MAX_VEHICLE_NUM; i++)
    {
        for(j =0; j <EP_ASSIST_RED_LIGHT_NUM; j++ )
        {
            memset(szSection,0x00,sizeof(szSection));
            sprintf(szSection,"%s%02d",sys_ini_section[e_EP_ASSIST_REDLIGHT_INFO], i);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].cRedLightNum);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cEpAssistRedLightNum]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].cEnableFlag);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cEpARLEnable]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].iBeginTime1);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLBeginTime1]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].iLastTime1);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLEndTime1]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].iBeginTime2);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLBeginTime2]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].iLastTime2);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLEndTime2]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].iBeginTime3);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLBeginTime3]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpAssistRedLight[i][j].iLastTime3);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpAssistRedLightKey[e_cEpAssistRedLight],
                    j,
                    EpAssistRedLightKey[e_cARLEndTime3]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

        }
    }
    
    SaveWriteFile();
    return 0;
}

static int   SavePpJpgOsdConf(void)
{
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PP_JPG_OSD_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucBusNumOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paBusNumOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucCarColorOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paCarColorOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucCarDirOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paCarDirOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucCarTypeOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paCarTypeOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucCurSpeedOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paCurSpeedOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucDevSnOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paDevSnOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucDirNameOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paDirNameOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucIntelReguDesOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paIntelReguDesOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucLaneNameOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paLaneNameOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucLPColorOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paLPColorOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucMarkSpeedOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paMarkSpeedOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucPpNameOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paPpNameOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucPunishSpeedOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paPunishSpeedOsdEn],\
                    szTemp);                    

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucReguCodeOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paReguCodeOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucSnapTimeOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paSnapTimeOsdEn],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucSpeedRatioOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paSpeedRatioOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpJpgOsdConf.ucStandReguDesOsdEn);
    cfgSockSetValue(szSection,\
                    paJpgOsdConfInfoKey[e_paStandReguDesOsdEn],\
                    szTemp);

    SaveWriteFile();
    return 0;
    

}


static int   SaveEpJpgOsdConf(void)
{
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_JPG_OSD_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucDevSnOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epDevSnOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucRoadNameOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epRoadNameOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucDirNameOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epDirNameOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucLaneNameOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epLaneNameOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucSnapTimeOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epSnapTimeOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucBusNumOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epBusNumOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucLPColorOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epLPColorOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucCarTypeOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epCarTypeOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucCarColorOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epCarColorOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucReguCodeOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epReguCodeOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucStandReguDesOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epStandReguDesOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucIntelReguDesOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epIntelReguDesOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucRDBeginTimeOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epRDBeginTimeOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucRDCycleOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epRDCycleOsdEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpJpgOsdConf.ucRoadDespOsdEn);
    cfgSockSetValue(szSection,\
                    epJpgOsdConfInfoKey[e_epRDDespOsdEn],\
                    szTemp);
                    
    SaveWriteFile();
    return 0;
    

}

/* 电警遇堵禁入配置参数保存 */
static  int  SaveEPSOTNoEntryConf(void)
{
    int i =0;
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    for(i =0; i < MAX_VEHICLE_NUM; i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_EP_SOT_NO_ENTRY_INFO], i);

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].cEnableFlag);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cEpSOTNEEnable]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].iBeginTime1);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEBeginTime1]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].iLastTime1);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEEndTime1]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].iBeginTime2);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEBeginTime2]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].iLastTime2);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEEndTime2]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].iBeginTime3);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEBeginTime3]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        
        sprintf(szTemp,"%d", g_sysConfModule->s_EpSOTNoEntry[i].iLastTime3);
        sprintf(szInfoKey,"%s",
                EpSOTNoEntryKey[e_cSOTNEEndTime3]
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp); 

    }
    
    SaveWriteFile();
    return 0;
}

static  int  SaveEPTimeRangeConf(void)
{
    int i =0;
    int j =0;
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    for(i =0; i < MAX_VEHICLE_NUM; i++)
    {
        for(j =0; j <EP_FORBID_MAX_DIR; j++ )
        {
            memset(szSection,0x00,sizeof(szSection));
            sprintf(szSection,"%s%02d",sys_ini_section[e_EP_TIME_RANGE_INFO], i);

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].cForbidType);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    "Type"
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].cRange1Enable);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpRange1Enable]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].cRange2Enable);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpRange2Enable]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].iBeginTime1);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpBeginTime1]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].iLastTime1);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpLastTime1]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].iBeginTime2);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpBeginTime2]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

            memset(szTemp,0x00,sizeof(szTemp));
            memset(szInfoKey,0x00,sizeof(szInfoKey));
            
            sprintf(szTemp,"%d", g_sysConfModule->s_EpTimeRange[i][j].iLastTime2);
            sprintf(szInfoKey,"%s.Seq%02d.%s",
                    EpTimeRangeKey[e_cEpForbitType],
                    j,
                    EpTimeRangeKey[e_cEpLastTime2]
                    );
            cfgSockSetValue(szSection,szInfoKey,szTemp); 

        }
    }
    
    SaveWriteFile();
    return 0;
}

static int   SaveEpVLoopStepIntervalConf(void)
{
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_VLOOP_STEP_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane0Step01);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane0Stp01],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane0Step12);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane0Stp12],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane1Step01);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane1Stp01],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane1Step12);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane1Stp12],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane2Step01);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane2Stp01],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane2Step12);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane2Stp12],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane3Step01);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane3Stp01],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpVLoopStepInfo.uiLane3Step12);
    cfgSockSetValue(szSection,\
                    SysVEpStepInterKey[e_lane3Stp12],\
                    szTemp);
                    
    SaveWriteFile();
    return 0;
    

}

static  int  SaveFuncEnableConf(void)
{
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_FUNC_ENABLE_FLAG_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.cFlowRateFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_flowRate],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.cAlarmRecFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_alarmRec],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.cSpeedAmendFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_speedAmend],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.cFlowRateInter);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_flowRateInter],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.cXmlFileSendFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_xmlFileSendFlag],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usFlashStartLine);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_flashStartLine],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucDspFramDifEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_dspFrmDiffEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucDspStopDistan);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_dspStopDistance],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucDspOneLampEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_dspOneLampEn],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucRedLightDelay);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_redLightDelay],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucRedTurnGreenSnapEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_redTurnGreenSnapEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucNoDirReportSnapEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_noDirReportSnapEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usFreqFlashDelayUSec);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_freqFlashDelaySec],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usppFLightDelayUSec);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_ppFlashLightDelaySec],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usepFLightDelayUSec);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_epFlashLightDelaySec],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucDebugLogSaveFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_debugShellLogSaveFlag],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucArithDebugShowFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_arithDebugShowFlag],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucVDLGreenMode);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_vdlGreenDectFlag],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucPressLineSanpFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_pressLineSanpFlag],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucBackWordsSanpFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_backWordsSanpFlag],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucEpPpSnap0Or1);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_epPpSnap0Or1],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucUdiskWeldSta);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_UdiskWeldSta],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usFreqFlashStartLine);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_freqFlashStartLine],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucEpCarSpeedOsdEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_epCarSpeedOsdEn],\
                    szTemp);
					
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucUdiskEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_uDiskEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucEpPpSnapPosition);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_EpPpSnapPosition],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usRedLReviseShutter);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_RedLReviseShutter],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usNetVehiclePort);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_NetVehiclePort],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucUFdiskEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_UFdiskEnFlag],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cGammaVal);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_GammaVal],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cFlashLineMode);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_flashLinesMode],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cDyncJudFlashLines);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_DyncJudgeFlashLines],\
                    szTemp);

    memset(szTemp, 0x05, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cSpeedAmendX);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_speedAmendX],\
                    szTemp);

    memset(szTemp, 0x05, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cSpeedAmendY);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_speedAmendY],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usCsSpeedAmendNumerator);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_ppCsSpeedAmend],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucEpPpPicMode);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_epPpPicMode],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usSaturation);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_fpgaSatuPam],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.usGPixsThresh);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_fpgaGPixThresh],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cDubalFEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_dubalFModeEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%f", g_sysConfModule->s_FuncEnableInfo.fDubalFShut);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_dubalFShutVal],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%f", g_sysConfModule->s_FuncEnableInfo.fDubalFGain);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_dubalFGainVal],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cRedDecSaturation);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_redDectSaturation],\
                    szTemp);
                    
    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cRedDecBright);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_redDectBright],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucPlateSegFlag);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_PlateSegFlag],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucPicQuality);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_PicQuality],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucRedLReviseDegree);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_RedLReviseDegree],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucChangeRoad);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_ChangeRoadSnapFlag],\
                    szTemp);

    for (int i = 0 ;i < 4 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.cLaneChgType[i]);
        sprintf(szInfoKey,"%s%02d",
                SysFuncEnableKey[e_crSnapMode],
                i
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucPicAssistDebug);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_PicAssistDebug],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucCrossLineParkEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_CrossLineParkEn],\
                    szTemp);


    for (int i = 0 ;i < 5 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_FuncEnableInfo.aucPressLine[i]);
        sprintf(szInfoKey,"%s%02d",
                SysFuncEnableKey[e_cPressLine],
                i
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucSDEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_SDEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.ucSDFdiskEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_SDFdiskEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cOnvifEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_cOnvifEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cSipSdkEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_cSipSdkEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cV2En);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_cV2En],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cV3En);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_cV3En],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cHaloTakeOutEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_HaloTakeOutEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cHaloAreaRange);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_HaloAreaRange],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cGB28181En);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_cGB28181En],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.cRestartTimeEn);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_RestartTimeEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp, "%d", g_sysConfModule->s_FuncEnableInfo.uiRestartTime);
    cfgSockSetValue(szSection,\
                    SysFuncEnableKey[e_RestartTime],\
                    szTemp);

    SaveWriteFile();
    return 0;
}

static  int  SavePpFuncEnConf(void)
{
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PP_FUNC_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucOsdPlateScopeEn);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_OsdPlateScopeEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.usPlateScopeIncSize);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_PlateScopeIncSize],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucRadarSpeedGap);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_RadarSpeedGap],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucRadarForwardLim);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_RadarForwardLim],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucRadarBackwardLim);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_RadarBackwardLim],\
                    szTemp);                    

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucisShieldReverse);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_isShieldReverse],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucIsDecTaillight);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_isDecTaillight],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucFaceIdenEn);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_faceIdenEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucOlviaRadaParalEn);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_OlviaRadaParalEn],\
                    szTemp);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PpFuncConfInfo.ucFaceOvenlayEn);
    cfgSockSetValue(szSection,\
                    PpFuncEnableKey[e_faceOverlayEn],\
                    szTemp);

    SaveWriteFile();
    return 0;
}

int   SaveCamCtrlConf(void)
{
    int i =0;
    int j =0;
	char szTemp[512];
	char szInfoKey[512];
	char szSection[32];

	if(NULL==g_sysConfModule)
		return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_CAMERA_CTRL_CONF_INFO]);

    for(i =0; i < MAX_CAM_GROUP_NUM; i++)
    {
    	for(j=0; j < MAX_CAM_CTRL_NUM; j++)
    	{

    		memset(szTemp,0x00,sizeof(szTemp));
    		memset(szInfoKey,0x00,sizeof(szInfoKey));
    		
    		sprintf(szTemp,"%d",g_sysConfModule->s_CameraCtrlConfInfo[i][j].cCamGroup);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camGroup]
                    );
    		cfgSockSetValue(szSection,szInfoKey,szTemp);

    		memset(szTemp,0x00,sizeof(szTemp));
    		memset(szInfoKey,0x00,sizeof(szInfoKey));
    		
    		sprintf(szTemp,"%d",g_sysConfModule->s_CameraCtrlConfInfo[i][j].uiCamaraType);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camType]
                    );
    		cfgSockSetValue(szSection,szInfoKey,szTemp); 
    		
    		memset(szTemp,0x00,sizeof(szTemp));
    		memset(szInfoKey,0x00,sizeof(szInfoKey));
    		
    		sprintf(szTemp,"%d",g_sysConfModule->s_CameraCtrlConfInfo[i][j].cCtlType);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camManner]
                    );
    		cfgSockSetValue(szSection,szInfoKey,szTemp); 


    		memset(szTemp,0x00,sizeof(szTemp));
    		memset(szInfoKey,0x00,sizeof(szInfoKey));

            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMaxVal = \
            judgeCamCtlVal(g_sysConfModule->s_CameraCtrlConfInfo[i][j].uiCamaraType,\
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMaxVal);
    		
    		sprintf(szTemp,"%f",g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMaxVal);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camMaxVal]
                    );
    		cfgSockSetValue(szSection,szInfoKey,szTemp); 


    		memset(szTemp,0x00,sizeof(szTemp));
    		memset(szInfoKey,0x00,sizeof(szInfoKey));

            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMinVal = \
            judgeCamCtlVal(g_sysConfModule->s_CameraCtrlConfInfo[i][j].uiCamaraType,\
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMinVal);
    		
    		sprintf(szTemp,"%f",g_sysConfModule->s_CameraCtrlConfInfo[i][j].fMinVal);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camMinVal]
                    );
    		cfgSockSetValue(szSection,szInfoKey,szTemp); 

    		memset(szTemp,0x00,sizeof(szTemp));
    		memset(szInfoKey,0x00,sizeof(szInfoKey));

            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fFixVal = \
            judgeCamCtlVal(g_sysConfModule->s_CameraCtrlConfInfo[i][j].uiCamaraType,\
            g_sysConfModule->s_CameraCtrlConfInfo[i][j].fFixVal);
    		
    		sprintf(szTemp,"%f",g_sysConfModule->s_CameraCtrlConfInfo[i][j].fFixVal);
            sprintf(szInfoKey,"CamGroup%02d.CtlSeq%02d.%s",
                    i,
                    j,
                    CameraCtrlConfInfoKey[e_camFixVal]
                    );
    		cfgSockSetValue(szSection,szInfoKey,szTemp); 

        }
    }
    SaveWriteFile();
    return 0;
}
static  int  SavePicNetHddConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PIC_NET_HDD_CONF_INFO]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PicNetHddConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdEnable],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PicNetHddConfInfo.uiStorageType);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdType],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_PicNetHddConfInfo.uiIpAddr;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdIpAddr],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PicNetHddConfInfo.szFolderName);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdFolder],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PicNetHddConfInfo.szUserName);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdUserName],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PicNetHddConfInfo.szPassWord);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdPassWord],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PicNetHddConfInfo.uiAvaiable);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdAvaiable],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PicNetHddConfInfo.uiUDiskSum);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_uDiskCheckSum],\
                    szTemp);
                    
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SavePicNetHddConf error!\n");
    }
    return ret;
    
}
static  int  SaveRecNetHddConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_REC_NET_HDD_CONF_INFO]);
        
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_RecNetHddConfInfo.uiEnable);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdEnable],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_RecNetHddConfInfo.uiStorageType);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdType],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_RecNetHddConfInfo.uiIpAddr;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdIpAddr],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_RecNetHddConfInfo.szFolderName);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdFolder],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_RecNetHddConfInfo.szFolderName);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdFolder],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_RecNetHddConfInfo.szUserName);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdUserName],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_RecNetHddConfInfo.szPassWord);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdPassWord],\
                    szTemp);
     memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_RecNetHddConfInfo.uiAvaiable);
    cfgSockSetValue(szSection,\
                    NetHddConfInfoKey[e_hdAvaiable],\
                    szTemp);
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveRecNetHddConf error!\n");
    }
    return ret;
    
}

static  int  SaveNRMConf(void)
{
	int i =0;
    int ret =0;
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];
    struct in_addr in;
    
    if(NULL==g_sysConfModule)
        return (-1);
    

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_NRM_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_NrmConfInfo.cEnable);
    cfgSockSetValue(szSection,\
                    NrmConfInfoKey[e_NRMEn],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_NrmConfInfo.uiMainNRMIp;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    NrmConfInfoKey[e_MainNRMIP],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_NrmConfInfo.uiBakNRMIp;
    in.s_addr = htonl(in.s_addr);
	sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    NrmConfInfoKey[e_BakNRMIP],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_NrmConfInfo.usMainNRMPort);
    cfgSockSetValue(szSection,\
                    NrmConfInfoKey[e_MainNRMPort],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_NrmConfInfo.usBakNRMPort);
    cfgSockSetValue(szSection,\
                    NrmConfInfoKey[e_BakNRMPort],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_NrmConfInfo.szPuID);
    cfgSockSetValue(szSection,\
                    NrmConfInfoKey[e_NRUPUId],\
                    szTemp);

	for (i =0 ; i< MAX_CHANNEL_NUM; i++ )
    {
	    memset(szTemp,0x00,sizeof(szTemp));
	    in.s_addr = g_sysConfModule->s_NrmConfInfo.uiLastNRUIp[i];
	    in.s_addr = htonl(in.s_addr);

	    memset(szInfoKey,0x00,sizeof(szInfoKey));
	    sprintf(szInfoKey,"%s_%d", NrmConfInfoKey[e_LastNRUIp], i);
		sprintf(szTemp,"%s",inet_ntoa(in));
	    cfgSockSetValue(szSection,\
	                    szInfoKey,\
	                    szTemp);
	                    
	    memset(szTemp,0x00,sizeof(szTemp));
	    memset(szInfoKey,0x00,sizeof(szInfoKey));
	    sprintf(szInfoKey,"%s_%d", NrmConfInfoKey[e_LastNRUHttpPort], i);
	    sprintf(szTemp,"%d",g_sysConfModule->s_NrmConfInfo.usLastNRUHttpPort[i]);
	    cfgSockSetValue(szSection,\
	                    szInfoKey,\
	                    szTemp);
	}
	


    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveNRMConf error!\n");
    }
    return ret;
    
}

static  int  SaveDctlConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    for (int i=0;i<MAX_DCTL_NUM;i++)
    {
        memset(szSection,0x00,sizeof(szSection));
        sprintf(szSection,"%s%02d",sys_ini_section[e_DCTL_CONF_INFO],i);
            
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].cEnable);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_hdEnable],\
                        szTemp);
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].cComId);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dComId],\
                        szTemp);
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].uiVehicleType);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dVehDect],\
                        szTemp);
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].cDatabits);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dDatabits],\
                        szTemp);
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].cStopbits);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dStopbits],\
                        szTemp);
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].cParity);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dParity],\
                        szTemp);
        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].uiBaudrate);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dBaudrate],\
                        szTemp);

        memset(szTemp,0x00,sizeof(szTemp));
        sprintf(szTemp,"%d",g_sysConfModule->s_DctlConfInfo[i].usMaxMeasureSpeed);
        cfgSockSetValue(szSection,\
                        DctlConfInfoKey[e_dMaxMeasureSpeed],\
                        szTemp);
                        
    }
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}
static  int  SavePlatformConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PLATFORM_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Platform330ConfInfo.cEnable);
    cfgSockSetValue(szSection,\
                    PlatformConfInfoKey[e_p330Enable],\
                    szTemp);

    for (int i = 0 ;i < 4 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_Platform330ConfInfo.sz330DriveWayId[i]);
        sprintf(szInfoKey,"%s%02d",
                PlatformConfInfoKey[e_p330DriveWayId],
                i
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Platform330ConfInfo.sz330DevId);
    cfgSockSetValue(szSection,\
                    PlatformConfInfoKey[e_p330DevId],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Platform330ConfInfo.sz330DirId);
    cfgSockSetValue(szSection,\
                    PlatformConfInfoKey[e_p330DirId],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Platform330ConfInfo.sz330Key);
    cfgSockSetValue(szSection,\
                    PlatformConfInfoKey[e_p330Key],\
                    szTemp);
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}

static  int  SaveEp330Conf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    char szInfoKey[256];

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_330_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpPlatform330ConfInfo.cEnable);
    cfgSockSetValue(szSection,\
                    ep330ConfInfoKey[e_epP330Enable],\
                    szTemp);

    for (int i = 0 ;i < 4 ; i++)
    {
        memset(szTemp,0x00,sizeof(szTemp));
        memset(szInfoKey,0x00,sizeof(szInfoKey));
        sprintf(szTemp,"%d",g_sysConfModule->s_EpPlatform330ConfInfo.sz330DriveWayId[i]);
        sprintf(szInfoKey,"%s%02d",
                ep330ConfInfoKey[e_epP330DriveWayId],
                i
                );
        cfgSockSetValue(szSection,szInfoKey,szTemp);
    }
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpPlatform330ConfInfo.sz330DevId);
    cfgSockSetValue(szSection,\
                    ep330ConfInfoKey[e_epP330DevId],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpPlatform330ConfInfo.sz330DirId);
    cfgSockSetValue(szSection,\
                    ep330ConfInfoKey[e_epP330DirId],\
                    szTemp);
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpPlatform330ConfInfo.sz330Key);
    cfgSockSetValue(szSection,\
                    ep330ConfInfoKey[e_epP330Key],\
                    szTemp);
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}

/* begin add by wangb 2010-3-22 for v3 platform */
static  int  SaveV3Conf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_V3_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_PlatformV3ConfInfo.uiV3SerIp;
    in.s_addr = htonl(in.s_addr);
    sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    v3ConfInfoKey[e_v3ServerIp],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV3ConfInfo.usV3Port);
    cfgSockSetValue(szSection,\
                    v3ConfInfoKey[e_v3ServerPort],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PlatformV3ConfInfo.szDevNo);
    cfgSockSetValue(szSection,\
                    v3ConfInfoKey[e_v3DevNo],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PlatformV3ConfInfo.szDevPwd);
    cfgSockSetValue(szSection,\
                    v3ConfInfoKey[e_v3DevPwd],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV3ConfInfo.cV3ServerSeq);
    cfgSockSetValue(szSection,\
                 v3ConfInfoKey[e_v3ServerSeq],\
                 szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV3ConfInfo.cV3TimeSynFlag);
    cfgSockSetValue(szSection,\
              v3ConfInfoKey[e_v3TimeSyn],\
              szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV3ConfInfo.cV3Enable);
    cfgSockSetValue(szSection,\
                 v3ConfInfoKey[e_v3Enable],\
                 szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}
/* end add by wangb 2010-3-22 for v3 platform */

/* begin add by wangb 2012-1-10 for v2 platform */
static  int  SaveV2Conf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_V2_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_PlatformV2ConfInfo.uiV3SerIp;
    in.s_addr = htonl(in.s_addr);
    sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    v2ConfInfoKey[e_v2ServerIp],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV2ConfInfo.usV3Port);
    cfgSockSetValue(szSection,\
                    v2ConfInfoKey[e_v2ServerPort],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PlatformV2ConfInfo.szDevNo);
    cfgSockSetValue(szSection,\
                    v2ConfInfoKey[e_v2DevNo],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PlatformV2ConfInfo.szDevPwd);
    cfgSockSetValue(szSection,\
                    v2ConfInfoKey[e_v2DevPwd],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV2ConfInfo.cV3ServerSeq);
    cfgSockSetValue(szSection,\
                 v2ConfInfoKey[e_v2ServerSeq],\
                 szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV2ConfInfo.cV3TimeSynFlag);
    cfgSockSetValue(szSection,\
              v2ConfInfoKey[e_v2TimeSyn],\
              szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_PlatformV2ConfInfo.cV3Enable);
    cfgSockSetValue(szSection,\
                 v2ConfInfoKey[e_v2Enable],\
                 szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}
/* end add by wangb 2012-1-10 for v2 platform */

static  int  SaveGB28181Conf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_GB28181_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Gb28181ConfInfo.szServerId);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ServerId],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Gb28181ConfInfo.szServerDomain);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ServerDomain],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Gb28181ConfInfo.szServerIp);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ServerIp],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Gb28181ConfInfo.uiServerPort);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ServerPort],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Gb28181ConfInfo.uiRegisterExpiry);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181RegisterExpiry],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Gb28181ConfInfo.uiKeepLiveTime);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181KeepLiveTime],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Gb28181ConfInfo.uiConMode);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ConMode],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Gb28181ConfInfo.szClientId);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ClientId],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Gb28181ConfInfo.szClientUserName);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ClientUserName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_Gb28181ConfInfo.szClientPasswd);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181ClientPasswd],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Gb28181ConfInfo.uiPackMode);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181PackMode],\
                    szTemp);                    

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_Gb28181ConfInfo.cEnFlag);
    cfgSockSetValue(szSection,\
                    gb28181ConfInfoKey[e_Gb28181EnFlag],\
                    szTemp);                    


    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}

/* begin add by wb 2010-8-6 for nm platform */
static  int  SaveNMConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    struct in_addr in;

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_NM_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_sysNMConfInfo.uiNMSerIp;
    in.s_addr = htonl(in.s_addr);
    sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    nmConfInfoKey[e_nMServerIp],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_sysNMConfInfo.usNMPort);
    cfgSockSetValue(szSection,\
                    nmConfInfoKey[e_nMServerPort],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_sysNMConfInfo.cEnable);
    cfgSockSetValue(szSection,\
                    nmConfInfoKey[e_nMEnable],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_sysNMConfInfo.cDefaultGWEn);
    cfgSockSetValue(szSection,\
                    nmConfInfoKey[e_defaultGWEn],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    in.s_addr = g_sysConfModule->s_sysNMConfInfo.uiDefaultGWIp;
    in.s_addr = htonl(in.s_addr);
    sprintf(szTemp,"%s",inet_ntoa(in));
    cfgSockSetValue(szSection,\
                    nmConfInfoKey[e_defaultGWIp],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_sysNMConfInfo.uiDefaultHttpdPort);
    cfgSockSetValue(szSection,\
                    nmConfInfoKey[e_defaultHttpPort],\
                    szTemp);
    
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
}
/* end add by wangb 2010-3-22 for v3 platform */

static  int  SavePPIrregInfoConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_PP_IRREG_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szOverSpeedSmallPercent50Code);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppOverSpeedSmallPercent50Code],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szOverSpeedSmallPercent50Name);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppOverSpeedSmallPercent50Name],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szOverSpeedLargePercent50Code);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppOverSpeedLargePercent50Code],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szOverSpeedLargePercent50Name);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppOverSpeedLargePercent50Name],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szReverseRunCode);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppReverseRunCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szReverseRunName);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppReverseRunName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szInfringeProhibCode);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppInfringeProhibCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_PpIrregConfInfo.szInfringeProhibName);
    cfgSockSetValue(szSection,\
                    SysPpIrregInfoKey[e_ppInfringeProhibName],\
                    szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SavePPIrregInfoConf error!\n");
    }
    return ret;
}

/*Start of wb on 2012-5-17 10:45 2.0.0.1*/

/* 保存电警违章类型描述及代码配置 */
static  int  SaveEPIrregInfoConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_IRREG_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpIrregConfInfo.cDispFlag);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epIrregDispFlag],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szRunRedCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epRunRDCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szRunRedName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epRunRDName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szReverseRunCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epReverseRunCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szReverseRunName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epReverseRunName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szInfringForbidLineCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epInfringForbidLineCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szInfringForbidLineName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epInfringForbidLineName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szInfringDirLaneRunCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epInfringDirLaneRunCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szInfringDirLaneRunName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epInfringDirLaneRunName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szStopOnTrafficNoEntryCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epStopOnTrafficNoEntryCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szStopOnTrafficNoEntryName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epStopOnTrafficNoEntryName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szHighwayInfringDirLaneCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_HighwayInfringDirLaneCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szHighwayInfringDirLaneName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_HighwayInfringDirLaneName],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szStoppedVehicleCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_StoppedVehicleCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpIrregConfInfo.szStoppedVehicleName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_StoppedVehicleName],\
                    szTemp);


    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveElecPoliceIrregInfo error!\n");
    }
    return ret;
}
/*End of wb on 2012-5-17 10:45 2.0.0.1*/

/* 保存电警新增违章类型描述及代码配置 */
static  int  SaveEPNewAddIrregInfoConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];
    
    if(NULL==g_sysConfModule)
        return (-1);

    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_NEWADD_IRREG_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpNewAddIrregConfInfo.szRunInNoLaneCode);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epRunInNoLaneCode],\
                    szTemp);
    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpNewAddIrregConfInfo.szRunInNoLaneName);
    cfgSockSetValue(szSection,\
                    SysEpIrregInfoKey[e_epRunInNoLaneName],\
                    szTemp);



    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveEPNewAddIrregInfoConf error!\n");
    }
    return ret;
}
/*End of wb on 2012-5-17 10:45 2.0.0.1*/

/*Start of wb on 2012-5-17 14:34 2.0.0.1 for 添加电警车牌识别配置*/

static  int  SaveEPLPRInfoConf(void)
{
    int ret =0;
    char szTemp[256];
    char szSection[256];
    //char szInfoKey[256];
    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_EP_LPR_INFO_CONF]);

    memset(szTemp, 0x00, sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpLprConfInfo.ucLPREn);
    cfgSockSetValue(szSection,\
                    epLPRConfInfoKey[e_evLPREn],\
                    szTemp);
                    
    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%s",g_sysConfModule->s_EpLprConfInfo.szLPRFirstChar);
    cfgSockSetValue(szSection,\
                    epLPRConfInfoKey[e_evLPRFirstCode],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpLprConfInfo.ucLPRCConfidence);
    cfgSockSetValue(szSection,\
                    epLPRConfInfoKey[e_evLPRCConfiden],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpLprConfInfo.usLPRWidth);
    cfgSockSetValue(szSection,\
                    epLPRConfInfoKey[e_evLPRWidth],\
                    szTemp);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_EpLprConfInfo.usLPRHeight);
    cfgSockSetValue(szSection,\
                    epLPRConfInfoKey[e_evLPRHeight],\
                    szTemp);

    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("SaveEPLprConf error!\n");
    }
    return ret;
}
/*End of wb on 2012-5-17 14:34 2.0.0.1*/

static  int  SaveAppModeConf(void)
{
    int ret =0;
    char szSection[32];
    char szTemp[256];

    
    if(NULL==g_sysConfModule)
        return (-1);
    
    memset(szSection,0x00,sizeof(szSection));
    sprintf(szSection,"%s",sys_ini_section[e_APPMODE_CONF_INFO]);

    memset(szTemp,0x00,sizeof(szTemp));
    sprintf(szTemp,"%d",g_sysConfModule->s_AppMode);
    cfgSockSetValue(szSection,\
                     AppModeConfInfoKey[e_appmode],
                    szTemp);
    ret = SaveWriteFile();
    if( 0!= ret )
    {
        CONF_ERROR("%s error!\n", __FUNCTION__);
    }
    return ret;
     
}

static int MakeDefaultConfFile(void)
{
	int nRet = 0;
	
	nRet = DefaultSysConfValue();
	nRet += SaveConfFile();

	return nRet;
}
static int DefaultSysConfValue(void)
{	
    int nRet = 0;
	if(NULL==g_sysConfModule)
		return (-1);

	nRet += LoadConf();
	return 0;
}
static int SaveConfFile(void)
{
    int nRet = 0;

    nRet += SaveNetConf();
    nRet += SaveRtspConf();
    nRet += SaveRecConf();
    nRet += SaveVideoConf();
    nRet += SavePassPortOverallConf();
    nRet += SavePassPortVehicleConf();
    nRet += SaveElecPolliceOverallConf();
    nRet += SaveElecPolliceVehicleConf();
    nRet += SaveVLConf();
    nRet += SavePassPortFtpConf();
    nRet += SaveElecPoliceFtpConf();
    nRet += SaveNtpConf(); 
    nRet += SaveCamCtrlConf();
    nRet += SaveDctlConf(); 
    nRet += SavePlatformConf();
    nRet += SaveV3Conf();
    nRet += SavePicNetHddConf();
    nRet += SaveRecNetHddConf(); 
    nRet += SaveSysDefConf();
    nRet += SaveWriteFile();
    nRet += SaveAppModeConf();
    nRet += SaveNMConf();
    nRet += SaveCamTimeRangeConf();
    nRet += SaveFuncEnableConf();
    nRet += SaveEPTimeRangeConf();
    nRet += SaveEpVLoopStepIntervalConf();
    nRet += SaveEp330Conf();
    nRet += SaveV2Conf();
    nRet += SaveEPAssistRedLightConf();
    nRet += SaveEPIrregInfoConf();
    nRet += SaveEPLPRInfoConf();
    nRet += SavePpFuncEnConf();
    nRet += SaveEPSOTNoEntryConf();
    nRet += SaveEpJpgOsdConf();
    nRet += SavePpJpgOsdConf();
    nRet += SavePPIrregInfoConf();
	nRet += SavePassPortBackupsFtpConf();
    nRet += SaveNRMConf();
    nRet += SaveEpBackupsFtpConf();
    nRet += SaveEPNewAddIrregInfoConf();
    nRet += SaveGB28181Conf();
    return nRet;
}



