/**************************************
Date:2017-03-01
Note:
PB1,同步信号输入口
PB2,电机控制端口
PE5，顺时针方向输入口
PE6，逆时针方向输入口
**************************************/
#include "sys.h"
//#include "uart1.h
#include "UART1.h"
#include "pump.H"
#include "key.h"
//#include "Speed_test.h"
#define strength_max   85  //软启动值，与过零信号相关
#define strength_value 66//73  //强度设置值，值越小，输出越大
//pwm=strength_value-(strength-1);//*1;
u8 ON_flg=0,min=5,strength=0;//运行标志位，使用时间变量，强度档位
u8 buwei[7]={0x5a,0xa5,0x04,0x80,0x03,0x00,0x00};
u8 time_strength_data[16]={0x5a,0xa5,0x05,0x82,0x20,0x02,0x00,0x00,0x5a,0xa5,0x05,0x82,0x20,0x00,0x00,0x00};
u8 temp_stop_stare[16]={0x5a,0xa5,0x05,0x82,0x00,0x14,0x00,0x00,0x5a,0xa5,0x05,0x82,0x00,0x12,0x00,0x00};
u8 rightFlag=0,leftFlag=0;//旋转编码器标志位
u8 reg_flag=0;
u16 beep_timer=0;
u8 pump_count=0,pump2_flg=0;
u8 strength_flag=0,min_flag=0;//限制档位提示
u8 setmin_flag=0,setmin_flag1=0,sure_flag=0,min_dec_flag=0,state=0;//时间设置提示
//u16 speed_temp=0,speed_circle=0,rotate_speed=0,qianbai=0,shige=0;   //脉冲计数，圈数，测试转速，显示千位百位，显示十位个位
u16 temp_timer=0,temp=85,tempcount=0,pwm=80,Mode=1;// 电机软启动计时位，软启动电机速度控制 ，软启动计时，运行速度控制位，模式区分
int Bias,Pwm,Last_bias;
extern u8 FrameBuff[],buwei_count;
extern u8 FrameBuff_flg;
extern u8 pump_flg,buwei_flag;
extern u16 qiangdu[];

/*系统时钟配置函数，选择外部高速时钟，分频为1*/
void RCC_init(void)
{ 
  CLK_DeInit();
  CLK_HSECmd(ENABLE);//外部时钟开  
  CLK_LSICmd(DISABLE);//内部低频RC关  
  CLK_HSICmd(DISABLE);//内部高频RC关 
  CLK_ClockSwitchConfig(CLK_SWITCHMODE_AUTO ,         //自动切换
                        CLK_SOURCE_HSE,               //新的时钟源：外部高速时钟
                        DISABLE,                      //关闭切换中断
                        CLK_CURRENTCLOCKSTATE_DISABLE);//切换后旧时钟源关闭
  CLK_SYSCLKConfig(CLK_PRESCALER_CPUDIV1);//CPU时钟1分频
}
/*TIM2定时器初始化*/
void TIM2_init()
{
  TIM1_DeInit();
  TIM1_TimeBaseInit(0,TIM1_COUNTERMODE_UP,1600,0);
  //TIM1_TimeBaseInit(TIM1_PRESCALER_16,100);// 100us
  TIM1_ITConfig(TIM1_IT_UPDATE, ENABLE);
  TIM1_SetCounter(0x0000);
#if defined( DM_I )//12.1寸显示屏
  // TIM1_Cmd(DISABLE);
  TIM1_Cmd(ENABLE);
#endif
#if defined( DM_II )//12.1寸显示屏
  TIM1_Cmd(ENABLE);
#endif
}
/*定时器初始化*/
void TIM3_init()
{
  TIM3_DeInit();
  TIM3_TimeBaseInit(TIM3_PRESCALER_16,5000);//5ms
  TIM3_ITConfig(TIM3_IT_UPDATE, ENABLE);
  TIM3_SetCounter(0x0000);
  TIM3_Cmd(ENABLE);
}

