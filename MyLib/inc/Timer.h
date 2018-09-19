#ifndef __TIMER_H__
#define __TIMER_H__

extern void TIM0_Init(void); //初始化定时器0为计数功能
extern void TIM2_Init(void); //初始化定时器2为定时用户开始用水标志
extern void TIM3_Init(void); //初始化定时器3为定时用户结束用水标志
extern void TIM4_Init(void); //初始化定时器3为心跳包标志

extern void TIM0_Isr(void); //定时器0中断处理函数
extern void TIM2_Isr(void); //定时器2中断处理函数
extern void TIM3_Isr(void); //定时器3中断处理函数
extern void TIM4_Isr(void); //定时器4中断处理函数

#endif
