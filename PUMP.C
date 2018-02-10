#include "stm8s.h"
#include "sys.h"
#include "pump.H"

/**********************************************************************/
u8 pump_flg=0,port_flag=0,pump3_flg=0;        //ͬ���źű�־.
extern u8 ON_flg,pump_count,pump2_flg;
/*************************IO��ʼ��*************************
��ע��PB1Ϊ������ƽ�
      PB0Ϊ�����źż��
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
  
    //EXTI_SENSITIVITY_FALL_LOW �½��غ͵͵�ƽ����
  //EXTI_SENSITIVITY_RISE_ONLY �������ش���
  //EXTI_SENSITIVITY_FALL_ONLY ���½��ش���
//EXTI_SENSITIVITY_RISE_FALL �����غ��½��ش���
  
}
/******************************************************************************
* ��������: pump_deal()
* �������: uchar L_cycle,uchar H_cycle
* ��    ��: 
* ��    ��: 2013/10/2,��С��
*******************************************************************************/
void pwm_deal(u16 L_cycle,u16 H_cycle)             //100us*cycle
{
  static u16 pwm_timer=0;
  if(pump3_flg==1)        //ͬ���źŹ����־
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
      pump3_flg=0;       //���ͬ���źű�־
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