/*
 * my_spi.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: lenovoi7
 */
#include "my_spi.h"

 MySpi::MySpi(){
  CS_Gpio = new GPIO(GPIOA, GPIO_Pin_4);
  Spi_TimeOut=0x4000;

  //MOSI, MISO, SCK GPIO configuration
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  GPIO_InitTypeDef GPIO_SPI_Pins_initstruct;
  GPIO_SPI_Pins_initstruct.GPIO_Mode	= GPIO_Mode_AF;
  GPIO_SPI_Pins_initstruct.GPIO_OType = GPIO_OType_PP;
  GPIO_SPI_Pins_initstruct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
  GPIO_SPI_Pins_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_SPI_Pins_initstruct.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
  GPIO_Init(GPIOA, &GPIO_SPI_Pins_initstruct);
  GPIO_PinAFConfig(GPIOA , GPIO_PinSource5 , GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  SPI_InitTypeDef SPI_InitStruct;
  SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128;
  SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
  SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
  SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
  SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
  SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
  SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
  SPI_Init(SPI1, &SPI_InitStruct);
  SPI_Cmd(SPI1,ENABLE);

  CS_Gpio->Set();
 }
int MySpi::Transfer(uint8_t *Data_IN, uint8_t *Data_OUT, int size){
  CS_Gpio->Reset();
  SPI_I2S_ReceiveData(SPI1);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

  for(int i=0; i<size; i++){
	SPI_I2S_SendData(SPI1, Data_OUT[i]);
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);

	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    Data_IN[i] = SPI_I2S_ReceiveData(SPI1);
  }

  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  CS_Gpio->Set();

  return 1;
}
