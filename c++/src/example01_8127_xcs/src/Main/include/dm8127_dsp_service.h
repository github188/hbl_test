#ifndef _DM8127_DSP_SERVICE_H_
#define _DM8127_DSP_SERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

#define NET_DEV_SN_LENGTH	20            /*���Ӿ���򿨿����кų�������*/
#define MAX_JPEG_NUM		6
#define MAX_IRREG_CAR_NUM    6 			   /* һ֡���Υ�³������� */
#define LIGHT_MAX_NUM		3


#define MAX_DSP_IRREG_CODE_LEN (8)
#define MAX_DSP_IRREG_NAME_LEN (64)
#define MAX_FACE_NUM 5  /* ����������� */
/* ������Ϣ�ص��ӿ����� */
#define COMM_ALARM_PDC 1	/* �����ϱ� ��Ӧ�ṹ�� flowstat_status_t*/
#define COMM_ALARM_VL  2	/* ��Ƶ��ʧ���˶�����Ӧ�ṹ�� alarm_report_t */
#define COMM_ALARM_LS  3	/* ���״̬�ϱ���Ӧ�ṹ�� "trafficlight_status_t" */

/*--------------------------------------��������趨-------------------------------------*/
#define NET_DEV_NOERROR					0		/*û�д���*/
#define NET_DEV_SDK_INIT_ERROR			1		/*SDK������ʼ������*/
#define NET_DEV_SDK_START_ERROR			2		/*SDK�����������֮������ʧ��*/
#define NET_DEV_SDK_STOP_ERROR			3		/*SDKֹͣʧ��*/
#define NET_DEV_SDK_CLEAN_ERROR			4		/*SDK��Դ���ʧ��*/

#define NET_DEV_SET_CONFIG_SIZE_ERROR	5		/*�������ýṹ���С����*/
#define NET_DEV_SET_CONFIG_RESULT_ERROR	6		/*�������÷��ش���*/
#define NET_DEV_SET_CONFIG_PARAM_ERROR  7		/*�������ò�������*/

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



#define NET_DEV_SET_VENC_PARAM						0x0101		/*�����������---------venc_param_t*/
#define NET_DEV_SET_VENC_STATUS						0x0102		/*����״̬����---------VENC_STATUS_PARAM*/

#define NET_DEV_SET_OSD_CHANNAME					0x0201		/*osd���ӳ�����--------channame_param_t*/
#define NET_DEV_SET_OSD_TIME_TITLE					0x0202		/*osdʱ�����----------time_param_t*/
#define NET_DEV_SET_PP_OSD_STRING					0x0203		/*����osd�ַ�������--------osd_param_t*/
#define NET_DEV_SET_EP_OSD_SET_STRING				0x0204		/*���Ӿ�����Ļ���ýӿ�--------osd_param_t*/
#define NET_DEV_SET_PP_ROAD_DIRECTION_OSD			0x0205		/*����·�ڷ���osd����------osd_param_t*/
#define NET_DEV_SET_PP_ROAD_OSD						0x0206		/*����·����osd����------osd_param_t*/
#define NET_DEV_SET_PP_ROAD_DESC_OSD				0x0207		/*����·������osd����-----osd_param_t*/
#define NET_DEV_SET_PP_LANE_OSD						0x0208		/*���ڳ�����osd����------osd_param_t*/
#define NET_DEV_SET_EP_ROAD_DIRECTION_OSD			0x0209		/*�羯·�ڷ���osd����------osd_param_t*/
#define NET_DEV_SET_EP_ROAD_OSD						0x0210		/*�羯·����osd����------osd_param_t*/
#define NET_DEV_SET_EP_ROAD_DESC_OSD				0x0211		/*�羯·������osd����-----osd_param_t*/
#define NET_DEV_SET_EP_LANE_OSD						0x0212		/*�羯������osd����------osd_param_t*/
#define NET_DEV_SET_PP_JPG_OSD_CONF					0x0213		/*����ͼƬosd����������Ϣ----DSP_PP_JPG_OSD_CONF*/
#define NET_DEV_SET_EP_JPG_OSD_CONF					0x0214		/*�羯ͼƬosd����������Ϣ-----DSP_EP_JPG_OSD_CONF*/
#define NET_DEV_SET_JPG_OSD_COLOR					0x0215		/*����ͼƬ��Ļ��ɫ-------JPG_OSD_COLOR_PARAM*/

#define NET_DEV_SET_VILIB_PARAM						0x0301		/*������Ȧ���������------VILib_feature_t*/
#define NET_DEV_SET_VIRTUAL_LOOP					0x0302		/*��������Ȧλ��-------VirtualLoop_feature_t*/
#define NET_DEV_SET_GT5000_SNAPSHOT_SIZE_SET		0x0303		/*���Ӿ����趨ץ������---------HSLRECT*/
#define NET_DEV_SET_GT5000_SHINE_OR_NOT				0x0304		/*������Ƿ���-------------GT5000_SHINEMODE_t*/
#define NET_DEV_SET_LIMIT_SPEED						0x0305		/*�����޶��ٶ�--------LIMIT_SPEED_PARAM*/
#define NET_DEV_SET_CAR_FLOWSTAT					0x0306		/*���ó�����ͳ����Ϣ---------FlowStatInfo*/
#define NET_DEV_SET_FORBID_DIRC						0x0307		/*���ó�����ֹͨ�з���-------forbid_direct_param*/
#define NET_DEV_SET_LANE_FORBID_ATTRIBUTE			0x0308		/*���ó�����ֹͨ������-------lane_attribut_info_t*/
#define NET_DEV_SET_CARTRACK_ENABLE					0x0309		/*���Ӿ���켣����ʹ��-----EP_CARTRACK_PARAM*/
#define NET_DEV_SET_SNAP_DIRECTION					0x0310		/*����ץ�ķ���-----SNAP_DIRECTION_PARAM*/
#define NET_DEV_SET_LOOP_MODE						0x0311		/*�㷨���ģʽ------ALG_LOOP_MODE_PARAM*/
#define NET_DEV_SET_VEHICLE_DETECT_TYPE				0x0312		/*������ⷽʽ-------VEHICLE_DETECT_TYPE*/
#define NET_DEV_SET_FORCE_KEY_FRAME					0x0313		/*ǿ�ƹؼ�֡--------int chan_id*/
#define NET_DEV_SET_VALID_LANE_NUM					0x0314		/*������Ч������-----VALID_LANE_NUM_PARAM*/
#define NET_DEV_SET_LICENCE_PLATE_RECOGNIZE			0x0315		/*����ʶ����Ϣ����--------LICENCE_PLATE_INFO*/
#define NET_DEV_SET_PLATE_VIDEO_DEBUG				0x0316		/*���Ƶ��Խӿ�------PLATE_VIDEO_DEBUG_PARAM*/
#define NET_DEV_SET_VIDEO_SPEED_MEASURE				0x0317		/*��Ƶ����-------SPEED_VISUAL_DETECTION*/
#define NET_DEV_SET_TIME_PERIOD_DIRECTION			0x0318		/*��ʱ��ε���ץ������------TIME_PERIOD_DIRECTION*/
#define NET_DEV_SET_STOP_ON_NO_ENTRY				0x0319		/*·��������������---------DSP_SOT_NO_ENTRY_INFO*/
#define NET_DEV_SET_TL_STATUS_INFO					0x0320		/*��ͨ��״̬��Ϣ-------DEV_TL_STATUS_INFO*/

