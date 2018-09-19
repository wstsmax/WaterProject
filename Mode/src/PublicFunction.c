#include "STC8A8K32S4A12.h"

#include "PublicFunction.h"
#include "PublicMode.h"
#include "PrivateMode.h"
#include "Packet.h"
#include "Order.h"
#include "Usart.h"
#include "Gprs.h"
#include "IODeal.h"
#include "Delay.h"
#include "EEPROM.h"

#include "string.h"
#include "stdlib.h"
#include "ctype.h"

void Valve_Switch(int Switch)
{
    if (Switch == ON)
    {
        Valve_Open();
    }
    if (Switch == OFF)
    {
        Valve_Close();
    }
}

int Get_Hvalue(int Check)
{
    int xdata Hvalue = 0;
    Hvalue = (short)(Check >> 8);
    return Hvalue;
}

int Get_Lvalue(int Check)
{
    int xdata Lvalue = 0;
    Lvalue = (short)(Check & 0xff);
    return Lvalue;
}

int Get_Long_H(unsigned long Check)
{
    int xdata Hvalue = 0;
    Hvalue = (short)(Check >> 24);
    return Hvalue;
}

int Get_Long_M1(unsigned long Check)
{
    int xdata Lvalue = 0;
    Lvalue = (short)(Check >> 16);
    return Lvalue;
}
int Get_Long_M2(unsigned long Check)
{
    int xdata Hvalue = 0;
    Hvalue = (short)(Check >> 8);
    return Hvalue;
}

int Get_Long_L(unsigned long Check)
{
    int xdata Lvalue = 0;
    Lvalue = (short)(Check & 0xff);
    return Lvalue;
}

unsigned int Get_HLvalue(unsigned char Hvalue, unsigned char Lvalue)
{
    unsigned int xdata HLvalue = 0;
    HLvalue = (Hvalue << 8) + Lvalue;
    return HLvalue;
}

unsigned long Get_HMLvalue(int Hvalue, int M1value, int M2value, int Lvalue)
{
    unsigned long xdata HMLvalue = 0;
    HMLvalue = (unsigned long)(Hvalue << 24) + (M1value << 16) + (M2value << 8) + Lvalue;
    return HMLvalue;
}

void Get_Check(int *HCheck, int *LCheck)
{
    int xdata Check;
    Check = DeviceID_01 + DeviceID_02 + DeviceID_03 + DeviceID_04 + DeviceMode + Order + DeviceStatus + DeviceFlow_01 + DeviceFlow_02 + RechargeFlow_01 + RechargeFlow_02 + RechargeDay_01 + RechargeDay_02 + SurplusFlow_01 + SurplusFlow_02 + SurplusDay_01 + SurplusDay_02 + UsedFlow_01 + UsedFlow_02 + UsedDay_01 + UsedDay_02 + WaterTDS_01 + WaterTDS_02 + GPRSSignal + ICCID1_01 + ICCID1_02 + ICCID2_01 + ICCID2_02 + ICCID3_01 + ICCID3_02 + ICCID4_01 + ICCID4_02 + ICCID5_01 + ICCID5_02;
    *HCheck = Get_Hvalue(Check);
    *LCheck = Get_Lvalue(Check);
}

int Char_to_Int(char ch)
{
    if (isdigit(ch))
        return ch - 48;
    if (ch < 'A' || (ch > 'F' && ch < 'a') || ch > 'z')
        return -1;
    if (isalpha(ch))
        return isupper(ch) ? ch - 55 : ch - 87;

    return -1;
}

int Hex_to_Int(char *hex)
{
    int xdata len;
    int xdata num = 0;
    int xdata temp;
    int xdata bits;
    int xdata i;
    len = strlen(hex);

    for (i = 0, temp = 0; i < len; i++, temp = 0)
    {
        temp = Char_to_Int(*(hex + i));
        bits = (len - i - 1) * 4;
        temp = temp << bits;
        num = num | temp;
    }
    return num;
}

unsigned long Flow_to_Valve_FlowMode(void)
{
    unsigned int xdata HL = 0;
    unsigned long xdata HLC = 0;
    int i;
    HL = Get_HLvalue(RechargeFlow_01, RechargeFlow_02);
    for (i = 0; i < HL; i++)
    {
        HLC = HLC + Valve1L;
    }
    return HLC;
}

unsigned long Flow_to_Valve_PublicMode(void)
{
    unsigned int xdata HL = 0;
    unsigned long xdata HLC = 0;
    int i;
    HL = Get_HLvalue(DeviceFlow_01, DeviceFlow_02);
    for (i = 0; i < HL; i++)
    {
        HLC = HLC + Valve001L;
    }
    return HLC;
}

