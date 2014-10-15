#define NRF24L01_SPI				SPI3
#define NRF24L01_SPI_PINS			TM_SPI_PinsPack_2
//Change CSN pin. This is for SPI communication
#define NRF24L01_CSN_RCC			RCC_AHB1Periph_GPIOD
#define NRF24L01_CSN_PORT			GPIOD
#define NRF24L01_CSN_PIN			GPIO_Pin_0

//Change CE pin. This pin is used to enable/disable transmitter/receiver functionality
#define NRF24L01_CE_RCC				RCC_AHB1Periph_GPIOH
#define NRF24L01_CE_PORT			GPIOH
#define NRF24L01_CE_PIN				GPIO_Pin_0
