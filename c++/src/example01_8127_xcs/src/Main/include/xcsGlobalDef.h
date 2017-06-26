#ifndef _XCS_GLOBAL_DEF_H__
#define _XCS_GLOBAL_DEF_H__

#include "xcsCommDef.h"

/* 指定默认配置客户端版本 */
#define FIX_DEC_CONF_VER (0x02010203)

/* proto type define */
#define XCS_PROTO_TYPE 1
#define DVS_PROTO_TYPE 2

/*device type*/
#define DVS_DEVICE_TYPE_NAME			"XCS"
#define DVS_DEVICE_TYPE_ID			    (0x01)
#define DVS_DEVICE_HARDWARE_VER		    (0x1000000)
#define DVS_PRODUCT_TYPE				(0x00000000)



/*device ability relatively define*/
#define DSP_NUM						(1)
#define	VCHAN_PER_DSP				(1)
#define	ACHAN_PER_DSP				(4)
#define	VCAP_PER_DSP				(4)
#define	TOTAL_CHANNEL_NUM			(DSP_NUM*VCHAN_PER_DSP)

/*encode relatively define*/
#define	DEFAULT_ENC_TYPE			(VTYPE_H264)

#if defined(CYC500XKW4C)||defined(CYC500XAW)
#define	DEFAULT_ENC_BITRATE			(2048)		//kbps
#define	DEFAULT_ENC_FORMAT			(VFMT_1080P)
#else
#define	DEFAULT_ENC_BITRATE			(1536)		//kbps
#define	DEFAULT_ENC_FORMAT			(VFMT_720P)
#endif

#if defined(CYC500KKW2C)
#define	DEFAULT_ENC_FRAMERATE		(17)
#elif defined(CYC200XKW)||defined(CYC200MK)||defined(CYC200K2)||defined(CYC200K3)
#define	DEFAULT_ENC_FRAMERATE		(17)
#elif defined(CYC800XKW4C)
#define	DEFAULT_ENC_FRAMERATE		(10)
#elif defined(CYC500XKW4C)||defined(CYC500XAW)
#define	DEFAULT_ENC_FRAMERATE		(25)
#else
#define	DEFAULT_ENC_FRAMERATE		(9)
#endif

#define	DEFAULT_ENC_IFRAMEINTV		(75)
#define DEFAULT_AUDIO_BIND			(1)
#define DEFAULT_ENC_CBRVBR			(SET_CBR)
#define DEFAULT_IMAGE_QUALITY		(2)
#define DEFAULT_TIME_X				(44)
#define DEFAULT_TIME_Y				(32)
#define DEFAULT_ENV_OSD_X			(48)
#define DEFAULT_ENV_OSD_Y			(68)
#define DEFAULT_ENV_LINE2LINE		(32)
#define DEFAULT_CAPTION_X			(44)
#define DEFAULT_CAPTION_Y			(400)
#define DEFAULT_TIME_ENABLE			(1)
#define DEFAULT_WEEK_ENABLE			(0)


#define	DEFAULT_AENC_TYPE				(ATYPE_G711A)

/*image relatively define*/
#define	DEFAULT_IMAGE_BRIGHTNESS		(50)
#define	DEFAULT_IMAGE_HUE				(50)
#define	DEFAULT_IMAGE_SATURATION		(50)
#define	DEFAULT_IMAGE_CONTRAST			(50)

#define MAX_SECTION_NUM   (2)

