#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "chute.h"
#include "sensor_bola.h"
#include "config.h"

#define CHUTE_BAIXO_PERIPH RCC_AHB1Periph_GPIOE
#define CHUTE_ALTO_PERIPH RCC_AHB1Periph_GPIOE


#define CHUTE_BAIXO_PIN GPIO_Pin_10
#define CHUTE_ALTO_PIN GPIO_Pin_7


#define CHUTE_BAIXO_PORT GPIOE
#define CHUTE_ALTO_PORT GPIOE


u32 chutar_baixo(u32 tempo)
{
    if (sensor_bola(0) == 0 || sensor_bola(1) == 0)
    {
        u32 i = 0;
        u32 j = 0;
        u32 k = 0;
        GPIO_SetBits(CHUTE_BAIXO_PORT, CHUTE_BAIXO_PIN);
        for (i = tempo; i; i--)
        {
            for (j = tempo; j; j--)
            {
                k = j + i;
            }
        }
        GPIO_ResetBits(CHUTE_BAIXO_PORT, CHUTE_BAIXO_PIN);
        return k;
    }
    else
        return 0;
}

u32 chutar_alto(u32 tempo)
{
    if (sensor_bola(0) == 0 || sensor_bola(1) == 0)
    {
        u32 i = 0;
        u32 j = 0;
        u32 k = 0;
        GPIO_SetBits(CHUTE_ALTO_PORT, CHUTE_ALTO_PIN);
        for (i = tempo; i; i--)
        {
            for (j = tempo; j; j--)
            {
                k = j + i;
            }
        }
        GPIO_ResetBits(CHUTE_ALTO_PORT, CHUTE_ALTO_PIN);
        return k;
    }
    else
        return 0;
}

void chute_init()
{
    RCC_AHB1PeriphClockCmd(CHUTE_ALTO_PERIPH, ENABLE);
    RCC_AHB1PeriphClockCmd(CHUTE_BAIXO_PERIPH, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = CHUTE_BAIXO_PIN;
    GPIO_Init(CHUTE_BAIXO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(CHUTE_BAIXO_PORT, CHUTE_BAIXO_PIN);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = CHUTE_ALTO_PIN;
    GPIO_Init(CHUTE_ALTO_PORT, &GPIO_InitStructure);
    GPIO_ResetBits(CHUTE_ALTO_PORT, CHUTE_ALTO_PIN);
}
