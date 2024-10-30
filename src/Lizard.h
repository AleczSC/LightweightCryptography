#ifndef LIZARD_H_
#define LIZARD_H_

#include <stdint.h>

typedef struct 
{
    uint8_t key[15];
    uint8_t iv[8];
    uint8_t NFSR2[12];
    uint8_t NFSR1[4];
    
} lizardStruct;

//Functions
uint8_t f1(uint8_t *S);
uint8_t f2(uint8_t *S, uint8_t *B);
uint8_t fa(uint8_t *S, uint8_t *B);
void initState(lizardStruct *lizard);
void updateNFSRs(uint8_t *S, uint8_t *B);
uint8_t generateKeyStreamByte(lizardStruct *lizard);
void lizard_encrypt(lizardStruct *lizard, uint8_t *plaintext, uint16_t length);


#endif