#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include "ispParam.h"
#include "icx274CCDCtl.h"
#include "dsp.h"
#include "dspParamManage.h"

#define USE_PIRIS_FUN_ENABLE 0 	/* 是否使用自动光圈功能 */

#if defined(CYC500XKW4C)||defined(CYC500XAW)
static char s_cPIrisPos =0; /* 自动光圈初始位置 */

int m_arm2fpga_fd =-1;
ARM2FPGA_SEND_INFO m_cmd_send_info;

#define ARM2FPGA_WRITE2(addr,data,slp) do{\
    memset(&m_cmd_send_info, 0x00, sizeof(m_cmd_send_info));\
    m_cmd_send_info.addr1 = (addr);\
    m_cmd_send_info.data1 = (data);\
    ret = ioctl(m_arm2fpga_fd,CMD_WRITE,&m_cmd_send_info);\
    usleep(slp);\
}while(0)

#define ARM2FPGA_READ(addr,data,slp) do{\
    memset(&m_cmd_send_info, 0x00, sizeof(m_cmd_send_info));\
    m_cmd_send_info.addr1 = (addr);\
    m_cmd_send_info.data1 = (data);\
    ret = ioctl(m_arm2fpga_fd,CMD_READ,&m_cmd_send_info);\
    usleep(slp);\
}while(0)

#define CYCLE_DELAY (0*1000)
#define OUT_1 1
#define OUT_0   0

struct arm2fpga_comm_packet
{
	unsigned short packet_head;
	unsigned char  packet_dir;
	unsigned char  packet_payload;
	unsigned short packet_cmdid;
	unsigned char  packet_param_num;
	unsigned char  packet_param[36];
	unsigned char  packet_crc;
}__attribute__((packed));

unsigned char  make_packet_crc(unsigned char *buff,int  len)
{
	int i = 0 ;
	unsigned char crc = 0 ;
	
	for ( i = 0 ;i <len ; i ++)
	{
			crc += buff[i];
	}
	return crc;
}

int arm2fpga(int fd, unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
	int ret = 0;
	struct  arm2fpga_comm_packet  packet;
	memset(&packet,0x00,sizeof(packet));
	
	packet.packet_head 		= 0x5943;
	packet.packet_dir 		= 0;
	packet.packet_payload 	= 40;
	packet.packet_cmdid 	= 0x0026;  /*光圈控制FPGA 命令字*/
	packet.packet_param_num = 4;
	packet.packet_param[0] 	= a; //a; +
	packet.packet_param[1] 	= b; //b; -
	packet.packet_param[2] 	= c; //c; +
	packet.packet_param[3] 	= d; //d; -
	packet.packet_crc		= make_packet_crc((unsigned char*)&packet,sizeof(packet)-1);
	
	ret = write(fd,(unsigned char *)&packet, sizeof(packet));
	if(ret == 0)
	{
		//ICX274CTL_ERROR("%s %d write i2cAddr:0x%x, i2cData:%d,%d,%d,%d\n",__FUNCTION__,__LINE__, packet.packet_cmdid,\
		//packet.packet_param[0],packet.packet_param[1], packet.packet_param[2], packet.packet_param[3]);
	}
	return ret; // 0:成功，1: 不成功
}

