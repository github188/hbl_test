/***********************************************************************************/
//                 file : hbl_file.h
//        creation date : 20-10-2017 09:48
//           created by : Hubert Li
//        last modified : 2017年10月20日  9:48:19
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

#ifndef _HBL_FILE_H
#define _HBL_FILE_H

#define FILE_MAX_NUM 128

struct file_struct {
  int valid;
  FILE *fp;
  int write_enable;
  int read_enable;
}

#endif
#endif