/*record relatively define*/
#define DEFAULT_DISK_COUNT					(2)
#define DEFAULT_PARTITION_COUNT_PER_DISK	(12)
#define DEFAULT_PARTITION_COUNT				(DEFAULT_DISK_COUNT*DEFAULT_PARTITION_COUNT_PER_DISK)
#define DEFAULT_REC_MODE					(REC_PLAN)
#define DEFAULT_STORAGE_MODE				(STORAGE_FULL_OVERWRITE)
#define DEFAULT_STORAGE_REC_DAYS			(10) /*修改默认录像10天*/
#define DFFAULT_PRE_RECORD_SECOND			(3)
#define DEFAULT_REC_SECOND_PER_FILE			(10*60)
#define DEFAULT_REC_DISK_PER_CHANNEL		(12)
#define DEFAULT_MAX_PATH_LEN			(128)
#define DEFAULT_REC_FILE_SIZE			(256*1024*1024)
#define MIN_PARTITION_SPACE			    (1024) /*MB*/
#define DEFAULT_REC_FORMAT				(REC_FORMAT_ASF)
#define DEFAULT_PLANT_TIME_SEGMENT	(6)

/*protocol relatively define*/

/* virtual loop info */

#define	MAX_XCS_CLIENT_NUM		            (2)
#define MAX_XCS_USER_COUNT		            (16)

#if defined(CYC500XZD)||defined(CYC200XZD)
#define MAX_CAM_GROUP_NUM                   (4)     /**摄像机参数最大组数**/ 
#define MAX_CAM_CTRL_NUM                    (8)     /**摄像机控制最大项**/
#else
#define MAX_CAM_GROUP_NUM                   (4)     /**摄像机参数最大组数**/ 
#define MAX_CAM_CTRL_NUM                    (12)    /**摄像机控制最大项**/
#endif

#define MAX_VEHICLE_NUM                     (3)     /**最大车道数目**/
#define MAX_DCTL_NUM                        (5)

#define MAX_FLASH_ADDR_SUM                    4        /* 最大支持几个闪光灯 */

#define  MAX_DEV_CODE_LEN                    12
#define  EXT_DEV_CODE_LEN                    8

#define  PP_DEFAULT_PORT_NAME                "请设置卡口名称"
#define  PP_DEFAULT_DIRECTION                "请设置卡口方向"
#define  PP_DEFAULT_DEV_CODE                 "PPDEV"


#define  DEFAULT_PORT_NAME                   "请设置电警路口名称"
#define  DEFAULT_DIRECTION                   "请设置电警路口方向"
#define  DEFAULT_ROAD_REMARKS				 "左1直行1右1"
/**卡口车道默认配置信息**/

#define  DEFAULT_PP_CAPTIONX       		    (10)
#define  DEFAULT_PP_CAPTIONY       		    (10)

#if defined(CYC500KKW2C)
#define	 DEFAULT_PP_LANES					2
#else
#define	 DEFAULT_PP_LANES					1	
#endif

#if defined(CYC500KD)
#define	 DEFAULT_SD_SNAP_EN					1
#else
#define	 DEFAULT_SD_SNAP_EN					0	
#endif

#if defined(CYC500XKW4C)||defined(CYC500XAW)
#define  DEFAULT_PP_AREASTARTX     		    (230)
#define  DEFAULT_PP_AREASTARTY     		    (1100)
#define  DEFAULT_PP_AREAWIDTH      		    (1024)
#define  DEFAULT_PP_AREAHEIGHT    		    (672)

#define  DEFAULT_PP_VLSTARTX     			(140)
#define  DEFAULT_PP_VLSTARTY     			(1220)
#define  DEFAULT_PP_VLWIDTH      			(1160)
#define  DEFAULT_PP_VLHEIGHT     			(200)

#elif defined(CYC200XZD)||defined(CYC200KX3)

#define  DEFAULT_PP_AREASTARTX     		    (10)
#define  DEFAULT_PP_AREASTARTY     		    (200)
#define  DEFAULT_PP_AREAWIDTH      		    (1024)
#define  DEFAULT_PP_AREAHEIGHT    		    (672)

#define  DEFAULT_PP_VLSTARTX     			(60)
#define  DEFAULT_PP_VLSTARTY     			(750)
#define  DEFAULT_PP_VLWIDTH      			(1400)
#define  DEFAULT_PP_VLHEIGHT     			(200)

