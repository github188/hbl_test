
#ifndef __REBOOT_H_
#define __REBOOT_H_

#ifdef __cplusplus
extern "C" {
#endif

int DelayReboot(int nSecond);
int monSockInit();
int monSockUninit();
int monSockSend(const char *command);

#ifdef __cplusplus
}
#endif

#endif

