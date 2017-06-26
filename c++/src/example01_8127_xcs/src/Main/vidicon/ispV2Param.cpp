#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ispV2Param.h"
#include "icx274CCDCtl.h"
#include "dsp.h"
#include "dspParamManage.h"

#if defined(CYC800XKW4C)

int m_arm2fpga_fd =-1;
ARM2FPGA_SEND_INFO m_cmd_send_info;

#define ARM2FPGA_WRITE2(addr,data,slp) do{\
    memset(&m_cmd_send_info, 0x00, sizeof(m_cmd_send_info));\
    if(addr > 0xff)\
    {\
    	m_cmd_send_info.addr1 = (addr&0xff);\
	    m_cmd_send_info.data1 = (data);\
    }\
    else\
    {\
	    m_cmd_send_info.addr1 = (addr);\
	    m_cmd_send_info.data1 = (data);\
    }\
    ret = ioctl(m_arm2fpga_fd,CMD_WRITE,&m_cmd_send_info);\
    usleep(slp);\
}while(0)

#define ARM2FPGA_READ(addr,data,slp) do{\
    memset(&m_cmd_send_info, 0x00, sizeof(m_cmd_send_info));\
    if(addr > 0xff)\
    {\
    	m_cmd_send_info.addr1 = (addr&0xff);\
	    m_cmd_send_info.data1 = (data);\
    }\
    else\
    {\
	    m_cmd_send_info.addr1 = (addr);\
	    m_cmd_send_info.data1 = (data);\
    }\
    ret = ioctl(m_arm2fpga_fd,CMD_READ,&m_cmd_send_info);\
    usleep(slp);\
}while(0)

/* isp i2c bank选择 */
int ispI2cBankSelect(int iBank)
{	
	int ret =0;
	ARM2FPGA_WRITE2(0xf1, iBank, SPI_COM_WAIT); 
	return ret;
}

int ispI2cCmdSet(char * szI2cCmd)
{
    int ret =0;
	char *delim=" ";   
    char *pcTmp = NULL;
    unsigned short usI2cAddr=0x0;
    unsigned char ucI2cData=0x0;
    
    if(NULL == szI2cCmd)
    {
        ICX274CTL_ERROR("v2 %s %d  in param is null.\n",__FUNCTION__,__LINE__);
        return -1;
    }

	pcTmp = strtok(szI2cCmd,delim);
    usI2cAddr = strtol(pcTmp, NULL, 16);

	pcTmp = strtok(NULL,delim);
    ucI2cData = strtol(pcTmp, NULL, 16);

    ICX274CTL_ERROR("v2 %s %d write cmd is:%s,i2cAddr:0x%x, i2cData:0x%x.\n",__FUNCTION__,__LINE__, szI2cCmd, usI2cAddr, ucI2cData);

    if(usI2cAddr > 0xff)
    {
    	ispI2cBankSelect(1);
    }
    else
    {
    	ispI2cBankSelect(0);
    }
    
    ARM2FPGA_WRITE2(usI2cAddr, ucI2cData, SPI_COM_WAIT); 

    ARM2FPGA_READ(usI2cAddr, ucI2cData, SPI_COM_WAIT);

    ICX274CTL_ERROR("v2 %s %d read i2c data,Addr:0x%x,Data:0x%x.\n",__FUNCTION__,__LINE__, usI2cAddr, m_cmd_send_info.data1);
    
    return 0;
}

int ispI2cCmdGet(char * szI2cCmd)
{
    int ret =0;
	char *delim=" ";
    char *pcTmp = NULL;
    unsigned short usI2cAddr=0x0;
    unsigned char ucI2cData=0x0;

    if(NULL == szI2cCmd)
    {
        ICX274CTL_ERROR("v2 %s %d  in param is null.\n",__FUNCTION__,__LINE__);
        return -1;
    }

	pcTmp = strtok(szI2cCmd,delim);
    usI2cAddr = strtol(pcTmp, NULL, 16);

    if(usI2cAddr > 0xff)
    {
    	ispI2cBankSelect(1);
    }
    else
    {
    	ispI2cBankSelect(0);
    }

    ARM2FPGA_READ(usI2cAddr, ucI2cData, SPI_COM_WAIT);

    ICX274CTL_ERROR("v2 %s %d read i2c data,Addr:0x%x,Data:0x%x.\n",__FUNCTION__,__LINE__, usI2cAddr, m_cmd_send_info.data1);
    
    return 0;
}

