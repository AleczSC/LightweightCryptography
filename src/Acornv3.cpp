#include <Acornv3.h>

inline uint32_t KSG128_32(uint32_t *S)
{
    // ks = S12 ^ S154 ^ maj(S235, S61, S193) ^ ch(S230, S111, S66);
    return GET_32BITS(S, 0, 12) ^ GET_32BITS(S, 4, 26) ^ MAJ(GET_32BITS(S, 7, 11), GET_32BITS(S, 1, 29), GET_32BITS(S, 6, 1)) ^ CH(GET_32BITS(S, 7, 6), GET_32BITS(S, 3, 15), GET_32BITS(S, 2, 2));
}

inline uint8_t KSG128_8(uint32_t *S)
{
    //ks = S12 ^ S154 ^ maj(S235, S61, S193) ^ ch(S230, S111, S66)
    return (S[0] >> 12) ^ ((S[4] << 2) ^ (S[5] >> 30)) ^ MAJ(S[7] >> 13, (S[1] << 5) ^ (S[2] >> 27), S[6] >> 23) ^ CH(S[7] >> 18, S[3] >> 9, S[2] >> 22);
}

inline uint32_t FBK128_32(uint32_t *S, uint32_t ca, uint32_t cb, uint32_t ks)
{
    // f = S0 ^ ~S107 ^ maj(S244, S23, S160) ^ (ca & S196) ^ (cb & ks);
    return S[0] ^ (~GET_32BITS(S, 3, 11)) ^ MAJ(GET_32BITS(S, 7, 20), GET_32BITS(S, 0, 23), S[5]) ^ (ca & GET_32BITS(S, 6, 4)) ^ (cb & ks);
}

inline uint8_t FBK128_8(uint32_t *S, uint8_t ca, uint8_t cb, uint8_t ks)
{
    // f = S0 ^ ~S107 ^ maj(S244, S23, S160) ^ (ca & S196) ^ (cb & ks);
    return (S[0] >> 24) ^ (~(S[3] >> 13)) ^ MAJ(S[7] >> 4, S[0] >> 1, S[5] >> 24) ^ (ca & (S[6] >> 20)) ^ (cb & ks);
}

uint32_t stateUpdate128_32(uint32_t *S, uint32_t m, uint32_t ca, uint32_t cb)
{
    uint32_t ks, f;
    uint32_t temp;

    /*Step 1. Update using six LFSRs*/
    //S289 = S289 ^ S235 ^ S230
    temp = GET_32BITS(S, 7, 11) ^ GET_32BITS(S, 7, 6);
    S[9] ^= temp >> 1;

    //S230 = S230 ^ S196 ^ S193
    temp = GET_32BITS(S, 6, 4) ^ GET_32BITS(S, 6, 1);
    S[7] ^= temp >> 6;
    S[8] ^= temp << 26;

    //S193 = S193 ^ S160 ^ S154
    temp = S[5] ^ GET_32BITS(S, 4, 26);
    S[6] ^= temp >> 1;
    S[7] ^= temp << 31;

    //S154 = S154 ^ S111 ^ S107
    temp = GET_32BITS(S, 3, 15) ^ GET_32BITS(S, 3, 11);
    S[4] ^= temp >> 26;
    S[5] ^= temp << 6;

    //S107 = S107 ^ S66 ^ S61
    temp = GET_32BITS(S, 2, 2) ^ GET_32BITS(S, 1, 29);
    S[3] ^= temp >> 11;
    S[4] ^= temp << 21;

    //S61 = S61 ^ S23 ^ S0
    temp = GET_32BITS(S, 0, 23) ^ S[0];
    S[1] ^= temp >> 29;
    S[2] ^= temp << 3;

    /*Step 2: Generate 32 keystream bits*/
    ks = KSG128_32(S);

    /*Step 3: Generate the 32 nonlinear feedback bits*/
    f = FBK128_32(S, ca, cb, ks);

    /*Step 4: Shift the 293 bit register 32 times and add the feedback bits*/
    for(uint8_t i = 0; i < 9; i++)
        S[i] = S[i + 1];

    f ^= m;
    S[8] ^= f >> 5;
    S[9] = f << 27;

    //Update finished. Return the keystream.
    return ks;
}

