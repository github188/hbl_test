#ifndef _DM8127_DSP_SERVICE_H_
#define _DM8127_DSP_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NET_DEV_SN_LENGTH	20            /*电子警察或卡口序列号长度设置*/
#define MAX_JPEG_NUM		6
#define MAX_IRREG_CAR_NUM    6 			   /* 一帧最大违章车辆总数 */
#define LIGHT_MAX_NUM		3


#define MAX_DSP_IRREG_CODE_LEN (8)
#define MAX_DSP_IRREG_NAME_LEN (64)
#define MAX_FACE_NUM 5  /* 最大人脸个数 */
/* 报警信息回调接口字义 */
#define COMM_ALARM_PDC 1	/* 流量上报 对应结构体 flowstat_status_t*/
#define COMM_ALARM_VL  2	/* 视频丢失、运动检测对应结构体 alarm_report_t */
#define COMM_ALARM_LS  3	/* 红灯状态上报对应结构体 "trafficlight_status_t" */

/*--------------------------------------错误代号设定-------------------------------------*/
#define NET_DEV_NOERROR					0		/*没有错误*/
#define NET_DEV_SDK_INIT_ERROR			1		/*SDK启动初始化错误*/
#define NET_DEV_SDK_START_ERROR			2		/*SDK参数配置完成之后启动失败*/
#define NET_DEV_SDK_STOP_ERROR			3		/*SDK停止失败*/
#define NET_DEV_SDK_CLEAN_ERROR			4		/*SDK资源清空失败*/

#define NET_DEV_SET_CONFIG_SIZE_ERROR	5		/*参数配置结构体大小错误*/
#define NET_DEV_SET_CONFIG_RESULT_ERROR	6		/*参数配置返回错误*/
#define NET_DEV_SET_CONFIG_PARAM_ERROR  7		/*参数配置参数错误*/

//For debug
typedef enum
{
    LV_NONE		= 0x0,
    LV_ERROR	= 0x1,
    LV_WARNING	= 0x2,
    LV_INFO		= 0x4,
    LV_ALL		= 0x7,
} E_DEBUG_LEVEL;

