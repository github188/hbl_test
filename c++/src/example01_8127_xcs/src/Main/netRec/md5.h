
#ifndef   __MD5__
#define   __MD5__

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
/**************************************************************************
函数名称:           MD5Gen
函数描述:           计算MD5 值
调用本函数函数:  
被访问的表:        无  
被修改的表:        无   
输入:                 input 输入数据
                        inlen 输入数据长度
输出:                 output 输出结果 
返回:                 无
其它说明:           无
***************************************************************************/
void  MD5Gen (unsigned char *output, unsigned char *input, unsigned int inlen);
void MD5Stream(unsigned char *output, FILE *stream);
#ifdef __cplusplus
}
#endif

#endif 