int ispCCDParamInit(void)
{
    int ret =0;
    m_arm2fpga_fd = open(ARM2FPGA_DEV,O_RDWR);

    if(m_arm2fpga_fd < 0)
    {
        ICX274CTL_ERROR("v2 %s %d  open [%d] failed \n",__FILE__,__LINE__, ARM2FPGA_DEV);
        return -1;
    }
    
    /* 白平衡固定为自动模式 */
    ispI2cBankSelect(0);
    ARM2FPGA_WRITE2(WB_MODE_ADDR,WB_MODE_ON, SPI_COM_WAIT);
    
    return 0;
}


void newIspExposeSet(int iShutterMode, int iGainMode, int iFixShutVal, int iFixGainVal, int iMaxShutVal, int iMaxGainVal)
{
    int ret =0;
    int iTotalLine =0;

	ispI2cBankSelect(0);

	iTotalLine = newIspTotalLinesGet();
    /* 增加模式控制 */
    if(( (iShutterMode == INTERVAL) && ( iGainMode == INTERVAL) ) \
    ||((iShutterMode == AUTO) && ( iGainMode == AUTO) ) )
    {
        unsigned int uMidExp=0;
        unsigned int uMaxExp = iTotalLine/iMaxShutVal;
        if(uMaxExp <= 0)
        {
            uMaxExp =1;
        }

        unsigned int uMinExp = iTotalLine/iFixShutVal;
        if(uMinExp <= 0)
        {
            uMinExp =1;
        }

        uMidExp = ( uMinExp + uMaxExp)/2;

        ICX274CTL_ERROR("v2 %s %d,iMaxShutVal:%d,uMaxExp:%d,iMaxGainVal:%d.\n",\
        __FUNCTION__, __LINE__, iMaxShutVal, uMaxExp, iMaxGainVal);

        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_AUTO, SPI_COM_WAIT);


        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MIN_L_ADDR,(0x04&0xFF), SPI_COM_WAIT);     
        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MIN_H_ADDR,((0x04>>8)&0xF), SPI_COM_WAIT);
        
        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MID_L_ADDR,(uMidExp&0xFF), SPI_COM_WAIT);     
        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MID_H_ADDR,((uMidExp>>8)&0xF), SPI_COM_WAIT);

        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MAX_L_ADDR,(uMaxExp&0xFF), SPI_COM_WAIT);     
        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MAX_H_ADDR,((uMaxExp>>8)&0xF), SPI_COM_WAIT);

        ARM2FPGA_WRITE2(EXPOSURE_AUTO_GAIN_MIN_L_ADDR, iFixGainVal&0xFF,    SPI_COM_WAIT); 
        ARM2FPGA_WRITE2(EXPOSRE_AUTO_GAIN_MIN_H_ADDR,  (iFixGainVal>>8)&0xF, SPI_COM_WAIT); 

        ARM2FPGA_WRITE2(EXPOSURE_AUTO_GAIN_MID_L_ADDR, ((iFixGainVal+iMaxGainVal)/2)&0xFF, SPI_COM_WAIT); 
        ARM2FPGA_WRITE2(EXPOSRE_AUTO_GAIN_MID_H_ADDR,  (((iFixGainVal+iMaxGainVal)/2)>>8)&0xF, SPI_COM_WAIT); 

        ARM2FPGA_WRITE2(EXPOSURE_AUTO_GAIN_MAX_L_ADDR, iMaxGainVal&0xFF, SPI_COM_WAIT); 
        ARM2FPGA_WRITE2(EXPOSRE_AUTO_GAIN_MAX_H_ADDR,  (iMaxGainVal>>8)&0xF, SPI_COM_WAIT); 
        
    }
    else if( ((iShutterMode == FIX) && ( iGainMode == AUTO ))\
    || ((iShutterMode == FIX) && ( iGainMode == INTERVAL ) ) )
    {
        unsigned int uExp = iTotalLine/iFixShutVal;

        if(uExp <= 0)
        {
            uExp =1;
        }

        ICX274CTL_ERROR("v2 %s %d,iFixShutVal:%d,uExp:%d\n",__FUNCTION__, __LINE__, iFixShutVal, uExp);

        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_SHUTTER_PRI, SPI_COM_WAIT);
        
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_L_ADDR,(uExp&0xFF), SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_H_ADDR,((uExp>>8)&0xF), SPI_COM_WAIT);   
    }
    else if( ((iShutterMode == AUTO) && ( iGainMode == FIX))\
    || ( (iShutterMode == INTERVAL) && ( iGainMode == FIX) ))
    {
        ICX274CTL_ERROR("v2 %s %d,iFixGainVal:%d\n",__FUNCTION__, __LINE__,iFixGainVal);
        
        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_GAIN_PRI, SPI_COM_WAIT);
        
        ARM2FPGA_WRITE2(MANUAL_GAIN_L_ADDR,iFixGainVal&0xFF, SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_GAIN_H_ADDR,( iFixGainVal >> 8)&0xF, SPI_COM_WAIT);
    }
    else if( ( iShutterMode == FIX ) && ( iGainMode == FIX ) ) 
    {
        unsigned int uExp = iTotalLine/iFixShutVal;

        if(uExp <= 0)
        {
            uExp =1;
        }

        ICX274CTL_ERROR("v2 %s %d,iFixShutVal:%d,uExp:%d\n",__FUNCTION__, __LINE__, iFixShutVal, uExp);

        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_MANUAL_MODE, SPI_COM_WAIT);
        
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_L_ADDR,(uExp&0xFF), SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_H_ADDR,((uExp>>8)&0xF), SPI_COM_WAIT);   

        ARM2FPGA_WRITE2(MANUAL_GAIN_L_ADDR,iFixGainVal&0xFF, SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_GAIN_H_ADDR,( iFixGainVal >> 8)&0xF, SPI_COM_WAIT);
    }
    else
    {
        ICX274CTL_ERROR("v2 %s %d  mode error,shutMode:%d,gainMode:%d.\n",__FILE__,__LINE__, iShutterMode, iGainMode);
    }

}

