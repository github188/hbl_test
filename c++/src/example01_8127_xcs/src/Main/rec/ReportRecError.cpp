#include <unistd.h>
#include "ReportRecError.h"
#include "McLog.h"
#include "Reboot.h"
#include "SysConf.h"
#include "nfsMmc.h"

extern int xcsSysReboot(void);

int ReportRecError(ERECERROR error)
{
	int ret =0;
	if (error == eWriteFailed)
	{
		if ( 0 == checkmount(e_nfs))
		{
			mcLog(LMT_REC,LT_ERROR,"nfs rec write failed  && reboot .\n");
		}
		//DelayReboot(10);
		mcLog(LMT_REC,LT_ERROR,"nfs write file faile! reboot xcs system.\n");
		xcsSysReboot();
	}
	else if(eNewAsfFailed == error)
	{
	    mcLog(LMT_REC,LT_ERROR,"nfs new libasf failed! reboot xcs system.\n");
		xcsSysReboot();
	}
	return ret;
}

int ReportPicSaveError(ERECERROR error)
{
	int ret =0;
	if (error == eWriteFailed)
	{
		if (0 == checkmount(e_udisk))
		{
			mcLog(LMT_REC,LT_ERROR,"udisk file write failed  && reboot .\n");
		}
		//DelayReboot(10);
		mcLog(LMT_REC,LT_ERROR,"pic server write file faile! reboot xcs system.\n");
		xcsSysReboot();
	}
	return ret;
}


