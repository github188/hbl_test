
#ifndef   __MD5__
#define   __MD5__

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************
��������:           MD5Gen
��������:           ����MD5 ֵ
���ñ���������:  
�����ʵı�:        ��  
���޸ĵı�:        ��   
����:                 input ��������
                        inlen �������ݳ���
���:                 output ������ 
����:                 ��
����˵��:           ��
***************************************************************************/
void  MD5Gen (unsigned char *output, unsigned char *input, unsigned int inlen);
void MD5Stream(unsigned char *output, FILE *stream);
#ifdef __cplusplus
}
#endif

#endif 

