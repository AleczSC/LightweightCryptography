#ifndef HUMMINGBIRD_2_H_
#define HUMMINGIBIRD_2_H_

#include <stdint.h>

typedef struct
{
    uint8_t key[16];
    uint8_t iv[8];
    uint16_t state[8];
} Hummingibird2Struct;

uint16_t useBox(uint16_t x, const uint8_t box[4][16]);
inline uint16_t L(uint16_t x);
uint16_t invL(uint16_t y);
inline uint16_t f(uint16_t x);
inline uint16_t invf(uint16_t y);
inline uint16_t WD16(uint16_t x, uint16_t a, uint16_t b, uint16_t c, uint16_t d);
uint16_t invWD16(uint16_t y, uint16_t a, uint16_t b, uint16_t c, uint16_t d);
void initState(uint16_t *K, uint16_t *R);
uint16_t encryptWord(uint16_t P, uint16_t *K, uint16_t *R);
uint16_t decryptWord(uint16_t Ct, uint16_t *K, uint16_t *R);

void stateUpdate(uint16_t *R, uint16_t t1, uint16_t t2, uint16_t t3);

void Hummingbird_encrypt(Hummingibird2Struct *hummingbird, uint8_t *plaintext, uint16_t length);
void Hummingbird_decrypt(Hummingibird2Struct *hummingbird, uint8_t *ciphertext, uint16_t length);
uint16_t TwoBytesToWord(uint8_t *x);
void wordToTwoBytes(uint16_t x, uint8_t *y);
inline uint16_t lcs(uint16_t x, uint8_t s);
inline uint16_t rcs(uint16_t x, uint8_t s);
inline uint16_t addMod65536(uint16_t a, uint16_t b);
inline uint16_t invAddMod65536(uint16_t a, uint16_t b);

#endif