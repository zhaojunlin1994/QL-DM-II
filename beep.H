#ifndef   _beep_H
#define   _beep_H
/*************************函数声明*************************/
void BEEP_IO_INIT();                     //蜂鸣器IO初始化函数
void beep(u8 count,u8 time);      //蜂鸣器服务子函数
void beep_dispose();                    //蜂鸣器鸣响处理函数，可放于中断内周期性调用

#endif