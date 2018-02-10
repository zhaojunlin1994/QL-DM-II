#ifndef __SYS_H
#define __SYS_H

#include "stm8s.h"
void int_clk();  //时钟初始化函数
void int_timer_a();   //Timer_A初始化
void int_timer_b();   //Timer_B初始化
void timer_jishi();   //倒计时函数
void MODE();

void RCC_init(void);
void TIM2_init();
void TIM3_init();
void TIM4_init();
void delay_ms(unsigned int ms);

void RE1107_init();
void reg_right();
void reg_left();
void selectPlate1(u8 sPlate);
void selectPlate2(u8 sPlate);
void selectPlate3(u8 sPlate);
void save_DGUS_data();
void read_DGUS(u8 addr1,u8 addr2,u8 num);
void Set_Appointment();
void runningStatus(u8 sta);
void keepTempDis(u8 kTempDis);
void StreamGatch();
void MakeGatch();
void defaultSet();
void open_plc_bit(u8 opb);
void close_plc_bit(u8 cpb);
void MCU_PLC_SCAN();
void clock(u8 t);
void defaultSet();
void PID_init();
void beep_init();
void beep(u8 count,u16 time);
/*--------------------------------------------------------------------------
**宏定义
--------------------------------------------------------------------------*/
#define T_TIME 60 /*10mm放蜡默认时间60*/
#define T_TIME_ADD 6 /*加1mm加放蜡时间6*/

//--------运行状态 --

#define STOP 1      //-    
#define MAKE 2      //-  
#define FUSION 3    //-    
#define PUT 4       //-   
#define OK 5        //-    
//--------------------



#endif