void RE1107_init()
{ 
  GPIO_DeInit(GPIOE);
  GPIO_Init(GPIOE, GPIO_PIN_6, GPIO_MODE_IN_PU_IT);
  GPIO_Init(GPIOE, GPIO_PIN_7, GPIO_MODE_IN_PU_IT);
  EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOE, EXTI_SENSITIVITY_FALL_ONLY);//下降沿中断
}

void beep_init()
{ 
  GPIO_DeInit(GPIOD);
  GPIO_Init(GPIOD, GPIO_PIN_7, GPIO_MODE_OUT_PP_LOW_FAST);
  GPIO_WriteLow(GPIOD, GPIO_PIN_7);
}
/*延时*/
void delay_ms(unsigned int ms) 
{  
  while(ms--) 
  { 
    u16 ti=0;
    for(ti=0;ti<500;ti++)
    {
      ;
    }
  } 
} 
void DGUSChangePage(u8 num)
{
  u8 page[7]={0x5a,0xa5,0x04,0x80,0x03,0x00,0x00};
  page[6]=num;
  //UART2SendString(page,7);
  UART2_PutFrame(page,0,7); 
}

#if defined( DM_I )//12.1寸显示屏
/***************************
函数名称：deal_uart_data()
函数功能：串口接收指令数据处理
适用型号：DM-I
返回值：无
***************************/
void deal_uart_data()
{
  u8 temp_data[9]={0};
  if(FrameBuff_flg==1)
  {
    for(u8 i=0;i<9;i++)
    {
      temp_data[i]=FrameBuff[i];
      FrameBuff[i]=0;
    }
    if(temp_data[0]==0x5a)
    {
      switch(temp_data[5])
      {
      case 1://手动
         if((temp_data[7]==0x7e)&&(temp_data[8]==0x60)&&(ON_flg==0))          
        {
          temp_stop_stare[5]=0x10;
          temp_stop_stare[13]=0x11;
          temp_stop_stare[15]=0x00;
          temp_stop_stare[7]=0x01;
          UART2_PutFrame(temp_stop_stare,0,16);
          Mode=1;//手动模式标志位
          beep(1,10);
        } 
        break;  
        
      case 2://自动
        if((temp_data[7]==0x21)&&(temp_data[8]==0x31)&&(ON_flg==0))          
        {
          Mode=2;//自动模式标志位
          temp_stop_stare[5]=0x10;
          temp_stop_stare[13]=0x11;
          temp_stop_stare[15]=0x01;
          temp_stop_stare[7]=0x00;
          UART2_PutFrame(temp_stop_stare,0,16);
          beep(1,10);
        } 
        break;
        
      case 3://启动
        if((temp_data[7]==0x40)&&(temp_data[8]==0x32)&&(ON_flg==0))          
        { 
          //TIM2_Cmd(ENABLE);
          ON_flg=1; 
          strength=1;
          temp_stop_stare[5]=0x14;
          temp_stop_stare[13]=0x12;
          temp_stop_stare[15]=0x01;
          temp_stop_stare[7]=0x00;
          UART2_PutFrame(temp_stop_stare,0,16);
          beep(1,10);
        }
        break;
      case 4://停止
        if((temp_data[7]==0x23)&&(temp_data[8]==0x33)&&(ON_flg==1))          
        {
         // TIM2_Cmd(DISABLE);
          ON_flg=0;
          strength=0;
          min=5;
          temp_timer=0;temp=85;pwm=85;tempcount=0;
          temp_stop_stare[5]=0x14;
          temp_stop_stare[13]=0x12;//12
          temp_stop_stare[15]=0x00;
          temp_stop_stare[7]=0x01;
          UART2_PutFrame(temp_stop_stare,0,16);
          time_strength_data[7]=0x05;
          UART2_PutFrame(temp_stop_stare,0,16);
          beep(1,10);

          min_flag=0;
          setmin_flag=0;
          setmin_flag1=0;
          sure_flag=0;
        }  
        break;
      case 5://强度-
        if((temp_data[7]==0x24)&&(temp_data[8]==0x34)&&(ON_flg==1))          
        {  
          if((strength>1)&&(Mode==1))//手动模式
          {
            {
              strength--;
              if(min_flag==0)strength_flag=0;
              beep(1,10); 
            }    
          }
        }
          break;
      case 6://强度+
        if((temp_data[7]==0x5e)&&(temp_data[8]==0x36)&&(ON_flg==1))          
        {
          if((strength<=9)&&(Mode==1))
          {
            strength++;
            if(min_flag==1)
            {
              if(strength>=7)
              {
                strength=7;
                strength_flag=1;
              }
            }
            beep(1,10);
          }   
        }
        break;
      case 7://时间-
        if((temp_data[7]==0x42)&&(temp_data[8]==0x62)&&(ON_flg==0))          
        {
          if(min>1)
          { 
            min--;
            if(min<=8)
            {
              min_flag=0;
              if((setmin_flag==0)&&(setmin_flag1==1))
              {
                setmin_flag=2;
                setmin_flag1=2;
              }
//              if((setmin_flag==0)&&(setmin_flag1==2))
//              {
//                setmin_flag=2;
//                setmin_flag1=2;
//              }
//              else
//              {
//                setmin_flag=0;
//                setmin_flag1=0;
//              }
              sure_flag=0;
            }
            beep(1,10);
          }
        }
        break;
      case 8://时间+
        if((temp_data[7]==0x26)&&(temp_data[8]==0x37)&&(ON_flg==0))          
        {     
          if(min<12)
          {
            if(min<8) min++;
            if(min>=8)
            {  
              min_flag=1;min_dec_flag=1;
              setmin_flag=1;setmin_flag1=0;
              if(sure_flag==1)
              {
                min++;setmin_flag=0;setmin_flag1=0;
              }
            }
            beep(1,10);
          }
        }
        break;
      case 10://确定
        if((temp_data[7]==0x23)&&(temp_data[8]==0x34)&&(ON_flg==0))          
        {     
          setmin_flag=0;setmin_flag1=0;sure_flag=1;min_dec_flag=0;state=0;
        }
        break;
      case 11://取消
        if((temp_data[7]==0x23)&&(temp_data[8]==0x35)&&(ON_flg==0))          
        {     
          DGUSChangePage(0); 
          setmin_flag=0;setmin_flag1=1;min_dec_flag=0;state=0;
        }
        break; 
      default:
        FrameBuff_flg=0;
        for(u8 i=0;i<9;i++)
        {
          temp_data[i]=0;
        }
        break;
      }     
    }
    FrameBuff_flg=0;
    for(u8 i=0;i<9;i++)
    {
      temp_data[i]=0;
    }
  }
}
#endif
/******************************************************************************
* 函数名称: therapy_time_C_down()
* 输入参数: uint cycle
* 功    能: 治疗时间倒计时
* 作    者: 2013/10/2,邓小浪
*******************************************************************************/
void therapy_time_C_down(u16 cycle)
{
  static u16 therapy_timer=0;   //治疗时间计时
  if(ON_flg!=0)
  {
    therapy_timer++;
    if(therapy_timer>=cycle)
    {   
      therapy_timer=0;
      min--;
    } 
    if(min<=0)
    {
      ON_flg=0;
      strength=0;
      min=5;
      temp_timer=0;temp=strength_max;pwm=strength_max;tempcount=0;
      temp_stop_stare[5]=0x14;
      temp_stop_stare[13]=0x12;
      temp_stop_stare[15]=0x00;
      temp_stop_stare[7]=0x01;
      UART2_PutFrame(temp_stop_stare,0,16);
      time_strength_data[7]=0x05;
      UART2_PutFrame(temp_stop_stare,0,16);
      beep(1,300);
      min_flag=0;
      setmin_flag=0;
      setmin_flag1=0;
      sure_flag=0;
    }
  }
  else
  {
    therapy_timer=0;
  }
}
/********************
函数：void speed(u16 cycle)
功能：计算每分钟转速
date：2017-3-10
扫描周期：10ms
********************/
//void speed(u16 cycle)
//{
//  static u16 count=0;
//  if(ON_flg!=0)
//  {
//    count++;
//    if(count>=cycle)
//    {   
//      count=0;
//      speed_temp=speed_count;
//      speed_count=0;
//    }
//    speed_temp=speed_temp*100;
//    speed_circle=speed_temp>>2;//圈数=脉冲数除以4，四个孔
//   // speed_circle=((speed_circle<<6)+(speed_circle<<5)+(speed_circle<<2));//10ms*100=1000ms=1S
//    rotate_speed=(speed_circle<<6)-(speed_circle<<2);//乘以60=1min的转数=转速RPM
//    rotate_speed=rotate_speed>>1;//转速  
//    /*显示数据处理*/
////    qianbai=(rotate_speed<<7)-(rotate_speed<<5)+(rotate_speed<<2);
////    qianbai=(rotate_speed/100);
////    shige=(rotate_speed%100);
////    time_strength_data[7]=qianbai;time_strength_data[15]=shige;
////    UART2_PutFrame(time_strength_data,0,16);  
//  }
//}
/*********************************
函数名称：Diapaly()
功能：显示更新函数，时间，强度值的显示值更新
型号：根据预编译可选
返回值：无
*********************************/
void Diapaly()
{
  static u8 temp_timer=0;
  static u8 temp_min=0;
  static u8 temp_strength=0;
  if((temp_min!=min)||(temp_strength!=strength))
  {
    temp_timer++;
    if(temp_timer>=5)
    {
      temp_timer=0;
#if defined( DM_I )//12.1寸显示屏 
      time_strength_data[7]=temp_min=min;
      time_strength_data[4]=0x00;time_strength_data[5]=0x18;//time
      time_strength_data[12]=0x00;time_strength_data[13]=0x16;//qiangdu
      time_strength_data[15]=temp_strength=strength;
      UART2_PutFrame(time_strength_data,0,16);  
#endif
#if defined( DM_II )//5寸显示屏
      time_strength_data[4]=0x00;time_strength_data[5]=0x0A;
      time_strength_data[7]=temp_min=min;
      time_strength_data[12]=0x00;time_strength_data[13]=0x0D;
      time_strength_data[15]=temp_strength=(strength<<3)+(strength<<1);     
      UART2_PutFrame(time_strength_data,0,16);  
#endif

      pwm=strength_value-((strength<<1)-1);//*1;
    }
  }
}
/************************************
函数名称：Auto_mode()
函数功能：自动模式下的参数切换，加到70%档位后强度自动减小
返回值：无
************************************/
void Auto_mode()
{
  static u16 countA=0,countB=0,flag=0;
  if((Mode==2)&&(ON_flg==1))//自动模式
  {
    countA++;
    if(countA>=200)//5秒-1000
    {
      countA=0;
      countB++;
      if(countB>=4)//20S
      {
        countB=0;
        if((strength<=6)&&(flag==0))
        {
          strength=strength+1;
          if(strength==7)
          {
            flag=1;
          }
        }
        if(flag==1)
        {
          strength=strength-1;
          if(strength<=2)
          {
            strength=2;flag=0;
          }
        } 
      }
    }
  }
}

