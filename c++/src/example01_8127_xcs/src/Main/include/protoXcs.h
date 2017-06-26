/******************************************************************************
 * protoXcs.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.09, wangbin Create
 * --------------------
 ******************************************************************************/

#ifndef _PROTOCOL_XCS__H__
#define _PROTOCOL_XCS__H__
#include <time.h>

#include "xcsGlobalDef.h"

#define XCS_PROTOCOL_VER    (1)

#define XCS_SYNC_WORD   0x5A5B

/* 1. login protocol  */
#define  CPH_LOG                        0x00        /* 登陆协议头 */
/* 1.1 用户登录 */
#define  CPE_LOG_USER_LOGIN             0x00

/* 1.2 用户退出 */
#define  CPE_LOG_USER_LOGOUT            0x01

/* 2. video channel protocol  */
#define  CPH_VCHANN                     0x01        /* 视频通道协议头 */
/* 2.1 请求实时图像 */
#define  CPE_VCHANN_REQ_RTVIDEO         0x00

/* 2.2 断开实时图像 */
#define  CPE_VCHANN_CUT_RTVIDEO         0x01

/* 2.3 前端图片手动抓拍 */
#define  CPE_VCHANN_SNAP_SHOT           0x02

/* 2.4 保留 */

/* 2.5 抓拍参数查询 */
#define  CPE_VCHANN_SNAP_PARAM_QUERY    0x04

/* 2.6 抓拍参数设置 */
#define  CPE_VCHANN_SNAP_PARAM_SET      0x05

/* 2.7 视频参数查询 */
#define  CPE_VCHANN_VIDEO_PARAM_QUERY   0x06

/* 2.8 视频参数配置 */
#define  CPE_VCHANN_VIDEO_PARAM_CONFIG  0x07

/* 2.9 数据上报 */
#define  CPE_VCHANN_DATA_REPORT         0x08
/* 2.10 修改上报数据类型*/
#define  CPE_VCHAN_DATA_REPORT_SET      0x09

/* 3.0 外设控制协议 */
#define  CPH_DEV_CTL                    0x02        /* 外设控制协议头 */

/* 3.1 采集设备属性查询 */
#define  CPE_DEV_CTL_CAP_QUERY          0x00

/* 3.2 采集设备属性配置 */
#define  CPE_DEV_CTL_CAP_SET            0x01



/* 4. query protocol  */
#define  CPH_XCS_QUERY                  0x03       /* 查询协议头 */

/* 4.1 系统时间查询 */
#define  CPE_SYS_TIME_QUERY             0x00

/* 4.2 系统时间设置 */
#define  CPE_SYS_TIME_SET               0x01

/* 4.3 系统版本查询 */
#define  CPE_SYS_RELEASE_QUERY          0x02

/* 4.4 系统日志查询 */
#define  CPE_SYS_LOG_QUERY              0x03

/* 4.5 系统日志下载 */
#define  CPE_SYS_LOG_DOWN               0x04

/* 4.6 远程升级 */
#define  CPE_REMOTE_UPDATE              0x05

/* 4.7 升级进度报告 */
#define  CPE_UPDATE_PROGRESS            0x06

/* 4.8 远程重启 */
#define  CPE_REMOTE_REBOOT              0x07

/* 4.9 NTP服务器查询 */
#define  CPE_NTP_SERVER_QUERY           0x08

/* 4.10 NTP服务器设置 */
#define  CPE_NTP_SERVER_SET             0x09

/* 4.11 远程恢复设备出厂设置 */
#define  CPE_REMOTE_RESTORE_DEV_PARAM   0x0a

/* 4.12 定时重启时间查询 */
#define  CPE_RESTART_TIME_QUERY         0x0b

/* 4.13 定时重启时间设置 */
#define  CPE_RESTART_TIME_SET           0x0c

/* 5.Bayonet protocol */
#define CPH_BAYONET                     0x04        /* 卡口相关协议头 */

/* 5.1 卡口全局信息查询 */
#define CPE_BAYONET_GLOBAL_INFO_QUERY   0x00

/* 5.2 卡口全局信息配置 */
#define CPE_BAYONET_GLOBAL_INFO_SET     0x01

/* 5.3 卡口车道信息查询 */
#define CPE_BAYONET_CAR_CHANNEL_INFO_QUERY  0x02

/* 5.4 卡口车道信息配置 */
#define CPE_BAYONET_CAR_CHANNEL_INFO_SET    0x03

/* 5.5 卡口虚拟线圈高级信息查询 */
#define CPE_BAYONET_VIRTUAL_LOOP_INFO_QUERY 0x04

/* 5.6 卡口虚拟线圈高级信息设置 */
#define CPE_BAYONET_VIRTUAL_LOOP_INFO_SET   0x05

/* 5.7 卡口手动抓拍 */
#define CPE_BAYONET_HAND_SNAP               0x06

/* 5.8 卡口保留 */
#define CPE_BAYONET_RESERVE                 0x07

/* 5.9 卡口FTP信息查询 */
#define CPE_BAYONET_FTP_INFO_QUERY          0x08

/* 5.10 卡口FTP信息设置 */
#define CPE_BAYONET_FTP_INFO_SET            0x09

/* 5.11 卡口图片字幕叠加信息查询 */
#define CPE_BAYONET_JPG_OSD_CONF_QUERY      0x0a

/* 5.12 卡口图片字幕叠加信息设置 */
#define CPE_BAYONET_JPG_OSD_CONF_SET        0x0b

/* 5.13 卡口违章类型描述及代码查询 */
#define CPE_BAYONET_IRREG_INFO_QUERY		0x0c

/* 5.14 卡口违章类型描述及代码配置 */
#define CPE_BAYONET_IRREG_INFO_SET			0x0d

/* 5.15 备份用卡口FTP信息查询 */
#define CPE_BAYONET_BACKUPS_FTP_INFO_QUERY  0x0e

/* 5.16 备份用卡口FTP信息设置 */
#define CPE_BAYONET_BACKUPS_FTP_INFO_SET    0x0f


/* 6. electronic police protocol */
#define CPH_ELECT_POLICE                        0x05        /* 电子警察相关协议头 */

/* 6.1 电子警察全局信息查询 */
#define CPE_ELECT_POLICE_GLOBAL_INFO_QUERY      0x00

/* 6.2 电子警察全局信息配置 */
#define CPE_ELECT_POLICE_GLOBAL_INFO_SET        0x01

/* 6.3 电子警察车道信息查询 */
#define CPE_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY 0x02

/* 6.4 电子警察车道信息设置 */
#define CPE_ELECT_POLICE_CAR_CHANNEL_INFO_SET   0x03

/* 6.5 电子警察红灯状态查询 */
#define CPE_ELECT_POLICE_RED_LIGHT_STA_QUERY    0x04

/* 6.6 模拟车辆行驶抓拍 */
#define CPE_ELECT_POLICE_SIMU_RUN_SNAP          0x05

/* 6.7 强制红灯查询 */
#define CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY 0x06

/* 6.8 强制红灯配置 */
#define CPE_ELECT_POLICE_RED_LIGHT_ENFORCE_SET   0x07

/* 6.9 电子警察FTP信息查询 */
#define CPE_ELECT_POLICE_FTP_INFO_QUERY          0x08

/* 6.10 电子警察FTP信息设置 */
#define CPE_ELECT_POLICE_FTP_INFO_SET            0x09

/* 6.11 电子警察禁止行驶时间段查询 */
#define CPE_ELECT_POLICE_TIME_RANGE_QUERY        0x0a

/* 6.12 电子警察禁止行驶时间段设置 */
#define CPE_ELECT_POLICE_TIME_RANGE_SET          0x0b


/* 6.13 电子警察辅助红灯查询 */
#define CPE_EP_ASSIST_RED_LIGHT_QUERY        	 0x0c

/* 6.14 电子警察辅助红灯设置 */
#define CPE_EP_ASSIST_RED_LIGHT_SET		         0x0d

/* 6.15 电警车牌识别信息查询 */
#define CPE_ELECT_POLICE_LPR_QUERY				 0x0e

/* 6.16 电警车牌识别信息配置 */
#define CPE_ELECT_POLICE_LPR_SET				 0x0f

/* 6.17 电警违章类型描述及代码查询 */
#define CPE_ELECT_POLICE_IRREG_INFO_QUERY		 0x10

/* 6.18 电警违章类型描述及代码配置 */
#define CPE_ELECT_POLICE_IRREG_INFO_SET			 0x11

/* 6.19 电子警察遇堵禁入查询 */
#define CPE_EP_SOT_NO_ENTRY_QUERY        	 	 0x12

/* 6.20 电子警察遇堵禁入设置 */
#define CPE_EP_SOT_NO_ENTRY_SET			         0x13

/* 6.21 电警图片字幕叠加信息查询 */
#define CPE_EP_JPG_OSD_CONF_QUERY   			 0x14

/* 6.22 电警图片字幕叠加信息设置 */
#define CPE_EP_JPG_OSD_CONF_SET        			 0x15

/* 6.23 备份用卡口FTP信息查询 */
#define CPE_EP_BACKUPS_FTP_INFO_QUERY  			 0x16

/* 6.24 备份用卡口FTP信息设置 */
#define CPE_EP_BACKUPS_FTP_INFO_SET    			 0x17

/* 6.25 电警新增违章类型描述及代码查询 */
#define CPE_ELECT_POLICE_NEWADD_IRREG_INFO_QUERY 0x18

/* 6.26 电警新增违章类型描述及代码配置 */
#define CPE_ELECT_POLICE_NEWADD_IRREG_INFO_SET	 0x19

/* 7. record protocol */
#define CPH_XCS_RECORD                      0x06

/* 7.1 磁盘信息查询 */
#define CPE_RECORD_DISK_INFO_QUERY          0x00

/* 7.2 保留 */

/* 7.3 录像下载、回放方式查询 */
#define CPE_RECORD_DOWNLOAD_PLAYTYPE_QUERY  0x02

/* 7.4 录像下载回放方式设置 */
#define CPE_RECORD_DOWNLOAD_PLAYTYPE_SET    0x03

/* 7.5 周期性录像计划查询 */
#define CPE_RECORD_PERIOD_PLAN_QUERY        0x04

/* 7.6 周期性录像计划增加 */
#define CPE_RECORD_PERIOD_PLAN_ADD          0x05

/* 7.7 周期性录像计划删除 */
#define CPE_RECORD_PERIOD_PLAN_DEL          0x06

/* 7.8 周期性录像计划更新 */
#define CPE_RECORD_PERIOD_PLAN_UPDATE       0x07

/* 7.9 通道录像参数查询 */
#define CPE_RECORD_CHANNEL_PARAM_QUERY      0x08

/* 7.10 通道录像参数配置 */
#define CPE_RECORD_CHANNEL_PARAM_SET        0x09

/* 7.11 检索录像记录 */
#define CPE_XCS_RECORD_SEARCH               0x0A

/* 7.12 录像文件回放 */
#define CPE_XCS_RECORD_PLAYBACK             0x0B