int pIrisIncrease(int fd_check)
{
	int ret,i;
	int iCycle =1;
	unsigned int step[4]={0};

	/*1 0 0 0 */
	step[0] = OUT_0;
	step[1] = OUT_0;
	step[2] = OUT_0;
	step[3] = OUT_1;
	
	for ( i = 0 ;i <iCycle; i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);
	
	/* 1 0 1 0 */
	
	step[0] = OUT_0;
	step[1] = OUT_0;
	step[2] = OUT_1;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);		
			
	/* 0 0 1 0 */
	step[0] = OUT_1;
	step[1] = OUT_0;
	step[2] = OUT_1;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);					

	/* 0 1 1 0 */
	step[0] = OUT_1;
	step[1] = OUT_1;
	step[2] = OUT_1;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);				
				
  /* 0 1 0 0  */
	step[0] = OUT_1;
	step[1] = OUT_1;
	step[2] = OUT_0;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);			
	
  /* 0 1 0 1 */
	step[0] = OUT_1;
	step[1] = OUT_1;
	step[2] = OUT_0;
	step[3] = OUT_0;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);		

	/* 0 0 0 1 */
	step[0] = OUT_1;
	step[1] = OUT_0;
	step[2] = OUT_0;
	step[3] = OUT_0;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);
	
  /* 1 0 0 1 */
	step[0] = OUT_0;
	step[1] = OUT_0;
	step[2] = OUT_0;
	step[3] = OUT_0;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(500*1000);	

}

int pIrisReduce(int fd_check)
{
	int ret,i;
	int iCycle =1;
	unsigned int step[4]={0};

	/*1 0 0 0 */
	step[0] = OUT_0;
	step[1] = OUT_0;
	step[2] = OUT_0;
	step[3] = OUT_1;
	
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);
	
	/* 1 0 1 0 */
	
	step[0] = OUT_0;
	step[1] = OUT_0;
	step[2] = OUT_0;
	step[3] = OUT_0;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);			
			
	/* 0 0 1 0 */
	step[0] = OUT_1;
	step[1] = OUT_0;
	step[2] = OUT_0;
	step[3] = OUT_0;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);					

	/* 0 1 1 0 */
	step[0] = OUT_1;
	step[1] = OUT_1;
	step[2] = OUT_0;
	step[3] = OUT_0;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);					
				
  /* 0 1 0 0  */
	step[0] = OUT_1;
	step[1] = OUT_1;
	step[2] = OUT_0;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);
	
  /* 0 1 0 1 */
	step[0] = OUT_1;
	step[1] = OUT_1;
	step[2] = OUT_1;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);

	/* 0 0 0 1 */
	step[0] = OUT_1;
	step[1] = OUT_0;
	step[2] = OUT_1;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	usleep(CYCLE_DELAY);			
	
  /* 1 0 0 1 */
	step[0] = OUT_0;
	step[1] = OUT_0;
	step[2] = OUT_1;
	step[3] = OUT_1;
	for ( i = 0 ;i <iCycle;i++)
	{
		ret = arm2fpga(fd_check,step[0],step[1],step[2],step[3]);
	}
	//usleep(CYCLE_DELAY);	
	usleep(5*1000);

	return 1;
}

int pIrisInit(void)
{
#if USE_PIRIS_FUN_ENABLE
	int i =0;
	s_cPIrisPos =0;
	for(i=0; i< 15; i++)
	{
		pIrisIncrease(m_arm2fpga_fd);
	}
#endif
	return 1;
}

int pIrisPosAdjust(char cPos)
{
#if USE_PIRIS_FUN_ENABLE
	int i =0;
	char cAdjustVal =0;

	ICX274CTL_ERROR("%s %d: cPos:%d, IrisCurPos:%d.\n",__FUNCTION__,__LINE__, cPos, s_cPIrisPos);
	
	if(cPos > s_cPIrisPos)
	{
		cAdjustVal = cPos - s_cPIrisPos;
		for(i =0; i< cAdjustVal; i++)
		{
			pIrisReduce(m_arm2fpga_fd);
		}
	}
	else
	{
		cAdjustVal = s_cPIrisPos - cPos;
		for(i =0; i< cAdjustVal; i++)
		{
			pIrisIncrease(m_arm2fpga_fd);
		}
	}
	s_cPIrisPos = cPos;
#endif

	return 1;
}

