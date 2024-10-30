#include <Fruit-80.h>

uint8_t rIndex(uint8_t Cr)
{
    return Cr >> 3;
}

uint8_t pIndex(uint8_t Cr)
{
    return (Cr >> 1) & 0x1F;
}

uint8_t qIndex(uint8_t Cr)
{
    return Cr & 0x1F;
}

uint8_t roundKeyFunction(uint8_t *k, uint8_t Cr)
{
    uint8_t r, p, q;
    uint8_t kt;

    r = rIndex(Cr);
    p = pIndex(Cr) + 16;
    q = qIndex(Cr) + 48;

    kt = (GET_BIT(k, r) & GET_BIT(k, p) & GET_BIT(k, q)) ^ (GET_BIT(k, r) & GET_BIT(k, p)) ^ (GET_BIT(k, p) & GET_BIT(k, q)) \
       ^ (GET_BIT(k, r) & GET_BIT(k, q)) ^ GET_BIT(k, p);
    kt = kt >> 7;

    kt ^= (GET_BIT(k, r) & GET_BIT(k, p)) ^ (GET_BIT(k, p) & GET_BIT(k, q)) ^ (GET_BIT(k, r) & GET_BIT(k, q)) ^ GET_BIT(k, r) ^ GET_BIT(k, p) ^ GET_BIT(k, p);

    return kt & 0x81;
}

uint8_t gFunction(uint8_t kt, uint8_t *l, uint8_t *n, uint8_t Cr)
{
    uint8_t n37;

    n37 = (kt << 7) ^ l[0] ^ n[0] ^ (n[1] << 2) ^ (n[2] << 4) ^ ((n[1] << 4) & (n[0] << 3)) \
        ^ ((n[1] << 6) & (n[3] << 1)) ^ ((n[0] << 5) & (n[2] << 7) & (n[3] << 7)) \
        ^ (n[1] & (n[2] << 2)) ^ ((n[3] << 4) & (n[3] << 6) & n[4] & (n[4] << 2));

    return n37 & 0x80;
}

uint8_t fFunction(uint8_t *l)
{
    uint8_t l43;

    l43 = l[0] ^ l[1] ^ (l[2] << 2) ^ (l[2] << 7) ^ (l[3] << 4) ^ (l[4] << 5);

    return l43 & 0x80;
}

uint8_t hFunction(uint8_t kt, uint8_t *l, uint8_t *n)
{
    uint8_t h;

    h = (kt & ((n[4] << 4) ^ (l[2] << 3))) ^ ((l[0] << 6) & (l[1] << 7)) ^ ((l[0] << 1) & (l[2] << 6)) \
      ^ ((n[4] << 3) & (l[3] << 3)) ^((n[0] << 1) & n[3]) ^ ((n[0] << 1) & (n[4] << 1) & (l[5] << 2));

    return h & 0x80;
}

uint8_t outputFunction(uint8_t h, uint8_t *n, uint8_t *l)
{
    uint8_t z;

    z = h ^ n[0] ^ (n[0] << 7) ^ (n[2] << 3) ^ (n[3] << 5) ^ (n[4] << 4) ^ (l[4] << 4);

    return z & 0x80;
}

void initCipher(fruitv2Struct *fruit)
{
    uint8_t kt, n37, l43, h, z;
    (*fruit).Cr = 0x00;
    //Extend the iv bits
    uint8_t iv[10] = {0x00};
    

    for(uint8_t i = 0; i < 9; i++)
    {
        iv[i + 1] ^= fruit -> iv[i] >> 2;
        if(i != 8)
            iv[i + 2] ^= fruit -> iv[i] << 6;
    }
    iv[0] |= 0x80;

    //Load the key into the NFSR and LFSR
    for (uint8_t i = 0; i < 5; i++)
        fruit -> NFSR[i] = fruit -> key[i];
    fruit -> NFSR[4] &= 0xF8;

    memset(fruit -> LFSR, 0, 6);
    for (uint8_t i = 0; i < 6 ; i++)
    {
        fruit -> LFSR[i] ^= fruit -> key[4 + i] << 5;
        if (i != 5)
            fruit -> LFSR[i] ^= fruit -> key[5 + i] >> 3;
    }

    //Clock the cipher 80 times and use the iv and z as feedback
    for(uint8_t i = 0; i < 80; i++)
    {
        kt = roundKeyFunction(fruit -> key, (*fruit).Cr);
        l43 = fFunction(fruit -> LFSR);
        h = hFunction(kt, fruit -> LFSR, fruit -> NFSR);
        z = outputFunction(h, fruit -> NFSR, fruit -> LFSR);
        n37 = gFunction((kt << 7) ^ GET_BIT(iv, i) ^ z, fruit -> LFSR, fruit -> NFSR, (*fruit).Cr);

        updateCipher(fruit, n37, l43 ^ z ^ GET_BIT(iv, i));
    }

    fruit -> Cr = fruit -> NFSR[0] >> 1;
    fruit -> Cr &= 0xFE;
    fruit -> Cr ^= fruit -> LFSR[0] >> 7;
    fruit -> LFSR[0] |= 0x80;

    for (uint8_t i = 0; i < 80; i++)
    {
        kt = roundKeyFunction(fruit -> key, (*fruit).Cr);
        l43 = fFunction(fruit -> LFSR);
        h = hFunction(kt, fruit -> LFSR, fruit -> NFSR);
        z = outputFunction(h, fruit -> NFSR, fruit -> LFSR);
        n37 = gFunction(kt, fruit -> LFSR, fruit -> NFSR, (*fruit).Cr);

        updateCipher(fruit, n37, l43);
    }

}

void updateCipher(fruitv2Struct *fruit, uint8_t n37, uint8_t l43)
{
    arrayLeftShift(fruit -> NFSR, 1, 5);
    fruit -> NFSR[4] &= 0xF0;
    arrayLeftShift(fruit -> LFSR, 1, 6);
    fruit -> LFSR[5] &= 0xC0;

    fruit -> NFSR[4] ^= n37 >> 4;
    fruit -> LFSR[5] ^= l43 >> 2;

    fruit -> Cr++;
    if (fruit -> Cr == 0x80)
        fruit -> Cr = 0x00;
}

uint8_t generateKeyStreamByte(fruitv2Struct *fruit)
{
    uint8_t kt, n37, l43, h, z;
    uint8_t KS;

    KS = 0x00;
    for(uint8_t i = 0; i < 8; i++)
    {
        kt = roundKeyFunction(fruit -> key, fruit -> Cr);
        n37 = gFunction(kt, fruit -> LFSR, fruit -> NFSR, fruit -> Cr);
        l43 = fFunction(fruit -> LFSR);
        h = hFunction(kt, fruit -> LFSR, fruit -> NFSR);
        z = outputFunction(h, fruit -> NFSR, fruit -> LFSR);

        KS ^= z >> i;
        updateCipher(fruit, n37, l43);
    }

    return KS;
}

void fruit80_encrypt(fruitv2Struct *fruit, uint8_t *plaintext, uint16_t length)
{
    uint8_t KS;
    initCipher(fruit);

    for(uint16_t i = 0; i < length; i++)
    {
        KS = generateKeyStreamByte(fruit);
        plaintext[i] ^= KS;
    }
}

void arrayLeftShift(uint8_t *array, uint8_t shift, uint8_t length)
{
    for(uint8_t i = 0; i < length; i++)
    {
        array[i] = array[i] << shift;
        if (i != length - 1)
            array[i] ^= array[i + 1] >> (8 - shift);
    }
}