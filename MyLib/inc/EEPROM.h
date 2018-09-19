#ifndef __EEPROM_H__
#define __EEPROM_H__

extern void IapIdle();                      //关闭IAP（关闭EEPROM的读写功能）
extern char IapRead(int addr);              //从IAP指定地址读数据（从EEPROM指定地址读数据）
extern void IapProgram(int addr, char dat); //向IAP指定地址写数据（向EEPROM指定地址写数据）
extern void IapErase(int addr);             //清除IAP中指定地址数据（清除EEPROM指定地址数据）

#endif