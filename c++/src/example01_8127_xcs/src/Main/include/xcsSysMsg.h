/******************************************************************************
 * xcsSysMsg.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.16, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef XCS_SYS_MSGQ_H
#define XCS_SYS_MSGQ_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


/* 各模块发送消息中的模块ID宏定义 */
#define SYS_MSG_DCTL_MODULE_ID              1       /* 外设控制模块ID */
#define SYS_MSG_NTP_MODULE_ID               2       /* NTP对时模块ID */
#define SYS_MSG_DSP_MODULE_ID               3       /* DSP模块ID */
#define SYS_MSG_PROTOCOL_MODULE_ID          4       /* PROTOCOL,XCS协议模块ID */
#define SYS_MSG_NETFS_MODULE_ID             5       /* 网络文件系统检测 */
#define SYS_MSG_DEBUG_SHELL_MODULE_ID       6       /* dbg调试模块ID */

#define TRAFFIC_LIGHT_MANA_MODULE_ID		7		/* 交通灯管理模块 */

/* DSP模块向主线程通信消息类型宏定义 */

/* 抓拍参数设置消息 */
#define  MSG_VCHANN_SNAP_PARAM     0x00
/* 视频参数配置消息 */
#define  MSG_VIDEO_PARAM_SET       0x01
/* 数据上报消息 */
#define  MSG_VCHANN_DATA_REPORT    0x02

/* 采集设备属性配置消息 */
#define  MSG_DEV_CTL_CAP           0x03
#define  MSG_DEV_CTL_RED_LIHGT     0x04

/* 系统时间设置消息 */
#define  MSG_SYS_TIME_SET          0x09
/* 远程升级消息 */
#define  MSG_REMOTE_UPDATE         0x0a
/* 远程重启消息 */
#define  MSG_REMOTE_REBOOT         0x0b
/* NTP服务器设置消息 */
#define  MSG_NTP_SERVER_SET        0x0c

/* 远程恢复设备出厂设置 */
#define  MSG_REMOTE_RESTORE_PARAM  0x0d


/* 卡口全局信息配置 */
#define MSG_BAYONET_GLOBAL_INFO_SET         0x11
/* 卡口车道信息配置 */
#define MSG_BAYONET_CAR_CHANNEL_INFO_SET    0x12
/* 卡口虚拟线圈高级信息设置 */
#define MSG_BAYONET_HIGH_VL_INFO_SET        0x13
/* 卡口手动抓拍 */
#define MSG_BAYONET_MANUAL_SNAP             0x14
/* 卡口FTP信息设置 */
#define MSG_BAYONET_FTP_INFO_SET            0x15

/* 电子警察全局信息配置 */
#define MSG_EP_GLOBAL_INFO_SET              0x21
/* 电子警察车道信息设置 */
#define MSG_EP_CAR_CHANNEL_INFO_SET         0x22
/* 模拟电子警察手动抓拍 */
#define MSG_ELECT_POLICE_SIMU_RUN_SNAP      0x23
/* 强制红灯配置 */
#define MSG_EP_RED_LIGHT_ENFORCE_SET        0x24
/* 电子警察FTP信息设置 */
#define MSG_EP_FTP_INFO_SET                 0x25
/* 电子警察车道禁止行驶时间段设置 */
#define MSG_EP_TIME_RANGE_INFO_SET          0x26

/* 电子警察车道辅助红灯设置 */
#define MSG_EP_ASSIST_RED_LIGHT_SET         0x27

/* 电子警察车牌识别设置 */
#define MSG_EP_LPR_INFO_SET			        0x28

/* 电子警察违章类型描述及代码设置 */
#define MSG_EP_IRREG_INFO_SET			    0x29

/* 电子警察遇堵禁入设置 */
#define MSG_EP_SOT_NO_ENTRY_SET		        0x2a

/* 电警图片osd叠加设置 */
#define MSG_EP_JPG_OSD_SET		        	0x2b

/* 卡口图片osd叠加设置 */
#define MSG_PP_JPG_OSD_SET		        	0x2c

/* 卡口违章类型描述及代码设置 */
#define MSG_PP_IRREG_INFO_SET			    0x2d

/* 录像下载回放方式设置 */
#define MSG_RECORD_DOWNLOAD_PLAYTYPE_SET    0x31
/* 周期性录像计划增加 */
#define MSG_RECORD_PERIOD_PLAN_ADD          0x32
/* 周期性录像计划删除 */
#define MSG_RECORD_PERIOD_PLAN_DEL          0x33
/* 周期性录像计划更新 */
#define MSG_RECORD_PERIOD_PLAN_UPDATE       0x34
/* 通道录像参数配置 */
#define MSG_RECORD_CHANNEL_PARAM_SET        0x35

/* 摄像机时间段设置 */
#define MSG_CAMARA_TIME_RANGE_SET           0x40
/* 摄像机控制内容设置 */
#define MSG_CAMARA_CTL_INFO_SET             0x41
/* 网络信息配置 */
#define MSG_NET_INFO_SET                    0x42
/* 图片网络硬盘信息配置 */
#define MSG_NET_PIC_NET_SAVE_INFO_SET       0x43
/* 录像网络硬盘信息配置 */
#define MSG_NET_RECORD_NET_SAVE_INFO_SET    0x44
/* 数据上报参数设置*/
#define MSG_DATA_REPORT_PARAM_SET           0x45
/* NTP对时报告*/
#define MSG_NTP_TIME_REPORT                 0x46

/* 330平台接入信息设置 */
#define MSG_330_DEV_CONNECT_INFO_SET        0x51

/* 电警330平台接入信息设置 */
#define MSG_EP_330_DEV_CONNECT_INFO_SET     0x52

/* 云存储信息设置变动 */
#define MSG_NRM_INFO_SET     				0x53


/* 原始数据序列采集功能消息 */
#define MSG_ORG_IMAG_INFO_SET               0x61

/* 交通灯管理模块消息 */
#define MSG_TRAFFIC_LIGHT_INFO_SET          0x71

/* 交通灯检测模式发生变化 */
#define MSG_TRAFFIC_LIGHT_MODE_SET          0x72

/* XCS客户端异常退出 */
#define MSG_XCS_EXCEPT_DISCONNECT           0xa0

/* 模拟电子警察手动抓拍 */
#define MSG_UART_SIMU_RUN_SNAP      0x73

/* 系统消息队列长度 */
#define SYS_MSG_MAX_DATA_LEN  256
#define _PACKED_ __attribute__((packed))


typedef struct tagsysRecMsgStr
{
	unsigned int    uiModuleId;                     /* 发送消息的模块ID */
	unsigned int    uiMsgType;                      /* 消息类型 */
    unsigned char   ucChannelId;                    /* 通道号 */
    unsigned char   ucCarRoadId;                    /* 车道号 */
	unsigned char   ucRsv[2];
	unsigned char   aucData[SYS_MSG_MAX_DATA_LEN] __attribute__((aligned(32)));  /* 消息内容，256字节 */
}_PACKED_ STR_SYS_APP_MSG;

/* 卡口手动抓拍消息结构 */

/* 客户端异常退出消息结构 */
typedef struct tagClientDisconExcept
{
    unsigned int    uiIp;
    unsigned int    uiSessionId;
    unsigned short  usPort;
    unsigned short  usDataUploadPort;   
    unsigned int    uiDataUploadIp;
}_PACKED_ STR_EXCEPT_DISCON_CLIENT;

#endif /* XCS_SYS_MSGQ_H */
