#ifndef __REPORT_REC_ERROR_H_
#define __REPORT_REC_ERROR_H_

#ifdef __cplusplus
extern "C" 
{
#endif
	typedef enum tagRecError
	{
		eWriteFailed =0,
		eNoSpace,
		eNewAsfFailed,
	}ERECERROR;

	int ReportRecError(ERECERROR error);
	int ReportPicSaveError(ERECERROR error);

#ifdef __cplusplus
};
#endif

#endif