#define NET_DEV_SET_EP_VI_DETECT_TYPE				0x0371		/*���Ӿ������ü�ⷽʽ---------EP_VEHICLE_DETECT_TYPE*/
#define NET_DEV_SET_EP_VIRTUAL_LOOP					0x0372		/*���õ��Ӿ���ģʽ������Ȧ-------AREARECT*/
#define NET_DEV_SET_EPVL_OFFSET						0x0373		/*����������Ȧ���Ӿ���ץ�ĵ�0���͵�2�����1��֮��Ĳ���-----vlep_offset_param*/
#define	NET_DEV_SET_EPVL_FEATURE					0x0374		/*����������Ȧ���ƺ�����������ز���------EPVLFeature*/
#define NET_DEV_SET_EPVL_LANE_ND_RED_LIGHT			0x0375		/*���ó���������ʻ�ķ���ͺ��״̬------lane_nd_red_light_flag*/
#define NET_DEV_SET_EPVL_LANE_ASSIST_DETECT_PARAM	0x0376		/*�����羯�㷨����������-----ep_assist_detect_param*/
#define NET_DEV_SET_EP_IRREG_INFO					0x0377		/*���Ӿ���Υ����Ϣ--------DSP_EP_IRREG_INFO*/
#define NET_DEV_SET_EP_SN							0x0378		/*���Ӿ���SN-----char *pSN*/
#define NET_DEV_SET_EP_LPR							0x0379		/*���Ӿ��쳵��ʶ��------LICENCE_PLATE_INFO*/

#define NET_DEV_SET_PPVL_ENABLE						0x03c1		/*����������Ȧʹ��------PP_VL_ENABLE_PARAM*/
#define NET_DEV_SET_PP_SNAP_TARGET					0x03c2		/*����ץ��Ŀ��---------PPVL_SNAP_TARGET_INFO*/
#define NET_DEV_SET_PPVL_SNAPSTEP_INFO				0x03c3		/*��������������Ȧץ������------PPVL_SNAP_STEPS_INFO*/
#define NET_DEV_SET_PP_DEBUG						0x03c4		/*����ץ���������--------PPVL_SNAP_DEBUG_INFO*/
#define NET_DEV_SET_PP_IRREG_INFO					0x03c5		/*����Υ����Ϣ---------DSP_PP_IRREG_INFO*/
#define NET_DEV_SET_PP_SN							0x03c6		/*����SN---------char *pSN*/


#define NET_DEV_SET_NOTIFY_RED_LIGHT				0x0401		/*��ƿ�ʼʱ��---------NOTIFY_RED_LIGHT_PARAM*/
//#define NET_DEV_SET_TRAFFICLIGHT_COLOR_THRESHOLD	0x0402		/*���ú��̵Ʒ�ֵ---------TRAFFICLIGHT_COLOR_THRESHOLD*/
//#define NET_DEV_SET_TRAFFICLIGHT_RECT				0x0403		/*���ú��̵�����----------TrafficLightRect*/
#define NET_DEV_SET_EP_REDLIGHT_FLAG				0x0404		/*���õ��Ӿ����Ʊ�ʾ---------EP_REDLIGHT_FLAG_PARAM*/
//#define NET_DEV_SET_REVISE_REDLIGHT_DEGREE			0x0405		/*��������̶�------int degree*/
#define NET_DEV_SET_TLGRPS_CFG						0x0406

#define NET_DEV_SET_RADAR_DETECT_ENABLE				0x0501		/*�״���ʹ��------RADAR_DETECT_PARAM*/
//#define NET_DEV_SET_FLASH_PORT						0x0502	/*���������------Lane_Flash_Port_t*/

#define NET_DEV_SET_GET_VERSION						0x0601		/*��ȡ�汾��Ϣ*/

#define NET_DEV_SET_CCDPARAMCFG  					0x0701 		/* CCD�������� ---- NET_DEV_CCDPARAMCFG */
#define NET_DEV_SET_I2CPARAM						0x0702		/*I2C�������� -----char * I2C_PARAM_T*/

#define NET_DEV_SET_TIME							0x0801		/*����DSPʱ�� ------ int seconds*/

//��ȡ������Ϣ��cmd����
#define NET_DEV_GET_CCDPARAMCFG						0x1701		/*��ȡCCD��������-------CUR_CAMERA_STATUS*/

typedef struct
{
    unsigned int chan_id;
    unsigned int lane_id;
    unsigned int step0;		//step0 ��step1֮���֡��
    unsigned int step2;		// step1 �� step2֮���֡��
    char resv[64];
} vlep_offset_param;

typedef struct
{
    int	 chan_id;			/*ͨ����*/
    int  lane_id;			/*������*/
    int  ThrDayToNight;		//����תΪҹ��Ĳο�����
    int  ThrNightToDay;		//ҹ��תΪ����Ĳο�����
    int  corrNightThresh;	// ҹ�����ֵ����
    int  corrDayThresh;		// �������ֵ����
    int  corrMotoThresh;	// Ħ�г����ֵ����
    int  corrEveningThresh;	//�ƻ�����ֵ --�ο�ֵ20
    int  dayMotorWidth;
    int  nightMotorWidth;
    int  capDelay;
    int  minPlateWidth;
    int  maxPlateWidth;
    char resv[128];
} VILib_feature_t;

/*
 * ����������
 */
typedef struct
{
    unsigned int  chan_id;		/*ͨ����*/
    unsigned int  encType;      /*��������*/
    unsigned int  imgFmt;       /*�����ʽ*/
    unsigned int  keyIntv;      /*I֡���*/
    unsigned int  bitRate;      /*����kbps*/
    unsigned int  frameRate;    /*֡��*/
    unsigned int  nEncQuality;  /*��������*/
    unsigned int  nMotionDetect;
    unsigned int  is_cbr;
    char resv[128];
} venc_param_t ;

/*
 * ͨ�����Ʋ���
 */
typedef struct
{
    unsigned int chan_id;  /*ͨ����*/
    unsigned int startX;
    unsigned int startY;
    unsigned int width;
    unsigned int height;
    unsigned int name[16];
    char resv[128];
} channame_param_t;

