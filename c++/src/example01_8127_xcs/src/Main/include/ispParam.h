#ifndef _ISP_PARAM_H_
#define _ISP_PARAM_H_

#if defined(CYC500XKW4C)||defined(CYC500XAW)
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#if defined(CYC200XKW)||defined(CYC200MK)||defined(CYC200K2)
#define TOTAL_LINES         (1272)
#else
#define TOTAL_LINES         (1812)
#endif

#define ARM2FPGA_DEV          "/dev/misc/armtofpga"

#define CMD_WRITE             (15)
#define CMD_READ              (16)

#define MAX_ISP_PARA_SET       (4)

#define EXPOSURE_MODE_ADDR                     (0x90)

#define EXPOSURE_AUTO_SHUTTER_MAX_L_ADDR     (0x9C)  
#define EXPOSURE_AUTO_SHUTTER_MAX_H_ADDR     (0x9D)

#define EXPOSURE_AUTO_SHUTTER_MID_L_ADDR     (0x9E)  
#define EXPOSURE_AUTO_SHUTTER_MID_H_ADDR     (0x9F)

#define EXPOSURE_AUTO_SHUTTER_MIN_L_ADDR     (0xA0)  
#define EXPOSURE_AUTO_SHUTTER_MIN_H_ADDR     (0xA1)


#define EXPOSURE_AUTO_GAIN_MAX_L_ADDR         (0xA2)
#define EXPOSRE_AUTO_GAIN_MAX_H_ADDR          (0xA3)

#define EXPOSURE_AUTO_GAIN_MID_L_ADDR         (0xA4)
#define EXPOSRE_AUTO_GAIN_MID_H_ADDR          (0xA5)

#define EXPOSURE_AUTO_GAIN_MIN_L_ADDR         (0xA6)
#define EXPOSRE_AUTO_GAIN_MIN_H_ADDR          (0xA7)


#define MANUAL_SHUTTER_L_ADDR       (0x98)
#define MANUAL_SHUTTER_H_ADDR       (0x99)
#define MANUAL_GAIN_L_ADDR          (0x9A)
#define MANUAL_GAIN_H_ADDR          (0x9B)

#define WB_MODE_ADDR            (0x78)
#define WB_MODE_ON              (0x03)
#define WB_MODE_OFF             (0x00)

#define SHARPEN_MODE_ADDR       (0x58)
#define SHARPEN_MODE_ON_HIGH    (0x0F)
#define SHARPEN_MODE_ON_MID     (0x07)
#define SHARPEN_MODE_ON_LOW     (0x03)
#define SHARPEN_MODE_OFF        (0x00)
#define SHARPEN_MODE_MANUAL     (0x03)

#define SHARPEN_DEGREE_ADDR       	(0xce)
#define SHARPEN_DEGREE_AUTO_HIGH    (0x07)
#define SHARPEN_DEGREE_AUTO_MID     (0x07)
#define SHARPEN_DEGREE_AUTO_LOW     (0x03)
#define SHARPEN_DEGREE_MANUAL_ON    (0x01)
#define SHARPEN_DEGREE_MANUAL_OFF   (0x00)


#define DN_2D_MODE_ADDR          (0x50)
#define DN_2D_DEGREE_HIGH        (0x7f)
#define DN_2D_DEGREE_MID         (0x77)
#define DN_2D_DEGREE_LOW         (0x37)
#define DN_2D_MODE_ON             (0x13)
#define DN_2D_MODE_OFF            (0x00)

#define DN_3D_MODE_ADDR          (0x68)
#define DN_3D_MODE_ON            (0x01)
#define DN_3D_MODE_OFF           (0x00)

#define DN_3D_DEGREE_ADDR        (0x6f)
#define DN_3D_DEGREE_HIGH        (0x30)
#define DN_3D_DEGREE_MID         (0x28)
#define DN_3D_DEGREE_LOW         (0x20)
#define DN_3D_DEGREE_DEF         (0x28)

#define JUDGE_BRIGHT_ADDR		(0x96)


#define BRIGHTNESS_ADDR         (0xCB)
#define CONTRAST_ADDR          (0xCA)
#define SATURATION_ADDR        (0xCD)

#define WDR_MODE_ADDR           (0x38)
#define WDR_MODE_ON             (0x37)//(0x77)
#define WDR_MODE_OFF            (0x00)
#define WDR_MODE_MANUAL         (0x33)

#define WDR_MODE_HIGH           (0x33)
#define WDR_MODE_MID            (0x27)
#define WDR_MODE_LOW            (0x30)
 
#define DEFOGGING_MODE_ADDR      (0x38)
#define DEFOGGING_MODE_ON        (0x77)
#define DEFOGGING_MODE_OFF        (0x00)
#define DEFOGGING_MODE_MANUAL    (0x33)

#define EXPOSURE_AUTO           (0x8b)
#define EXPOSURE_SHUTTER_PRI    (0x8a)
#define EXPOSURE_GAIN_PRI       (0x89)
#define EXPOSURE_MANUAL_MODE    (0x88)

#define BRIGHTNESS_DEF_VAL      (128)
#define CONTRAST_DEF_VAL        (128)
#define SATURATION_DEF_VAL      (128)
#define SHUTTER_MAX_DEF_VAL     (200)
#define GAIN_MAX_DEF_VAL        (600)

#define SPI_COM_WAIT          (40000) //us

typedef enum mode_val{
    MODE_ON =1,
    MODE_OFF=0,
    MODE_MANUAL=2
}MODE_VAL;

typedef enum level_val{
   LEVEL_H = 0,
   LEVEL_M=1,
   LEVEL_L=2
}LEVEL_VAL;

typedef struct isp_param_info{
    unsigned int uStartHour;
    unsigned int uStartMinute;
    unsigned int uEndHour;
    unsigned int uEndMinute;
    
    unsigned int nExposureMode;
    unsigned int nShutterMax;
    unsigned int nGainMax;
    unsigned int nManualShutter;
    unsigned int nManualGain;

    unsigned int nWhiteBalanceMode;
    unsigned int nSharpenMode;
    unsigned int nSharpenLevel;
    
    unsigned int n2DnoiseMode;
    unsigned int n3DnoiseMode;

    unsigned int nBrightness;
    unsigned int nContrast;
    unsigned int nSaturation;
    
    unsigned int nGammaMode;
    unsigned int nWideDynamicRangeMode;    
    unsigned int nWideDynamicRangeLevel;

    unsigned int nDefogMode;
    unsigned int nDefogLevel;
}ISP_PARAM_INFO_S,*LPISP_PARAM_INFO_S;


typedef struct param_send_tag
{
	char addr1;
	char data1;
	char addr2;
	char data2;
}ARM2FPGA_SEND_INFO;

typedef struct param_rev_tag
{
	char addr1;
	char addr2;
}ARM2FPGA_RECV_INFO;

int ispCCDParamInit(void);
#if 0
void newIspExposeSet(int iShutterMode, int iGainMode, int iFixShutVal, int iFixGainVal, int iMaxShutVal, int iMaxGainVal);
int newIspParamSet(int iMode, int iType, int iFixVal);
#endif
int ispI2cCmdSet(char * szI2cCmd);
int ispI2cCmdGet(char * szI2cCmd);

int pIrisPosAdjust(char cPos);

#endif

#endif

