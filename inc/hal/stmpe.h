#pragma once

#include <hal/i2c.h>

typedef enum{
	STMPE_GPIO_Pin0=1<<0,
	STMPE_GPIO_Pin1=1<<1,
	STMPE_GPIO_Pin2=1<<2,
	STMPE_GPIO_Pin3=1<<3,
	STMPE_GPIO_Pin4=1<<4,
	STMPE_GPIO_Pin5=1<<5,
	STMPE_GPIO_Pin6=1<<6,
	STMPE_GPIO_Pin7=1<<7,
} STMPE_GPIO_Pin;

typedef enum{
	STMPE_GPIO_Mode_IN=0,
	STMPE_GPIO_Mode_OUT=1,
} STMPE_GPIO_Mode;

typedef enum{
	STMPE_GPIO_EDGE_Detection_Nothing=0x00,
	STMPE_GPIO_EDGE_Detection_Rising=0x01,
	STMPE_GPIO_EDGE_Detection_Falling=0x02,
	STMPE_GPIO_EDGE_Detection_RisingFalling=0x03,
} STMPE_GPIO_EDGE_Detection;

typedef struct {
	  uint8_t GPIO_Pin;
	  STMPE_GPIO_Mode GPIO_Mode;
	  STMPE_GPIO_EDGE_Detection GPIO_EDGE_Detection;
} STMPE_GPIO_InitTypeDef;

class STMPE
{
public:
	STMPE(I2C &i2c, uint8_t address);
	void GPIO_Init(STMPE_GPIO_InitTypeDef *STMPE_GPIO_InitStructure);
	void GPIO_SetBits(STMPE_GPIO_Pin Pins);
	void GPIO_ResetBits(STMPE_GPIO_Pin Pins);
	uint8_t GPIO_ReadInputData();
	uint8_t GPIO_ReadInterruptStatus();
	uint8_t GPIO_ReadGPIOInterruptStatus();
	uint8_t GPIO_ClearInterruptStatus();

	void SoftReset();
	void GPIOCLK_Cmd(uint8_t NewState);

	int8_t SelfTest();

	uint8_t I2C_TimeoutUserCallback(void);

protected:
	I2C *_i2c;
	const uint8_t _address;
};



#define STMPE_REG_CHIP_ID					0x00
#define STMPE_REG_ID_VER					0x02
#define STMPE_REG_SYS_CTRL1					0x03
#define STMPE_REG_SYS_CTRL2					0x04
#define STMPE_REG_INT_CTRL					0x09
#define STMPE_REG_INT_EN					0x0A
#define STMPE_REG_INT_STA					0x0B
#define STMPE_REG_GPIO_INT_EN				0x0C
#define STMPE_REG_GPIO_INT_STA				0x0D
#define STMPE_REG_GPIO_SET_PIN				0x10
#define STMPE_REG_GPIO_CLR_PIN				0x11
#define STMPE_REG_GPIO_MP_STA				0x12
#define STMPE_REG_GPIO_DIR					0x13
#define STMPE_REG_GPIO_ED_STA				0x14
#define STMPE_REG_GPIO_RE					0x15
#define STMPE_REG_GPIO_FE					0x16
#define STMPE_REG_GPIO_ALT_FUNCT			0x17

#define STMPE_REG_SYS_CTRL1_HIBERNATE		(1<<0)
#define STMPE_REG_SYS_CTRL1_SOFT_RESET		(1<<1)

#define STMPE_REG_SYS_CTRL2_ADC_OFF			(1<<0)
#define STMPE_REG_SYS_CTRL2_TSC_OFF			(1<<1)
#define STMPE_REG_SYS_CTRL2_GPIO_OFF		(1<<2)
#define STMPE_REG_SYS_CTRL2_TS_OFF			(1<<3)
