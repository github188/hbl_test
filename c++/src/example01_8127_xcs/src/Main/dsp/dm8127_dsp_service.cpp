#include "dm8127_dsp_service.h"

#if 0

/* 返回最后操作的错误码。 */
int NET_DVE_GetLastError()
{
	return 0;
}

int NET_CYC_Init()
{
	return 0;
}

int NET_CYC_Start()
{
	return 0;
}

int NET_CYC_Stop()
{
	return 0;
}

int NET_CYC_Cleanup()
{
	return 0;
}

int NET_CYC_SetDEVConfig(int dwCommand, void* lpInBuffer, int dwInBufferSize)
{
	return 0;
}

int NET_CYC_SetJpegDataCallBack(fJpegDataCallBack cbJpegDataCallBack, unsigned int dwUser)
{
	return 0;
}

int NET_CYC_SetRealDataCallBack(int iChannel, fRealDataCallBack cbRealDataCallBack, int dwUser)
{
	return 0;
}

int NET_CYC_SetDevMessageCallBack(MSGCallBack fMessageCallBack, void* pUser)
{
	return 0;
}

int NET_DEV_ManualSnap(FORCE_SNAP* param, int  iSnapType)
{
	return 0;
}

int NET_DEV_Get_Ability(int dwAbilityType, char* PInBuf, int dwInLength, char* pOutBuf, int dwOutLength)
{
	return 0;
}

int NET_CYC_SetDEVConfig_Test(int dwCommand, void* lpInBuffer, int dwInBufferSize)
{
	return 0;
}
#else
int NET_CYC_SetDEVConfig_Test(int dwCommand, void* lpInBuffer, int dwInBufferSize)
{
	return 0;
}
#endif


