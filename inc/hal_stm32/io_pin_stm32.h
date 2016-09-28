#include <stm32f4xx_gpio.h>
#include <stm32f4xx_syscfg.h>
#include <stm32f4xx_exti.h>
#include <hal/io_pin.h>

class IO_Pin_STM32:public IO_Pin
{
public:
	IO_Pin_STM32(IO_Pin_Mode mode, GPIO_TypeDef *GPIOx, uint32_t pins, GPIOPuPd_TypeDef pupd=GPIO_PuPd_NOPULL, GPIOOType_TypeDef GPIO_OType=GPIO_OType_PP, uint8_t af=0, EXTITrigger_TypeDef EXTITrigger=(EXTITrigger_TypeDef)0, EXTIMode_TypeDef EXTIMode=EXTI_Mode_Interrupt);
	void Init(IO_Pin_Mode mode, GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStructure, uint8_t af=0, EXTITrigger_TypeDef EXTITrigger=(EXTITrigger_TypeDef)0, EXTIMode_TypeDef EXTIMode=EXTI_Mode_Interrupt);
	void Set();
	void Reset();
	void Write(uint8_t value);
	uint8_t Read();
	uint8_t GetIRQChannel();
	void Interrupt(uint8_t newstate);
protected:
	GPIO_TypeDef *_gpio;
	uint32_t _pins;
	uint8_t _NVIC_IRQChannel;
};
