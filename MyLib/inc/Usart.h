#ifndef __USART_H__
#define __USART_H__

extern void Uart1_Init(void);                    //初始化串口1
extern void Uart1_Send(unsigned char Buf);       //串口1发送一个字符
extern void Uart1_Send_Data(unsigned char *Buf); //串口1发送一个字符串
extern int Uart1_Recv_NUM_AT(void);              //串口1接收AT指令大小是否正确
extern int Uart1_Recv_NUM_Packet(void);          //串口1接收数据包大小是否正确
extern void Uart1_Recv_Deal(void);               //串口1接收数据处理
extern void Clear_Buf(char *Rx_Buffer);          //清空串口接收缓冲区
extern void Uart1_Isr(void);                     //串口1中断处理函数

#endif