/*
 *��Ļ������Ϣ
*/
typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;
    unsigned int startX;
    unsigned int startY;
    unsigned int width;
    unsigned int height;
    unsigned int name[16];
    char resv[128];
} osd_param_t;

/*
 * ͨ��ʱ������
 */
typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int startX;	/*4CIF����*/
    unsigned int startY;	/*4CIF����*/
    char resv[64];
} time_param_t ;

typedef struct
{
    int	 chan_id;	/*ͨ����*/
    int  lane_id;   /*������*/
    int  bShowFlag;	/*��ʾ��־*/
    int  x1;		//��
    int  x2;		//��
    int  y1;		//��
    int  y2;		//��
    char resv[64];
} VirtualLoop_feature_t;

typedef struct
{
    int chan_id;	/*ͨ����*/
    int lane_id;	/*������*/
    int startX;
    int startY;
    int width;
    int height;
    int bShow;		//�Ƿ���ʾ��Ȧ��1:��ʾ��0:����ʾ
    char resv[64];
} HSLRECT;

// ���Ӿ��� �������
typedef struct _THRESHOLD
{
    int max;
    int min;
} THRESHOLD;

typedef struct _COLOR_THRESHOLD
{
    THRESHOLD rthr;	//��
    THRESHOLD gthr;	//��
    THRESHOLD bthr;	//��
    char resv[16];
} COLOR_THRESHOLD;

typedef struct _TRAFFICLIGHT_COLOR_THRESHOLD
{
    int		chan_id;    /*ͨ����*/
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
    int chn;				//ͨ���ţ�
    char index;				//��ͨ�����ţ�
    char bShow;				//�Ƿ���ʾ��ͨ�ƿ�    1:��ʾ��Ȧ��0:����ʾ��Ȧ
    char bTrafficRevise;	//�Ƿ���������        1:������0:���޸�
    char bTrafficDetect;	//�Ƿ���Ƶ�����̵�:    1:��⣬0:�����
    RECT rect[4];
    char bTrafficStrobe;	//�Ƿ�Ƶ������,1:����,0:������
    char bFreqFrames;		//Ƶ������֡������
    char bSingleColorTrafficLight;	//�Ƿ��ǵ�һ��ɫ��ͨ�ƣ�0:���̻������������Ľ�ͨ��,1:���̻ƹ���һ����ͨ��
    unsigned char bRedLightS;	//��ͨ����Ϊ���Ͷȣ�Խ��Խ��
    unsigned char bRedLightV;	//��ͨ����Ϊ���ȣ�Խ��Խ��
    unsigned char ucMapRLNum;	//��Ӧ��Ƽ����
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
    e_INNERVATION_LOOP = 0, //������Ȧ��ʽ
    e_VIRTUAL_LOOP,  //������Ȧ��ʽ
    e_DISABLE_LOOP,	 //��ֹ������Ȧ���
} Vehicle_Detect_type_t;

/*�����������*/
typedef struct
{
    int chan_id;
    int lane_id;
    Vehicle_Detect_type_t detect_type;
    char resv[64];
} VEHICLE_DETECT_TYPE;

/*EP�����������*/
typedef struct
{
    int lane_id;
    Vehicle_Detect_type_t detect_type;
    int bayonetSnap;
    char resv[64];
} EP_VEHICLE_DETECT_TYPE;

//�����
typedef struct
{
    int X;
    int Y;
} COPOINT;

//��������
typedef struct
{
    int chan_id;	/*ͨ����*/
    int bShowFlag;	/*��ʾ��־*/
    int snapIntv;	/*ץ�ļ��֡��*/
    int snapType;	/*ץ������ ͼƬ�ĺϳɷ�ʽ*/
    COPOINT  Up0;
    COPOINT  Up1;
    COPOINT  Bottom0;
    COPOINT  Bottom1;
    COPOINT  Leave0; //�뿪ץ��������ߵ�
    COPOINT  Leave1; //�뿪ץ���������ұߵ�
    char resv[128];
} AREARECT;


typedef struct
{
    unsigned int chan_id;		//ͨ����,����Ĭ��Ϊ0
    unsigned int lane_id;		//������,��0��ʼ
    unsigned int report_flag;	//�Ƿ��ϴ�������ͳ��,1-�ϴ���0-���ϴ���
    unsigned int inter_time;	//�ϴ�������ʱ����(��λ:S)
    unsigned char resv[64];		//����
} FlowStatInfo;

typedef struct
{
    unsigned int lane_id;		/*ͨ����*/
    unsigned int forbid_direct;	/*��ֹ��ʽ*/
    unsigned int groupId;		//ÿ�����������飬���0��1��2��3
    unsigned int flag1;			//0:ȡ����ֹ��1:������ֹ
    unsigned int begin1_time;	//��ʼʱ��(��λ:s)
    unsigned int last1_time;	//����ʱ��(��λ:s)
    unsigned int flag2;			//0:ȡ����ֹ��1:������ֹ
    unsigned int begin2_time;	//��ʼʱ��(��λ:s)
    unsigned int last2_time;	//����ʱ��(��λ:s)
    char resv[128];
} forbid_direct_param;

/* ������ʻ������ */
typedef struct
{
    unsigned int lane;
    unsigned int direction; //direction������ϡ�
    char resv[64];
} lane_attribut_info_t;


typedef struct
{
    unsigned int lane; //������
    unsigned int minPlateWidth; //��С���ƿ��
    unsigned int maxPlateWidth; //����ƿ��
    unsigned int dayToNightThr; //����ת��������
    char resv[64]; //����
} EPVLFeature;

/*������Ȧ��ץ��Ŀ��*/
typedef enum _EN_SNAP_TARGET
{
    WholeVehicle = 1,       //��������
    AutoMobile = 2,         //������
    MotoBike = 3            //�ǻ�����
} EN_SNAP_TARGET;

typedef struct _PPVL_SNAP_TARGET_INFO
{
    unsigned int    chan_id;
    unsigned int    lane_id;
    EN_SNAP_TARGET  daySnapTarget;           /*����ץ��Ŀ��*/
    EN_SNAP_TARGET  nightSnapTarget;         /*����ץ��Ŀ��*/
    unsigned int    beginTime;               /*��ʼʱ��(��)*/
    unsigned int    durationTime;            /*����ʱ��(��)*/
    EN_SNAP_TARGET  durationTarget;          /*����ʱ���ڵ�ץ��Ŀ��*/
    unsigned int    ifPlateLighted;          /*�����Ƿ�����0��ʾδ������1��ʾ����*/
    char resv[64];
} PPVL_SNAP_TARGET_INFO;

