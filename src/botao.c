#include "stm32f4xx_conf.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "botao.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"
#include "stm32f4_discovery.h"

EXTI_InitTypeDef   EXTI_InitStructure;

void botao_init()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
    //GPIO_Init(GPIOA,&GPIO_InitStructure);
}

void EXTILine0_Config(void)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    //EXTI_Init(&EXTI_InitStructure);
    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //NVIC_Init(&NVIC_InitStructure);
    //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
}


#define WAIT(n) for (i = 0; i < n * 1000000; i++)
void EXTI0_IRQHandler()
{
    if (EXTI_GetITStatus(EXTI_Line0) != RESET)
    {
        int opcao_menu;
        int i;
        int j;
INICIO:
        // piscar todos os leds para sinalizar que entrou
        Led_Acender(0);
        Led_Acender(1);
        Led_Acender(2);
        Led_Acender(3);
        WAIT(10);
        Led_Apagar(0);
        Led_Apagar(1);
        Led_Apagar(2);
        Led_Apagar(3);
        // começar na opção 0
        opcao_menu = 0;
        Led_Acender(opcao_menu);
ESPERANDO:
        // esperar até o botao ser apertado
        Led_Acender(opcao_menu);
        WAIT(20)
        {
            if (botao_apertado())
            {
                // garantia que o botao foi apertado (tratar bouncing)
                WAIT(1) if (!botao_apertado()) continue;
                goto APERTADO;
            }
        }
        // Apagar os leds e tchau!!
        Led_Apagar(0);
        Led_Apagar(1);
        Led_Apagar(2);
        Led_Apagar(3);
        goto FIM;
APERTADO:
        // se soltar antes de 3M muda a opção,
        // se passar mais que isso segurando, entra na rotina
        WAIT(3)
        {
            if (!botao_apertado())
            {
                // trocar de opção, e de led
                Led_Apagar(opcao_menu);
                opcao_menu = (opcao_menu + 1) % 4;
                Led_Acender(opcao_menu);
                // esperar
                goto ESPERANDO;
            }
        }
        goto SELECIONADO;
SELECIONADO:
        // piscar o led selecionado
        Led_Apagar(opcao_menu);
        WAIT(5);
        Led_Acender(opcao_menu);
        switch (opcao_menu)
        {
        case 0:
            for (j = 0; j < 10; j++)
            {
                Led_Acender(0);
                Led_Acender(1);
                Led_Acender(2);
                Led_Acender(3);
                WAIT(j);
                Led_Apagar(0);
                Led_Apagar(1);
                Led_Apagar(2);
                Led_Apagar(3);
                WAIT(3);
            }
            //TODO
            break;
        case 1:
            WAIT(10);
            //TODO
            break;
        case 2:
            //while(!botao_apertado())
            while (botao_apertado()) drible(1);
            break;
        case 3:
            while (botao_apertado()) drible(1);
            break;
            break;
        }
        goto INICIO;
FIM:
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}
#undef WAIT

u8 botao_apertado()
{
    return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
}