#elif defined(CYC200K2)||defined(CYC200MK)||defined(CYC200K3)
#define  DEFAULT_PP_AREASTARTX     		    (250)
#define  DEFAULT_PP_AREASTARTY     		    (500)
#define  DEFAULT_PP_AREAWIDTH      		    (1024)
#define  DEFAULT_PP_AREAHEIGHT    		    (672)

#define  DEFAULT_PP_VLSTARTX     			(60)
#define  DEFAULT_PP_VLSTARTY     			(750)
#define  DEFAULT_PP_VLWIDTH      			(1400)
#define  DEFAULT_PP_VLHEIGHT     			(200)

#elif defined(CYC500KD)
#define  DEFAULT_PP_AREASTARTX     		    (800)
#define  DEFAULT_PP_AREASTARTY     		    (540)
#define  DEFAULT_PP_AREAWIDTH      		    (1024)
#define  DEFAULT_PP_AREAHEIGHT    		    (672)

#define  DEFAULT_PP_VLSTARTX     			(560)
#define  DEFAULT_PP_VLSTARTY     			(780)
#define  DEFAULT_PP_VLWIDTH      			(1400)
#define  DEFAULT_PP_VLHEIGHT     			(200)

#else
#define  DEFAULT_PP_AREASTARTX     		    (10)
#define  DEFAULT_PP_AREASTARTY     		    (200)
#define  DEFAULT_PP_AREAWIDTH      		    (1024)
#define  DEFAULT_PP_AREAHEIGHT    		    (672)

#define  DEFAULT_PP_VLSTARTX     			(60)
#define  DEFAULT_PP_VLSTARTY     			(750)
#define  DEFAULT_PP_VLWIDTH      			(1400)
#define  DEFAULT_PP_VLHEIGHT     			(200)
#endif

#define  DEFAULT_PP_MAXSPEED				(60)
#define  DEFAULT_PP_MINSPEED				(5)
#define  DEFAULT_PP_COILDIST              	(500)


/**卡口虚拟线圈默配置信息**/

#define  DEFAULT_VL_DAY2NIGHT            	(30)
#define  DEFAULT_VL_NIGHT2DAY				(50)
#define  DEFAULT_VL_NIGHTTHRESH				(500)
#define  DEFAULT_VL_DAYTHRESH				(1000)
#define  DEFAULT_VL_MOTOTHRESH           	(350)
#define  DEFAULT_VL_EVENTHRESH           	(10)
#define  DEFAULT_VL_DAYMOTOWIDTH         	(240)
#define  DEFAULT_VL_NIGHTMOTOWIDTH      	(240)
#define  DEFAULT_VL_CAPDELAYI				(10)

/* 车道最多方向种类 */
#define MAX_LANE_DIR_TYPE            (7)

/* 交通灯最多类型宏定义 */
#define MAX_LIGHT_TYPE               (4)

/**电子警察默认配置信息**/
#define  EP_DEFAULT_DEV_CODE                 "EPDEVNO"

#define  DEFAULT_EP_REDSTARTX        		(0)
#define  DEFAULT_EP_REDSTARTY				(0)
#define  DEFAULT_EP_REDWIDTH				(0)
#define  DEFAULT_EP_REDHEIGHT				(0)

#if defined(CYC500XCKW)||defined(CYC500XZD)||defined(CYC500XKW)||defined(CYC500XLKW)||defined(CYC500KX2)||defined(CYC500JX)
#define  DEFAULT_EP_VL_X3             		(50)
#define  DEFAULT_EP_VL_Y3             		(400)

#define  DEFAULT_EP_VL_X1             		(50)
#define  DEFAULT_EP_VL_Y1             		(1400)

#define  DEFAULT_EP_VL_X2             		(800)
#define  DEFAULT_EP_VL_Y2             		(1900)

#define	 DEFAULT_MIN_PLATE_SIZE				90
#define	 DEFAULT_MAX_PLATE_SIZE				120

