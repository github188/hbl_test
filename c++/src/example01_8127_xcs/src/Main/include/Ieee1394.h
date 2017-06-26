#ifndef  _IEEE1394_GPIO_TRIGGER_H
#define  _IEEE1394_GPIO_TRIGGER_H


#define     SET_FLASH_ATTR   			    (0)
#define     SET_SNAP_ATTR		 			(1)
#define     SET_SNAP_MANNUAL			    (2)
#define     SEND_RS485_ADDR                 (4)
#define     GET_TGVD_INTR                   (5)


#define  	DSP_IO_BASE      				(0x80000000)
#define  	DSP_MEM_BASE 		 			(0x80000000)

#define     PCIBAR0TRL_OFFSET     	        (0x485C0)   //PCIBAR0TRL��ַΪ0x0204 85C0

#define  	DSPPREG      					(DSP_IO_BASE+PCIBAR0TRL_OFFSET)
#define 	ARM_TO_DSP_SNAP_READY		    (0x87564312)    	/*  ARM ֪ͨDSP����׼���ñ�־  */
#define 	DSP_TO_ARM_SNAP_OK		  	    (0x1234abcd) 		/*  DSP ֪ͨARM����׼���ñ�־  */
#define 	VIRTUAL_SNAP_FLAG				(0x87786556)		/*	������ץ�ı�־*/

#define		VCAP_BUFF_FLASH_FLAG_BASE_ADDR 	(0x89F10000)
#define		VCAP_BUFF_FLASH_FLAG_SIZE		(0x00010000)


#define		ARM2DSPCOMM_BASE_ADDR   		(0x89F20000)
#define		ARM2DSPCOMM_SIZE 			    (0x00010000)
#define		DSP2ARMCOMM_BASE_ADDR			(0x89F30000)
#define		DSP2ARMCOMM_SIZE				(0x00010000)


#define 	MAX_SNAP_VEHICLE_NUM			(5)

#define     SNAP_TYPE_SPEED             (0x01)    /* ����ץ�� */
#define     SNAP_TYPE_CLIP              (0x02)    /* �̵� ����ץ�� */
#define     SNAP_TYPE_3_PLUS_1          (0x03)    /* ��� ����һ */
#define     SNAP_TYPE_6_PLUS_1          (0x04)    /* ��� ����һ */
#define     SNAP_TYPE_VLOOP             (0x05)    /* ������Ȧ */
#define     SNAP_TYPE_REVERSE           (0x06)    /* ����ץ�� */
#define     SNAP_TYPE_9_PLUS_1          (0x07)    /* ��ƾź�һ */
#define     SNAP_TYPE_3_SEPARATE        (0x08)    /* �������� */
#define     SNAP_TYPE_SINGAL_COIL       (0x09)    /* ����Ȧץ�� */
#define     SNAP_TYPE_RIDE_LINES        (0x0a)    /* ����������ץ�� */
#define     SNAP_TYPE_RADAR             (0x0b)    /* �״����ץ�� */ 
#define     SNAP_TYPE_EP_REVERSE        (0x0c)    /* �羯����ץ�� */
#define     SNAP_TYPE_3_PLUS_1_LEFT     (0x0d)    /* ��� ����һ�� */
#define     SNAP_TYPE_4_SEPARATE        (0x0e)    /* ���Ŷ���ģʽ */
#define     SNAP_TYPE_DIR_REVERSE       (0x0f)    /* ���򳵵�����ץ�� */
#define     SNAP_TYPE_TRUCK             (0x10)    /* �����ץ�� */

#define 	SNAP_TYPE_RECORD_INFORM	    (0xaa)	/* ��Ʊ���¼������֪ͨ */
#define     SNAP_TYPE_CX_NULL_SNAP		(0xab)	/*��Ϫ�������*/


/* �ظ���Ȧץ����Ϣ�ṹ */
typedef struct _LANE_SNAP_INFO
{
	unsigned int        uiFlashFlg;
	unsigned char		ucRoadId;		/*  ������ */
	unsigned char		ucSnapType;		/*  ץ������ */
	unsigned char		ucStepNum;		/*  һ���м��� */
	unsigned char		ucCurrentStep;	/*  ��ǰ�ǵڼ��� */
	unsigned int		uiSpeed;		/*  �ٶ� */
	unsigned int        uiIndex;
	unsigned int        uiFlashTimeSec; /* ץ�ĵ����� */
	unsigned int        uiFlashTimeUsec;/* ץ�ĵ�΢�� */
	
	unsigned short 		usGroupId;    /* ��ǰץ����� */
	unsigned short 		usRsv;        /* ���ֽڱ��� */
	unsigned int        uiResv[2]; 
}LANE_SNAP_INFO;

typedef struct _SNAP_INFO
{
	LANE_SNAP_INFO	laneInfo[MAX_SNAP_VEHICLE_NUM];	/*���������Ϣ*/
}SNAP_INFO;

#define  MANNUAL_SNAP    			    (1)
#define  AUTO_SNAP						(2)


#define  MAX_BUFFER_STEP    	        (6)
#define  MAX_QUEUE_NUM 	    	        (12)