uint8_t stateUpdate128_8(uint32_t *S, uint8_t m, uint8_t ca, uint8_t cb)
{
    uint8_t ks, f;
    uint8_t temp;

    /*Step 1. Update using six LFSRs*/
    //S289 = S289 ^ S235 ^ S230
    temp = (S[7] >> 13) ^ (S[7] >> 18);
    S[9] ^= (uint32_t) temp << 23;

    //S230 = S230 ^ S196 ^ S193
    temp = (S[6] >> 20) ^ (S[6] >> 23);
    S[7] ^= (uint32_t) temp << 18;

    //S193 = S193 ^ S160 ^ S154
    temp = (S[5] >> 24) ^ ((S[4] << 2) ^ (S[5] >> 30));
    S[6] ^= (uint32_t) temp << 23;

    //S154 = S154 ^ S111 ^ S107
    temp = (S[3] >> 9) ^ (S[3] >> 13);
    S[4] ^= temp >> 2;
    S[5] ^= (uint32_t) temp << 30;

    //S107 = S107 ^ S66 ^ S61
    temp = (S[2] >> 22) ^ ((S[1] << 5) ^ (S[2] >> 27));
    S[3] ^= (uint32_t) temp << 13;

    //S61 = S61 ^ S23 ^ S0
    temp = GET_32BITS(S, 0, 23) ^ S[0];
    temp = (S[0] >> 1) ^ (S[0] >> 24);
    S[1] ^= temp >> 5;
    S[2] ^= (uint32_t) temp << 29;

    /*Step 2: Generate 8 keystream bits*/
    ks = KSG128_8(S);

    /*Step 3: Generate the 8 nonlinear feedback bits*/
    f = FBK128_8(S, ca, cb, ks);

    /*Step 4: Shift the 293 bit register 32 times and add the feedback bits*/
    for(uint8_t i = 0; i < 9; i++)
    {
        S[i] = S[i] << 8;
        S[i] ^= S[i + 1] >> 24;
    }

    f ^= m;
    S[8] = f >> 5;
    S[9] = (uint32_t) f << 27;

    //Update finished. Return the keystream.
    return ks;
}

void initState(uint32_t* S, uint8_t* K, uint8_t * IV)
{
    uint32_t m;

    for(uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            m <<= 8;
            m ^= K[(i << 2) + j];
        }
        stateUpdate128_32(S, m, 0xFFFFFFFF, 0xFFFFFFFF);
    }

    for(uint8_t i = 0; i < 4; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            m <<= 8;
            m ^= IV[(i << 2) + j];
        }
        stateUpdate128_32(S, m, 0xFFFFFFFF, 0xFFFFFFFF);
    }

    for(uint8_t i = 0; i < 4; i ++)
    {
        m <<= 8;
        m ^= K[i];
    }
    stateUpdate128_32(S, m ^ 0x80000000, 0xFFFFFFFF, 0xFFFFFFFF);

    for(uint8_t i = 1; i < 48; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            m <<= 8;
            m ^= K[((i << 2) % 0x0F + j)];
        }
        stateUpdate128_32(S, m, 0xFFFFFFFF, 0xFFFFFFFF);
    }
}

void updateStateAD(uint32_t* S, uint8_t *ad, uint32_t adlen)
{
    uint32_t m;

    for(uint8_t i = 0; i < adlen; i++)
    {
        m = ad[i];
        stateUpdate128_8(S, m, 0xFF, 0xFF);
    }

    stateUpdate128_32(S, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFF);
    stateUpdate128_32(S, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF);
    stateUpdate128_32(S, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF);
    stateUpdate128_32(S, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF);

    stateUpdate128_32(S, 0x00000000, 0x00000000, 0xFFFFFFFF);
    stateUpdate128_32(S, 0x00000000, 0x00000000, 0xFFFFFFFF);
    stateUpdate128_32(S, 0x00000000, 0x00000000, 0xFFFFFFFF);
    stateUpdate128_32(S, 0x00000000, 0x00000000, 0xFFFFFFFF);
}

