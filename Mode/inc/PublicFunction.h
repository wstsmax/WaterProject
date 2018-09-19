#ifndef __PUBLICFUNCTION_H__
#define __PUBLICFUNCTION_H__

extern void Valve_Switch(int Switch); //阀门开启开关

//进制转换函数
extern int Get_Hvalue(int Check);                                                    //获取十进制数转换为十六进制数后高八位
extern int Get_Lvalue(int Check);                                                    //获取十进制数转换为十六进制数后低八位
extern int Get_Long_H(unsigned long Check);                                          //获取长整型十进制数转换为十六进制高八位
extern int Get_Long_M1(unsigned long Check);                                         //获取长整型十进制数转换为十六进制中高八位
extern int Get_Long_M2(unsigned long Check);                                         //获取长整型十进制数转换为十六进制中低八位
extern int Get_Long_L(unsigned long Check);                                          //获取长整型十进制数转换为十六进制低八位
extern unsigned int Get_HLvalue(unsigned char Hvalue, unsigned char Lvalue);         //将高位十六进制和低位十六进制转换为十进制
extern unsigned long Get_HMLvalue(int Hvalue, int M1value, int M2value, int Lvalue); //将高位，中高位，中低位，低位十六进制转换为十进制
extern void Get_Check(int *HCheck, int *LCheck);                                     //计算校验位
extern int Char_to_Int(char ch);                                                     //将字符型转化为整型
extern int Hex_to_Int(char *hex);                                                    //将十六进制字符串转换为整型
//单位转换函数
extern unsigned long Flow_to_Valve_FlowMode(void);   //将接收到的流量转化为脉冲数
extern unsigned long Flow_to_Valve_PublicMode(void); //将接收到的流量转化为脉冲数
extern void Valve_to_Flow(void);                     //将脉冲数转化为上传的流量数
//数据包处理函数
extern void Recovery_PacketData(void);   //将数据包恢复出厂设置
extern void SystemInit_PacketData(void); //将数据包初始化设置

#endif