#define  SIZE_OF_UINT32			(sizeof(unsigned int))		
#define  SNAP_STR_SIZE    	    (sizeof(LANE_SNAP_INFO))

#define  SIZE_OF_64					(64)

#define  OFFSET(x)          (x*MAX_BUFFER_STEP*SIZE_OF_64)



/* ������Ȧץ�Ľṹ */
typedef struct 
{
    unsigned int     uiVirtualSnapFlg;
	unsigned int 	 uiFlashChannel;	/*  ����ͨ�� */
	unsigned int 	 uiSnapStep;		/*	ץ�Ĳ��� */
}VIRTAUL_SNAP_INFO;
/* �������� */
#define  FLASH_CUSTOM                      	(0x0001)  /* ָ���������� */
#define  FLASH_POLLING                      (0x0002)  /* ��ѯ���� */
#define  MAX_FLASH_NUM                    	(6)
#define  MAX_FLASH_CHANNEL_NUM      	    (5)


#define	SET_TTL_FLASH_MODE (7)
typedef struct
{
	 unsigned char szTTLTrigMode[MAX_FLASH_NUM];	/*  TTL����ƴ�����ʽ */
	 unsigned char szTTLTrigLevel[MAX_FLASH_NUM];	/*  TTL����ģʽ�µĴ�����ƽ*/
	 unsigned int  szTTLStrobeTime[MAX_FLASH_NUM];	/*  ����Ƴ�����ʱ��us*/
	 unsigned int  szTTLMpll[MAX_FLASH_NUM];		/*  Ƶ���Ʊ�Ƶϵ��*/ 
	 
	 unsigned int  szTTLFlashEnable[MAX_FLASH_NUM]; /*   �����ʹ�ܱ�־ */
	 unsigned int  szTTLTrigStartLine[MAX_FLASH_NUM];/*   �����źŴ���ʱ��*/
	 unsigned int  szStrobStartLine[MAX_FLASH_NUM]; /*Ƶ���ƴ���ʱ����ʼ��*/
	 unsigned int  szTTLMACSync[MAX_FLASH_NUM];   	/*Ƶ���ƽ���ͬ���ź��ǵ�������Ľ��������ź�������ķ����ź�֮�����λ���λ��Ӧ��ʱ������20us*/
	 unsigned int  szResvered[MAX_FLASH_NUM]; 		/*  ����  */
}TTL_FLASH_CONF_INFO;


/* ����ƴ���ģʽ����ȡֵ��*/
#define TTL_FLASH			(0x00)	/*�����*/
#define TTL_FLASH_STROBE	(0x01)	/*Ƶ����*/

/* ����ƴ�����ƽȡֵ(�����ں˼�Ӳ��ԭ���½��غ���������Ҫ����һ�£���Ӧ�ò���Ҫ���޸�)��*/
#define TTL_TRIG_RISE_EDGE	(0x00)	/*�����ش���*/
#define TTL_TRIG_FAIL_EDGE	(0x01)	/*�½��ش���*/
#define TTL_TRIG_HIGH_LEVEL	(0x02)	/*�ߵ�ƽ����*/
#define TTL_TRIG_LOW_LEVEL	(0x03)	/*�͵�ƽ����*/
 
//ioctl(fd,SET_TTL_FLASH_MODE ,&ttl_flash_conf_info);


#define FLASH_TRIG_MODE_RS485               (0x00)
#define FLASH_TRIG_MODE_GPIO                (0x80)
#define FLASH_TRIG_MODE_GPIO_RISE_EDGE      (0x81)
#define FLASH_TRIG_MODE_GPIO_FAIL_EDGE      (0x82)
#define FLASH_TRIG_MODE_GPIO_HIGH_LEVEL     (0x83)
#define FLASH_TRIG_MODE_GPIO_LOW_LEVEL      (0x84)

/**�����������Ϣ**/ 
typedef struct 
{
	unsigned char    ucEnable;						/*   �Ƿ�ʹ��*/
    unsigned char    ucFlashType;                   /*   ��������ⷽʽ	*/
    unsigned char    ucFlashNum;                    /*   ����ͨ�����������Ŀ	*/
    unsigned char    ucTrigMode;       				/*   ����	*/
    unsigned char    szFlashAddr[MAX_FLASH_NUM];    /*   ����ͨ��ÿ������Ƶ�485 ��ַ	*/
    unsigned char 	 resv2[2];						/*   ���� */
}FLASH_CHANNLE_CONF_INFO;

typedef struct
{
	unsigned int  			uiChannelNum;						/*�ܵ�����ͨ����Ŀ*/
	FLASH_CHANNLE_CONF_INFO strFlashConf[MAX_FLASH_CHANNEL_NUM];/*ÿ������ͨ������Ϣ*/
}FLASH_CONF_INFO;

#define  DSP_PCI_WRITE(addr,value)    (memcpy((void *)addr,(void *)&value,sizeof(value)))
#define  DSP_PCI_READ(addr,value)			(memcpy((void *)&value,(void*)addr,sizeof(value)))
#endif
