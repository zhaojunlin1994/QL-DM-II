#include "Speed_test.h"
#include "sys.h"
#include "uart1.h"
#include "key.h"
u16 speed_count=0;
/********************
Note:PC6�˿ڲ���ʹ�ܣ��͵�ƽʹ�ܣ���PC7Ϊ��������˿�
Version��Ӳ���汾V1.1
Date��2017-3-9

********************/
void SPEED_IO_INIT()
{
  // GPIO_DeInit(GPIOD);
  GPIO_Init(GPIOD, GPIO_PIN_2, GPIO_MODE_OUT_PP_LOW_FAST);
  // GPIO_WriteHigh(GPIOC, GPIO_PIN_6);//����ʹ�ܣ�Ĭ�Ϲز���
  GPIO_WriteLow(GPIOD, GPIO_PIN_2);//����ʹ�ܣ�������
  GPIO_Init(GPIOD, GPIO_PIN_0, GPIO_MODE_IN_PU_IT);
  //EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_LOW);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);
}
void speed_test()
{
  if(ON_flg==1)//����״̬
  {
    speed_count=speed_count+1;
  }
  else
  {
    GPIO_WriteHigh(GPIOC, GPIO_PIN_6);
  }
}
/*******************
�жϷ�����
PD���жϣ��½��غ͵͵�ƽ����
�ٶȲ���
Date��2017-4-13
********************/
INTERRUPT_HANDLER(EXTI_PORTD_IRQHandler, 6)
{
  speed_test();
}