/* 7.13 录像文件下载 */
#define CPE_RECORD_FILE_DOWNLOAD            0x0C

/* 8. CAMARA info protocol */
#define CPH_CAMARA                          0x07

/* 8.1 高清摄像机属性查询 */
#define CPE_CAMARA_ATTRIB_QUERY             0x00

/* 8.2 高清摄像机参数范围查询 */
#define CPE_CAMARA_PARAM_QUERY              0x01

/* 8.3 摄像机状态参数查询 */
#define CPE_CAMARA_STA_INFO_QUERY           0x02

/* 8.4 保留 */

/* 8.5 摄像机控制内容查询 */
#define CPE_CAMARA_CTL_INFO_QUERY           0x04

/* 8.6 摄像机控制内容设置 */
#define CPE_CAMARA_CTL_INFO_SET             0x05

/* 8.7 摄像机时间段查询 */
#define CPE_CAMARA_TIME_RANGE_QUERY         0x06

/* 8.8 摄像机时间段设置 */
#define CPE_CAMARA_TIME_RANGE_SET           0x07

/* 8.9 摄像机默认配置参数查询 */
#define CPE_CAMARA_DEFAULT_CONF_QUERY       0x08


/* 9. net info protocol */
#define CPH_NET_INFO                        0x08

/* 9.1 网络信息查询 */
#define CPE_NET_INFO_QUERY                  0x00

/* 9.2 网络信息配置 */
#define CPE_NET_INFO_SET                    0x01

/* 9.3 图片网络存储信息查询 */
#define CPE_NET_PIC_NET_SAVE_INFO_QUERY     0x02

/* 9.4 图片网络硬盘信息配置 */
#define CPE_NET_PIC_NET_SAVE_INFO_SET       0x03

/* 9.5 录像网络存储信息查询 */
#define CPE_NET_RECORD_NET_SAVE_INFO_QUERY  0x04

/* 9.6 录像网络硬盘信息配置 */
#define CPE_NET_RECORD_NET_SAVE_INFO_SET    0x05

/* 9.7 云存储NRM信息查询 */
#define CPE_NET_RECORD_MANAGE_INFO_QUERY	0x06

/* 9.8 云存储NRM信息配置 */
#define CPE_NET_RECORD_MANAGE_INFO_SET	    0x07

/* 10. dev connect protocol */
#define CPH_DEV_CONNECT_PRO                 0x09

/* 10.1 330平台接入信息查询 */
#define CPE_330_PLATFORM_CON_QUERY          0x00

/* 10.2 330平台接入信息配置 */
#define CPE_330_PLATFORM_CON_SET            0x01


/* 10.3 v3平台接入信息查询 */
#define CPE_V3_PLATFORM_CON_QUERY           0x02

/* 10.4 v3平台接入信息配置 */
#define CPE_V3_PLATFORM_CON_SET             0x03

/* 10.5 电警330平台接入信息查询 */
#define CPE_EP_330_PLATFORM_CON_QUERY       0x04

/* 10.6 电警330平台接入信息配置 */
#define CPE_EP_330_PLATFORM_CON_SET         0x05

/* 10.7 v2平台接入信息查询 */
#define CPE_V2_PLATFORM_CON_QUERY           0x06

/* 10.8 v2平台接入信息配置 */
#define CPE_V2_PLATFORM_CON_SET             0x07

/* 10.9 Gb28181平台接入信息查询 */
#define CPE_GB28181_PLATFORM_CON_QUERY      0x08

/* 10.10 Gb28181平台接入信息配置 */
#define CPE_GB28181_PLATFORM_CON_SET        0x09


/* 11.0 heartbeat */
#define CPH_HEART_BEAT						0x0a

/* 12.0 netManage Info */
#define CPH_NET_MANAGE_PRO					0x0b

/* 12.1 网管信息查询 */
#define CPE_NET_MANAGE_QUERY          		0x00



/* 13. ack protocol  */
#define  CPH_ACK                    0x7A       /* ACK协议头 */
#define  CPH_NAK                    0x7C       /* NAK协议头 */

/***********************************************************************************/
/* data structure */
/***********************************************************************************/
#define _PACKED_ __attribute__((packed))



/* 所有协议里面关于范围的定义，均用此结构 */
typedef struct tagXCSStrArea
{
    unsigned short usAreaPosX;      /* 区域左上角坐标X */
    unsigned short usAreaPosY;      /* 区域左上角坐标Y */
    unsigned short usAreaWidth;     /* 区域宽度 */
    unsigned short usAreaHeight;    /* 区域高度 */
}_PACKED_ STR_XCS_AREA, *PSTR_XCS_AREA;

typedef struct tagStrPacketHeader
{
	unsigned int	SyncWord:16;
	unsigned int 	Version:4;
	unsigned int	IsAckOrNot:4;
	unsigned int	DataType:8;
	unsigned int	SubDataType:8;
    unsigned int    resv:24;
	unsigned char	ucErrorCode;
    unsigned char   ucResv2[3];
	unsigned int	uiPayloadLength;
}_PACKED_ STR_PACKET_HEADER, *PSTR_PACKET_HEADER;

/* 1.1 用户登录，只支持单用户登录 */
typedef struct tagStrUserLogin
{
	char cUserName[32];		    //用户名称
	char cUserPasswd[32];		//用户密码
	unsigned int uiDecConfVer;  //指定匹配客户端版本
	char cResv[124];
}_PACKED_ STR_USER_LOGIN , *PSTR_USER_LOGIN;

typedef struct tagStrUserLoginRep
{
	unsigned int uiDecConfVer;  //指定匹配客户端版本
	char cResv[128];
}_PACKED_ STR_USER_LOGIN_REP , *PSTR_USER_LOGIN_REP;

/* 2. video channel protocol  */
/* 2.1 请求实时图像 */
typedef struct tagStrRealVQuery
{
    unsigned char   ucChannel;
    char            cVideoConnMode;
    unsigned char   ucConnectOK;    // 连接成功与否
						    		//=1，成功监控
							        //=0，监控失败，原因见bReason
	unsigned char   ucConReason;    // 如果连接不成功，指出其原因:
                                    //该通道目前不存在			0x01
									//该通道目前正在远程回放	0x02
									//该通道目前正在本地回放	0x04
									//该通道目前正在远程备份	0X08
									//图像连结数>=4				0x80
									//网络发送参数错误			0x81
									//视频丢失					0x82
									//通道保护					0x83
									//该通道正在单独监控		0x84
									//该通道当前无效编码		0x85
    unsigned int    ReceiveIpAddr;
    unsigned short  ReceivePort;
    char            bActive;      
    char            bStream;        
    unsigned int    nPackLen;
}_PACKED_ STR_REAL_VIDEO_QUERY, *PSTR_REAL_VIDEO_QUERY;

typedef struct tagStrRealVQuery STR_REAL_VIDEO_RESULT, *PSTR_REAL_VIDEO_RESULT;

/* 2.2 断开实时图像 */

typedef struct tagStrRealVQuery STR_REAL_VIDEO_CUT, *PSTR_REAL_VIDEO_STOP;

/* 2.3 前端图片手动抓拍 */
typedef struct tagStrVChannelSnapShop
{
    char    cVChannel;          /* 视频通道号 从1开始 */
    char    cRev[131];
}_PACKED_ STR_VCHANN_SNAP_SHOT, *PSTR_VCHANN_SNAP_SHOT;

/* 2.5 抓拍参数查询 */
typedef struct tagStrVChannelSnapParamQuery
{
    char    cPicQuality;        /* 图像质量等级，1~5 */
    char    cRev[131];
}_PACKED_ STR_VCHANN_SNAP_PARAM_QUERY, *PSTR_VCHANN_SNAP_PARAM_QUERY;

/* 2.6 抓拍参数设置 */
typedef STR_VCHANN_SNAP_PARAM_QUERY STR_VCHANN_SNAP_PARAM_SET;

/* 2.7 视频参数查询 */
typedef struct tagStrVChannelVParamQuery
{
    unsigned short  usChannel;          /* 视频通道号 从1开始 */
    char            cVQuality;          /* 视频质量 1~5 */
    char            cFrameRate;
    unsigned int    uiBitRate;
    char            cKeyFInterval;      /* 关键帧间隔 */
    char            cImageResol;        /* 图像分辨率 */
    char            cEncodeType;
    char            cVBR;               /* 是否变码率 */
    unsigned short  usXPos;             /* 字幕在屏幕上坐标的x值 */
    unsigned short  usYPos;             /* 字幕在屏幕上坐标的y值 */
    char            szContent[64];     /* 字幕内容 */

	unsigned short  usDebugFrameX;
	unsigned short  usDebugFrameY;

	unsigned char   ucDebugFrameEn;
    unsigned char   ucTimeDisMode;		/* 时间显示格式 0:无毫秒 1:有毫秒 */
	unsigned short  usTimeXPos;			/* 时间显示坐标 X */

	unsigned short  usTimeYPos;			/* 时间显示坐标 Y */
	
    char            cRev[118];
}_PACKED_ STR_VCHANN_VIDEO_PARAM_QUERY, *PSTR_VCHANN_VIDEO_PARAM_QUERY;

/* 2.8 视频参数配置 */
typedef STR_VCHANN_VIDEO_PARAM_QUERY STR_VCHANN_VIDEO_PARAM_SET;

#if 0
/* 2.9 数据上报 */
/* ePassPortSnap的ParamData数据结构 */
typedef struct tagStrPassPortSnapPic
{
    unsigned int    uiSubVer;
    char            cCarChannel;
    char            cCarType;
    char            cRev1[1];
    char            cCarDir;
    unsigned short  usCarLen;
    unsigned short  usCarSpeed;
    char            cInfringeSta;       /* 是否违章 */
    char            cInfringeTyte;      /* 违章类型 */
    STR_AREA        strPlateNumArea;    /* 车牌区域定义 */
    char            cConfident;         /* 置信度 */
    char            cRev[1];
}_PACKED_ STR_PASS_PORT_SNAP_PIC, *PSTR_PASS_PORT_SNAP_PIC;

/* EElectronicPoliceSnap的ParamData数据结构 */
typedef struct tagStrElectPoliceSnapPic
{
    unsigned int    uiSubVer;
    char            cCarChannel;
    char            cRev1[2];
    char            cConfident;         /* 置信度 */
    STR_AREA        strPlateNumArea;    /* 车牌区域定义 */
    char            cRev[1];
}_PACKED_ STR_ELECT_POLICE_SNAP_PIC, *PSTR_ELECT_POLICE_SNAP_PIC;

/* 数据类型为EManualSnap,ePassPortSnap，EElectronicPoliceSnap时消息内容数据结构定义 */
typedef struct tagStrDataReportConnect
{
    unsigned int    dwParamLen;
    char            *pbParamData;
    char            bPicNum;
    char            bRev1[3];
    unsigned int    *pPicLen;           /* 每张图片长度，需根据图片张数动态创建该缓冲长度:4*bPicNum */
    unsigned int    *pPicData;          /* 图片实际内容，4字节对齐 */
}_PACKED_ STR_DATA_REPORT_CONTENT, *PSTR_DATA_REPORT_CONTENT;

