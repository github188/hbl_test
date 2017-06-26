#ifndef __NTP_CONF_H
#define  __NTP_CONF_H

typedef struct
{
    bool            bEnable;                /* 是否使能NTP      */
    unsigned int    uiNtpServerIp;         /* NTP 服务器 IP    */
    unsigned short  usNtpPort;              /* NTP 服务端口     */
    unsigned short  usLocalPort;
    unsigned int    uiInterval;             /* NTP 对时周期     */
    unsigned int    uiTimeLag;              /* NTP 对时误差     */ 
    bool            bForced;                /*强制对时*/
}STR_NTP_CLIENT;

#define     DBG_ERR    1
#define		DBG_INFO   2
#define 	DBG_WARN   3
#define 	DBG_ALL    4



#endif
