#ifndef __NTP_CONF_H
#define  __NTP_CONF_H

typedef struct
{
    bool            bEnable;                /* �Ƿ�ʹ��NTP      */
    unsigned int    uiNtpServerIp;         /* NTP ������ IP    */
    unsigned short  usNtpPort;              /* NTP ����˿�     */
    unsigned short  usLocalPort;
    unsigned int    uiInterval;             /* NTP ��ʱ����     */
    unsigned int    uiTimeLag;              /* NTP ��ʱ���     */ 
    bool            bForced;                /*ǿ�ƶ�ʱ*/
}STR_NTP_CLIENT;

#define     DBG_ERR    1
#define		DBG_INFO   2
#define 	DBG_WARN   3
#define 	DBG_ALL    4



#endif
