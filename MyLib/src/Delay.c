#include "STC8A8K32S4A12.h"

#include "Delay.h"
#include "intrins.h"

void Delay_S(int S)
{
    unsigned char i, j, k;
    while (S--)
    {
        _nop_();
        _nop_();
        i = 122;
        j = 193;
        k = 128;
        do
        {
            do
            {
                while (--k)
                    ;
            } while (--j);
        } while (--i);
    }
}

void Delay_MS(int MS)
{
    unsigned char i, j;
    while (MS--)
    {
        _nop_();
        i = 32;
        j = 40;
        do
        {
            while (--j)
                ;
        } while (--i);
    }
}

// void Delay_US(int US)
// {
//     unsigned char i;
//     while (US--)
//     {
//         i = 6;
//         while (--i)
//             ;
//     }
// }
