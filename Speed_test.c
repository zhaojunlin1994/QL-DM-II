#include "Speed_test.h"
#include "sys.h"
#include "uart1.h"
#include "key.h"
u16 speed_count=0;
/********************
Note:PC6端口测速使能（低电平使能），PC7为测速输入端口
Version：硬件版本V1.1
Date：2017-3-9

********************/
void SPEED_IO_INIT()
{
  // GPIO_DeInit(GPIOD);
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
  // GPIO_WriteHigh(GPIOC, GPIO_PIN_6);//测速使能，默认关测速
  GPIO_WriteLow(GPIOD, GPIO_PIN_2);//测速使能，开测速
  GPIO_Init(GPIOD, GPIO_PIN_0, GPIO_MODE_IN_PU_IT);
  //EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_LOW);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);
}
void speed_test()
{
  if(ON_flg==1)//运行状态
  {
    speed_count=speed_count+1;
  }
  else
  {
    GPIO_WriteHigh(GPIOC, GPIO_PIN_6);
  }
}
/*******************
中断服务函数
PD口中断，下降沿和低电平触发
速度测试
Date：2017-4-13
********************/
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  speed_test();
}


