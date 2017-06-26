#ifndef  _IEEE1394_GPIO_TRIGGER_H
#define  _IEEE1394_GPIO_TRIGGER_H


#define     SET_FLASH_ATTR   			    (0)
#define     SET_SNAP_ATTR		 			(1)
#define     SET_SNAP_MANNUAL			    (2)
#define     SEND_RS485_ADDR                 (4)
#define     GET_TGVD_INTR                   (5)


#define  	DSP_IO_BASE      				(0x80000000)
#define  	DSP_MEM_BASE 		 			(0x80000000)

#define     PCIBAR0TRL_OFFSET     	        (0x485C0)   //PCIBAR0TRL地址为0x0204 85C0

#define  	DSPPREG      					(DSP_IO_BASE+PCIBAR0TRL_OFFSET)
#define 	ARM_TO_DSP_SNAP_READY		    (0x87564312)    	/*  ARM 通知DSP数据准备好标志  */
#define 	DSP_TO_ARM_SNAP_OK		  	    (0x1234abcd) 		/*  DSP 通知ARM数据准备好标志  */
#define 	VIRTUAL_SNAP_FLAG				(0x87786556)		/*	有虚拟抓拍标志*/

#define		VCAP_BUFF_FLASH_FLAG_BASE_ADDR 	(0x89F10000)
#define		VCAP_BUFF_FLASH_FLAG_SIZE		(0x00010000)


#define		ARM2DSPCOMM_BASE_ADDR   		(0x89F20000)
#define		ARM2DSPCOMM_SIZE 			    (0x00010000)
#define		DSP2ARMCOMM_BASE_ADDR			(0x89F30000)
#define		DSP2ARMCOMM_SIZE				(0x00010000)


#define 	MAX_SNAP_VEHICLE_NUM			(5)

#define     SNAP_TYPE_SPEED             (0x01)    /* 测速抓拍 */
#define     SNAP_TYPE_CLIP              (0x02)    /* 绿灯 剪切抓拍 */
#define     SNAP_TYPE_3_PLUS_1          (0x03)    /* 红灯 三合一 */
#define     SNAP_TYPE_6_PLUS_1          (0x04)    /* 红灯 六合一 */
#define     SNAP_TYPE_VLOOP             (0x05)    /* 虚拟线圈 */
#define     SNAP_TYPE_REVERSE           (0x06)    /* 逆行抓拍 */
#define     SNAP_TYPE_9_PLUS_1          (0x07)    /* 红灯九合一 */
#define     SNAP_TYPE_3_SEPARATE        (0x08)    /* 独立三张 */
#define     SNAP_TYPE_SINGAL_COIL       (0x09)    /* 单线圈抓拍 */
#define     SNAP_TYPE_RIDE_LINES        (0x0a)    /* 机动车骑线抓拍 */
#define     SNAP_TYPE_RADAR             (0x0b)    /* 雷达测速抓拍 */ 
#define     SNAP_TYPE_EP_REVERSE        (0x0c)    /* 电警逆行抓拍 */
#define     SNAP_TYPE_3_PLUS_1_LEFT     (0x0d)    /* 红灯 三合一左 */
#define     SNAP_TYPE_4_SEPARATE        (0x0e)    /* 四张独立模式 */
#define     SNAP_TYPE_DIR_REVERSE       (0x0f)    /* 单向车道逆行抓拍 */
#define     SNAP_TYPE_TRUCK             (0x10)    /* 大货车抓拍 */

#define 	SNAP_TYPE_RECORD_INFORM	    (0xaa)	/* 红灯报警录像启动通知 */
#define     SNAP_TYPE_CX_NULL_SNAP		(0xab)	/*慈溪特殊空拍*/


/* 地感线圈抓拍信息结构 */
typedef struct _LANE_SNAP_INFO
{
	unsigned int        uiFlashFlg;
	unsigned char		ucRoadId;		/*  车道号 */
	unsigned char		ucSnapType;		/*  抓拍类型 */
	unsigned char		ucStepNum;		/*  一共有几步 */
	unsigned char		ucCurrentStep;	/*  当前是第几步 */
	unsigned int		uiSpeed;		/*  速度 */
	unsigned int        uiIndex;
	unsigned int        uiFlashTimeSec; /* 抓拍的秒数 */
	unsigned int        uiFlashTimeUsec;/* 抓拍的微秒 */
	
	unsigned short 		usGroupId;    /* 当前抓拍组号 */
	unsigned short 		usRsv;        /* 四字节保留 */
	unsigned int        uiResv[2]; 
}LANE_SNAP_INFO;

typedef struct _SNAP_INFO
{
	LANE_SNAP_INFO	laneInfo[MAX_SNAP_VEHICLE_NUM];	/*车道相关信息*/
}SNAP_INFO;

#define  MANNUAL_SNAP    			    (1)
#define  AUTO_SNAP						(2)


