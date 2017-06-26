#ifndef __RECORD_H__
#define __RECORD_H__

//#include "dvs_common.h"

#define MAX_PATH_NAME_LEN (128)
#define DEFAULT_FILE_TIME (10*60)           /*十分钟*/
#define DEFAULT_FRAME_NUM (25*2*10*60)      /*十分钟*/

/*打印信息间隔*/
#define REC_TIME_INTERVAL (30)

/*录像最小空间和一次性删除录像空间大小，以M为单位*/
#define LIMIT_FREE_SPACE    (512)
#define DELET_ONCE_SICE       (1024)

/*存储类型*/
enum PARTITION_TYPE
{
    REC_DISK=1,
    REC_NAS
};


typedef struct tagSearchFileInfo
{
	unsigned long ulBeginTime;
	unsigned long ulEndTime;
	unsigned long ulSize;
	char szFileName[128];
}SFI;

typedef struct tagRecStatusInfo
{
	unsigned int bRecing;//是否正在录像
	unsigned int nFileSize;//当前录像文件大小
	char szFileName[256];//当前录像文件名
}RSI,*PRSI;


/*录像策略*/
#define POLICY_FULL_STOP                      (1)
#define POLICY_FULL_OVERWRITE           (2)
#define POLICY_SEVERAL_DAYS               (3)
#define POLICY_QUOTA                             (4)
#define POLICY_DEFAULT              POLICY_FULL_OVERWRITE
/*录像参数*/
typedef struct
{
    unsigned short QuotaDay;
    unsigned short QuotaSpace;
}QUOTA_PARAM;

typedef union
{
    unsigned long SeveralDays;
    QUOTA_PARAM QuotaParam;
}POLICY_PARAM;



/***********************************************************
* 函数名称: RecInit
* 参数描述:
*                         nTotalChanNum:总通道数
*                         RecPreTime:预录时间，单位秒，如果有音频需*2
*                         pBuf:缓存头地址
*                         BufLen:缓存大小
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 初始化本模块，在系统启动时执行一次
***********************************************************/
int RecInit(int nTotalChanNum);



/***********************************************************
* 函数名称: RecUnInit
* 参数描述:
*
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 退出本模块，在系统退出时执行一次
***********************************************************/
int RecUnInit();



/***********************************************************
* 函数名称: SetBufInfo
* 参数描述:
*                         ChanNo:通道号
*                         QueLen:需要队列长度:预录时间*帧率(如果有音频再乘2)
*                         pBuf:缓存头地址
*                         BufLen:缓存大小
* 返回:            成功返回0 ，失败返回-1,-2,-3
* 函数作用: 初始化通道缓存
***********************************************************/
int SetBufInfo(int ChanNo ,int QueLen, char *pBuf, int BufLen);



/***********************************************************
* 函数名称: SetRecPolicy
* 参数描述:
*                         nChanNo:通道号
*                         RecPolicy:录像策略
*                         POLICY_PARAM:策略参数
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 设置通道策略
***********************************************************/
int SetRecPolicy(int nChanNo, int RecPolicy, POLICY_PARAM Param);



/***********************************************************
* 函数名称: AddRecCycleTime
* 参数描述:
*                         nChanNo:通道号
*                         nDay:星期几(0代表每天)
*                         tBegin，tEnd:开始结束时间
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 设置周期性计划
***********************************************************/
int AddRecCycleTime(int nChanNo, int nDay, unsigned int tBegin, unsigned int tEnd);



/***********************************************************
* 函数名称: DelRecCycleTime
* 参数描述:
*                         nChanNo:通道号
*                         nDay:星期几(0代表每天)
*                         tBegin，tEnd:开始结束时间
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 删除周期性计划
***********************************************************/
int DelRecCycleTime(int nChanNo, int nDay, unsigned int tBegin, unsigned int tEnd);



/***********************************************************
* 函数名称: AddRecOnceTime
* 参数描述:
*                         nChanNo:通道号
*                         tBegin，tEnd:开始结束时间
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 设置一次性计划
***********************************************************/
int AddRecOnceTime(int nChanNo, unsigned long tBegin, unsigned long tEnd);



/***********************************************************
* 函数名称: DelRecOnceTime
* 参数描述:
*                         nChanNo:通道号
*                         tBegin，tEnd:开始结束时间
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 删除一次性计划
***********************************************************/
int DelRecOnceTime(int nChanNo, unsigned long tBegin, unsigned long tEnd);



/***********************************************************
* 函数名称: AddRecPartition
* 参数描述:
*                         nChanNo:通道号
*                         sPartitionName:分区名
*                         PartitionType:分区类型
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 添加通道存储分配
***********************************************************/
int AddRecPartition(int nChanNo, char *sPartitionName, PARTITION_TYPE PartitionType);



/***********************************************************
* 函数名称: RecStart
* 参数描述:
*
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 启动录像线程
***********************************************************/
int RecStart();



/***********************************************************
* 函数名称: RecDataWrite
* 参数描述:
*                         nChannel:通道号（从0开始）
*                         pFrame:帧首地址
*                         nLen:帧长度
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 写入一帧录像
***********************************************************/						
int RecDataWrite(int nChannel,unsigned char *pFrame,int nLen);	