int newIspParamSet(int iMode, int iType, int iFixVal)
{
    int ret =0;

    ICX274CTL_ERROR("v2 %s %d,iMode:%d, iType:%d, iFixVal:%d.\n",__FUNCTION__,__LINE__, iMode, iType, iFixVal);
	ispI2cBankSelect(1);

    switch(iType)
    {
        case CAMERA_PARAM_GREY:
        {
        	ispI2cBankSelect(0);
            /* 透雾更换为配置目标亮度值 */
            ARM2FPGA_WRITE2(JUDGE_BRIGHT_ADDR, iFixVal, SPI_COM_WAIT);
            ICX274CTL_INFO("v2 目标亮度设置值:%d.\n",iFixVal);
        }
        break;
   
        case CAMERA_HAND_CONTRAST:
        {
            if( 0 == iFixVal )
            {
                ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_OFF, SPI_COM_WAIT);
                ICX274CTL_INFO("v2 宽动态 关,iFixVal:%d.\n",iFixVal);
            }
            else if(1 == iFixVal)
            {
                ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_AUTO, SPI_COM_WAIT);
                ICX274CTL_INFO("v2 宽动态 自动,iFixVal:%d.\n",iFixVal);
            }
            else if( 2 == iFixVal ) /* high */
            {

                ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_MANUAL,SPI_COM_WAIT);         

                 ARM2FPGA_WRITE2(0x11a,0x80,SPI_COM_WAIT);
                 ARM2FPGA_WRITE2(0x11c,0x10,SPI_COM_WAIT);

                 ICX274CTL_INFO("v2 宽动态 高,iFixVal:%d.\n",iFixVal);
            }
            else if(3 == iFixVal)
            {
                 ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_MANUAL,SPI_COM_WAIT);
                 
                 ARM2FPGA_WRITE2(0x11a,0x60,SPI_COM_WAIT);
                 ARM2FPGA_WRITE2(0x11c,0x18,SPI_COM_WAIT);


                 ICX274CTL_INFO("v2 宽动态 中,iFixVal:%d.\n",iFixVal);
            }
            else if(4 == iFixVal)
            {
                 ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_MANUAL,SPI_COM_WAIT);   
                 
                 ARM2FPGA_WRITE2(0x11a,0x40,SPI_COM_WAIT);
                 ARM2FPGA_WRITE2(0x11c,0x1c,SPI_COM_WAIT);

                 ICX274CTL_INFO("v2 宽动态 低,iFixVal:%d.\n",iFixVal);
            }
        }
        break;
        
        case CAMERA_PARAM_COLOUR:
        {
            /* 后处理亮度配置 */
            ARM2FPGA_WRITE2(BRIGHTNESS_ADDR, iFixVal, SPI_COM_WAIT);
            ICX274CTL_INFO("v2 亮度:%d.\n",iFixVal);
        }
        break;
        
        case CAMERA_PARAM_MINSCALE:
        {
            ARM2FPGA_WRITE2(CONTRAST_ADDR, iFixVal, SPI_COM_WAIT);
            ICX274CTL_INFO("v2 对比度,iFixVal:%d.\n",iFixVal);
        }
        break;

        case CAMERA_PARAM_SHARPEN:
        {
            ARM2FPGA_WRITE2(SATURATION_ADDR, iFixVal, SPI_COM_WAIT);

            ICX274CTL_INFO("v2 饱和度,iFixVal:%d.\n",iFixVal);
        }
        break;