#define  DEFAULT_EP_GROUD_X            		(900)  /* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(1500)  /* 车道范围Y坐标 */

#define	 DEFAULT_STROBE_FLASH_FRE			8

#elif defined(CYC800JX)||defined(CYC800XKW4C)

#define  DEFAULT_EP_VL_X3             		(140)
#define  DEFAULT_EP_VL_Y3             		(600)

#define  DEFAULT_EP_VL_X1             		(140)
#define  DEFAULT_EP_VL_Y1             		(1600)

#define  DEFAULT_EP_VL_X2             		(1000)
#define  DEFAULT_EP_VL_Y2             		(2300)

#define	 DEFAULT_MIN_PLATE_SIZE				110
#define	 DEFAULT_MAX_PLATE_SIZE				150

#define	 DEFAULT_STROBE_FLASH_FRE			8

#define  DEFAULT_EP_GROUD_X            		(1024)  /* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(2300)  /* 车道范围Y坐标 */


#elif defined(CYC200XZD)||defined(CYC200KX3)||defined(CYC200XKW)
#define  DEFAULT_EP_VL_X3             		(80)
#define  DEFAULT_EP_VL_Y3             		(400)

#define  DEFAULT_EP_VL_X1             		(80)
#define  DEFAULT_EP_VL_Y1             		(1080)

#define  DEFAULT_EP_VL_X2             		(500)
#define  DEFAULT_EP_VL_Y2             		(1500)

#define	 DEFAULT_MIN_PLATE_SIZE				80
#define	 DEFAULT_MAX_PLATE_SIZE				110

#define	 DEFAULT_STROBE_FLASH_FRE			4

#define  DEFAULT_EP_GROUD_X            		(80)  	/* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(1200)  /* 车道范围Y坐标 */

#elif defined(CYC200K2)||defined(CYC200MK)||defined(CYC200K3)

#define  DEFAULT_EP_VL_X3             		(50)
#define  DEFAULT_EP_VL_Y3             		(400)

#define  DEFAULT_EP_VL_X1             		(50)
#define  DEFAULT_EP_VL_Y1             		(1200)

#define  DEFAULT_EP_VL_X2             		(380)
#define  DEFAULT_EP_VL_Y2             		(1500)

#define	 DEFAULT_MIN_PLATE_SIZE				90
#define	 DEFAULT_MAX_PLATE_SIZE				120

#define	 DEFAULT_STROBE_FLASH_FRE			4

#define  DEFAULT_EP_GROUD_X            		(1024)  /* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(1500)  /* 车道范围Y坐标 */

#elif defined(CYC500KD)

#define  DEFAULT_EP_VL_X3             		(50)
#define  DEFAULT_EP_VL_Y3             		(400)

#define  DEFAULT_EP_VL_X1             		(50)
#define  DEFAULT_EP_VL_Y1             		(1400)

#define  DEFAULT_EP_VL_X2             		(800)
#define  DEFAULT_EP_VL_Y2             		(1900)

#define	 DEFAULT_MIN_PLATE_SIZE				120
#define	 DEFAULT_MAX_PLATE_SIZE				140

#define  DEFAULT_EP_GROUD_X            		(900)  /* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(1500)  /* 车道范围Y坐标 */

#define	 DEFAULT_STROBE_FLASH_FRE			8

#elif defined(CYC500XKW4C)||defined(CYC500XAW)

#define  DEFAULT_EP_VL_X3             		(50)
#define  DEFAULT_EP_VL_Y3             		(400)

#define  DEFAULT_EP_VL_X1             		(50)
#define  DEFAULT_EP_VL_Y1             		(1400)

#define  DEFAULT_EP_VL_X2             		(800)
#define  DEFAULT_EP_VL_Y2             		(1900)

#define	 DEFAULT_MIN_PLATE_SIZE				90
#define	 DEFAULT_MAX_PLATE_SIZE				120

#define  DEFAULT_EP_GROUD_X            		(900)  /* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(1500)  /* 车道范围Y坐标 */