extern int shell_print(const char* format, ...);
extern int* g_debug_level;
#define DVS_DEBUG(level, format, args...) do{\
        if (g_debug_level != NULL)\
        {\
            if (level & *g_debug_level)\
            {\
                shell_print("%s(%d) : "format"\n",__FUNCTION__, __LINE__,  ##args);\
            }\
        }\
        else\
        {\
            if (level & LV_ERROR)\
            {\
                shell_print("%s(%d) : "format"\n",__FUNCTION__, __LINE__,  ##args);\
            }\
        }\
    }while(0)



#define NET_DEV_SET_VENC_PARAM						0x0101		/*编码参数设置---------venc_param_t*/
#define NET_DEV_SET_VENC_STATUS						0x0102		/*编码状态设置---------VENC_STATUS_PARAM*/

#define NET_DEV_SET_OSD_CHANNAME					0x0201		/*osd叠加车道名--------channame_param_t*/
#define NET_DEV_SET_OSD_TIME_TITLE					0x0202		/*osd时间叠加----------time_param_t*/
#define NET_DEV_SET_PP_OSD_STRING					0x0203		/*卡口osd字符串叠加--------osd_param_t*/
#define NET_DEV_SET_EP_OSD_SET_STRING				0x0204		/*电子警察字幕配置接口--------osd_param_t*/
#define NET_DEV_SET_PP_ROAD_DIRECTION_OSD			0x0205		/*卡口路口方向osd叠加------osd_param_t*/
#define NET_DEV_SET_PP_ROAD_OSD						0x0206		/*卡口路口名osd叠加------osd_param_t*/
#define NET_DEV_SET_PP_ROAD_DESC_OSD				0x0207		/*卡口路口描述osd叠加-----osd_param_t*/
#define NET_DEV_SET_PP_LANE_OSD						0x0208		/*卡口车道名osd叠加------osd_param_t*/
#define NET_DEV_SET_EP_ROAD_DIRECTION_OSD			0x0209		/*电警路口方向osd叠加------osd_param_t*/
#define NET_DEV_SET_EP_ROAD_OSD						0x0210		/*电警路口名osd叠加------osd_param_t*/
#define NET_DEV_SET_EP_ROAD_DESC_OSD				0x0211		/*电警路口描述osd叠加-----osd_param_t*/
#define NET_DEV_SET_EP_LANE_OSD						0x0212		/*电警车道名osd叠加------osd_param_t*/
#define NET_DEV_SET_PP_JPG_OSD_CONF					0x0213		/*卡口图片osd叠加配置信息----DSP_PP_JPG_OSD_CONF*/
#define NET_DEV_SET_EP_JPG_OSD_CONF					0x0214		/*电警图片osd叠加配置信息-----DSP_EP_JPG_OSD_CONF*/
#define NET_DEV_SET_JPG_OSD_COLOR					0x0215		/*车道图片字幕颜色-------JPG_OSD_COLOR_PARAM*/

#define NET_DEV_SET_VILIB_PARAM						0x0301		/*虚拟线圈库参数设置------VILib_feature_t*/
#define NET_DEV_SET_VIRTUAL_LOOP					0x0302		/*画虚拟线圈位置-------VirtualLoop_feature_t*/
#define NET_DEV_SET_GT5000_SNAPSHOT_SIZE_SET		0x0303		/*电子警察设定抓拍区域---------HSLRECT*/
#define NET_DEV_SET_GT5000_SHINE_OR_NOT				0x0304		/*闪光灯是否开启-------------GT5000_SHINEMODE_t*/
#define NET_DEV_SET_LIMIT_SPEED						0x0305		/*设置限定速度--------LIMIT_SPEED_PARAM*/
#define NET_DEV_SET_CAR_FLOWSTAT					0x0306		/*设置车流量统计信息---------FlowStatInfo*/
#define NET_DEV_SET_FORBID_DIRC						0x0307		/*设置车道禁止通行方向-------forbid_direct_param*/
#define NET_DEV_SET_LANE_FORBID_ATTRIBUTE			0x0308		/*设置车道禁止通行属性-------lane_attribut_info_t*/
#define NET_DEV_SET_CARTRACK_ENABLE					0x0309		/*电子警察轨迹跟踪使能-----EP_CARTRACK_PARAM*/
#define NET_DEV_SET_SNAP_DIRECTION					0x0310		/*设置抓拍方向-----SNAP_DIRECTION_PARAM*/
#define NET_DEV_SET_LOOP_MODE						0x0311		/*算法检测模式------ALG_LOOP_MODE_PARAM*/
#define NET_DEV_SET_VEHICLE_DETECT_TYPE				0x0312		/*车辆检测方式-------VEHICLE_DETECT_TYPE*/
#define NET_DEV_SET_FORCE_KEY_FRAME					0x0313		/*强制关键帧--------int chan_id*/
#define NET_DEV_SET_VALID_LANE_NUM					0x0314		/*设置有效车道数-----VALID_LANE_NUM_PARAM*/
#define NET_DEV_SET_LICENCE_PLATE_RECOGNIZE			0x0315		/*车牌识别信息设置--------LICENCE_PLATE_INFO*/
#define NET_DEV_SET_PLATE_VIDEO_DEBUG				0x0316		/*车牌调试接口------PLATE_VIDEO_DEBUG_PARAM*/
#define NET_DEV_SET_VIDEO_SPEED_MEASURE				0x0317		/*视频测速-------SPEED_VISUAL_DETECTION*/
#define NET_DEV_SET_TIME_PERIOD_DIRECTION			0x0318		/*按时间段单向抓拍配置------TIME_PERIOD_DIRECTION*/
#define NET_DEV_SET_STOP_ON_NO_ENTRY				0x0319		/*路口滞留参数配置---------DSP_SOT_NO_ENTRY_INFO*/
#define NET_DEV_SET_TL_STATUS_INFO					0x0320		/*交通灯状态信息-------DEV_TL_STATUS_INFO*/

#define NET_DEV_SET_EP_VI_DETECT_TYPE				0x0371		/*电子警察设置检测方式---------EP_VEHICLE_DETECT_TYPE*/
#define NET_DEV_SET_EP_VIRTUAL_LOOP					0x0372		/*设置电子警察模式虚拟线圈-------AREARECT*/
#define NET_DEV_SET_EPVL_OFFSET						0x0373		/*设置虚拟线圈电子警察抓拍第0步和第2步与第1步之间的步数-----vlep_offset_param*/
#define	NET_DEV_SET_EPVL_FEATURE					0x0374		/*设置虚拟线圈车牌和亮度门限相关参数------EPVLFeature*/
#define NET_DEV_SET_EPVL_LANE_ND_RED_LIGHT			0x0375		/*设置车道正常行驶的方向和红灯状态------lane_nd_red_light_flag*/
#define NET_DEV_SET_EPVL_LANE_ASSIST_DETECT_PARAM	0x0376		/*辅助电警算法检测参数配置-----ep_assist_detect_param*/
#define NET_DEV_SET_EP_IRREG_INFO					0x0377		/*电子警察违章信息--------DSP_EP_IRREG_INFO*/
#define NET_DEV_SET_EP_SN							0x0378		/*电子警察SN-----char *pSN*/
#define NET_DEV_SET_EP_LPR							0x0379		/*电子警察车牌识别------LICENCE_PLATE_INFO*/

#define NET_DEV_SET_PPVL_ENABLE						0x03c1		/*卡口虚拟线圈使能------PP_VL_ENABLE_PARAM*/
#define NET_DEV_SET_PP_SNAP_TARGET					0x03c2		/*卡口抓拍目标---------PPVL_SNAP_TARGET_INFO*/
#define NET_DEV_SET_PPVL_SNAPSTEP_INFO				0x03c3		/*卡口设置虚拟线圈抓拍张数------PPVL_SNAP_STEPS_INFO*/
#define NET_DEV_SET_PP_DEBUG						0x03c4		/*卡口抓拍相关配置--------PPVL_SNAP_DEBUG_INFO*/
#define NET_DEV_SET_PP_IRREG_INFO					0x03c5		/*卡口违章信息---------DSP_PP_IRREG_INFO*/
#define NET_DEV_SET_PP_SN							0x03c6		/*卡口SN---------char *pSN*/


#define NET_DEV_SET_NOTIFY_RED_LIGHT				0x0401		/*红灯开始时间---------NOTIFY_RED_LIGHT_PARAM*/
//#define NET_DEV_SET_TRAFFICLIGHT_COLOR_THRESHOLD	0x0402		/*设置红绿灯阀值---------TRAFFICLIGHT_COLOR_THRESHOLD*/
//#define NET_DEV_SET_TRAFFICLIGHT_RECT				0x0403		/*设置红绿灯区域----------TrafficLightRect*/
#define NET_DEV_SET_EP_REDLIGHT_FLAG				0x0404		/*设置电子警察红灯标示---------EP_REDLIGHT_FLAG_PARAM*/
//#define NET_DEV_SET_REVISE_REDLIGHT_DEGREE			0x0405		/*红灯修正程度------int degree*/
#define NET_DEV_SET_TLGRPS_CFG						0x0406

#define NET_DEV_SET_RADAR_DETECT_ENABLE				0x0501		/*雷达检测使能------RADAR_DETECT_PARAM*/
//#define NET_DEV_SET_FLASH_PORT						0x0502	/*闪光灯配置------Lane_Flash_Port_t*/

#define NET_DEV_SET_GET_VERSION						0x0601		/*获取版本信息*/

#define NET_DEV_SET_CCDPARAMCFG  					0x0701 		/* CCD参数配置 ---- NET_DEV_CCDPARAMCFG */
#define NET_DEV_SET_I2CPARAM						0x0702		/*I2C参数配置 -----char * I2C_PARAM_T*/

#define NET_DEV_SET_TIME							0x0801		/*设置DSP时间 ------ int seconds*/

//获取配置信息的cmd定义
#define NET_DEV_GET_CCDPARAMCFG						0x1701		/*获取CCD参数配置-------CUR_CAMERA_STATUS*/

typedef struct
{
    unsigned int chan_id;
    unsigned int lane_id;
    unsigned int step0;		//step0 和step1之间的帧数
    unsigned int step2;		// step1 和 step2之间的帧数
    char resv[64];
} vlep_offset_param;

typedef struct
{
    int	 chan_id;			/*通道号*/
    int  lane_id;			/*车道号*/
    int  ThrDayToNight;		//白天转为夜间的参考门限
    int  ThrNightToDay;		//夜间转为白天的参考门限
    int  corrNightThresh;	// 夜间相关值门限
    int  corrDayThresh;		// 白天相关值门限
    int  corrMotoThresh;	// 摩托车相关值门限
    int  corrEveningThresh;	//黄昏门限值 --参考值20
    int  dayMotorWidth;
    int  nightMotorWidth;
    int  capDelay;
    int  minPlateWidth;
    int  maxPlateWidth;
    char resv[128];
} VILib_feature_t;

/*
 * 编码器参数
 */
typedef struct
{
    unsigned int  chan_id;		/*通道号*/
    unsigned int  encType;      /*编码类型*/
    unsigned int  imgFmt;       /*编码格式*/
    unsigned int  keyIntv;      /*I帧间隔*/
    unsigned int  bitRate;      /*码率kbps*/
    unsigned int  frameRate;    /*帧率*/
    unsigned int  nEncQuality;  /*编码质量*/
    unsigned int  nMotionDetect;
    unsigned int  is_cbr;
    char resv[128];
} venc_param_t ;

/*
 * 通道名称参数
 */
typedef struct
{
    unsigned int chan_id;  /*通道号*/
    unsigned int startX;
    unsigned int startY;
    unsigned int width;
    unsigned int height;
    unsigned int name[16];
    char resv[128];
} channame_param_t;

/*
 *字幕叠加信息
*/
typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;
    unsigned int startX;
    unsigned int startY;
    unsigned int width;
    unsigned int height;
    unsigned int name[16];
    char resv[128];
} osd_param_t;

