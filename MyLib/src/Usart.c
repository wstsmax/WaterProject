#include "STC8A8K32S4A12.h"

#include "Usart.h"
#include "Gprs.h"
#include "Packet.h"
#include "Order.h"
#include "EEPROM.h"
#include "PublicFunction.h"

#include "string.h"

#define FOSC 24000000UL
#define BRT (65536 - FOSC / 115200 / 4)

void Uart1_Init(void)
{
    SCON = 0x50;    //8位数据,可变波特率
    AUXR = 0x40;    //定时器1时钟为Fosc,即1T
    AUXR &= 0xFE;   //串口1选择定时器1为波特率发生器
    TMOD = 0x00;    //设定定时器1为16位自动重装方式
    TL1 = BRT;      //设定定时初值
    TH1 = BRT >> 8; //设定定时初值
    ET1 = 0;        //禁止定时器1中断
    TR1 = 1;        //启动定时器1
    //BUSY = 0;
    //P3M1 = 0x00;
    //P3M0 = 0x03;
    EA = 1;
    ES = 1;
}

void Uart1_Send(unsigned char Buf)
{
    SBUF = Buf;
    while (!TI)
        ;
    TI = 0;
}

void Uart1_Send_Data(unsigned char *Buf)
{
    while (*Buf)
    {
        Uart1_Send(*Buf++);
    }
}

int Uart1_Recv_NUM_AT(void)
{
    char xdata *RecStr = Rx_Buffer_AT;
    char xdata Num = 0;
    int xdata Count = 0;
Loop:
    *RecStr = SBUF;
    Count = 0;
    RI = 0;
    if (Num < 50) //数据包长度为36个字符,尝试连续接收36个字符
    {
        Num++;
        RecStr++;
        while (!RI)
        {
            P50 = 0;
            Count++;
            if (Count > 200)
            {
                P50 = 1;
                return 0; //接收数据等待延迟，等待时间太久会导致CPU运算闲置，太短会出现"数据包被分割",默认count=130
            }
        }
        goto Loop;
    }
    P50 = 1;
    return 1;
}

int Uart1_Recv_NUM_Packet(void)
{
    char xdata *RecStr = Rx_Buffer;
    char xdata Num = 0;
    int xdata Count = 0;
Loop:
    *RecStr = SBUF;
    Count = 0;
    RI = 0;
    if (Num < 35) //数据包长度为36个字符,尝试连续接收36个字符
    {
        Num++;
        RecStr++;
        while (!RI)
        {
            P51 = 0;
            Count++;
            if (Count > 200)
            {
                P51 = 1;
                //Send_OK_Flag = False;
                return 0; //接收数据等待延迟，等待时间太久会导致CPU运算闲置，太短会出现"数据包被分割",默认count=130
            }
        }
        goto Loop;
    }
    P51 = 1;
    //Send_OK_Flag = False;
    return 1;
}

void Uart1_Recv_Deal(void)
{
    int xdata Check;
    int xdata HCheck;
    int xdata LCheck;
    //提取数据包中的数据，并存储到本地变量中
    DeviceID_01 = Rx_Buffer[0];
    DeviceID_02 = Rx_Buffer[1];
    DeviceID_03 = Rx_Buffer[2];
    DeviceID_04 = Rx_Buffer[3];
    DeviceMode = Rx_Buffer[4];
    Order = Rx_Buffer[5];
    DeviceStatus = Rx_Buffer[6];
    DeviceFlow_01 = Rx_Buffer[7];
    DeviceFlow_02 = Rx_Buffer[8];
    RechargeFlow_01 = Rx_Buffer[9];
    RechargeFlow_02 = Rx_Buffer[10];
    RechargeDay_01 = Rx_Buffer[11];
    RechargeDay_02 = Rx_Buffer[12];
    SurplusFlow_01 = Rx_Buffer[13];
    SurplusFlow_02 = Rx_Buffer[14];
    SurplusDay_01 = Rx_Buffer[15];
    SurplusDay_02 = Rx_Buffer[16];
    UsedFlow_01 = Rx_Buffer[17];
    UsedFlow_02 = Rx_Buffer[18];
    UsedDay_01 = Rx_Buffer[19];
    UsedDay_02 = Rx_Buffer[20];
    WaterTDS_01 = Rx_Buffer[21];
    WaterTDS_02 = Rx_Buffer[22];
    GPRSSignal = Rx_Buffer[23];
    ICCID1_01 = Rx_Buffer[24];
    ICCID1_02 = Rx_Buffer[25];
    ICCID2_01 = Rx_Buffer[26];
    ICCID2_02 = Rx_Buffer[27];
    ICCID3_01 = Rx_Buffer[28];
    ICCID3_02 = Rx_Buffer[29];
    ICCID4_01 = Rx_Buffer[30];
    ICCID4_02 = Rx_Buffer[31];
    ICCID5_01 = Rx_Buffer[32];
    ICCID5_02 = Rx_Buffer[33];
    Check_01 = Rx_Buffer[34];
    Check_02 = Rx_Buffer[35];
    //计算校验位
    Clear_Buf(Rx_Buffer);
    Check = DeviceID_01 + DeviceID_02 + DeviceID_03 + DeviceID_04 + DeviceMode + Order + DeviceStatus + DeviceFlow_01 + DeviceFlow_02 + RechargeFlow_01 + RechargeFlow_02 + RechargeDay_01 + RechargeDay_02 + SurplusFlow_01 + SurplusFlow_02 + SurplusDay_01 + SurplusDay_02 + UsedFlow_01 + UsedFlow_02 + UsedDay_01 + UsedDay_02 + WaterTDS_01 + WaterTDS_02 + GPRSSignal + ICCID1_01 + ICCID1_02 + ICCID2_01 + ICCID2_02 + ICCID3_01 + ICCID3_02 + ICCID4_01 + ICCID4_02 + ICCID5_01 + ICCID5_02;
    HCheck = (short)(Check >> 8);   //提取校验位高八位
    LCheck = (short)(Check & 0xff); //提取校验位低八位

    if ((Check_01 != HCheck) && (Check_02 != LCheck)) //比较计算的校验位是否与数据包中校验位相等
    {
        Order = COMMAND_ERRORUPLOAD;
    }
}

void Clear_Buf(char *Rx_Buf)
{
    memset(Rx_Buf, 0x00, MAX_SIZE);
}

void Uart1_Isr(void) interrupt 4 using 1
{
    //ES = 0;
    if (Send_OK_Flag == True)
    {
        if (Uart1_Recv_NUM_Packet())
        {
            //数据包长度正确则执行以下代码
            Uart1_Recv_Deal();
        }
    }
    else
    {
        Uart1_Recv_NUM_AT();
    }
    //RI = 0; //接收并处理一次数据后把接收中断标志清除一下，拒绝响应在中断接收忙的时候发来的请求
    //ES = 1;
}
