#ifndef _SHA1_H
#define _SHA1_H
typedef struct {
    unsigned long state[5];
    unsigned long count[2];
    unsigned char buffer[64];
} SHA_CTX;

void SHA1_Transform(unsigned long state[5], const unsigned char buffer[64]);
void SHA1_Init(SHA_CTX* context);
void SHA1_Update(SHA_CTX* context, const unsigned char* data, unsigned long len);
void SHA1_Final(unsigned char digest[20], SHA_CTX* context);

#endif