#define	 DEFAULT_STROBE_FLASH_FRE			4

#else

#define  DEFAULT_EP_VL_X3             		(50)
#define  DEFAULT_EP_VL_Y3             		(400)

#define  DEFAULT_EP_VL_X1             		(50)
#define  DEFAULT_EP_VL_Y1             		(1200)

#define  DEFAULT_EP_VL_X2             		(380)
#define  DEFAULT_EP_VL_Y2             		(1500)

#define	 DEFAULT_MIN_PLATE_SIZE				160
#define	 DEFAULT_MAX_PLATE_SIZE				200

#define	 DEFAULT_STROBE_FLASH_FRE			4

#define  DEFAULT_EP_GROUD_X            		(1024)  /* 车道范围X坐标 */
#define  DEFAULT_EP_GROUD_Y            		(1500)  /* 车道范围Y坐标 */

#endif

#define  KEY_RED                "Red"
#define  KEY_GREEN            	"Green"
#define  KEY_BLUE               "Blue"

#define  DEFAULT_RED_LIGHT_R_LOW_VALUE      (200)
#define  DEFAULT_RED_LIGHT_G_LOW_VALUE      (120)
#define  DEFAULT_RED_LIGHT_B_LOW_VALUE      (0)

#define  DEFAULT_RED_LIGHT_R_HIGH_VALUE     (254)
#define  DEFAULT_RED_LIGHT_G_HIGH_VALUE     (253)
#define  DEFAULT_RED_LIGHT_B_HIGH_VALUE     (220)


#define  DEFAULT_GREEN_LIGHT_R_LOW_VALUE    (0)
#define  DEFAULT_GREEN_LIGHT_G_LOW_VALUE    (220)
#define  DEFAULT_GREEN_LIGHT_B_LOW_VALUE    (220)

#define  DEFAULT_GREEN_LIGHT_R_HIGH_VALUE   (100)
#define  DEFAULT_GREEN_LIGHT_G_HIGH_VALUE   (252)
#define  DEFAULT_GREEN_LIGHT_B_HIGH_VALUE   (251)


#define  DEFAULT_YELLOW_LIGHT_R_LOW_VALUE    (220)
#define  DEFAULT_YELLOW_LIGHT_G_LOW_VALUE    (0)
#define  DEFAULT_YELLOW_LIGHT_B_LOW_VALUE    (220)

#define  DEFAULT_YELLOW_LIGHT_R_HIGH_VALUE   (250)
#define  DEFAULT_YELLOW_LIGHT_G_HIGH_VALUE   (100)
#define  DEFAULT_YELLOW_LIGHT_B_HIGH_VALUE   (249)





/**图片合成方式**/
#define  PIC_STYLE_NONE			        (0x0001)
#define	 PIC_STYLE_THREE		        (0x0002)
#define  PIC_STYLE_SIX			        (0x0003)
#define  PIC_STYLE_NINE			        (0x0004)
#define	 PIC_SEPARATE_THREE		        (0x0005)
#define	 PIC_STYLE_THREE_LEFT	        (0x0006)
#define	 PIC_SEPARATE_FOUR		        (0x0007)

#define  DEFAULT_PIC_STYLE		         PIC_STYLE_THREE


/* 车道行驶方向定义 */
#define  LANE_DIR_STRAIGHT               (0x0001)
#define  LANE_DIR_LEFT					 (0x0002)
#define  LANE_DIR_RIGHT                  (0x0004)
#define  LANE_DIR_RETURN                 (0x0008)
#define  LANE_DIR_BICYCLE                (0x0010) /* 非机动车道 */
#define  LANE_DIR_STRAIGHT_WAIT          (0x0020) /* 直行待驶区 */
#define  LANE_DIR_LEFT_WAIT				 (0x0040) /* 左转待驶区 */


#define  DEFAULT_LANE_DIR		         LANE_DIR_STRAIGHT