/*����������Ȧץ�Ĳ���*/
typedef struct _PPVL_SNAP_STEPS_INFO
{
    unsigned int chan_id;
    unsigned int lane_id;
    unsigned int SnapStepsAmount;           //����ץ��Ϊ1������ץ��Ϊ2
    unsigned int IntervalTime;              //����ץ��ģʽ�£�����ץ�ĵ�ʱ����(����)
    char resv[64];
} PPVL_SNAP_STEPS_INFO;

typedef struct _LICENCE_PLATE_INFO
{
    unsigned int chan_id;			/*ͨ����*/
    unsigned int enable;               //����ʶ��ʹ�ܱ�־��0 ��ʹ�ܣ�1 ʹ��
    unsigned int firstCharConf;        //���ַ����Ŷ�
    unsigned int defaultFirstChar;     //Ĭ�����ַ�
    unsigned short usLPRWidth;			/* ����ʶ���� */
    unsigned short usLPRHeight;			/* ����ʶ��߶� */
    char resv[64];
} LICENCE_PLATE_INFO;

/*��Ƶ���ٽṹ��Ķ�*/
typedef struct _SPEED_VISUAL_DETECTION
{
    unsigned int chan_id;				/*ͨ����*/
    unsigned int enable;                    //ʹ�ܱ�־��1=ʹ�ܣ�0=��ʹ��
    unsigned int maxSpeed;                  //����maxSpeed��Ϊ������ʻ
    unsigned int punishSpeed;               //������ֵ������д���
    unsigned short actualDistance;          //������Ȧ����Ļ�Ϸ���Ӧ��ʵ�ʾ���(����)
    char resv[66]; //�����ֽ�
} SPEED_VISUAL_DETECTION;

typedef struct
{
    unsigned char ucNormalDirection;	//��������������ʻ����
    unsigned char ucRedLightFlag; 		//�÷���ĺ�Ʊ�ʾ
    unsigned char ucAidRedLightFlag;	//������ͨ�ƣ������жϴ�����
    char resv[13];
} normal_direction_red_light_flag;

typedef struct
{
    unsigned int uiLane; //������
    normal_direction_red_light_flag strNDRedLight[7]; //����������ʻ�ķ���ͺ��״̬
    char resv[128];
} lane_nd_red_light_flag;



typedef struct
{
    unsigned int uiLane; //�����ţ���0��ʼ���
    unsigned int uiFrmDiffEnable; //�Ƿ���֡����
    unsigned int uiStopDistance; //������ײ���ͣ���ߵ�ˮƽ����
    unsigned char ucOneLampEnable; //�����Ƽ��ʹ��
    unsigned char ucRedToGreenSnap; //��ת�̺��ץ��ʹ��
    unsigned char ucNoDirectionSnap; //û�м�⵽��������ץ��ʹ��ʹ��
    unsigned char ucEPoliceDebug; 	//�羯�����ʹ��
    unsigned char ucReverseSnap;	//����ץ��ʹ��
    unsigned char ucRideLineSnap; 	//����ץ��ʹ��
    unsigned char ucKakouFlash; 	//��������ʹ��
    unsigned char ucKakouSnapDelay[4]; //����ץ���ӳ�֡�����ֱ������1~4
    unsigned char ucMainPicStep;	//��дͼƬ�뿨��ץ�Ĳ���,0��ʾ��0����1��ʾ��1��

    unsigned char ucOsdSpeed; 		//�Ƿ�����ٶ�

    unsigned char ucKakowPicMode;	//����ͼƬ�ϳ�ģʽ(0:ȫ��+��д;1:ȫͼ)
    unsigned short usRedLReviseShutter ;//����������ŷ�ֵ �����ڸÿ���ʱ���������
    unsigned char ucPlateSegFlag; 	//���Ʒָ��ʾ
    unsigned char ucPicQuality; 	//ͼƬ����������ֵ��Χ(1~100)��ֵԽ��ͼƬЧ��Խ��
    unsigned char ucRedLReviseDegree; //���Ŵ������ÿ���ֵ��������̶�
    unsigned char ucChangeRoad;			//�Ƿ�֧�ֱ��ץ��
    char cLaneChgType[4];			//���������������ץ�����ã�0:���б����ץ�ģ�1:ֻץ����������2:ֻץ�����ұ��

    unsigned char ucPicAssistDebug;  //ͼƬ������Ϣ�Ƿ�ʹ��
    unsigned char	ucCrossLineParkEn;		//�Ƿ�ʹ��Խ��ͣ��Υ��ץ��
    unsigned char ucResever[2];

    unsigned char ucResever2[3];
    unsigned char aucPressLine[5];	 //�������Ƿ�ץ������ʹ�ܣ������±꼴Ϊ�����߱��

    char resv[128];
} ep_assist_detect_param;

typedef struct _PPVL_SNAP_DEBUG_INFO
{
    unsigned int    chan_id;
    unsigned int    lane_id;
    unsigned char   isOsdPlateScope;	//�Ƿ���ͼƬ�ϵ��ӳ���ʶ������
    unsigned char   isShieldReverse;	//�Ƿ��ϱ�����ͼƬ(Ĭ��ֵ1)
    unsigned short  plateScopeIncSize;	//(������㷨����)���Ʒ�Χ�����ӵ�ֵ
    unsigned char   radarSpeedGap;		//�״���ֵ(������϶���֡���ٶȺ������ٶ�ʱ�϶�����һ����)
    unsigned char   radarForwardLim;	//�״��ٶ���ǰ��Χ(�ٶ���ǰ����ͬ��������ֵ)
    unsigned char   radarBackwardLim;	//�״��ٶ����Χ(�ٶ��ӳٱ���ͬ��������ֵ)

    unsigned char   isDecTaillight;		//�Ƿ�ʹ��β�Ƽ��

    unsigned char	ucFaceIdenEn;		//����ʶ��ʹ��
    unsigned char	ucFaceOvenlayEn;	//���������Ƿ�ʹ��
    char resv[126];		//�����ֽ�
} PPVL_SNAP_DEBUG_INFO;

typedef enum _EP_IRREG_CODE
{
    e_RedCode = 1, 						/* �����Υ�´��� */
    e_ReverseCode = 2, 					/* ����Υ�´��� */
    e_InfringForbidLineCode = 3,		/* Υ����ֹ������ʻΥ�´��� */
    e_InfringDirLaneRunCode = 4, 		/* �������򳵵���ʻΥ�´��� */
    e_StopOnTrafficNoEntryRunCode = 5,	/* ·�����£���ֹʻ��Υ�´��� */
    e_dspHighwayInfringDirLaneCode = 6, /* ����·�������򳵵���ʻΥ�´��� */
} EP_IRREG_CODE;