/* 数据类型为EComCaptureInfo, EComOutputInfo时消息内容数据结构定义 */
typedef struct tagStrDataReportContent
{
    unsigned int   uiSubVer;            /* 版本号 */
    unsigned int   uiDATALen;           /* 数据长度 */
    unsigned int   *pDataContent;       /* 数据内容 */
}_PACKED_ STR_DATA_REPORT_CONTENT, *PSTR_DATA_REPORT_CONTENT;

/* 数据上报数据结构定义 */
typedef struct tagStrVChannelDataReport
{
    unsigned int    uiVersion;          /* 版本号 */
    unsigned int    uiDataType;
    time_t          tTime;              /* 通知信息触发时间 */
    unsigned int    *pData;             /* 上报数据内容 */
}_PACKED_ STR_VCHANN_DATA_REPORT, *PSTR_VCHANN_DATA_REPORT;
#endif
/* 2.10  数据上报参数修改 */

typedef struct tagStrDataReportSet
{
    unsigned int   uiRecvIp;
    unsigned short usRecvPort;
    unsigned short usConnectType;
    unsigned int   uiRecvType;
    unsigned char  resv[128];
}_PACKED_ STR_DATA_REPORT_SET;


/* 3.0 外设控制协议 */

/* 3.1 采集设备属性查询 */
typedef struct tagStrDevCtlCapQuery
{
    char            cEnable;            /* 使能位 */
    char            cComId;             /* 串口号id */
    char            cDataBits;          /* 数据位 */
    char            cStopBits;          /* 停止位 */
    char            cParity;            /* 奇偶校验位 */
    char            cRev1[3];
    unsigned int    uiBaudrate;         /* 波特率 */
    unsigned int    uiVehicleType;      /* 车检器类型 */
    unsigned short  usVehHighSpeed;     /* 车检器最高能测速度 */
    char            cRev2[126];
}_PACKED_ STR_DEV_CTL_CAP_QUERY, *PSTR_DEV_CTL_CAP_QUERY;

/* 3.2 采集设备属性配置 */
typedef struct tagStrDevCtlCapQuery STR_DEV_CTL_CAP_SET;

/* 4. query protocol  */

/* 4.1 系统时间查询 */
typedef struct tagStrSysTimeQuery
{
    unsigned int    uiSysTime;          /* 系统时间，以秒为单位 */
    char            cRev[128];
}_PACKED_ STR_SYS_TIME_QUERY, *PSTR_SYS_TIME_QUERY;

/* 4.2 系统时间设置 */
typedef STR_SYS_TIME_QUERY STR_SYS_TIME_SET;

/* 4.3 系统版本查询 */
typedef struct tagStrSysReleaseQuery
{
    unsigned int    uiProcType;         /* 产品型号 */
    char            cSerialNum[16];     /* 序列号 */
    unsigned int    uiHardwareVer;      /* 硬件版本号 */
    unsigned int    uiSoftwareVer;      /* 软件版本号 */
    unsigned int    uiProtocolVer;      /* 协议版本号 */
    char			szcMac[18];			/* MAC地址 */
    char            cRev[128];
}_PACKED_ STR_SYS_RELEASE_QUERY, *PSTR_SYS_RELEASE_QUERY;

/* 4.4 系统日志查询 */
typedef struct tagStrSysLogPcQuery
{
    unsigned int    uiLogType;
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    char            cRev[128];
}_PACKED_ STR_SYS_LOG_PC_QUERY, *PSTR_SYS_LOG_PC_QUERY;

typedef struct tagStrSysLogQueryAck
{
    unsigned int    uiLogType;
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    unsigned int    uiLogLen;
    char            *pbLogData;
}_PACKED_ STR_SYS_LOG_QUERY_ACK, *PSTR_SYS_LOG_QUERY_ACK;

/* 4.5 系统日志下载 */
typedef struct tagStrSysLogPcDown
{
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    char            cRev[128];
}_PACKED_ STR_SYS_LOG_PC_DOWN, *PSTR_SYS_LOG_PC_DOWN;

typedef struct tagStrSysLogDownAck
{
    unsigned int    uiBeginTime;
    unsigned int    uiEndTime;
    unsigned int    uiLogLen;
    char            *pbLogData;
}_PACKED_ STR_SYS_LOG_DOWN_ACK, *PSTR_SYS_LOG_DOWN_ACK;

/* 4.6 远程升级 */
typedef struct tagStrRemoteUpdate
{
    unsigned char   ucURL[256];
    unsigned char   ucRcv[128];
}_PACKED_ STR_REMOTE_UPDATE, *PSTR_REMOTE_UPDATE;

/* 4.7 升级进度报告 */
typedef struct tagStrUpdateProgress
{
    unsigned int    uiUpdateStep;       /* 升级步骤1:检验升级包 2:擦除 3:写入 */
    unsigned int    uiPercentage;       /* 百分比，表示擦除和写入的时间 */
    char            cRev[128];
}_PACKED_ STR_UPDATE_PROGRESS, *PSTR_UPDATE_PROGRESS;


/* 4.9 NTP服务器查询 */
typedef struct tagStrNtpServerQuery
{
    unsigned int    uiEnable;
    unsigned int    uiServerIp;
    unsigned short  usJudgeCycle;		/* 对时周期(单位:分钟) */
	unsigned char   ucJudegPrecision;	/* 对时精度(单位:秒) */
    char            cRev[125];
}_PACKED_ STR_NTP_SERVER_QUERY, *PSTR_NTP_SERVER_QUERY;

/* 4.10 NTP服务器设置 */
typedef STR_NTP_SERVER_QUERY STR_NTP_SERVER_SET;

/* 4.11 定时重启时间查询 */
typedef struct tagStrRestartTimeQuery
{
    unsigned int    uiRestartTime;          /* 定时重启时间，以秒为单位 */
    char			cEnFlag;				/* 定时重启使能标志 */
    char            cRev[127];
}_PACKED_ STR_RESTART_TIME_QUERY, *PSTR_RESTART_TIME_QUERY;

/* 4.12 定时重启时间设置 */
typedef STR_RESTART_TIME_QUERY STR_RESTART_TIME_SET;

/* 5.Bayonet protocol */

/* 5.1 卡口全局信息查询 */
typedef struct tagStrBayonetGlobalInfoQuery
{
    char    szRoadName[32];
    char    szDirName[32];
    char	szPassPortId[8];	 	 /* 卡口ID */
	char	cOSDColor;				 /* 图片叠加OSD颜色 */
	char    cValidLanes;			 /* 实际使用车道数 */
	char    cVLWorkMode;			 /* 虚拟线圈工作模式 */

	unsigned char ucLPREn;			 /* 车牌识别使能标志 */
	char    szLPRFirstChar[32];		 /* 车牌首字符 */
	unsigned char ucLPRCConfidence;	 /* 车牌首字符置信度 */
	unsigned char ucVSpeedMeaFlag;	 /* 视频测速使能标志 */

	unsigned short usVSpeedMeaLen;	 /* 视频测速实际距离(单位:厘米) */

	unsigned char aucFreqFlashEn[4];  /* 是否使能频闪功能 */
    unsigned char aucFlashTrigMode[4];/* 闪光灯触发方式 */
    unsigned char aucStrobeFlashFre[4];/* 频闪灯倍频系数 */
    char     szPpDevCode[MAX_DEV_CODE_LEN];/* 卡口设备编号 */
    char     szDirId[8];                 /* 方向ID */
    
    unsigned char   ucFillInLSwitchEn;    /* 补光切换使能标志 */
    unsigned char   ucGrayThresholdVal;    /* 切换灰度阀值 */
    unsigned char    ucCheckSwitchCycle;    /* 检测视频灰度值周期 */
    
    char            cRsv;
    char            szExPpDevCode[EXT_DEV_CODE_LEN];/* 扩展设备编号字符串 */

	unsigned char	ucSmartSwitchEn;		/* 补光智能切换使能 */
	unsigned char	ucTimerSwitchEn;		/* 补光定时切换使能 */
	char			cRsv2[2];
	unsigned int	uiFillInLightBeginTime;	/* 补光起始时间 */
	unsigned int	uiFillInLightLastTime;	/* 持续时间 */    


    char     cRev[24];
}_PACKED_ STR_BAYONET_GLOBAL_INFO_QUERY, *PSTR_BAYONET_GLOBAL_INFO_QUERY;

/* 5.2 卡口全局信息配置 */
typedef struct tagStrBayonetGlobalInfoQuery STR_BAYONET_GLOBAL_INFO_SET;


/* 5.3 卡口车道信息查询 */
typedef struct tagStrBayonetCarChannelInfoPcQuery
{
    unsigned int   uiCarRoadId;     /* 查询车道号ID */
}_PACKED_ STR_BAYONET_CAR_CHANNEL_INFO_PC_QUERY, *PSTR_BAYONET_CAR_CHANNEL_INFO_PC_QUERY;

/* nCoilDesp COM 类型时的结构定义 */
typedef struct tagStrCoilComDesp
{
    char    cComId;
    char    cBoardId;               /* 板卡号 */
    char    cFrontCoilId;           /* 车检器前线圈 */
    char    cBehindCoilId;          /* 车检器后线圈 */
    char    cGroupId;               /* 地感线圈组号，对应科骏车检器车道号 */
    char    cRev[13];
}_PACKED_ STR_COIL_COM_DESP, *PSTR_COIL_COM_DESP;

/* nCoilDesp GPIO 类型时的结构定义 */
typedef struct tagSTRCOILGPIODESP
{
    char    cFrontCoilId;           /* 车检器前线圈 */
    char    cBehindCoilId;          /* 车检器后线圈 */
    char    cRev[16];
}_PACKED_ STR_COIL_GPIO_DESP, *PSTR_COIL_GPIO_DESP;

typedef union tagSTR_COIL_DESP
{
    STR_COIL_COM_DESP strComCoilDesp;
    STR_COIL_GPIO_DESP strGpioCoilDesp;
}_PACKED_ STR_COIL_DESP, *PSTR_COIL_DESP;

typedef struct tagStrXcsTimeSlotSnapObj
{
    unsigned int uiBeginTimeSecs;	/* 起始时间 */
    unsigned int uiLastTimeSecs;	/* 持续时间 */
    unsigned char ucSnapObj;		/* 抓拍目标 */
    unsigned char ucRsv[3];
}_PACKED_ STR_XCS_TIME_SLOT_SNAP_OBJ, *PSTR_XCS_TIME_SLOT_SNAP_OBJ;

