#ifndef FRUIT_80_H_
#include <stdint.h>
#include <string.h>

#define GET_BIT(A, I) ((A)[(I) >> 3] << ((I) & 0x07))

typedef struct
{
    uint8_t key[10];
    uint8_t iv[9];
    uint8_t LFSR[6];
    uint8_t NFSR[5];
    uint8_t Cr;
} fruitv2Struct;

//Functions 
uint8_t rIndex(uint8_t Cr);
uint8_t pIndex(uint8_t Cr);
uint8_t qIndex(uint8_t Cr);
uint8_t roundKeyFunction(uint8_t *k, uint8_t Cr);
uint8_t gFunction(uint8_t kt, uint8_t *l, uint8_t *n, uint8_t Cr);
uint8_t fFunction(uint8_t *l);
uint8_t hFunction(uint8_t kt, uint8_t *l, uint8_t *n);
uint8_t outputFunction(uint8_t h, uint8_t *n, uint8_t *l);
void initCipher(fruitv2Struct *fruit);
void updateCipher(fruitv2Struct *fruit, uint8_t n37, uint8_t l43);
uint8_t generateKeyStreamByte(fruitv2Struct *fruit);
void fruit80_encrypt(fruitv2Struct *fruit, uint8_t *plaintext, uint16_t length);
void arrayLeftShift(uint8_t *array, uint8_t shift, uint8_t length);

#endif