typedef struct STR_DSP_EP_IRREG_INFO
{
    int iDispFlag;					/* Υ�������Ƿ���ӵ�ͼƬ�� */
    EP_IRREG_CODE epIrregCode;		/*Υ�´�����*/
    int iReserve[2];
    char szIrregCode[MAX_DSP_IRREG_CODE_LEN]; /* �����Υ�´��� */
    char szIrregName[MAX_DSP_IRREG_NAME_LEN]; /* �����Υ������ */
    char resv[128];
} DSP_EP_IRREG_INFO;



typedef enum _PP_IRREG_CODE
{
    e_OverSpeedSmallPercent50Code = 1,	/* ����С��50% */
    e_OverSpeedLargePercent50Code = 2,	/* ���ٴ���50% */
    e_ReverseRunCode = 3,				/* ���� */
    e_InfringeProhibCode = 4,			/* Υ�������־ */
} PP_IRREG_CODE;

typedef struct STR_DSP_PP_IRREG_INFO
{
    PP_IRREG_CODE epIrregCode;		/*Υ�´�����*/
    int iReserve[2];
    char szIrregCode[MAX_DSP_IRREG_CODE_LEN];
    char szIrregName[MAX_DSP_IRREG_NAME_LEN];
    char resv[128];
} DSP_PP_IRREG_INFO;

typedef struct
{
    int 			lane_id;			/*������*/
    char			cSnapDir;			/* ץ�ķ���(0:����;1:����) */
    char			cSDEnFlag;			/* �Ƿ�ʹ�� */
    char			cTruckEnFlag;		/* �����ʱ����Ƿ���Ч */
    char 			cRsv[1];
    unsigned int 	iSDBeginTime1;
    unsigned int 	iSDLastTime1;		/* ����ʱ��1 */
    unsigned int 	iSDBeginTime2;
    unsigned int 	iSDLastTime2;		/* ����ʱ��2 */
    unsigned int 	iSDBeginTime3;
    unsigned int 	iSDLastTime3; 		/* ����ʱ��3 */
    char resv[128];						/* �����ֽ� */
} TIME_PERIOD_DIRECTION;

typedef struct
{
    unsigned int 	lane_id;			/*������*/
    char			cSOTNoEntryEnFlag;	/* �Ƿ�ʹ�� */
    char 			cRsv[3];
    unsigned int 	iNEBeginTime1;
    unsigned int 	iNELastTime1;		/* ����ʱ��1 */
    unsigned int 	iNEBeginTime2;
    unsigned int 	iNELastTime2;		/* ����ʱ��2 */
    unsigned int 	iNEBeginTime3;
    unsigned int 	iNELastTime3;		/* ����ʱ��3 */
    char resv[128];						/* �����ֽ� */
} DSP_SOT_NO_ENTRY_INFO;

//���ý�ͨ��״̬
typedef struct _TL_STATUS_INFO
{
    char cTLNum;	//��ͨ�Ʊ��
    char cTLStatus;	//��ͨ��״̬
    char resv[6];
} TL_STATUS_INFO;

typedef struct _DEV_TL_STATUS_INFO
{
    TL_STATUS_INFO tlInfo[36];
    char resv[128];
} DEV_TL_STATUS_INFO;




typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int step1port;
    unsigned int step2port;
    char resv[64];
} Lane_Flash_Port_t;

typedef struct STR_DSP_PP_JPG_OSD_CONF
{
    unsigned int chan_id;			/*ͨ����*/
    unsigned char ucDevSnOsdEn;		/* �豸��� */
    unsigned char ucPpNameOsdEn;		/* ������ */
    unsigned char ucDirNameOsdEn;		/* ���� */
    unsigned char ucLaneNameOsdEn;	/* ������ */

    unsigned char ucSnapTimeOsdEn;	/* ץ��ʱ�� */
    unsigned char ucCarDirOsdEn;		/* ���з��� */
    unsigned char ucBusNumOsdEn;		/* ���ƺ��� */
    unsigned char ucLPColorOsdEn;		/* ������ɫ */

    unsigned char ucCarTypeOsdEn;		/* ���� */
    unsigned char ucCarColorOsdEn;	/* ������ɫ */
    unsigned char ucCurSpeedOsdEn;	/* ���� */
    unsigned char ucPunishSpeedOsdEn;	/* �����ٶ� */

    unsigned char ucMarkSpeedOsdEn;	/* ��ʾ�ٶ� */
    unsigned char ucSpeedRatioOsdEn;	/* ���ٱ��� */
    unsigned char ucReguCodeOsdEn;	/* Υ�´��� */
    unsigned char ucStandReguDesOsdEn;/* ��׼Υ������ */

    unsigned char ucIntelReguDesOsdEn;/* �ڲ�Υ������ */
    char resv[127]; /* �����ֽ� */
} DSP_PP_JPG_OSD_CONF;

typedef struct STR_DSP_EP_JPG_OSD_CONF
{
    unsigned int 	chan_id;			/*ͨ����*/
    unsigned char   ucDevSnOsdEn;		/* �豸��� */
    unsigned char	ucRoadNameOsdEn;	/* ·���� */
    unsigned char	ucDirNameOsdEn;		/* ���� */
    unsigned char	ucLaneNameOsdEn;	/* ������ */

    unsigned char	ucSnapTimeOsdEn;	/* ץ��ʱ�� */
    unsigned char	ucBusNumOsdEn;		/* ���ƺ��� */
    unsigned char	ucLPColorOsdEn;		/* ������ɫ */
    unsigned char	ucCarTypeOsdEn;		/* ���� */

    unsigned char	ucCarColorOsdEn;	/* ������ɫ */
    unsigned char	ucReguCodeOsdEn;	/* Υ�´��� */
    unsigned char	ucStandReguDesOsdEn;/* ��׼Υ������ */
    unsigned char	ucIntelReguDesOsdEn;/* �ڲ�Υ������ */

    unsigned char	ucRDBeginTimeOsdEn; /* ��ƿ�ʼʱ�� */
    unsigned char	ucRDCycleOsdEn;		/* ������� */
    unsigned char	ucRoadDespOsdEn;	/* ·������ */
    char resv[125];	/* �����ֽ� */
} DSP_EP_JPG_OSD_CONF;

typedef struct
{
    unsigned int lane_id;	/*������*/
    unsigned int color;		/*��ɫ*/
    char resv[64];
} JPG_OSD_COLOR_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int max_speed;	/*����ٶ�*/
    unsigned int min_speed;	/*��С�ٶ�*/
    char resv[64];
} LIMIT_SPEED_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int status;	/*״ֵ̬*/
    char resv[64];
} VENC_STATUS_PARAM;

