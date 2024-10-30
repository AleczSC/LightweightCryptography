#include <Lizard.h>


uint8_t f1(uint8_t *S)
{
    uint8_t S30;

    S30 = S[0] ^ (S[0] << 2) ^ (S[0] << 5) ^ (S[0] << 6) ^ (S[1] << 7) ^ (S[2] << 1) ^ (S[2] << 2) ^ (S[2] << 4) ^ (S[3] << 1) \
        ^ (S[1] & (S[2] << 2)) ^ (S[1] & (S[2] << 4)) ^ ((S[1] << 4) & (S[2] << 5)) ^ ((S[1] << 6) & (S[2] << 3)) ^ ((S[2] << 1) & (S[2] << 5)) ^ ((S[2] << 4) & (S[2] << 6)) \
        ^ ((S[0] << 4) & (S[1] << 4) & (S[2] << 6)) ^ ((S[0] << 4) & (S[2] << 3) & (S[2] << 6)) ^ ((S[0] << 7) & (S[2] << 4) & (S[2] << 5)) ^ (S[1] & (S[2] << 2) & (S[2] << 6)) \
        ^ (S[1] & (S[2] << 4) & (S[2] << 6)) ^ ((S[1] << 4) & (S[2] << 3) & (S[2] << 6)) ^ ((S[2] << 4) & (S[2] << 5) & (S[2] << 6)) ^ ((S[0] << 4) & (S[0] << 7) & (S[1] << 4) & (S[2] << 5)) \
        ^ ((S[0] << 4) & (S[0] << 7) & (S[2] << 3) & (S[2] << 5)) ^ ((S[0] << 4) & (S[1] << 6) & (S[2] << 5) & (S[2] << 6)) ^ ((S[0] << 4) & (S[2] << 3) & (S[2] << 5) & (S[2] << 6)) \
        ^ ((S[0] << 7) & S[1] & (S[2] << 2) & (S[2] << 5)) ^ ((S[0] << 7) & S[1] & (S[2] << 4) & (S[2] << 5)) & ((S[0] << 7) & (S[1] << 4) & (S[2] << 3) & (S[2] << 5)) \
        ^ (S[1] & (S[2] << 2) & (S[2] << 5) & (S[2] << 6)) ^ (S[1] & (S[2] << 4) & (S[2] << 5) & (S[2] << 6)) ^ ((S[1] << 4) & (S[2] << 3) & (S[2] << 5) & (S[2] << 6));

    return S30 & 0x80;
}

uint8_t f2(uint8_t *S, uint8_t *B)
{
    uint8_t B89;

    B89 = S[0] ^ B[0] ^ B[3] ^ (B[6] << 1) ^ (B[9] << 7) ^ (B[10] << 4) ^ ((B[0] << 3) & (B[7] << 3)) ^ ((B[1] << 2) & (B[2] << 4)) \
        ^ ((B[1] << 7) & B[2]) ^ ((B[3] << 1) & (B[6] << 5)) ^ ((B[4] << 3) & (B[5] << 2)) ^ ((B[6] << 7) & (B[7] << 2)) ^ ((B[7] << 4) & (B[9] << 2)) \
        ^ ((B[2] << 4) & (B[2] << 6) & (B[2] << 7)) ^ ((B[7] << 6) & (B[8] << 4) & B[9]) ^ ((B[9] << 5) & B[10] & (B[10] << 1) & (B[10] << 3));

    return B89 & 0x80;
}

uint8_t fa(uint8_t *S, uint8_t *B)
{
    uint8_t l, q, t, ts;

    l = (B[0] << 7) ^ (B[1] << 3) ^ (B[3] << 6) ^ B[5] ^ (B[5] << 5) ^ (B[6] << 6) ^ (B[8] << 7);
    l &= 0x80;

    q = ((B[0] << 4) & (B[2] << 5)) ^ ((B[1] << 1) & (B[6] << 4)) ^ ((B[2] << 2) & (B[4] << 5)) ^ ((B[5] << 4) & (B[9] << 4));
    q &= 0x80;

    t = (B[0] << 5) ^ (B[1] & (B[10] << 2)) ^ ((B[4] << 2) & (B[8] << 3) & (B[9] << 1)) ^ ((B[0] << 2) & (B[3] << 4) & (B[5] << 1) & (B[8] << 1)) \
      ^ ((B[1] << 5) & (B[3] << 5) & (B[6] << 2) & B[8] & (B[9] << 3)) ^ ((B[0] << 6) & (B[1] << 6) & (B[3] << 2) & B[4] & (B[5] << 7) & (B[7] << 5)) \
      ^ ((B[0] << 1) & (B[2] << 3) & (B[3] << 3) & (B[5] << 3) & (B[7] << 1) & (B[8] << 2) & (B[9] << 6));
    t &= 0x80;

    ts = (S[2] << 7) ^ ((S[0] << 3) & S[2]) ^ ((S[1] << 1) & (S[1] << 5) & B[6]) ^ ((S[0] << 1) & S[3] & (B[4] << 6) & (B[7] << 7));

    return l ^ q ^ t ^ ts;
}