typedef struct tagSTR_BAYONET_CAR_CHANNEL_INFO_QUERY
{
    unsigned int    uiVersion;
    unsigned int    uiCarRoadId;
    unsigned short  usOSDPosX;              /* 抓拍图片上的OSD显示x坐标 */
    unsigned short  usOSDPosY;              /* 抓拍图片上的OSD显示y坐标 */
    char            szOSDContent[32];       /* 抓拍图片上的OSD内容 */
    STR_XCS_AREA    strRoadArea;            /* 车道区域定义 */
    unsigned short  usFlashLightType;       /* 闪光灯或补光灯类型 */
    unsigned short  usFlashLightCtlType;    /* 闪光灯或补光灯控制方式 */
    char            cFlashComId;            /* 闪光灯控制串口号 */
    char            cLightDevType;          /* 闪光灯设备类型 */
    char            cLightCtlMode;          /* 闪光灯控制模式 */
	/*Start of ITS on 2011-5-3 11:3 2.0.0.1*/
	char			cSingalCoilPos;			/* 单线圈位置定义 */
	/*End of ITS on 2011-5-3 11:3 2.0.0.1*/
    unsigned short  ausFlashAddr[4];        /*  闪光灯485地址，或者控制端口，前面的有效，
                                                不使用时设为-1。闪光灯地址由闪光灯设备自己决定。
                                                GPIO控制端口号从1开始。*/
    unsigned short  usDetectType;           /* 检测类型:虚拟或地感线圈 */
    
	/*Start of ITS on 2011-5-3 11:5 2.0.0.1*/
	char			cGroudCoilDecType;		/* 地感线圈检测类型:双线圈或单线圈 */
	char			cSingalCoilSnapType;	/* 单线圈抓拍类型:进线圈或出线圈抓拍 */
	/*End of ITS on 2011-5-3 11:5 2.0.0.1*/

    STR_XCS_AREA    strVLArea;              /* 虚拟线圈区域定义 */
    unsigned short  usVehicleLinkMode;      /* 车检器连接方式 */
    STR_COIL_DESP   strCoilDesp;            /* 地感线圈描述 */
    
	char			cVLSnapDir;				/* 虚拟线圈抓拍方向 */
	char 			cVLDecEn;				/* 虚拟线圈检测使能 */
	unsigned short  usSingalCoilHigh;		/* 单线圈下的前线圈高度 */
	unsigned char   ucDaySnapObj;			/* 抓拍目标类型，1:全类型；2:机动车;3:非机动车 */
	unsigned char   ucNightSnapObj;			/* 抓拍目标类型，1:全类型(需补光)；2:机动车;3:非机动车 */	

	unsigned char   ucRsv1[4];    
	unsigned char   ucSnapPicNum;			/* 抓拍图片张数配置 */
	unsigned short	usSnapPicInter;			/* 两张图片抓拍间隔时长(单位:毫秒) */

    STR_XCS_TIME_SLOT_SNAP_OBJ strTimeSlotSnapObj;/* 时间段抓拍目标配置 */
    
	char			cCoilSwitchSta;		 	/* 线圈检测模式切换状态 */
   	char			cInLightSta;	     	/* 补光方式状态 */
    char			cCoilSwitchFlag;		/* 线圈切换使能标志 */

    unsigned short  usIrreSpeed;            /* 违章速度门限 */
    unsigned short  usMaxSpeed;             /* 超速最大门限 */
    unsigned short  usMinSpeed;             /* 超速最小门限 */
    unsigned short  usCoilDistance;         /* 前后地感线圈距离(厘米为单位) */
    char			cVLDispFlag;    		/* 虚拟线圈是否显示标志 */
    char			cLaneDispFlag;    		/* 车道范围是否显示标志 */
    unsigned short  usPPProtoVer;			/* 卡口车道信息协议小版本号 */
	char			cRsv5[4];				/* 保留字节 */

    unsigned char	ucInLightEn;      		/* 是否使能辅助补光 */
    char			szLaneId[8];			/* 车道id */

    char			cSnapDir;				/* 抓拍方向(0:正向;1:逆向) */
	char			cSDEnFlag;				/* 是否使能 */
	char			cTruckEnFlag;			/* 大货车是否抓拍使能 */
	char 			cRsv[1];
	unsigned int 	iSDBeginTime1;
	unsigned int 	iSDLastTime1; 			/* 持续时间1 */
	unsigned int 	iSDBeginTime2;
	unsigned int 	iSDLastTime2; 			/* 持续时间2 */	
	unsigned int 	iSDBeginTime3;
	unsigned int 	iSDLastTime3; 			/* 持续时间3 */
	
    char            cRev3[83];
}_PACKED_ STR_BAYONET_CAR_CHANNEL_INFO_QUERY, *PSTR_BAYONET_CAR_CHANNEL_INFO_QUERY;


/* 5.4 卡口车道信息配置 */
typedef struct tagSTR_BAYONET_CAR_CHANNEL_INFO_QUERY STR_BAYONET_CAR_CHANNEL_INFO_SET;

/* 5.5 卡口虚拟线圈高级信息查询 */
typedef struct tagSTR_BAYONET_VL_INFO_PC_QUERY
{
    unsigned int   uiCarRoadId;        /* 查询的车道号 */
}_PACKED_ STR_BAYONET_VL_INFO_PC_QUERY, *PSTR_BAYONET_VL_INFO_PC_QUERY;

typedef struct tagSTR_BAYONET_VL_INFO
{   
    unsigned int    uiVersion;
    unsigned int    uiCarRoadId;            /* 车道号id */
    unsigned short  usThrDayToNight;        /* 白天转为夜间的参考门限 */
    unsigned short  usThrNightToDay;        /* 夜间转为白天的参考门限 */
    unsigned int    uiCorrNightThresh;      /* 夜间相关值门限 */
    unsigned int    uiCorrDayThresh;        /* 白天相关值门限 */
    unsigned int    uiCorrMotorThresh;      /* 摩托车相关值门限 */
    unsigned int    uiCorrEveningThresh;    /* 黄昏门限值 */
    unsigned int    uiDayMotorWidth;        /* 白天摩托车宽度门限值 */
    unsigned int    uiNightMotorWidth;      /* 夜间摩托车宽度门限值 */
    unsigned char 	ucCapDelayInterval;		/* 采集延迟帧数 */

    char			cCapDelayNum;			/* 车牌检测间隔帧数 */
	char        	cMinPlatePixelSize;		/* 最小车牌像素 */
	char        	cMaxPlatePixelSize;		/* 最大车牌像素 */	
    char            cResv[124];
}_PACKED_ STR_BAYONET_VL_INFO, *PSTR_BAYONET_VL_INFO;
    
/* 5.6 卡口虚拟线圈高级信息设置 */
typedef STR_BAYONET_VL_INFO STR_BAYONET_VL_INFO_SET;

/* 5.7 卡口手动抓拍 */
typedef struct tagSTR_BAYONET_HAND_SNAP
{ 
    unsigned int    uiCarRoadId;
    unsigned int    uiCarSpeed;
    char            cResv[128];
}_PACKED_ STR_BAYONET_HAND_SNAP, *PSTR_BAYONET_HAND_SNAP;

/* 5.8 卡口保留 */

/* 5.9 卡口FTP信息查询 */
typedef struct tagSTR_BAYONET_FTP_INFO_QUERY
{ 
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char            cResv1[18];
    char            szUserName[32];
    char            szUserPasswd[32];
    char			cTRecEnable;
    unsigned char   ucTRecLast;
    unsigned short  usTRecInterval;
    char            cResv2[124];
}_PACKED_ STR_BAYONET_FTP_INFO_QUERY, *PSTR_BAYONET_FTP_INFO_QUERY;

/* 5.10 卡口FTP信息设置 */
typedef STR_BAYONET_FTP_INFO_QUERY STR_BAYONET_FTP_INFO_SET;

/* 5.11 卡口图片OSD字幕叠加信息查询 */
typedef struct tagSTR_BAYONET_JPG_OSD_CONF_QUERY
{ 
    unsigned char   ucDevSnOsdEn;		/* 设备编号 */
	unsigned char	ucPpNameOsdEn;		/* 卡口名 */
	unsigned char	ucDirNameOsdEn;		/* 方向 */
	unsigned char	ucLaneNameOsdEn;	/* 车道名 */
	
	unsigned char	ucSnapTimeOsdEn;	/* 抓拍时间 */
	unsigned char	ucCarDirOsdEn;		/* 车行方向 */
	unsigned char	ucBusNumOsdEn;		/* 车牌号码 */
	unsigned char	ucLPColorOsdEn;		/* 车牌颜色 */
	
	unsigned char	ucCarTypeOsdEn;		/* 车型 */
	unsigned char	ucCarColorOsdEn;	/* 车身颜色 */
	unsigned char	ucCurSpeedOsdEn;	/* 车速 */
	unsigned char	ucPunishSpeedOsdEn;	/* 处罚速度 */
	
	unsigned char	ucMarkSpeedOsdEn;	/* 标示速度 */
	unsigned char	ucSpeedRatioOsdEn;	/* 超速比例 */
	unsigned char	ucReguCodeOsdEn;	/* 违章代码 */
	unsigned char	ucStandReguDesOsdEn;/* 标准违章描述 */
	
	unsigned char	ucIntelReguDesOsdEn;/* 内部违章描述 */
    char            cRsv[127];
    
}_PACKED_ STR_BAYONET_JPG_OSD_CONF_QUERY, *PSTR_BAYONET_JPG_OSD_CONF_QUERY;

/* 5.12 卡口图片OSD字幕叠加信息配置 */
typedef STR_BAYONET_JPG_OSD_CONF_QUERY STR_BAYONET_JPG_OSD_CONF_SET;

/* 5.13 查询卡口违章类型描述及代码 */
typedef struct tagSTR_PP_IRREG_CONF_INFO_QUERY
{
	char	szOverSpeedSmallPercent50Code[8];	/* 超速小于50%的违章代码 */
	char	szOverSpeedSmallPercent50Name[64];	/* 超速小于50%的违章名称 */

	char	szOverSpeedLargePercent50Code[8];	/* 超速大于50%的违章代码 */
	char	szOverSpeedLargePercent50Name[64];	/* 超速大于50%的违章名称 */
	
	char	szReverseRunCode[8];				/* 逆行违章代码 */
	char	szReverseRunName[64];				/* 逆行违章名称 */

	char	szInfringeProhibCode[8]; 			/* 机动车违反禁令标志指示的违章代码 */
	char	szInfringeProhibName[64];			/* 机动车违反禁令标志指示的违章名称 */

	char	cRsv[256];
}PP_IRREG_CONF_INFO_QUERY,*PPP_IRREG_CONF_INFO_QUERY;

/* 5.14 配置卡口违章类型描述及代码 */
typedef struct tagSTR_PP_IRREG_CONF_INFO_QUERY PP_IRREG_CONF_INFO_SET;

/* 5.15 备份卡口FTP信息查询 */
typedef struct tagSTR_BAYONET_BACKUPS_FTP_INFO_QUERY
{
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char            cResv1[18];
    char            szUserName[32];
    char            szUserPasswd[32];
    char            cResv2[128];
}_PACKED_ STR_BAYONET_BACKUPS_FTP_INFO_QUERY, *PSTR_BAYONET_BACKUPS_FTP_INFO_QUERY;

/* 5.16 备份卡口FTP信息设置 */
typedef STR_BAYONET_BACKUPS_FTP_INFO_QUERY STR_BAYONET_BACKUPS_FTP_INFO_SET;

