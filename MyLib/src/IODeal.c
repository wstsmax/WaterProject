#include "IODeal.h"
#include "Delay.h"

#include "STC8A8K32S4A12.h"

void Valve_Init(void)
{
    P2M1 = 0x00;
    P2M0 = 0x30;
}

void Valve_Open(void)
{

    P24 = 0;
    P25 = 1;
    Delay_MS(20);
    P25 = 0;
}

void Valve_Close(void)
{
    P24 = 1;
    P25 = 0;
    Delay_MS(20);
    P24 = 0;
}

