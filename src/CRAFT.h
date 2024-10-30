#ifndef CRAFT_H_
#define CRAFT_H_

#include <stdint.h>

#define TOTAL_ROUNDS 31

typedef struct{
    uint8_t TK0[8];
    uint8_t TK1[8];
    uint8_t TK2[8];
    uint8_t TK3[8];
}   TweakeysStruct;

extern const uint8_t Sbox[16];
extern const uint8_t Pbox[16];
extern const uint8_t RC[32];
extern const uint8_t Qbox[16];

void CRAFT_encrypt(uint8_t *plaintext, uint8_t *K, uint8_t *T);
void CRAFT_decrypt(uint8_t *ciphertext, uint8_t *K, uint8_t *T);
void permutation(uint8_t *vector, const uint8_t *box);
void subBoxToAllVector(uint8_t *vector);
void mixColumns(uint8_t *I);


#endif