int ispI2cCmdSet(char * szI2cCmd)
{
    int ret =0;
	char *delim=" ";   
    char *pcTmp = NULL;
    unsigned char ucI2cAddr=0x0;
    unsigned char ucI2cData=0x0;
    

    if(NULL == szI2cCmd)
    {
        ICX274CTL_ERROR("%s %d  in param is null.\n",__FUNCTION__,__LINE__);
        return -1;
    }

	pcTmp = strtok(szI2cCmd,delim);
    ucI2cAddr = strtol(pcTmp, NULL, 16);

	pcTmp = strtok(NULL,delim);
    ucI2cData = strtol(pcTmp, NULL, 16);

    ICX274CTL_ERROR("%s %d write cmd is:%s,i2cAddr:0x%x, i2cData:0x%x.\n",__FUNCTION__,__LINE__, szI2cCmd, ucI2cAddr, ucI2cData);
    
    ARM2FPGA_WRITE2(ucI2cAddr, ucI2cData, SPI_COM_WAIT); 

    ARM2FPGA_READ(ucI2cAddr, ucI2cData, SPI_COM_WAIT);

    ICX274CTL_ERROR("%s %d read i2c data,Addr:0x%x,Data:0x%x.\n",__FUNCTION__,__LINE__, m_cmd_send_info.addr1, m_cmd_send_info.data1);
    
    return 0;
}

int ispI2cCmdGet(char * szI2cCmd)
{
    int ret =0;
	char *delim=" ";
    char *pcTmp = NULL;
    unsigned char ucI2cAddr=0x0;
    unsigned char ucI2cData=0x0;

    if(NULL == szI2cCmd)
    {
        ICX274CTL_ERROR("%s %d  in param is null.\n",__FUNCTION__,__LINE__);
        return -1;
    }

	pcTmp = strtok(szI2cCmd,delim);
    ucI2cAddr = strtol(pcTmp, NULL, 16);

    ARM2FPGA_READ(ucI2cAddr, ucI2cData, SPI_COM_WAIT);

    ICX274CTL_ERROR("%s %d read i2c data,Addr:0x%x,Data:0x%x.\n",__FUNCTION__,__LINE__, m_cmd_send_info.addr1, m_cmd_send_info.data1);
    
    return 0;
}

int ispCCDParamInit(void)
{
    int ret =0;
    m_arm2fpga_fd = open(ARM2FPGA_DEV,O_RDWR);

    if(m_arm2fpga_fd < 0)
    {
        ICX274CTL_ERROR("%s %d  open [%d] failed \n",__FILE__,__LINE__, ARM2FPGA_DEV);
        return -1;
    }

	/* 自动光圈初始化为最大光圈 */
	pIrisInit();
    
    ARM2FPGA_WRITE2(0xc8, 0x5f, SPI_COM_WAIT);

    /* 白平衡固定为自动模式 */
    ARM2FPGA_WRITE2(WB_MODE_ADDR,WB_MODE_ON, SPI_COM_WAIT);
    
    return 0;
}

