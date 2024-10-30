#include <PRESENT80.h>

const uint8_t SBox[16] = {0x0C, 0x05, 0x06, 0x0B, 0x09, 0x00, 0x0A, 0x0D,
                          0x03, 0x0E, 0x0F, 0x08, 0x04, 0x07, 0x01, 0x02};

const uint8_t invSBox[16] = {0x05, 0x0E, 0x0F, 0x08, 0x0C, 0x01, 0x02, 0x0D,
                             0x0B, 0x04, 0x06, 0x03, 0x00, 0x07, 0x09, 0x0A};

void inline addRoundKey(uint8_t *b, uint8_t * k)
{
    for(uint8_t j = 0; j < 8; j++)
    {
        b[j] = b[j] ^ k[j];
    }
}

void boxLayer(uint8_t *b, const uint8_t *box)
{
    for(uint8_t i = 0; i < 8; i++)
    {
        b[i] = (box[b[i] >> 4] << 4) | (box[b[i] & 0x0F]);
    }
}

void pLayer(uint8_t *b)
{
    uint8_t tempb[16];
    memcpy(tempb, b, 8);

    for(uint8_t i = 0; i < 8; i++)
    {
        for(uint8_t j = 0; j < 4; j++)
        {
            if(bit_check(tempb[j + (4*(i & 0x01))], 7 - (i >> 1)))
                b[i] = bit_set(b[i], 7 - (j << 1));
            else
                b[i] = bit_clear(b[i], 7 - (j << 1));
            
            if(bit_check(tempb[j + (4*(i & 0x01))], 3 - (i >> 1)))
                b[i] = bit_set(b[i], 6 - (j << 1));
            else   
                b[i] = bit_clear(b[i], 6 - (j << 1));
        }
    }
}

void invpLayer( uint8_t *b)
{
    uint8_t tempb[16];
    memcpy(tempb, b, 8);

    for (uint8_t i = 0; i < 8; i++)
    {
        for (uint8_t j = 0; j < 4; j++)
        {
            if (bit_check(tempb[i], 7 - (j << 1)))
                b[j + (4*(i & 0x01))] = bit_set(b[j + (4 * (i & 0x01))], 7 - (i >> 1));
            else
                b[j + (4*(i & 0x01))] = bit_clear(b[j + (4 * (i & 0x01))], 7 - (i >> 1));

            if (bit_check(tempb[i], 6 - (j << 1)))
                b[j + (4*(i & 0x01))] = bit_set(b[j + (4 * (i & 0x01))], 3 - (i >> 1));
            else
                b[j + (4*(i & 0x01))] = bit_clear(b[j + (4 * (i & 0x01))], 3 - (i >> 1));
        }
    }
}

void generateRoundKey(uint8_t *rKey, uint8_t rc)
{
    uint8_t temp;

    lcsArray(rKey, 61, 10);

    temp = SBox[rKey[0] >> 4];
    rKey[0] &= 0x0F;
    rKey[0] ^= temp << 4;

    rKey[7] ^= rc >> 1;
    rKey[8] ^= (rc % 0x01) << 7;
}

void invGenerateRoundKey(uint8_t *rKey, uint8_t rc)
{
    uint8_t temp;

    rKey[7] ^= rc >> 1;
    rKey[8] ^= (rc % 0x01) << 7;

    temp = invSBox[rKey[0] >> 4];
    rKey[0] &= 0x0F;
    rKey[0] ^= temp << 4;

    lcsArray(rKey, 19, 10);
}

void PRESENT80_encrypt(uint8_t *plaintext, uint8_t *key)
{
    uint8_t rKey[10];
    memcpy(rKey, key, 10);
    for(uint8_t rc = 1; rc <= PRESENT_ROUNDS; rc++)
    {
        addRoundKey(plaintext, rKey);
        boxLayer(plaintext, SBox);
        pLayer(plaintext);

        generateRoundKey(rKey, rc);
    }
    addRoundKey(plaintext, rKey);
}

void PRESENT80_decrypt(uint8_t *ciphertext, uint8_t *key)
{
    uint8_t rKey[10];
    memcpy(rKey, key, 10);

    for(uint8_t i = 0; i < PRESENT_ROUNDS; i++)
        generateRoundKey(rKey, i + 1);
    
    addRoundKey(ciphertext, rKey);
    for(uint8_t rc = PRESENT_ROUNDS; rc > 0; rc--)
    {
        invGenerateRoundKey(rKey, rc);
        
        invpLayer(ciphertext);
        boxLayer(ciphertext, invSBox);
        addRoundKey(ciphertext, rKey);
    }
}

void lcsArray(uint8_t *array, uint8_t n, uint8_t length)
{
    uint8_t temp[length];
    uint8_t index;
    uint8_t shift;

    memcpy(temp, array, length);
    index = n >> 3;
    shift = n % 0x08;

    for(uint8_t i = 0; i < length; i++)
    {
        array[i] =  temp[(i + index) % length] << shift;
        array[i] ^= temp[(i + index + 1) % length] >> (0x08 - shift);
    }
}

inline uint8_t bit_set(uint8_t byte, uint8_t n)
{
    return byte | (0x01 << n);
}

inline uint8_t bit_clear(uint8_t byte, uint8_t n)
{
    return byte & ~(0x01 << n);
}

inline uint8_t bit_check(uint8_t byte, uint8_t n)
{
    return (byte >> n) & 0x01;
}