#define  MAX_BUFFER_STEP    	        (6)
#define  MAX_QUEUE_NUM 	    	        (12)



#define  SIZE_OF_UINT32			(sizeof(unsigned int))		
#define  SNAP_STR_SIZE    	    (sizeof(LANE_SNAP_INFO))

#define  SIZE_OF_64					(64)

#define  OFFSET(x)          (x*MAX_BUFFER_STEP*SIZE_OF_64)



/* 虚拟线圈抓拍结构 */
typedef struct 
{
    unsigned int     uiVirtualSnapFlg;
	unsigned int 	 uiFlashChannel;	/*  闪光通道 */
	unsigned int 	 uiSnapStep;		/*	抓拍步骤 */
}VIRTAUL_SNAP_INFO;
/* 闪光类型 */
#define  FLASH_CUSTOM                      	(0x0001)  /* 指定闪光类型 */
#define  FLASH_POLLING                      (0x0002)  /* 轮询闪光 */
#define  MAX_FLASH_NUM                    	(6)
#define  MAX_FLASH_CHANNEL_NUM      	    (5)


#define	SET_TTL_FLASH_MODE (7)
typedef struct
{
	 unsigned char szTTLTrigMode[MAX_FLASH_NUM];	/*  TTL闪光灯触发方式 */
	 unsigned char szTTLTrigLevel[MAX_FLASH_NUM];	/*  TTL触发模式下的触发电平*/
	 unsigned int  szTTLStrobeTime[MAX_FLASH_NUM];	/*  闪光灯持续的时间us*/
	 unsigned int  szTTLMpll[MAX_FLASH_NUM];		/*  频闪灯倍频系数*/ 
	 
	 unsigned int  szTTLFlashEnable[MAX_FLASH_NUM]; /*   闪光灯使能标志 */
	 unsigned int  szTTLTrigStartLine[MAX_FLASH_NUM];/*   闪光信号触发时间*/
	 unsigned int  szStrobStartLine[MAX_FLASH_NUM]; /*频闪灯触发时间起始行*/
	 unsigned int  szTTLMACSync[MAX_FLASH_NUM];   	/*频闪灯交流同步信号是调整输入的交流方波信号与输出的方波信号之间的相位差，相位对应的时间间隔是20us*/
	 unsigned int  szResvered[MAX_FLASH_NUM]; 		/*  保留  */
}TTL_FLASH_CONF_INFO;


/* 闪光灯触发模式设置取值：*/
#define TTL_FLASH			(0x00)	/*闪光灯*/
#define TTL_FLASH_STROBE	(0x01)	/*频闪灯*/

/* 闪光灯触发电平取值(由于内核及硬件原因，下降沿和上升沿需要倒置一下，但应用不需要做修改)：*/
#define TTL_TRIG_RISE_EDGE	(0x00)	/*上升沿触发*/
#define TTL_TRIG_FAIL_EDGE	(0x01)	/*下降沿触发*/
#define TTL_TRIG_HIGH_LEVEL	(0x02)	/*高电平触发*/
#define TTL_TRIG_LOW_LEVEL	(0x03)	/*低电平触发*/
 
//ioctl(fd,SET_TTL_FLASH_MODE ,&ttl_flash_conf_info);


#define FLASH_TRIG_MODE_RS485               (0x00)
#define FLASH_TRIG_MODE_GPIO                (0x80)
#define FLASH_TRIG_MODE_GPIO_RISE_EDGE      (0x81)
#define FLASH_TRIG_MODE_GPIO_FAIL_EDGE      (0x82)
#define FLASH_TRIG_MODE_GPIO_HIGH_LEVEL     (0x83)
#define FLASH_TRIG_MODE_GPIO_LOW_LEVEL      (0x84)

/**闪光灯配置信息**/ 
typedef struct 
{
	unsigned char    ucEnable;						/*   是否使能*/
    unsigned char    ucFlashType;                   /*   闪光灯闪光方式	*/
    unsigned char    ucFlashNum;                    /*   闪光通道的闪光灯数目	*/
    unsigned char    ucTrigMode;       				/*   保留	*/
    unsigned char    szFlashAddr[MAX_FLASH_NUM];    /*   闪光通道每个闪光灯的485 地址	*/
    unsigned char 	 resv2[2];						/*   保留 */
}FLASH_CHANNLE_CONF_INFO;

typedef struct
{
	unsigned int  			uiChannelNum;						/*总的闪光通道数目*/
	FLASH_CHANNLE_CONF_INFO strFlashConf[MAX_FLASH_CHANNEL_NUM];/*每个闪光通道的信息*/
}FLASH_CONF_INFO;

#define  DSP_PCI_WRITE(addr,value)    (memcpy((void *)addr,(void *)&value,sizeof(value)))
#define  DSP_PCI_READ(addr,value)			(memcpy((void *)&value,(void*)addr,sizeof(value)))
#endif
