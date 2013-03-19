#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "chute.h"
#include "sensor_bola.h"
#include "config.h"

#define CHUTE_PERIPH RCC_APB2Periph_GPIOC

#ifdef CHUTE_BARRO
	//#define CHUTE_PIN GPIO_Pin_2
	#define CHUTE_PIN GPIO_Pin_11
#else
	#define CHUTE_PIN GPIO_Pin_10
#endif

#define CHUTE_PORT GPIOC


u32 chutar(u32 tempo)
{
	if(sensor_bola(0)==0 || sensor_bola(1)==0)
	{
		u32 i=0;
		u32 j=0;
		u32 k=0;
		GPIO_SetBits(CHUTE_PORT, CHUTE_PIN);
		for(i=tempo;i;i--)
		{
			for(j=tempo;j;j--){
				k=j+i;
			}
		}
		GPIO_ResetBits(CHUTE_PORT, CHUTE_PIN);

		return k;
	}
	else
		return 0;
}

void chute_init()
{
	RCC_APB2PeriphClockCmd(CHUTE_PERIPH,ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin=CHUTE_PIN;
	GPIO_Init(CHUTE_PORT, &GPIO_InitStructure);
	GPIO_ResetBits(CHUTE_PORT, CHUTE_PIN);
}
