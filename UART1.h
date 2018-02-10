#ifndef   _UART1_H
#define   _UART1_H
void USART2_Initial(void);
void UART2_PutFrame(u8 *Ptr,u8 start_Lenth,u8 Lenth);
void PLC_SCAN();
void MCU_SEND_PLC(u8 temp1,u8 temp2,u8 temp3);
void temp_scan(u16 temp1,u16 temp2);
void uart1_deal();
void UART2SendString(u16* Data,u16 len);
#define FRAMEBUF_SIZE      32      //最大帧长度

#endif