#if 0       
        case CAMERA_PARAM_WB:
        {
            /* 白平衡更改为3D降噪 */
            if( 0 == iFixVal )
            {
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR1,DN_3D_MODE_OFF1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR2,DN_3D_MODE_OFF2, SPI_COM_WAIT);

                ICX274CTL_INFO("v2 3D降噪 关,iFixVal:%d.\n",iFixVal);
            }
            else if( 1 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR1,DN_3D_AUTO_EN1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR2,DN_3D_AUTO_EN2, SPI_COM_WAIT);

                ICX274CTL_INFO("v2 3D降噪 自动,iFixVal:%d.\n",iFixVal);
            }
            else if( 2 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR1,DN_3D_MANUAL_ON1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR2,DN_3D_MANUAL_ON2, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x145, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x146, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x147, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x148, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x149, 0x0c, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14a, 0x2b, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14b, 0x0c, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14c, 0x2b, SPI_COM_WAIT);

                ICX274CTL_INFO("v2 3D降噪 手动强,iFixVal:%d.\n",iFixVal);
            }
            else if( 3 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR1,DN_3D_MANUAL_ON1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR2,DN_3D_MANUAL_ON2, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x145, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x146, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x147, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x148, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x149, 0x08, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14a, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14b, 0x08, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14c, 0x40, SPI_COM_WAIT);

                ICX274CTL_INFO("v2 3D降噪 手动中,iFixVal:%d.\n",iFixVal);
            }
            else if( 4 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR1,DN_3D_MANUAL_ON1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR2,DN_3D_MANUAL_ON2, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x145, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x146, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x147, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x148, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x149, 0x04, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14a, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14b, 0x04, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x14c, 0x80, SPI_COM_WAIT);

                ICX274CTL_INFO("v2 3D降噪 手动弱,iFixVal:%d.\n",iFixVal);
            }
        }
        break;
