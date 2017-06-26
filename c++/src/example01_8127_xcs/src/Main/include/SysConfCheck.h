/******************************************************************************
 * SysConfCheck.h:
 * 
 * Copyright 2009 - 2100 HANGZHOU PEGVISION Technology Co.,Ltd.
 * 
 * DESCRIPTION:  
 *     
 * modification history
 * --------------------
 * 01a, 2009.12.12, xcsworker Create
 * --------------------
 ******************************************************************************/
#ifndef _SYSCONF_CHECK_H__
#define _SYSCONF_CHECK_H__

#include "xcsCommDef.h"
#include "xcsGlobalDef.h"

#define IS_P_CHANNEL_VALID(x)				(((int)x>=0)&&((int)x<TOTAL_CHANNEL_NUM))		/*channel program-internal*/


#define IS_ENC_TYPE_VALID(x)				((x==VTYPE_MPEG4)||(x==VTYPE_H264))
#define IS_ENC_FORMAT_VALID(x)			((x==VFMT_CIF)||(x==VFMT_4CIF)||(x==VFMT_V2CIF)||(x==VFMT_720P))

#define IS_ENC_IFRAMEINTV_VALID(x)	((x>=25)&&(x<=100))
#define IS_ENC_QUALITY_VALID(x)			((x==0)||(x==1)||(x==2)||(x==3)||(x==4)||(x==5)||\
																			(x==256)||(x==257)||(x==258))
#define IS_AENC_TYPE_VALID(x)				((x==ATYPE_G711A))


#define IS_VALID_DATE(y,m,d) ((y>=1970&&y<=2070)&&(m>=1&&m<=12)&&(d>=1&&d<=31))
#define IS_VALID_TIME(h,m,s) ((h>=0&&h<24)&&(m>=0&&m<60)&&(s>=0&&s<60))

#endif

