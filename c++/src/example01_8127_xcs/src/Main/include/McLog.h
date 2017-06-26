#ifndef _MC_LOG_H__
#define _MC_LOG_H__

//����log��󳤶�
#define MAX_LOG_LEN				(128)

//Logģ������
typedef enum tagLogMoudleType
{
    LMT_UNDEFINE = 0,
    LMT_LOG,
    LMT_SYSCONF,
    LMT_SHELL,
    LMT_DCTL,
    LMT_ALERT,
    LMT_RECV,
    LMT_SEND,
    LMT_DVSPH,
    LMT_GEPH,
    LMT_VRP,
    LMT_REC,
    LMT_DSPCOM,
    LMT_SYSCALL,
    LMT_DSPFW,
    LMT_ALARM,
    LMT_NULL
}ELogModuleType;

//Log����
typedef enum tagLogType
{
    LT_EXCEP	= 0x01,
    LT_ERROR	= 0x02,
    LT_WARNING  = 0x04,
    LT_INFO		= 0x08,
    LT_DEBUG	= 0x10,
    LT_ALL		= 0xff,
    LT_NONE		= 0x00
}ELogType;

#ifdef __cplusplus
extern "C" {
#endif

/***********************************************************
* ��������: mcLogInit
* ��������:
*			szCfgFileName:��־·���ļ���
* ����ֵ:  �����ɹ�����0 ������ʧ�ܷ���-1
* ��������:��ʼ��Logģ�飬��ϵͳ����ʱִ��һ��
***********************************************************/
int mcLogInit(const char * szCfgFileName);


/***********************************************************
* ��������: mcLogUnInit
* ��������:
* ����ֵ:  �����ɹ�����0 ������ʧ�ܷ���-1
* ��������:�˳�Logģ�飬��ϵͳ�˳�ʱִ��
***********************************************************/
int mcLogUnInit();


/***********************************************************
* ��������: mcLog
* ��������:
* 			lm��ģ�����ͣ�ȡֵΪELogModuleType
*			lt: Log���ͣ�ȡֵΪELogType
*			format,...:Log����
* ����ֵ:  �޷���
* ��������:д��־
***********************************************************/
void mcLog(ELogModuleType lm, ELogType lt, const char* format, ...);


/***********************************************************
* ��������: mcLogQuery
* ��������:
*			lt: Log���ͣ�ȡֵΪELogType
*			utcBeginTime:��ʼʱ��
*			utcEndTime:����ʱ��
*			pBuf����־��Ż���
*			nBufLen����־������󳤶�
* ����ֵ:  �����ɹ�����0 ������ʧ�ܷ���-1
* ��������:��־��ѯ
***********************************************************/
int mcLogQuery(ELogType lt,unsigned int utcBeginTime,unsigned int utcEndTime,char *pBuf,int nBufLen);



/***********************************************************
* ��������: mcLogShow
* ��������:	
*			bEnable:�Ƿ��ӡlog��1-����־��ӡ��0-�ر���־��ӡ��
* ����ֵ:  �����ɹ�����0 ������ʧ�ܷ���-1
* ��������:����־��ӡ���ܣ�������־ʱͬʱ��ӡ
***********************************************************/
int mcLogShow(int bEnable);



/***********************************************************
* ��������: mcDebugSwitch
* ��������:
*			nLevel:���Եȼ���0-�رգ�1-4�����Եȼ���
* ����ֵ:  �����ɹ�����0 ������ʧ�ܷ���-1
* ��������:��Log���Կ���
***********************************************************/
int mcDebugSwitch(int nLevel);



/***********************************************************
* ��������: shell_print
* ��������:
*			format,...:��ӡ����
* ����ֵ:  �����ɹ�����0 ������ʧ�ܷ���-1
* ��������:��ӡ������Ϣ
***********************************************************/
int shell_print(const char* format, ...);


int initShellDebug(int iSaveFile, int iCheckType);

int shell_file_print(const char* format, ...);

#ifdef __cplusplus
}
#endif

#endif