/*
 * 通道时间坐标
 */
typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int startX;	/*4CIF坐标*/
    unsigned int startY;	/*4CIF坐标*/
    char resv[64];
} time_param_t ;

typedef struct
{
    int	 chan_id;	/*通道号*/
    int  lane_id;   /*车道号*/
    int  bShowFlag;	/*显示标志*/
    int  x1;		//左
    int  x2;		//右
    int  y1;		//上
    int  y2;		//下
    char resv[64];
} VirtualLoop_feature_t;

typedef struct
{
    int chan_id;	/*通道号*/
    int lane_id;	/*车道号*/
    int startX;
    int startY;
    int width;
    int height;
    int bShow;		//是否显示线圈，1:显示；0:不显示
    char resv[64];
} HSLRECT;

// 电子警察 红灯区域
typedef struct _THRESHOLD
{
    int max;
    int min;
} THRESHOLD;

typedef struct _COLOR_THRESHOLD
{
    THRESHOLD rthr;	//红
    THRESHOLD gthr;	//绿
    THRESHOLD bthr;	//蓝
    char resv[16];
} COLOR_THRESHOLD;

typedef struct _TRAFFICLIGHT_COLOR_THRESHOLD
{
    int		chan_id;    /*通道号*/
    COLOR_THRESHOLD red;
    COLOR_THRESHOLD green;
    COLOR_THRESHOLD yellow;
    char resv[128];
} TRAFFICLIGHT_COLOR_THRESHOLD;

typedef struct _RECT
{
    int left;
    int top;
    int right;
    int bottom;
} RECT;

typedef struct
{
    int chn;				//通道号；
    char index;				//交通灯组编号；
    char bShow;				//是否显示交通灯框，    1:显示线圈，0:不显示线圈
    char bTrafficRevise;	//是否红灯修正；        1:修正，0:不修改
    char bTrafficDetect;	//是否视频检测红绿灯:    1:检测，0:不检测
    RECT rect[4];
    char bTrafficStrobe;	//是否频闪修正,1:修正,0:不修正
    char bFreqFrames;		//频闪修正帧数配置
    char bSingleColorTrafficLight;	//是否是单一颜色交通灯，0:红绿黄是三个独立的交通灯,1:红绿黄共用一个交通灯
    unsigned char bRedLightS;	//普通描述为饱和度，越大越红
    unsigned char bRedLightV;	//普通描述为亮度，越大越亮
    unsigned char ucMapRLNum;	//对应红灯检测器
    char reserve[130];
} TrafficLightRect;

typedef enum
{
    SHINE_LED_MODE = 0,
    NO_SHINE_LED_MODE,
} GT5000_SHINEMODE;

typedef struct
{
    int chan_id;
    GT5000_SHINEMODE shineMode;
    char resv[64];
} GT5000_SHINEMODE_t;

typedef enum
{
    e_INNERVATION_LOOP = 0, //动感线圈方式
    e_VIRTUAL_LOOP,  //虚拟线圈方式
    e_DISABLE_LOOP,	 //禁止虚拟线圈检测
} Vehicle_Detect_type_t;

/*车辆检测类型*/
typedef struct
{
    int chan_id;
    int lane_id;
    Vehicle_Detect_type_t detect_type;
    char resv[64];
} VEHICLE_DETECT_TYPE;

/*EP车辆检测类型*/
typedef struct
{
    int lane_id;
    Vehicle_Detect_type_t detect_type;
    int bayonetSnap;
    char resv[64];
} EP_VEHICLE_DETECT_TYPE;

//坐标点
typedef struct
{
    int X;
    int Y;
} COPOINT;

//矩形区域
typedef struct
{
    int chan_id;	/*通道号*/
    int bShowFlag;	/*显示标志*/
    int snapIntv;	/*抓拍间隔帧数*/
    int snapType;	/*抓拍类型 图片的合成方式*/
    COPOINT  Up0;
    COPOINT  Up1;
    COPOINT  Bottom0;
    COPOINT  Bottom1;
    COPOINT  Leave0; //离开抓拍区域左边点
    COPOINT  Leave1; //离开抓拍区域线右边点
    char resv[128];
} AREARECT;


typedef struct
{
    unsigned int chan_id;		//通道号,高清默认为0
    unsigned int lane_id;		//车道号,从0开始
    unsigned int report_flag;	//是否上传车流量统计,1-上传；0-不上传。
    unsigned int inter_time;	//上传车流量时间间隔(单位:S)
    unsigned char resv[64];		//保留
} FlowStatInfo;

typedef struct
{
    unsigned int lane_id;		/*通道号*/
    unsigned int forbid_direct;	/*禁止方式*/
    unsigned int groupId;		//每个车道有四组，编号0，1，2，3
    unsigned int flag1;			//0:取消禁止，1:开启禁止
    unsigned int begin1_time;	//开始时间(单位:s)
    unsigned int last1_time;	//持续时间(单位:s)
    unsigned int flag2;			//0:取消禁止，1:开启禁止
    unsigned int begin2_time;	//开始时间(单位:s)
    unsigned int last2_time;	//持续时间(单位:s)
    char resv[128];
} forbid_direct_param;

/* 车道行驶方向定义 */
typedef struct
{
    unsigned int lane;
    unsigned int direction; //direction定义见上。
    char resv[64];
} lane_attribut_info_t;


typedef struct
{
    unsigned int lane; //车道号
    unsigned int minPlateWidth; //最小车牌宽度
    unsigned int maxPlateWidth; //最大车牌宽度
    unsigned int dayToNightThr; //白天转晚上门限
    char resv[64]; //保留
} EPVLFeature;

/*虚拟线圈的抓拍目标*/
typedef enum _EN_SNAP_TARGET
{
    WholeVehicle = 1,       //所有类型
    AutoMobile = 2,         //机动车
    MotoBike = 3            //非机动车
} EN_SNAP_TARGET;

typedef struct _PPVL_SNAP_TARGET_INFO
{
    unsigned int    chan_id;
    unsigned int    lane_id;
    EN_SNAP_TARGET  daySnapTarget;           /*白天抓拍目标*/
    EN_SNAP_TARGET  nightSnapTarget;         /*晚上抓拍目标*/
    unsigned int    beginTime;               /*开始时间(秒)*/
    unsigned int    durationTime;            /*持续时间(秒)*/
    EN_SNAP_TARGET  durationTarget;          /*持续时间内的抓拍目标*/
    unsigned int    ifPlateLighted;          /*车牌是否补亮，0表示未补亮，1表示补亮*/
    char resv[64];
} PPVL_SNAP_TARGET_INFO;

/*设置虚拟线圈抓拍步数*/
typedef struct _PPVL_SNAP_STEPS_INFO
{
    unsigned int chan_id;
    unsigned int lane_id;
    unsigned int SnapStepsAmount;           //单张抓拍为1，两张抓拍为2
    unsigned int IntervalTime;              //两张抓拍模式下，两步抓拍的时间间隔(毫秒)
    char resv[64];
} PPVL_SNAP_STEPS_INFO;

