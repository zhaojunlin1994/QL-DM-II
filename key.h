#ifndef __KEY_H
#define __KEY_h

#include "stm8s.h"
extern u8 over;
extern u8 beep_flag;
extern u8 page;
extern u8 Mode_FLAG;
extern u8 SJ;
extern u8 QD;
extern u8 HOT;
extern u8 ON_FLAG;
extern u8 beeptime;
extern u8 point;
extern u8 pressure;
extern u8 CF;
extern u8 reg_flag;
extern u8 char_flag;
extern u8 buwei[];
extern u8 ON_flg,min,strength;//运行标志位，使用时间变量，强度档位
extern u8 time_strength_data[16];
extern u8 temp_stop_stare[16];
extern u8 rightFlag;
extern u8 leftFlag;
extern u8 reg_flag,min;
extern u16 temp_timer,temp,tempcount,pwm,Mode;
extern u8 setmin_flag,setmin_flag1,sure_flag,min_dec_flag,min_flag,state;//时间设置提示
void key_init(void);
void TR_init(void);
void key_right(void);
void key_left(void);
void key_start(void);
void key_return(void);
void key_choose(void);
void key_meun(void);
void key_ok(void);

void reg_right(void);
void reg_left(void);
void key_reg(void);
//void beep(void);
void key_scan(void);

#endif