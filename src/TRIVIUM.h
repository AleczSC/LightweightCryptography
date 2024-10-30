#ifndef TRIVIUM_H_

#include <stdint.h>
#include <string.h>

#define GET_PREVIOUS_32BITS(A, index, shift) (((A)[(index)] >> (31 - shift)) | ((A)[(index) - 1] << ((shift) + 1)))

typedef struct {
    uint8_t key[10];
    uint8_t iv[10];
    uint32_t state[9];
} triviumStruct;

//Functions 
void initState(triviumStruct *trivium);
uint32_t keyStreamGeneration_32(uint32_t *s);
uint8_t keyStreamGeneration_8(uint32_t *s);
void trivium_encrypt(triviumStruct *trivium, uint8_t *plaintext, uint16_t length);

#endif