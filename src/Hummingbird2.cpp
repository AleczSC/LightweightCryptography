#include <Hummingbird-2.h>

const uint8_t S[4][16] = 
{
    { 7, 12, 14,  9,  2,  1,  5, 15, 11,  6, 13,  0,  4,  8, 10,  3},
    { 4, 10,  1,  6,  8, 15,  7, 12,  3,  0, 14, 13,  5,  9, 11,  2},
    { 2, 15, 12,  1,  5,  6, 10, 13, 14,  8,  3,  4,  0, 11,  9,  7},
    {15,  4,  5,  8,  9,  7,  2,  1, 10,  3,  0, 14,  6, 12, 13, 11}
};

const uint8_t invS[4][16] = 
{
    {11,  5,  4, 15, 12,  6,  9,  0, 13,  3, 14,  8,  1, 10,  2,  7},
    { 9,  2, 15,  8,  0, 12,  3,  6,  4, 13,  1, 14,  7, 11, 10,  5},
    {12,  3,  0, 10, 11,  4,  5, 15,  9, 14,  6, 13,  2,  7,  8,  1},
    {10,  7,  6,  9,  1,  2, 12,  5,  3,  4,  8, 15, 13, 14, 11,  0}
};

uint16_t useBox(uint16_t x, const uint8_t box[4][16])
{
    uint16_t y = 0x0000;

    for(uint8_t i = 0; i < 4; i++)
    {
        y ^= box[i][x & 0x000F]; 
        x >>= 4;
        y = lcs(y, 12);
    }

    return y;
}

inline uint16_t L(uint16_t x)
{
    return x ^ lcs(x, 6) ^ lcs(x, 10);
}

inline uint16_t invL(uint16_t y)
{
    /*
    uint16_t x = 0x0000;
    while(1)
    {
        if ((x == (y ^ lcs(x, 6) ^ lcs(x, 10))) || (x == 0xFFFF))
            break;

        x++;
    }

    return x;
    */

   return y ^ lcs(y,2) ^ lcs(y, 4) ^ lcs(y, 12) ^ lcs(y, 14);
}

inline uint16_t f(uint16_t x)
{
    return L(useBox(x, S));
}

inline uint16_t invf(uint16_t y)
{
    return useBox(invL(y), invS);
}

inline uint16_t WD16(uint16_t x, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
    return f(f(f(f(x ^ a) ^ b) ^ c)^ d);
}

uint16_t invWD16(uint16_t y, uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
    return invf(invf(invf(invf(y) ^ d) ^ c) ^ b) ^ a;
}

void initState(uint16_t *K, uint16_t *R)
{
    uint16_t t1, t2, t3, t4;
    
    for(uint16_t i = 0; i < 4; i++)
    {
        t1 = WD16(addMod65536(R[0], ~i + 1), K[0], K[1], K[2], K[3]);
        t2 = WD16(addMod65536(R[1], t1), K[4], K[5], K[6], K[7]);
        t3 = WD16(addMod65536(R[2], t2), K[0], K[1], K[2], K[3]);
        t4 = WD16(addMod65536(R[3], t3), K[4], K[5], K[6], K[7]);

        R[0] = lcs(addMod65536(R[0], t4), 3);
        R[1] = rcs(addMod65536(R[1], t1), 1);
        R[2] = lcs(addMod65536(R[2], t2), 8);
        R[3] = lcs(addMod65536(R[3], t3), 1);
        R[4] = R[4] ^ R[0];
        R[5] = R[5] ^ R[1];
        R[6] = R[6] ^ R[2];
        R[7] = R[7] ^ R[3];
    }
}

uint16_t encryptWord(uint16_t P, uint16_t *K, uint16_t *R)
{
    uint16_t t1, t2, t3;

    t1 = WD16(addMod65536(R[0], P), K[0], K[1], K[2], K[3]);
    t2 = WD16(addMod65536(R[1], t1), K[4] ^ R[4], K[5] ^ R[5], K[6] ^ R[6], K[7] ^ R[7]);
    t3 = WD16(addMod65536(R[2], t2), K[0] ^ R[4], K[1] ^ R[5], K[2] ^ R[6], K[3] ^ R[7]);
    
    P = addMod65536(WD16(addMod65536(R[3], t3), K[4], K[5], K[6], K[7]), R[0]);
    
    stateUpdate(R, t1, t2, t3);

    return P;
}

