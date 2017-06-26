#ifndef _ISP_V2_PARAM_H_
#define _ISP_V2_PARAM_H_

#if defined(CYC800XKW4C)

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>

#define TOTAL_LINES         (2045)

#define ARM2FPGA_DEV          "/dev/misc/armtofpga"

#define CMD_WRITE             (15)
#define CMD_READ              (16)

#define MAX_ISP_PARA_SET       (4)

#define EXPOSURE_MODE_ADDR                   (0x40)

#define EXPOSURE_AUTO_SHUTTER_MAX_L_ADDR     (0x4c)
#define EXPOSURE_AUTO_SHUTTER_MAX_H_ADDR     (0x4D)

#define EXPOSURE_AUTO_SHUTTER_MID_L_ADDR     (0x4E)  
#define EXPOSURE_AUTO_SHUTTER_MID_H_ADDR     (0x4F)

#define EXPOSURE_AUTO_SHUTTER_MIN_L_ADDR     (0x50)  
#define EXPOSURE_AUTO_SHUTTER_MIN_H_ADDR     (0x51)


#define EXPOSURE_AUTO_GAIN_MAX_L_ADDR        (0x52)
#define EXPOSRE_AUTO_GAIN_MAX_H_ADDR         (0x53)

#define EXPOSURE_AUTO_GAIN_MID_L_ADDR        (0x54)
#define EXPOSRE_AUTO_GAIN_MID_H_ADDR         (0x55)

#define EXPOSURE_AUTO_GAIN_MIN_L_ADDR        (0x56)
#define EXPOSRE_AUTO_GAIN_MIN_H_ADDR         (0x57)


#define MANUAL_SHUTTER_L_ADDR       (0x48)
#define MANUAL_SHUTTER_H_ADDR       (0x49)
#define MANUAL_GAIN_L_ADDR          (0x4A)
#define MANUAL_GAIN_H_ADDR          (0x4B)

#define WB_MODE_ADDR            	(0xa0)
#define WB_MODE_ON              	(0x13)
#define WB_MODE_OFF             	(0x00)

#define SHARPEN_MODE_ADDR       	(0x150)
#define SHARPEN_MODE_AUTO_HIGH    	(0x3F)
#define SHARPEN_MODE_AUTO_LOW     	(0x0f)

#define SHARPEN_MODE_MANUAL     	(0x03)
#define SHARPEN_MODE_OFF        	(0x00)

#define DN_2D_MODE_ADDR1          (0x100)
#define DN_2D_MODE_ADDR2          (0x101)

#define DN_2D_AUTO_HIGH1		 (0x7f)
#define DN_2D_AUTO_HIGH2		 (0x8f)

#define DN_2D_AUTO_MID1          (0x7f)
#define DN_2D_AUTO_MID2          (0xe0)

#define DN_2D_AUTO_LOW1	         (0x7f)
#define DN_2D_AUTO_LOW2 	     (0x80)

#define DN_2D_MANUAL_ON1         (0x07)
#define DN_2D_MANUAL_ON2         (0x00)

#define DN_2D_MODE_OFF	    	 (0x00)


#define DN_3D_MODE_ADDR1         (0x140)
#define DN_3D_MODE_ADDR2         (0x141)

#define DN_3D_AUTO_EN1			 (0xff)
#define DN_3D_AUTO_EN2			 (0x01)

#define DN_3D_MANUAL_ON1         (0x1f)
#define DN_3D_MANUAL_ON2         (0x00)

#define DN_3D_MODE_OFF1	    	 (0x00)
#define DN_3D_MODE_OFF2 	     (0x00)


#define JUDGE_BRIGHT_ADDR		(0x46)


#define BRIGHTNESS_ADDR         (0x163)
#define CONTRAST_ADDR           (0x162)
#define SATURATION_ADDR         (0x164)

#define WDR_MODE_ADDR           (0x118)

#define WDR_MODE_AUTO           (0x1f)
#define WDR_MODE_OFF            (0x00)
#define WDR_MODE_MANUAL         (0x0f)


#define EXPOSURE_AUTO           (0x03)
#define EXPOSURE_SHUTTER_PRI    (0x01)
#define EXPOSURE_GAIN_PRI       (0x02)
#define EXPOSURE_MANUAL_MODE    (0x00)

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
void newIspExposeSet(int iShutterMode, int iGainMode, int iFixShutVal, int iFixGainVal, int iMaxShutVal, int iMaxGainVal);
int newIspParamSet(int iMode, int iType, int iFixVal);
int ispI2cCmdSet(char * szI2cCmd);
int ispI2cCmdGet(char * szI2cCmd);

#endif

#endif

