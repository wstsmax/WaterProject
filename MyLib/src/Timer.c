#include "STC8A8K32S4A12.h"

#include "Timer.h"
#include "Packet.h"
#include "Order.h"
#include "EEPROM.h"

void TIM0_Init(void) //10毫秒@24.000MHz
{
    TMOD = 0x04; //外部计数模式
    TL0 = 0xff;
    TH0 = 0xff;
    TR0 = 1; //启动定时器
    ET0 = 1; //使能定时器中断
    EA = 1;
}

void TIM2_Init(void) //1毫秒@24.000MHz
{
    AUXR |= 0x04;  //定时器时钟1T模式
    T2L = 0x40;    //设置定时初值
    T2H = 0xA2;    //设置定时初值
    AUXR &= ~0x10; //定时器2开始计时
    IE2 |= ET2;
    EA = 1;
}

void TIM3_Init(void) //1毫秒@24.000MHz
{
    T4T3M |= 0x02;  //定时器时钟1T模式
    T3L = 0x40;     //设置定时初值
    T3H = 0xA2;     //设置定时初值
    T4T3M &= ~0x08; //定时器3开始计时
    IE2 |= ET3;     //使能定时器中断
    EA = 1;
}

void TIM4_Init(void) //1毫秒@24.000MHz
{
    T4T3M |= 0x20; //定时器时钟12T模式
    T4L = 0x40;    //设置定时初值
    T4H = 0xA2;    //设置定时初值
    T4T3M |= 0x80; //定时器4开始计时
    IE2 |= ET4;
    EA = 1;
}

void TIM0_Isr() interrupt 1 using 1
{
    ValveCount++;
    ValveCountDown--;
    ValveAll++;
}

void TIM2_Isr() interrupt 12 using 1
{
    if (PollTimer_Start == 0)
    {
        TempTimer_Start = ValveCount;
    }
    PollTimer_Start++;
    if (PollTimer_Start == 2000)
    {
        PollTimer_Start = 0;
        if (ValveCount - TempTimer_Start >= 5)
        {
            //ValveCount = 0;
            StartBool = True;
            AUXR &= ~0x10; //用户开始用水定时器关闭
            T4T3M |= 0x08; //用户结束用水定时器开启
        }
    }
    AUXINTIF &= ~T2IF; //清中断标志
}

void TIM3_Isr() interrupt 19 using 1
{
    if (PollTimer_Stop == 0)
    {
        TempTimer_Stop = ValveCount;
    }

    PollTimer_Stop++;
    if (PollTimer_Start_Stop == 3000)
    {
        PollTimer_Stop = 0;
        PollTimer_Start_Stop = 0;
        StopBool = True;
        T4T3M &= ~0x08;
        if (IapRead(EEPROM_DeviceMode) == MODE_FLOW)
        {
            AUXR |= 0x10;
        }
    }
    if (PollTimer_Stop == 100)
    {
        PollTimer_Stop = 0;
        if (ValveCount - TempTimer_Stop <= 5)
        {
            PollTimer_Start_Stop = PollTimer_Start_Stop + 100;
        }
        else
        {
            PollTimer_Start_Stop = 0;
        }
        TempTimer_Stop = ValveCount;
    }
    AUXINTIF &= ~T3IF;
}

void TIM4_Isr() interrupt 20 using 1
{
    PollTimer++;
    if (PollTimer == 360000)
    {
        ICCIDBool = True;
        HeartPacketBool = True;
        PollTimer = 0;
    }
    AUXINTIF &= ~T4IF; //清中断标志
}
