#ifndef __ORDER_H__
#define __ORDER_H__

#define MAX_SIZE 36
#define MAX_SIZE_AT 50
#define MAX_TEMP 10

/*
	服务器IP及端口
*/
#define IP "n2190e7033.iok.la"
#define Port "19565"

/*
	命令表
*/
/*
	设备上传命令
*/
#define COMMAND_HEARTBEATPACKET 0x00  //心跳包
#define COMMAND_RECV_DEVICECLOSE 0x11 //关机命令回执
#define COMMAND_RECV_DEVICEOPEN 0x22  //开机命令回执
#define COMMAND_RECV_IDCODE 0x44	  //ID编码回执
#define COMMAND_RECV_RECHARGE 0x55	//充值命令回执
#define COMMAND_USEWATERUPLOAD 0x06   //用水同步命令
#define COMMAND_RECV_MODESWITCH 0x88  //模式切换回执
#define COMMAND_RECV_SYSTEMINIT 0x99  //系统初始化回执
#define COMMAND_RECV_RECOVERY 0xAA	//恢复出厂设置回执
#define COMMAND_RECV_TIME 0xBB		  //用时同步回执
#define COMMAND_STATUSSWITCH 0x0C	 //设备状态变更
#define COMMAND_RECV_DEVICEINFOR 0xDD //查询设备信息回执
#define COMMAND_RECV_DEVICELOCK 0xFF  //机器锁定命令回执
#define COMMAND_ERRORUPLOAD 0xCC	  //错误包上报
#define COMMAND_RECV_USEWATER 0x91	//取水命令回执
#define COMMAND_RECV_STOPWATER 0x92   //停水命令回执

/*
	服务器下发命令
*/
#define COMMAND_DEVICECLOSE 0x01 //关机命令
#define COMMAND_DEVICEOPEN 0x02  //开机命令
#define COMMAND_IDCODE 0x04		 //ID编码
#define COMMAND_RECHARGE 0x05	//充值命令
#define COMMAND_MODESWITCH 0x08  //模式切换
#define COMMAND_SYSTEMINIT 0x09  //系统初始化
#define COMMAND_RECOVERY 0x0A	//恢复出厂设置
#define COMMAND_USETIME 0x0B	 //用时同步
#define COMMAND_DEVICEINFOR 0x0D //查询设备信息
#define COMMAND_DEVICELOCK 0x0F  //机器锁定命令
#define COMMAND_USEWATER 0x81	//取水命令
#define COMMAND_STOPWATER 0x82   //停水命令

/*
	计费模式
*/
#define MODE_TIME 0x00	//时长模式
#define MODE_FLOW 0x01	//流量模式
#define MODE_BUYOUT 0x03  //买断模式
#define MODE_SETMEAL 0x04 //套餐模式
#define MODE_PUBLIC 0x05  //共享模式

/*
	设备模式
*/
#define STATUS_RECOVERY 0x00	   //出厂模式
#define STATUS_OPENDEVICE 0x01	 //开机模式
#define STATUS_ARREARAGE 0x02	  //欠费模式
#define STATUS_CLOSEDEVICE 0x04	//关机模式
#define STATUS_WAITACTIVATION 0x06 //待激活模式
#define STATUS_SENDDATAHIGH 0x08   //频发数据
#define STATUS_LOCKDEVICE 0x0C	 //锁定模式
#define STATUS_TDSPOLICE 0x21	  //TDS报警

/*
	EEPROM
*/
#define EEPROM_DeviceID 0x0001		  //设备ID
#define EEPROM_DeviceMode 0x0201	  //设备计费模式
#define EEPROM_Order 0x0401			  //命令
#define EEPROM_DeviceStatus 0x0601	//设备状态
#define EEPROM_DeviceFlow 0x0801	  //本次消费流量
#define EEPROM_RechargeFlow 0x0A01	//充值流量
#define EEPROM_RechargeDay 0x0C01	 //充值天数
#define EEPROM_SurplusFlow 0x0E01	 //剩余流量
#define EEPROM_SurplusDay 0x1001	  //剩余天数
#define EEPROM_UsedFlow 0x1201		  //已使用流量
#define EEPROM_UsedDay 0x1401		  //已使用天数
#define EEPROM_WaterTDS 0x1601		  //纯水TDS值
#define EEPROM_GPRSSignal 0x1801	  //GPRS信号强度
#define EEPROM_ICCID 0x1A01			  //ICCID
#define EEPROM_Vavle 0x1C01			  //剩余脉冲（流量）
#define EEPROM_VavleALL 0x2001		  //总使用脉冲（流量）
#define EEPROM_OpenDeviceCount 0x1E01 //开机次数

#define ON 0x01
#define OFF 0x00

#define True 0x01
#define False 0x00

#endif
