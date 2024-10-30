#ifndef PICCOLO80_V2_H_
#define PICCOLO80_V2_H_

#include <stdint.h>

#define ROUNDS_80 25
#define GF_POLY 0x13
#define DEG_GF_POLY 4

typedef struct {
  uint16_t wk[4];  //whitening keys
  uint16_t rk[2*ROUNDS_80];  //round keys: 2 round keys are encoded in a 32-bit word */
} KeyStruct;

extern const uint8_t SBox[16];
extern const uint8_t M[4][4];
extern const uint32_t con80[ROUNDS_80];

//Functions
void keySchedule(uint8_t *K, KeyStruct *KS);
void invKeySchedule(uint8_t *K, KeyStruct *KS);
uint8_t gm(uint8_t a, uint8_t b);
uint16_t F_function(uint16_t xData);
void RP(uint8_t *data);
void RP16(uint16_t *data);
void encrypt(uint8_t *plaintext, KeyStruct *KS);
void data8to16(uint8_t *data, uint16_t *X);
void data16to8(uint16_t *X, uint8_t *data);

#endif