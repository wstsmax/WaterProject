#include "STC8A8K32S4A12.h"

#include "Gprs.h"
#include "PublicFunction.h"
#include "Usart.h"
#include "Order.h"
#include "Packet.h"
#include "Delay.h"
#include "EEPROM.h"

#include "stdlib.h"
#include "string.h"

int RECV_Return(char *Recv1, char *Recv2)
{
	if ((Recv1 == 0) && (Recv2 == 0)) //如果Recv1，Recv2为NULL
	{
		return True;
	}
	if ((Recv1 != 0) && (Recv2 != 0)) //如果Recv1，Recv2不为NULL
	{
		if ((strstr(Rx_Buffer_AT, Recv1) && strstr(Rx_Buffer_AT, Recv2)) != NULL) //Rx_Buffer中是否同时包含Recv1，Recv2
			return True;
		else
			return False;
	}
	else if (Recv1 != 0) //如果Recv1不为NULL
	{
		if (strstr(Rx_Buffer_AT, Recv1) != NULL) //Rx_Buffer中是否包含Recv1
			return True;
		else
			return False;
	}
	else //如果Recv1不为NULL
	{
		if (strstr(Rx_Buffer_AT, Recv2) != NULL) //Rx_Buffer中是否包含Recv2
			return True;
		else
			return False;
	}
}

int RECV_Return_Special(char *Recv1, char *Recv2)
{
	if ((Recv1 == 0) && (Recv2 == 0)) //如果Recv1，Recv2为NULL
	{
		return True;
	}
	if ((Recv1 != 0) && (Recv2 != 0)) //如果Recv1，Recv2不为NULL
	{
		if ((strstr(Rx_Buffer, Recv1) && strstr(Rx_Buffer, Recv2)) != NULL) //Rx_Buffer中是否同时包含Recv1，Recv2
			return True;
		else
			return False;
	}
	else if (Recv1 != 0) //如果Recv1不为NULL
	{
		if (strstr(Rx_Buffer, Recv1) != NULL) //Rx_Buffer中是否包含Recv1
			return True;
		else
			return False;
	}
	else //如果Recv1不为NULL
	{
		if (strstr(Rx_Buffer, Recv2) != NULL) //Rx_Buffer中是否包含Recv2
			return True;
		else
			return False;
	}
}

void GPRS_Init()
{
	Uart1_Init(); //初始化串口1
	P35 = 0;
	Delay_S(1);
	Delay_MS(200);
	P35 = 1;
}
void GPRS_Mode(void)
{
	Clear_Buf(Rx_Buffer_AT);				 //清空接收缓冲区
	Uart1_Send_Data("AT\n");				 //串口发送 "AT"
	Delay_MS(500);							 //延时500毫秒（0.5秒）
	while (RECV_Return("OK", NULL) == False) //检测接受缓冲区中是否包含"OK"，如果不包含执行下列语句
	{
		Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
		Uart1_Send_Data("AT\n"); //串口发送 "AT"
		Delay_MS(500);			 //延时500毫秒（0.5秒）
								 //Uart2_Send_Data(Rx_Buffer);
	}
	Clear_Buf(Rx_Buffer_AT);					 //清空接收缓冲区
	Uart1_Send_Data("AT+CPIN?\r\n");			 //串口发送 "AT+CPIN?"
	Delay_MS(500);								 //延时500毫秒（0.5秒）
	while (RECV_Return("+CPIN:", "OK") == False) //检测接受缓冲区中是否包含"+CPIN: READY"和"OK"，如果不包含执行下列语句
	{
		Clear_Buf(Rx_Buffer_AT);		 //清空接收缓冲区
		Uart1_Send_Data("AT+CPIN?\r\n"); //串口发送 "AT+CPIN?"
		Delay_MS(500);					 //延时500毫秒（0.5秒）
	}
	Clear_Buf(Rx_Buffer_AT);					 //清空接收缓冲区
	Uart1_Send_Data("AT+CREG?\r\n");			 //串口发送 "AT+CREG?"
	Delay_MS(500);								 //延时500毫秒（0.5秒）
	while (RECV_Return("+CREG:", NULL) == False) //检测接受缓冲区中是否包含"CREG:"，如果不包含执行下列语句
	{
		Clear_Buf(Rx_Buffer_AT);		 //清空接收缓冲区
		Uart1_Send_Data("AT+CREG?\r\n"); //串口发送 "AT+CREG?"
		Delay_MS(500);					 //延时500毫秒（0.5秒）
	}
	//Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
	//Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
	Clear_Buf(Rx_Buffer_AT);				 //清空接收缓冲区
	Uart1_Send_Data("AT+CIPQSEND=0\r\n");	//串口发送 "AT+CREG?"
	Delay_MS(500);							 //延时500毫秒（0.5秒）
	while (RECV_Return("OK", NULL) == False) //检测接受缓冲区中是否包含"CREG:"，如果不包含执行下列语句
	{
		Clear_Buf(Rx_Buffer_AT);			  //清空接收缓冲区
		Uart1_Send_Data("AT+CIPQSEND=0\r\n"); //串口发送 "AT+CREG?"
		Delay_MS(500);						  //延时500毫秒（0.5秒）
	}
	Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
}

