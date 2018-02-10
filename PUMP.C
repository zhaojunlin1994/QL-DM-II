#include "stm8s.h"
#include "sys.h"
#include "pump.H"

/**********************************************************************/
u8 pump_flg=0,port_flag=0,pump3_flg=0;        //同步信号标志.
extern u8 ON_flg,pump_count,pump2_flg;
/*************************IO初始化*************************
备注：PB1为电机控制脚
      PB0为过零信号检测
*/
void pump_io()
{
  GPIO_DeInit(GPIOB);
  GPIO_Init(GPIOB, GPIO_PIN_1, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_WriteLow(GPIOB, GPIO_PIN_1);
  EXTI_DeInit();
  GPIO_Init(GPIOB, GPIO_PIN_2, GPIO_MODE_IN_PU_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_FALL_ONLY);
 //   EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOB, EXTI_SENSITIVITY_FALL_ONLY);
  
    //EXTI_SENSITIVITY_FALL_LOW 下降沿和低电平触发
  //EXTI_SENSITIVITY_RISE_ONLY 仅上升沿触发
  //EXTI_SENSITIVITY_FALL_ONLY 仅下降沿触发
//EXTI_SENSITIVITY_RISE_FALL 上升沿和下降沿触发
  
}
/******************************************************************************
* 函数名称: pump_deal()
* 输入参数: uchar L_cycle,uchar H_cycle
* 功    能: 
* 作    者: 2013/10/2,邓小浪
*******************************************************************************/
void pwm_deal(u16 L_cycle,u16 H_cycle)             //100us*cycle
{
  static u16 pwm_timer=0;
  if(pump3_flg==1)        //同步信号过零标志
  { 
    pwm_timer++;
    if(pwm_timer==L_cycle)
    {
      GPIO_WriteHigh(GPIOB, GPIO_PIN_1);
    }
    if(pwm_timer==H_cycle)
    {
      GPIO_WriteLow(GPIOB, GPIO_PIN_1);
      pwm_timer=0;
      pump3_flg=0;       //清除同步信号标志
    }
  }
  else
  {
    pwm_timer=0;
  }
}

INTERRUPT_HANDLER(EXTI_PORTB_IRQHandler, 4)
{
  port_flag=1;
  if(port_flag==1)
  {
    pump_flg=1;
    //port_flag=0;
  }
  if(pump2_flg==1)
  {
    pump3_flg=1;
    pump2_flg=0;
  }
}