typedef struct
{
    unsigned int lane_id;		/*������*/
    unsigned int bEnableFlag;	/*����ʹ�ܱ�־*/
    char resv[64];
} EP_CARTRACK_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int direction;	/*ץ�ķ���*/
    char resv[64];
} SNAP_DIRECTION_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int loop_mode;	/*�㷨��ⷽʽ*/
    char resv[64];
} ALG_LOOP_MODE_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int sec;		/*����*/
    unsigned int msec;		/*������*/
    char resv[64];
} NOTIFY_RED_LIGHT_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int bEnableFlag;	/*ʹ�ܱ�־*/
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
    unsigned int chan_id;	/*ͨ����*/
    unsigned int debugEnable;	/*����ʹ��*/
    unsigned int x;		/*���Կ�Xλ��*/
    unsigned int y;		/*���Կ�Yλ��*/
    char resv[64];
} PLATE_VIDEO_DEBUG_PARAM;

typedef struct
{
    unsigned int lane_id;	/*������*/
    unsigned int bRedFlag;	/*���Ӿ����Ʊ�ʾ*/
    char resv[64];
} EP_REDLIGHT_FLAG_PARAM;

typedef struct
{
    unsigned int chan_id;	/*ͨ����*/
    unsigned int lane_id;	/*������*/
    unsigned int detect_enable;	/*�״���ʹ��*/
    char resv[64];
} RADAR_DETECT_PARAM;

typedef struct
{
    int    chan_id;
    int    lane_id;
    int	   iContinuSnapEn;
    char resv[64];
} FORCE_SNAP;

///{��Ƽ��ӿڶ���
#define TL_YEL_TIM_MAX     10   /*�Ƶ�ʱ�����Ҫ����10��*/
#define TL_HGT_WTH_MAX     257  /*���鳤�����ֵ*/
#define TL_HGT_WTH_MIN     8    /*���鳤����Сֵ*/
#define TL_GRPS_MAX        6    /*��������*/
#define TL_LGTS_MAX        8    /*ÿ������������*/
#define TL_RED_LGTS_MAX    16   /*���е����к�Ƹ���*/

#define TL_LGT_CLR_OFF     0   /*��ɫ/�ư�*/
#define TL_LGT_CLR_RED     1   /*��ɫ*/
#define TL_LGT_CLR_YEL     2   /*��ɫ*/
#define TL_LGT_CLR_GRE     4   /*��ɫ*/

#define TL_LGT_SHP_ARR     1   /*��ͷ*/
#define TL_LGT_SHP_RND     2   /*Բ��*/
#define TL_LGT_SHP_NUM     4   /*����*/
#define TL_LGT_SHP_BAR     8   /*����*/
#define TL_LGT_SHP_ETC     0   /*����*/

#define TL_LGT_ORT_LFT     1   /*����*/
#define TL_LGT_ORT_STR     2   /*ֱ��*/
#define TL_LGT_ORT_RGT     4   /*����*/

#define TL_INIT_METH       0   /*���ó�ʼ���ķ�ʽ:��ʼ����*/
#define TL_SERROI_MEHT     1   /*Ƶ������*/

#define TL_SGL_ON          1   /*�źż������ʾ�����*/
#define TL_SGL_OFF         0   /*�źż������ʾ��ư�*/

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
    unsigned char ucLgtClr;             /*����ɫ����7λ��ʾ�Ƿ��޸�*/
    unsigned char ucLgtShp;             /*����״*/
    unsigned char ucLgtYelTime;         /*�Ƶ�ʱ��*/
    unsigned char ucLgtOrt;             /*��ָʾ����*/

    unsigned char ucMapLgtNO;           /*��Ƽ������*/
    unsigned char ucRsv[15];
} TL_TrcLgtS;

/**/
typedef struct tagTrcGrp
{
    unsigned char ucLgtNums;                  /*�����н�ͨ�Ƹ���������(�졢�̡��ơ����ֵƵȵ�)*/
    char resv0[3];

    TL_PotPosS    stGrpTopLftPos;             /*���齻ͨ����������*/
    TL_PotPosS    stGrpBtmRgtPos;             /*���齻ͨ����������*/
    TL_GrpDirE    enGrpDir;                   /*�����еƵ����з���*/

    TL_TrcLgtS    astTrcLgt[TL_LGTS_MAX];     /*����Ϣ(������ɫ����״��)*/
    char resv[64];
} TL_TrcGrpS;


/**/
typedef struct tagTrcLgtsGrps
{
    unsigned char  enable;		/* ��Ƽ���㷨ʹ�� */
    unsigned char  bShowRect;	/* �Ƿ���ʾ�����Ȧ */
    unsigned char  ucGrpNums;	/* ��Ч������ */
    unsigned char  ucSglUser;   /* ��Ƽ�ⷽʽ(��Ƶ�����ߺ�Ƽ�������) */

	unsigned char ucUsedVDRL;	/* �Ƿ�ʹ�ó�������� */
	unsigned char ucRLRevise;	/* �������ʹ�ܱ�־ */
	unsigned char ucRLStrobe;	/* Ƶ������ʹ�ܱ�־ */
	unsigned char ucRLSFrmNum;	/* Ƶ������֡�� */

	char cEraseHaloEnable;		/* ��������ʹ�� */
	char cEraseHaloPixels;		/* ��������������չ���ص��� */
	char cTLParamSetEnable; 	/* ��������ʹ�ܣ���Ϊ0�������������Ч */
	char cRedModifyMode;	 	/* �����������:1����ɰ汾 2���Ͼɰ汾 
												3���·�����ȫ�Զ���4���·������Զ�1��
												5���·������ֶ�,���ް׵㣩
												6���·������ֶ����ް׵㣩*/

	char cRLAmendLevel;			/* ���������ǳ�ȼ� */	
	char cRsv[15];

    TL_TrcGrpS astTrcGrp[TL_GRPS_MAX];	/* ���к��������ò��� */
    char resv[256];
} TL_TrcLgtsGrpsS;
///}

/* -------------------------------------------ͼƬ�ص��ӿڶ���------------------------------------------------------------*/
typedef struct _SNAP_PIC_TIME
{
    unsigned int uiSec;
    unsigned int uiMSec;
} SNAP_PIC_TIME;

typedef struct tag_FACE_PIC_INFO_
{
    unsigned int uiPicAddrOffset;	/* ����ڻ���buf��ַ��ƫ�� */
    unsigned int uiPicLen;			/* ����ͼƬ��С */
} STR_FACE_PIC_INFO;