/* 6. electronic police protocol */

/* 红灯区域数据结构定义 */
typedef struct tagSTR_RED_LIGHT_AREA
{
    STR_XCS_AREA        strArea;           /* 红灯区域定义 */
}_PACKED_ STR_RED_LIGHT_AREA, *PSTR_RED_LIGHT_AREA;

typedef struct STR_XCS_RED_LIGHT_MODE_SET
{
	unsigned char ucLightStrobeEn;		/* 频闪修正是否使能 */
	unsigned char ucLightStrobeFrame;	/* 频闪修正帧数 */
	unsigned char ucSingalColLight;		/* 是否是单色灯(0:单色灯;1:复色灯) */
	unsigned char ucMapRDNo;			/* 映射到红灯检测器的红灯号 */
} _PACKED_ STR_XCS_RED_LIGHT_MODE_SET, *PSTR_XCS_RED_LIGHT_MODE_SET;



/* 交通灯颜色阐值 */
typedef struct tagSTR_LIGHT_THRESHOLD
{
    unsigned char ucRed;
    unsigned char ucGreen;
    unsigned char ucBlue;
    unsigned char ucResv;
}_PACKED_ STR_LIGHT_THRESHOLD, *PSTR_LIGHT_THRESHOLD;

/* 6.1 电子警察全局信息查询 */
typedef struct tagSTR_ELECT_POLICE_GLOBAL_INFO_QUERY
{ 
    char                szCarRoadName[32];    /* 车道名 */
    char	            szDirection[32];
    unsigned short      usPicMixStyle;        /* 图片合成方式:3合1或6合1 */
    unsigned short	    usFlashLightType;     /* 闪光灯类型 */
    unsigned short	    usFlashCtlType;       /* 控制类型:串口或IO口 */
    char	            cFlashComId;          /* 闪光灯控制串口号 */
    char	            cFlashDevType;        /* 闪光灯设备类型 */
    unsigned short	    ausFlashAddr[4];	  /* 闪光灯 */
  	unsigned char 		aucFreqFlashEn[4];	  /* 频闪灯是否使能标志 */
	unsigned char		aucFlashTrigMode[4];  /* 闪光灯触发方式 */
    unsigned char		aucStrobeFlashFre[4]; /* 频闪灯倍频系数 */
	char				szEpDevCode[MAX_DEV_CODE_LEN];	  /* 电警设备编号 */

    STR_RED_LIGHT_AREA	strRedArea[6][4];
    
    STR_LIGHT_THRESHOLD strRedLow;
    STR_LIGHT_THRESHOLD strRedHigh;
    STR_LIGHT_THRESHOLD strGreenLow;
    STR_LIGHT_THRESHOLD strGreenHigh;
    STR_LIGHT_THRESHOLD strYellowLow;
    STR_LIGHT_THRESHOLD strYellowHigh;
    char				szRoadRemarks[32];
	char        	    cDetectType;           	 /* 检测线圈类型 */
	char				cValidChanSum;			 /* 实际有效车道总数 */
	char				cCarDistance;			 /* 最后一张图片机动车离停车线距离（单位米） */
	char				cFrameInterval;			 /* 抓拍间隔帧数（范围1~5） */
	char				cLightAmendDisFlag;		 /* 红灯修正线圈是否显示标志 */

	/*Start of ITS on 2011-5-9 14:48 2.0.0.1*/
	char				cPPSendEn;				 /* 卡口图片是否上报标志 */
	/*End of ITS on 2011-5-9 14:48 2.0.0.1*/
	
	unsigned short		usEPGProVer;			 /* 电警全局信息协议小版本号 */
	char            	cLightCtlMode;			 /* 闪光灯控制模式 */
	
	/*Start of ITS on 2011-4-19 9:23 2.0.0.1*/
	char				cOSDColour;				 /* 叠加字体颜色(0:黑白 1:彩色) */
	char				cRLAmendLevel;			 /* 红灯修正深浅等级 */
	/*End of ITS on 2011-4-19 9:23 2.0.0.1*/
	
	char				cRLAmednEn;			 	 /* 红灯修正使能标志 */
	char				szRoadId[8];			 /* 路口ID */

	char				cRLDecType;				 /* 红灯检测方式 */
	
	STR_XCS_RED_LIGHT_MODE_SET strRedLightMode[6];/* 红灯修正相关模式配置 */
	char				cPPFlashEn;				 /* 卡口图片是否闪光 */
	char				szDirId[8];				 /* 方向ID */

	char				cACSyncSwitchEn;		 /* AC交流相位切换使能 */
	unsigned char		ucACSwitchThreshold;	 /* AC交流相位切换灰度阀值 */
	unsigned char		ucACDecCycle;			 /* AC交流相位检测周期 */
	
	char				cRedModifyMode;			 /* 红灯修正方法：1：最旧版本 2：较旧版本 
													3：新方法（全自动）4：新方法（自动1）
													5：新方法（手动,不修白点）
													6：新方法（手动，修白点）*/
													
	unsigned short		usDayACPhase;			 /* 白天AC交流相位值 */
	unsigned short		usNightACPhase;			 /* 晚上AC交流相位值 */

	char				szExEpDevCode[EXT_DEV_CODE_LEN];/* 扩展设备编号字符串 */
	unsigned char		ucSmartSwitchEn;		/* 补光智能切换使能 */
	unsigned char		ucTimerSwitchEn;		/* 补光定时切换使能 */
	char				cRsv2[2];
	unsigned int		uiFillInLightBeginTime;	/* 补光起始时间 */
	unsigned int		uiFillInLightLastTime;	/* 持续时间 */
    char	            cResv[14];
}_PACKED_ STR_ELECT_POLICE_GLOBAL_INFO_QUERY, *PSTR_ELECT_POLICE_GLOBAL_INFO_QUERY;

/* 6.2 电子警察全局信息配置 */
typedef struct tagSTR_ELECT_POLICE_GLOBAL_INFO_QUERY STR_ELECT_POLICE_GLOBAL_INFO_SET;

/* 6.3 电子警察车道信息查询 */
typedef struct tagSTR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY
{ 
    unsigned int   uiCarRoadId;        /* 查询的车道号 */
}_PACKED_ STR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY, *PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_PC_QUERY;



/* nRedDetectDesp COM 类型时的结构定义 */
typedef struct tagSTR_RED_LIGHT_COM_DESP
{
    char    cComId;
    char    cCarRoadId;
    char    cRev[28];
}_PACKED_ STR_RED_LIGHT_COM_DESP, *PSTR_RED_LIGHT_COM_DESP;

/* nRedDetectDesp GPIO 类型时的结构定义 */
typedef struct tagSTR_RED_LIGHT_GPIO_DESP
{
    char    cGpioId;
    char    cRev[29];
}_PACKED_ STR_RED_LIGHT_GPIO_DESP, *PSTR_RED_LIGHT_GPIO_DESP;

typedef union tagSTR_EP_RED_LIGHT_DESP
{
    STR_RED_LIGHT_COM_DESP    strEPComRedLightDesp;
    STR_RED_LIGHT_GPIO_DESP   strEPGpioRedLightDesp;
}_PACKED_ STR_EP_RED_LIGHT_DESP, *PSTR_EP_RED_LIGHT_DESP;

/* 电警虚拟线圈四点坐标结构定义 */
typedef struct STR_EP_VL_POINT
{
	unsigned short usVLX1;
	unsigned short usVLY1;
}_PACKED_ STR_EP_VL_POINT, *PSTR_EP_VL_POINT;

/* 电警交通灯结构体定义 */
typedef struct TAG_STR_XCS_TRAFFIC_LIGHT
{
	char cEnable;
	char cLightType;
	char cVehicleNum;
	char cLSta;			/* 红灯状态，在协议模块中是保留字节，在实际使用中是红灯状态值 */
}_PACKED_ STR_XCS_TRAFFIC_LIGHT, *PSTR_XCS_TRAFFIC_LIGHT;

typedef struct tagSTR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY
{
    unsigned int            uiVersion;
    unsigned int            uiCarRoadId;
    unsigned short          usOSDPosX;           /* 抓拍图片上的OSD显示x坐标 */
    unsigned short          usOSDPosY;           /* 抓拍图片上的OSD显示y坐标 */
    char                    szOSDContent[32];    /* 抓拍图片上的OSD内容 */
    STR_XCS_AREA            strRoadArea;         /* 车道区域定义 */
    unsigned short          usRedLDetectType;    /* 红灯检测方式 */
    unsigned short          usRedLConnectMode;   /* 红灯连接模式 */
    STR_EP_RED_LIGHT_DESP   strRedLDesp;         /* 红灯检测方式描述 */
    
    unsigned char        	ucMinPlatePixelSize; /* 最小车牌像素 */
	unsigned char        	ucMaxPlatePixelSize; /* 最大车牌像素 */
	unsigned char        	ucCorrEveningThresh; /* 黄昏门限值 */
    unsigned short        	usCoilDist;          /* 线圈之间的距离，厘米为单位 */
    unsigned char           ucPPDelaySnapFrames; /* 卡口图片延时抓拍帧数 */
    char                    cRev1[24];

    unsigned short          usVehicleLinkMode;   /* 车检器连接方式 */
    STR_COIL_DESP           strCoilDesp;         /* 地感线圈描述 */
    unsigned char           ucSimSnapFlag;       /* 模拟红灯抓拍标志 */
    

	/*Start of ITS on 2011-4-2 13:47 2.0.0.1 for 温州独立车道线圈检测模式切换功能 */
	char        	    	cDetectType;         /* 检测线圈类型 */
	char					cCoilSwitchFlag;	 /* 线圈检测模式切换使能标志 */
	char					cCoilSwitchSta;		 /* 线圈检测模式切换状态 */
	/*End of ITS on 2011-4-2 13:47 2.0.0.1*/
	

    /* begin add for 500-x version 2010-8-10 */
    STR_EP_VL_POINT			strVLPoint[4];		 			/* 虚拟线圈坐标位置定义 */
    STR_XCS_AREA			strTrafficLigntArea; 			/* 虚拟线圈下每车道对应的交通灯像素坐标 */
    STR_XCS_TRAFFIC_LIGHT   strTrafficLight[MAX_LIGHT_TYPE];/* 交通灯类型 */
    char                    cLaneDir;			 			/* 车道行驶方向定义 */
    char					cVLDispFlag;					/* 虚拟线圈是否显示标志 */
    char					cLaneLoopDispFlag;				/* 车道范围线圈是否显示标志 */
    /* end add for 500-x version 2010-8-10 */

    /*Start of ITS on 2011-4-19 9:25 2.0.0.1*/
    char					cCarNoDir;						/* 禁行方向属性定义 */
    /*End of ITS on 2011-4-19 9:25 2.0.0.1*/


    unsigned short 			usEPProVer; 					/* 电警车道信息协议小版本号 */

    /*Start of ITS on 2011-4-2 13:55 2.0.0.1*/
    unsigned short	    	usFlashLightType;     			/* 闪光灯类型 */
    /*End of ITS on 2011-4-2 13:55 2.0.0.1*/
    
    char					cRsv5[4];						/* 保留字节 */
    
	char				    cCarDistance;			 		/* 最后一张图片机动车离停车线距离（单位米） */
	char				    cFrameInterval;			 		/* 抓拍间隔帧数（范围1~5） */
	
	/*Start of ITS on 2011-4-2 13:55 2.0.0.1*/
	char					cRedLightDectSta;				/* 红灯检测模式状态查询 */
	/*End of ITS on 2011-4-2 13:55 2.0.0.1*/

	char					cInLightSta;					/* 补光方式状态查询 */
	char					cVLDecEn;						/* 虚拟线圈是否使能检测标志 */

	/*Start of ITS on 2011-4-19 9:26 2.0.0.1*/
	STR_EP_VL_POINT			strNoLaneVLPoint[2];		 	/* 不按车道行驶虚拟线坐标位置定义 */
	/*End of ITS on 2011-4-19 9:26 2.0.0.1*/

	/*Start of ITS on 2011-5-3 11:8 2.0.0.1*/
	char					cCarTrackEn;					/* 轨迹分析功能使能位 */
	/*End of ITS on 2011-5-3 11:8 2.0.0.1*/

	/*Start of ITS on 2011-6-25 17:5 2.0.0.1*/
	char					cCarSnapDir;					/* 是否判断车行驶方向 */
	/*End of ITS on 2011-6-25 17:5 2.0.0.1*/
	char					szLaneId[8];					/* 车道Id */

	char					cSwitchLaneEn;			 		/* 是否为可变车道 */
    char					cSwitchLaneLeftNo;		 		/* 可变车道左转方向灯号 */
    char					cSwitchLaneStraightNo;		 	/* 可变车道直行方向灯号 */
    char                    cRsv3[48];
}_PACKED_ STR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY, *PSTR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY;

