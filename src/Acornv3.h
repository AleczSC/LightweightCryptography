#ifndef ACORNV3_H
#define ACORNV3_H

#include <stdint.h>
#include <string.h>

#define GET_32BITS(A, index, shift) (((A)[(index)] << (shift)) | ((A)[(index) + 1] >> (32 - (shift))))

#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) ^ (z)))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))

typedef struct
{
    uint8_t key[16];
    uint8_t iv[16];
    uint8_t tag[16];
    uint32_t state[10];
} Acornv3Struct;

//Functions
inline uint32_t KSG128_32(uint32_t *S);
inline uint8_t KSG128_8(uint32_t *S);
inline uint32_t FBK128_32(uint32_t *S, uint32_t ca, uint32_t cb, uint32_t ks);
inline uint8_t FBK128_8(uint32_t *S, uint8_t ca, uint8_t cb, uint8_t ks);
uint32_t stateUpdate128_32(uint32_t *S, uint32_t m, uint32_t ca, uint32_t cb);
uint8_t stateUpdate128_8(uint32_t *S, uint8_t m, uint8_t ca, uint8_t cb);
void initState(uint32_t* S, uint8_t* K, uint8_t * IV);
void updateStateAD(uint32_t* S, uint8_t *ad, uint32_t adlen);
void Acornv3_encrypt(uint8_t* plaintext, Acornv3Struct* acornv3, uint8_t* ad, uint32_t pclen, uint32_t adlen);
void generateTag(Acornv3Struct* acornv3);
void Acornv3_decrypt(uint8_t* ciphertext, Acornv3Struct* acornv3, uint8_t* ad, uint32_t pclen, uint32_t adlen);

#endif