typedef struct _LICENCE_PLATE_INFO
{
    unsigned int chan_id;			/*通道号*/
    unsigned int enable;               //车辆识别使能标志，0 不使能；1 使能
    unsigned int firstCharConf;        //首字符置信度
    unsigned int defaultFirstChar;     //默认首字符
    unsigned short usLPRWidth;			/* 车牌识别宽度 */
    unsigned short usLPRHeight;			/* 车牌识别高度 */
    char resv[64];
} LICENCE_PLATE_INFO;

/*视频测速结构体改动*/
typedef struct _SPEED_VISUAL_DETECTION
{
    unsigned int chan_id;				/*通道号*/
    unsigned int enable;                    //使能标志，1=使能，0=不使能
    unsigned int maxSpeed;                  //超过maxSpeed则为超速行驶
    unsigned int punishSpeed;               //超过该值，则进行处罚
    unsigned short actualDistance;          //虚拟线圈到屏幕上方对应的实际距离(厘米)
    char resv[66]; //保留字节
} SPEED_VISUAL_DETECTION;

typedef struct
{
    unsigned char ucNormalDirection;	//车道允许正常行驶方向
    unsigned char ucRedLightFlag; 		//该方向的红灯标示
    unsigned char ucAidRedLightFlag;	//辅助交通灯，用于判断待行区
    char resv[13];
} normal_direction_red_light_flag;

typedef struct
{
    unsigned int uiLane; //车道号
    normal_direction_red_light_flag strNDRedLight[7]; //车道正常行驶的方向和红灯状态
    char resv[128];
} lane_nd_red_light_flag;



typedef struct
{
    unsigned int uiLane; //车道号，从0开始编号
    unsigned int uiFrmDiffEnable; //是否开启帧差检测
    unsigned int uiStopDistance; //摄像机底部到停车线的水平距离
    unsigned char ucOneLampEnable; //单车灯检测使能
    unsigned char ucRedToGreenSnap; //红转绿红灯抓拍使能
    unsigned char ucNoDirectionSnap; //没有检测到车辆方向抓拍使能使能
    unsigned char ucEPoliceDebug; 	//电警库调试使能
    unsigned char ucReverseSnap;	//逆行抓拍使能
    unsigned char ucRideLineSnap; 	//骑线抓拍使能
    unsigned char ucKakouFlash; 	//卡口闪光使能
    unsigned char ucKakouSnapDelay[4]; //卡口抓拍延迟帧数，分别代表车道1~4
    unsigned char ucMainPicStep;	//特写图片与卡口抓拍步骤,0表示第0步、1表示第1步

    unsigned char ucOsdSpeed; 		//是否叠加速度

    unsigned char ucKakowPicMode;	//卡口图片合成模式(0:全景+特写;1:全图)
    unsigned short usRedLReviseShutter ;//红灯修正快门阀值 ，大于该快门时做红灯修正
    unsigned char ucPlateSegFlag; 	//车牌分割标示
    unsigned char ucPicQuality; 	//图片编码质量，值范围(1~100)，值越大，图片效果越好
    unsigned char ucRedLReviseDegree; //快门大于配置快门值红灯修正程度
    unsigned char ucChangeRoad;			//是否支持变道抓拍
    char cLaneChgType[4];			//车道车辆变道类型抓拍配置，0:所有变道都抓拍，1:只抓拍向左变道，2:只抓拍向右变道

    unsigned char ucPicAssistDebug;  //图片调试信息是否使能
    unsigned char	ucCrossLineParkEn;		//是否使能越线停车违章抓拍
    unsigned char ucResever[2];

    unsigned char ucResever2[3];
    unsigned char aucPressLine[5];	 //车道线是否抓拍配置使能，数组下标即为车道线编号

    char resv[128];
} ep_assist_detect_param;

typedef struct _PPVL_SNAP_DEBUG_INFO
{
    unsigned int    chan_id;
    unsigned int    lane_id;
    unsigned char   isOsdPlateScope;	//是否在图片上叠加车牌识别区域
    unsigned char   isShieldReverse;	//是否上报逆行图片(默认值1)
    unsigned short  plateScopeIncSize;	//(相对于算法报的)车牌范围所增加的值
    unsigned char   radarSpeedGap;		//雷达间断值(连续间断多少帧的速度后，再来速度时认定是另一辆车)
    unsigned char   radarForwardLim;	//雷达速度向前范围(速度提前报可同步到车辆值)
    unsigned char   radarBackwardLim;	//雷达速度向后范围(速度延迟报可同步到车辆值)

    unsigned char   isDecTaillight;		//是否使能尾灯检测

    unsigned char	ucFaceIdenEn;		//人脸识别使能
    unsigned char	ucFaceOvenlayEn;	//人脸叠加是否使能
    char resv[126];		//保留字节
} PPVL_SNAP_DEBUG_INFO;

typedef enum _EP_IRREG_CODE
{
    e_RedCode = 1, 						/* 闯红灯违章代码 */
    e_ReverseCode = 2, 					/* 逆行违章代码 */
    e_InfringForbidLineCode = 3,		/* 违反禁止标线行驶违章代码 */
    e_InfringDirLaneRunCode = 4, 		/* 不按导向车道行驶违章代码 */
    e_StopOnTrafficNoEntryRunCode = 5,	/* 路口遇堵，禁止驶入违章代码 */
    e_dspHighwayInfringDirLaneCode = 6, /* 高速路不按导向车道行驶违章代码 */
} EP_IRREG_CODE;

typedef struct STR_DSP_EP_IRREG_INFO
{
    int iDispFlag;					/* 违章名称是否叠加到图片上 */
    EP_IRREG_CODE epIrregCode;		/*违章代码编号*/
    int iReserve[2];
    char szIrregCode[MAX_DSP_IRREG_CODE_LEN]; /* 闯红灯违章代码 */
    char szIrregName[MAX_DSP_IRREG_NAME_LEN]; /* 闯红灯违章名称 */
    char resv[128];
} DSP_EP_IRREG_INFO;



typedef enum _PP_IRREG_CODE
{
    e_OverSpeedSmallPercent50Code = 1,	/* 超速小于50% */
    e_OverSpeedLargePercent50Code = 2,	/* 超速大于50% */
    e_ReverseRunCode = 3,				/* 逆行 */
    e_InfringeProhibCode = 4,			/* 违反禁令标志 */
} PP_IRREG_CODE;

typedef struct STR_DSP_PP_IRREG_INFO
{
    PP_IRREG_CODE epIrregCode;		/*违章代码编号*/
    int iReserve[2];
    char szIrregCode[MAX_DSP_IRREG_CODE_LEN];
    char szIrregName[MAX_DSP_IRREG_NAME_LEN];
    char resv[128];
} DSP_PP_IRREG_INFO;