void initState(lizardStruct *lizard)
{
    uint8_t tempKey[4];
    uint8_t z;
    uint8_t B89, S30;

    //Phase 1: Key and IV loading
    for (uint8_t j = 0; j < 8; j++)
        lizard -> NFSR2[j] = lizard -> key[j] ^ lizard -> iv[j];
    for (uint8_t j = 8; j < 12 ; j++)
        lizard -> NFSR2[j] = lizard -> key[j];
    lizard -> NFSR2[11] &= 0xC0;

    for (uint8_t i = 0; i < 4; i++)
    {
        lizard -> NFSR1[i] = lizard -> key[11 + i] << 2;
        
        if (i != 3)
            lizard -> NFSR1[i] ^= lizard -> key[12 + i] >> 6;
    }
    lizard -> NFSR1[3] &= 0xF8;
    lizard -> NFSR1[3] ^= ((lizard -> key[14] & 0x01) << 2) ^ 0x04;
    lizard -> NFSR1[3] ^= 0x02;

    //Phase 2: Grain-like mixing
    for (uint8_t i = 0; i < 128; i++)
    {
        z = fa(lizard -> NFSR1, lizard -> NFSR2);
        updateNFSRs(lizard -> NFSR1, lizard -> NFSR2);

        lizard -> NFSR1[3] ^= z >> 6;
        lizard -> NFSR2[11] ^= z >> 1;
    }

    //Phase 3: Second Key Addition
    for (uint8_t j = 0; j < 12; j++)
        lizard -> NFSR2[j] ^= lizard -> key[j];
    lizard -> NFSR2[11] &= 0xC0;

    for (uint8_t i = 0; i < 4; i++)
    {
            lizard -> NFSR1[i] ^= lizard -> key[11 + i] << 2;
        
        if (i != 3)
            lizard -> NFSR1[i] ^= lizard -> key[12 + i] >> 6;
    }
    lizard -> NFSR1[3] &= 0xF8;
    lizard -> NFSR1[3] |= 0x02;
    
    //Phase 4: Final diffusion
    for (uint8_t i = 0; i < 128; i++)
    {
        updateNFSRs(lizard -> NFSR1, lizard -> NFSR2);
    }


}   

void updateNFSRs(uint8_t *S, uint8_t *B)
{
    uint8_t S30, B89;

    S30 = f1(S);
    B89 = f2(S, B);

    for (uint8_t i = 0; i < 4; i++)
    {
        S[i] <<= 1;
        if (i != 2)
            S[i] ^= S[i + 1] >> 7;

    }
    S[3] ^= S30 >> 6;

    for (uint8_t j = 0; j < 12 ; j++)
    {
        B[j] <<= 1;
        if (j != 11)
            B[j] ^= B[j + 1] >> 7;
    }
    B[11] ^= B89 >> 1;
}

uint8_t generateKeyStreamByte(lizardStruct *lizard)
{
    uint8_t z, KS;

    KS = 0;
    for (uint8_t i = 0; i < 8; i++)
    {
        z = fa(lizard -> NFSR1, lizard -> NFSR2);
        updateNFSRs(lizard -> NFSR1, lizard -> NFSR2);

        KS ^= z >> i;
    }

    return KS;
}

void lizard_encrypt(lizardStruct *lizard, uint8_t *plaintext, uint16_t length)
{
    uint8_t KS;
    initState(lizard);

    for(uint16_t i = 0; i < length; i++)
    {
        KS = generateKeyStreamByte(lizard);
        plaintext[i] ^= KS;
    }
}