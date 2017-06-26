/***********************************************************************************/
//                 file : hbl_print.c
//        creation date : 14-06-2017 15:16
//           created by : Hubert Li
//        last modified : Wed 14 Jun 2017 06:34:25 PM CST
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
#include <stdio.h>
#include <stdarg.h>
#include "../include/hbl_print.h"


int hbl_print(int debug,int type,FILE *fp,char *format,...) {
  
  va_list param;
  va_start(param,format);

