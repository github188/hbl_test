#ifndef _MC_LOG_H__
#define _MC_LOG_H__

//单条log最大长度
#define MAX_LOG_LEN				(128)

//Log模块类型
typedef enum tagLogMoudleType
{
    LMT_UNDEFINE = 0,
    LMT_LOG,
    LMT_SYSCONF,
    LMT_SHELL,
    LMT_DCTL,
    LMT_ALERT,
    LMT_RECV,
    LMT_SEND,
    LMT_DVSPH,
    LMT_GEPH,
    LMT_VRP,
    LMT_REC,
    LMT_DSPCOM,
    LMT_SYSCALL,
    LMT_DSPFW,
    LMT_ALARM,
    LMT_NULL
}ELogModuleType;

//Log类型
typedef enum tagLogType
{
    LT_EXCEP	= 0x01,
    LT_ERROR	= 0x02,
    LT_WARNING  = 0x04,
    LT_INFO		= 0x08,
    LT_DEBUG	= 0x10,
    LT_ALL		= 0xff,
    LT_NONE		= 0x00
}ELogType;

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* 函数名称: mcLogInit
* 参数描述:
*			szCfgFileName:日志路径文件名
* 返回值:  操作成功返回0 ，操作失败返回-1
* 函数作用:初始化Log模块，在系统启动时执行一次
***********************************************************/
int mcLogInit(const char * szCfgFileName);


/***********************************************************
* 函数名称: mcLogUnInit
* 参数描述:
* 返回值:  操作成功返回0 ，操作失败返回-1
* 函数作用:退出Log模块，在系统退出时执行
***********************************************************/
int mcLogUnInit();


/***********************************************************
* 函数名称: mcLog
* 参数描述:
* 			lm：模块类型，取值为ELogModuleType
*			lt: Log类型，取值为ELogType
*			format,...:Log内容
* 返回值:  无返回
* 函数作用:写日志
***********************************************************/
void mcLog(ELogModuleType lm, ELogType lt, const char* format, ...);


/***********************************************************
* 函数名称: mcLogQuery
* 参数描述:
*			lt: Log类型，取值为ELogType
*			utcBeginTime:开始时间
*			utcEndTime:结束时间
*			pBuf：日志存放缓冲
*			nBufLen：日志缓冲最大长度
* 返回值:  操作成功返回0 ，操作失败返回-1
* 函数作用:日志查询
***********************************************************/
int mcLogQuery(ELogType lt,unsigned int utcBeginTime,unsigned int utcEndTime,char *pBuf,int nBufLen);



/***********************************************************
* 函数名称: mcLogShow
* 参数描述:	
*			bEnable:是否打印log（1-打开日志打印，0-关闭日志打印）
* 返回值:  操作成功返回0 ，操作失败返回-1
* 函数作用:打开日志打印功能，保持日志时同时打印
***********************************************************/
int mcLogShow(int bEnable);



/***********************************************************
* 函数名称: mcDebugSwitch
* 参数描述:
*			nLevel:调试等级（0-关闭，1-4：调试等级）
* 返回值:  操作成功返回0 ，操作失败返回-1
* 函数作用:打开Log调试开关
***********************************************************/
int mcDebugSwitch(int nLevel);



/***********************************************************
* 函数名称: shell_print
* 参数描述:
*			format,...:打印内容
* 返回值:  操作成功返回0 ，操作失败返回-1
* 函数作用:打印调试信息
***********************************************************/
int shell_print(const char* format, ...);


int initShellDebug(int iSaveFile, int iCheckType);

int shell_file_print(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif



