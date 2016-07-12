/*
 * NRF24.h
 *
 *  Created on: Nov 16, 2015
 *      Author: Renan Pícoli e Gustavo Karl
 */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "stm32f4xx_gpio.h"
#include "own_libraries/REGISTER.h"
#include "own_libraries/NRF_REGISTER_MAP.h"
#include "own_libraries/SPI_interface.h"

#ifndef NRF24_H_
#define NRF24_H_

//definição de valores para habilitar auto ACK em pipes
#define ENAA_Disable_All_Pipes  0b00000000
#define ENAA_P0					0b00000001
#define ENAA_P1 				0b00000010
#define ENAA_P2 				0b00000100
#define ENAA_P3 				0b00001000
#define ENAA_P4 				0b00010000
#define ENAA_P5 				0b00100000
#define ENAA_All_Pipes  		0b00111111

//definição de valores para habilitar uso de pipes
#define ERX_Disable_All_Pipes  	0b00000000
#define ERX_P0					0b00000001
#define ERX_P1 					0b00000010
#define ERX_P2 					0b00000100
#define ERX_P3 					0b00001000
#define ERX_P4 					0b00010000
#define ERX_P5 					0b00100000
#define ERX_All_Pipes  			0b00111111

//definição de valores para configurar o registrador de largura de endereço (SETUP_AW)
#define AW_3_bytes 	0b00000001
#define AW_4_bytes 	0b00000010
#define AW_5_bytes 	0b00000011

//definition of SETUP_RETR delay configurations
#define		RETR_ARD_wait_250_us    0x00
#define		RETR_ARD_wait_500_us    0x10
#define		RETR_ARD_wait_750_us    0x20
#define		RETR_ARD_wait_1000_us   0x30
#define		RETR_ARD_wait_1250_us   0x40
#define		RETR_ARD_wait_1500_us   0x50
#define		RETR_ARD_wait_1750_us   0x60
#define		RETR_ARD_wait_2000_us   0x70
#define		RETR_ARD_wait_2250_us   0x80
#define		RETR_ARD_wait_2500_us   0x90
#define		RETR_ARD_wait_2750_us   0xa0
#define		RETR_ARD_wait_3000_us   0xb0
#define		RETR_ARD_wait_3250_us   0xc0
#define		RETR_ARD_wait_3500_us   0xd0
#define		RETR_ARD_wait_3750_us   0xe0
#define		RETR_ARD_wait_4000_us   0xf0

#define RX_Pipe_0 0
#define RX_Pipe_1 1
#define RX_Pipe_2 2
#define RX_Pipe_3 3
#define RX_Pipe_4 4
#define RX_Pipe_5 5

/*
no projeto em C, era permitido fazer i=RX_Pipe_0 e depois i++
mas em C++ isso não está sendo permitido,
erro: "Lvalue required".
Segundo http://ieng9.ucsd.edu/~cs30x/Lvalues%20and%20Rvalues.htm
enums não são lvalues
*/
//enumeration of pipes available for reception
//por padrão, C começa enumerações no zero
/*typedef enum{
	RX_Pipe_0,
	RX_Pipe_1,
	RX_Pipe_2,
	RX_Pipe_3,
	RX_Pipe_4,
	RX_Pipe_5
}pipe;*/

//enumeration of SETUP_RETR number of retransmissions configurations
//por padrão, C começa enumerações no zero, isso é importante para o valor correto ser gravado em SETUP_RETR
typedef enum{
	RETR_ARC_DISABLE_RETRANSMIT,
	RETR_ARC_1_RETRANSMIT,
	RETR_ARC_2_RETRANSMIT,
	RETR_ARC_3_RETRANSMIT,
	RETR_ARC_4_RETRANSMIT,
	RETR_ARC_5_RETRANSMIT,
	RETR_ARC_6_RETRANSMIT,
	RETR_ARC_7_RETRANSMIT,
	RETR_ARC_8_RETRANSMIT,
	RETR_ARC_9_RETRANSMIT,
	RETR_ARC_10_RETRANSMIT,
	RETR_ARC_11_RETRANSMIT,
	RETR_ARC_12_RETRANSMIT,
	RETR_ARC_13_RETRANSMIT,
	RETR_ARC_14_RETRANSMIT,
	RETR_ARC_15_RETRANSMIT
}RETR_ARCount;

//constants for RF_SETUP options
#define RF_SETUP_PLL_LOCK 			0b00010000
#define RF_SETUP_Data_Rate_1Mbps	0b00000000
#define RF_SETUP_Data_Rate_2Mbps	0b00001000
#define RF_SETUP_PWR_minus18dBm		0b00000000
#define RF_SETUP_PWR_minus12dBm		0b00000010
#define RF_SETUP_PWR_minus6dBm		0b00000100
#define RF_SETUP_PWR_0dBm			0b00000110
#define RF_SETUP_LNA_HCURR			0b00000001

//other useful constants
#define RX_EMPTY_MASK 	0b00000001
#define TX_EMPTY_MASK	0b00010000
#define RX_DR_MASK 		0b01000000
#define TX_DS_MASK		0b00100000
#define RX_P_NO_MASK	0b00001110