/* 车道禁止行驶类型宏定义 */
#define  EP_FORBID_DIR_LEFT               (0x0001)
#define  EP_FORBID_DIR_RIGHT              (0x0002)
#define  EP_FORBID_DIR_STRAIGHT           (0x0004)
#define  EP_FORBID_DIR_RETURN             (0x0008)

#define  EP_FORBID_MAX_DIR				  (0x0004)

/* 交通灯指示方向定义 */
#define  LIGHT_DIR_STRAIGHT               (0x0001)
#define  LIGHT_DIR_LEFT					  (0x0002)
#define  LIGHT_DIR_RIGHT                  (0x0003)
#define  LIGHT_DIR_RETURN                 (0x0004)
#define  LIGHT_DIR_BICYCLE                (0x0005)

#define  DEFAULT_LIGHT_DIR		         LIGHT_DIR_STRAIGHT

/* 电警辅助红灯相关宏定义 */
#define  EP_ASSIST_RED_LIGHT_NUM		  (0x0004)


/**车检器类型**/
#define  COIL_TYPE_HIT			            (0x0001)
#define  COIL_TYPE_KJ			            (0x0002)

#define  DEFAULT_COMID                     (0)
#define  DEFAULT_DATABITS                  (8)
#define  DEFAULT_STOPBITS                  (1)
#define  DEFAULT_PARITY                    (0)
#define  DEFAULT_BAUDRATE                  (115200)

#define	 DEFAULT_COIL_TYPE		            COIL_TYPE_KJ
/**线圈类型**/
#define  VIRTUAL_LOOP			            (0x0001)
#define  GROUD_LOOP				            (0x0002)
#define  DISABLE_LOOP_DET		            (0x0003)

#define  DEFAULT_LOOP 			            VIRTUAL_LOOP

/* 单线圈还是双线圈或者雷达测速抓拍 */
#define  GROUD_DUBAL_COIL					(0x01)
#define  GROUD_SINGAL_COIL_NO_SPEED			(0x02)
#define  GROUD_SINGAL_COIL_SPEED			(0x03)
#define  CS_RADAR_SPEED						(0x04)

/* 进线圈还是出线圈抓拍 */
#define  SINGAL_COIL_ENTER_SNAP				(0x01)
#define  SINGAL_COIL_LEAVE_SNAP				(0x02)

/* 前线圈还是后线圈抓拍 */
#define  SINGAL_COIL_POS_FRONT				(0x01)
#define  SINGAL_COIL_POS_BEHIND				(0x02)

/**补光类型**/
#define  LIGHT_TYPE_HALGON		            (0x0001)
#define  LIGHT_TYPE_FLASH		            (0x0002)

#if defined(CYC500XKW4C)||defined(CYC500XAW)
#define  DEFAULT_LIGHT_TYPE		            LIGHT_TYPE_HALGON
#else
#define  DEFAULT_LIGHT_TYPE		            LIGHT_TYPE_FLASH
#endif

/* 故障定义 */
#define  VEHICLE_BUG_OK						0
#define  VEHICLE_BUG_ERROR					1

/**红灯信号检测器**/
#define	 CONNECT_TYPE_COM		            (0x0001)/**连接方式为串口**/
#define  CONNECT_TYPE_GPIO		            (0x0002)/**连接方式为GPIO  **/

#define  DEFAULT_CONNECT_TYPE	            CONNECT_TYPE_GPIO

/**红灯检测方式**/
#define  RED_DETECT_VIDEO		            (0x0001)
#define	 RED_DETECT_SINGAL		            (0x0002)

#define  DEFAULT_RED_DETECT		            RED_DETECT_SINGAL

/**闪光灯设备类型**/
#define  FLASH_TYPE_ROAD		            (0x0001)

#define	 DEFAULT_FLASH_TYPE		            FLASH_TYPE_ROAD

