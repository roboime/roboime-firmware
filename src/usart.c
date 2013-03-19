#include "stm32f4xx_conf.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "linha_comando.h"
#include "usart.h"

char lc_usart1_buff[1024];
char lc_usart3_buff[1024];
char lc_uart4_buff[1024];

u32 USART1_Baudrate_val;
u32 USART3_Baudrate_val;
u32 UART4_Baudrate_val;

u8 uart4_debug=0;
u8 usart3_debug=0;

extern linha_comando_cmd_s comandos[];

linha_comando_s lc_usart1={
		0,
		0,
		1,
		USART1_PrintChar,
		1023,
		0,
		0,
		0,
		lc_usart1_buff,
		comandos
};

linha_comando_s lc_usart3={
		0,
		0,
		1,
		USART3_PrintChar,
		1023,
		0,
		0,
		0,
		lc_usart3_buff,
		comandos
};

linha_comando_s lc_uart4={
		0,
		0,
		1,
		UART4_PrintChar,
		1023,
		0,
		0,
		0,
		lc_uart4_buff,
		comandos
};


USART_InitTypeDef USART_InitTypeDef1={
		115200,
		USART_WordLength_8b,
		USART_StopBits_1,
		USART_Parity_No,
		(USART_Mode_Rx | USART_Mode_Tx ),
		USART_HardwareFlowControl_None
};

void USART1_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA |  RCC_APB2Periph_USART1,ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  Cara, Floating é Bizuleu!! Então eu vou definir como sendo Pull Down
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART1_Baudrate_val=USART_InitTypeDef1.USART_BaudRate;
	USART_Init(USART1,&USART_InitTypeDef1);
	USART_Cmd(USART1,ENABLE);

	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

u32 USART1_Get_Baudrate(){
	return USART1_Baudrate_val;
}

void USART1_Baudrate(u32 baudrate){
	USART_InitTypeDef1.USART_BaudRate=baudrate;
	USART1_Baudrate_val=USART_InitTypeDef1.USART_BaudRate;
	USART_Init(USART1,&USART_InitTypeDef1);
}


void USART1_PrintChar(char c){
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);//verifica se está desocupado
	USART_SendData(USART1,c & 0x1FF); //transmite
}


void USART1_IRQHandler(){
	char c=0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		/* Send the received data to the PC Host*/
		c=USART_ReceiveData(USART1);
//		pacoteTratar(dado); //TODO Comentei para compilar (Renault)
		if(uart4_debug){
			UART4_PrintChar(c);
		} else {
			 if(usart3_debug){
				USART3_PrintChar(c);
			 }
			 linha_comando_processar_char(c, &lc_usart1);
		}
	}

	/* If overrun condition occurs, clear the ORE flag and recover communication */
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET){
		(void)USART_ReceiveData(USART1);
	}

}

void USART3_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  Cara, Floating é Bizuleu!! Então eu vou definir como sendo Pull Down
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	USART3_Baudrate_val=USART_InitTypeDef1.USART_BaudRate;
	USART_Init(USART3,&USART_InitTypeDef1);
	USART_Cmd(USART3,ENABLE);

	USART_ITConfig(USART3,USART_IT_RXNE,ENABLE);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	/* Enable USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

u32 USART3_Get_Baudrate(){
	return USART3_Baudrate_val;
}

void USART3_Baudrate(u32 baudrate){
	USART_InitTypeDef1.USART_BaudRate=baudrate;
	USART3_Baudrate_val=USART_InitTypeDef1.USART_BaudRate;
	USART_Init(USART3,&USART_InitTypeDef1);
}


void USART3_PrintChar(char c){
	while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);//verifica se está desocupado
	USART_SendData(USART3,c & 0x1FF); //transmite
}


void USART3_IRQHandler(){
	char c=0;
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
		/* Send the received data to the PC Host*/
		c=USART_ReceiveData(USART3);
		if(usart3_debug){
			USART1_PrintChar(c);
		} else {
			linha_comando_processar_char(c, &lc_usart3);
		}
	}

	/* If overrun condition occurs, clear the ORE flag and recover communication */
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET){
		(void)USART_ReceiveData(USART3);
	}

}

void USART3_Debug(u8 estado){
	usart3_debug=estado;
}


void UART4_Init(){
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//  Cara, Floating é Bizuleu!! Então eu vou definir como sendo Pull Down
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	USART_DeInit(UART4);

	USART_InitTypeDef1.USART_BaudRate=9600;
	UART4_Baudrate_val=USART_InitTypeDef1.USART_BaudRate;

	USART_Init(UART4,&USART_InitTypeDef1);
	USART_Cmd(UART4,ENABLE);

	USART_ITConfig(UART4,USART_IT_RXNE,ENABLE);


	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable USART Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void UART4_Debug(u8 estado){
	uart4_debug=estado;
}

u32 UART4_Get_Baudrate(){
	return UART4_Baudrate_val;
}

void UART4_Baudrate(u32 baudrate){
	USART_InitTypeDef1.USART_BaudRate=baudrate;
	UART4_Baudrate_val=USART_InitTypeDef1.USART_BaudRate;
	USART_Init(UART4,&USART_InitTypeDef1);
}


void UART4_PrintChar(char c){
	while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);//verifica se está desocupado
	USART_SendData(UART4,c & 0x1FF); //transmite
}

void UART4_IRQHandler(){
	char c=0;

	if (USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
		/* Send the received data to the PC Host*/
		c=USART_ReceiveData(UART4);
		if(uart4_debug){
			USART1_PrintChar(c);
		} else {
			linha_comando_processar_char(c, &lc_uart4);
		}
	}

	/* If overrun condition occurs, clear the ORE flag and recover communication */
	if (USART_GetFlagStatus(UART4, USART_FLAG_ORE) != RESET){
		(void)USART_ReceiveData(UART4);
	}

}