/***********************************************************
* 函数名称: RecSearchByTime
* 参数描述:
*                         nChannel:通道号（从0开始）
*                         utcBeginTime:起始时间
*                         utcEndTime:结束时间
*                         pItem:存放查询结果指针
*                         nMaxCount:最大录像条数
*
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 按时间查询录像
***********************************************************/
int RecSearchByTime(int nChannel,unsigned long utcBeginTime,unsigned long utcEndTime, SFI *pItem,int nMaxCount);



/***********************************************************
* 函数名称: RecSearchByEvent
* 参数描述:
* 			      nChannel:通道号（从0开始）
*			      nEventType:事件类型，和报警类型定义相同，值为0xffff时查询所有事件
*			      utcBeginTime:起始时间
*			      utcEndTime:结束时间
* 			      pItem:存放查询结果指针
*			      nMaxCount:最大录像条数
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 按事件查询录像
***********************************************************/
int RecSearchByEvent(int nChannel,unsigned short nEventType,unsigned long utcBeginTime,unsigned long utcEndTime, SFI *pItem,int nMaxCount);
						
						

/***********************************************************
* 函数名称: RecDebugSwitch
* 参数描述:
* 			      nLevel:调试等级，取值范围0x1-0x4
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 修改录像调试等级
***********************************************************/	
int RecDebugSwitch(int nLevel);



/***********************************************************
* 函数名称: DumpPlan
* 参数描述:

* 返回:            成功返回0 ，失败返回-1
* 函数作用: 查看录像计划
***********************************************************/				
int DumpPlan();



/***********************************************************
* 函数名称: RecDump
* 参数描述:
*
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 当前录像文件信息查看
***********************************************************/	
int RecDump();



/***********************************************************
* 函数名称: DirDump
* 参数描述:
*			      nChannel:通道号（从0开始）
* 返回;            成功返回0 ，失败返回-1
* 函数作用: 当前录像目录查看
***********************************************************/	
int DirDump(int nChannel);



/***********************************************************
* 函数名称: PartitionInfoDump
* 参数描述:
*
* 返回;            成功返回0 ，失败返回-1
* 函数作用: 所有通道的分区信息
***********************************************************/	
int PartitionInfoDump();



/***********************************************************
* 函数名称: RecAlarm
* 参数描述:
* 			      nChannel:通道号（从0开始）
*			      nRecAlarmFlag:报警状态 1-报警，0-消警
*			      nDelayTime:持续录像时间，报警状态为0时有效
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 报警录像
***********************************************************/	
//int RecAlarm(int nChannel, EAlarmType eAlarmType, int nRecAlarmFlag, int nDelayTime);



/***********************************************************
* 函数名称: ManuleRec
* 参数描述:
* 			      nChannel:通道号（从0开始）
*			      bFlag:手动录像状态 1-开始，0-结束
*			      nDelayTime:手动录像延续时间
* 返回:            成功返回0 ，失败返回-1
* 函数作用: 手动录像
***********************************************************/	
int ManuleRec(int nChannel, int bFlag, int nDelayTime);



/***********************************************************
* 函数名称: IsPartitionExist
* 参数描述:
*			      szPartitionPath:分区路径
* 返回:            分区存在返回1 ，分区不存在返回0
* 函数作用: 分区是否存在
***********************************************************/	
int IsPartitionExist(const char *sPartitionPath);



/***********************************************************
* 函数名称: PartitionSpace
* 参数描述:
*
* 返回:             空间大小，单位为MB ，分区不存在返回0
* 函数作用:  获得该分区大小
***********************************************************/	
unsigned long PartitionSpace(const char *sPartitionPath);



/***********************************************************
* 函数名称: PartitionFreeSpace
* 参数描述:
*
* 返回:            分区剩余空间大小，单位为MB ，分区不存在返回0
* 函数作用: 获得该分区剩余空间大小，单位为MB
***********************************************************/	
unsigned long PartitionFreeSpace(const char *sPartitionPath);



/***********************************************************
* 函数名称: IsDirExist
* 参数描述:
*                         sDirName:目录路径
* 返回:            目录存在返回1 ，目录不存在返回0
* 函数作用: 目录是否存在
***********************************************************/	
int IsDirExist(const char * sDirName);



/***********************************************************
* 函数名称: RecSetFileProtect
* 参数描述:
*			      szFilePath:录像路径名
*			      status:1-设置保护状态,0-取消包含状态
* 返回:            成功返回0，失败返回-1
* 函数作用: 设置录像文件为保护文件，或者取消录像文件的保护状态
***********************************************************/	
int SetProtectState(const char *szFilePath,int bProStatus);



/***********************************************************
* 函数名称: IsDirExist
* 参数描述:
*                         szFilePath:文件详细路径
* 返回:            文件存在返回1 ，文件不存在返回0
* 函数作用: 文件是否存在
***********************************************************/	
int IsFileExist(const char *szFilePath);



/***********************************************************
* 函数名称: FileSize
* 参数描述:
*                         szFilePath:文件详细路径
* 返回:            返回文件大小，单位字节，文件不存在返回0
* 函数作用: 文件大小
***********************************************************/	
int FileSize(const char *szFilePath);



/***********************************************************
* 函数名称: RecStatusQuery
* 参数描述:
*			      nChannelId:录像通道号(从0开始)
*			      pInfo:查询结果存放结构
* 返回:            成功返回0，失败返回-1
* 函数作用: 查询当前录像状态
***********************************************************/	
int RecStatusQuery(int nChannelId,RSI *pInfo);


#endif

