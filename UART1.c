#include "stm8s.h"
#include "UART1.h"

extern u8 start_flag;
extern u8 Temperature1;
u8 UART2_TX_BUFF[100]={0};      //发送数据缓冲区
u8 UART2_TX_NUM;                //发送字节总数变量
u8 UART2_TX_CNT;                //发送字节计数变量
u8 DATA_dis[18]={0x5a,0xa5,0x0f,0x82,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

u8 TX_BUFF[20]={0};          //发送数据缓冲区
u8 TX_NUM;                    //发送字节总数变量
u8 TX_CNT;                    //发送字节计数变量

u8 FrameBuff[FRAMEBUF_SIZE];   //接受帧缓冲区数组
u8 FrameBuff_flg=0;

//串口配置函数
void USART2_Initial(void)
{
  UART2_DeInit();
#if defined( DM_II )//5寸显示屏
  UART2_Init((u32)115200, 
             UART2_WORDLENGTH_8D, 
             UART2_STOPBITS_1, 
             UART2_PARITY_NO, 
             UART2_SYNCMODE_CLOCK_DISABLE, 
             UART2_MODE_TXRX_ENABLE);
#endif 
#if defined( DM_I )//12寸显示屏
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
/******************串口1发送中断*********************/
INTERRUPT_HANDLER(UART2_TX_IRQHandler, 20)
{  
    UART2->SR&=~(1<<6);
  UART2_SendData8(TX_BUFF[TX_CNT]);//将本次发送的数据送入 TXBUFF
  TX_CNT++;                    //下一次依此发送后续字节
  if(TX_CNT>=TX_NUM)           //发送字节数值达到发送总数
  {
    UART2_ITConfig(UART2_IT_TC,DISABLE);//禁止UART0 发送中断，停止发送
  }
}
/******************串口1发送数据********************/
void UART2_PutFrame(u8 *Ptr,u8 start_Lenth,u8 Lenth)
{
  for(u8 i=start_Lenth;i<Lenth;i++)
  {
    TX_BUFF[i]=Ptr[i];           //待发送数据装入缓冲区
  }
  TX_CNT=start_Lenth;                     //发送字节计数清0
  TX_NUM=Lenth-start_Lenth;                 //发送字节总数
  UART2_ITConfig(UART2_IT_TC,ENABLE);//人为制造第一次发送中断
}
/******************串口三发送数据********************
函数名：void UART2SendString(u8* Data,u16 len)
函数功能：串口2发送数据
参数：Data，len；Data为发送的数据，len为发送数据长度
返回值：无
日期：2015.12.10
***************************************************/
void UART2SendString(u16* Data,u16 len)
{
  asm("sim");    // 关全局中断   
  unsigned char i;
  for(i=0;i<len;i++)
  {
    UART2_SendData8(Data[i]);                             /*发送TxBuffer2数组的字符*/
    while (UART2_GetFlagStatus(UART2_FLAG_TXE) == RESET); /* 查询发送缓冲区的字节是否已经发送出去 */
  }
  asm("rim");    // 开全局中断   
}
u8 rx1_flg=0;
/******************串口2接收中断*********************/
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