#ifndef   _PUMP_H
#define   _PUMP_H

/********************************/
void pump_io();
void pwm_deal(u16 L_cycle,u16 H_cycle);

#define   PUMP_BIT      BIT2              //����P2.2
#define   PUMP_DIR_IN   P2DIR&=~PUMP_BIT  //����P2.2Ϊ����
#define   PUMP_IES_ON   P2IES&=~PUMP_BIT   //P2.2����Ϊ�������ж�   �������Ŷ���
#define   PUMP_IE_ON    P2IE|=PUMP_BIT    //����P2.2�ж�
#define   PUMP_IE_OFF   P2IE&=~PUMP_BIT   //��ֹP2.2�ж�
#define   PUMP_IFG      P2IFG             //P2.2�жϱ�־
#define   PUMP_IN       (P2IN & PUMP_BIT) //P2.2����

#define   PUMP_OFF      P6OUT&=~BIT0    
#define   PUMP_ON       P6OUT|=BIT0
#define   PUMP_DIR_OUT  P6DIR|=BIT0      //����P5.5Ϊ���

#endif