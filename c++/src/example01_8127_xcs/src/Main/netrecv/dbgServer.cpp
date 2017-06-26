#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>	/*for socket(),bind(),close,etc*/
#include <sys/ioctl.h>
#include <sys/un.h>

#include "McLog.h"
#include "dsp.h"
#include "Record.h"
#include "SysConf.h"
#include "dbgServer.h"
#include "MmsSend.h"

#include "dsp_service.h"
#include "SysHandle.h"
#include "dspParamManage.h"

#if defined(CYC200XZD)||defined(CYC500XZD)
#include "zebraCtl.h"
#include "ccdCtl.h"
#else
#include "icx274CCDCtl.h"
#endif

#include "ispV2Param.h"
#include "ispParam.h"
#include "protoXHandle.h"

//#include "dm6437_service.h"
//#include "dm648_service.h"

/* socket是否采用非阻塞模式 */
#define SOCKET_NOBLOCK_FLAG 0

#define SOCKET_MSG_HEAD_LEN                         (2)
#define GET_ENC_VER                              	0xF704

#define DEBUG_SWITCH					0xF710
#define MMS_DUMP						0xF711	
#define REC_DUMP						0xF713
#define DIR_DUMP						0xF714
#define PLAN_DUMP						0xF715
#define SET_PASSPORT_SNAP               0xF740 // 卡口抓拍
#define SET_EPOLICE_SNAP  				0xF741 // 电子警察抓拍

#define QUERY_SNAP_INFO					0xF758 // 查询图片抓拍信息

#define DIRECT_READ_REG                 0xF780 // 直接读寄存器
#define DIRECT_WRITE_REG                0xF781 // 直接写寄存器

#define FRAME_RATE_GET                  0xF782 // 获取帧率
#define FRAME_RATE_SET                  0xF783 // 设置帧率

#define HUE_GET                         0xF784 // 获取色调
#define HUE_SET                         0xF785 // 设置色调

#define CS_RADAR_SET                    0xF790 //向川速雷达发送数据

#define JAI_CAM_CMD_SET                 0xF792 //向JAI摄像机发送配置参数命令

#define ISP_I2C_CMD_SET                 0xF793 //向新的isp FPGA发送配置参数设置命令

#define ISP_I2C_CMD_GET                 0xF794 //向新的isp FPGA发送配置参数读取命令

#define DSP_TEST_CMD_SET                0xF795 //向Dsp发送测试命令，全部为字符串

#define SHELL_SAVE_FIEL_SET             0xF800 //设置debugshell是否存储到文件
#define DBG_SERVER_LISTEN               0xF801 //守护进程监控xcs是否活着

#define KODAR_GAMMA_SET                 0xF810 // 设置gamma值
#define KODAR_CONTRAST_SET              0xF811 // 设置对比度值

#define KODAR_ADD_DYNC_FLASH_LINES      0xF812 // 增加动态快门生效行数
#define KODAR_REDUCE_DYNC_FLASH_LINES   0xF813 // 减少动态快门生效行数

#define KODAR_RBG_RED_REPARER_SET       0xF814 // 新的红灯修正算法参数配置

#define KODAR_FPGA_NEW_PARAM_SET        0xF815 // FPGA新参数饱和度、g分量阀值配置

#define KODAR_DUBALF_MODE_SET           0xF816  //双快门模式参数配置

#define KODAR_GAMMA_GET                 0xF820 // 获取gamma值
#define KODAR_CONTRAST_GET              0xF821 // 获取对比度值
#define KODAR_DENOISE_GET               0xF822 // 获取降噪级别

#define RED_DETECT_THRESHOLD            0xF900 // 设置红灯检测阀值

#define ONVIF_START_ENC                 0xfa00 //onvif使能码流编码
#define ONVIF_STOP_ENC                  0xfa01 //onvif关闭码流编码

#define WEB_MESSAGE_DVS_PROTOCOL        (0x5A5B)
#define WEB_MESSAGE_HEADLENGTH            4

#define WEB_MESSAGE_V2_PROTOCOL         (0x5A5C)

#define DEFAULT_ERROR_CODE				0xffff

static XCS_CLIENT s_v3Client;
static XCS_CLIENT s_v2Client;

extern char version_string[];

extern char g_cOnvifVideoSta;

