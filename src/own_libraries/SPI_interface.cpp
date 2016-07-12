/*
 * SPI_interface.cpp
 *
 *  Created on: Mar 28, 2016
 *      Author: Renan
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "own_libraries/SPI_interface.h"
#include "own_libraries/CONFIG.h" //para usar SPI_Clock_Cmd() e GPIO_Clock_Cmd()

//TODO: resolver a incompatibilidade de velocidades do NRF e do SPI
SPI::SPI(SPI_TypeDef* SPI,GPIO_TypeDef* CS_GPIO,uint16_t CS_Pin){
	NRF_SPI 	= SPI;
	NRF_CS_GPIO = CS_GPIO;
	NRF_CS_Pin 	= CS_Pin;

	//CSN CLOCK ENABLE
	GPIO_Clock_Cmd(NRF_CS_GPIO, ENABLE);

	//CSN GPIO configuration
	GPIO_InitTypeDef GPIO_CSN_initstruct;
	GPIO_CSN_initstruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_CSN_initstruct.GPIO_OType = GPIO_OType_PP;
	GPIO_CSN_initstruct.GPIO_Pin = NRF_CS_Pin;
	GPIO_CSN_initstruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_CSN_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(NRF_CS_GPIO, &GPIO_CSN_initstruct);
	GPIO_SetBits(NRF_CS_GPIO, NRF_CS_Pin);//CSN inicialization

	//SPI1 CLOCK ENABLE
	SPI_Clock_Cmd(NRF_SPI,ENABLE);
	//SPI1 CONFIG
	SPI_InitTypeDef SPI_InitStruct;										//SPI config
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;	//SPI config
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;							//SPI config
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;								//SPI config
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;						//SPI config
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		//SPI config
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;						//SPI config
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;							//SPI config
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;								//SPI config
	SPI_Init(NRF_SPI, &SPI_InitStruct);									//SPI config
	SPI_Cmd(NRF_SPI,ENABLE);											//SPI config
}

void SPI::ASSERT_CS(int STATE){
	if(STATE==SET){
		GPIO_SetBits(NRF_CS_GPIO,NRF_CS_Pin);
	}else if(STATE==RESET){
		GPIO_ResetBits(NRF_CS_GPIO,NRF_CS_Pin);
	}
}

/*
 * envia comando genérico de 8 bits ao NRF24 via SPI
 * cmd: comando a ser enviado pela discovery (master)
 * size: número de bytes que devem ser recebidos/enviados pelo SPI
 * ptr_send: ponteiro com os size bytes a serem enviados, use NULL para somente receber
 * ptr_receive: ponteiro com os size bytes a serem recebidos, use NULL para apenas enviar
 */
int SPI::CMD(uint8_t cmd, uint8_t size,uint8_t* ptr_send,uint8_t* ptr_receive){
	ASSERT_CS(RESET);

	//limpa o buffer de recepção
	SPI_I2S_ReceiveData(NRF_SPI);

	SPI_I2S_SendData(NRF_SPI,cmd);
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));
	uint8_t status = SPI_I2S_ReceiveData(NRF_SPI);

	uint8_t trash;

	while(size--){
		if(ptr_send!=0x00){
			SPI_I2S_SendData(NRF_SPI,*ptr_send);
			ptr_send++;
		}else{
			SPI_I2S_SendData(NRF_SPI,0x00);
		}
		while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));

		while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));
		if(ptr_receive!=0x00){
			*ptr_receive = SPI_I2S_ReceiveData(NRF_SPI);
			trash = *ptr_receive;
			ptr_receive++;
		}else{
			SPI_I2S_ReceiveData(NRF_SPI);
		}
	}

	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_BSY));

	ASSERT_CS(SET);
	return status;
}

//TODO: resolver a incompatibilidade de velocidades do NRF e da STM32 sem usar Delay_ms()
//escreve registrador do NRF24 via SPI
int SPI::W_REGISTER(uint8_t adress, uint8_t size,uint8_t* value){//VALUE: array com os bits do registrador SIZE: EM BYTES
	//verifica se adress é mesmo um  endereço
	if(adress > 0x1d){
		return 1;
	}

	uint8_t wcommand = adress;
	wcommand |= 32;

	return CMD(wcommand,size,value,0x00);
}//WORKED! but SPI seems too much fast to NRF

