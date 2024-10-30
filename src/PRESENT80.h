#ifndef PRESENT80_H
#define PRESENT80_H

#include <stdint.h>
#include <string.h>

#define PRESENT_ROUNDS 31

#include <PRESENT80.h>

/*
const uint8_t SBox[16] = {0x0C, 0x05, 0x06, 0x0B, 0x09, 0x00, 0x0A, 0x0D,
                          0x03, 0x0E, 0x0F, 0x08, 0x04, 0x07, 0x01, 0x02};

const uint8_t invSBox[16] = {0x05, 0x0E, 0x0F, 0x08, 0x0C, 0x01, 0x02, 0x0D,
                             0x0B, 0x04, 0x06, 0x03, 0x00, 0x07, 0x09, 0x0A};
*/
void inline addRoundKey(uint8_t *b, uint8_t * k);
void boxLayer(uint8_t *b, const uint8_t *box);
void pLayer(uint8_t *b);
void invpLayer( uint8_t *b);
void generateRoundKey(uint8_t *rKey, uint8_t rc);
void invGenerateRoundKey(uint8_t *rKey, uint8_t rc);
void PRESENT80_encrypt(uint8_t *plaintext, uint8_t *key);
void PRESENT80_decrypt(uint8_t *ciphertext, uint8_t *key);
void lcsArray(uint8_t *array, uint8_t n, uint8_t length);
inline uint8_t bit_set(uint8_t byte, uint8_t n);
inline uint8_t bit_clear(uint8_t byte, uint8_t n);
inline uint8_t bit_check(uint8_t byte, uint8_t n);
#endif
