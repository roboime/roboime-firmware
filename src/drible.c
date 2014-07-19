#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "drible.h"
#include "config.h"

#define DRIBLE_PERIPH RCC_AHB1Periph_GPIOE
#define DRIBLE_PORT GPIOE


#define DRIBLE_PIN GPIO_Pin_11


void drible(u8 estado)
{
    if (estado)
    {
        GPIO_SetBits(DRIBLE_PORT, DRIBLE_PIN);
    }
    else
    {
        GPIO_ResetBits(DRIBLE_PORT, DRIBLE_PIN);
    }
}
void drible_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(DRIBLE_PERIPH, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = DRIBLE_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DRIBLE_PORT, &GPIO_InitStructure);
    drible(0);
}
