

#ifndef _RTC_UTIL_H__
#define _RTC_UTIL_H__

#ifdef __cplusplus
extern "C" {
#endif

int SetRtc(unsigned int utc);
int GetRtc(unsigned int *utc);

int SetRtcTime(int yy,int mm,int dd,int hh,int mn,int ss);
int GetRtcTime(int *yy,int *mm,int *dd,int *hh,int *mn,int *ss);

#ifdef __cplusplus
}
#endif

#endif

