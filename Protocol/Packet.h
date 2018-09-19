#ifndef __PACKET_H__
#define __PACKET_H__

/*
	数据包
*/
extern unsigned char xdata DeviceID_01;		//设备ID_01				[01]
extern unsigned char xdata DeviceID_02;		//设备ID_02				[02]
extern unsigned char xdata DeviceID_03;		//设备ID_03				[03]
extern unsigned char xdata DeviceID_04;		//设备ID_04				[04]
extern unsigned char xdata DeviceMode;		//设备计费模式	            [05]
extern unsigned char xdata Order;			//命令				       [06]
extern unsigned char xdata DeviceStatus;	//设备状态			        [07]
extern unsigned char xdata DeviceFlow_01;   //本次消费流量_01	        [08]
extern unsigned char xdata DeviceFlow_02;   //本次消费流量_02	        [09]
extern unsigned char xdata RechargeFlow_01; //充值流量_01			    [10]
extern unsigned char xdata RechargeFlow_02; //充值流量_02			    [11]
extern unsigned char xdata RechargeDay_01;  //充值天数_01			    [12]
extern unsigned char xdata RechargeDay_02;  //充值天数_02			    [13]
extern unsigned char xdata SurplusFlow_01;  //剩余流量_01			    [14]
extern unsigned char xdata SurplusFlow_02;  //剩余流量_02			    [15]
extern unsigned char xdata SurplusDay_01;   //剩余天数_01			    [16]
extern unsigned char xdata SurplusDay_02;   //剩余天数_02			    [17]
extern unsigned char xdata UsedFlow_01;		//已使用流量_01		    [18]
extern unsigned char xdata UsedFlow_02;		//已使用流量_02		    [19]
extern unsigned char xdata UsedDay_01;		//已使用天数_01		    [20]
extern unsigned char xdata UsedDay_02;		//已使用天数_02		    [21]
extern unsigned char xdata WaterTDS_01;		//纯水TDS值_01			    [22]
extern unsigned char xdata WaterTDS_02;		//纯水TDS值_02			    [23]
extern unsigned char xdata GPRSSignal;		//GPRS信号强度_01	        [24]
extern unsigned char xdata ICCID1_01;		//ICCID1_01				[25]
extern unsigned char xdata ICCID1_02;		//ICCID1_02				[26]
extern unsigned char xdata ICCID2_01;		//ICCID2_01				[27]
extern unsigned char xdata ICCID2_02;		//ICCID2_01				[28]
extern unsigned char xdata ICCID3_01;		//ICCID3_01				[29]
extern unsigned char xdata ICCID3_02;		//ICCID3_02				[30]
extern unsigned char xdata ICCID4_01;		//ICCID4_01				[31]
extern unsigned char xdata ICCID4_02;		//ICCID4_02				[32]
extern unsigned char xdata ICCID5_01;		//ICCID5_01				[33]
extern unsigned char xdata ICCID5_02;		//ICCID5_02				[34]
extern unsigned char xdata Check_01;		//校验位_01				[35]
extern unsigned char xdata Check_02;		//校验位_02				[36]

extern int xdata Buf_Length;	  //接收缓冲区大小
extern char xdata Rx_Buffer[];	//接收缓冲区
extern char xdata Rx_Buffer_AT[]; //接收缓冲区

extern unsigned long data ValveCountDown; //剩余脉冲计量值（递减）
extern unsigned char ValveCountDown_H;	//剩余脉冲计量值（递减）高八位
extern unsigned char ValveCountDown_M1;   //剩余脉冲计量值（递减）中高八位
extern unsigned char ValveCountDown_M2;   //剩余脉冲计量值（递减）中低八位
extern unsigned char ValveCountDown_L;	//剩余脉冲计量值（递减）低八位
extern unsigned long data ValveCount;	 //本次使用脉冲计量值（递增）
extern unsigned long data ValveAll;		  //总脉冲计量值
extern unsigned char ValveAll_H;		  //总使用脉冲计量值（递减）高八位
extern unsigned char ValveAll_M1;		  //总使用脉冲计量值（递减）中高八位
extern unsigned char ValveAll_M2;		  //总使用脉冲计量值（递减）中低八位
extern unsigned char ValveAll_L;		  //总使用脉冲计量值（递减）低八位
extern unsigned int code Valve1L;		  //1L水脉冲数
extern unsigned int code Valve001L;		  //0.01L水脉冲数

extern int data Lock; //锁定模式标志位

extern long data PollTimer;			  //心跳包定时器轮询数
extern int data PollTimer_Start;	  //开始用水定时器轮询数
extern int data PollTimer_Stop;		  //结束用水定时器轮询数 Ⅰ
extern int data PollTimer_Start_Stop; //结束用水定时器轮询数 Ⅱ
extern int data TempTimer_Start;	  //用户开始用水时脉冲计量数
extern int data TempTimer_Stop;		  //用户结束用水时脉冲计量数

extern int data StartBool;		 //用户开始用水标志位
extern int data StopBool;		 //用户结束用水标志位（流量模式）
extern int data HeartPacketBool; //心跳包标志位
extern int data ICCIDBool;		 //获取ICCID标志位

extern int data BUSY; //忙绿

extern int data OpenDeviceCount; //用户开机次数

extern int data Send_OK_Flag; //数据包发送标志位

#endif
