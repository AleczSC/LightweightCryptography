#include <CRAFT.h>

const uint8_t Sbox[] = {0xC, 0xA, 0xD, 0x3, 0xE, 0xB, 0xF, 0x7,
                        0x8, 0x9, 0x1, 0x5, 0x0, 0x2, 0x4, 0x6};

const uint8_t Pbox[] = {15, 12, 13, 14, 10, 9, 8, 11,
                         6,  5,  4,  7,  1, 2, 3,  0};



const uint8_t RC[] = {0x11, 0x84, 0x42, 0x25, 0x96, 0xC7, 0x63, 0xB1,
                      0x54, 0xA2, 0xD5, 0xE6, 0xF7, 0x73, 0x31, 0x14,
                      0x82, 0x45, 0x26, 0x97, 0xC3, 0x61, 0xB4, 0x52,
                      0xA5, 0xD6, 0xE7, 0xF3, 0x71, 0x34, 0x12, 0x85};

const uint8_t Qbox[] = {12, 10, 15, 5, 14, 8, 9,  2,
                        11,  3,  7, 4,  6, 0, 1, 13};


void CRAFT_encrypt(uint8_t *plaintext, uint8_t *K, uint8_t *T)
{
    uint8_t QT[8];
    uint8_t TK[4][8];
    uint8_t *Y;
    //TweakeysStruct tweakeys;

    //Copy all the T vector into QT vector because... reasons.
    for(uint8_t i = 0; i < 8; i++)
        QT[i] = T[i];

    //QPermutation
    permutation(QT, Qbox);

    //Compute all Tweakeys
    for (uint8_t i = 0; i < 4; i++)
    {
        TK[0][i] = K[i] ^ T[i];
        TK[1][i] = K[8 + i] ^ T[i];
        TK[2][i] = K[i] ^ QT[i];
        TK[3][i] = K[8 + i] ^ QT[i];
    }

    //Assign to Y the address of plaintext 
    Y = plaintext;

    //Start the rounds
    for(uint8_t i = 0; i <= TOTAL_ROUNDS; i++)
    {
        mixColumns(Y);
        Y[2] ^= RC[i];

        //AddTweakey   
        for(uint8_t j = 0; j < 8; j++)
            Y[j] ^= TK[i % 4][j];
        
        if(i != TOTAL_ROUNDS)
        {
            //PermuteNibbles
            permutation(Y, Pbox);
            //SubBox
            subBoxToAllVector(Y);
        }
    }
}

void CRAFT_decrypt(uint8_t *ciphertext, uint8_t *K, uint8_t *T)
{
    uint8_t QT[8];
    uint8_t TK[4][8];
    uint8_t *Y;

    //Copy all the T vector into QT vector because... reasons.
    for(uint8_t i = 0; i < 8; i++)
        QT[i] = T[i];

    //QPermutation
    permutation(QT, Qbox);

    //Compute all Tweakeys for decryption
    for (int i = 0; i < 4; i++)
    {
        TK[0][i] = K[i] ^ T[i]; 
        TK[1][i] = K[8 + i] ^ T[i]; 
        TK[2][i] = K[i] ^ QT[i];    
        TK[3][i] = K[8 + i] ^ QT[i];
    }
    mixColumns(TK[0]);
    mixColumns(TK[1]);
    mixColumns(TK[2]);
    mixColumns(TK[3]);

    Y = ciphertext;
    for (int8_t i = TOTAL_ROUNDS; i >= 0; i--)
    {
        mixColumns(Y);
        Y[2] ^= RC[i];

        // AddTweakey
        for (uint8_t j = 0; j < 8; j++)
            Y[j] ^= TK[i % 4][j];

        if (i != 0)
        {
            // PermuteNibbles
            permutation(Y, Pbox);
            // SubBox
            subBoxToAllVector(Y);
        }
    }
}

void permutation(uint8_t *vector, const uint8_t *box)
{
    //Split the vector into nibbles
    uint8_t Vnibble[16];
    uint8_t tempNibble[16];

     for(uint8_t i = 0; i < 8; i++)
    {
        Vnibble[i << 1] = vector[i] >> 4;
        Vnibble[(i << 1) + 1] = vector[i] & 0x0F;
    }

    for(uint8_t i = 0; i < 16; i++)
        tempNibble[i] = Vnibble[box[i]];

    //concatenate the nibbles into bytes
    for(uint8_t i = 0; i < 8; i++)
    {
        vector[i] = tempNibble[i << 1] << 4;
        vector[i] ^= tempNibble[(i << 1) + 1] & 0x0F;
    }
}

void subBoxToAllVector(uint8_t *vector)
{
    //Split the vector into nibbles
    uint8_t Vnibble[16];
    uint8_t tempNibble[16];

     for(uint8_t i = 0; i < 8; i++)
    {
        Vnibble[i << 1] = vector[i] >> 4;
        Vnibble[(i << 1) + 1] = vector[i] & 0x0F;
    }

    for(uint8_t i = 0; i < 16; i++)
        tempNibble[i] = Sbox[Vnibble[i]];

    //concatenate the nibbles into bytes
    for(uint8_t i = 0; i < 8; i++)
    {
        vector[i] = tempNibble[i << 1] << 4;
        vector[i] ^= tempNibble[(i << 1) + 1] & 0x0F;
    }
}

/*
void QPermutation(uint8_t *T, uint8_t *QT)
{
    //Split the T and QT vector into nibbles
    uint8_t Tnibble[16];
    uint8_t QTnibble[16];

    for(uint8_t i = 0; i < 8; i++)
    {
        Tnibble[i << 1] = Tnibble[i] >> 4;
        Tnibble[(i << 1) + 1] = Tnibble[i] & 0x0F;
    }

    for(uint8_t i = 0; i < 16; i++)
        QTnibble[i] = Tnibble[Qbox[i]];
    
    //concatenate the nibbles of QT into bytes
    for(uint8_t i = 0; i < 8; i++)
    {
        QT[i] = QTnibble[i << 1] << 4;
        QT[i] ^= QTnibble[(i << 1) + 1] && 0x0F;
    }
}
*/
    
void mixColumns(uint8_t *I)
{
    I[0] ^= I[4] ^ I[6];
    I[1] ^= I[5] ^ I[7];
    I[2] ^= I[6];
    I[3] ^= I[7];
}