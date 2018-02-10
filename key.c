#include "key.h"
#include "sys.h"
#include "uart1.h"
static u8 key_flg;

u8 buwei_flag=0,buwei_count=0;

u8 moshi[8]={0x5a,0xa5,0x05,0x82,0x00,0x02,0x00,0x00};//模式

u8 Time[8]={0x5a,0xa5,0x05,0x82,0x00,0x0A,0x00,0x00};//时间
u8 Run[16]={0x5a,0xa5,0x05,0x82,0x00,0x04,0x00,0x00,0x5a,0xa5,0x05,0x82,0x00,0x0D,0x00,0x00};//运行状态,qiangdu
//u8 qiangdu[8]={0x5a,0xa5,0x05,0x82,0x00,0x0D,0x00,0x00};//强度
u8 temp_stop_stare1[16]={0x5a,0xa5,0x05,0x82,0x00,0x02,0x00,0x00,0x5a,0xa5,0x05,0x82,0x00,0x04,0x00,0x00};
/*****************************
函数名称：key_init()
返回值：无
功能：按键初始化，端口硬件上拉，
IO定义：PC1-PC6
*****************************/
void key_init()
{
 // EXTI_DeInit();//外部中断寄存器复位
  GPIO_Init(GPIOC, GPIO_PIN_1, GPIO_MODE_IN_PU_IT);//上拉，输入有中断
  GPIO_Init(GPIOC, GPIO_PIN_2, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_3, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_4, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_5, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOC, GPIO_PIN_6, GPIO_MODE_IN_PU_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOC, EXTI_SENSITIVITY_FALL_ONLY);//下降沿触发
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
      case 1: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_1)!=0) //自动/手动键
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
      case 2: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_2)!=0) //启动键
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
      case 3: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_3)!=0) //时间+键
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
              if((setmin_flag==1)&&(setmin_flag1==1))//确定功能
              {
                setmin_flag=0;setmin_flag1=0;sure_flag=1;min_dec_flag=0;    
                min_flag=1;min_dec_flag=1;
              }
              if(sure_flag==1)//确定
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
      case 4: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_4)!=0) //部位键
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
      case 5: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_5)!=0) //停止键
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
      case 6: if((GPIO_ReadInputData(GPIOC)&GPIO_PIN_6)!=0) //时间-键
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
            //DGUSChangePage(0); //取消功能
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
中断服务函数
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