/* 6.4 电子警察车道信息设置 */
typedef STR_ELECT_POLICE_CAR_CHANNEL_INFO_QUERY STR_ELECT_POLICE_CAR_CHANNEL_INFO_SET;

/* 6.5 电子警察红灯状态查询 */

typedef struct tagSTR_ELECT_POLICE_RED_L_STA_PC_QUERY
{ 
    unsigned int   uiCarRoadId;        /* 查询的车道号 */
}_PACKED_ STR_ELECT_POLICE_RED_L_STA_PC_QUERY, *PSTR_ELECT_POLICE_RED_L_STA_PC_QUERY;


typedef struct tagSTR_BAYONET_RED_L_STA_QUERY
{
    unsigned int    uiCarRoadId;
    unsigned char    ucRedSta;           /* 1:红灯 0:非红灯 */
    char            cRsv[3];
    STR_XCS_TRAFFIC_LIGHT   strTrafficLight[MAX_LIGHT_TYPE];/* 交通灯类型 */
    
    char 			cSwitchLaneLeftNo;	 		/* 可变车道左转方向灯号 */
    char 			cSwitchLaneLeftNoSta;		/* 可变车道左转方向灯状态 */

    char 			cSwitchLaneStraightNo; 		/* 可变车道直行方向灯号 */    
    char 			cSwitchLaneStraightNoSta;  	/* 可变车道直行方向灯状态 */
    char            cResv[108];
}_PACKED_ STR_ELEC_POLICE_RED_L_STA_QUERY, *PSTR_ELEC_POLICE_RED_L_STA_QUERY;

/* 6.6 模拟车辆行驶抓拍 */
typedef struct tagSTR_BAYONET_SIMU_RUN_SNAP
{
    unsigned int    uiCarRoadId;
    char	        cResv[128];
}_PACKED_ STR_ELECT_POLICE_SIMU_RUN_SNAP, *PSTR_ELECT_POLICE_SIMU_RUN_SNAP;

/* 6.7 强制红灯查询 */
typedef struct tagSTR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY
{ 
    unsigned int    uiCarRoadId;        /* 查询的车道号 */
}_PACKED_ STR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY, *PSTR_ELECT_POLICE_RED_L_ENFORCE_PC_QUERY;


typedef struct tagSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY
{
    unsigned int	uiCarRoadId;
    unsigned char	ucForceRedSta;      /* 1:强制红灯 0:非强制红灯 */
    char            cResv[131];
}_PACKED_ STR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY, *PSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY;

/* 6.8 强制红灯配置 */
typedef struct tagSTR_ELECT_POLICE_RED_LIGHT_ENFORCE_QUERY STR_ELECT_POLICE_RED_LIGHT_ENFORCE_SET;

/* 6.9 电子警察FTP信息查询 */
typedef struct tagSTR_ELECT_POLICE_FTP_INFO_QUERY
{
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char	        cResv1[18];
    char	        szUserName[32];
    char	        szUserPasswd[32];
    char	        cResv2[128];
}_PACKED_ STR_ELECT_POLICE_FTP_INFO_QUERY, *PSTR_ELECT_POLICE_FTP_INFO_QUERY;

/* 6.10 电子警察FTP信息设置 */
typedef STR_ELECT_POLICE_FTP_INFO_QUERY STR_ELECT_POLICE_FTP_INFO_SET;

/* 6.11 查询电警禁止行驶时间段信息 */
typedef struct tagSTR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY
{ 
    unsigned char    ucLaneId;        /* 查询的车道号 */
}_PACKED_ STR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY, *PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_PC_QUERY;

typedef struct tagSTR_FORBID_DIR_INFO
{
	char		cForbidDirType;
	char		cRange1Enable;
	char		cRange2Enable;
	char		cRsv1;
	int			iBeginTime1;
	int			iLastTime1; 	/* 持续时间1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* 持续时间2 */	
}STR_FORBID_DIR_INFO;

typedef struct tagSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY
{
	char				cLaneId;
	/*Start of ITS on 2011-4-19 10:12 2.0.0.1*/
	char				cRsv2[3];
	STR_FORBID_DIR_INFO strForbidDirInfo[4];
	/*End of ITS on 2011-4-19 10:12 2.0.0.1*/
	char        		cResv[128];
}_PACKED_ STR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY, *PSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY;

/* 6.12 设置电警禁止行驶时间段信息 */
typedef struct tagSTR_ELECT_POLICE_FORBIT_TIME_RANGE_QUERY STR_ELECT_POLICE_FORBIT_TIME_RANGE_SET;

/* 6.13 查询电警辅助红灯配置信息 */
typedef struct tagSTR_EP_ASSIST_RD_PC_QUERY
{ 
    unsigned char    ucLaneId;        /* 查询的车道号 */
}_PACKED_ STR_EP_ASSIST_RD_PC_QUERY, *PSTR_EP_ASSIST_RD_PC_QUERY;

typedef struct tagSTR_EP_ASSIST_RD_INFO
{
	char		cRedLightNum;
	char		cEnableFlag;
	char 		cRsv[2];
	int			iBeginTime1;
	int			iLastTime1; 	/* 持续时间1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* 持续时间2 */	
	int			iBeginTime3;
	int			iLastTime3; 	/* 持续时间3 */
}STR_EP_ASSIST_RD_INFO;

typedef struct tagSTR_EP_ASSIST_RD_QUERY
{
	char				cLaneId;
	char				cRsv2[3];
	STR_EP_ASSIST_RD_INFO strEpAssistRDInfo[4];
	char        		cResv[128];
}_PACKED_ STR_EP_ASSIST_RD_QUERY, *PSTR_EP_ASSIST_RD_QUERY;

/* 6.14 设置电警辅助红灯时间段信息 */
typedef struct tagSTR_EP_ASSIST_RD_QUERY STR_EP_ASSIST_RD_SET;

/* 6.15 查询电警违章类型描述及代码 */
/* 电警车牌识别配置信息 */
typedef struct tagSTR_EP_LPR_CONF_INFO_QUERY
{
	unsigned char ucLPREn;					/* 车牌识别使能标志 */
	unsigned char ucLPRCConfidence;			/* 车牌首字符置信度 */
	char    szLPRFirstChar[6];				/* 车牌首字符 */
	unsigned short usLPRWidth;				/* 车牌识别宽度 */
	unsigned short usLPRHeight;				/* 车牌识别高度 */

	char           cResv[128];
}EP_LPR_CONF_INFO_QUERY,*PEP_LPR_CONF_INFO_QUERY;

/* 6.16 配置电警违章类型描述及代码 */
typedef struct tagSTR_EP_LPR_CONF_INFO_QUERY EP_LPR_CONF_INFO_SET;

/* 6.17 查询电警违章类型描述及代码 */
typedef struct tagSTR_EP_IRREG_CONF_INFO_QUERY
{
	char 			cDispFlag;			/* 违章名称是否叠加到图片上 */

	char			szRunRedCode[8];	/* 闯红灯违章代码 */
	char			szRunRedName[64];	/* 闯红灯违章名称 */
	
	char			szReverseRunCode[8];/* 逆行违章代码 */
	char			szReverseRunName[64];/* 逆行违章名称 */

	char			szInfringForbidLineCode[8]; /* 违反禁止标线行驶违章代码 */
	char			szInfringForbidLineName[64];/* 违反禁止标线行驶违章名称 */

	char			szInfringDirLaneRunCode[8]; /* 不按导向车道行驶违章代码 */
	char			szInfringDirLaneRunName[64];/* 不按导向车道行驶违章名称 */

	char			szStopOnTrafficNoEntryCode[8]; /* 路口遇堵禁止进入违章代码 */
	char			szStopOnTrafficNoEntryName[64];/* 路口遇堵禁止进入违章描述 */

	char			szHighwayInfringDirLaneCode[8]; /* 高速路不按导向车道行驶违章代码 */
	char			szHighwayInfringDirLaneName[64];/* 高速路不按导向车道行驶违章名称 */

	char			szStoppedVehicleCode[8]; 		/* 违章停车违章代码 */
	char			szStoppedVehicleName[64];		/* 违章停车违章名称 */
	
	char			cRsv[40];
}EP_IRREG_CONF_INFO_QUERY,*PEP_IRREG_CONF_INFO_QUERY;

/* 6.18 配置电警违章类型描述及代码 */
typedef struct tagSTR_EP_IRREG_CONF_INFO_QUERY EP_IRREG_CONF_INFO_SET;

/* 6.19 查询电警遇堵禁入配置信息 */
typedef struct tagSTR_EP_SOT_NO_ENTRY_PC_QUERY
{ 
    unsigned char    ucLaneId;        /* 查询的车道号 */
}_PACKED_ STR_EP_SOT_NO_ENTRY_PC_QUERY, *PSTR_EP_SOT_NO_ENTRY_PC_QUERY;

typedef struct tagSTR_EP_SOT_NO_ENTRY_INFO
{
	char		cEnableFlag;
	char 		cRsv[3];
	int			iBeginTime1;
	int			iLastTime1; 	/* 持续时间1 */
	int			iBeginTime2;
	int			iLastTime2; 	/* 持续时间2 */	
	int			iBeginTime3;
	int			iLastTime3; 	/* 持续时间3 */
}STR_EP_SOT_NO_ENTRY_INFO;