//POINTER: array com os bits do registrador SIZE: EM BYTES
int SPI::R_REGISTER(uint8_t address, uint8_t size,uint8_t* pointer){
	if(address>0x1d)
		return -1;
	return CMD(address,size,0x00,pointer);
}//WORKED!

//devolve o tamanho em bytes da payload no topo da RX_FIFO
uint8_t SPI::R_RX_PL_WID(void){
	uint8_t width = 0;
	CMD(0x60,1,0x00,&width);

	return width;
}

//lê o topo da RX_PAYLOAD via SPI
int SPI::R_RX_PAYLOAD(uint8_t* pointer){// lê o RX_PAYLOAD e armazena em pointer
	ACTIVATE();//activate, para usar R_RX_PL_WID

	ASSERT_CS(RESET);

	SPI_I2S_SendData(NRF_SPI,0x60);//comando R_RX_PL_WID

	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	//tenta ler o tamanho da payload no topo da rx_fifo
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));
	uint8_t payload_width = SPI_I2S_ReceiveData(NRF_SPI);

	while(SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_BSY));
	ASSERT_CS(SET);

	//cheat: o comando de activate ou o de R_RX_PL_WID podem estar falhando
	payload_width = 5;
	for(int j=0;j<0xffff;j++);
	CMD(0x61,payload_width,0x00,pointer);//comando R_RX_PLD

	return 0;
}

int SPI::W_TX_PAYLOAD(uint8_t* pointer,uint8_t number){//number: número de bytes de pointer para transmitir
	uint8_t command = 0xa0;

	return CMD(command,number,pointer,0x00);
}

void SPI::FLUSH_TX(){
	ASSERT_CS(RESET);

	//verificar TXE conforme manual de referência
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));

	uint8_t flushcmd = 0b11100001;
	SPI_I2S_SendData(NRF_SPI,flushcmd);//envia o comando de limpar a TX fifo

	for(int j=0;j<0xffff;j++);//sem esse delay, o nrf24 não tem tempo para limpar a tx fifo

	//verificar TXE conforme manual de referência
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));

	SPI_I2S_ReceiveData(NRF_SPI);

	//verifica TXE, BSY conforme manual de referência
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_BSY));

	ASSERT_CS(SET);
	return;
}

void SPI::FLUSH_RX(){
	ASSERT_CS(RESET);

	//verificar TXE conforme manual de referência
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));

	uint8_t flushcmd = 0b11100010;
	SPI_I2S_SendData(NRF_SPI,flushcmd);//envia o comando de limpar a RX fifo

	Delay_ms(1);//sem esse delay, o nrf24 não tem tempo para limpar a tx fifo

	//verificar TXE conforme manual de referência
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));

	SPI_I2S_ReceiveData(NRF_SPI);

	//verifica TXE, BSY conforme manual de referência
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_BSY));

	ASSERT_CS(SET);
	return;
}

//TODO: fix NRF_ACTIVATE
void SPI::ACTIVATE(void){//activate, para usar R_RX_PL_WID, W_ACK_PAYLOAD, W_TX_PAYLOAD_NOACK

	uint8_t status;//enquanto estiver enviando cmd para o NRF24, ele mandará o status register

	//CS é  resetado
	ASSERT_CS(RESET);

	SPI_I2S_ReceiveData(NRF_SPI);// limpa o lixo em spi_dr
	SPI_I2S_SendData(NRF_SPI,0x50);//comando activate

	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	//espera enquanto não houver nada para ler
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));

	status = SPI_I2S_ReceiveData(NRF_SPI);

	SPI_I2S_SendData(NRF_SPI,0x73);//envia 0x73 para ativar os comandos
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_TXE));
	while(!SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_RXNE));
	SPI_I2S_ReceiveData(NRF_SPI);

	while(SPI_I2S_GetFlagStatus(NRF_SPI,SPI_I2S_FLAG_BSY));

	ASSERT_CS(SET);//seta o CS, fim da escrita
	return;
}//doesn't work

uint8_t SPI::NOP(void){
	return CMD(0xff,0,0x00,0x00);//NOP command, no data bytes, only sends back the content of STATUS register
}