/* ---------ÿ��JpegͼƬ�ڴ滮�� -----------

	jpegͷ��	    JPEG_HEADER
	�������ͷ��	VEHICLE_DETECT_HEADER
	jpeg����:
*/
typedef struct _JPEG_HEADER
{
    unsigned char 	snapType;           //ץ������
    unsigned char 	chn;		        //ͨ���� ��0��ʼ
    unsigned short  usPicDebugLen;		//�ɱ���λ�滻ΪͼƬ������Ϣ����
    unsigned int 	uiMsSeconds;	    //ʱ�����
    unsigned int 	seconds;	        //ץ��ʱ��
    unsigned int 	imgLen;		        //ͼƬ����
    //unsigned int 	jpgDataOffset;      //jpg��ͼ����ڻ���buf��ַ��ƫ��
    unsigned int 	snapGroupID;	    //�ڼ���ץ�ģ���0��ʼ�ۼ�
    unsigned int 	snapNum;	        //����ץ�Ĺ��м��ţ���1��ʼ
    unsigned int 	snapID;		        //����ץ�ĵĵڼ��ţ���0��ʼ
} JPEG_HEADER;

typedef struct _VEHICLE_DETECT_HEADER
{
    unsigned char 	lane;		        //�����ţ���0��ʼ
    unsigned char 	confidence;	        //���Ŷȣ� ������Ȧʱ�����������д���ظ���Ȧ��100
    unsigned char 	vehicleType;	    //��ʲô��
    unsigned char   DataType;     		//ͼƬ���ͣ� 21   --- ���Ӿ��촳���Υ�£�22  --- ����Υ�£� 23  --- ����Υ�£�ԭ��Ϊ�����ֽ�
    unsigned int	maxspeed;		    //��������ֵ
    unsigned int	speed;		        //�ٶ�
    unsigned int 	redLightSeconds;    //��ƿ�ʼʱ����
    unsigned int 	redLightMs;	        //��ƿ�ʼʱ�����
    unsigned int 	redLightDuration;   //��Ƴ���ʱ��
    RECT 		    rect;		        //������Ȥ�������򼴳�����Χ

    RECT    	    lpRect;             //��λ�����Ƶ�λ�ã��Ա��ڷŴ�û����0
    char 		    lpStr[12];          //���ƺ���
    unsigned int    LPREnable;          //����ʶ��ʹ��
    unsigned int    lpColor;            //������ɫ
    unsigned int    lpType;             //��������
    unsigned int    lpConfidenct;       //�������Ŷ�
    unsigned int    lpFCConfidenct;     //�������ַ����Ŷ�

    SNAP_PIC_TIME	strSnapPicTime[6];	//ץ��ͼƬ��ÿһ����Ӧ��ץ��ʱ�䣬��ץ�Ĳ�������

    /* ���������ؽṹ�壬��û��������⣬�����ʼ��Ϊ0 */
    unsigned char	ucFaceIdenEn;
    unsigned char	ucFaceSum;
    unsigned char 	ucFaceOvenlayEn;
    unsigned char 	ucSaveSmallPicEn;	/* �Ƿ���СͼƬ */
    RECT			strFaceSearchArea;
    RECT			aStrFaceIden[MAX_FACE_NUM];

    STR_FACE_PIC_INFO aStrFacePicInfo[MAX_FACE_NUM];
    char 			cDriverTrack;               //�г��켣
    char 			cRsv0[1];
    unsigned short  usSnapGroupId;              //�ظ���Ȧץ�ĳ������
    unsigned int	uiRsv[5];                   //����
} VEHICLE_DETECT_HEADER;


/* -------------------------------------------��Ƶ���ص��ӿڶ���------------------------------------------------------------*/

/* ---------H.264ÿ֡�ڴ滮�� -----------
	�������ͷ��    VENC_DATA_FIELD
	��˾˽��ͷ����: VIDEO_DATA_HEADER
	��������:
*/

/* ÿ֡����ͷ�ṹ�嶨�� */
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
	unsigned int uiLane;           /* �ó������ĸ����� */
	unsigned int uiSnapGroup;      /* ץ��ͼƬ��ţ�������ĳ�����ű���һ���ұ�֤Ψһ */ 
	unsigned int uiSnapPicSum;     /* ץ��ͼƬ������ */ 
	unsigned int uiSnapSeconds;    /* ��ʼץ��ʱ�伴��һ��Υ��ʱ�� ���� */
	unsigned int uiSnapMSeconds;   /* ��ʼץ��ʱ�伴��һ��Υ��ʱ�� ������ */
	unsigned int uiIrregFlag;      /* �Ƿ�Υ�£���ȷ���ó�Υ��ʱΪ1����֮Ϊ0���������жϸó�ȷ��Υ��ʱ��ֵ�����Ҹ�������ץ����Ϣ */
	unsigned int uiRsv[3];         /* ���� */
}EP_Alarm_Rec_Car_Info;

typedef struct 
{ 
    unsigned int uiFrameNum;            /* ֡�ţ�����ɲ��� */
    unsigned int iCarSum;                        /* ��֡�м���Υ�³��������֧��6�� */
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
/* -------------------------------------------������Ϣ�ص��ӿ�------------------------------------------------------------*/


/* ����ͳ���ϱ��ṹ�� */
typedef struct
{
    unsigned int chan_id; //ͨ����:��0��ʼ
    unsigned int lane_id; //������,��0��ʼ
    unsigned int begin_time; //��ʼʱ�䣬��λ(s)
    unsigned int end_time; //����ʱ�䣬��λ(s)
    unsigned int flow_num; //������
    unsigned int reserve[4];
} flowstat_status_t;

/* ��Ƶ��ʧ�����м��ȱ����ṹ�� */
typedef struct
{
    unsigned int chan_id;       /* ͨ���ţ���0��ʼ*/
    unsigned int alarm_type;    /* 1 - video loss  2 - motion detect*/
    unsigned int status;        /* video_loss�� 1-vloss,0-v exist;motion detect: 1-�ƶ�,0-δ�ƶ� */
} alarm_report_t;


/* ��ͨ��״̬�ϱ��ṹ�� */
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
    unsigned int index; 		//��Ƽ������
    unsigned int lane_id; 		//������,��0��ʼ
    TL_STATUS  status;    		//��ͨ��״̬
    unsigned int begin_time_s; 	//��ʼʱ�䣬��λ(s)
    unsigned int begin_time_ms; //��ʼʱ�䣬��λ(ms)
    unsigned int reserve[4];
} trafficlight_status_t;

typedef struct
{
	unsigned int lightnum;     //��ͨ�Ƹ���
	trafficlight_status_t LIGHT_STATUS[LIGHT_MAX_NUM];
}trafficlight_alarm_t;
/* ��Ƶ������ز�������
Members:

byBrightnessLevel
���ȣ�ȡֵ��Χ[0,100]

byContrastLevel
�Աȶȣ�ȡֵ��Χ[0,100]

bySharpnessLevel
��ȣ�ȡֵ��Χ[0,6]

bySaturationLevel
���Ͷȣ�ȡֵ��Χ[0,100]

byHueLevel
ɫ�ȣ�ȡֵ��Χ[0,100]

byGreyLevel
ƽ���Ҷȣ�ȡֵ��Χ[0,255]

*/

