#include "STC8A8K32S4A12.h"

#include "PublicMode.h"
#include "PrivateMode.h"
#include "PublicFunction.h"
#include "Packet.h"
#include "Order.h"
#include "Usart.h"
#include "Gprs.h"
#include "EEPROM.h"
#include "Delay.h"

#include "string.h"
#include "ctype.h"

void PublicMode_Start(void)
{
    int ValveFlag = OFF;
    Valve_Switch(OFF);
    IapErase(EEPROM_DeviceMode);
    IapProgram(EEPROM_DeviceMode, MODE_PUBLIC);
    AUXR &= ~0x10;  //用户开始用水定时器关闭
    T4T3M &= ~0x08; //用户结束用水定时器开启

    while (1)
    {
        //Uart1_Send_Data("AT\r\n");
        if (HeartPacketBool == True)
        {
            GPRS_SendHeartPacket();
            HeartPacketBool = False;
            Send_OK_Flag = True;
        }
        if ((IapRead(EEPROM_DeviceStatus)) == STATUS_WAITACTIVATION)
        {
            if (Order == COMMAND_SYSTEMINIT)
            {
                Valve_Switch(OFF);
                SystemInit_PacketData();
                GPRS_SendData(COMMAND_RECV_SYSTEMINIT, DeviceMode, DeviceStatus); //向服务器上传模式切换命令回执
                Order = COMMAND_MODESWITCH;
                return;
            }
            else if (Order == COMMAND_ERRORUPLOAD)
            {
                DeviceMode = IapRead(EEPROM_DeviceMode);
                DeviceStatus = IapRead(EEPROM_DeviceStatus);
                GPRS_SendData(COMMAND_ERRORUPLOAD, DeviceMode, DeviceStatus);
                Order = COMMAND_HEARTBEATPACKET;
            }
        }
        else
        {
            if (Order == COMMAND_ERRORUPLOAD)
            {
                DeviceMode = IapRead(EEPROM_DeviceMode);
                DeviceStatus = IapRead(EEPROM_DeviceStatus);
                GPRS_SendData(COMMAND_ERRORUPLOAD, DeviceMode, DeviceStatus);
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_DEVICECLOSE) //关机命令
            {
                Valve_Switch(OFF);
                DeviceStatus = STATUS_CLOSEDEVICE;
                IapErase(EEPROM_DeviceStatus);
                IapProgram(EEPROM_DeviceStatus, STATUS_CLOSEDEVICE);
                GPRS_SendData(COMMAND_RECV_DEVICECLOSE, MODE_PUBLIC, STATUS_CLOSEDEVICE); //向服务器上传关机命令回执
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_DEVICEOPEN) //开机命令
            {
                Valve_Switch(OFF);
                DeviceStatus = STATUS_OPENDEVICE;
                IapErase(EEPROM_DeviceStatus);
                IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                GPRS_SendData(COMMAND_RECV_DEVICEOPEN, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传开机命令回执
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_SYSTEMINIT) //模式切换
            {
                Valve_Switch(OFF);
                SystemInit_PacketData();
                GPRS_SendData(COMMAND_RECV_SYSTEMINIT, DeviceMode, DeviceStatus); //向服务器上传模式切换命令回执
                Order = COMMAND_MODESWITCH;
                return;
            }
            else if (Order == COMMAND_RECOVERY) //恢复出厂设置
            {
                Valve_Switch(OFF);
                Recovery_PacketData();
                GPRS_SendData(COMMAND_RECV_RECOVERY, DeviceMode, STATUS_RECOVERY); //向服务器上传恢复出厂设置回执
                Order = COMMAND_HEARTBEATPACKET;
                return;
            }
            else if (Order == COMMAND_DEVICEINFOR) //查询设备信息
            {
                DeviceMode = IapRead(EEPROM_DeviceMode);
                DeviceStatus = IapRead(EEPROM_DeviceStatus);
                GPRS_SendData(COMMAND_RECV_DEVICEINFOR, DeviceMode, DeviceStatus); //向服务器上传模式切换回执
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_DEVICELOCK) //机器锁定命令
            {
                Valve_Switch(OFF);
                if (Lock == ON)
                {
                    DeviceStatus = STATUS_LOCKDEVICE; //接收机器锁定命令，关闭脉冲阀
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_LOCKDEVICE);
                    Lock = OFF;
                }
                else if (Lock == OFF)
                {
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                    Lock = ON;
                }
                GPRS_SendData(COMMAND_RECV_DEVICELOCK, MODE_PUBLIC, DeviceStatus); //向服务器上传机器锁定回执
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_USEWATER) //取水命令
            {
                DeviceStatus = STATUS_OPENDEVICE;
                IapErase(EEPROM_DeviceStatus);
                IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                ValveCountDown = Flow_to_Valve_PublicMode();

                Valve_Switch(ON);
                ValveCount = 0;
                GPRS_SendData(COMMAND_RECV_USEWATER, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                PollTimer_Stop = 0;
                PollTimer_Start_Stop = 0;
                T4T3M |= 0x08; //用户结束用水定时器开启
                while (1)
                {
                    if (Order == COMMAND_STOPWATER)
                    {
                        Valve_Switch(OFF);
                        //ValveFlag = ON;
                        GPRS_SendData(COMMAND_RECV_STOPWATER, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                        //if (DeviceFlow_01 != 0x00 && DeviceFlow_02 != 0x00)
                        //GPRS_SendData(COMMAND_USEWATERUPLOAD, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                        goto STOPWATER;
                    }
                    else if (Order == COMMAND_ERRORUPLOAD)
                    {
                        DeviceMode = IapRead(EEPROM_DeviceMode);
                        DeviceStatus = IapRead(EEPROM_DeviceStatus);
                        GPRS_SendData(COMMAND_ERRORUPLOAD, DeviceMode, DeviceStatus);
                        Order = COMMAND_HEARTBEATPACKET;
                    }
                    if (ValveCountDown <= 0)
                    {
                        Valve_Switch(OFF);
                        ValveFlag = ON;
                        ValveCountDown = 0;
                        //GPRS_SendData(COMMAND_RECV_STOPWATER, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                        //if (DeviceFlow_01 != 0x00 && DeviceFlow_02 != 0x00)
                        //GPRS_SendData(COMMAND_USEWATERUPLOAD, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                        //goto STOPWATER;
                    }
                    if (StopBool == True)
                    {
                        StopBool = False;
                        //Valve_Switch(OFF);
                        //GPRS_SendData(COMMAND_RECV_STOPWATER, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                        if (ValveCountDown <= 0)
                            ValveCountDown = 0;
                        if (ValveCount != 0)
                            GPRS_SendData(COMMAND_USEWATERUPLOAD, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                        ValveCount = 0;
                        if (ValveFlag == ON)
                        {
                            ValveFlag = OFF;
                            goto STOPWATER;
                        }
                        T4T3M |= 0x08;
                        //ValveCountDown = 0;
                    }
                    //goto STOPWATER;
                }

            STOPWATER:
                AUXR &= ~0x10; //用户开始用水定时器关闭
                T4T3M |= 0x08;
                //DeviceStatus = STATUS_OPENDEVICE;

                ValveAll_H = Get_Long_H(ValveAll);
                ValveAll_M1 = Get_Long_M1(ValveAll);
                ValveAll_M2 = Get_Long_M2(ValveAll);
                ValveAll_L = Get_Long_L(ValveAll);

                IapErase(EEPROM_VavleALL);
                IapProgram(EEPROM_Vavle, ValveAll_H);
                IapProgram(EEPROM_Vavle + 1, ValveAll_M1);
                IapProgram(EEPROM_Vavle + 2, ValveAll_M2);
                IapProgram(EEPROM_Vavle + 3, ValveAll_L);
                //GPRS_SendData(COMMAND_RECV_STOPWATER, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                //GPRS_SendData(COMMAND_USEWATERUPLOAD, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                ValveCount = 0;
                ValveCountDown = 0;
                DeviceFlow_01 = 0x00;
                DeviceFlow_02 = 0x00;

                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_STOPWATER)
            {
                GPRS_SendData(COMMAND_RECV_STOPWATER, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                //GPRS_SendData(COMMAND_USEWATERUPLOAD, MODE_PUBLIC, STATUS_OPENDEVICE); //向服务器上传取水命令回执
                Order = COMMAND_HEARTBEATPACKET;
            }
        }

        if ((RECV_Return("CLOSED", NULL) == True) || (RECV_Return_Special("CLOSED", NULL) == True))
        {
            GPRS_ConnectClose();
            GPRS_ConnectCreat(IP, Port);
            IapRead(EEPROM_DeviceMode);
            IapRead(EEPROM_DeviceStatus);
            GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
            Send_OK_Flag = True;
            Order = COMMAND_MODESWITCH;
            Clear_Buf(Rx_Buffer_AT);
            Clear_Buf(Rx_Buffer);
            return;
        }
        if ((RECV_Return("PDP:", NULL) == True) || (RECV_Return_Special("PDP:", NULL) == True))
        {
            GPRS_ConnectClose();
            GPRS_ConnectCreat(IP, Port);
            IapRead(EEPROM_DeviceMode);
            IapRead(EEPROM_DeviceStatus);
            GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
            Send_OK_Flag = True;
            Order = COMMAND_MODESWITCH;
            Clear_Buf(Rx_Buffer_AT);
            Clear_Buf(Rx_Buffer);
            return;
        }
    }
}
