/******************************************************************************
 * hagdProtocol.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.11.21, wangbin Create
 * --------------------
 ******************************************************************************/
#ifndef _DEV_HAGD_PROTOCOL_H
#define _DEV_HAGD_PROTOCOL_H 

#define HAGD_MSG_FIFO_LEN    240     /* ���ٵĻ���ش�С */
#define HAGD_MSG_START_FLAG  0x7e    /* ��ʼ֡�ֽ� */
#define HAGD_MSG_END_FLAG    0x7f    /* ����֡�ֽ� */
#define HAGD_TMP_FRAME_LEN   0x20    /* ����֡���� */

/*������������ʽ*/
/*
�ֽ���	˵��				ֵ˵��
1		֡��ʼ				0x7E
1		�豸ID              0
1		�忨ID				0��15
2		��Ϣ�峤��			0��65536
�ɱ�	��Ϣ��				���忴Э��
2		CRCУ��				���豸ID��CRCУ�����
1		֡����				0x7F
*/


#define HIT_FRAME_START_FLAG  0x7E
#define HIT_FRAME_END_FLAG    0x7F
#define HIT_CHANGE_CODE_FLAG  0x7D

#define RADAR_FRAME_START_FLAG 0xFF

typedef struct tagHIT_FRAME_DATA
{
	char frameStart;
	char deviceid;
	char cardid;
	short msglen;
	short checkcrc;
	char frameEnd;
}__attribute__((packed))HIT_FRAME_DATA;


typedef struct tagRADAR_DATA
{
	char frameStart;
	char data[2];
	char checkSum;
}__attribute__((packed))RADAR_DATA;

/*
��Ϣ�峤��Ϊ3+m���ֽ�
�ֽ�	˵��		��ע
1		ͨѶ����	0x19
1		�忨ID		0~15��ͨ����Ȧ��⿨�Ĳ��뿪�ؽ������ã�
1		�ϴ���ͨ������m 1~4
1		ͨ��1 ʵʱ״̬ 
Bit7~6����ʻ����δ֪����0�����У�1�����У�2��
����ռ����ģ�飬��λΪ0��
Bit5����Ȧ״̬��������0��������1��
Bit4���ٶ�ģ����Ȧλ�á�ǰ��Ȧ��0������Ȧ��1��
Bit3~1��ģ��ID���ٶ�ģ��1��2��ռ����ģ��1��4��
Bit0��ģ�����͡�ռ����ģ�飺0���ٶ�ģ�飺1��

1		ͨ��mʵʱ״̬ ���ݸ�ʽͬͨ��1
*/
typedef struct tagREALTIME_STATUS
{
	char commCode;
	char cardId;
	char chanNum;
	char status[1];
}REALTIME_STATUS;

/* ����Ϊʵʱ״̬�ֽڸ�λ���� */
typedef struct tagRDATA
{
	unsigned char moduleType:1;
	unsigned char moduleId:3;
	unsigned char loopPos:1;
	unsigned char loopStatus:1;
	unsigned char direction:2;
}RDATA,*PRDATA;



/*ʵʱ�����ϴ�*/
/*
��Ϣ�峤��Ϊ4+4*m+7*n���ֽ�
�ֽ�		˵��		��ע
1			ͨѶ����	0x1a
1			�忨ID		0~15��ͨ����Ȧ��⿨�Ĳ��뿪�ؽ������ã�
1			�ϴ���ռ����ģ������m 1~4
1			�ϴ����ٶ�ģ������n 1~2
1			ռ����ģ��1 ID 1~4
2			ռ����ģ��1 ����ʱ�� 0~65535ms <���ֽڣ����ֽ�>
1			ռ����ģ��1
			���Ƶ�ʸı�ֵ
			0~255Hz

1			ռ����ģ��m ID 1~4
2			ռ����ģ��m����ʱ�� 0~65535ms <���ֽڣ����ֽ�>
1			ռ����ģ��m
			���Ƶ�ʸı�ֵ
			0~255Hz
1			�ٶ�ģ��1 ID 1~2
1		�ٶ�ģ��1����״̬ ���У�1�����У�2
1		�ٶ�ģ��1���� 0~255 (km/h)
1		�ٶ�ģ��1���� 0~255 (0.1m)
2		�ٶ�ģ��1ǰ��Ȧ
		����ʱ��
		0~65535ms <���ֽڣ����ֽ�>
1		�ٶ�ģ��1ǰ��Ȧ
		���Ƶ�ʸı�ֵ
		0~255 Hz

1		�ٶ�ģ��n ID 1~2
1		�ٶ�ģ��n����״̬ ���У�1�����У�2
1		�ٶ�ģ��n���� 0~255 (km/h)
1		�ٶ�ģ��n���� 0~255 (0.1m)
2		�ٶ�ģ��nǰ��Ȧ
����ʱ��
		0~65535ms<���ֽڣ����ֽ�>
		1 �ٶ�ģ��nǰ��Ȧ
		���Ƶ�ʸı�ֵ
		0~255Hz

*/
typedef struct tagREALTIME_DATA
{
	char commCode;
	char cardId;
	char nOcupNum;
	char nSpeedNum;
	char data[1];
}__attribute__((packed))REALTIME_DATA;

