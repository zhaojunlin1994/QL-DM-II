#include "stm8s.h"
#include "beep.H"
#include "sys.h"
/*****************************蜂鸣器函数******************************/
#define BEEP_L         P6OUT&=~BIT6    
#define BEEP_H         P6OUT|=BIT6 
#define BEEP_DIR_OUT   P6DIR|=BIT6 
/**********************蜂鸣器IO初始化函数*****************************/
u8 beep_timer=0;
void BEEP_IO_INIT()
{
  BEEP_DIR_OUT;         //设置蜂鸣器IO为输出方向
  BEEP_L;
}
/******************************************************************************
* 函数名称: beep()
* 输入参数: uchar count,uchar time         
* 功    能: 蜂鸣器
* 作    者: 2013/9/29,邓小浪
*******************************************************************************/
void beep(uchar count,uint time)
{
  if(count==1)
  {
    beep_timer=time;      //设置鸣响结束时间
    BEEP_H;               //开启蜂鸣器       
  }
  if(count>1)
  {
    for(int i = 0; i < count; ++i)
    {
      BEEP_H;
      delay_ms(100);
      BEEP_L;
      delay_ms(100);        
    }   
  }
}
/******************************************************************************
* 函数名称: beep_dispose()
* 输入参数: 
* 功    能: 蜂鸣器鸣响处理函数，可放于中断内周期性调用
* 作    者: 2013/9/29,邓小浪
*******************************************************************************/
void beep_dispose()
{
  if(beep_timer==0)
  {
    BEEP_L;     //若鸣响时间到，关闭蜂鸣器
  }
  else
  {
    if(beep_timer>0)
    {
      beep_timer--;    
    }
  }
}
