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

void PrivateMode_Start()
{
    unsigned long xdata ValveCountDown_New;
    unsigned long xdata ValveCountDown_Old;
    AUXR |= 0x10;
    IapErase(EEPROM_DeviceMode);
    IapProgram(EEPROM_DeviceMode, MODE_FLOW);
    StopBool = False;
    StartBool = False;
    //DeviceStatus == STATUS_WAITACTIVATION;
    while (1)
    {

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
                GPRS_SendData(COMMAND_RECV_DEVICECLOSE, MODE_FLOW, STATUS_CLOSEDEVICE); //向服务器上传关机命令回执
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_DEVICEOPEN) //开机命令
            {
                if (ValveCountDown > 0)
                {
                    Valve_Switch(ON); //打开脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                }
                else if (ValveCountDown == 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                else if (ValveCountDown < 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    DeviceStatus = STATUS_ARREARAGE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                GPRS_SendData(COMMAND_RECV_DEVICEOPEN, MODE_FLOW, DeviceStatus); //向服务器上传开机命令回执
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_RECHARGE) //充值命令
            {
                ValveCountDown_New = Flow_to_Valve_FlowMode();
                ValveCountDown_Old = ValveCountDown;
                ValveCountDown = ValveCountDown_Old + ValveCountDown_New;
                ValveCountDown_H = (unsigned short)Get_Long_H(ValveCountDown);
                ValveCountDown_M1 = (unsigned short)Get_Long_M1(ValveCountDown);
                ValveCountDown_M2 = (unsigned short)Get_Long_M2(ValveCountDown);
                ValveCountDown_L = (unsigned short)Get_Long_L(ValveCountDown);
                IapErase(EEPROM_Vavle);
                IapProgram(EEPROM_Vavle, ValveCountDown_H);
                IapProgram(EEPROM_Vavle + 1, ValveCountDown_M1);
                IapProgram(EEPROM_Vavle + 2, ValveCountDown_M2);
                IapProgram(EEPROM_Vavle + 3, ValveCountDown_L);

                if (ValveCountDown > 0)
                {
                    Valve_Switch(ON); //打开脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                }
                else if (ValveCountDown == 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                else if (ValveCountDown < 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    DeviceStatus = STATUS_ARREARAGE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }

                GPRS_SendData(COMMAND_RECV_RECHARGE, MODE_FLOW, DeviceStatus); //向服务器上传脉冲充值回执
                RechargeFlow_01 = 0x00;
                RechargeFlow_02 = 0x00;
                Order = COMMAND_HEARTBEATPACKET;
            }
            else if (Order == COMMAND_DEVICEINFOR) //查询设备信息
            {
                DeviceMode = IapRead(EEPROM_DeviceMode);
                DeviceStatus = IapRead(EEPROM_DeviceStatus);
                GPRS_SendData(COMMAND_RECV_DEVICEINFOR, DeviceMode, DeviceStatus); //向服务器上传模式切换回执
                Order = COMMAND_HEARTBEATPACKET;
                // return;
            }
            else if (Order == COMMAND_RECOVERY) //恢复出厂设置
            {
                Valve_Switch(OFF);
                Recovery_PacketData();
                GPRS_SendData(COMMAND_RECV_RECOVERY, DeviceMode, DeviceStatus); //向服务器上传恢复出厂设置回执
                Order = COMMAND_HEARTBEATPACKET;
                return;
            }
            else if (Order == COMMAND_SYSTEMINIT) //更换模式
            {
                Valve_Switch(OFF);
                SystemInit_PacketData();
                GPRS_SendData(COMMAND_RECV_SYSTEMINIT, DeviceMode, DeviceStatus); //向服务器上传模式切换命令回执
                Order = COMMAND_MODESWITCH;
                return;
            }
            else if (Order == COMMAND_DEVICELOCK) //机器锁定命令
            {
                //Valve_Switch(Lock); //第一次接收机器锁定命令，关闭脉冲阀，第二次接到机器锁定命令，打开脉冲阀
                if (Lock == ON)
                {
                    Valve_Switch(OFF);
                    DeviceStatus = STATUS_LOCKDEVICE; //关闭脉冲阀
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_LOCKDEVICE);
                    GPRS_SendData(COMMAND_RECV_DEVICELOCK, MODE_FLOW, DeviceStatus); //向服务器上传机器锁定回执
                    Order = COMMAND_HEARTBEATPACKET;
                    Lock = OFF;
                }
                else if (Lock == OFF)
                {
                    if (ValveCountDown > 0)
                    {
                        Valve_Switch(ON); //打开脉冲阀
                        DeviceStatus = STATUS_OPENDEVICE;
                        IapErase(EEPROM_DeviceStatus);
                        IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                    }
                    else if (ValveCountDown == 0)
                    {
                        Valve_Switch(OFF); //关闭脉冲阀
                        DeviceStatus = STATUS_OPENDEVICE;
                        IapErase(EEPROM_DeviceStatus);
                        IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                    }
                    else if (ValveCountDown < 0)
                    {
                        Valve_Switch(OFF); //关闭脉冲阀
                        DeviceStatus = STATUS_ARREARAGE;
                        IapErase(EEPROM_DeviceStatus);
                        IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                    }
                    GPRS_SendData(COMMAND_RECV_DEVICELOCK, MODE_FLOW, DeviceStatus); //向服务器上传机器锁定回执
                    Order = COMMAND_HEARTBEATPACKET;
                    Lock = ON;
                }
            }
            if (StartBool == True)
            {
                if (ValveCountDown > 0)
                {
                    Valve_Switch(ON); //打开脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                }
                else if (ValveCountDown == 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                else if (ValveCountDown < 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    DeviceStatus = STATUS_ARREARAGE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                StartBool = False;
            }
            if (StopBool == True) //用水同步命令
            {
                if (ValveCountDown > 0)
                {
                    Valve_Switch(ON); //打开脉冲阀
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_OPENDEVICE);
                }
                else if (ValveCountDown == 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    ValveCountDown = 0;
                    DeviceStatus = STATUS_OPENDEVICE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                else if (ValveCountDown < 0)
                {
                    Valve_Switch(OFF); //关闭脉冲阀
                    ValveCountDown = 0;
                    DeviceStatus = STATUS_ARREARAGE;
                    IapErase(EEPROM_DeviceStatus);
                    IapProgram(EEPROM_DeviceStatus, STATUS_ARREARAGE);
                }
                ValveCountDown_H = Get_Long_H(ValveCountDown);
                ValveCountDown_M1 = Get_Long_M1(ValveCountDown);
                ValveCountDown_M2 = Get_Long_M2(ValveCountDown);
                ValveCountDown_L = Get_Long_L(ValveCountDown);

                IapErase(EEPROM_Vavle);
                IapProgram(EEPROM_Vavle, ValveCountDown_H);
                IapProgram(EEPROM_Vavle + 1, ValveCountDown_M1);
                IapProgram(EEPROM_Vavle + 2, ValveCountDown_M2);
                IapProgram(EEPROM_Vavle + 3, ValveCountDown_L);

                ValveAll_H = Get_Long_H(ValveAll);
                ValveAll_M1 = Get_Long_M1(ValveAll);
                ValveAll_M2 = Get_Long_M2(ValveAll);
                ValveAll_L = Get_Long_L(ValveAll);

                IapErase(EEPROM_VavleALL);
                IapProgram(EEPROM_Vavle, ValveAll_H);
                IapProgram(EEPROM_Vavle + 1, ValveAll_M1);
                IapProgram(EEPROM_Vavle + 2, ValveAll_M2);
                IapProgram(EEPROM_Vavle + 3, ValveAll_L);

                if (ValveCount != 0)
                    GPRS_SendData(COMMAND_USEWATERUPLOAD, DeviceMode, DeviceStatus); //向服务器上传用水同步命令
                ValveCount = 0;
                StopBool = False;
            }
        }
        if ((RECV_Return("CLOSED", NULL) == True) || (RECV_Return_Special("CLOSED", NULL) == True))
        {
            GPRS_ConnectClose();
            GPRS_ConnectCreat(IP, Port);
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
            Send_OK_Flag = True;
            Order = COMMAND_MODESWITCH;
            Clear_Buf(Rx_Buffer_AT);
            Clear_Buf(Rx_Buffer);
            return;
        }
    }
}