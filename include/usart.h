#ifndef USART_H
#define USART_H

void USART1_Init();
void USART1_PrintChar(char c);
void USART1_Baudrate(u32 baudrate);
u32 USART1_Get_Baudrate();

void USART3_Init();
void USART3_PrintChar(char c);
void USART3_Baudrate(u32 baudrate);
u32 USART3_Get_Baudrate();
void USART3_Debug(u8 estado);

void UART4_Init();
void UART4_PrintChar(char c);
void UART4_Baudrate(u32 baudrate);
u32 UART4_Get_Baudrate();
void UART4_Debug(u8 estado);



#endif
