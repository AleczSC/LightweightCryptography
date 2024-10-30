#include <piccolo80_v2.h>

const uint8_t SBox[16] = {0xE, 0x4, 0xB, 0x2, 0x3, 0x8, 0x0, 0x9,
                          0x1, 0xA, 0x7, 0xF, 0x6, 0xC, 0x5, 0xD};

const uint8_t M[4][4] = {
                   {2, 3, 1, 1}, 
                   {1, 2, 3, 1},
                   {1, 1, 2, 3},
                   {3, 1, 1, 2}
                  };

const uint32_t con80[ROUNDS_80] = 
{
  0x071c293d, 0x1f1a253e, 0x1718213f, 0x2f163d38, 0x27143939,
  0x3f12353a, 0x3710313b, 0x4f0e0d34, 0x470c0935, 0x5f0a0536,
  0x57080137, 0x6f061d30, 0x67041931, 0x7f021532, 0x77001133,
  0x8f3e6d2c, 0x873c692d, 0x9f3a652e, 0x9738612f, 0xaf367d28,
  0xa7347929, 0xbf32752a, 0xb730712b, 0xcf2e4d24, 0xc72c4925
};

void keySchedule(uint8_t *K, KeyStruct *KS)
{
    uint32_t rkTemp;
    uint32_t kTemp;

    //Cleaning all wk's
    for (int i = 0; i < 4; i++)
        KS -> wk[i] = 0;
    
    //Assigning all wKeys
    KS -> wk[0] = ((uint16_t) K[0]) << 8; 
    KS -> wk[0] ^= (uint16_t) K[3];
    KS -> wk[1] = ((uint16_t) K[2]) << 8;
    KS -> wk[1] ^= (uint16_t) K[1];
    KS -> wk[2] = ((uint16_t) K[8]) << 8;
    KS -> wk[2] ^= (uint16_t) K[7];
    KS -> wk[3] = ((uint16_t) K[6]) << 8;
    KS -> wk[3] ^= (uint16_t) K[9];

    //now for the rKeys
    for (uint8_t i = 0; i < ROUNDS_80; i++)
    {
        if ((i % 5 == 0) || (i % 5) == 2)
        {
            kTemp = K[4]; kTemp <<=  8;
            kTemp ^= K[5]; kTemp <<= 8;
            kTemp ^= K[6]; kTemp <<= 8;
            kTemp ^= K[7];
        }

        else if ((i % 5 == 1) || (i % 5 == 4))
        {
            kTemp = K[0]; kTemp <<=  8;
            kTemp ^= K[1]; kTemp <<= 8;
            kTemp ^= K[2]; kTemp <<= 8;
            kTemp ^= K[3];   
        }

        else
        {
            kTemp = K[8]; kTemp <<=  8;
            kTemp ^= K[9]; kTemp <<= 8;
            kTemp ^= K[8]; kTemp <<= 8;
            kTemp ^= K[9];
        }

        rkTemp = con80[i] ^ kTemp;
        KS -> rk[i << 1] = rkTemp >> 16;
        KS -> rk[(i << 1) + 1] = rkTemp & 0x0000FFFF;
    }
}

void invKeySchedule(uint8_t *K, KeyStruct *KS)
{
    KeyStruct KSTemp;
    keySchedule(K, KS);

    KSTemp = *KS;

    KS -> wk[0] = KSTemp.wk[2];
    KS -> wk[1] = KSTemp.wk[3];
    KS -> wk[2] = KSTemp.wk[0];
    KS -> wk[3] = KSTemp.wk[1];

    for (uint8_t i = 0; i < ROUNDS_80; i++)
    {
        if ((i % 2) == 0)
        {
            KS -> rk[i << 1] = KSTemp.rk[(ROUNDS_80 << 1) - (i << 1) - 2];
            KS -> rk[(i << 1) + 1] = KSTemp.rk[(ROUNDS_80 << 1) - (i << 1) - 1];
        }

        else
        {
            KS -> rk[i << 1] = KSTemp.rk[(ROUNDS_80 << 1) - (i << 1) - 1];
            KS -> rk[(i << 1) + 1] = KSTemp.rk[(ROUNDS_80 << 1) - (i << 1) - 2];
        }
    }


}

