/***********************************************************************************/
//                 file : hbl_print.h
//        creation date : 14-06-2017 15:32
//           created by : Hubert Li
//        last modified : 2017年06月14日 15:32:02
//                                                                               
//            copyright : 2015-2019 by Hubert Li
//                             Strictly Confidential
//                             All rights reserved.
//                       No part of this hardware description, either
//                       material or conceptual may be copied or distributed,
//                       transmitted, transcribed, stored in a retrieval system
//                       or translated into any human or computer language in
//                       any form by any means, electronic, mechanical, manual
//                       or otherwise, or disclosed to third parties without
//                       the express written permission of Hubert Li
//                                                                               
/***********************************************************************************/

#ifndef _HBL_PRINT_H
#define _HBL_PRINT_H
#include <stdio.h>
#include <time.h>

//#define HBL_FUN

#define HBL_PRINT_DEBUG_SWITH  0Xffff 
#define HBL_PRINT_PRINT_SWITH  0Xffff

#define HBL_PRINTF  0X0001
#define HBL_SPRINTF 0X0002

#define HBL_DEBUG 1
#define HBL_PRINT 0

#define HBL_FUN_DEBUG_PRINT(format,...)\
do {\
    if (HBL_PRINTF&HBL_PRINT_DEBUG_SWITH) {\
      time_t timenow;\
      time(&timenow);\
      printf("%s     : ",ctime(&timenow));\
      printf("FILE:"__FILE__", LINE:%d,  "format,__LINE__,##__VA_ARGS__);\
    }\
}while(0)


#define HBL_FUN_PRINT(format,...)\
do {\
    if (HBL_PRINTF&HBL_PRINT_PRINT_SWITH) {\
      time_t timenow;\
      time(&timenow);\
      printf("%s     : ",ctime(&timenow));\
      printf("FILE:"__FILE__", LINE:%d,  "format,__LINE__,##__VA_ARGS__);\
    }\
}while(0)


#ifdef HBL_FUN

int hbl_print(int debug,int type,FILE *fp,char *format,...); 

#else

#define hbl_print(debug,type,fp,format,...)\
do {\
  if (debug == HBL_DEBUG) {\
    switch(type) {\
      case (HBL_PRINTF): {\
        HBL_FUN_DEBUG_PRINT(format,##__VA_ARGS__);\
        break; }\
      default:\
        break;\
    }\
  } else {\
    switch(type) {\
      case (HBL_PRINTF): {\
        HBL_FUN_PRINT(format,##__VA_ARGS__);\
        break; }\
      default:\
        break;\
    }\
  }\
}while(0)


#define debug(char *format,...) hbl_print(HBL_DEBUG,HBL_PRINTF,NULL,format,##__VA_ARGS__)
#define print(char *format,...) hbl_print(HBL_PRINT,HBL_PRINTF,NULL,format,##__VA_ARGS__)

#endif

#endif
