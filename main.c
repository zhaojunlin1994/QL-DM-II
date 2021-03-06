/**************包含头文件***************/
/***************************************
VER:V1.5.9
DATE:2017-3-2
修改日期:2017-10-10
NOTE:
显示屏，六个轻触按键，一个旋转编码器，带测速装置；
PD0 电机测速采集端口
PD2 测速使能端口
PC1-PC6 功能按键
PE6 PE7 旋转编码器输入口
PB1 电机输出控制端口
PB2 同步信号输入口
型号区分:(型号互换)
DM-I为12.1电阻式触摸屏，无轻触按键
DM-II为5寸非触摸屏，有轻触按键
1:修改电机堵转检测
2:优化电机软启动
3:添加PID参数调试
4:转速采样频率：100HZ
5:添加蜂鸣器驱动（分别触摸屏响应，计时到响应和故障响应）
6:需添加轻触按键响应方式
7:优化串口接收掉包问题
8:优化5寸屏程序
9:
2017-5-9:       发展会  5寸
2017-5-15问题：
显示屏触控掉包
电机忽快忽慢

2017-5-18
优化定时器2 ，开始前不打开
串口接收优化
再做一个页面，运行状态下删掉其他触控

2017-5-20
优化定时器,定时器1替换定时器2
DM-II波特率修改为9600,需验证型号兼容

2017-5-23
波特率修改为：5寸屏115200，12.1寸屏9600

2017-9-14
优化显示屏掉线后重新上电通信功能

2017-9-22
优化提示功能

2017-9-23
时间设置提示
强度设置提示
增大3档强度
修改串口发送数据方式

2017-9-25
优化旋转编码器造成显示屏闪屏

2017-10-10
5寸显示屏提示设置

2017-11-08
优化光电编码器操作
***************************************/ 
#include "stm8s.h"
#include "sys.h"
#include "key.h"
#include "UART1.h"
#include "PUMP.H"
#include "Speed_test.h"
extern u8 time_strength_data[];
void main(void)
{   
  RCC_init();//时钟初始化                     
  USART2_Initial();//串口初始化
  pump_io();//同步信号输入端口初始化
  RE1107_init();//旋转编码器初始化
  TIM2_init();//定时器2初始化
  TIM3_init();//定时器3初始化
#if defined( DM_II )
  key_init();
#endif
  beep_init();
  //SPEED_IO_INIT();
  // PID_init();
  beep(3,2);
  delay_ms(500);
  beep(3,2);
  delay_ms(500);
  beep(3,2); 
  enableInterrupts();//开总中断  
  while(1)
  { 
    ;
  }
}
#ifdef USE_FULL_ASSERT
void assert_failed(u8* file, u32 line)
{
  while (1)
  {
    ;
  }
}
#endif