/*
 * NRF24.cpp
 *
 *  Created on: Aug 17, 2016
 *      Author: lenovoi7
 */

#include <NRF24.h>

uint8_t addressRoboList[] = {0XC3, 0XC4, 0XC5, 0XC6, 0XC7, 0XC8, 0XC9,
		0XCA, 0XCB, 0XB3, 0XB4, 0XB5, 0XB6, 0XB7, 0XB8, 0XB9};

NRF24::NRF24() {
  NRF_Spi = new MySpi();
  NRF_CE = new GPIO(GPIOA, GPIO_Pin_3);
  NRF_CE->Reset();
}
int NRF24::Config(){
  uint8_t reg_value;
  if(is_rx)
    reg_value=0b00001011;
  else
	reg_value=0b00001010;
  WriteRegister(0x00, &reg_value, 1);
  reg_value=0b11110011;
  WriteRegister(0x04, &reg_value, 1);
  reg_value=0b00000111;
  WriteRegister(0x05, &reg_value, 1);
  reg_value=0b00000011;
  WriteRegister(0x1c, &reg_value, 1);
  reg_value=0b00000110;
  WriteRegister(0x1d, &reg_value, 1);
  FlushRx();
  FlushTx();
  reg_value=0b01110000;
  WriteRegister((uint8_t) 0x07, &reg_value, 1);
  for(int i=0;i<0xeeeee2;i++);
  return 1;
}
int NRF24::WriteRegister(uint8_t adress, uint8_t *new_value, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0x20|adress;
  for(int i=1; i<size+1; i++){
    buf_out[i]=new_value[i-1];
  }
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) size+1))
    return 0;
  //for(int i=0;i<0xee2;i++);
  return 1;
}

int NRF24::ReadRegister(uint8_t adress, uint8_t *value, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=adress;
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) size+1))
	return 0;
  for(int i=0; i<size; i++){
    value[i]=buf_in[i+1];
  }
  return 1;
}
int NRF24::WritePayload(uint8_t *data, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0b10100000;
  for(int i=1; i<size+1; i++){
	buf_out[i]=data[i-1];
  }
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) size+1))
	return 0;
  return 1;
}
int NRF24::ReadPayload(uint8_t *data, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0b01100001;
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) size+1))
	return 0;
  for(int i=0; i<size; i++){
    data[i]=buf_in[i+1];
  }
  return 1;
}
int NRF24::FlushTx(){
  uint8_t buf_in[1];
  uint8_t buf_out[1];
  buf_out[0]=0b11100001;
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) 1))
	return 0;
  for(int i=0;i<0xee2;i++);
  return 1;
}
int NRF24::FlushRx(){
  uint8_t buf_in[1];
  uint8_t buf_out[1];
  buf_out[0]=0b11100010;
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) 1))
	return 0;
  for(int i=0;i<0xee2;i++);
  return 1;
}
int NRF24::WriteAckPayload(uint8_t *data, int size){
  uint8_t buf_in[size+1];
  uint8_t buf_out[size+1];
  buf_out[0]=0b10101000;
  for(int i=1; i<size+1; i++){
	buf_out[i]=data[i-1];
  }
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) size+1))
	return 0;
  return 1;
}
int NRF24::CheckPayloadWidth(){
  uint8_t buf_in[2];
  uint8_t buf_out[2];
  buf_out[0]=0b01100000;
  if(!NRF_Spi->Transfer(buf_in, buf_out, (int) 2))
	return 0;
  return buf_in[1];
}
int NRF24::SetId(uint8_t roboId){
  uint8_t roboAddress[5];
  for(int i=0; i<5; i++){
    roboAddress[i]=addressRoboList[roboId];
  }
  WriteRegister(0x0A, roboAddress, 5);
  WriteRegister(0x10, roboAddress, 5);
  return 0;
}

int NRF24::Test(){
  uint8_t new_value=0x57;
  uint8_t value;
  WriteRegister(0x0F, &new_value, 1);
  ReadRegister(0x0F, &value, 1);
  return (value==new_value);
}
int NRF24::DataSent(){
  uint8_t value;
  ReadRegister(REG_STATUS, &value, 1);
  return (value&0b00100000);
}
int NRF24::DataReady(){
  uint8_t value;
  ReadRegister(REG_STATUS, &value, 1);
  return (value&0b01000000);
}
int NRF24::MaxRt(){
  uint8_t value;
  ReadRegister(REG_STATUS, &value, 1);
  return (value&0b00010000);
}
int NRF24::CleanDataSent(){
  uint8_t new_value=0b00100000;
  return WriteRegister(REG_STATUS, &new_value, 1);
}
int NRF24::CleanDataReady(){
  uint8_t new_value=0b01000000;
  return WriteRegister(REG_STATUS, &new_value, 1);
}
int NRF24::CleanMaxRt(){
  uint8_t new_value=0b00010000;
  return WriteRegister(REG_STATUS, &new_value, 1);
}
int NRF24::RxEmpty(){
  uint8_t value;
  ReadRegister(REG_FIFO_STATUS, &value, 1);
  return (value&0b00000001);
}
int NRF24::RxFull(){
  uint8_t value;
  ReadRegister(REG_FIFO_STATUS, &value, 1);
  return (value&0b00000010);
}
int NRF24::TxEmpty(){
  uint8_t value;
  ReadRegister(REG_FIFO_STATUS, &value, 1);
  return (value&0b00010000);
}
int NRF24::TxFull(){
  uint8_t value;
  ReadRegister(REG_FIFO_STATUS, &value, 1);
  return (value&0b00100000);
}