#if 0
void newIspExposeSet(int iShutterMode, int iGainMode, int iFixShutVal, int iFixGainVal, int iMaxShutVal, int iMaxGainVal)
{
    int ret =0;
    int iTotalLine =0;

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

        ICX274CTL_ERROR("%s %d,iMaxShutVal:%d,uMaxExp:%d,iMaxGainVal:%d,uMinExp:%d. \n",\
        __FUNCTION__, __LINE__, iMaxShutVal, uMaxExp, iMaxGainVal, uMinExp);

        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_AUTO, SPI_COM_WAIT);


        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MIN_L_ADDR,(uMinExp&0xFF), SPI_COM_WAIT);     
        ARM2FPGA_WRITE2(EXPOSURE_AUTO_SHUTTER_MIN_H_ADDR,((uMinExp>>8)&0xF), SPI_COM_WAIT);
        
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

        ICX274CTL_ERROR("%s %d,iFixShutVal:%d,uExp:%d\n",__FUNCTION__, __LINE__, iFixShutVal, uExp);

        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_SHUTTER_PRI, SPI_COM_WAIT);
        
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_L_ADDR,(uExp&0xFF), SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_H_ADDR,((uExp>>8)&0xF), SPI_COM_WAIT);   
    }
    else if( ((iShutterMode == AUTO) && ( iGainMode == FIX))\
    || ( (iShutterMode == INTERVAL) && ( iGainMode == FIX) ))
    {
        ICX274CTL_ERROR("%s %d,iFixGainVal:%d\n",__FUNCTION__, __LINE__,iFixGainVal);
        
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

        ICX274CTL_ERROR("%s %d,iFixShutVal:%d,uExp:%d\n",__FUNCTION__, __LINE__, iFixShutVal, uExp);

        ARM2FPGA_WRITE2(EXPOSURE_MODE_ADDR,EXPOSURE_MANUAL_MODE, SPI_COM_WAIT);
        
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_L_ADDR,(uExp&0xFF), SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_SHUTTER_H_ADDR,((uExp>>8)&0xF), SPI_COM_WAIT);   

        ARM2FPGA_WRITE2(MANUAL_GAIN_L_ADDR,iFixGainVal&0xFF, SPI_COM_WAIT);
        ARM2FPGA_WRITE2(MANUAL_GAIN_H_ADDR,( iFixGainVal >> 8)&0xF, SPI_COM_WAIT);
    }
    else
    {
        ICX274CTL_ERROR("%s %d  mode error,shutMode:%d,gainMode:%d.\n",__FILE__,__LINE__, iShutterMode, iGainMode);
    }

}