typedef struct tagSTR_EP_SOT_NOENTRY_QUERY
{
	char				cLaneId;
	char				cRsv2[3];
	STR_EP_SOT_NO_ENTRY_INFO strEpSOTNoEntryInfo;
	char        		cResv[128];
}_PACKED_ STR_EP_SOT_NOENTRY_QUERY, *PSTR_EP_SOT_NOENTRY_QUERY;

/* 6.20 设置电警遇堵禁入配置信息 */
typedef struct tagSTR_EP_SOT_NOENTRY_QUERY STR_EP_SOT_NOENTRY_SET;

/* 6.21 电警图片OSD字幕叠加信息查询 */
typedef struct tagSTR_EP_JPG_OSD_CONF_QUERY
{
    unsigned char   ucDevSnOsdEn;		/* 设备编号 */
	unsigned char	ucRoadNameOsdEn;	/* 路口名 */
	unsigned char	ucDirNameOsdEn;		/* 方向 */
	unsigned char	ucLaneNameOsdEn;	/* 车道名 */

	unsigned char	ucSnapTimeOsdEn;	/* 抓拍时间 */
	unsigned char	ucBusNumOsdEn;		/* 车牌号码 */
	unsigned char	ucLPColorOsdEn;		/* 车牌颜色 */
	unsigned char	ucCarTypeOsdEn;		/* 车型 */

	unsigned char	ucCarColorOsdEn;	/* 车身颜色 */
	unsigned char	ucReguCodeOsdEn;	/* 违章代码 */
	unsigned char	ucStandReguDesOsdEn;/* 标准违章描述 */
	unsigned char	ucIntelReguDesOsdEn;/* 内部违章描述 */

	unsigned char	ucRDBeginTimeOsdEn; /* 红灯开始时间 */
	unsigned char	ucRDCycleOsdEn;		/* 红灯周期 */
	unsigned char	ucRoadDespOsdEn;	/* 路口描述 */
	char            cRsv[125];

}_PACKED_ STR_EP_JPG_OSD_CONF_QUERY, *PSTR_EP_JPG_OSD_CONF_QUERY;

/* 6.22 电警图片OSD字幕叠加信息配置 */
typedef STR_EP_JPG_OSD_CONF_QUERY STR_EP_JPG_OSD_CONF_SET;

/* 6.23 备份电警FTP信息查询 */
typedef struct tagSTR_EP_BACKUPS_FTP_INFO_QUERY
{
    unsigned int    uiEnable;
    unsigned int    uiFtpIP;
    unsigned short  usFtpPort;
    char            cResv1[18];
    char            szUserName[32];
    char            szUserPasswd[32];
    char            cResv2[128];
}_PACKED_ STR_EP_BACKUPS_FTP_INFO_QUERY, *PSTR_EP_BACKUPS_FTP_INFO_QUERY;

/* 6.24 备份电警FTP信息设置 */
typedef STR_EP_BACKUPS_FTP_INFO_QUERY STR_EP_BACKUPS_FTP_INFO_SET;

/* 6.25 查询电警新增违章类型描述及代码 */
typedef struct tagSTR_EP_NEWADD_IRREG_CONF_INFO_QUERY
{
	char			szRunInNoLaneCode[8]; 		/* 不在机动车道内行驶违章代码 */
	char			szRunInNoLaneName[64];		/* 不在机动车道内行驶违章名称 */

	char			cRsv[512];
}EP_NEWADD_IRREG_CONF_INFO_QUERY,*PEP_NEWADD_IRREG_CONF_INFO_QUERY;

/* 6.26 配置电警新增违章类型描述及代码 */
typedef struct tagSTR_EP_NEWADD_IRREG_CONF_INFO_QUERY EP_NEWADD_IRREG_CONF_INFO_SET;

/* 7. record protocol */

/* 7.1 磁盘信息查询 */
typedef struct tagSTR_RECORD_DISK_INFO_QUERY
{
    unsigned int	uiDiskNum;      /* 磁盘分区编号 */
    unsigned int	uiCap;          /* 磁盘容量 */
    unsigned int	uiUsedCap;
    char            cResv[128];
}_PACKED_ STR_RECORD_DISK_INFO_QUERY, *PSTR_RECORD_DISK_INFO_QUERY;

/* 7.2 保留 */

/* 7.3 录像下载、回放方式查询 */
typedef struct tagSTR_RTSP_CONF_QUERY
{
	char	cRtspMode;
	char	cRsv1[3];
	char	szRtspIP[16];
	char	szRtspPath[32];
	short	sRtspPort;
	short	sHttpPort;
	char	szUserName[20];
	char	szPassword[20];
	char	cRsv2[128];
}_PACKED_ STR_RTSP_CONF_QUERY,*LPSTR_RTSP_CONF_QUERY;

/* 7.4 录像下载回放方式设置 */
typedef struct tagSTR_RTSP_CONF_QUERY STR_RTSP_CONF_SET;

/* 7.5 周期性录像计划查询 */
typedef struct tagSTR_RECORD_PERIOD_PLAN_PC_QUERY
{
    unsigned short    usChannelId;
}_PACKED_ STR_RECORD_PERIOD_PLAN_PC_QUERY, *PSTR_RECORD_PERIOD_PLAN_PC_QUERY;

typedef struct tagSTR_RECORD_PERIOD_PLAN_QUERY
{
    unsigned short	usVChannel;      
    char	        cWeekDay;       /* 表明哪个工作日， 
                                        =1，表明星期一；
                                        =2，表明星期二；
                                        …,
                                        =7，星期日
                                        =0，表示每天 */
    char	        cBeginHour;
    char	        cBeginMinute;
    char	        cEndHour;
    char	        cEndMinute;
    char	        cResv[17];
}_PACKED_ STR_RECORD_PERIOD_PLAN_QUERY, *PSTR_RECORD_PERIOD_PLAN_QUERY;

/* 7.6 周期性录像计划增加 */
typedef STR_RECORD_PERIOD_PLAN_PC_QUERY STR_RECORD_PERIOD_PLAN_ADD;

/* 7.7 周期性录像计划删除 */
typedef STR_RECORD_PERIOD_PLAN_PC_QUERY STR_RECORD_PERIOD_PLAN_DEL;

/* 7.8 周期性录像计划更新 */
typedef STR_RECORD_PERIOD_PLAN_PC_QUERY STR_RECORD_PERIOD_PLAN_UPDATE;

/* 7.9 通道录像参数查询 */
/* 当存储模式为ESMAuto，ESMStop，ESMLoop，ESMCustom时，返回应答数据 */
typedef struct tagSTR_RECORD_CHANNEL_PARAM_QUERY
{
    unsigned int	uiVChannel;
    unsigned int	uiRecDays;              /* 配额录像保存天数 */
    unsigned int	uiPreRecTime;           /* 预录时间，单位秒 */
    unsigned int	uiDiskPolicy;           /* 存储策略 */
    char            acDiskIndex[16];
}_PACKED_ STR_RECORD_CHANNEL_PARAM_QUERY, *PSTR_RECORD_CHANNEL_PARAM_QUERY;

/* 当存储模式为ESMSpace时，返回应答数据 */
typedef struct tagSTR_RECORD_CHANNEL_PARAM_SPACE_QUERY
{
    unsigned int	uiVChannel;
    unsigned short	usRecDays;              /* 配额录像保存天数 */
    unsigned short	usRecSpace;             /* 配额录像容量 */
    unsigned int	uiPreRecTime;           /* 预录时间，单位秒 */
    unsigned int	uiDiskPolicy;           /* 存储策略 */
    char            acDiskIndex[16];
}_PACKED_ STR_RECORD_CHANNEL_PARAM_SPACE_QUERY, *PSTR_RECORD_CHANNEL_PARAM_SPACE_QUERY;


/* 7.10 通道录像参数配置 */
typedef struct tagSTR_RECORD_CHANNEL_PARAM_SET
{
    unsigned int    uiDiskParamNum;
    void            *pVoid;
}_PACKED_ STR_RECORD_CHANNEL_PARAM_SET, *PSTR_RECORD_CHANNEL_PARAM_SET;

/* 7.11 检索录像记录 */
typedef struct tagSTR_RECORD_PC_SEARCH
{
    unsigned int	uiRecordType;
    unsigned int	uiBeginTime;
    unsigned int	uiEndTime;
    unsigned short	usVideoNum; 
    short   		nDigestType;        //预览摘要的类型，如果为0表示不启用摘要，1为JPEG
    char        	nReserved[4];       //预留
}_PACKED_ STR_RECORD_PC_SEARCH, *PSTR_RECORD_PC_SEARCH;

typedef struct tagSTR_QUERY_INDEX_HEAD
{
    unsigned int	uiTotalIndexNum;      /* 符合条件的总索引条数 */
    unsigned int	uiCurIndexNum;        /* 当前包符合条件的索引数 */
    unsigned int	uiIndexStrLen;        /* 索引结构长度 */
    unsigned int   nDigestStructLen;  //单个摘要结构的长度，如果为0表示不定长
}_PACKED_ STR_QUERY_INDEX_HEAD, *PSTR_QUERY_INDEX_HEAD;

typedef struct tagSTR_QUERY_INDEX
{
    unsigned int	uiRecordType;
    unsigned int	uiStartTime;
    unsigned int	uiEndTime;
    unsigned short	usVideoNum;
    char	        cResv1[2];
    unsigned int	uiFileSize;
	unsigned int    nFileOffset;		//事件在文件中的偏移位置
    char	        sFileName[128];
}_PACKED_ STR_QUERY_INDEX, *PSTR_QUERY_INDEX;

typedef struct
{
    short   nDigestSize;
    short   nDigestType; //预览摘要的类型，如果为0表示不启用摘要，1为JPEG
    char 	data[164]; //摘要数据，长度由nDigestSize指定
}_PACKED_ TRecDigest,*LPTRecDigest ;

typedef struct tagSTR_RECORD_SEARCH
{
    STR_QUERY_INDEX_HEAD strQueryIndexHead;
    STR_QUERY_INDEX      *pStrIndex;
}_PACKED_ STR_RECORD_SEARCH, *PSTR_RECORD_SEARCH;

/* 7.12 录像文件回放 */
typedef struct tagSTR_RECORD_PC_PLAYBACK
{
    char	cFileName[128];
    char	cResv[128];
}_PACKED_ STR_RECORD_PC_PLAYBACK, *PSTR_RECORD_PC_PLAYBACK;

typedef struct tagSTR_RECORD_PLAYBACK
{
    char	cFileUrl[256];
    char	cResv[128];
}_PACKED_ STR_RECORD_PLAYBACK, *PSTR_RECORD_PLAYBACK;

/* 7.13 录像文件下载 */
typedef struct tagSTR_RECORD_PC_FILE_DOWNLOAD
{
    char	cFileName[128];
    char	cResv[128];
}_PACKED_ STR_RECORD_PC_FILE_DOWNLOAD, *PSTR_RECORD_PC_FILE_DOWNLOAD;