CDbgServer::CDbgServer()
{
	m_sockFd = -1;
	memset(m_szSockName,0,sizeof(m_szSockName));
	m_recvBuf = (char *)(new int[MAX_DBG_BUF_LEN/4]);
	if(NULL == m_recvBuf)
	{
	    shell_print("malloc m_recvBuf failed\n");
	}
	m_sendBuf = (char *) (new int[MAX_DBG_BUF_LEN/4]);
	if(NULL == m_sendBuf)
	{
	    shell_print("malloc m_sendBuf failed\n");
	}

	/* v3平台接口 */
	memset(&s_v3Client, 0, sizeof(s_v3Client));
	
	s_v3Client.sockClient = -1;//INVALID_SOCKET;
    s_v3Client.sin_addr = s_v3Client.sin_port = 0;
            
    s_v3Client.pRecvBuf = (char *)m_recvBuf;
    s_v3Client.pSendBuf = (char *)m_sendBuf;

	s_v3Client.nRecvBufLen = MAX_DBG_BUF_LEN;
	s_v3Client.nSendBufLen = MAX_DBG_BUF_LEN;
	
    // FIXME: user id may be wrong in the future
    s_v3Client.bUserValid = 1;

    /* v2平台接口 */
    memset(&s_v2Client, 0, sizeof(s_v2Client));

	s_v2Client.sockClient = -1;//INVALID_SOCKET;
    s_v2Client.sin_addr = s_v2Client.sin_port = 0;
            
    s_v2Client.pRecvBuf = (char *)m_recvBuf;
    s_v2Client.pSendBuf = (char *)m_sendBuf;

	s_v2Client.nRecvBufLen = MAX_DBG_BUF_LEN;
	s_v2Client.nSendBufLen = MAX_DBG_BUF_LEN;
	
    // FIXME: user id may be wrong in the future
    s_v2Client.bUserValid = 1;
	
}

CDbgServer::~CDbgServer()
{

	if ( m_sockFd != -1 )
	{
		DbgServerClose();
	}
	memset(m_szSockName,0,128);
	if (m_recvBuf != NULL)
	{
		free(m_recvBuf);
		m_recvBuf = NULL;
	}

	if (m_sendBuf != NULL)
	{
		free(m_sendBuf);
		m_sendBuf = NULL;
	}
	
	
}

int CDbgServer::DbgServerOpen(char * szListenName)
{
	if ((strlen(m_szSockName) == 0) && (strlen(szListenName) > 0))
	{
	    m_sockFd = socket(AF_UNIX, SOCK_DGRAM, 0); 
	    if (m_sockFd < 0)
	    {
	        return -1;    
	    }

	    unlink(szListenName);

        strcpy(m_szSockName,szListenName);
		bzero(&m_dbgAddr, sizeof(m_dbgAddr));
		
		m_dbgAddr.sun_family      = AF_UNIX;
		
	    snprintf(m_dbgAddr.sun_path, sizeof(m_dbgAddr.sun_path), szListenName);
	    bind(m_sockFd, (struct sockaddr *)&m_dbgAddr, sizeof(m_dbgAddr));

#if SOCKET_NOBLOCK_FLAG
		//set socket to no-blocking mode
		bFlag = fcntl(m_sockFd, F_GETFL, 0);
		ret = fcntl(m_sockFd, F_SETFL, O_NONBLOCK|bFlag);
#endif

		m_bThreadExit = false;		
	    if (0 != pthread_create(&m_threadId, NULL, CDbgServer::DbgThread, (void*)this))
	    {
			goto error_ret;
	    }
	    shell_print("creat dbg socket success\n");
		return 0;
		
	}
	else 
	{
		//only one socket can be created
		shell_print("creat dbg socket failed\n");
		return -1;
	}
error_ret:
	close(m_sockFd);
	m_sockFd = -1;
    return -1;
	
}

int CDbgServer::DbgServerClose()
{
    m_bThreadExit = true;
    pthread_join(m_sockFd, 0);
    
	close(m_sockFd);
	m_sockFd = -1;
	return 0;
}

void CDbgServer::v3RespSend(void)
{
    int ret = -1 ;
    if (s_v3Client.nSendLen > 0)
    {
    	ret = sendto(m_sockFd, 
                     (char *)m_sendBuf,  
                     s_v3Client.nSendLen, 0, 
                     (struct sockaddr *)&m_fromAddr,
                     sizeof(struct sockaddr_un));
        //shell_print("sendto usock %d bytes\n", ret);
    	            
    	if (ret != s_v3Client.nSendLen)
    	{
    		shell_print("send v3 res message failed ret:%d,errno:%d\n", ret, errno) ;
    	}
    	s_v3Client.nSendLen = 0;
	}
}

