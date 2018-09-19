#ifndef __IODEAL_H__
#define __IODEAL_H__

extern void Valve_Init(void);  //将P2.4，P2.5设置为推挽输出
extern void Valve_Open(void);  //打开脉冲阀
extern void Valve_Close(void); //关闭脉冲阀

#endif