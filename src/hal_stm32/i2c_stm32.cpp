#include <hal_stm32/i2c_stm32.h>

#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"

I2C_STM32::I2C_STM32(IO_Pin &sdapin, IO_Pin &sclpin, I2C_TypeDef* i2c, uint32_t speed, uint32_t timeout):
	I2C(sdapin, sclpin),
	_i2c(i2c),
	_speed(speed),
	_timeout(timeout)
{
	Config();
}

void I2C_STM32::Config()
{
	I2C_InitTypeDef I2C_InitStructure;

	/* Habilitando CLK para os periféricos--------------------------------------*/
	if(_i2c==I2C1){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	} else if(_i2c==I2C2){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C2, DISABLE);
	} else if(_i2c==I2C3){
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C3, ENABLE);
		RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C3, DISABLE);
	} else {
		return;
	}

	I2C_GenerateSTOP(_i2c, ENABLE);
	I2C_SoftwareResetCmd(_i2c, ENABLE);
	I2C_SoftwareResetCmd(_i2c, DISABLE);

	/* Configurações referentes ao I2C--------------------------------------*/
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = _speed;

	I2C_Init(_i2c, &I2C_InitStructure);
	I2C_Cmd(_i2c, ENABLE);
}


uint8_t I2C_STM32::WriteRegBuffer (uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Buffer, uint8_t Size){
	uint32_t I2C_TimeOut;

	/* Start I2C-------------------------------------------------------------- */
	I2C_GenerateSTART(_i2c, ENABLE);
	I2C_TimeOut = _timeout;
	while (I2C_GetFlagStatus(_i2c,I2C_FLAG_SB) == RESET){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Transmite o endereço do DeviceAddress escravo como escrita--------------- */
	I2C_Send7bitAddress(_i2c, DeviceAddress, I2C_Direction_Transmitter);

	I2C_TimeOut = _timeout;
	while (!I2C_CheckEvent(_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Transmite o endereço do registrador do DeviceAddress selecionado--------- */
	I2C_SendData(_i2c, RegisterAddress);

	I2C_TimeOut = _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	while(Size--){
		/* Transmite o valor a ser escrito no registrador do DeviceAddress selecionado--- */
		I2C_SendData(_i2c, *(Buffer++));

		I2C_TimeOut = _timeout;
		while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
			if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
		}
	}

	/* Espera a flag BTF (byte transfer finished) fique em nivel alto antes de gerar o stop bit  */
	I2C_TimeOut = 2 * _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Send STOP Condition */
	I2C_GenerateSTOP(_i2c, ENABLE);

	return 0;
}

uint8_t I2C_STM32::ReadRegBuffer(uint8_t DeviceAddress, uint8_t RegisterAddress, uint8_t* Buffer, uint8_t Size){
	uint32_t I2C_TimeOut;

	/* Gera o sinal de Start I2C */
	I2C_GenerateSTART(_i2c, ENABLE);

	/* Espera até que seja gerado a condição de início */
	I2C_TimeOut = _timeout;
	while (!I2C_GetFlagStatus(_i2c,I2C_FLAG_SB)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do dispositivo para leitura (obs: 1 envio como 'escrita', direction_transmitter) */
	I2C_Send7bitAddress(_i2c, DeviceAddress, I2C_Direction_Transmitter);

	I2C_TimeOut = _timeout;
	while (!I2C_CheckEvent(_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do registrador onde será realizada a leitura */
	I2C_SendData(_i2c, RegisterAddress);

	I2C_TimeOut = _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o segundo sinal de Start */
	I2C_GenerateSTART(_i2c, ENABLE);

	I2C_TimeOut = _timeout;
	while (!I2C_GetFlagStatus(_i2c,I2C_FLAG_SB)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do DeviceAddress STMP para leitura */
	I2C_Send7bitAddress(_i2c, DeviceAddress, I2C_Direction_Receiver);

	I2C_TimeOut = _timeout;
	while (!I2C_CheckEvent(_i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	while(Size--){
		if(Size==0){
			I2C_AcknowledgeConfig(_i2c, DISABLE);

			/* Envia o sinal de Stop */
			I2C_GenerateSTOP(_i2c, ENABLE);
		} else {
			I2C_AcknowledgeConfig(_i2c, ENABLE);
		}
		I2C_TimeOut = 2 * _timeout;
		while ((!I2C_GetFlagStatus(_i2c, I2C_FLAG_RXNE))){
			if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
		}

		*(Buffer++)=I2C_ReceiveData(_i2c);
	}



	I2C_TimeOut = _timeout;
	while (_i2c->CR1 & I2C_CR1_STOP){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	I2C_AcknowledgeConfig(_i2c, ENABLE);

	I2C_ClearFlag(_i2c,I2C_FLAG_AF);

	return 0;
}

uint8_t I2C_STM32::Timeout() {
	I2C_GenerateSTOP(_i2c, ENABLE);
	I2C_SoftwareResetCmd(_i2c, ENABLE);
	I2C_SoftwareResetCmd(_i2c, DISABLE);
	Config();
	uint8_t error=SelfTest();
	return !error;
}

uint8_t I2C_STM32::WriteReg16Buffer(uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t* Buffer, uint16_t Size) {
	uint32_t I2C_TimeOut;

	/* Start I2C-------------------------------------------------------------- */
	I2C_GenerateSTART(_i2c, ENABLE);
	I2C_TimeOut = _timeout;
	while (I2C_GetFlagStatus(_i2c,I2C_FLAG_SB) == RESET){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Transmite o endereço do DeviceAddress escravo como escrita--------------- */
	I2C_Send7bitAddress(_i2c, DeviceAddress, I2C_Direction_Transmitter);

	I2C_TimeOut = _timeout;
	while (!I2C_CheckEvent(_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do registrador onde será realizada a leitura */
	I2C_SendData(_i2c, (RegisterAddress>>8) & 0xff);

	I2C_TimeOut = _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do registrador onde será realizada a leitura */
	I2C_SendData(_i2c, RegisterAddress & 0xff);

	I2C_TimeOut = _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	while(Size--){
		/* Transmite o valor a ser escrito no registrador do DeviceAddress selecionado--- */
		I2C_SendData(_i2c, *(Buffer++));

		I2C_TimeOut = _timeout;
		while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
			if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
		}
	}

	/* Espera a flag BTF (byte transfer finished) fique em nivel alto antes de gerar o stop bit  */
	I2C_TimeOut = 2 * _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Send STOP Condition */
	I2C_GenerateSTOP(_i2c, ENABLE);

	return 0;
}

uint8_t I2C_STM32::ReadReg16Buffer(uint8_t DeviceAddress, uint16_t RegisterAddress, uint8_t* Buffer, uint16_t Size) {
	uint32_t I2C_TimeOut;

	/* Gera o sinal de Start I2C */
	I2C_GenerateSTART(_i2c, ENABLE);

	/* Espera até que seja gerado a condição de início */
	I2C_TimeOut = _timeout;
	while (!I2C_GetFlagStatus(_i2c,I2C_FLAG_SB)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do dispositivo para leitura (obs: 1 envio como 'escrita', direction_transmitter) */
	I2C_Send7bitAddress(_i2c, DeviceAddress, I2C_Direction_Transmitter);

	I2C_TimeOut = _timeout;
	while (!I2C_CheckEvent(_i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do registrador onde será realizada a leitura */
	I2C_SendData(_i2c, (RegisterAddress>>8) & 0xff);

	I2C_TimeOut = _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do registrador onde será realizada a leitura */
	I2C_SendData(_i2c, RegisterAddress & 0xff);

	I2C_TimeOut = _timeout;
	while ((!I2C_GetFlagStatus(_i2c,I2C_FLAG_TXE)) && (!I2C_GetFlagStatus(_i2c,I2C_FLAG_BTF))){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o segundo sinal de Start */
	I2C_GenerateSTART(_i2c, ENABLE);

	I2C_TimeOut = _timeout;
	while (!I2C_GetFlagStatus(_i2c,I2C_FLAG_SB)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	/* Envia o endereço do DeviceAddress STMP para leitura */
	I2C_Send7bitAddress(_i2c, DeviceAddress, I2C_Direction_Receiver);

	I2C_TimeOut = _timeout;
	while (!I2C_CheckEvent(_i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	while(Size--){
		if(Size==0){
			I2C_AcknowledgeConfig(_i2c, DISABLE);

			/* Envia o sinal de Stop */
			I2C_GenerateSTOP(_i2c, ENABLE);
		} else {
			I2C_AcknowledgeConfig(_i2c, ENABLE);
		}
		I2C_TimeOut = 2 * _timeout;
		while ((!I2C_GetFlagStatus(_i2c, I2C_FLAG_RXNE))){
			if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
		}

		*(Buffer++)=I2C_ReceiveData(_i2c);
	}



	I2C_TimeOut = _timeout;
	while (_i2c->CR1 & I2C_CR1_STOP){
		if (I2C_TimeOut-- == 0) return Timeout();//((I2C_s->I2C_TimeoutUserCallback!=0)?I2C_s->I2C_TimeoutUserCallback():0xFF);
	}

	I2C_AcknowledgeConfig(_i2c, ENABLE);

	I2C_ClearFlag(_i2c,I2C_FLAG_AF);

	return 0;
}