/******************************************************************************
* 函数名称: beep()
* 输入参数: uchar count,uchar time         
* 功    能: 蜂鸣器
* 作    者: 2013/9/29,邓小浪
*******************************************************************************/
void beep(u8 count,u16 time)
{
  if(count==1)
  {
    beep_timer=time;//设置鸣响结束时间  
    GPIO_WriteHigh(GPIOD, GPIO_PIN_7);
  }
  if(count>1)
  {
    for(int i = 0; i < count; ++i)
    {
      GPIO_WriteHigh(GPIOD, GPIO_PIN_7);
      delay_ms(300);
      GPIO_WriteLow(GPIOD, GPIO_PIN_7);
      delay_ms(300);  
    }   
  }
  else
  {
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
   GPIO_WriteLow(GPIOD, GPIO_PIN_7);
  }
  else
  {
    if(beep_timer>0)
    {
      beep_timer--;    
    }
  }
}
/******************************************************
函数名称：TIM2定时中断
函数功能：1ms中断,计时
1：电机软启动
2：堵转及转速监测
返回值：无
初始日期：2015.12.12
修改日期：2017.3.30
******************************************************/
//#pragma vector=0x0F
//__interrupt void TIM2_UPD_OVF_BRK_IRQHandler(void) 
INTERRUPT_HANDLER(TIM1_UPD_OVF_TRG_BRK_IRQHandler, 11)
{
  TIM1_ClearFlag(TIM1_FLAG_UPDATE);                     //清除中断标志位
 //enableInterrupts();//开总中断 
  if(ON_flg==1)
  {  
    GPIO_WriteLow(GPIOC, GPIO_PIN_6);
    if(temp_timer<=50000)//电机软启动
    {
      temp_timer++;
      tempcount++;
      if(tempcount>=2000)//500ms
      {
        temp--;tempcount=0;//17020003-74,17020002-71,17020001-74
        if(temp<=66)temp=66;
      }
      pwm_deal(temp,85);
    }
    else
    {      
      pwm_deal(pwm,strength_max);
    }
  }
  else if(ON_flg==0)
  {
    GPIO_WriteLow(GPIOB, GPIO_PIN_1);
    temp_timer=0;
  }  
  // disableInterrupts();//开总中断 
}
/******************************************************
*@函数名称：TIM3定时中断
*@函数功能：5ms中断，处理数据
*@返回值：无
*@初始日期：2015.12.12
*@修改日期：2016.6.23
******************************************************/
INTERRUPT_HANDLER(TIM3_UPD_OVF_BRK_IRQHandler, 15)
{
  TIM3_ClearFlag(TIM3_FLAG_UPDATE);//清除中断标志位
  beep_dispose();
  therapy_time_C_down(12000);//时间更新函数
  Diapaly();//显示更新函数
#if defined( DM_I )//12.1寸显示屏
  deal_uart_data(); //串口处理函数
#endif
#if defined( DM_II )//5寸显示屏
  key_scan();//按键扫描
#endif 
  Auto_mode();//自动模式函数
    if(ON_flg==1)//旋转编码器
    {
      if(Mode==1)//手动模式
      {
        if(rightFlag==1)//顺时针
        {
          strength++;
          if(min_flag==1)
          {
            if(strength>=8)
            {
              strength=8;
              strength_flag=1;
            }
          }
          rightFlag=0;
          beep(1,8);
        }  
        if(leftFlag==1)//逆时针
        { 
          strength--;
          if(strength<2)strength=1;
          if(min_flag==0)strength_flag=0;
          leftFlag=0;
          beep(1,8);
        }
        if(strength>=10)//上限处理
        {
          strength=10;
        }
        if((min_flag==1)&&(strength>=7))
        {
          strength=7;
        }
        else
        {
        }
      }
    }
    if(ON_flg==0)//时间设置
    {
      // static u8 Flag1=0,Flag2=0;
      if((rightFlag==1)&&(leftFlag==0))//顺时针
      {
       // rightFlag=0; 
        if(min<13)
        {
          if((min<9)||((min>=8)&&(sure_flag==1)))
          {
            min=min+1;
          }
          if(min>=8)
          {  
            min_flag=1;min_dec_flag=1;
            setmin_flag=1;setmin_flag1=0;
            if(sure_flag==1)
            {
              setmin_flag=0;setmin_flag1=0;min_dec_flag=0;
            }
          }
          beep(1,10);
        } 
      }  
     if((leftFlag==1)&&(rightFlag==0))//&&(setmin_flag==0)&&(setmin_flag1==0)||((leftFlag==1)&&(setmin_flag==0)&&(setmin_flag1==1)))//逆时针
      { 
        if(min>1)//&&(min_dec_flag==0))
        {     
          min=min-1;
          if(min<=8)
          {
            min_flag=0;
            if((setmin_flag==0)&&(setmin_flag1==1))
            {
              setmin_flag=2;
              setmin_flag1=2;
            }
            sure_flag=0;
          }
          if(state==1)
          {
            setmin_flag=0;
            setmin_flag1=0;
          }
          beep(1,8);
        }    
        //leftFlag=0;    
      } 
      if((min>=8)&&(sure_flag==0))
      {
        min=8;
      }
      if(min>=12)//上限处理
      {
        min=12;
      }
      /*********/
      rightFlag=0; leftFlag=0; 
    }
  if(pump_flg==1)//判断过零标志位 
  {
    pump_count++;
    if(pump_count>=2)//10ms
    {
      pump_count=0;
      pump_flg=0;
      pump2_flg=1;
    }
  }
#if defined Safety_Tips//档位限制提示
  static u8 count1,count_flag;
  static u16 count;//档位限制计数值
  u8 page1[7]={0x5a,0xa5,0x04,0x80,0x03,0x00,0x00};
  if(ON_flg==1)
  { 
    count++;
    if(count>=400)//2s
    {
      count=0;
      if(count_flag==1)count1++;
      if(strength_flag==1)
      {
        strength_flag=0;
        #if defined( DM_I )//12.1寸显示屏
        page1[6]=0X0C;
        #endif
        
        #if defined( DM_II )//5寸显示屏
        page1[6]=0X06;
        #endif   
        
        UART2_PutFrame(page1,0,7); 
        count_flag=1;
      }
      if(count1>=2)//5s
      {
        count1=0;
        if(count_flag==1)//&&(strength_flag==0))
        {
          count_flag=0;count1=0;
          strength_flag=0;
          page1[6]=0X00;
          UART2_PutFrame(page1,0,7); 
          // DGUSChangePage(0);
        }
      }
    }
  }
#endif

#if defined Settime_Tips//时间设置提示 
  static u16 Settime_count;//,Settime_count1;//设置时间计数值
  u16 page[7]={0x5a,0xa5,0x04,0x80,0x03,0x00,0x00};
  if(ON_flg==0)
  { 
    Settime_count++;
    if(Settime_count>=100)
    {
      Settime_count=0;
      if((setmin_flag==1)&&(setmin_flag1==0))
      {
        setmin_flag1=1;
        #if defined( DM_I )//12.1寸显示屏
        page[6]=0X0D;
        #endif
        #if defined( DM_II )//5寸显示屏
        page[6]=0X07;
        #endif
        UART2SendString(page,7);
        state=1;
        // DGUSChangePage(13);
      }
      if((setmin_flag==0)&&(setmin_flag1==0))
      {
        buwei_count=0;buwei[6]=0x00;
        page[6]=0X00;
        UART2SendString(page,7);
        setmin_flag=0;setmin_flag1=1;
      }
    }
  }
#endif
  else
  {
    
  }
}
/*************************************
函数功能：中断服务函数
函数入口：PE6 PE7
*************************************/
INTERRUPT_HANDLER(EXTI_PORTE_IRQHandler, 7)
{
  if ((GPIO_ReadInputData(GPIOE) & GPIO_PIN_6) == 0x00)//&&((GPIO_ReadInputData(GPIOE) & GPIO_PIN_7) == 0x01))
  { 
    rightFlag=1;//右旋标志位
  }
  if ((GPIO_ReadInputData(GPIOE) & GPIO_PIN_7) == 0x00)//&&((GPIO_ReadInputData(GPIOE) & GPIO_PIN_7) == 0x00))
  { 
    leftFlag=1;//左旋标志位
  }
  else
  {
  }
}
