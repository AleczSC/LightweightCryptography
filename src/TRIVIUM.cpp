#include <TRIVIUM.h>

void initState(triviumStruct *trivium)
{
    uint32_t temp;
    
    /*Loading the 80-bit key and 80-bit iv into the state*/
    memset(trivium -> state, 0x00, 36);
    
    //Loading key
    memcpy(trivium -> state, trivium -> key, 10);

    //Loading iv
    for(uint8_t i = 0; i < 4; i++)
    {
        temp <<= 8;
        temp ^= trivium -> iv[i];
    }
    trivium -> state[2] ^= temp >> 29;
    trivium -> state[3] ^= temp << 3;

    for(uint8_t i = 4; i < 8; i++)
    {
        temp <<= 8;
        temp ^= trivium -> iv[i];
    }
    trivium -> state[3] ^= temp >> 29;
    trivium -> state[4] ^= temp << 3;

    trivium -> state[4] ^= (trivium -> iv[8]) >> 5;
    trivium -> state[5] ^= ((uint32_t) trivium -> iv[9]) << 27;

    //Setting the 3 LSB to one
    trivium -> state[8] ^= 0x00000007;

    for (uint8_t i = 0; i < 36; i++)
    {
        keyStreamGeneration_32(trivium -> state);
    }

}

uint32_t keyStreamGeneration_32(uint32_t *s)
{
    uint32_t t1, t2, t3;

    t1 = GET_PREVIOUS_32BITS(s, 2, 1) ^ GET_PREVIOUS_32BITS(s, 2, 28);
    t2 = GET_PREVIOUS_32BITS(s, 5, 1) ^ GET_PREVIOUS_32BITS(s, 5, 16);
    t3 = GET_PREVIOUS_32BITS(s, 7, 18) ^ s[8];

    t1 ^= GET_PREVIOUS_32BITS(s, 2, 26) & GET_PREVIOUS_32BITS(s, 2, 27) ^ GET_PREVIOUS_32BITS(s, 5, 10);
    t2 ^= GET_PREVIOUS_32BITS(s, 5, 14) & GET_PREVIOUS_32BITS(s, 5, 15) ^ GET_PREVIOUS_32BITS(s, 8, 7);
    t3 ^= GET_PREVIOUS_32BITS(s, 8, 29) & GET_PREVIOUS_32BITS(s, 8, 30) ^ GET_PREVIOUS_32BITS(s, 2, 4);

    for(uint8_t i = 8; i > 0; i--)
        s[i] = s[i - 1];

    s[0] = t3;

    s[3] &= 0x00000003;
    s[2] &= 0xFFFFFFFC;
    s[3] ^= t1 << 2;
    s[2] ^= t1 >> 30;

    s[6] &= 0x00003FFF; 
    s[5] &= 0xFFFFC000;
    s[6] ^= t2 << 14;
    s[5] ^= t2 >> 18;

    return t1 ^ t2 ^ t3;
}

uint8_t keyStreamGeneration_8(uint32_t *s)
{
    uint8_t t1, t2, t3;

    t1 = (s[2] >> 30) ^ (s[1] << 2) ^ (s[2] >> 3);
    t2 = (s[5] >> 30) ^ (s[4] << 2) ^ (s[5] >> 15);
    t3 = (s[7] >> 13) ^ s[8];

    t1 ^= (s[2] >> 5) & (s[2] >> 4) ^ (s[5] >> 21);
    t2 ^= (s[5] >> 17) & (s[2] >> 16) ^ (s[8] >> 24);
    t3 ^= (s[8] >> 2) & (s[8] >> 1) ^ ((s[2] >> 27) ^ (s[1] << 5));

    for(uint8_t i = 8; i > 0; i--)
    {
        s[i] >>= 8;
        s[i] ^= s[i - 1] << 24;
    }

    s[0] ^= ((uint32_t) t3) << 24;

    s[3] &= 0x03FFFFFF;
    s[2] &= 0xFFFFFFFC;
    s[3] ^= ((uint32_t) t1) << 26;
    s[2] ^= t1 >> 6;

    s[5] &= 0xFFFFC03F; 
    s[5] ^= ((uint16_t) t2) << 6;

    return t1 ^ t2 ^ t3;
}

void trivium_encrypt(triviumStruct *trivium, uint8_t *plaintext, uint16_t length)
{
    uint32_t KS;

    initState(trivium);

    while(length >= 4)
    {
        KS = keyStreamGeneration_32(trivium -> state);

        for(int8_t i = 3; i >= 0; i--)
        {
            plaintext[i] ^= KS;
            KS >>= 8;
        }
        
        plaintext += 4;
        length -= 4;
    }

    while(length)
    {
        KS = keyStreamGeneration_8(trivium -> state);
        *plaintext ^= KS;

        plaintext++;
        length--;
    }
}