#include "Packet.h"
#include "Order.h"
/*
	数据包
*/
unsigned char xdata DeviceID_01 = 0x00;				 //设备ID_01				[01]
unsigned char xdata DeviceID_02 = 0x00;				 //设备ID_02				[02]
unsigned char xdata DeviceID_03 = 0x00;				 //设备ID_03				[03]
unsigned char xdata DeviceID_04 = 0x00;				 //设备ID_04				[04]
unsigned char xdata DeviceMode = MODE_PUBLIC;		 //设备计费模式	         [05]
unsigned char xdata Order = COMMAND_HEARTBEATPACKET; //命令				    [06]
unsigned char xdata DeviceStatus = STATUS_RECOVERY;  //设备状态			    [07]
unsigned char xdata DeviceFlow_01 = 0x00;			 //本次消费流量_01	        [08]
unsigned char xdata DeviceFlow_02 = 0x00;			 //本次消费流量_02	        [09]
unsigned char xdata RechargeFlow_01 = 0x00;			 //充值流量_01			    [10]
unsigned char xdata RechargeFlow_02 = 0x00;			 //充值流量_02			    [11]
unsigned char xdata RechargeDay_01 = 0x00;			 //充值天数_01			    [12]
unsigned char xdata RechargeDay_02 = 0x00;			 //充值天数_02			    [13]
unsigned char xdata SurplusFlow_01 = 0x00;			 //剩余流量_01			    [14]
unsigned char xdata SurplusFlow_02 = 0x00;			 //剩余流量_02			    [15]
unsigned char xdata SurplusDay_01 = 0x00;			 //剩余天数_01			    [16]
unsigned char xdata SurplusDay_02 = 0x00;			 //剩余天数_02			    [17]
unsigned char xdata UsedFlow_01 = 0x00;				 //已使用流量_01		    [18]
unsigned char xdata UsedFlow_02 = 0x00;				 //已使用流量_02		    [19]
unsigned char xdata UsedDay_01 = 0x00;				 //已使用天数_01		    [20]
unsigned char xdata UsedDay_02 = 0x00;				 //已使用天数_02		    [21]
unsigned char xdata WaterTDS_01 = 0x00;				 //纯水TDS值_01			[22]
unsigned char xdata WaterTDS_02 = 0x00;				 //纯水TDS值_02			[23]
unsigned char xdata GPRSSignal = 0x00;				 //GPRS信号强度	        [24]
unsigned char xdata ICCID1_01 = 0x00;				 //ICCID1_01				[25]
unsigned char xdata ICCID1_02 = 0x00;				 //ICCID1_02				[26]
unsigned char xdata ICCID2_01 = 0x00;				 //ICCID2_01				[27]
unsigned char xdata ICCID2_02 = 0x00;				 //ICCID2_01				[28]
unsigned char xdata ICCID3_01 = 0x00;				 //ICCID3_01				[29]
unsigned char xdata ICCID3_02 = 0x00;				 //ICCID3_02				[30]
unsigned char xdata ICCID4_01 = 0x00;				 //ICCID4_01				[31]
unsigned char xdata ICCID4_02 = 0x00;				 //ICCID4_02				[32]
unsigned char xdata ICCID5_01 = 0x00;				 //ICCID5_01				[33]
unsigned char xdata ICCID5_02 = 0x00;				 //ICCID5_02				[34]
unsigned char xdata Check_01 = 0x00;				 //校验位_01				[35]
unsigned char xdata Check_02 = 0x00;				 //校验位_02				[36]

int data StartBool = False;		  //用户开始用水标志位
int data StopBool = False;		  //用户结束用水标志位（流量模式）
int data HeartPacketBool = False; //心跳包标志位
int data ICCIDBool = False;		  //获取ICCID标志位

int xdata Buf_Length = 0;					   //接收缓冲区大小
char xdata Rx_Buffer[MAX_SIZE] = {0x00};	   //接收缓冲区
char xdata Rx_Buffer_AT[MAX_SIZE_AT] = {0x00}; //接收缓冲区
//char xdata Rx_Buffer_AT[MAX_SIZE] = {0x00}; //接收缓冲区

unsigned long data ValveCountDown = 0;  //剩余脉冲计量值（递减）
unsigned char ValveCountDown_H = 0x00;  //剩余脉冲计量值（递减）高八位
unsigned char ValveCountDown_M1 = 0x00; //剩余脉冲计量值（递减）中高八位
unsigned char ValveCountDown_M2 = 0x00; //剩余脉冲计量值（递减）中低八位
unsigned char ValveCountDown_L = 0x00;  //剩余脉冲计量值（递减）低八位
unsigned long data ValveCount = 0;		//本次使用脉冲计量值（递增）
unsigned long data ValveAll = 0;		//总脉冲计量值
unsigned char ValveAll_H = 0x00;		//总使用脉冲计量值（递减）高八位
unsigned char ValveAll_M1 = 0x00;		//总使用脉冲计量值（递减）中高八位
unsigned char ValveAll_M2 = 0x00;		//总使用脉冲计量值（递减）中低八位
unsigned char ValveAll_L = 0x00;		//总使用脉冲计量值（递减）低八位

unsigned int code Valve1L = 1380; //1L水脉冲数
unsigned int code Valve001L = 14; //0.01L水脉冲数

int data Lock = ON; //锁定模式标志位

long data PollTimer = 0;		   //心跳包定时器轮询数
int data PollTimer_Start = 0;	  //开始用水定时器轮询数
int data PollTimer_Stop = 0;	   //结束用水定时器轮询数 Ⅰ
int data PollTimer_Start_Stop = 0; //结束用水定时器轮询数 Ⅱ
int data TempTimer_Start = 0;	  //用户开始用水时脉冲计量数
int data TempTimer_Stop = 0;	   //用户结束用水时脉冲计量数

int data BUSY; //忙绿

int data OpenDeviceCount = 0; //用户开机次数

int data Send_OK_Flag = False; //数据包发送标志位