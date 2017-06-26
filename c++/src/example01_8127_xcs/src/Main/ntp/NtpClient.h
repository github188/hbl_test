#ifndef __NTPCLIENT_H
#define __NTPCLIENT_H
#include "msgQLib.h"
#include "NtpConf.h"

#ifdef __cplusplus
extern "C"

{
#endif

extern int shell_print(const char* format, ...);

class CNtpClient
{
public:
    CNtpClient(MSG_Q_ID MsgId);                             /*  初始化 NTP SERVER模块       */
    ~CNtpClient();                                          /*  反初始化 NTP SERVER模块     */
    bool    NtpClientInit(STR_NTP_CLIENT *pNtpClient);
    bool    NtpClientSetServer(STR_NTP_CLIENT *pNtpClient);   /*  设置 NTP SERVER             */
    bool    NtpClientSwitchDbg(int level);              /*  NTP 调试信息开关            */
    
private:
    STR_NTP_CLIENT  m_strNtpClient;
    MSG_Q_ID        m_strSendMsgId;
    bool            m_bQuit;
    int             m_siSockFd;
    int             m_siDbgLevel;
    pthread_t       m_strNtpPid;
    unsigned int    m_auiTimeOfSend[2];
    struct timeval m_strSetTv;
    static void     *pNtpThreadFunc(void *arg);
    bool            NtpSetupRecive(void);
    bool            NtpSetupTransmit(void);
    bool            DoSetServerIP(STR_NTP_CLIENT *pNtpClient, int iMode);
    void            PrimaryLoop(int mode );
    int              ParseData(unsigned int *data, struct ntptime *arrival, 
                                unsigned int *time_of_send);
};

#define DEFAULT_NTP_SERVERIP                (0xffffffff)              
#define DEFAULT_NTP_PORT                    (123)
#define DEFAULT_NTP_INTERVAL                (600)
#define DEFAULT_NTP_TIMELAG                 (5)
#define MIN_INTERVAL                         (15)  
#define NTP_DEBUG(level, format, args...) do{\
	if (1)\
	{\
		if (level <= m_siDbgLevel)\
		{\
			shell_print("ntp mode:%s(%d) : "format"\n",__FUNCTION__, __LINE__,  ##args);\
		}\
	}\
	else\
	{\
		if (0)\
		{\
			shell_print("ntp mode:%s(%d) : "format"\n",__FUNCTION__, __LINE__,  ##args);\
		}\
	}\
	}while(0)
	
//----------------


#ifdef __cplusplus
}
#endif



#endif
