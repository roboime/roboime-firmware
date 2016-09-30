#include <stm32f4xx_rcc.h>
#include <hal_stm32/spi_stm32.h>

const uint32_t SPI_STM32::SPI_TIMEOUT=0x4000;

SPI_STM32::SPI_STM32(SPI_TypeDef *spi, IO_Pin &SS_PIN):
	_spi(spi),
	_SS_PIN(&SS_PIN)
{
    if(_spi==SPI1){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	} else if(_spi==SPI2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	} else if(_spi==SPI3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);
	} else if(_spi==SPI4){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI4, ENABLE);
	} else if(_spi==SPI5){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI5, ENABLE);
	} else if(_spi==SPI6){
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI6, ENABLE);
	} else {
		return;
	}
    SPI_I2S_DeInit(_spi);
    SPI_InitTypeDef SPI_InitStructure;
    SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_CRCPolynomial=0;
    SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_16;
	SPI_Init(_spi, &SPI_InitStructure);
	SPI_CalculateCRC(_spi, DISABLE);
	SPI_Cmd(_spi, ENABLE);
	Release();
}

SPI_STM32::SPI_STM32(const SPI_STM32& spi_stm32, IO_Pin& SS_PIN):
	SPI_STM32(spi_stm32)
{
	_SS_PIN=&SS_PIN;
}

uint8_t SPI_STM32::WriteByte(uint8_t byte) {
	return WriteBuffer(&byte, 1);
}

void SPI_STM32::Assert() {
	_SS_PIN->Reset();
}

void SPI_STM32::Release() {
	_SS_PIN->Set();
}

uint8_t SPI_STM32::WriteBuffer(uint8_t* buffer, uint16_t size) {
	uint32_t resp=0;
	uint32_t timeout_t=0;
	while(size--){
		timeout_t=SPI_TIMEOUT;
		while(SPI_I2S_GetFlagStatus(_spi, SPI_I2S_FLAG_TXE) == RESET){
			if(timeout_t--==0) return Timeout();
		}

		SPI_I2S_SendData(_spi, *buffer++);

		timeout_t=SPI_TIMEOUT;
		while (SPI_I2S_GetFlagStatus(_spi, SPI_I2S_FLAG_BSY) == SET){
			if(timeout_t--==0) return Timeout();
		}

		timeout_t=SPI_TIMEOUT;
		while (SPI_I2S_GetFlagStatus(_spi, SPI_I2S_FLAG_RXNE) == RESET){
			if(timeout_t--==0) return Timeout();
		}
		resp += SPI_I2S_ReceiveData(_spi);
	}
//	for(timeout_t=0;timeout_t<200;timeout_t++);
	return resp;
}

uint8_t SPI_STM32::SelfTest() {
	return 0;
}

uint8_t SPI_STM32::Timeout() {
	return -1;
}