#endif
        case CAMERA_GAMMA2:
        {
            /* 清晰度界面显示为锐度 */
            if(0 == iFixVal)
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_OFF, SPI_COM_WAIT);

                ICX274CTL_INFO("v2 清晰度 关,iFixVal:%d.\n",iFixVal);
            }
            else if(1 == iFixVal)
            {        
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_AUTO_HIGH, SPI_COM_WAIT);    

                ICX274CTL_INFO("v2 清晰度 自动强,iFixVal:%d.\n",iFixVal);
            }
            else if(2 == iFixVal)
            {        
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_AUTO_LOW, SPI_COM_WAIT);    

                ICX274CTL_INFO("v2 清晰度 自动弱,iFixVal:%d.\n",iFixVal);
            }            
            else if(3 == iFixVal)
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_MANUAL,SPI_COM_WAIT); 
                
                 ARM2FPGA_WRITE2(0x151, 0x04, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x152, 0x04, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x157, 0xc0, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x158, 0xc0, SPI_COM_WAIT); 

                 ICX274CTL_INFO("v2 清晰度 高,iFixVal:%d.\n",iFixVal);
            }
            else if( 4 == iFixVal)
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_MANUAL,SPI_COM_WAIT); 
                
                 ARM2FPGA_WRITE2(0x151, 0x08, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x152, 0x08, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x157, 0x90, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x158, 0x90, SPI_COM_WAIT); 

                 ICX274CTL_INFO("v2 清晰度 中,iFixVal:%d.\n",iFixVal);
            }
            else if( 5 == iFixVal )
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_MANUAL,SPI_COM_WAIT); 
                
                 ARM2FPGA_WRITE2(0x151, 0x0c, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x152, 0x0c, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x157, 0x60, SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x158, 0x60, SPI_COM_WAIT); 

                 ICX274CTL_INFO("v2 清晰度 低,iFixVal:%d.\n",iFixVal);
            }
        }
        break;
     
        case CAMERA_DENOISE:
        {
            /* 2d降噪 */
            if(0 == iFixVal )
            {
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_MODE_OFF, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_MODE_OFF, SPI_COM_WAIT);
                
                ICX274CTL_INFO("v2 2D降噪 关,iFixVal:%d.\n",iFixVal);
            }
            else if( 1 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_AUTO_HIGH1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_AUTO_HIGH2, SPI_COM_WAIT);
                
                ICX274CTL_INFO("v2 2D降噪 自动强,iFixVal:%d.\n",iFixVal);
            }
            else if( 2 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_AUTO_MID1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_AUTO_MID2, SPI_COM_WAIT);
                
                ICX274CTL_INFO("v2 2D降噪 自动中,iFixVal:%d.\n",iFixVal);
            }
            else if( 3 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_AUTO_LOW1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_AUTO_LOW2, SPI_COM_WAIT);
                
                ICX274CTL_INFO("v2 2D降噪 自动弱,iFixVal:%d.\n",iFixVal);
            }
            else if( 4 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_MANUAL_ON1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_MANUAL_ON2, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x101, 0x60, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x102, 0xff, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x103, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x104, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x104, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x106, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x107, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x108, 0xff, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x109, 0x04, SPI_COM_WAIT);
                ICX274CTL_INFO("v2 2D降噪 手动强,iFixVal:%d.\n",iFixVal);
            }
            else if( 5 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_MANUAL_ON1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_MANUAL_ON2, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x101, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x102, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x103, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x104, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x104, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x106, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x107, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x108, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x109, 0x08, SPI_COM_WAIT);
                ICX274CTL_INFO("v2 2D降噪 手动中,iFixVal:%d.\n",iFixVal);
            }
            else if( 6 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR1, DN_2D_MANUAL_ON1, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR2, DN_2D_MANUAL_ON2, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x101, 0x20, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x102, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x103, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x104, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x104, 0x80, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x106, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x107, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x108, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x109, 0x10, SPI_COM_WAIT);
                ICX274CTL_INFO("v2 2D降噪 手动弱,iFixVal:%d.\n",iFixVal);
            }            
        }
        break;

        default:
            break;
    }
    return 0;
}

#endif