void GPRS_ConnectCreat(char *ServerIP, char *ServerPort)
{
	Uart1_Send_Data("AT+CIPSTART="); //串口发送 "AT+CIPSTART="
	Uart1_Send(34);					 //串口发送 "
	Uart1_Send_Data("TCP");			 //串口发送 "TCP"
	Uart1_Send(34);					 //串口发送 "
	Uart1_Send(44);					 //串口发送 ,
	Uart1_Send(34);					 //串口发送 "
	Uart1_Send_Data(ServerIP);		 //串口发送 ServerIP
	Uart1_Send(34);					 //串口发送 "
	Uart1_Send(44);					 //串口发送 ,
	Uart1_Send(34);					 //串口发送 "
	Uart1_Send_Data(ServerPort);	 //串口发送 ServerPort
	Uart1_Send(34);					 //串口发送 "
	Uart1_Send_Data("\r\n");		 //串口发送 "\r\n"
	Delay_MS(5000);					 //延时2000毫秒（2秒）
	while (1)						 //检测接受缓冲区中是否包含"CONNECT OK"，如果不包含执行下列语句
	{
		if (RECV_Return("CONNECT OK", NULL) == True)
		{
			Clear_Buf(Rx_Buffer_AT);
			return;
		}
		if (RECV_Return("ALREADY CONNECT", NULL) == True)
		{
			Clear_Buf(Rx_Buffer_AT);
			return;
		}
		if (RECV_Return("CONNECT FAIL", NULL) == True)
		{
			Clear_Buf(Rx_Buffer_AT);		 //清空接收缓冲区
			Uart1_Send_Data("AT+CIPSTART="); //串口发送 AT+CIPSTART="
			Uart1_Send(34);					 //串口发送 "
			Uart1_Send_Data("TCP");			 //串口发送 "TCP"
			Uart1_Send(34);					 //串口发送 "
			Uart1_Send(44);					 //串口发送 ,
			Uart1_Send(34);					 //串口发送 "
			Uart1_Send_Data(ServerIP);		 //串口发送 ServerIP
			Uart1_Send(34);					 //串口发送 "
			Uart1_Send(44);					 //串口发送 ,
			Uart1_Send(34);					 //串口发送 "
			Uart1_Send_Data(ServerPort);	 //串口发送 ServerPort
			Uart1_Send(34);					 //串口发送 "
			Uart1_Send_Data("\r\n");		 //串口发送 "\r\n"
											 //Delay_MS(5000);					 //延时2000毫秒（2秒）
											 //Uart2_Send_Data(Rx_Buffer);
											 //return;
		}
	}
	//Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
}