/* ʵ�������ϱ�ռ����ģ��״̬���� */
typedef  struct tagMouduleOccuppy
{
	char  cModuleId;        //ռ����ģ��ID (1~4)
	short nExistTime;       //ռ����ģ�����ʱ��
	char  cMaxFc;           //ռ����ģ�����Ƶ�ʸı�ֵ
}__attribute__((packed))MODULE_OCUPPY;


/* ʵ�������ϱ��ٶ�ģ��״̬���� */
typedef struct  tagModuleSpeed
{
	char cSpeedModeId;          //ʵ�������ٶ�ģ��ID (1~2)
	char cCarChannelDirSta;     //��������״̬��1:���У�2:����
	char cCarSpeed;             //����(0~255(km/h))
	char cCarLen;               //����(0~255(0.1m))
	short nExistTime;           //ǰ��Ȧ����ʱ��(0~65535 ms)
	char cMaxFc;                //ǰ��Ȧ���Ƶ�ʸı�ֵ(0~255HZ)
}__attribute__((packed))MODULE_SPEED;

/* ������ʵʱ�����ϱ����ݽṹ���� */
typedef struct hagdRealData
{
        //MODULE_OCUPPY   realDataOcuppyStr; //���ڻص�����û�д�����ʱ�����Ÿýṹ
        MODULE_SPEED    realDataSpeedStr;
}HAGDA_REAL_DATA_STR;

/* ������״̬�ϱ����ݽṹ���� */
typedef struct hagdReadSta
{
    char cChannelNum;       //������
    char cModuleType;       //ռ����ģ�顢�ٶ�ģ�飻0:ռ����ģ�飬1:�ٶ�ģ��
    char cModuleId;         //��ģ�������ID;ռ����ģ�鷶Χ(1~4)���ٶ�ģ�鷶Χ(1~2)
    char cSpeedModulePos;   //�ٶ�ģ������Ȧλ��;0:ǰ��Ȧ��1:����Ȧ
    char cLineCycleSta;     //��Ȧ״̬;0:������1:����
    char cCarDir;           //����ʻ����0:ʾ֪״̬��1:���У�2:���У�ע��:ռ����ģ���λ����
    char reserved[2];
}HAGD_REAL_STA_STR;


#define HAGDA_REAL_STA_REPORT   0x19    //������ʵʱ״̬�ϱ�
#define HAGDA_REAL_DATA_REPORT  0x1A    //������ʵʱ�����ϱ�

#define HAGDA_OCCUPY_STA_REPORT 0x0     //������ʵʱ״̬�ϱ�
#define HAGDA_SPEED_STA_REPORT  0x1     //������ʵʱ�����ϱ�


typedef union hagdProtoData
{
    HAGD_REAL_STA_STR strHagdRealSta;       //ʵʱ״̬�ϱ�
    HAGDA_REAL_DATA_STR strHagdRealData;    //ʵʱ�����ϱ�
}HAGD_PROTO_DATA_STR;

/* ������Э����ϸ���� */
typedef struct hagdProtostr
{
    char  cCommuId;                     //ͨѶģ�鶨�壬0x19:ʵʱ״̬��0x1a:ʵʱ����
    char  cBoardId;                     //�忨ID,ͨ�����貦�뿪��ȷ��
    char  reserved[2];
    HAGD_PROTO_DATA_STR strProtoData;   //������Э�������ϱ��ṹ
}HAGD_PROTO_STR;


#endif /* _DEV_HAGD_PROTOCOL_H */