typedef struct
{
    int 			lane_id;			/*车道号*/
    char			cSnapDir;			/* 抓拍方向(0:正向;1:逆向) */
    char			cSDEnFlag;			/* 是否使能 */
    char			cTruckEnFlag;		/* 大货车时间段是否生效 */
    char 			cRsv[1];
    unsigned int 	iSDBeginTime1;
    unsigned int 	iSDLastTime1;		/* 持续时间1 */
    unsigned int 	iSDBeginTime2;
    unsigned int 	iSDLastTime2;		/* 持续时间2 */
    unsigned int 	iSDBeginTime3;
    unsigned int 	iSDLastTime3; 		/* 持续时间3 */
    char resv[128];						/* 保留字节 */
} TIME_PERIOD_DIRECTION;

typedef struct
{
    unsigned int 	lane_id;			/*车道号*/
    char			cSOTNoEntryEnFlag;	/* 是否使能 */
    char 			cRsv[3];
    unsigned int 	iNEBeginTime1;
    unsigned int 	iNELastTime1;		/* 持续时间1 */
    unsigned int 	iNEBeginTime2;
    unsigned int 	iNELastTime2;		/* 持续时间2 */
    unsigned int 	iNEBeginTime3;
    unsigned int 	iNELastTime3;		/* 持续时间3 */
    char resv[128];						/* 保留字节 */
} DSP_SOT_NO_ENTRY_INFO;

//配置交通灯状态
typedef struct _TL_STATUS_INFO
{
    char cTLNum;	//交通灯编号
    char cTLStatus;	//交通灯状态
    char resv[6];
} TL_STATUS_INFO;

typedef struct _DEV_TL_STATUS_INFO
{
    TL_STATUS_INFO tlInfo[36];
    char resv[128];
} DEV_TL_STATUS_INFO;




typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int step1port;
    unsigned int step2port;
    char resv[64];
} Lane_Flash_Port_t;

typedef struct STR_DSP_PP_JPG_OSD_CONF
{
    unsigned int chan_id;			/*通道号*/
    unsigned char ucDevSnOsdEn;		/* 设备编号 */
    unsigned char ucPpNameOsdEn;		/* 卡口名 */
    unsigned char ucDirNameOsdEn;		/* 方向 */
    unsigned char ucLaneNameOsdEn;	/* 车道名 */

    unsigned char ucSnapTimeOsdEn;	/* 抓拍时间 */
    unsigned char ucCarDirOsdEn;		/* 车行方向 */
    unsigned char ucBusNumOsdEn;		/* 车牌号码 */
    unsigned char ucLPColorOsdEn;		/* 车牌颜色 */

    unsigned char ucCarTypeOsdEn;		/* 车型 */
    unsigned char ucCarColorOsdEn;	/* 车身颜色 */
    unsigned char ucCurSpeedOsdEn;	/* 车速 */
    unsigned char ucPunishSpeedOsdEn;	/* 处罚速度 */

    unsigned char ucMarkSpeedOsdEn;	/* 标示速度 */
    unsigned char ucSpeedRatioOsdEn;	/* 超速比例 */
    unsigned char ucReguCodeOsdEn;	/* 违章代码 */
    unsigned char ucStandReguDesOsdEn;/* 标准违章描述 */

    unsigned char ucIntelReguDesOsdEn;/* 内部违章描述 */
    char resv[127]; /* 保留字节 */
} DSP_PP_JPG_OSD_CONF;

typedef struct STR_DSP_EP_JPG_OSD_CONF
{
    unsigned int 	chan_id;			/*通道号*/
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
    char resv[125];	/* 保留字节 */
} DSP_EP_JPG_OSD_CONF;

typedef struct
{
    unsigned int lane_id;	/*车道号*/
    unsigned int color;		/*颜色*/
    char resv[64];
} JPG_OSD_COLOR_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int max_speed;	/*最大速度*/
    unsigned int min_speed;	/*最小速度*/
    char resv[64];
} LIMIT_SPEED_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int status;	/*状态值*/
    char resv[64];
} VENC_STATUS_PARAM;

typedef struct
{
    unsigned int lane_id;		/*车道号*/
    unsigned int bEnableFlag;	/*跟踪使能标志*/
    char resv[64];
} EP_CARTRACK_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int direction;	/*抓拍方向*/
    char resv[64];
} SNAP_DIRECTION_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int loop_mode;	/*算法检测方式*/
    char resv[64];
} ALG_LOOP_MODE_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int sec;		/*秒数*/
    unsigned int msec;		/*毫秒数*/
    char resv[64];
} NOTIFY_RED_LIGHT_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int bEnableFlag;	/*使能标志*/
    char resv[64];
} PP_VL_ENABLE_PARAM;

typedef struct
{
    unsigned int chan_id;
    unsigned int lane_id;
    char resv[64];
} VALID_LANE_NUM_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int debugEnable;	/*调试使能*/
    unsigned int x;		/*调试框X位置*/
    unsigned int y;		/*调试框Y位置*/
    char resv[64];
} PLATE_VIDEO_DEBUG_PARAM;

typedef struct
{
    unsigned int lane_id;	/*车道号*/
    unsigned int bRedFlag;	/*电子警察红灯标示*/
    char resv[64];
} EP_REDLIGHT_FLAG_PARAM;

typedef struct
{
    unsigned int chan_id;	/*通道号*/
    unsigned int lane_id;	/*车道号*/
    unsigned int detect_enable;	/*雷达检测使能*/
    char resv[64];
} RADAR_DETECT_PARAM;

typedef struct
{
    int    chan_id;
    int    lane_id;
    int	   iContinuSnapEn;
    char resv[64];
} FORCE_SNAP;

///{红灯检测接口定义
#define TL_YEL_TIM_MAX     10   /*黄灯时长最长不要超过10秒*/
#define TL_HGT_WTH_MAX     257  /*灯组长宽最大值*/
#define TL_HGT_WTH_MIN     8    /*灯组长宽最小值*/
#define TL_GRPS_MAX        6    /*最大灯组数*/
#define TL_LGTS_MAX        8    /*每个灯组最大灯数*/
#define TL_RED_LGTS_MAX    16   /*所有灯组中红灯个数*/

#define TL_LGT_CLR_OFF     0   /*无色/灯暗*/
#define TL_LGT_CLR_RED     1   /*红色*/
#define TL_LGT_CLR_YEL     2   /*黄色*/
#define TL_LGT_CLR_GRE     4   /*绿色*/

#define TL_LGT_SHP_ARR     1   /*箭头*/
#define TL_LGT_SHP_RND     2   /*圆饼*/
#define TL_LGT_SHP_NUM     4   /*数字*/
#define TL_LGT_SHP_BAR     8   /*条形*/
#define TL_LGT_SHP_ETC     0   /*其他*/

#define TL_LGT_ORT_LFT     1   /*向左*/
#define TL_LGT_ORT_STR     2   /*直行*/
#define TL_LGT_ORT_RGT     4   /*向右*/

#define TL_INIT_METH       0   /*调用初始化的方式:初始调用*/
#define TL_SERROI_MEHT     1   /*频繁调用*/