void GPRS_ConnectClose(void)
{
	Clear_Buf(Rx_Buffer);
	Uart1_Send_Data("AT+CIPSHUT\r\n");
	Delay_MS(5000);
	while (RECV_Return("SHUT OK", NULL) == False)
	{
		Clear_Buf(Rx_Buffer);
		Uart1_Send_Data("AT+CIPSHUT\r\n");
		Delay_MS(5000);
	}
	Clear_Buf(Rx_Buffer);
}

void GPRS_SendData(char Command, char Mode, char Status)
{
	int Send_Num = 0;
	int xdata HCheck;		 //校验位高位
	int xdata LCheck;		 //校验位低位
	Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
	Send_OK_Flag = False;
	GPRS_Signal();
	Uart1_Send_Data("AT+CIPSEND=36\r\n"); //串口发送 "AT+CIPSEND"
	//Delay_MS(2000);							//延时500毫秒（0.5秒）
	while (1) //检测接受缓冲区中是否包含">"，如果不包含执行下列语句
	{
		if (RECV_Return(">", NULL) == True)
		{
			goto SENDFLAG;
		}
		if ((RECV_Return("ERROR", NULL) == True) || (RECV_Return_Special("ERROR", NULL) == True))
		{
			GPRS_ConnectClose();
			GPRS_ConnectCreat(IP, Port);
			IapRead(EEPROM_DeviceMode);
			IapRead(EEPROM_DeviceStatus);
			GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
			Clear_Buf(Rx_Buffer_AT);
			Clear_Buf(Rx_Buffer);
		}
		//Clear_Buf(Rx_Buffer_AT);			  //清空接收缓冲区
		//Uart1_Send_Data("AT+CIPSEND=36\r\n"); //串口发送 "AT+CIPSEND"
		//Delay_MS(2000);
	}
SENDFLAG:
	Clear_Buf(Rx_Buffer_AT); //清空接收缓冲区
	Clear_Buf(Rx_Buffer);
	DeviceID_01 = IapRead(EEPROM_DeviceID);
	DeviceID_02 = IapRead(EEPROM_DeviceID + 1);
	DeviceID_03 = IapRead(EEPROM_DeviceID + 2);
	DeviceID_04 = IapRead(EEPROM_DeviceID + 3);
	Order = Command;	   //发送命令
	DeviceMode = Mode;	 //发送设备模式
	DeviceStatus = Status; //发送设备状态
	Valve_to_Flow();	   //将脉冲转换为流量
	ICCID1_01 = IapRead(EEPROM_ICCID);
	ICCID1_02 = IapRead(EEPROM_ICCID + 1);
	ICCID2_01 = IapRead(EEPROM_ICCID + 2);
	ICCID2_02 = IapRead(EEPROM_ICCID + 3);
	ICCID3_01 = IapRead(EEPROM_ICCID + 4);
	ICCID3_02 = IapRead(EEPROM_ICCID + 5);
	ICCID4_01 = IapRead(EEPROM_ICCID + 6);
	ICCID4_02 = IapRead(EEPROM_ICCID + 7);
	ICCID5_01 = IapRead(EEPROM_ICCID + 8);
	ICCID5_02 = IapRead(EEPROM_ICCID + 9);
	Get_Check(&HCheck, &LCheck); //计算校验位
	Check_01 = HCheck;			 //保存校验位高位
	Check_02 = LCheck;			 //保存校验位低位
	Uart1_Send(DeviceID_01);	 //串口发送 设备ID_01
	Uart1_Send(DeviceID_02);	 //串口发送 设备ID_02
	Uart1_Send(DeviceID_03);	 //串口发送 设备ID_03
	Uart1_Send(DeviceID_04);	 //串口发送 设备ID_04
	Uart1_Send(DeviceMode);		 //串口发送 设备计费模式
	Uart1_Send(Order);			 //串口发送 命令
	Uart1_Send(DeviceStatus);	//串口发送 设备状态
	Uart1_Send(DeviceFlow_01);   //串口发送 本次消费流量_01
	Uart1_Send(DeviceFlow_02);   //串口发送 本次消费流量_02
	Uart1_Send(RechargeFlow_01); //串口发送 充值流量_01
	Uart1_Send(RechargeFlow_02); //串口发送 充值流量_02
	Uart1_Send(RechargeDay_01);  //串口发送 充值天数_01
	Uart1_Send(RechargeDay_02);  //串口发送 充值天数_02
	Uart1_Send(SurplusFlow_01);  //串口发送 剩余流量_01
	Uart1_Send(SurplusFlow_02);  //串口发送 剩余流量_02
	Uart1_Send(SurplusDay_01);   //串口发送 剩余天数_01
	Uart1_Send(SurplusDay_02);   //串口发送 剩余天数_02
	Uart1_Send(UsedFlow_01);	 //串口发送 已使用流量_01
	Uart1_Send(UsedFlow_02);	 //串口发送 已使用流量_02
	Uart1_Send(UsedDay_01);		 //串口发送 已使用天数_01
	Uart1_Send(UsedDay_02);		 //串口发送 已使用天数_02
	Uart1_Send(WaterTDS_01);	 //串口发送 纯水TDS值_01
	Uart1_Send(WaterTDS_02);	 //串口发送 纯水TDS值_02
	Uart1_Send(GPRSSignal);		 //串口发送 GPRS信号强度
	Uart1_Send(ICCID1_01);		 //串口发送 ICCID1_01
	Uart1_Send(ICCID1_02);		 //串口发送 ICCID1_02
	Uart1_Send(ICCID2_01);		 //串口发送 ICCID2_01
	Uart1_Send(ICCID2_02);		 //串口发送 ICCID2_02
	Uart1_Send(ICCID3_01);		 //串口发送 ICCID3_01
	Uart1_Send(ICCID3_02);		 //串口发送 ICCID3_02
	Uart1_Send(ICCID4_01);		 //串口发送 ICCID4_01
	Uart1_Send(ICCID4_02);		 //串口发送 ICCID4_02
	Uart1_Send(ICCID5_01);		 //串口发送 ICCID5_01
	Uart1_Send(ICCID5_02);		 //串口发送 ICCID5_02
	Uart1_Send(Check_01);		 //串口发送 校验位_01
	Uart1_Send(Check_02);		 //串口发送 校验位_02
	Order = COMMAND_HEARTBEATPACKET;
	//Uart1_Send(0x1A); //串口发送 0x1A
	while (1)
	{
		if (RECV_Return("OK", NULL) == True) //检测接受缓冲区中是否包含"SEND OK"，如果不包含执行下列语句
		{
			Send_OK_Flag = True;
			return;
		}
	}
}
void GPRS_SendHeartPacket(void)
{
	int Send_Num = 0;
	Clear_Buf(Rx_Buffer_AT);
	Send_OK_Flag = False;
	DeviceID_01 = IapRead(EEPROM_DeviceID);
	DeviceID_02 = IapRead(EEPROM_DeviceID + 1);
	DeviceID_03 = IapRead(EEPROM_DeviceID + 2);
	DeviceID_04 = IapRead(EEPROM_DeviceID + 3);
	Uart1_Send_Data("AT+CIPSEND=4\r\n"); //串口发送 "AT+CIPSEND"
	//Delay_MS(2000);							//延时500毫秒（0.5秒）
	while (1) //检测接受缓冲区中是否包含">"，如果不包含执行下列语句
	{
		if (RECV_Return(">", NULL) == True)
		{
			goto SENDHEARTFLAG;
		}
		if ((RECV_Return("ERROR", NULL) == True) || (RECV_Return_Special("ERROR", NULL) == True))
		{
			GPRS_ConnectClose();
			GPRS_ConnectCreat(IP, Port);
			IapRead(EEPROM_DeviceMode);
			IapRead(EEPROM_DeviceStatus);
			GPRS_SendData(COMMAND_HEARTBEATPACKET, DeviceMode, DeviceStatus);
			Order = COMMAND_HEARTBEATPACKET;
			Clear_Buf(Rx_Buffer_AT);
			Clear_Buf(Rx_Buffer);
		}
	}
SENDHEARTFLAG:
	Clear_Buf(Rx_Buffer_AT);
	Uart1_Send(DeviceID_01); //串口发送 心跳命令
	Uart1_Send(DeviceID_02); //串口发送 0x00
	Uart1_Send(DeviceID_03); //串口发送 0x00
	Uart1_Send(DeviceID_04); //串口发送 0x00
	//Uart1_Send(0x1A);	//串口发送 0x1A
	while (1)
	{
		if (RECV_Return("OK", NULL) == True) //检测接受缓冲区中是否包含"SEND OK"，如果不包含执行下列语句
		{
			Send_OK_Flag = True;
			return;
		}
	}
}

