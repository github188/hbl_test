#include "NtpClient.h"
#include "Ntp.h"

CNtp::CNtp()
{
    
}

CNtp::CNtp(MSG_Q_ID  msgId)
{
    m_cNtpHandle = new CNtpClient(msgId);
}

CNtp::~CNtp()
{
    delete m_cNtpHandle;
}
bool CNtp::NtpInit(STR_NTP_CLIENT * pNtpClient)
{
    return m_cNtpHandle->NtpClientInit(pNtpClient);
}
bool CNtp::NtpSetServer(STR_NTP_CLIENT * pNtpClient)
{
    return m_cNtpHandle->NtpClientSetServer(pNtpClient);
}
bool CNtp::NtpSwitchDbg(int level)
{
    return m_cNtpHandle->NtpClientSwitchDbg(level);
}