typedef struct tagSTR_RECORD_FILE_DOWNLOAD
{
    char	cFileUrl[256];
    char	cResv[128];
}_PACKED_ STR_RECORD_FILE_DOWNLOAD, *PSTR_RECORD_FILE_DOWNLOAD;

/* 8. CAMARA info protocol */

/* 8.1 高清摄像机属性查询 */
typedef struct tagSTR_CAMARA_ATTRIB_PC_QUERY
{
    unsigned int   uiHdCameraId;
}_PACKED_ STR_CAMARA_ATTRIB_PC_QUERY, *PSTR_CAMARA_ATTRIB_PC_QUERY;

typedef struct tagSTR_CAMARA_ATTRIB_QUERY
{
    unsigned int	uiHdCameraId;
    unsigned int	uiWidth;
    unsigned int	uiHeight;
    unsigned int	uiPixels;
    char	        cResv[128];
}_PACKED_ STR_CAMARA_ATTRIB_QUERY, *PSTR_CAMARA_ATTRIB_QUERY;


/* 8.2 摄像机配置参数范围查询 */
typedef struct tagSTR_CAMARA_CTL
{
    char	cCamaraType;
    char	cResv1[2];
    char	cParamIndex;
    char 	cCtlType;
    char	cResv2[3];
    float	fMinVal;              /* 固定值 / 最小值 */
    float	fMaxVal;              /* 最大值 */
}_PACKED_ STR_CAMARA_CTL, *PSTR_CAMARA_CTL;

typedef struct tagSTR_CAMARA_CTL_PARAM_QUERY
{
    char	        cCtrlNum;
    char	        cResv[3];
    STR_CAMARA_CTL  pstrCamaraCtl[10];
}_PACKED_ STR_CAMARA_CTL_PARAM_QUERY, *PSTR_CAMARA_CTL_PARAM_QUERY;

/* 8.3 摄像机状态参数查询 */
typedef struct tagSTR_CAMARA_STA_INFO_QUERY
{
    float	fShutter;
    float	fGain;
    float	fContrast;
    float	fSaturation;
    float	fBright;
    float	fGamma;
    float	fExposure;
    /* add for xcc200 */
    float   fAperture;
    float	fIso;
    float	fZoom;
    float   fFocus;
    float   fAWB;
    float   fSharpness;
    char	cResv[104];
}_PACKED_ STR_CAMARA_STA_INFO_QUERY, *PSTR_CAMARA_STA_INFO_QUERY;


/* 8.5 摄像机控制内容查询 */

typedef struct tagSTR_CAMARA_CTL_INFO_PC_QUERY
{
    char	cParamIndex;
}_PACKED_ CAMARA_CTL_INFO_PC_QUERY, *PCAMARA_CTL_INFO_PC_QUERY;

typedef struct tagSTR_CAMARA_CTL_INFO_QUERY
{
    char	        cCtrlNum;
    char	        cResv[2];
    char			cParamIndex;
    STR_CAMARA_CTL  pstrCamaraCtl[MAX_CAM_CTRL_NUM];
}_PACKED_ STR_CAMARA_CTL_INFO_QUERY, *PSTR_CAMARA_CTL_INFO_QUERY;

/* 8.6 摄像机控制内容设置 */
typedef struct tagSTR_CAMARA_CTL_INFO_QUERY  STR_CAMARA_CTL_INFO_SET;

/* 8.7 摄像机时间段查询 */
typedef struct tagSTR_CAMARA_TIME_RANGE_PC_QUERY
{
    char	cParamIndex;
}_PACKED_ CAMARA_TIME_RANGE_PC_QUERY, *PCAMARA_TIME_RANGE_PC_QUERY;

typedef struct tagSTR_XCS_CAMERA_TIME_RANGE
{
	char		cGroupId;
	char		cEnable;
	char		szSceneName[30];
	int			iBeginTime;
	int			iLastTime;
	char		cIrisPos;
	char 		cRsv[127];
}XCS_CAMERA_TIME_RANGE_INFO_QUERY, *PXCS_CAMERA_TIME_RANGE_INFO_QUERY;

/* 8.8 摄像机时间段设置 */
typedef struct tagSTR_XCS_CAMERA_TIME_RANGE  XCS_CAMERA_TIME_RANGE_INFO_SET;


/* 9. net info protocol */

/* 9.1 网络信息查询 */
typedef struct tagSTR_NET_INFO_QUERY
{
    unsigned int	uiIPAddr;
    unsigned int	uiNetMask;
    unsigned int	uiGateWay;
    char	        cResv[128];
}_PACKED_ STR_NET_INFO_QUERY, *PSTR_NET_INFO_QUERY;

/* 9.2 网络信息配置 */
typedef STR_NET_INFO_QUERY STR_NET_INFO_SET;

/* 9.3 图片网络存储信息查询 */
typedef struct tagSTR_PIC_NET_SAVE_INFO_QUERY
{
    unsigned int	uiEnable;           /* 是否启用网络存储 */
    unsigned int	uiStorageType;      /* 存储类型，NFS，SAMBA */
    unsigned int	uiIpAddr;
    char	        szFolderName[32];
    char	        szUserName[32];
    char	        szPassWord[32];
    char	        cResv[128];
}_PACKED_ STR_PIC_NET_SAVE_INFO_QUERY, *PSTR_PIC_NET_SAVE_INFO_QUERY;

/* 9.4 图片网络硬盘信息配置 */
typedef STR_PIC_NET_SAVE_INFO_QUERY STR_PIC_NET_SAVE_INFO_SET;

/* 9.5 录像网络存储信息查询 */
typedef STR_PIC_NET_SAVE_INFO_QUERY STR_RECORD_NET_SAVE_QUERY;

/* 9.6 录像网络硬盘信息配置 */
typedef STR_PIC_NET_SAVE_INFO_QUERY STR_RECORD_NET_SAVE_SET;

/* 9.7 云存储配置信息查询 */
typedef struct tagSTR_NET_REC_MANAGE_QUERY
{
	char			cEnable;
	char			cRsv[3];
    unsigned int	uiMainNRMIp;
    unsigned int	uiBakNRMIp;
    unsigned short	usMainNRMPort;
    unsigned short	usBakNRMPort;
	char			szPuID[32];
    char	        cResv[128];
}_PACKED_ STR_NET_REC_MANAGE_QUERY, *PSTR_NET_REC_MANAGE_QUERY;

/* 9.8 云存储配置信息设置 */
typedef STR_NET_REC_MANAGE_QUERY STR_NET_REC_MANAGE_SET;

/* 10. dev connect protocol */

/* 10.1 330平台接入信息查询 */
typedef struct tagSTR_330_CON_QUERY
{
    char cVer;                       /* 版本号 */
    char cEnable;                    /* 是否接入 */
    char cLaneSeq1;                  /* 车道序号 */
    char cLaneSeq2;
    char cLaneSeq3;
    char cLaneSeq4;
    char szResv[2];
    char sz330DevId[128];            /* 330设备ID */
    char sz330DirId[16];             /* 330方向ID */
    char sz330Key[256];              /* 330密钥 */
}_PACKED_ STR_330_CON_QUERY, *PSTR_330_CON_QUERY;

/* 10.2 网络信息配置 */
typedef struct tagSTR_330_CON_QUERY STR_330_CON_SET;

/* 10.3 v3平台接入信息查询 */
typedef struct tagSTR_V3_CON_QUERY
{
    unsigned int    uiV3SerIp;
    unsigned short  usV3Port;
    unsigned char   ucRev[2];
    char            szDevNo[20];
    char            szDevPwd[16];
    char            cV3ServerSeq;
    char            cV3TimeSynFlag;
    char            cV3Enable;
    char            cVersion;
}_PACKED_ STR_V3_CON_QUERY, *PSTR_V3_CON_QUERY;

/* 10.4 v3平台接入信息配置 */
typedef struct tagSTR_V3_CON_QUERY STR_V3_CON_SET;

typedef struct tagSmallProtoVer
{
	unsigned short usPPChanelVer;
	unsigned short usEPGlobalVer;
	unsigned short usEPChanelVer;
	unsigned short usRsv;
}_PACKED_ STR_SMALL_PRO_VER;


/* 10.5 电警330平台接入信息查询 */
typedef struct tagSTR_330_CON_QUERY STR_EP_330_CON_QUERY;

/* 10.6 网络信息配置 */
typedef struct tagSTR_330_CON_QUERY STR_EP_330_CON_SET;

/* 10.7 Gb28181平台接入信息查询 */
typedef struct tagSTR_GB28181_CON_QUERY
{
	char			szServerId[32];			/* 服务器id */
	char			szServerDomain[32];		/* 服务器域 */
	char			szServerIp[32];			/* 服务器ip */
    unsigned int    uiServerPort;			/* 服务器port */
    unsigned int	uiRegisterExpiry;		/* 注册有效时长 */
    unsigned int    uiKeepLiveTime;			/* 心跳周期 */
    unsigned int    uiConMode;				/* 连接方式: 0:udp 1:tcp */
    char            szClientId[32];
    char            szClientUserName[32];
    char            szClientPasswd[32];

    char			cEnFlag;				/* 启用标志 */
    char			cRsv[127];
}_PACKED_ STR_GB28181_CON_QUERY, *PSTR_GB28181_CON_QUERY;

/* 10.8 Gb28181平台接入信息配置 */
typedef struct tagSTR_GB28181_CON_QUERY STR_GB28181_CON_SET;

/* 12. net manage info proto */

/* 网管信息查询 */
typedef struct tagStrNetManageQuery
{
    unsigned int   uiDataLen;       /* 字符长度 */
    char           *pcData;			/* 字符内容 */
}_PACKED_ STR_NET_MANAGE_QUERY, *PSTR_NET_MANAGE_QUERY;

/***********************************************************************************/
/* some default parameter */
/***********************************************************************************/
#ifndef XCS_NT_PACK_HEAD_LENGTH
#define  NT_PACK_HEAD_LENGTH		16
#endif

/* 返回错误码宏定义 */
#define     ERROR_TIME_OUT        0xFF /* 超时 */
#define     ERROR_WRITE_CONF      0xFE /* 写配置失败 */
#define     ERROR_READ_CONF       0xFD /* 读配置失败 */
#define     ERROR_FUNC_CODE       0xFC /* 功能码错 */
#define     ERROR_PSW_NAME        0x01

/* 协议小版本号宏定义 */
#define XCS_PRO_INVALID_VER0 0x0
#define XCS_PRO_INVALID_VER1 0xFFFF

/* 卡口车道协议小版本号 */
#define XCS_PRO_PP_VER		0x1001
#define XCS_PRO_PP_VER_2	0x1002

/* 电警车道协议小版本号 */
#define XCS_PRO_EP_VER		0x1001
#define XCS_PRO_EP_VER_2	0x1002

/* 电警全局协议小版本号 */
#define XCS_PRO_EPG_VER		0x1001
#define XCS_PRO_EPG_VER_2	0x1002

#endif /* _PROTOCOL_XCS__H__ */