#define TL_SGL_ON          1   /*信号检测器表示红灯亮*/
#define TL_SGL_OFF         0   /*信号检测器表示红灯暗*/

/**/
typedef struct tagPotPos
{
    unsigned short usPointX;
    unsigned short usPointY;
} TL_PotPosS;

/**/
typedef enum tagGrpDir
{
    GRP_DIR_HOR = 0,
    GRP_DIR_VER = 1,
} TL_GrpDirE;

/**/
typedef struct tagTrcLgt
{
    unsigned char ucLgtClr;             /*灯颜色，第7位表示是否修复*/
    unsigned char ucLgtShp;             /*灯形状*/
    unsigned char ucLgtYelTime;         /*黄灯时间*/
    unsigned char ucLgtOrt;             /*灯指示方向*/

    unsigned char ucMapLgtNO;           /*红灯检测器号*/
    unsigned char ucRsv[15];
} TL_TrcLgtS;

/**/
typedef struct tagTrcGrp
{
    unsigned char ucLgtNums;                  /*该组中交通灯个数，包括(红、绿、黄、数字灯等等)*/
    char resv0[3];

    TL_PotPosS    stGrpTopLftPos;             /*该组交通灯左上坐标*/
    TL_PotPosS    stGrpBtmRgtPos;             /*该组交通灯右下坐标*/
    TL_GrpDirE    enGrpDir;                   /*灯组中灯的排列方向*/

    TL_TrcLgtS    astTrcLgt[TL_LGTS_MAX];     /*灯信息(方向、颜色、形状等)*/
    char resv[64];
} TL_TrcGrpS;


/**/
typedef struct tagTrcLgtsGrps
{
    unsigned char  enable;		/* 红灯检测算法使能 */
    unsigned char  bShowRect;	/* 是否显示红灯线圈 */
    unsigned char  ucGrpNums;	/* 有效灯组数 */
    unsigned char  ucSglUser;   /* 红灯检测方式(视频检测或者红灯检测器检测) */

	unsigned char ucUsedVDRL;	/* 是否使用车检器红灯 */
	unsigned char ucRLRevise;	/* 红灯修正使能标志 */
	unsigned char ucRLStrobe;	/* 频闪修正使能标志 */
	unsigned char ucRLSFrmNum;	/* 频闪修正帧数 */

	char cEraseHaloEnable;		/* 光晕修正使能 */
	char cEraseHaloPixels;		/* 消除光晕向外扩展像素点数 */
	char cTLParamSetEnable; 	/* 参数配置使能，若为0表明该组参数无效 */
	char cRedModifyMode;	 	/* 红灯修正方法:1：最旧版本 2：较旧版本 
												3：新方法（全自动）4：新方法（自动1）
												5：新方法（手动,不修白点）
												6：新方法（手动，修白点）*/

	char cRLAmendLevel;			/* 红灯修正深浅等级 */	
	char cRsv[15];

    TL_TrcGrpS astTrcGrp[TL_GRPS_MAX];	/* 所有红灯组的配置参数 */
    char resv[256];
} TL_TrcLgtsGrpsS;
///}

/* -------------------------------------------图片回调接口定义------------------------------------------------------------*/
typedef struct _SNAP_PIC_TIME
{
    unsigned int uiSec;
    unsigned int uiMSec;
} SNAP_PIC_TIME;

typedef struct tag_FACE_PIC_INFO_
{
    unsigned int uiPicAddrOffset;	/* 相对于缓存buf基址的偏移 */
    unsigned int uiPicLen;			/* 人脸图片大小 */
} STR_FACE_PIC_INFO;

/* ---------每张Jpeg图片内存划分 -----------

	jpeg头：	    JPEG_HEADER
	车辆检测头：	VEHICLE_DETECT_HEADER
	jpeg数据:
*/
typedef struct _JPEG_HEADER
{
    unsigned char 	snapType;           //抓拍类型
    unsigned char 	chn;		        //通道， 从0开始
    unsigned short  usPicDebugLen;		//由保留位替换为图片调试信息长度
    unsigned int 	uiMsSeconds;	    //时间毫秒
    unsigned int 	seconds;	        //抓拍时间
    unsigned int 	imgLen;		        //图片长度
    //unsigned int 	jpgDataOffset;      //jpg大图相对于缓存buf基址的偏移
    unsigned int 	snapGroupID;	    //第几组抓拍，从0开始累加
    unsigned int 	snapNum;	        //这组抓拍共有几张，从1开始
    unsigned int 	snapID;		        //这组抓拍的第几张，从0开始
} JPEG_HEADER;

typedef struct _VEHICLE_DETECT_HEADER
{
    unsigned char 	lane;		        //车道号，从0开始
    unsigned char 	confidence;	        //置信度， 虚拟线圈时：按检测结果填写；地感线圈：100
    unsigned char 	vehicleType;	    //是什么车
    unsigned char   DataType;     		//图片类型， 21   --- 电子警察闯红灯违章，22  --- 禁左违章， 23  --- 禁右违章，原来为保留字节
    unsigned int	maxspeed;		    //超速门限值
    unsigned int	speed;		        //速度
    unsigned int 	redLightSeconds;    //红灯开始时间秒
    unsigned int 	redLightMs;	        //红灯开始时间毫秒
    unsigned int 	redLightDuration;   //红灯持续时间
    RECT 		    rect;		        //最大感兴趣车牌区域即车道范围

    RECT    	    lpRect;             //定位出车牌的位置，以便于放大，没有填0
    char 		    lpStr[12];          //车牌号码
    unsigned int    LPREnable;          //车牌识别使能
    unsigned int    lpColor;            //车牌颜色
    unsigned int    lpType;             //车牌类型
    unsigned int    lpConfidenct;       //车牌置信度
    unsigned int    lpFCConfidenct;     //车牌首字符置信度

    SNAP_PIC_TIME	strSnapPicTime[6];	//抓拍图片中每一步对应的抓拍时间，按抓拍步骤排列

    /* 人脸检测相关结构体，若没有人脸检测，必须初始化为0 */
    unsigned char	ucFaceIdenEn;
    unsigned char	ucFaceSum;
    unsigned char 	ucFaceOvenlayEn;
    unsigned char 	ucSaveSmallPicEn;	/* 是否有小图片 */
    RECT			strFaceSearchArea;
    RECT			aStrFaceIden[MAX_FACE_NUM];

    STR_FACE_PIC_INFO aStrFacePicInfo[MAX_FACE_NUM];
    char 			cDriverTrack;               //行车轨迹
    char 			cRsv0[1];
    unsigned short  usSnapGroupId;              //地感线圈抓拍车辆编号
    unsigned int	uiRsv[5];                   //保留
} VEHICLE_DETECT_HEADER;


/* -------------------------------------------视频流回调接口定义------------------------------------------------------------*/

/* ---------H.264每帧内存划分 -----------
	编码参数头：    VENC_DATA_FIELD
	公司私有头定义: VIDEO_DATA_HEADER
	编码数据:
*/

