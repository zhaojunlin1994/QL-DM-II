#include "stm8s.h"
#include "UART1.h"

extern u8 start_flag;
extern u8 Temperature1;
u8 UART2_TX_BUFF[100]={0};      //�������ݻ�����
u8 UART2_TX_NUM;                //�����ֽ���������
u8 UART2_TX_CNT;                //�����ֽڼ�������
u8 DATA_dis[18]={0x5a,0xa5,0x0f,0x82,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

u8 TX_BUFF[20]={0};          //�������ݻ�����
u8 TX_NUM;                    //�����ֽ���������
u8 TX_CNT;                    //�����ֽڼ�������

u8 FrameBuff[FRAMEBUF_SIZE];   //����֡����������
u8 FrameBuff_flg=0;

//�������ú���
void USART2_Initial(void)
{
  UART2_DeInit();
#if defined( DM_II )//5����ʾ��
  UART2_Init((u32)115200, 
             UART2_WORDLENGTH_8D, 
             UART2_STOPBITS_1, 
             UART2_PARITY_NO, 
             UART2_SYNCMODE_CLOCK_DISABLE, 
             UART2_MODE_TXRX_ENABLE);
#endif 
#if defined( DM_I )//12����ʾ��
  UART2_Init((u32)9600, 
             UART2_WORDLENGTH_8D, 
             UART2_STOPBITS_1, 
             UART2_PARITY_NO, 
             UART2_SYNCMODE_CLOCK_DISABLE, 
             UART2_MODE_TXRX_ENABLE);
#endif 
  UART2_ITConfig(UART2_IT_RXNE_OR, ENABLE);
  UART2_Cmd(ENABLE);
ITC_SetSoftwarePriority(ITC_IRQ_UART2_RX, ITC_PRIORITYLEVEL_1);
}
/******************����1�����ж�*********************/
INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
{  
    UART2->SR&=~(1<<6);
  UART2_SendData8(TX_BUFF[TX_CNT]);//�����η��͵��������� TXBUFF
  TX_CNT++;                    //��һ�����˷��ͺ����ֽ�
  if(TX_CNT>=TX_NUM)           //�����ֽ���ֵ�ﵽ��������
  {
    UART2_ITConfig(UART2_IT_TC,DISABLE);//��ֹUART0 �����жϣ�ֹͣ����
  }
}
/******************����1��������********************/
void UART2_PutFrame(u8 *Ptr,u8 start_Lenth,u8 Lenth)
{
  for(u8 i=start_Lenth;i<Lenth;i++)
  {
    TX_BUFF[i]=Ptr[i];           //����������װ�뻺����
  }
  TX_CNT=start_Lenth;                     //�����ֽڼ�����0
  TX_NUM=Lenth-start_Lenth;                 //�����ֽ�����
  UART2_ITConfig(UART2_IT_TC,ENABLE);//��Ϊ�����һ�η����ж�
}
/******************��������������********************
��������void UART2SendString(u8* Data,u16 len)
�������ܣ�����2��������
������Data��len��DataΪ���͵����ݣ�lenΪ�������ݳ���
����ֵ����
���ڣ�2015.12.10
***************************************************/
void UART2SendString(u16* Data,u16 len)
{
  asm("sim");    // ��ȫ���ж�   
  unsigned char i;
  for(i=0;i<len;i++)
  {
    UART2_SendData8(Data[i]);                             /*����TxBuffer2������ַ�*/
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET); /* ��ѯ���ͻ��������ֽ��Ƿ��Ѿ����ͳ�ȥ */
  }
  asm("rim");    // ��ȫ���ж�   
}
u8 rx1_flg=0;
/******************����2�����ж�*********************/
INTERRUPT_HANDLER(UART2_RX_IRQHandler, 21)
{
  static u8 UART_RcvCut=0;
  UART2_ClearITPendingBit(UART2_IT_RXNE); 
  if(UART2_GetFlagStatus(UART2_FLAG_RXNE)!=SET)
  {
    if(UART_RcvCut<9)
    {
      FrameBuff[UART_RcvCut]=UART2_ReceiveData8();
      if(UART_RcvCut<9)
      {
        UART_RcvCut++;
        if(FrameBuff[0]==0x5A)
        {
          if(UART_RcvCut>=9)
          {
            FrameBuff_flg=1;
            UART_RcvCut=0;
          }
        }
        else
        {
          UART_RcvCut=0;FrameBuff_flg=0;
          for(u8 i=0;i<32;i++)
          {
            FrameBuff[i]=0;
          }
        }
      }   
      else 
      {
        UART_RcvCut=0;FrameBuff_flg=0;
        for(u8 i=0;i<9;i++)
        {
          FrameBuff[i]=0;
        }
      }
    }
    else
    {
      UART_RcvCut=0;FrameBuff_flg=0;
      for(u8 i=0;i<9;i++)
      {
        FrameBuff[i]=0;
      }
    }
  }
  else
  {
    UART_RcvCut=0;FrameBuff_flg=0;
    for(u8 i=0;i<9;i++)
    {
      FrameBuff[i]=0;
    }
  }
}