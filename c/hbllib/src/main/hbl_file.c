/***********************************************************************************/
//                 file : hbl_file.c
//        creation date : 20-10-2017 09:52
//           created by : Hubert Li
//        last modified : 2017年10月20日 10:53:09
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



#include <malloc.h>
#include <string.h>
#include "../include/hbl_print.h"
#include "../include/hbl_error.h"
#include "../include/hbl_file.h"

// init file group, often done at begin of the program
int file_group_init(struct file_struct *file_group,int max_num) { //-- {{{
  
  debug("here is in function: file_group_init\n");

  if (max_num > FILE_MAX_NUM) {
    print("input file max num is to big: %d\n",max_num);
    return ERROR_INT;
  } else {
    if ((file_group = (struct file_struct *)malloc(sizeof(struct file_struct)*max_num)) == NULL) {
      print("malloc error\n")
      return ERROR_INT;
    }
    
    memset(file_group,0,sizeof(struct file_struct)*max_num);
  }

  return SUCESS_INT;
}//--}}}


//
int file_group_destroy(struct file_struct *file_group,int max_num) { //-- {{{

  int i;
  debug("here is in function: file_group_destroy\n");
  
  for (i = 0;i < max_num; i++) {
    if ((file_group + i)->valid = 1) {
      fclose((file_group+i)->fp);
    }
  }

  free(file_group);
  return SUCESS_INT;
}//-- }}}


//
FILE file_open(struct file_struct *file_group,int max_num,char *dir,char *rw_mode) { //-- {{{
  
  int i;
  FILE *fp;

  for (i = 0; i < max_num; i++) {
    if ((file_group + i)->valid = 0) {
      if ((fp = fopen(dir,rw_mode)) == NULL) {
        print("fopen file %s error\n",dir);
        return ERROR_POINT;
      }
      
      (file_group + i)->valid = 1;
      (file_group + i)->fp = fp;

      if (strstr(rw_mode,"w") != NULL) {
        (file_group + i)->write_enable = 1;
      } else {
        (file_group + i)->write_enable = 0;
      }

      if (strstr(rw_mode,"r") != NULL) {
        (file_group + i)->read_enable = 1;
      } else {
        (file_group + i)->read_enable = 0;
      }

      return fp;
    }
  }

  return ERROR_POINT;
}//-- }}}
      
//
int file_close(struct file_struct *file_group,int max_num,FILE *fp) { //-- {{{
  
  int i;
  for (i = 0; i < max_num; i++) {
    if ((file_group + i)->valid = 1) {
      if ((file_group + i)->fp == fp) {
        fclose(fp);
        (file_group + i)->valid = 0;
        (file_group + i)->write_enable = 0;
        (file_group + i)->read_enable = 0;
        return SUCESS_INT;
      }
    }
  }

  return ERROR_INT;
}//-- }}}
      

  

