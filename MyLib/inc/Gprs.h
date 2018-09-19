#ifndef __GPRS_H__
#define __GPRS_H__

extern int RECV_Return(char *Recv1, char *Recv2);                //判断串口接收AT指令的回显数据是否正确数据
extern int RECV_Return_Special(char *Recv1, char *Recv2);        //判断串口接受数据包时是否有AT指令存在
extern void GPRS_Init(void);                                     //初始化GPRS模块
extern void GPRS_Mode(void);                                     //初始化GPRS通讯模式
extern void GPRS_ConnectCreat(char *ServerIP, char *ServerPort); //GPRS模块以TCP方式连接服务器
extern void GPRS_SendHeartPacket(void);                          //GPRS模块向服务器上传心跳包
extern void GPRS_SendData(char Command, char Mode, char Status); //GPRS模块向服务器上传数据
extern void GPRS_ConnectClose(void);                             //关闭与服务器的连接
extern void GPRS_Signal(void);                                   //获取GPRS信号强度
extern void GPRS_ICCID(void);                                    //获取SIM卡的ICCID号

#endif