void Valve_to_Flow(void)
{
    unsigned int xdata UseFlow;
    unsigned int xdata SurplusFlow;
    unsigned int xdata UsedFlow;
    UseFlow = ValveCount / Valve001L;
    if (DeviceMode == MODE_PUBLIC)
        SurplusFlow = ValveCountDown / Valve001L;
    else if (DeviceMode == MODE_FLOW)
        SurplusFlow = ValveCountDown / Valve1L;
    UsedFlow = ValveAll / Valve1L;
    DeviceFlow_01 = (unsigned short)Get_Hvalue(UseFlow);
    DeviceFlow_02 = (unsigned short)Get_Lvalue(UseFlow);
    SurplusFlow_01 = (unsigned short)Get_Hvalue(SurplusFlow);
    SurplusFlow_02 = (unsigned short)Get_Lvalue(SurplusFlow);
    UsedFlow_01 = (unsigned short)Get_Hvalue(UsedFlow);
    UsedFlow_02 = (unsigned short)Get_Lvalue(UsedFlow);
}

void Recovery_PacketData(void)
{
    IapErase(EEPROM_DeviceID);
    IapProgram(EEPROM_DeviceID, 0x00);
    IapProgram(EEPROM_DeviceID + 1, 0x00);
    IapProgram(EEPROM_DeviceID + 2, 0x00);
    IapProgram(EEPROM_DeviceID + 3, 0x00);

    IapErase(EEPROM_DeviceMode);
    IapProgram(EEPROM_DeviceMode, MODE_PUBLIC);
    DeviceMode = MODE_PUBLIC;

    Order = COMMAND_HEARTBEATPACKET; //命令

    IapErase(EEPROM_DeviceStatus);
    IapProgram(EEPROM_DeviceStatus, STATUS_RECOVERY);
    DeviceStatus = STATUS_RECOVERY;

    DeviceFlow_01 = 0x00;   //本次消费流量_01	        [08]
    DeviceFlow_02 = 0x00;   //本次消费流量_02	        [09]
    RechargeFlow_01 = 0x00; //充值流量_01			    [10]
    RechargeFlow_02 = 0x00; //充值流量_02			    [11]
    RechargeDay_01 = 0x00;  //充值天数_01			    [12]
    RechargeDay_02 = 0x00;  //充值天数_02			    [13]

    SurplusFlow_01 = 0x00; //剩余流量_01			    [14]
    SurplusFlow_02 = 0x00; //剩余流量_02			    [15]

    SurplusDay_01 = 0x00; //剩余天数_01			    [16]
    SurplusDay_02 = 0x00; //剩余天数_02			    [17]

    UsedFlow_01 = 0x00; //已使用流量_01		    [18]
    UsedFlow_02 = 0x00; //已使用流量_02		    [19]

    UsedDay_01 = 0x00; //已使用天数_01		    [20]
    UsedDay_02 = 0x00; //已使用天数_02		    [21]

    WaterTDS_01 = 0x00; //纯水TDS值_01			[22]
    WaterTDS_02 = 0x00; //纯水TDS值_02			[23]

    IapErase(EEPROM_Vavle);
    IapProgram(EEPROM_Vavle, 0x00);
    IapProgram(EEPROM_Vavle + 1, 0x00);
    IapProgram(EEPROM_Vavle + 2, 0x00);
    IapProgram(EEPROM_Vavle + 3, 0x00);

    IapErase(EEPROM_VavleALL);
    IapProgram(EEPROM_VavleALL, 0x00);
    IapProgram(EEPROM_VavleALL + 1, 0x00);
    IapProgram(EEPROM_VavleALL + 2, 0x00);
    IapProgram(EEPROM_VavleALL + 3, 0x00);
}

void SystemInit_PacketData(void)
{
    Order = COMMAND_HEARTBEATPACKET; //命令
    IapErase(EEPROM_DeviceMode);
    IapProgram(EEPROM_DeviceMode, DeviceMode);
    IapErase(EEPROM_DeviceStatus);
    IapProgram(EEPROM_DeviceStatus, DeviceStatus);

    ValveCountDown = Get_HLvalue(SurplusFlow_01, SurplusFlow_02) * Valve1L;
    ValveCountDown_H = Get_Long_H(ValveCountDown);
    ValveCountDown_M1 = Get_Long_M1(ValveCountDown);
    ValveCountDown_M2 = Get_Long_M2(ValveCountDown);
    ValveCountDown_L = Get_Long_L(ValveCountDown);

    IapErase(EEPROM_Vavle);
    IapProgram(EEPROM_Vavle, ValveCountDown_H);
    IapProgram(EEPROM_Vavle + 1, ValveCountDown_M1);
    IapProgram(EEPROM_Vavle + 2, ValveCountDown_M2);
    IapProgram(EEPROM_Vavle + 3, ValveCountDown_L);
}