/* 每帧数据头结构体定义 */
typedef  struct tagVENC_DATA_FIELD
{
    unsigned int flags: 16;
    unsigned int hdr_len: 8;
    unsigned int reserved: 8;

    unsigned int is_cbr: 1;
    unsigned int bitrate: 15;   /*kbps*/
    unsigned int is_pal: 1;
    unsigned int reserved2: 15;
    unsigned int seconds;       /*LocalTime*/
} VENC_DATA_FIELD, *PVENC_DATA_FIELD;

typedef struct tagVIDEO_DATA_HEADER
{
    unsigned int     headFlag;
    unsigned int     frameLength;

    unsigned char     frameType;
    unsigned char     frameRate;
    unsigned char      imgFmt;
    unsigned char     encType;
    unsigned int 	sequence;
    long long     timeStamp;
} VIDEO_DATA_HEADER, *PVIDEO_DATA_HEADER;


typedef struct
{
    int flag;
    int nal_count;
    int nal_length;
} NAL_HEADER;

typedef struct 
{ 
	unsigned int uiLane;           /* 该车属于哪个车道 */
	unsigned int uiSnapGroup;      /* 抓拍图片组号，与检测出的车辆编号保持一致且保证唯一 */ 
	unsigned int uiSnapPicSum;     /* 抓拍图片总张数 */ 
	unsigned int uiSnapSeconds;    /* 起始抓拍时间即第一步违章时间 秒数 */
	unsigned int uiSnapMSeconds;   /* 起始抓拍时间即第一步违章时间 毫秒数 */
	unsigned int uiIrregFlag;      /* 是否违章（在确定该车违章时为1，反之为0），即在判断该车确定违章时赋值，并且更新所有抓拍信息 */
	unsigned int uiRsv[3];         /* 保留 */
}EP_Alarm_Rec_Car_Info;

typedef struct 
{ 
    unsigned int uiFrameNum;            /* 帧号，可填可不填 */
    unsigned int iCarSum;                        /* 该帧有几辆违章车辆，最大支持6辆 */
    EP_Alarm_Rec_Car_Info aStrCarInfo[MAX_IRREG_CAR_NUM];
}EP_Alarm_Rec_Frame_Info;

#pragma pack (4)
typedef struct
{
	EP_Alarm_Rec_Frame_Info	Str_EpAlarmRecFrameInfo;
    VENC_DATA_FIELD venc_data_field;
    VIDEO_DATA_HEADER video_data_header;
    NAL_HEADER    nal_header;
} TRANSPORT_HEADER;
#pragma pack ()
/* -------------------------------------------报警信息回调接口------------------------------------------------------------*/


/* 流量统计上报结构体 */
typedef struct
{
    unsigned int chan_id; //通道号:从0开始
    unsigned int lane_id; //车道号,从0开始
    unsigned int begin_time; //开始时间，单位(s)
    unsigned int end_time; //结束时间，单位(s)
    unsigned int flow_num; //车流量
    unsigned int reserve[4];
} flowstat_status_t;

/* 视频丢失、运行检测等报警结构体 */
typedef struct
{
    unsigned int chan_id;       /* 通道号，从0开始*/
    unsigned int alarm_type;    /* 1 - video loss  2 - motion detect*/
    unsigned int status;        /* video_loss： 1-vloss,0-v exist;motion detect: 1-移动,0-未移动 */
} alarm_report_t;


/* 交通灯状态上报结构体 */
typedef enum _TL_STATUS
{
    e_INVALID_LIGHT = 0,
    e_RED_LIGHT = 1,
    e_GREEN_LIGHT = 2,
    e_YELLOW_LIGHT = 3,
    e_NONE_LIGHT = 4,
    e_ERROR_LIGHT = 5
} TL_STATUS;

typedef struct
{
    unsigned int index; 		//红灯检测器号
    unsigned int lane_id; 		//车道号,从0开始
    TL_STATUS  status;    		//交通灯状态
    unsigned int begin_time_s; 	//开始时间，单位(s)
    unsigned int begin_time_ms; //开始时间，单位(ms)
    unsigned int reserve[4];
} trafficlight_status_t;

typedef struct
{
	unsigned int lightnum;     //交通灯个数
	trafficlight_status_t LIGHT_STATUS[LIGHT_MAX_NUM];
}trafficlight_alarm_t;
/* 视频后处理相关参数配置
Members:

byBrightnessLevel
亮度，取值范围[0,100]

byContrastLevel
对比度，取值范围[0,100]

bySharpnessLevel
锐度，取值范围[0,6]

bySaturationLevel
饱和度，取值范围[0,100]

byHueLevel
色度，取值范围[0,100]

byGreyLevel
平均灰度，取值范围[0,255]

*/

typedef struct
{
    unsigned char byBrightnessLevel;  /* 亮度 */
    unsigned char byContrastLevel;    /* 对比度 */
    unsigned char bySharpnessLevel;   /* 锐度 */
    unsigned char bySaturationLevel;  /* 饱和度 */
    unsigned char byHueLevel;         /* 色度 */
    unsigned char byGreyLevel;        /* 平均灰度 */
    char byRes[30];
} NET_DEV_VIDEOEFFECT, *LPNET_DEV_VIDEOEFFECT;

/* 自动曝光结构体
Members
byExposureMode
0-自动，1-手动
byRes
保留
dwMaxExposureVal
最大曝光即最大快门值（单位us）
dwMinExposureVal
最小曝光即最小快门值(固定模式时，配置固定快门值)（单位us）
*/
typedef struct
{
    char  byExposureMode;
    char  byRes[3];
    int   dwMaxExposureVal;
    int   dwMinExposureVal;
    char  resv[32];
} NET_DEV_EXPOSURE, *LPNET_DEV_EXPOSURE;

/* 增益控制结构体
Members:
byGainMode
增益模式，0：自动，1：手动
byRes
保留，置为0
dwMaxGainValue
最大增益值，单位dB
dwMinGainValue
最小增益值(当为手动模式时，配置手动增益值)，单位dB
*/

typedef struct
{
    char byGainMode;
    char byRes[3];
    int  dwMaxGainValue;
    int  dwMinGainValue;
    char resv[32];
} NET_DEV_GAIN, *LPNET_DEV_GAIN;

/* 宽动态控制结构体
Members
byWDREnabled
宽动态是否启用，0-不启用，1-启用
byWDRLevel
0-3 (强、中、弱)
*/
typedef struct
{
    char  byWDREnabled;
    char  byWDRLevel;
    char  byRes[14];
} NET_DEV_WDR, *LPNET_DEV_WDR;

/* 2D降噪
Members
by2DNoiseRemoveEn
2D降噪是否启用，0-不启用，1-启用
by2DNoiseRemoveLevel
0-5 (自动强、自动中、自动弱、手动强、手动中、手动弱)
*/
typedef struct
{
    char  by2DNoiseRemoveEn;
    char  by2DNoiseRemoveLevel;
    char  byRes[14];
} NET_DEV_2D_NOISEREMOVE, *LPNET_DEV_2D_NOISEREMOVE;

