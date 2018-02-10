#ifndef   _PUMP_H
#define   _PUMP_H

/********************************/
void pump_io();
void pwm_deal(u16 L_cycle,u16 H_cycle);

#define   PUMP_BIT      BIT2              //接于P2.2
#define   PUMP_DIR_IN   P2DIR&=~PUMP_BIT  //设置P2.2为输入
#define   PUMP_IES_ON   P2IES&=~PUMP_BIT   //P2.2设置为上升沿中断   气泵引脚定义
#define   PUMP_IE_ON    P2IE|=PUMP_BIT    //允许P2.2中断
#define   PUMP_IE_OFF   P2IE&=~PUMP_BIT   //禁止P2.2中断
#define   PUMP_IFG      P2IFG             //P2.2中断标志
#define   PUMP_IN       (P2IN & PUMP_BIT) //P2.2输入

#define   PUMP_OFF      P6OUT&=~BIT0    
#define   PUMP_ON       P6OUT|=BIT0
#define   PUMP_DIR_OUT  P6DIR|=BIT0      //设置P5.5为输出

#endif
