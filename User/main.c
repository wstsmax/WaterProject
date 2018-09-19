#include "STC8A8K32S4A12.h"

#include "PublicMode.h"
#include "PrivateMode.h"
#include "PublicFunction.h"

#include "Packet.h"
#include "Order.h"

#include "Usart.h"
#include "Gprs.h"
#include "Timer.h"
#include "IODeal.h"
#include "Delay.h"
#include "EEPROM.h"

#include "stdlib.h"
#include "string.h"

int main(void)
{
    P51 = 0;
    GPRS_Init(); //初始化GPRS模块
    Delay_S(5);
    P51 = 1;

    Valve_Init();                                      //初始化脉冲阀
    TIM0_Init();                                       //初始化脉冲计数器
    TIM2_Init();                                       //初始化开始用水计时器
    TIM3_Init();                                       //初始化结束用水计时器
    TIM4_Init();                                       //初始化心跳包计时器
    OpenDeviceCount = IapRead(EEPROM_OpenDeviceCount); //读取开机次数
    if (OpenDeviceCount == 0x00)
    {
        OpenDeviceCount++;
        IapErase(EEPROM_OpenDeviceCount);
        IapProgram(EEPROM_OpenDeviceCount, OpenDeviceCount);
        Recovery_PacketData();
        P50 = 0;
        Delay_MS(500);
        P50 = 1;
    }
    else
    {
        OpenDeviceCount++;
        IapErase(EEPROM_OpenDeviceCount);
        IapProgram(EEPROM_OpenDeviceCount, OpenDeviceCount);
        DeviceMode = IapRead(EEPROM_DeviceMode);
        DeviceStatus = IapRead(EEPROM_DeviceStatus);
        ValveCountDown_H = IapRead(EEPROM_Vavle);
        ValveCountDown_M1 = IapRead(EEPROM_Vavle + 1);
        ValveCountDown_M2 = IapRead(EEPROM_Vavle + 2);
        ValveCountDown_L = IapRead(EEPROM_Vavle + 3);
        ValveCountDown = Get_HMLvalue(ValveCountDown_H, ValveCountDown_M1, ValveCountDown_M2, ValveCountDown_L);

        ValveAll_H = IapRead(EEPROM_VavleALL);
        ValveAll_M1 = IapRead(EEPROM_VavleALL + 1);
        ValveAll_M2 = IapRead(EEPROM_VavleALL + 2);
        ValveAll_L = IapRead(EEPROM_VavleALL + 3);
        P50 = 0;
        Delay_MS(500);
        P50 = 1;
    }
    GPRS_Mode();
    GPRS_ICCID();
    GPRS_ConnectClose();
    GPRS_ConnectCreat(IP, Port);
    while (1)
    {
        DeviceID_01 = IapRead(EEPROM_DeviceID);
        DeviceID_02 = IapRead(EEPROM_DeviceID + 1);
        DeviceID_03 = IapRead(EEPROM_DeviceID + 2);
        DeviceID_04 = IapRead(EEPROM_DeviceID + 3);
        if ((DeviceID_01 == 0x00) && (DeviceID_02 == 0x00) && (DeviceID_03 == 0x00) && (DeviceID_04 == 0x00))
        {
            DeviceStatus = STATUS_WAITACTIVATION;
            IapErase(EEPROM_DeviceStatus);
            IapProgram(EEPROM_DeviceStatus, STATUS_WAITACTIVATION);
            GPRS_SendData(COMMAND_IDCODE, DeviceMode, DeviceStatus);
            while (1)
            {
                if (Order == COMMAND_IDCODE) //ID编码
                {
                    DeviceMode = IapRead(EEPROM_DeviceMode);
                    DeviceStatus = IapRead(EEPROM_DeviceStatus);
                    IapErase(EEPROM_DeviceID);
                    IapProgram(EEPROM_DeviceID, DeviceID_01);
                    IapProgram(EEPROM_DeviceID + 1, DeviceID_02);
                    IapProgram(EEPROM_DeviceID + 2, DeviceID_03);
                    IapProgram(EEPROM_DeviceID + 3, DeviceID_04);
                    GPRS_SendData(COMMAND_RECV_IDCODE, DeviceMode, DeviceStatus);
                    Order = COMMAND_HEARTBEATPACKET;
                    goto MODESWITCH;
                }
                if ((RECV_Return("CLOSED", NULL) == True) || (RECV_Return_Special("CLOSED", NULL) == True))
                {
                    GPRS_ConnectClose();
                    GPRS_ConnectCreat(IP, Port);
                    DeviceStatus = STATUS_WAITACTIVATION;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_WAITACTIVATION);
                    GPRS_SendData(COMMAND_IDCODE, DeviceMode, DeviceStatus);
                    Send_OK_Flag = True;
                    Order = COMMAND_HEARTBEATPACKET;
                    Clear_Buf(Rx_Buffer_AT);
                    Clear_Buf(Rx_Buffer);
                }
                if ((RECV_Return("PDP:", NULL) == True) || (RECV_Return_Special("PDP:", NULL) == True))
                {
                    GPRS_ConnectClose();
                    GPRS_ConnectCreat(IP, Port);
                    DeviceStatus = STATUS_WAITACTIVATION;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_WAITACTIVATION);
                    GPRS_SendData(COMMAND_IDCODE, DeviceMode, DeviceStatus);
                    Send_OK_Flag = True;
                    Order = COMMAND_HEARTBEATPACKET;
                    Clear_Buf(Rx_Buffer_AT);
                    Clear_Buf(Rx_Buffer);
                }
                if (ICCIDBool == True)
                {
                    ICCIDBool = False;
                    GPRS_SendData(COMMAND_IDCODE, DeviceMode, DeviceStatus);
                }
            }
        }
        else
        {
            while (1)
            {
                if (Order != COMMAND_MODESWITCH)
                {
                    GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
                    goto MODESWITCH;
                }
                if ((RECV_Return("CLOSED", NULL) == True) || (RECV_Return_Special("CLOSED", NULL) == True))
                {
                    GPRS_ConnectClose();
                    GPRS_ConnectCreat(IP, Port);
                    IapRead(EEPROM_DeviceMode);
                    IapRead(EEPROM_DeviceStatus);
                    GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
                    Send_OK_Flag = True;
                    Order = COMMAND_HEARTBEATPACKET;
                    Clear_Buf(Rx_Buffer_AT);
                    Clear_Buf(Rx_Buffer);
                }
                if ((RECV_Return("PDP:", NULL) == True) || (RECV_Return_Special("PDP:", NULL) == True))
                {
                    GPRS_ConnectClose();
                    GPRS_ConnectCreat(IP, Port);
                    IapRead(EEPROM_DeviceMode);
                    IapRead(EEPROM_DeviceStatus);
                    GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
                    Send_OK_Flag = True;
                    Order = COMMAND_HEARTBEATPACKET;
                    Clear_Buf(Rx_Buffer_AT);
                    Clear_Buf(Rx_Buffer);
                }
                goto MODESWITCH;
            }
        }
    MODESWITCH:
        Order = COMMAND_HEARTBEATPACKET;
        if (IapRead(EEPROM_DeviceMode) == MODE_PUBLIC)
        {
            PublicMode_Start();
        }
        else if (IapRead(EEPROM_DeviceMode) == MODE_FLOW)
        {
            PrivateMode_Start();
        }
        if ((RECV_Return("CLOSED", NULL) == True) || (RECV_Return_Special("CLOSED", NULL) == True))
        {
            GPRS_ConnectClose();
            GPRS_ConnectCreat(IP, Port);
            IapRead(EEPROM_DeviceMode);
            IapRead(EEPROM_DeviceStatus);
            GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
            Send_OK_Flag = True;
            Order = COMMAND_HEARTBEATPACKET;
            Clear_Buf(Rx_Buffer_AT);
            Clear_Buf(Rx_Buffer);
            goto MODESWITCH;
        }
        if ((RECV_Return("PDP:", NULL) == True) || (RECV_Return_Special("PDP:", NULL) == True))
        {
            GPRS_ConnectClose();
            GPRS_ConnectCreat(IP, Port);
            IapRead(EEPROM_DeviceMode);
            IapRead(EEPROM_DeviceStatus);
            GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
            Send_OK_Flag = True;
            Order = COMMAND_HEARTBEATPACKET;
            Clear_Buf(Rx_Buffer_AT);
            Clear_Buf(Rx_Buffer);
            goto MODESWITCH;
        }
    }
}