/* 闪光灯控制模式 */
#define  FLASH_CTL_MODE_SPECIFY				0x0001
#define  FLASH_CTL_MODE_ROUND				0x0002

/* 默认最大有效车道宏定义 */
#define  MAX_VALID_CHAN_SUM		            (0x0004)

/* 默认最大运行车距 */
#define  DEFAULT_CAR_DISTANCE               (20)

/* 默认九合一间隔帧数 */
#define  DEFAULT_FRAME_INTERVAL             (5)

/**摄像机控制项**/

#define  ESHUTTER				            (0x00)
#define  EGAIN					            (0x01)
#define  EEXPOSURE				            (0x02) //平均灰度
#define  ECONTRAST				            (0x03) //自动白平衡
#define  EBRIGHTNESS			            (0x04) //亮度，固定为16
#define  EGAMMA					            (0x05) 
#define  ESHARPNESS			            	(0x06) //MinScal
#define  ESATURATION			            (0x07) //颜色校正

#define  EISO					            (0x08)  /* 感光度 */
#if defined(XCC200)
#define  EZOOM					            (0x09)	/* 缩放 */
#define  EFOCUS					            (0x0A)	/* 手动对焦 */
#endif
#define  EAWB								(0x0B)  /* 降噪 */

#define  DEFAULT_CAMERA_MINVAL            (0x00000000)
#define  DEFAULT_CAMERA_MAXVAL            (0x10000)
#define  DEFAULT_CAMERA_FIXVAL            (0x200)

/* 切换状态初始值 */
#define	 SWITCH_DEFAULT_VAL					( 0xff )


/**ftp 默认配置**/
#define  DEFAULT_FTP_SERVERIP             "255.255.255.0"
#define  DEFAULT_FTP_PORT                  (21)
#define  DEFAULT_FTP_USER                  ""
#define  DEFAULT_FTP_PASSWD                ""

/**NTP 默认配置**/
#define  DEFAULT_NTP_SERVER                "255.255.255.0"

/**V3 平台默认配置**/
#define  DEFAULT_V3_SERVERIP             "255.255.255.0"
#define  DEFAULT_V3_SERVERPORT           (7777)

/**V2 平台默认配置**/
#define  DEFAULT_V2_SERVERIP             "255.255.255.0"
#define  DEFAULT_V2_SERVERPORT           (7777)


/**NM 平台默认配置**/
#define  DEFAULT_NM_SERVERIP             "192.168.0.250"
#define  DEFAULT_NM_SERVERPORT           (7777)

/* 默认网关配置 */
#define  DEFAULT_GW_SERVERIP             "255.255.255.0"

#define DEFAULT_APERTURE		0
#define DEFAULT_SHUTTER			1
#define DEFAULT_GAIN			2
#define DEFAULT_COLOR			1
#define DEFAULT_WHITEBALANCE	2
#define DEFAULT_BACKCOMPENSATE	0
#define DEFAULT_MIRROR			0
#define DEFAULT_WBVALUE			(0x7FF)
#define DEFAULT_AEOPDWH			10
#define DEFAULT_AEOPDWV			7
#define DEFAULT_AEOPDUNIT		8
#define DEFAULT_AEOPDWHSET		6
#define DEFAULT_AEOPDWVSET		0

/* 区分新旧isp算法 */
#if defined(CYC500XKW4C)||defined(CYC500XAW)
#define OLD_ISP_SET		0	/* 上海isp为0,公司isp为1 */
#define NEW_PIC_ARITHC	0	/* 上海isp为0,公司isp为1，灰度值必须固定为60,对比度即亮度要大于等于3*/
#else
#define OLD_ISP_SET		1	/* 上海isp为0,公司isp为1 */
#define NEW_PIC_ARITHC	1	/* 上海isp为0,公司isp为1，灰度值必须固定为60,对比度即亮度要大于等于3*/
#endif

#define USE_FACE_IDENT	1   /* 是否使能人脸识别 */

#endif /* _XCS_GLOBAL_DEF_H__ */


