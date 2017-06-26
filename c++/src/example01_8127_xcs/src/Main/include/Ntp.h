#ifndef __NTP_H
#define __NTP_H

class CNtpClient;
class CNtp
{
 public:
        CNtp();
        CNtp(MSG_Q_ID msgId);
        ~CNtp();
    bool    NtpInit(STR_NTP_CLIENT *pNtpClient);
    bool    NtpSetServer(STR_NTP_CLIENT *pNtpClient);
    bool    NtpSwitchDbg(int level);
private:
    CNtpClient *m_cNtpHandle;
    
};
#endif
