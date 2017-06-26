#ifndef _RAW_REC_H
#define  _RAW_REC_H

#define MAX_RAW_WRITE_BUFF_SIZE   (8*1024)

#define STREAM_TYPE_Y    (0)
#define STREAM_TYPE_YUV  (1)
#define STREAM_TYPE_JPG	 (2)

int RawRecInit(int iChannelNum);

int RawRecUninit(void);

int RawRecStart(int channel);
int RawRecStop(int channle);
int RawRecDataWrite(int channel,unsigned char *pFrame,int len,int iStreamType);
int RawRecFtpServerSet(const char *pcServerIp,const char *pcUserName,const char *pcPassWord);


#endif
