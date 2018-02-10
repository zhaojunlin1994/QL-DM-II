#include "key.h"
#include "sys.h"
#include "uart1.h"
static u8 key_flg;

u8 buwei_flag=0,buwei_count=0;

u8 moshi[8]={0x5a,0xa5,0x05,0x82,0x00,0x02,0x00,0x00};//ģʽ

u8 Time[8]={0x5a,0xa5,0x05,0x82,0x00,0x0A,0x00,0x00};//ʱ��
u8 Run[16]={0x5a,0xa5,0x05,0x82,0x00,0x04,0x00,0x00,0x5a,0xa5,0x05,0x82,0x00,0x0D,0x00,0x00};//����״̬,qiangdu
//u8 qiangdu[8]={0x5a,0xa5,0x05,0x82,0x00,0x0D,0x00,0x00};//ǿ��
u8 temp_stop_stare1[16]={0x5a,0xa5,0x05,0x82,0x00,0x02,0x00,0x00,0x5a,0xa5,0x05,0x82,0x00,0x04,0x00,0x00};
/*****************************
�������ƣ�key_init()
����ֵ����
���ܣ�������ʼ�����˿�Ӳ��������
IO���壺PC1-PC6
*****************************/
void key_init()
{
 // EXTI_DeInit();//�ⲿ�жϼĴ�����λ
  GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_PU_IT);//�������������ж�
  GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_ONLY);//�½��ش���
}

void key_scan(void)
{
  static u8 key_timer=0,mode_count=0;
  if(key_flg!=0)
  {
    key_timer++;
    if(key_timer>=2)
    {
      key_timer=0;
      switch (key_flg)
      {
      case 1: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_1)!=0) //�Զ�/�ֶ���
      { 
        buwei_flag=0;
        if((ON_flg==0)&&(setmin_flag==0))
        {
          mode_count++;
          if(mode_count==1)
          {
            Mode=1;
            moshi[7]=0x00; 
          }
          if(mode_count>=2)
          {
            mode_count=0;Mode=2;
            moshi[7]=0x01;
          }
          UART2_PutFrame(moshi,0,8);
          beep(1,8);
        }
        key_flg=0;
      }break;
      case 2: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_2)!=0) //������
      {    
        if((ON_flg==0)&&(setmin_flag==0))
      {
        buwei_flag=0;
        Run[7]=0x01;
        ON_flg=1; 
        strength=1;
        Run[15]=0x0a;
        UART2_PutFrame(Run,0,16);
        beep(1,8);
      }
        key_flg=0;
      }break;        
      case 3: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_3)!=0) //ʱ��+��
      { 
        buwei_flag=0;
        if(ON_flg==0)
        { 
          if(min<12)
          {
            if(min<8) min++;
            if(min>=8)
            {  
              // buwei[6]=0x00;buwei_count=0;
              if((setmin_flag==1)&&(setmin_flag1==1))//ȷ������
              {
                setmin_flag=0;setmin_flag1=0;sure_flag=1;min_dec_flag=0;    
                min_flag=1;min_dec_flag=1;
              }
              if(sure_flag==1)//ȷ��
              {
                min++;
                if(min==9)
                {
                  buwei_count=0;buwei[6]=0x00;
                  setmin_flag=0;setmin_flag1=0;
                }
                //sure_flag=0;
                state=0;
              }
              if(sure_flag==0)
              {
                setmin_flag=1;setmin_flag1=0;
              }
            }
            if(min>=12)min=12;  
            time_strength_data[4]=0x00;time_strength_data[5]=0x0A;
            time_strength_data[7]=min;
            UART2_PutFrame(time_strength_data,0,16);
            beep(1,10); 
          }
        }
        key_flg=0;
      }break;
      case 4: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_4)!=0) //��λ��
      { 
        if(setmin_flag==0)
        {
        buwei_flag=1;
        buwei_count++;
        if(buwei_count>=6)buwei_count=0;
        buwei[6]=buwei_count;
        UART2_PutFrame(buwei,0,7);
        beep(1,8);
        }
        key_flg=0;
      }break;
      case 5: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_5)!=0) //ֹͣ��
      { 
        if((ON_flg==1)&&(setmin_flag==0))
        {
          buwei_flag=0;
          Run[7]=0x00;
          ON_flg=0;
          strength=0;
          Run[15]=0x00;
          min=5;
          temp_timer=0;temp=88;pwm=88;tempcount=0;
          UART2_PutFrame(Run,0,16);
          beep(1,8);
        }
        key_flg=0;
      }break;
      case 6: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_6)!=0) //ʱ��-��
      { 
        buwei_flag=0;
        if((ON_flg==0)&&(min>1))
        {
          if(min>1)
          { 
           if(setmin_flag==0) min--;
            if(min<=8)
            {
              min_flag=0;
              sure_flag=0;
              if(setmin_flag==1)
              {
                setmin_flag=0;setmin_flag1=0;
                sure_flag=0;
              }
            }
            beep(1,10);
          }
          time_strength_data[4]=0x00;time_strength_data[5]=0x0A;
          time_strength_data[7]=min;
          UART2_PutFrame(time_strength_data,0,16);
          beep(1,8);
          if(setmin_flag==1)
          {
            //DGUSChangePage(0); //ȡ������
            buwei_count=0;buwei[6]=0x00;
            setmin_flag=0;setmin_flag1=1;min_dec_flag=0;
          }
        }
        key_flg=0;
      }break;         
      }    
    }
  }
}

/****************** 
�жϷ�����
******************/
INTERRUPT_HANDLER(EXTI_PORTC_IRQHandler, 5)
{
  if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_1)==0)
  {
    if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_1)==0)
    {
      key_flg=1;
    }
  }
  if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_2)==0)
  {
    if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_2)==0)
    {
      key_flg=2;
    }
  }
  if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_3)==0)
  {
    if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_3)==0)
    {
      key_flg=3;
    }
  }
  if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_4)==0)
  {
    if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_4)==0)
    {
      key_flg=4;
    }
  }
  if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_5)==0)
  {
    if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_5)==0)
    {
      key_flg=5;
    }
  }
  if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_6)==0)
  {
    if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_6)==0)
    {
      key_flg=6;
    }
  }
}