void CDbgServer::v2RespSend(void)
{
    int ret = -1 ;
    if (s_v2Client.nSendLen > 0)
    {
    	ret = sendto(m_sockFd, 
                     (char *)m_sendBuf,  
                     s_v2Client.nSendLen, 0, 
                     (struct sockaddr *)&m_fromAddr,
                     sizeof(struct sockaddr_un));
        //shell_print("sendto usock %d bytes\n", ret);
    	            
    	if (ret != s_v2Client.nSendLen)
    	{
    		shell_print("send v2 res message failed ret:%d,errno:%d\n", ret, errno) ;
    	}
    	s_v2Client.nSendLen = 0;
	}
}



void *CDbgServer::DbgThread(void *ptr)
{
    CDbgServer *pThis = (CDbgServer *)ptr;
    int readlen;
    int fromlen;
    int ret;
 	unsigned short message_type = 0;
	unsigned short message_len = 0;
    writeThreadInfo("dbgServer");
    CHandleXcsPtl cKJXcsPtl;
    cKJXcsPtl.m_uiThreadFlag = DVS_PROTO_TYPE;
    fromlen = sizeof(pThis->m_fromAddr);
    while (!pThis->m_bThreadExit)
    {    
    	// socket is blocking
    	readlen = recvfrom(pThis->m_sockFd,pThis->m_recvBuf,MAX_DBG_BUF_LEN, 0,
    	                (struct sockaddr *)&(pThis->m_fromAddr), (socklen_t *)&fromlen);
#if SOCKET_NOBLOCK_FLAG
		if(readlen == 0)
		{
			usleep(10000);
			continue;
		}
		else if(readlen < 0)
		{
			usleep(10000);
			if(errno != 11)
			{
				syslog(LOG_ERR|LOG_USER,"error:dbgServer recvfrom, ret:%d, error:%d strerror:%s\n",\
	            readlen, errno, strerror(errno));
			}
			continue;
		}
#endif
        //shell_print("readlen = %d .\n",readlen);
    	memcpy(&message_type , pThis->m_recvBuf , sizeof(unsigned short)) ;
    	memcpy(&message_len , pThis->m_recvBuf + sizeof(unsigned short) , sizeof(unsigned short)) ;
    
    	switch (message_type)
        {
            case DBG_SERVER_LISTEN:
                *(int *)(pThis->m_recvBuf) = 0x11223344;
                ret = sendto(pThis->m_sockFd, pThis->m_recvBuf, 4, 0,\
                    (struct sockaddr *)&(pThis->m_fromAddr), sizeof(struct sockaddr_un));
                if(ret != 4)
                {
                    shell_print("dbg send failed,ret:%d!\n", ret);
                }
                else
                {
                    //shell_print("dbg send ok,ret:%d!", ret);
                    //shell_print("dbg server test nfs!\n");
                }
                break;
           	case WEB_MESSAGE_DVS_PROTOCOL:
                {
                    s_v3Client.nRecvLen = readlen;
                    ret = cKJXcsPtl.DataPump(&s_v3Client);
                    //shell_print("dvs process ret %d\n", ret);
                    pThis->v3RespSend();
                }
        		break;
            case WEB_MESSAGE_V2_PROTOCOL:
                {
                    *(unsigned short *)(pThis->m_recvBuf) = 0x5A5B;
                    s_v2Client.nRecvLen = readlen;
                    ret = cKJXcsPtl.DataPump(&s_v2Client);
                    //shell_print("dvs process ret %d\n", ret);
                    pThis->v2RespSend();
                }
        		break;
    	    case DEBUG_SWITCH:
    		{
    			char szModuleName[32] = {0};
    			int nLevel = 0;

    			strcpy(szModuleName, (pThis->m_recvBuf + 4));
    			memcpy(&nLevel,pThis->m_recvBuf + 4+strlen(szModuleName)+1,sizeof(int));

    			DebugSwitch(szModuleName,nLevel);
             }
    		break;
            case SET_PASSPORT_SNAP:
            {
    			shell_print("Set pass port Snap .\n");
                STR_SYS_APP_MSG strTmpSysMsg;
                memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
                strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
                strTmpSysMsg.uiMsgType = MSG_BAYONET_MANUAL_SNAP;
                PSTR_BAYONET_HAND_SNAP pstrHandSnapInfo = (PSTR_BAYONET_HAND_SNAP)(strTmpSysMsg.aucData);
                pstrHandSnapInfo->uiCarRoadId = 0;
                pstrHandSnapInfo->uiCarSpeed = 200;
                ret = sendMsgToMainApp(&strTmpSysMsg);
                if(-1 == ret)
                {
                    shell_print("send bayonet hand snap failed.\n");
                }
                else
                {
                    shell_print("send bayonet hand snap msg ok.\n");
                }
            }
            break;
            case CS_RADAR_SET:
            {
    			int iLen = 0;
                int i =0;
                int iDex =0;
                char cCmd[32] ={0};
            	char cParam[32] = {0};
            	char cData[32] ={0};
            	int nTmp = 0;
            	char cFirstFlag =0;
            	
    			strcpy(cParam, (pThis->m_recvBuf + 4));
                iLen = strlen(cParam);
                shell_print("cs cmd is %s", cParam);
                for(nTmp =0; nTmp < iLen; nTmp++)
                {
                    if(    ((0x30 <= cParam[nTmp]) && ( 0x39 >= cParam[nTmp]) )\
                        ||  ( (0x41 <= cParam[nTmp]) && ( 0x5a >= cParam[nTmp]) )\
                        || ( (0x61 <= cParam[nTmp]) && ( 0x7a >= cParam[nTmp]) ) )
                    {
                        if( cFirstFlag == 0 )
                        {
                            cFirstFlag =1;
                            cCmd[iDex] = nTmp;
                            iDex++;
                        }
                    }
                    else
                    {
                        cFirstFlag =0;
                    }
                }
                
                for(i =0; i< iDex; i++)
                {
                    cData[i] = strtol(cParam + cCmd[i], NULL, 16);
                    shell_print("cmd len:%d data is 0x%x!\n", iDex, cData[i]);
                }
                
                cData[iDex] = '\0';

                dCtlModuleCapWirte(DEV_TYPE_CS, cData, iDex);
            }
            break;
            case JAI_CAM_CMD_SET:
            {
                int iLen =0;
            	char cParam[32] = {0};
            	char szCmdEnd[32]="\r\n";

            	memset(cParam, 0, sizeof(cParam));
    			strcpy(cParam, (pThis->m_recvBuf + 4));
    			strcat(cParam, szCmdEnd);
                iLen = strlen(cParam);
                shell_print("len:%d,jai cmd is %s", iLen,cParam);
                //dsp_service_set_transparent_param(cParam);
            }
            break;
            case DSP_TEST_CMD_SET:
            {
                int iLen =0;
            	char cParam[32] = {0};

            	memset(cParam, 0, sizeof(cParam));
    			strcpy(cParam, (pThis->m_recvBuf + 4));
                iLen = strlen(cParam);
                shell_print("len:%d,dst test cmd is %s", iLen,cParam);
                //dsp_service_test_cmd(cParam);
            }
            break;
            
#if defined(CYC500XKW4C)||defined(CYC500XAW)
            case ISP_I2C_CMD_SET:
            {
                int iLen =0;
            	char cParam[32] = {0};

            	memset(cParam, 0, sizeof(cParam));
    			strcpy(cParam, (pThis->m_recvBuf + 4));
                iLen = strlen(cParam);
                shell_print("set len:%d,isp i2c cmd is %s", iLen,cParam);
                ispI2cCmdSet(cParam);
            }
            break;
            
            case ISP_I2C_CMD_GET:
            {
                int iLen =0;
            	char cParam[32] = {0};

            	memset(cParam, 0, sizeof(cParam));
    			strcpy(cParam, (pThis->m_recvBuf + 4));
                iLen = strlen(cParam);
                shell_print("get len:%d,isp i2c cmd is %s", iLen,cParam);
                ispI2cCmdGet(cParam);
            }
            break;
#endif            

#if defined(CYC500XZD)||defined(CYC200XZD)  
            case DIRECT_READ_REG:
            {
                unsigned int reg_addr;
                int ret;
                unsigned int value;
       			memcpy(&reg_addr, (pThis->m_recvBuf + 4), sizeof(int));
       			ret = cameraDirectReadReg(reg_addr,&value);
       			shell_print("get reg :addr 0x%x value 0x%x ret = %d .\n",reg_addr,value,ret);
            }
            break;
            case DIRECT_WRITE_REG:
            {
                unsigned int reg_addr;
                int ret;
                unsigned int value;
       			memcpy(&reg_addr, (pThis->m_recvBuf + 4), sizeof(int));
       			memcpy(&value, (pThis->m_recvBuf + 8), sizeof(int));   			
       			ret = cameraDirectWriteReg(reg_addr,value);
       			shell_print("set reg :addr 0x%x value 0x%x ret = %d .\n",reg_addr,value,ret);
       			            
            }
            break;

            case FRAME_RATE_SET:
            {
                //int ret;
                float fvalue;
       			memcpy(&fvalue, (pThis->m_recvBuf + 4), sizeof(int));
       			ret = dc1394_frameRate_set(fvalue);
       			shell_print("set Rrame Rate %f .\n", fvalue);
            }
            break;
            
            case FRAME_RATE_GET:
            {
                //int ret;
                float value;
       			ret = dc1394_frameRate_get(&value);
       			shell_print("get frame rate %f.\n", value);
            }
            break;
#endif            
            case HUE_SET:
            {
                //int ret;
                float fvalue;
       			memcpy(&fvalue, (pThis->m_recvBuf + 4), sizeof(int));
       			//ret = dc1394_Hue_set(fvalue);
       			shell_print("set white balance %f .\n", fvalue);
            }
            break;
            case HUE_GET:
            {
                //int ret;
                float value;
       			//ret = dc1394_Hue_get(&value);
       			shell_print("get white balance %f.\n", value);
            }
            break;
        	case SET_EPOLICE_SNAP:
    		{
    			int chanid =0;
    			int vehicleId =0;

    			shell_print("Set Electronic Police Snap .\n");
    			memcpy(&chanid, (pThis->m_recvBuf +4), sizeof(int));
    			memcpy(&vehicleId, (pThis->m_recvBuf + 8), sizeof(int));
    			shell_print("chanid = %d vehicleid = %d .\n", chanid, vehicleId);

                STR_SYS_APP_MSG strTmpSysMsg;
                memset(&strTmpSysMsg, 0, sizeof(strTmpSysMsg));
                strTmpSysMsg.uiModuleId = SYS_MSG_PROTOCOL_MODULE_ID;
                strTmpSysMsg.uiMsgType = MSG_ELECT_POLICE_SIMU_RUN_SNAP;
                strTmpSysMsg.ucChannelId = chanid;
                strTmpSysMsg.ucCarRoadId = vehicleId;
                ret = sendMsgToMainApp(&strTmpSysMsg);
                if(-1 == ret)
                {
                    shell_print("send elect police force red light failed.\n");
                }
                else
                {
                    shell_print("send elect police force red light msg ok.\n");
                }
    		}
    		break;
            case QUERY_SNAP_INFO:
            {
    			mainJpegSnapWayInfo();
            }
            break;
            case GET_ENC_VER:
            {

       			int nDspID;
     	   		memcpy(&nDspID, (pThis->m_recvBuf + 4), sizeof(int));

        		shell_print("\n%s\n", version_string);
    			dspPrintVer();
    			//dsp_service_get_version();
    		#if defined(CYC500XKW4C)||defined(CYC500XAW)
    			//shell_print("libdm648.so %s\n", DM648GetVersion());
    		#elif defined(CYC500XZD)||defined(CYC200XZD)
    			dm368ProgramVerQuery();
    		#else
    			#if defined(CYC200KT)||defined(CYC200MK)||defined(CYC200K2)
				#else
    			shell_print("libdm6437.so %s\n", DM6437GetVersion());    		    
    			#endif
    		#endif
        	}
       		break;

      	    case MMS_DUMP:
    		{
    			MmsDump();
    		}
    		break;

   #if !defined(CYC500XZD)&&!defined(CYC200XZD)            
    		case KODAR_ADD_DYNC_FLASH_LINES:
    		{
    		    float fvalue;
       			memcpy(&fvalue, (pThis->m_recvBuf + 4), sizeof(int));
       			shell_print("add dync flash lines val: %f .\n", fvalue);
                kodarJudgeFlashLinesSave(0, (char)fvalue);
                kodarJudgeFlashLinesConf();
    		}
    		break;
    		
    		case KODAR_REDUCE_DYNC_FLASH_LINES:
    		{
    		    float fvalue;
       			memcpy(&fvalue, (pThis->m_recvBuf + 4), sizeof(int));
       			shell_print("cut dync flash lines val: %f .\n", fvalue);
                kodarJudgeFlashLinesSave(1, (char)fvalue);
                kodarJudgeFlashLinesConf();
    		}
    		break;

            case KODAR_FPGA_NEW_PARAM_SET:
            {
                unsigned int uiSatuVal;
                unsigned int uiGPixVal;
       			memcpy(&uiSatuVal, (pThis->m_recvBuf + 4), sizeof(int));
       			memcpy(&uiGPixVal, (pThis->m_recvBuf + 8), sizeof(int));
       			shell_print("pfga new param,satu:%d;gPixThresh:%d.\n", uiSatuVal, uiGPixVal);
       			
                kodarFpgaNewParamSave((unsigned short)uiSatuVal, (unsigned short)uiGPixVal);
                kodarFpgaNewParamSet();
            }
            break;

            case KODAR_DUBALF_MODE_SET:
            {
                char cDubalFEn =0;
                float   fShutVal=0;
                float   fGainVal=0;
                memcpy(&cDubalFEn, (pThis->m_recvBuf + 4), sizeof(char));
                memcpy(&fShutVal, (pThis->m_recvBuf + 5), sizeof(int));
                memcpy(&fGainVal, (pThis->m_recvBuf + 9), sizeof(int));

                shell_print("dubal shut enable:%d,shutVal:%f,gainVal:%f.\n", cDubalFEn, fShutVal, fGainVal);
                kodarDubalShutParamSave(cDubalFEn,fShutVal,fGainVal);
                kodarDubalShutParamSet(DUBAL_FLASH_FORCE_SWITCH);
            }
            break;
	#endif
            case RED_DETECT_THRESHOLD:
            {
                char cStaturation =0;
                char cBrightness =0;
                memcpy(&cStaturation, (pThis->m_recvBuf + 4), sizeof(char));
                memcpy(&cBrightness, (pThis->m_recvBuf + 5), sizeof(char));

                shell_print("red dect threshold satu:%d,bright:%d.\n", cStaturation, cBrightness);
                redDectThresholdParamSave(cStaturation, cBrightness);
                dspEPRedLightAreaSet(RL_NORMAL_SET_MODE, 0);
            }
            break;
#if 0
            case ONVIF_START_ENC:
            {
                shell_print("dbg onvif start enc!\n");
                g_cOnvifVideoSta =1;
                dspEncStaCtl(0, 1);
            }
            break;

            case ONVIF_STOP_ENC:
            {
                shell_print("dbg onvif stop enc!\n");
                g_cOnvifVideoSta =0;
                dspEncStaCtl(0, 0);
            }
            break;
#endif

#if 0
        case KODAR_RBG_RED_REPARER_SET:
    		{
                REDLIGHT_REPAIR_PARAM strRedRepairParam;
       			memcpy(&strRedRepairParam, (pThis->m_recvBuf + 4), sizeof(strRedRepairParam));
       			shell_print("set new rgb red reparer:%f.%f.%d.%d.\n",\
       			strRedRepairParam.fRatio, strRedRepairParam.fRedness,\
       			strRedRepairParam.iFixWhite, strRedRepairParam.iDetectRed);
       			
                newRGBRedRepareParamSave(strRedRepairParam);
                newRGBRedRepareParamConf();
    		}
    		break;


    		case KODAR_GAMMA_GET:
    		{
    		    char cVal;
       			cVal = kodarGammaValGet();
       			shell_print("get kodar gamma val:%d.\n", cVal);
    		}
    		break;
    		case KODAR_CONTRAST_GET:
    		{
    		    char cVal;
       			cVal = kodarContrastValGet();
       			shell_print("get kodar contrast val:%d.\n", cVal);
    		}
    		break;
  		
    		case KODAR_DENOISE_GET:
    		{
    		    char cVal;
       			cVal = kodarDeNoiseValGet();
       			shell_print("get kodar deNoise val:%d.\n", cVal);
    		}
    		break;
    #endif  
    		
#ifdef MODULE_REC
            case REC_DUMP:
            {
                RecDump();
            }
            break;
            case DIR_DUMP:
            {
                int nChannel = 0;

                memcpy(&nChannel, (pThis->m_recvBuf + 4), sizeof(int));

                DirDump(nChannel);
            }
            break;
            case PLAN_DUMP:
            {
                DumpPlan();
            }
            break;
#endif
        	default :
        		shell_print("recv unknown message , message_type is %d\n" , message_type) ;
        		break ;
        }
    }	

    shell_print("argsSever exiting...\n");    

    return NULL;
}