int newIspParamSet(int iMode, int iType, int iFixVal)
{
    int ret =0;

    ICX274CTL_ERROR("%s %d,iMode:%d, iType:%d, iFixVal:%d.\n",__FUNCTION__,__LINE__, iMode, iType, iFixVal);

    switch(iType)
    {
        case CAMERA_PARAM_GREY:
        {
            /* 透雾更换为配置目标亮度值 */
            ARM2FPGA_WRITE2(JUDGE_BRIGHT_ADDR, iFixVal, SPI_COM_WAIT);
            ICX274CTL_INFO("目标亮度设置值:%d.\n",iFixVal);
        }
        break;
   
        case CAMERA_HAND_CONTRAST:
        {
            if( 0 == iFixVal )
            {
                ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_OFF, SPI_COM_WAIT);
                ICX274CTL_INFO("宽动态关,iFixVal:%d.\n",iFixVal);
            }
            else if(1 == iFixVal)
            {
                ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_ON, SPI_COM_WAIT);
                ICX274CTL_INFO("宽动态 自动,iFixVal:%d.\n",iFixVal);
            }
            else if( 2 == iFixVal ) /* high */
            {

                ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_HIGH,SPI_COM_WAIT);         

                 ARM2FPGA_WRITE2(0x3f,0x14,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x40,0x10,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x41,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x42,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x43,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x44,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x45,0x10,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x46,0x14,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x47,0x10,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x48,0xc,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x49,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4a,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4b,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4c,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4d,0xc,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4e,0x10,SPI_COM_WAIT); 

                 ICX274CTL_INFO("宽动态 高,iFixVal:%d.\n",iFixVal);
            }
            else if(3 == iFixVal)
            {
                 ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_MID,SPI_COM_WAIT);         
                 ARM2FPGA_WRITE2(0x3f,0x12,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x40,0xe,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x41,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x42,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x43,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x44,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x45,0xe,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x46,0x12,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x47,0xc,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x48,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x49,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4a,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4b,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4c,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4d,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4e,0x10,SPI_COM_WAIT); 

                 ICX274CTL_INFO("宽动态 中,iFixVal:%d.\n",iFixVal);
            }
            else if(4 == iFixVal)
            {
                 ARM2FPGA_WRITE2(WDR_MODE_ADDR,WDR_MODE_LOW,SPI_COM_WAIT);         
                 ARM2FPGA_WRITE2(0x3f,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x40,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x41,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x42,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x43,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x44,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x45,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x46,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x47,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x48,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x49,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4a,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4b,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4c,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4d,0x9,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x4e,0x9,SPI_COM_WAIT); 

                 ICX274CTL_INFO("宽动态 低,iFixVal:%d.\n",iFixVal);
            }
        }
        break;
        
        case CAMERA_PARAM_COLOUR:
        {
            /* 后处理亮度配置 */
            ARM2FPGA_WRITE2(BRIGHTNESS_ADDR, iFixVal, SPI_COM_WAIT);
            ICX274CTL_INFO("亮度:%d.\n",iFixVal);
        }
        break;
        
        case CAMERA_PARAM_MINSCALE:
        {
            ARM2FPGA_WRITE2(CONTRAST_ADDR, iFixVal, SPI_COM_WAIT);
            ICX274CTL_INFO("对比度,iFixVal:%d.\n",iFixVal);
        }
        break;

        case CAMERA_PARAM_WB:
        {
            /* 白平衡更改为3D降噪 */
            if( 0 == iFixVal )
            {
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR,DN_3D_MODE_OFF, SPI_COM_WAIT);            

                ICX274CTL_INFO("3D降噪 关,iFixVal:%d.\n",iFixVal);
            }
            else if( 1 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR,DN_3D_MODE_ON, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_DEGREE_ADDR,DN_3D_DEGREE_HIGH, SPI_COM_WAIT);

                ICX274CTL_INFO("3D降噪 强,iFixVal:%d.\n",iFixVal);
            }
            else if( 2 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR,DN_3D_MODE_ON, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_DEGREE_ADDR,DN_3D_DEGREE_MID, SPI_COM_WAIT);

                ICX274CTL_INFO("3D降噪 中,iFixVal:%d.\n",iFixVal);
            }
            else if( 3 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_3D_MODE_ADDR,DN_3D_MODE_ON, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(DN_3D_DEGREE_ADDR,DN_3D_DEGREE_LOW, SPI_COM_WAIT);

                ICX274CTL_INFO("3D降噪 弱,iFixVal:%d.\n",iFixVal);
            }
        }
        break;

        case CAMERA_PARAM_SHARPEN:
        {
            ARM2FPGA_WRITE2(SATURATION_ADDR, iFixVal, SPI_COM_WAIT);

            ICX274CTL_INFO("饱和度,iFixVal:%d.\n",iFixVal);
        }
        break;

        case CAMERA_GAMMA2:
        {
            /* 清晰度界面显示为锐度 */
            if(0 == iFixVal)
            {        
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_OFF, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_MANUAL_OFF, SPI_COM_WAIT);

                ICX274CTL_INFO("清晰度 关,iFixVal:%d.\n",iFixVal);
            }
            else if(1 == iFixVal)
            {        
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_ON_HIGH, SPI_COM_WAIT);    
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_AUTO_HIGH, SPI_COM_WAIT);
                ICX274CTL_INFO("清晰度 自动强,iFixVal:%d.\n",iFixVal);
            }
            else if(2 == iFixVal)
            {        
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_ON_MID, SPI_COM_WAIT);    
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_AUTO_MID, SPI_COM_WAIT);
                ICX274CTL_INFO("清晰度 自动中,iFixVal:%d.\n",iFixVal);
            }
            else if(3 == iFixVal)
            {        
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_ON_LOW, SPI_COM_WAIT);    
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_AUTO_LOW, SPI_COM_WAIT);
                ICX274CTL_INFO("清晰度 自动弱,iFixVal:%d.\n",iFixVal);
            }            
            else if(4 == iFixVal)
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_MANUAL,SPI_COM_WAIT); 
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_MANUAL_ON, SPI_COM_WAIT);
                 ARM2FPGA_WRITE2(0x59,0x10,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5a,0x40,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5b,0x4,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5c,0xa0,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5d,0x0,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5e,0x0,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5f,0x2,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x60,0xc0,SPI_COM_WAIT); 
                 
                 ARM2FPGA_WRITE2(0xcf,0x04,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0xd0,0xff,SPI_COM_WAIT); 

                 ICX274CTL_INFO("清晰度 高,iFixVal:%d.\n",iFixVal);
            }
            else if( 5 == iFixVal)
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_MANUAL,SPI_COM_WAIT); 
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_MANUAL_ON, SPI_COM_WAIT);                
                 ARM2FPGA_WRITE2(0x59,0x20,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5a,0x20,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5b,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5c,0x80,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5d,0x0,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5e,0xc0,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5f,0x1,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x60,0x80,SPI_COM_WAIT); 

                 ARM2FPGA_WRITE2(0xcf,0x06,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0xd0,0xc0,SPI_COM_WAIT); 

                 ICX274CTL_INFO("清晰度 中,iFixVal:%d.\n",iFixVal);
            }
            else if( 6 == iFixVal )
            {
                ARM2FPGA_WRITE2(SHARPEN_MODE_ADDR,SHARPEN_MODE_MANUAL,SPI_COM_WAIT); 
                ARM2FPGA_WRITE2(SHARPEN_DEGREE_ADDR,SHARPEN_DEGREE_MANUAL_ON, SPI_COM_WAIT);                
                 ARM2FPGA_WRITE2(0x59,0x80,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5a,0x8,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5b,0xc,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5c,0x60,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5d,0x0,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5e,0x80,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x5f,0x1,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0x60,0x40,SPI_COM_WAIT); 

                 ARM2FPGA_WRITE2(0xcf,0x0a,SPI_COM_WAIT); 
                 ARM2FPGA_WRITE2(0xd0,0x80,SPI_COM_WAIT); 

                 ICX274CTL_INFO("清晰度 低,iFixVal:%d.\n",iFixVal);
            }
        }
        break;
        
        case CAMERA_DENOISE:
        {
            /* 2d降噪 */
            if(0 == iFixVal )
            {
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_MODE_OFF, SPI_COM_WAIT);

                ICX274CTL_INFO("2D降噪 关,iFixVal:%d.\n",iFixVal);
            }
            else if( 1 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_DEGREE_HIGH, SPI_COM_WAIT);
                ICX274CTL_INFO("2D降噪 自动强,iFixVal:%d.\n",iFixVal);
            }
            else if( 2 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_DEGREE_MID, SPI_COM_WAIT);
                ICX274CTL_INFO("2D降噪 自动中,iFixVal:%d.\n",iFixVal);
            }
            else if( 3 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_DEGREE_LOW, SPI_COM_WAIT);
                ICX274CTL_INFO("2D降噪 自动弱,iFixVal:%d.\n",iFixVal);
            }
            else if( 4 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_MODE_ON, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x51, 0xff, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x52, 0x04, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x53, 0xff, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x54, 0x04, SPI_COM_WAIT);
                ICX274CTL_INFO("2D降噪 手动强,iFixVal:%d.\n",iFixVal);
            }
            else if( 5 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_MODE_ON, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x51, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x52, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x53, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x54, 0x10, SPI_COM_WAIT);
                ICX274CTL_INFO("2D降噪 手动中,iFixVal:%d.\n",iFixVal);
            }
            else if( 6 == iFixVal )
            {        
                ARM2FPGA_WRITE2(DN_2D_MODE_ADDR,DN_2D_MODE_ON, SPI_COM_WAIT);

                ARM2FPGA_WRITE2(0x51, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x52, 0x40, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x53, 0x10, SPI_COM_WAIT);
                ARM2FPGA_WRITE2(0x54, 0x40, SPI_COM_WAIT);
                ICX274CTL_INFO("2D降噪 手动弱,iFixVal:%d.\n",iFixVal);
            }            
        }
        break;

        default:
            break;
    }
    return 0;
}
#endif

#endif