//faz toda a configuração do TX mode e do RX mode
typedef struct{
	uint8_t channel; 			//determines the frequency in which the nRF42 will operate
	uint8_t ENAA_Px; 			//determines which pipes will have auto ACK enabled
	uint8_t ERX_Px;  			//determines which pipes will be used
	uint8_t AW_x_bytes;			//setup of Address Widths (common for all data pipes)
	uint8_t RETR_ARC_and_ARD;	//setup of Automatic Retransmission
	uint8_t RF_SETUP;			//setup of LNA gain, output power, air data rate and PLL lock signal

	uint8_t* TX_ADDR;			//setup of address of the RECEIVING node

	//IMPORTANTE: NÃO ALTERAR A ORDEM DESSAS VARIÁVEIS, NEM INTERCALAR OUTRAS VARIÁVEIS AQUI
	uint8_t payload_width_pipe_0;//the payload width associated with that pipe, still doesn't allow DPL
	uint8_t payload_width_pipe_1;//the payload width associated with that pipe, still doesn't allow DPL
	uint8_t payload_width_pipe_2;//the payload width associated with that pipe, still doesn't allow DPL
	uint8_t payload_width_pipe_3;//the payload width associated with that pipe, still doesn't allow DPL
	uint8_t payload_width_pipe_4;//the payload width associated with that pipe, still doesn't allow DPL
	uint8_t payload_width_pipe_5;//the payload width associated with that pipe, still doesn't allow DPL

	uint8_t* RX_ADDR_P0;
	uint8_t* RX_ADDR_P1;
	uint8_t* RX_ADDR_P2;
	uint8_t* RX_ADDR_P3;
	uint8_t* RX_ADDR_P4;
	uint8_t* RX_ADDR_P5;
}config_Struct;

class NRF : private SPI, public NRF_REGISTER_MAP {
//atributos
private:
	GPIO_TypeDef* NRF_CE_GPIO;	//usar como default: GPIOA
	uint16_t NRF_CE_Pin;		//usar como default: GPIO_Pin_3
	GPIO_TypeDef* NRF_IRQ_GPIO;
	uint16_t NRF_IRQ_Pin;

//métodos
public:
	//CONSTRUTOR
	NRF(GPIO_TypeDef* CE_GPIO = GPIOA,uint16_t CE_Pin = GPIO_Pin_3,
		GPIO_TypeDef* IRQ_GPIO= GPIOC,uint16_t IRQ_Pin= GPIO_Pin_5,
		SPI_TypeDef* SPI = SPI1,
		GPIO_TypeDef* CS_GPIO 	= GPIOA,uint16_t CS_Pin  = GPIO_Pin_4,
		GPIO_TypeDef* SCK_GPIO 	= GPIOA,uint16_t SCK_Pin = GPIO_Pin_5,
		GPIO_TypeDef* MISO_GPIO = GPIOA,uint16_t MISO_Pin=GPIO_Pin_6,
		GPIO_TypeDef* MOSI_GPIO = GPIOA,uint16_t MOSI_Pin=GPIO_Pin_7);
	void begin();
	uint16_t IRQ_Pin(){return NRF_IRQ_Pin;};
	GPIO_TypeDef* IRQ_GPIO(){return NRF_IRQ_GPIO;};
	void start_listen();
	void stop_listen();
	uint8_t RECEIVE(uint8_t* data);
	uint8_t SEND(uint8_t* data, uint8_t size = 5);
	void RX_configure(config_Struct* pointer);
	void TX_configure(config_Struct* pointer);
	void RX_configure();
	void TX_configure();

private:
	//métodos
	int W_REGISTER(uint8_t adress, uint8_t size,uint8_t* value) {return SPI::W_REGISTER(adress, size,value); };//WORKED! but SPI seems too much fast to NRF
	int R_REGISTER(uint8_t adress, uint8_t size,uint8_t* pointer) {return SPI::R_REGISTER(adress, size, pointer); };//WORKED!
	void RESET_ALL_REGISTERS(void);//WORKED!  but fails with read-only registers
	void ASSERT_CE(int STATE);
	void stdbyI_to_TX(uint8_t channel);
	void stdbyI_to_RX(uint8_t channel);//WORKED!
	void EN_AA_setup(uint8_t ENAA_Px);//WORKED!
	void EN_RXADDR_setup(uint8_t ERX_Px);//WORKED!
	void SETUP_AW_setup(uint8_t AW);//WORKED!
	void SETUP_RETR_setup(uint8_t setup_retries);//WORKED!
	void RF_CH_setup(uint8_t channel);//WORKED!
	void RF_SETUP_setup(uint8_t configuration);//WORKED!
	void RX_ADDR_Px_setup(uint8_t RX_Pipe,uint8_t* pointer);
	void RX_ADDR_P0_setup(uint8_t* pointer);//WORKED!
	void RX_ADDR_P1_setup(uint8_t* pointer);//WORKED!
	void TX_ADDR_setup(uint8_t* pointer);
	void RX_PW_Px_setup(uint8_t RX_Pipe, uint8_t payload_width);
	uint8_t DATA_READY(void);//WORKED!
	uint8_t TRANSMITTED(void);
	void READ_RX_FIFO(uint8_t* pointer);
	uint8_t GET_PIPE_FOR_READING(void);
};

//void NRF_REGISTERS_INIT(NRF_REGISTERS* pointer);//WORKED!
//void NRF_READ_ALL_REGISTERS(NRF_REGISTERS* pointer);//WORKED!

#endif /* NRF24_H_ */