uint16_t decryptWord(uint16_t Ct, uint16_t *K, uint16_t *R)
{
    uint16_t t1, t2, t3;

    t3 = invAddMod65536(invWD16(invAddMod65536(Ct, R[0]), K[4], K[5], K[6], K[7]), R[3]);
    t2 = invAddMod65536(invWD16(t3, K[0] ^ R[4], K[1] ^ R[5], K[2] ^ R[6], K[3] ^ R[7]), R[2]);
    t1 = invAddMod65536(invWD16(t2, K[4] ^ R[4], K[5] ^ R[5], K[6] ^ R[6], K[7] ^ R[7]), R[1]);

    Ct = invAddMod65536(invWD16(t1, K[0], K[1], K[2], K[3]), R[0]);
    
    stateUpdate(R, t1, t2, t3);

    return Ct;
}

void stateUpdate(uint16_t *R, uint16_t t1, uint16_t t2, uint16_t t3)
{
    R[7] = R[7] ^ addMod65536(addMod65536(addMod65536(R[3], R[0]), t3), t1);
    R[6] = R[6] ^ addMod65536(R[2], t2);
    R[5] = R[5] ^ addMod65536(R[1], t1);
    R[4] = R[4] ^ addMod65536(R[0], t3);
    R[3] = addMod65536(R[3], addMod65536(R[0], addMod65536(t3, t1)));
    R[2] = addMod65536(R[2], t2);
    R[1] = addMod65536(R[1], t1);
    R[0] = addMod65536(R[0], t3);
}

void Hummingbird_encrypt(Hummingibird2Struct *hummingbird, uint8_t *plaintext, uint16_t length)
{
    uint16_t K[8];
    uint16_t *R = hummingbird -> state;
    uint16_t P;
    uint16_t Ct;

    for(uint8_t i = 0; i < 8; i++)
    {
        R[i] = TwoBytesToWord(&(hummingbird -> iv[(i & 0x03) << 1]));
        K[i] = TwoBytesToWord(&(hummingbird -> key[i << 1]));
    }

    initState(K, R);

    for(uint16_t i = 0; i < length; i += 2)
    {
        P = TwoBytesToWord(&plaintext[i]);
        Ct = encryptWord(P, K, R);

        wordToTwoBytes(Ct, &plaintext[i]);
    }

    if (length & 0x0001)
    {
        P = (uint16_t) plaintext[length - 1] << 8;
        Ct = encryptWord(P, K, R);
        wordToTwoBytes(Ct, &plaintext[length - 1]);
    }
}

void Hummingbird_decrypt(Hummingibird2Struct *hummingbird, uint8_t *ciphertext, uint16_t length)
{
    uint16_t K[8];
    uint16_t *R = hummingbird -> state;
    uint16_t P;
    uint16_t Ct;

    for(uint8_t i = 0; i < 8; i++)
    {
        R[i] = TwoBytesToWord(&(hummingbird -> iv[(i & 0x03) << 1]));
        K[i] = TwoBytesToWord(&(hummingbird -> key[i << 1]));
    }

    initState(K, R);
    for(uint16_t i = 0; i < length; i += 2)
    {
        Ct = TwoBytesToWord(&ciphertext[i]);
        P = decryptWord(Ct, K, R);

        wordToTwoBytes(P, &ciphertext[i]);
    }

    if (length & 0x0001)
    {
        Ct = (uint16_t) ciphertext[length - 1] << 8;
        P = decryptWord(Ct, K, R);
        wordToTwoBytes(P, &ciphertext[length - 1]);
    }
}

uint16_t TwoBytesToWord(uint8_t *x)
{
    return ((uint16_t)(x[0]) << 8) | x[1];
}

void wordToTwoBytes(uint16_t x, uint8_t *y)
{
    y[0] = x >> 8;
    y[1] = x & 0x00FF;
}

inline uint16_t lcs(uint16_t x, uint8_t s)
{
    return (x << s) | (x >> (16 - s));
}

inline uint16_t rcs(uint16_t x, uint8_t s)
{
    return (x >> s) | (x << (16 - s));
}

inline uint16_t addMod65536(uint16_t a, uint16_t b)
{
    return a + b;
}

inline uint16_t invAddMod65536(uint16_t a, uint16_t b)
{
    return a - b;
}