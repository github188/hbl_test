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

#define HAGD_MSG_FIFO_LEN    240     /* 开辟的缓冲池大小 */
#define HAGD_MSG_START_FLAG  0x7e    /* 起始帧字节 */
#define HAGD_MSG_END_FLAG    0x7f    /* 结束帧字节 */
#define HAGD_TMP_FRAME_LEN   0x20    /* 缓冲帧长度 */

/*哈工大码流格式*/
/*
字节数	说明				值说明
1		帧起始				0x7E
1		设备ID              0
1		板卡ID				0～15
2		消息体长度			0～65536
可变	消息体				具体看协议
2		CRC校验				从设备ID到CRC校验结束
1		帧结束				0x7F
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
消息体长度为3+m个字节
字节	说明		备注
1		通讯代码	0x19
1		板卡ID		0~15（通过线圈检测卡的拨码开关进行设置）
1		上传的通道数量m 1~4
1		通道1 实时状态 
Bit7~6：行驶方向。未知方向：0；正行：1；逆行：2。
（对占有率模块，该位为0）
Bit5：线圈状态。出车：0；进车：1。
Bit4：速度模块线圈位置。前线圈：0；后线圈：1。
Bit3~1：模块ID。速度模块1～2，占有率模块1～4。
Bit0：模块类型。占有率模块：0；速度模块：1。

1		通道m实时状态 数据格式同通道1
*/
typedef struct tagREALTIME_STATUS
{
	char commCode;
	char cardId;
	char chanNum;
	char status[1];
}REALTIME_STATUS;

/* 以下为实时状态字节各位含义 */
typedef struct tagRDATA
{
	unsigned char moduleType:1;
	unsigned char moduleId:3;
	unsigned char loopPos:1;
	unsigned char loopStatus:1;
	unsigned char direction:2;
}RDATA,*PRDATA;



/*实时数据上传*/
/*
消息体长度为4+4*m+7*n个字节
字节		说明		备注
1			通讯代码	0x1a
1			板卡ID		0~15（通过线圈检测卡的拨码开关进行设置）
1			上传的占有率模块数量m 1~4
1			上传的速度模块数量n 1~2
1			占有率模块1 ID 1~4
2			占有率模块1 存在时间 0~65535ms <高字节，低字节>
1			占有率模块1
			最大频率改变值
			0~255Hz

1			占有率模块m ID 1~4
2			占有率模块m存在时间 0~65535ms <高字节，低字节>
1			占有率模块m
			最大频率改变值
			0~255Hz
1			速度模块1 ID 1~2
1		速度模块1车道状态 正行：1；逆行：2
1		速度模块1车速 0~255 (km/h)
1		速度模块1车长 0~255 (0.1m)
2		速度模块1前线圈
		存在时间
		0~65535ms <高字节，低字节>
1		速度模块1前线圈
		最大频率改变值
		0~255 Hz

1		速度模块n ID 1~2
1		速度模块n车道状态 正行：1；逆行：2
1		速度模块n车速 0~255 (km/h)
1		速度模块n车长 0~255 (0.1m)
2		速度模块n前线圈
存在时间
		0~65535ms<高字节，低字节>
		1 速度模块n前线圈
		最大频率改变值
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

/* 实际数据上报占有率模块状态定义 */
typedef  struct tagMouduleOccuppy
{
	char  cModuleId;        //占有率模块ID (1~4)
	short nExistTime;       //占有率模块存在时间
	char  cMaxFc;           //占用率模块最大频率改变值
}__attribute__((packed))MODULE_OCUPPY;


/* 实际数据上报速度模块状态定义 */
typedef struct  tagModuleSpeed
{
	char cSpeedModeId;          //实际数据速度模块ID (1~2)
	char cCarChannelDirSta;     //车道方向状态。1:正行；2:逆行
	char cCarSpeed;             //车速(0~255(km/h))
	char cCarLen;               //车长(0~255(0.1m))
	short nExistTime;           //前线圈存在时间(0~65535 ms)
	char cMaxFc;                //前线圈最大频率改变值(0~255HZ)
}__attribute__((packed))MODULE_SPEED;

/* 车检器实时数据上报数据结构定义 */
typedef struct hagdRealData
{
        //MODULE_OCUPPY   realDataOcuppyStr; //由于回调里面没有处理，暂时不开放该结构
        MODULE_SPEED    realDataSpeedStr;
}HAGDA_REAL_DATA_STR;

/* 车检器状态上报数据结构定义 */
typedef struct hagdReadSta
{
    char cChannelNum;       //车道号
    char cModuleType;       //占有率模块、速度模块；0:占有率模块，1:速度模块
    char cModuleId;         //各模块下序号ID;占有率模块范围(1~4)；速度模块范围(1~2)
    char cSpeedModulePos;   //速度模块下线圈位置;0:前线圈；1:后线圈
    char cLineCycleSta;     //线圈状态;0:出车，1:进车
    char cCarDir;           //车行驶方向；0:示知状态；1:正行；2:逆行；注意:占有率模块该位不用
    char reserved[2];
}HAGD_REAL_STA_STR;


#define HAGDA_REAL_STA_REPORT   0x19    //哈工大实时状态上报
#define HAGDA_REAL_DATA_REPORT  0x1A    //哈工大实时数据上报

#define HAGDA_OCCUPY_STA_REPORT 0x0     //哈工大实时状态上报
#define HAGDA_SPEED_STA_REPORT  0x1     //哈工大实时数据上报


typedef union hagdProtoData
{
    HAGD_REAL_STA_STR strHagdRealSta;       //实时状态上报
    HAGDA_REAL_DATA_STR strHagdRealData;    //实时数据上报
}HAGD_PROTO_DATA_STR;

/* 哈工大协议详细定义 */
typedef struct hagdProtostr
{
    char  cCommuId;                     //通讯模块定义，0x19:实时状态；0x1a:实时数据
    char  cBoardId;                     //板卡ID,通过外设拨码开关确定
    char  reserved[2];
    HAGD_PROTO_DATA_STR strProtoData;   //哈工大协议数据上报结构
}HAGD_PROTO_STR;


#endif /* _DEV_HAGD_PROTOCOL_H */