void GPRS_Signal(void)
{
	char Signal[3];
	Clear_Buf(Rx_Buffer_AT);
	Uart1_Send_Data("AT+CSQ\r\n");
	Delay_MS(500);
	//Uart2_Send_Data(Rx_Buffer);
	while (RECV_Return("+CSQ: ", NULL) == False)
	{
		Clear_Buf(Rx_Buffer_AT);
		Uart1_Send_Data("AT+CSQ\r\n");
		Delay_MS(500);
		//Uart2_Send_Data(Rx_Buffer);
	}
	GPRSSignal = atoi(strncpy(Signal, strchr(Rx_Buffer_AT, ' '), 3));
	IapErase(EEPROM_GPRSSignal);
	IapProgram(EEPROM_GPRSSignal, GPRSSignal);
}

void GPRS_ICCID(void)
{
	unsigned char Signal[21] = {0x00};
	char Temp[3] = {0x00, 0x00, 0x00};
	Clear_Buf(Rx_Buffer_AT);
	Uart1_Send_Data("AT+ICCID\r\n");
	Delay_MS(500);
	while (RECV_Return("+ICCID: ", NULL) == False)
	{
		Clear_Buf(Rx_Buffer_AT);
		Uart1_Send_Data("AT+ICCID\r\n");
		Delay_MS(500);
	}
	strncpy(Signal, strchr(Rx_Buffer_AT, ' ') + 1, 20);
	strncpy(Temp, Signal + 0, 2);
	ICCID1_01 = Hex_to_Int(Temp);
	IapErase(EEPROM_ICCID);
	IapProgram(EEPROM_ICCID, ICCID1_01);

	strncpy(Temp, Signal + 2, 2);
	ICCID1_02 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 1, ICCID1_02);

	strncpy(Temp, Signal + 4, 2);
	ICCID2_01 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 2, ICCID2_01);

	strncpy(Temp, Signal + 6, 2);
	ICCID2_02 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 3, ICCID2_02);

	strncpy(Temp, Signal + 8, 2);
	ICCID3_01 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 4, ICCID3_01);

	strncpy(Temp, Signal + 10, 2);
	ICCID3_02 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 5, ICCID3_02);

	strncpy(Temp, Signal + 12, 2);
	ICCID4_01 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 6, ICCID4_01);

	strncpy(Temp, Signal + 14, 2);
	ICCID4_02 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 7, ICCID4_02);

	strncpy(Temp, Signal + 16, 2);
	ICCID5_01 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 8, ICCID5_01);

	strncpy(Temp, Signal + 18, 2);
	ICCID5_02 = Hex_to_Int(Temp);
	IapProgram(EEPROM_ICCID + 9, ICCID5_02);
}