/* 3D降噪
Members
By3DNoiseRemoveEn
3D降噪是否启用，0-不启用，1-启用
By3DNoiseRemoveLevel
0-5 (强、中、弱)
*/
typedef struct
{
    char  by3DNoiseRemoveEn;
    char  by3DNoiseRemoveLevel;
    char  byRes[14];
} NET_DEV_3D_NOISEREMOVE, *LPNET_DEV_3D_NOISEREMOVE;

/* ccd参数配置结构体
Members
dwSize
结构体大小
struVideoEffect
视频效果参数
struExposure
曝光参数
struGain
增益参数
struWdr
宽动态参数
struWhiteBalance
白平衡参数
2D降噪
str2DNoiseRemove
3D降噪
str3DNoiseRemove
*/
typedef struct
{
    int                     dwSize;   /* 结构体大小 */
    NET_DEV_VIDEOEFFECT     struVideoEffect;
    NET_DEV_EXPOSURE        struExposure;
    NET_DEV_GAIN            struGain;
    NET_DEV_WDR             struWdr;
    NET_DEV_2D_NOISEREMOVE  str2DNoiseRemove;
    NET_DEV_3D_NOISEREMOVE  str3DNoiseRemove;
    char resv[128];
} NET_DEV_CCDPARAMCFG,  *LPNET_DEV_CCDPARAMCFG;

typedef struct
{
    unsigned int feature_shutter_available;
    unsigned int feature_gain_available;
    unsigned int feature_bakclight_compensate_available;
    unsigned int feature_aperture_available;
    unsigned int image_width;
    unsigned int image_height;
    unsigned int totalpixels;

    //Camera Control Related Param
    float MinExposureValue;
    float MaxExposureValue;

    //Camera Control Related Param
    float MinShutterValue;
    float MaxShutterValue;

    float MinGainValue;
    float MaxGainValue;

    float MinBrightNessValue;
    float MaxBrightNessValue;

    float MinHueValue;
    float MaxHueValue;

    float MinSharpNessValue;
    float MaxSharpNessValue;

    float MinSaturationValue;
    float MaxSaturationValue;

    float MinGammaValue;
    float MaxGammaValue;

    float MinContrastValue;
    float MaxContrastValue;

    float MinIsoValue;
    float MaxIsoValue;

    int   CapBufCnt;

    char resv[128];
} cam_info_t;

typedef struct
{

	unsigned int cbSize;

    float fCurGain; //增益
    float fCurShutter; //快门
    float fCurGrey; //灰度


    unsigned int curluminance;         //亮度值
    unsigned int curContrast;		//对比度
 	unsigned int curSaturation;		//饱和度

	unsigned int curWdr;			//宽动态
	unsigned int curSharpness;		//锐度值
	unsigned int cur2D_noise;		//2d降噪
	unsigned int cur3D_noise;		//3d降噪

    short sI2CAddr;			//I2C反馈地址
	short sI2CData;			//I2C反馈数据
    unsigned int resversed[4];
}CUR_CAMERA_STATUS;

typedef struct
{
	unsigned int cbSize;
    unsigned int version;
    char compile_info[256];
    char ver_instr[128];
    char resv[128];
} version_info_t;

typedef enum _VER_TYPE_T
{
    e_RdkMcfw_type = 0,
    e_dsp_type,
    e_hardware_type,
    e_link_ep_type,
    e_link_lpr_type,
	e_link_osd_type,
	e_link_rlt_type
} VER_TYPE_T;

int NET_DEV_GET_Version(VER_TYPE_T version_type, version_info_t* param);

/* coreID: dsp 0  video 1 vpss 2 A8 3 */
int NET_DEV_GET_PRINT_LEVEL(int coreID, int level);

int ccdCamInfoGet(cam_info_t* pCamerainfo);

int ccd_Get_Cur_Cam_Param(CUR_CAMERA_STATUS * param);

/* -------------------------------------------错误码接口定义-------------------------------------------------------------*/

/* 返回最后操作的错误码。 */
int NET_DVE_GetLastError();


/* -------------------------------------------初始化及反初始化接口定义---------------------------------------------------*/

/* 初始化SDK，调用其他SDK函数的前提。下载M3、C64x程序，以默认参数创建电警Chain，为参数配置做好准备。*/
int  NET_CYC_Init();

/* 动态参数配置完后，开始工作 */
int  NET_CYC_Start();

/*停止工作*/
int  NET_CYC_Stop();


/* 释放SDK资源，在结束之前最后调用。 */
int  NET_CYC_Cleanup();


/* --------------------------------------------设置设备的配置信息---------------------------------------------------- */

int  NET_CYC_SetDEVConfig(
    int		dwCommand,
    void*	lpInBuffer,
    int		dwInBufferSize
);

/* 图片流回调接口定义 */
/* 回调函数 */

typedef void(*fJpegDataCallBack)(
    char*			pBuffer,
    unsigned int	dwBufSize,
    unsigned int	dwUser
);

int  NET_CYC_SetJpegDataCallBack(fJpegDataCallBack cbJpegDataCallBack, unsigned int dwUser);
/* 视频流回调接口定义 */
typedef void(*fRealDataCallBack)(
    int		iChannel,
    char*	pBuffer,
    unsigned int dwBufSize,
    unsigned int dwUser
);

int  NET_CYC_SetRealDataCallBack(
    int					iChannel,
    fRealDataCallBack	cbRealDataCallBack,
    int					dwUser
);

typedef void(*MSGCallBack)(
    int		iCommand,
    char*	pAlarmInfo,
    int		dwBufLen,
    void*	pUser
);

int NET_CYC_SetDevMessageCallBack(
    MSGCallBack	fMessageCallBack,
    void*		pUser
);

/* -------------------------------------------命令式接口------------------------------------------------------------*/

/* 手动抓拍接口函数
    iContinuSnapEn: 0-只抓拍一次；1-连接抓拍，抓拍间隔按实际配置参数为准
*/
//iSnapType  0  卡口抓拍   1 电警抓拍
int NET_DEV_ManualSnap(FORCE_SNAP* param, int  iSnapType);

/*-----------------------------------------设备能力查询-----------------------------------------------------------*/
int NET_DEV_Get_Ability(int dwAbilityType, char* PInBuf, int dwInLength, char* pOutBuf, int dwOutLength);


/*获取设备的配置信息
*/
int NET_CYC_GetDEVConfig(
    int   dwCommand,		//配置命令
    int   chan_id,			//不使用的时候设置成0xff
    int	  lane_id,			//不使用的时候设置成0xff
    void* lpOutBuffer,		//接收数据的缓冲指针
    int   dwOutBufferSize,	//接收数据的缓冲长度
    int*  lpBytesReturned	//实际收到的数据长度指针
);

int NET_CYC_SetDEVConfig_Test(int dwCommand, void* lpInBuffer, int dwInBufferSize);

#ifdef __cplusplus
}
#endif

#endif  /* _DM8127_DSP_SERVICE_H_*/
