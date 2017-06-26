
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
#include <time.h>
#include <unistd.h>

#include "rtc_util.h"
#include "McLog.h"

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
unsigned long
utcmktime (unsigned int year, unsigned int mon,
	unsigned int day, unsigned int hour,
	unsigned int min, unsigned int sec)
{
	if (0 >= (int) (mon -= 2)) {	/* 1..12 -> 11,12,1..10 */
		mon += 12;		/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return (((
		(unsigned long) (year/4 - year/100 + year/400 + 367*mon/12 + day) +
			year*365 - 719499
	    )*24 + hour /* now have hours */
	  )*60 + min /* now have minutes */
	)*60 + sec; /* finally seconds */
}

int SetRtc(unsigned int utc)
{
	struct tm	tmUTC;
	int yy,mm,dd,hh,mn,ss;

	gmtime_r((time_t*)&utc,&tmUTC);

	yy = tmUTC.tm_year+1900;
	mm = tmUTC.tm_mon+1;
	dd = tmUTC.tm_mday;
	hh = tmUTC.tm_hour;
	mn = tmUTC.tm_min;
	ss = tmUTC.tm_sec;

	return SetRtcTime(yy,mm,dd,hh,mn,ss);
}

int GetRtc(unsigned int *utc)
{
	int nRet = 0;
	//struct tm	tmUTC;
	int yy,mm,dd,hh,mn,ss;

	if(utc==NULL)
		return (-1);

	yy = mm = dd = hh = mn = ss =0;

	nRet = GetRtcTime(&yy,&mm,&dd,&hh,&mn,&ss);

	if(nRet ==0)
	{
		*utc = utcmktime(yy,mm,dd,hh,mn,ss);
		shell_print("###### get rtc time ok,sec:%d\n", *utc);
		return 0;
	}
	else
	{
		shell_print("###### get rtc time failed\n");
		return (-1);
	}
}

int SetRtcTime(int yy,int mm,int dd,int hh,int mn,int ss)
{
	int nRet;
	int fd_rtc= -1;
	struct rtc_time s_rtc;
	
	fd_rtc = open("/dev/misc/rtc",O_RDONLY);
	if( fd_rtc<0 )
		return -1;
		
	s_rtc.tm_year = yy;
	s_rtc.tm_mon = mm;
	s_rtc.tm_mday = dd;
	s_rtc.tm_hour = hh;
	s_rtc.tm_min = mn;
	s_rtc.tm_sec = ss;
	
	nRet = ioctl(fd_rtc, RTC_SET_TIME, &s_rtc);
	
	if(fd_rtc>0)
	{
		close(fd_rtc);
	}
	
	if(nRet<0)
	{
		return -1;
	}
	else
	{
	    shell_print("%s,%d.write rtc time:%4d-%2d-%2d,%2d:%2d:%2d.\n",__FUNCTION__, __LINE__,\
	    yy, mm, dd, hh, mn, ss);

	    shell_print("%s,%d.write rtc time:%4d-%2d-%2d,%2d:%2d:%2d.\n",__FUNCTION__, __LINE__,\
	    yy, mm, dd, hh, mn, ss);
		return 0;
	}
}

int GetRtcTime(int *yy,int *mm,int *dd,int *hh,int *mn,int *ss)
{
	int nRet;
	int fd_rtc= -1;
	struct rtc_time g_rtc;

	*yy = 0;
	*mm = 0;
	*dd = 0;
	*hh = 0;
	*mn = 0;
	*ss = 0;

	fd_rtc = open("/dev/misc/rtc",O_RDONLY);
	if( fd_rtc<0 )
		return -1;

	nRet = ioctl(fd_rtc, RTC_RD_TIME, &g_rtc);	
	if(nRet>=0)
	{
		*yy = g_rtc.tm_year;
		*mm = g_rtc.tm_mon;
		*dd = g_rtc.tm_mday;
		*hh = g_rtc.tm_hour;
		*mn = g_rtc.tm_min;
		*ss = g_rtc.tm_sec;
	}
	
	if(fd_rtc>0)
	{
		close(fd_rtc);
	}
		
	if(nRet<0)
	{
		return -1;
	}
	else
	{
	    printf("%s,%d.read rtc time:%4d-%2d-%2d,%2d:%2d:%2d.\n",__FUNCTION__, __LINE__,\
	    *yy, *mm, *dd, *hh, *mn, *ss);
	    
	    shell_print("%s,%d.read rtc time:%4d-%2d-%2d,%2d:%2d:%2d.\n",__FUNCTION__, __LINE__,\
	    *yy, *mm, *dd, *hh, *mn, *ss);
	    
		return 0;
	}
}

