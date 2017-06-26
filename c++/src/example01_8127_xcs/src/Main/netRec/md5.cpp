
#include <stdio.h>
#include <string.h>

#include "md5.h"

typedef unsigned long   UINT4;
typedef unsigned char  *POINTERCHAR;

/* MD5 context. */
typedef struct
{
    unsigned long   state[4];  /* state (ABCD) */
    unsigned long   count[2];  /* number of bits, modulo 2^64 (lsb first) */
    unsigned char   buffer[64]; /* input buffer */
} MD5_CTX;

static void MD5Init    (MD5_CTX *context);

static void MD5Update  (MD5_CTX *context, 
                            unsigned char *input, 
                            unsigned int inputlength);

static void MD5Final   (unsigned char* digest, MD5_CTX *context);

/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform (UINT4[4], unsigned char[64]);
static void Encode (unsigned char *, UINT4 *, unsigned int);
static void Decode (UINT4 *, unsigned char *, unsigned int);
static void MD5_memcpy (POINTERCHAR, POINTERCHAR, unsigned int);
static void MD5_memset (POINTERCHAR, int, unsigned int);

static unsigned char PADDING[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define K(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (unsigned long)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
 (a) += G ((b), (c), (d)) + (x) + (unsigned long)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
 (a) += H ((b), (c), (d)) + (x) + (unsigned long)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
 (a) += K ((b), (c), (d)) + (x) + (unsigned long)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }


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
void  MD5Gen (unsigned char *output, unsigned char *input, unsigned int inlen)
{
    MD5_CTX         context;

    MD5Init (&context);
    MD5Update (&context, input, inlen);
    MD5Final (output, &context);
}

void MD5Stream(unsigned char *output, FILE *stream)
{

#define BLOCKSIZE 4096    
    unsigned char buffer[BLOCKSIZE + 72];    
    
    size_t sum;
    MD5_CTX         context;

    MD5Init (&context);
    
    while (1)    
    {      
    /* We read the file in blocks of BLOCKSIZE bytes.  One call of the    
        computation function processes the whole buffer so that with the    
        next round of the loop another block can be read.  */      
        size_t n;      
        unsigned int    i;
    
        sum = 0;      

        /* Read block.  Take care for partial reads.  */      
        do   
        {     
            n = fread ((char *)buffer, 1, BLOCKSIZE - sum, stream);   
            sum += n; 
        }while (sum < BLOCKSIZE && n != 0);      

        if (n == 0 && ferror (stream))        
            return;    // error

        if (n == 0) break;
        
        /* Update number of bits */
        if ((context.count[0] += ((unsigned long) BLOCKSIZE << 3))
                    < ((unsigned long) BLOCKSIZE << 3))
            context.count[1]++;
        context.count[1] += ((unsigned long) BLOCKSIZE >> 29);

        
        /*
         * Transform as many times as possible.
         */
        for (i = 0; i + 63 < BLOCKSIZE; i += 64)
            MD5Transform (context.state, &buffer[i]);
    }        

    MD5Update (&context, buffer, sum);
    MD5Final (output, &context);

}

/**************************************************************************
函数名称:           MD5Init
函数描述:           MD5运算初始化
调用本函数函数: rc_md5_calc
被访问的表:        无  
被修改的表:        无   
输入:                 无
输出:                 context 
返回:                 无
其它说明:           无
***************************************************************************/
static void MD5Init (MD5_CTX *context)
{
    context->count[0] = context->count[1] = 0;

    /*
     * Load magic initialization constants.
     */
    context->state[0] = 0x67452301;
    context->state[1] = 0xefcdab89;
    context->state[2] = 0x98badcfe;
    context->state[3] = 0x10325476;
}

/**************************************************************************
函数名称:           MD5Update
函数描述:           MD5运算
调用本函数函数: rc_md5_calc
被访问的表:        无  
被修改的表:        无   
输入:                 input  输入数据
                        inputLen 输入数据长度
输出:                 context 计算后的数据
返回:                 无
其它说明:           无
***************************************************************************/
static void MD5Update (MD5_CTX *context, unsigned char *input, unsigned int inputLen)
{
    unsigned int    i;
    unsigned int    index;
    unsigned int    partLen;

    /* Compute number of bytes mod 64 */
    index = (unsigned int) ((context->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((context->count[0] += ((unsigned long) inputLen << 3))
                < ((unsigned long) inputLen << 3))
        context->count[1]++;
    context->count[1] += ((unsigned long) inputLen >> 29);

    partLen = 64 - index;

    /*
     * Transform as many times as possible.
     */
    if (inputLen >= partLen)
    {
        MD5_memcpy
            ((POINTERCHAR) & context->buffer[index], (POINTERCHAR) input, partLen);
        MD5Transform (context->state, context->buffer);

        for (i = partLen; i + 63 < inputLen; i += 64)
            MD5Transform (context->state, &input[i]);

        index = 0;
    }
    else
        i = 0;

    /* Buffer remaining input */
    MD5_memcpy
        ((POINTERCHAR) & context->buffer[index], (POINTERCHAR) & input[i],
         inputLen - i);
}

/**************************************************************************
函数名称:           MD5Final
函数描述:           
调用本函数函数: rc_md5_calc
被访问的表:        无  
被修改的表:        无   
输入:                 context  输入数据?
输出:                 digest 
返回:                 无
其它说明:           无
***************************************************************************/
static void MD5Final (unsigned char *digest, MD5_CTX *context)
{
    unsigned char   bits[8];
    unsigned int    index,
                    padLen;

    /* Save number of bits */
    Encode (bits, context->count, 8);

    /*
     * Pad out to 56 mod 64.
     */
    index = (unsigned int) ((context->count[0] >> 3) & 0x3f);
    padLen = (index < 56) ? (56 - index) : (120 - index);
    MD5Update (context, PADDING, padLen);

    /* Append length (before padding) */
    MD5Update (context, bits, 8);

    /* Store state in digest */
    Encode (digest, context->state, 16);

    /*
     * Zeroize sensitive information.
     */
    MD5_memset ((POINTERCHAR) context, 0, sizeof (*context));
}

/**************************************************************************
函数名称:           MD5Transform
函数描述:           解码
调用本函数函数:  MD5Update
被访问的表:        无  
被修改的表:        无   
输入:                 state  
                        block
输出:                 无
返回:                 无
其它说明:           无
***************************************************************************/
static void MD5Transform (UINT4 *state, unsigned char *block)
{
    unsigned long           a = state[0];
    unsigned long           b = state[1];
    unsigned long           c = state[2];
    unsigned long           d = state[3];
    unsigned long           x[16];

    Decode (x, block, 64);

    /* Round 1 */
    FF (a, b, c, d, x[0], S11, 0xd76aa478); /* 1 */
    FF (d, a, b, c, x[1], S12, 0xe8c7b756);/* 2 */
    FF (c, d, a, b, x[2], S13, 0x242070db);/* 3 */
    FF (b, c, d, a, x[3], S14, 0xc1bdceee);/* 4 */
    FF (a, b, c, d, x[4], S11, 0xf57c0faf);/* 5 */
    FF (d, a, b, c, x[5], S12, 0x4787c62a);/* 6 */
    FF (c, d, a, b, x[6], S13, 0xa8304613);/* 7 */
    FF (b, c, d, a, x[7], S14, 0xfd469501);/* 8 */
    FF (a, b, c, d, x[8], S11, 0x698098d8);/* 9 */
    FF (d, a, b, c, x[9], S12, 0x8b44f7af);/* 10 */
    FF (c, d, a, b, x[10], S13, 0xffff5bb1);/* 11 */
    FF (b, c, d, a, x[11], S14, 0x895cd7be);/* 12 */
    FF (a, b, c, d, x[12], S11, 0x6b901122);/* 13 */
    FF (d, a, b, c, x[13], S12, 0xfd987193);/* 14 */
    FF (c, d, a, b, x[14], S13, 0xa679438e);/* 15 */
    FF (b, c, d, a, x[15], S14, 0x49b40821);/* 16 */

    /* Round 2 */
    GG (a, b, c, d, x[1], S21, 0xf61e2562);/* 17 */
    GG (d, a, b, c, x[6], S22, 0xc040b340);/* 18 */
    GG (c, d, a, b, x[11], S23, 0x265e5a51);/* 19 */
    GG (b, c, d, a, x[0], S24, 0xe9b6c7aa);/* 20 */
    GG (a, b, c, d, x[5], S21, 0xd62f105d);/* 21 */
    GG (d, a, b, c, x[10], S22, 0x2441453);/* 22 */
    GG (c, d, a, b, x[15], S23, 0xd8a1e681);/* 23 */
    GG (b, c, d, a, x[4], S24, 0xe7d3fbc8);/* 24 */
    GG (a, b, c, d, x[9], S21, 0x21e1cde6);/* 25 */
    GG (d, a, b, c, x[14], S22, 0xc33707d6);/* 26 */
    GG (c, d, a, b, x[3], S23, 0xf4d50d87);/* 27 */
    GG (b, c, d, a, x[8], S24, 0x455a14ed);/* 28 */
    GG (a, b, c, d, x[13], S21, 0xa9e3e905);/* 29 */
    GG (d, a, b, c, x[2], S22, 0xfcefa3f8);/* 30 */
    GG (c, d, a, b, x[7], S23, 0x676f02d9);/* 31 */
    GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);/* 32 */

    /* Round 3 */
    HH (a, b, c, d, x[5], S31, 0xfffa3942);/* 33 */
    HH (d, a, b, c, x[8], S32, 0x8771f681);/* 34 */
    HH (c, d, a, b, x[11], S33, 0x6d9d6122);/* 35 */
    HH (b, c, d, a, x[14], S34, 0xfde5380c);/* 36 */
    HH (a, b, c, d, x[1], S31, 0xa4beea44);/* 37 */
    HH (d, a, b, c, x[4], S32, 0x4bdecfa9);/* 38 */
    HH (c, d, a, b, x[7], S33, 0xf6bb4b60);/* 39 */
    HH (b, c, d, a, x[10], S34, 0xbebfbc70);/* 40 */
    HH (a, b, c, d, x[13], S31, 0x289b7ec6);/* 41 */
    HH (d, a, b, c, x[0], S32, 0xeaa127fa);/* 42 */
    HH (c, d, a, b, x[3], S33, 0xd4ef3085);/* 43 */
    HH (b, c, d, a, x[6], S34, 0x4881d05);/* 44 */
    HH (a, b, c, d, x[9], S31, 0xd9d4d039);/* 45 */
    HH (d, a, b, c, x[12], S32, 0xe6db99e5);/* 46 */
    HH (c, d, a, b, x[15], S33, 0x1fa27cf8);/* 47 */
    HH (b, c, d, a, x[2], S34, 0xc4ac5665);/* 48 */

    /* Round 4 */
    II (a, b, c, d, x[0], S41, 0xf4292244);/* 49 */
    II (d, a, b, c, x[7], S42, 0x432aff97);/* 50 */
    II (c, d, a, b, x[14], S43, 0xab9423a7);/* 51 */
    II (b, c, d, a, x[5], S44, 0xfc93a039);/* 52 */
    II (a, b, c, d, x[12], S41, 0x655b59c3);/* 53 */
    II (d, a, b, c, x[3], S42, 0x8f0ccc92);/* 54 */
    II (c, d, a, b, x[10], S43, 0xffeff47d);/* 55 */
    II (b, c, d, a, x[1], S44, 0x85845dd1);/* 56 */
    II (a, b, c, d, x[8], S41, 0x6fa87e4f);/* 57 */
    II (d, a, b, c, x[15], S42, 0xfe2ce6e0);/* 58 */
    II (c, d, a, b, x[6], S43, 0xa3014314);/* 59 */
    II (b, c, d, a, x[13], S44, 0x4e0811a1);/* 60 */
    II (a, b, c, d, x[4], S41, 0xf7537e82);/* 61 */
    II (d, a, b, c, x[11], S42, 0xbd3af235);/* 62 */
    II (c, d, a, b, x[2], S43, 0x2ad7d2bb);/* 63 */
    II (b, c, d, a, x[9], S44, 0xeb86d391);/* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;

    /*
     * Zeroize sensitive information.
     */
    MD5_memset ((POINTERCHAR) x, 0, sizeof (x));
}


/**************************************************************************
函数名称:           Encode
函数描述:           解码
调用本函数函数:  MD5Transform
被访问的表:        无  
被修改的表:        无   
输入:                 input  编码数据
                        len 编码数据长度
输出:                 output 输出buffer 
返回:                 无
其它说明:           无
***************************************************************************/
static void Encode (unsigned char *output, UINT4 *input, unsigned int len)
{
    unsigned int    i;
    unsigned int    j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[j] = (unsigned char) (input[i] & 0xff);
        output[j + 1] = (unsigned char) ((input[i] >> 8) & 0xff);
        output[j + 2] = (unsigned char) ((input[i] >> 16) & 0xff);
        output[j + 3] = (unsigned char) ((input[i] >> 24) & 0xff);
    }
}

/**************************************************************************
函数名称:           Decode
函数描述:           解码
调用本函数函数:  MD5Transform
被访问的表:        无  
被修改的表:        无   
输入:                 input  解码数据
                        len 解码数据长度
输出:                 output 输出buffer 
返回:                 无
其它说明:           无
***************************************************************************/
static void Decode (UINT4 *output, unsigned char *input, unsigned int len)
{
    unsigned int    i;
    unsigned int    j;

    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        output[i] = ((unsigned int) input[j]) | (((unsigned int) input[j + 1]) << 8) |
        (((unsigned int) input[j + 2]) << 16) | (((unsigned int) input[j + 3]) << 24);
    }
}

/**************************************************************************
函数名称:           MD5_memcpy
函数描述:           拷贝数据
调用本函数函数:  MD5Update
被访问的表:        无  
被修改的表:        无   
输入:                 input  要拷贝的数据指针
                        len 拷贝数据的长度
输出:                 output 输出buffer 
返回:                 无
其它说明:           无
***************************************************************************/
static void MD5_memcpy (POINTERCHAR output, POINTERCHAR input, unsigned int len)
{
    memcpy(output, input, len);
}

/**************************************************************************
函数名称:           MD5_memset
函数描述:           设置数据到buffer 中
调用本函数函数:  MD5Transform MD5Final
被访问的表:        无  
被修改的表:        无   
输入:                 value  要设置的数据
                        len buffer的长度
输出:                 output 输出buffer 
返回:                 无
其它说明:           无
***************************************************************************/
static void MD5_memset (POINTERCHAR output, int value, unsigned int len)
{
    memset(output, value, len);
}