uint8_t gm(uint8_t a, uint8_t b) 
{
    /* galois multiplication in F_16 */
    uint8_t g = 0;
    int i;
    for (i = 0; i < DEG_GF_POLY; i++) 
    {
        if ( (b & 0x1) == 1 ) { g ^= a; }
        uint8_t hbs = (a & 0x8);
        a <<= 0x1;
        if ( hbs == 0x8) { a ^= GF_POLY; }
        b >>= 0x1;
    }

    return g;
}

uint16_t F_function(uint16_t xData)
{
    uint8_t x[4];
    uint8_t y[4];

    for (uint8_t i = 3; i <= 0; i--)
    {
        x[i] = xData & 0x0F;
        x[i] = SBox[x[i]];
        xData >>= 4;
    }

    /* multiply matrix and column */
    y[0] = (gm(x[0],M[0][0]) ^ gm(x[1],M[0][1]) ^ gm(x[2],M[0][2]) ^ gm(x[3],M[0][3]));
    y[1] = (gm(x[0],M[1][0]) ^ gm(x[1],M[1][1]) ^ gm(x[2],M[1][2]) ^ gm(x[3],M[1][3]));
    y[2] = (gm(x[0],M[2][0]) ^ gm(x[1],M[2][1]) ^ gm(x[2],M[2][2]) ^ gm(x[3],M[2][3]));
    y[3] = (gm(x[0],M[3][0]) ^ gm(x[1],M[3][1]) ^ gm(x[2],M[3][2]) ^ gm(x[3],M[3][3]));

    xData = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        y[i] = SBox[y[i]];
        xData ^= y[i] & 0x0F;
        xData <<= 4;

        if (i == 2) break;

        
    }

    return xData;
}

void RP(uint8_t *data)
{
    uint8_t tempData[8];

    for (uint8_t i = 0; i < 8; i++)
        tempData[i] = data[i];
    
    data[0] = tempData[2];
    data[1] = tempData[7];
    data[2] = tempData[4];
    data[3] = tempData[1];
    data[4] = tempData[6];
    data[5] = tempData[3];
    data[6] = tempData[0];
    data[7] = tempData[5];
}

void RP16(uint16_t *data)
{
    uint16_t tempData[4];

    for (uint8_t i = 0; i < 4; i++)
        tempData[i] = data[i];

    data[0] =  tempData[1] & 0xFF00; //x0 = x2
    data[0] ^= tempData[3] & 0x00FF; //x1 = x7
    data[1] =  tempData[2] & 0xFF00; //x2 = x4
    data[1] ^= tempData[0] & 0x00FF; //x3 = x4
    data[2] =  tempData[3] & 0xFF00; //x4 = x6
    data[2] ^= tempData[1] & 0x00FF; //x5 = x3
    data[3] =  tempData[0] & 0xFF00; //x6 = x0
    data[3] ^= tempData[2] & 0x00FF; //x7 = x5

}

void encrypt(uint8_t *plaintext, KeyStruct *KS)
{
    uint16_t X[4];

    data8to16(plaintext, X);

    X[0] ^= KS -> wk[0];
    X[2] ^= KS -> wk[1];

    for (uint8_t i = 0; i < (ROUNDS_80 - 1); i++)
    {
        X[1] = X[1] ^ F_function(X[0]) ^ KS -> rk[i << 1];
        X[3] = X[3] ^ F_function(X[2]) ^ KS -> rk[(i << 1) + 1];

        RP16(X);
    }

    X[1] = X[1] ^ F_function(X[0]) ^ KS -> rk[(ROUNDS_80 << 1) - 2];
    X[3] = X[3] ^ F_function(X[2]) ^ KS -> rk[(ROUNDS_80 << 1) - 1];
    X[0] ^= KS -> wk[2];
    X[2] ^= KS -> wk[3];

    data16to8(X, plaintext);


}

void data8to16(uint8_t *data, uint16_t *X)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        X[i] = ((uint16_t) data[i << 1]) << 8;
        X[i] ^= (uint16_t) data[(i << 1) + 1];
    }
}

void data16to8(uint16_t *X, uint8_t *data)
{
    for (uint8_t i = 0; i < 4; i++)
    {
        data[i << 1] = (X[i] & 0xFF00) >> 8;
        data[(i << 1) + 1] = X[i] & 0x00FF;
    }

}