typedef struct
{
    unsigned char byBrightnessLevel;  /* ���� */
    unsigned char byContrastLevel;    /* �Աȶ� */
    unsigned char bySharpnessLevel;   /* ��� */
    unsigned char bySaturationLevel;  /* ���Ͷ� */
    unsigned char byHueLevel;         /* ɫ�� */
    unsigned char byGreyLevel;        /* ƽ���Ҷ� */
    char byRes[30];
} NET_DEV_VIDEOEFFECT, *LPNET_DEV_VIDEOEFFECT;

/* �Զ��ع�ṹ��
Members
byExposureMode
0-�Զ���1-�ֶ�
byRes
����
dwMaxExposureVal
����ع⼴������ֵ����λus��
dwMinExposureVal
��С�ع⼴��С����ֵ(�̶�ģʽʱ�����ù̶�����ֵ)����λus��
*/
typedef struct
{
    char  byExposureMode;
    char  byRes[3];
    int   dwMaxExposureVal;
    int   dwMinExposureVal;
    char  resv[32];
} NET_DEV_EXPOSURE, *LPNET_DEV_EXPOSURE;

/* ������ƽṹ��
Members:
byGainMode
����ģʽ��0���Զ���1���ֶ�
byRes
��������Ϊ0
dwMaxGainValue
�������ֵ����λdB
dwMinGainValue
��С����ֵ(��Ϊ�ֶ�ģʽʱ�������ֶ�����ֵ)����λdB
*/

typedef struct
{
    char byGainMode;
    char byRes[3];
    int  dwMaxGainValue;
    int  dwMinGainValue;
    char resv[32];
} NET_DEV_GAIN, *LPNET_DEV_GAIN;

/* ��̬���ƽṹ��
Members
byWDREnabled
��̬�Ƿ����ã�0-�����ã�1-����
byWDRLevel
0-3 (ǿ���С���)
*/
typedef struct
{
    char  byWDREnabled;
    char  byWDRLevel;
    char  byRes[14];
} NET_DEV_WDR, *LPNET_DEV_WDR;

/* 2D����
Members
by2DNoiseRemoveEn
2D�����Ƿ����ã�0-�����ã�1-����
by2DNoiseRemoveLevel
0-5 (�Զ�ǿ���Զ��С��Զ������ֶ�ǿ���ֶ��С��ֶ���)
*/
typedef struct
{
    char  by2DNoiseRemoveEn;
    char  by2DNoiseRemoveLevel;
    char  byRes[14];
} NET_DEV_2D_NOISEREMOVE, *LPNET_DEV_2D_NOISEREMOVE;

/* 3D����
Members
By3DNoiseRemoveEn
3D�����Ƿ����ã�0-�����ã�1-����
By3DNoiseRemoveLevel
0-5 (ǿ���С���)
*/
typedef struct
{
    char  by3DNoiseRemoveEn;
    char  by3DNoiseRemoveLevel;
    char  byRes[14];
} NET_DEV_3D_NOISEREMOVE, *LPNET_DEV_3D_NOISEREMOVE;

/* ccd�������ýṹ��
Members
dwSize
�ṹ���С
struVideoEffect
��ƵЧ������
struExposure
�ع����
struGain
�������
struWdr
��̬����
struWhiteBalance
��ƽ�����
2D����
str2DNoiseRemove
3D����
str3DNoiseRemove
*/
typedef struct
{
    int                     dwSize;   /* �ṹ���С */
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

    float fCurGain; //����
    float fCurShutter; //����
    float fCurGrey; //�Ҷ�


    unsigned int curluminance;         //����ֵ
    unsigned int curContrast;		//�Աȶ�
 	unsigned int curSaturation;		//���Ͷ�

	unsigned int curWdr;			//��̬
	unsigned int curSharpness;		//���ֵ
	unsigned int cur2D_noise;		//2d����
	unsigned int cur3D_noise;		//3d����

    short sI2CAddr;			//I2C������ַ
	short sI2CData;			//I2C��������
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

/* -------------------------------------------������ӿڶ���-------------------------------------------------------------*/

/* �����������Ĵ����롣 */
int NET_DVE_GetLastError();


/* -------------------------------------------��ʼ��������ʼ���ӿڶ���---------------------------------------------------*/

/* ��ʼ��SDK����������SDK������ǰ�ᡣ����M3��C64x������Ĭ�ϲ��������羯Chain��Ϊ������������׼����*/
int  NET_CYC_Init();

/* ��̬����������󣬿�ʼ���� */
int  NET_CYC_Start();

/*ֹͣ����*/
int  NET_CYC_Stop();


/* �ͷ�SDK��Դ���ڽ���֮ǰ�����á� */
int  NET_CYC_Cleanup();


/* --------------------------------------------�����豸��������Ϣ---------------------------------------------------- */

int  NET_CYC_SetDEVConfig(
    int		dwCommand,
    void*	lpInBuffer,
    int		dwInBufferSize
);

/* ͼƬ���ص��ӿڶ��� */
/* �ص����� */

typedef void(*fJpegDataCallBack)(
    char*			pBuffer,
    unsigned int	dwBufSize,
    unsigned int	dwUser
);

int  NET_CYC_SetJpegDataCallBack(fJpegDataCallBack cbJpegDataCallBack, unsigned int dwUser);
/* ��Ƶ���ص��ӿڶ��� */
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

/* -------------------------------------------����ʽ�ӿ�------------------------------------------------------------*/

/* �ֶ�ץ�Ľӿں���
    iContinuSnapEn: 0-ֻץ��һ�Σ�1-����ץ�ģ�ץ�ļ����ʵ�����ò���Ϊ׼
*/
//iSnapType  0  ����ץ��   1 �羯ץ��
int NET_DEV_ManualSnap(FORCE_SNAP* param, int  iSnapType);

/*-----------------------------------------�豸������ѯ-----------------------------------------------------------*/
int NET_DEV_Get_Ability(int dwAbilityType, char* PInBuf, int dwInLength, char* pOutBuf, int dwOutLength);


/*��ȡ�豸��������Ϣ
*/
int NET_CYC_GetDEVConfig(
    int   dwCommand,		//��������
    int   chan_id,			//��ʹ�õ�ʱ�����ó�0xff
    int	  lane_id,			//��ʹ�õ�ʱ�����ó�0xff
    void* lpOutBuffer,		//�������ݵĻ���ָ��
    int   dwOutBufferSize,	//�������ݵĻ��峤��
    int*  lpBytesReturned	//ʵ���յ������ݳ���ָ��
);

int NET_CYC_SetDEVConfig_Test(int dwCommand, void* lpInBuffer, int dwInBufferSize);

#ifdef __cplusplus
}
#endif

#endif  /* _DM8127_DSP_SERVICE_H_*/
