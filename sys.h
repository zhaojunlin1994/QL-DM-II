#ifndef __SYS_H
#define __SYS_H

#include "stm8s.h"
void int_clk();  //ʱ�ӳ�ʼ������
void int_timer_a();   //Timer_A��ʼ��
void int_timer_b();   //Timer_B��ʼ��
void timer_jishi();   //����ʱ����
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
**�궨��
--------------------------------------------------------------------------*/
#define T_TIME 60 /*10mm����Ĭ��ʱ��60*/
#define T_TIME_ADD 6 /*��1mm�ӷ���ʱ��6*/

//--------����״̬ --

#define STOP 1      //-    
#define MAKE 2      //-  
#define FUSION 3    //-    
#define PUT 4       //-   
#define OK 5        //-    
//--------------------



#endif