void Acornv3_encrypt(uint8_t* plaintext, Acornv3Struct* acornv3, uint8_t* ad, uint32_t pclen, uint32_t adlen)
{
    uint32_t m;
    uint32_t KS;

    memset(acornv3 -> state, 0x00, 10 << 2);
    initState(acornv3 -> state, acornv3 -> key, acornv3 -> iv);
    updateStateAD(acornv3 -> state, ad, adlen);

    while(pclen >= 4)
    {
        for(uint8_t i = 0; i < 4; i++)
        {
            m <<= 8;
            m ^= plaintext[i];
        }
        KS = stateUpdate128_32(acornv3 -> state, m, 0xFFFFFFFF, 0x00000000);
        m ^= KS;

        for(int8_t i = 3; i >= 0; i--)
        {
            plaintext[i] = m;
            m >>= 8;
        }
        
        plaintext += 4;
        pclen -= 4;
    }

    while(pclen)
    {
        m = *plaintext;
        KS = stateUpdate128_8(acornv3 -> state, m, 0xFF, 0x00);
        *plaintext ^= KS;

        plaintext++;
        pclen--;
    }

    stateUpdate128_32(acornv3 -> state, 0x80000000, 0xFFFFFFFF, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);

    stateUpdate128_32(acornv3 -> state, 0x00000000, 0x00000000, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0x00000000, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0x00000000, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0x00000000, 0x00000000);

    generateTag(acornv3);
}

void generateTag(Acornv3Struct* acornv3)
{
    uint32_t KS;

    for(uint8_t i = 0; i < 20; i++)
        stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF);

    for(uint8_t i = 0; i < 4; i++)
    {
        KS = stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF);
        for(int8_t j = 3; j >= 0; j--)
        {
            acornv3 -> tag[(i << 2) + j] = KS;
            KS >>= 8;
        }
    }
}

void Acornv3_decrypt(uint8_t* ciphertext, Acornv3Struct* acornv3, uint8_t* ad, uint32_t pclen, uint32_t adlen)
{
    uint32_t m;
    uint32_t KS;

    uint8_t tempTag[16];
    memcpy(tempTag, acornv3 -> tag, 16);

    memset(acornv3 -> state, 0x00, 10 << 2);
    initState(acornv3 -> state, acornv3 -> key, acornv3 -> iv);
    updateStateAD(acornv3 -> state, ad, adlen);

    while (pclen >= 4)
    {
        m = 0x00000000;
        KS = stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);
        for(int8_t i = 3; i >= 0; i--)
        {
            ciphertext[i] ^= KS;
            KS >>= 8;
            m ^= ciphertext[i] << ((3 - i) << 3);
        }
        acornv3 -> state[8] ^= m >> 5;
        acornv3 -> state[9] ^= m << 27;
    
        ciphertext += 4;
        pclen -= 4;
    }

    while (pclen)
    {
        KS = stateUpdate128_8(acornv3 -> state, 0x00, 0xFF, 0x00);
        *ciphertext ^= KS;

        acornv3 -> state[8] ^= (*ciphertext) >> 5;
        acornv3 -> state[9] ^= ((uint32_t) *ciphertext) << 27;

        ciphertext++;
        pclen--;
    }

    stateUpdate128_32(acornv3 -> state, 0x80000000, 0xFFFFFFFF, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);
    stateUpdate128_32(acornv3 -> state, 0x00000000, 0xFFFFFFFF, 0x00000000);

    for(uint8_t i = 0; i < 4; i++)
        stateUpdate128_32(acornv3 -> state, 0x00000000, 0x00000000, 0x00000000);

    generateTag(acornv3);

    if (memcmp(acornv3 -> tag, tempTag, 16))
        memset(ciphertext, 0x00, pclen);
}
/*
uint8_t inline maj8(uint8_t x, uint8_t y, uint8_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

inline uint32_t maj32(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & y) ^ (x & z) ^ (y & z);
}

uint8_t inline ch8(uint8_t x, uint8_t y, uint8_t z)
{
    return (x & z) ^ (~x & z);
}

inline uint32_t ch32(uint32_t x, uint32_t y, uint32_t z)
{
    return (x & z) ^ (~x & z);
}
*/