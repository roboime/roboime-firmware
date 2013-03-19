#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include <stdio.h>

#include "config.h"
#include "motores.h"
#include "usart.h"
#include "pid.h"
#include "timer.h"
#include "leds.h"
#include "linha_comando.h"



//Define Motor 0
#define MOTOR_0_TIM TIM1

#define MOTOR_0_A_H_PORT GPIOA
#define MOTOR_0_A_L_PORT GPIOA
#define MOTOR_0_B_H_PORT GPIOA
#define MOTOR_0_B_L_PORT GPIOA

#define MOTOR_0_A_H_PIN GPIO_Pin_8
#define MOTOR_0_A_L_PIN GPIO_Pin_12
#define MOTOR_0_B_H_PIN GPIO_Pin_11
#define MOTOR_0_B_L_PIN GPIO_Pin_13

#define MOTOR_0_A_H_TIM TIM1
#define MOTOR_0_B_H_TIM TIM1

#define MOTOR_0_A_H_REG TIM1->CCR1
#define MOTOR_0_B_H_REG TIM1->CCR4

//Define Motor 1
#define MOTOR_1_TIM TIM1

#define MOTOR_1_A_H_PORT GPIOB
#define MOTOR_1_A_L_PORT GPIOA
#define MOTOR_1_B_H_PORT GPIOB
#define MOTOR_1_B_L_PORT GPIOB

#define MOTOR_1_A_H_PIN GPIO_Pin_0 //remapeado
#define MOTOR_1_A_L_PIN GPIO_Pin_14
#define MOTOR_1_B_H_PIN GPIO_Pin_1 //remapeado
#define MOTOR_1_B_L_PIN GPIO_Pin_2

#define MOTOR_1_A_H_TIM TIM1
#define MOTOR_1_B_H_TIM TIM1

#define MOTOR_1_A_H_REG TIM1->CCR2
#define MOTOR_1_B_H_REG TIM1->CCR3

//Define Motor 2
#define MOTOR_2_TIM TIM8

#define MOTOR_2_A_H_PORT GPIOC
#define MOTOR_2_A_L_PORT GPIOB
#define MOTOR_2_B_H_PORT GPIOC
#define MOTOR_2_B_L_PORT GPIOB

#define MOTOR_2_A_H_PIN GPIO_Pin_6
#define MOTOR_2_A_L_PIN GPIO_Pin_4
#define MOTOR_2_B_H_PIN GPIO_Pin_7
#define MOTOR_2_B_L_PIN GPIO_Pin_5

#define MOTOR_2_A_H_TIM TIM8
#define MOTOR_2_B_H_TIM TIM8

#define MOTOR_2_A_H_REG TIM8->CCR1
#define MOTOR_2_B_H_REG TIM8->CCR2

//Define Motor 3
#define MOTOR_3_TIM TIM8

#define MOTOR_3_A_H_PORT GPIOC
#define MOTOR_3_A_L_PORT GPIOA
#define MOTOR_3_B_H_PORT GPIOC
#define MOTOR_3_B_L_PORT GPIOA

#define MOTOR_3_A_H_PIN GPIO_Pin_8
#define MOTOR_3_A_L_PIN GPIO_Pin_2
#define MOTOR_3_B_H_PIN GPIO_Pin_9
#define MOTOR_3_B_L_PIN GPIO_Pin_3

#define MOTOR_3_A_H_TIM TIM8
#define MOTOR_3_B_H_TIM TIM8

#define MOTOR_3_A_H_REG TIM8->CCR3
#define MOTOR_3_B_H_REG TIM8->CCR4

#define MOTOR_MINIMO 0
#define MOTOR_MAXIMO 1000

//TODO Remover
#define MOTOR_0_POSICAO TIM_GetCounter(TIM2)
#define MOTOR_1_POSICAO TIM_GetCounter(TIM3)
#define MOTOR_2_POSICAO TIM_GetCounter(TIM4)
#define MOTOR_3_POSICAO TIM_GetCounter(TIM5)


#define TRUE 1
#define FALSE 0

CONTROLADOR_S cont_m1, cont_m0, cont_m2, cont_m3;

float motor_get_velocidade(u8 motor){
	switch(motor){
		case 0:
			return cont_m0.realimentacao;
		case 1:
			return cont_m1.realimentacao;
		case 2:
			return cont_m2.realimentacao;
		case 3:
			return cont_m3.realimentacao;
		default:
			return -1.0;
	}

}

u8 motor_velocidade(u8 motor, float vel){
	switch(motor){
		case 0:
			cont_m0.entrada=vel;
			return TRUE;
		case 1:
			cont_m1.entrada=vel;
			return TRUE;
		case 2:
			cont_m2.entrada=vel;
			return TRUE;
		case 3:
			cont_m3.entrada=vel;
			return TRUE;
		default:
			return FALSE;
	}
}
void motores_amostrar(){
	static s16 posicao[4];
	s16 posicao_new=0;
	s16 velocidade[4];

	posicao_new=(s16)MOTOR_0_POSICAO;
	velocidade[0]=-(posicao_new-posicao[0]);
	posicao[0]=posicao_new;

	posicao_new=(s16)MOTOR_1_POSICAO;
	velocidade[1]=-(posicao_new-posicao[1]);
	posicao[1]=posicao_new;

	posicao_new=(s16)MOTOR_2_POSICAO;
	velocidade[2]=-(posicao_new-posicao[2]);
	posicao[2]=posicao_new;

	posicao_new=(s16)MOTOR_3_POSICAO;
	velocidade[3]=-(posicao_new-posicao[3]);
	posicao[3]=posicao_new;

	float veloc0=(float)velocidade[0]*Get_Freq()/3920.0;
	cont_m0.realimentacao=(s16)(veloc0*10);

	float veloc1=(float)velocidade[1]*Get_Freq()/3920.0;
	cont_m1.realimentacao=(s16)(veloc1*10);

	float veloc2=(float)velocidade[2]*Get_Freq()/3920.0;
	cont_m2.realimentacao=(s16)(veloc2*10);

	float veloc3=(float)velocidade[3]*Get_Freq()/3920.0;
	cont_m3.realimentacao=(s16)(veloc3*10);

	pidService(&cont_m0);
	pidService(&cont_m1);
	pidService(&cont_m2);
	pidService(&cont_m3);

	motor_tensao(0,(s16)cont_m0.saida);
	motor_tensao(1,(s16)cont_m1.saida);
	motor_tensao(2,(s16)cont_m2.saida);
	motor_tensao(3,(s16)cont_m3.saida);

	static int temp=0;
	if((temp++)%50==0){
		display_matlab(veloc0, veloc1, veloc2, veloc3);
	}

}


u8 motor_parar(u8 motor){
	return motor_tensao(motor, MOTOR_MINIMO);
}

u8 motor_frear(u8 motor){
	switch(motor){
		case 0:
			GPIO_ResetBits(MOTOR_0_A_L_PORT, MOTOR_0_A_L_PIN);
			GPIO_ResetBits(MOTOR_0_B_L_PORT, MOTOR_0_B_L_PIN);
			MOTOR_0_A_H_REG=MOTOR_0_B_H_REG=MOTOR_MAXIMO;
			TIM_GenerateEvent(MOTOR_0_A_H_TIM,TIM_EventSource_Update);
			TIM_GenerateEvent(MOTOR_0_B_H_TIM,TIM_EventSource_Update);
			return TRUE;
		case 1:
			GPIO_ResetBits(MOTOR_1_A_L_PORT, MOTOR_1_A_L_PIN);
			GPIO_ResetBits(MOTOR_1_B_L_PORT, MOTOR_1_B_L_PIN);
			MOTOR_1_A_H_REG=MOTOR_1_B_H_REG=MOTOR_MAXIMO;
			TIM_GenerateEvent(MOTOR_1_A_H_TIM,TIM_EventSource_Update);
			TIM_GenerateEvent(MOTOR_1_B_H_TIM,TIM_EventSource_Update);
			return TRUE;
		case 2:/* Inserido Navarro */
			GPIO_ResetBits(MOTOR_2_A_L_PORT, MOTOR_2_A_L_PIN);
			GPIO_ResetBits(MOTOR_2_B_L_PORT, MOTOR_2_B_L_PIN);
			MOTOR_2_A_H_REG=MOTOR_2_B_H_REG=MOTOR_MAXIMO;
			TIM_GenerateEvent(MOTOR_2_A_H_TIM,TIM_EventSource_Update);
			TIM_GenerateEvent(MOTOR_2_B_H_TIM,TIM_EventSource_Update);
		case 3:/* Inserido Navarro */
			GPIO_ResetBits(MOTOR_3_A_L_PORT, MOTOR_3_A_L_PIN);
			GPIO_ResetBits(MOTOR_3_B_L_PORT, MOTOR_3_B_L_PIN);
			MOTOR_3_A_H_REG=MOTOR_3_B_H_REG=MOTOR_MAXIMO;
			TIM_GenerateEvent(MOTOR_3_A_H_TIM,TIM_EventSource_Update);
			TIM_GenerateEvent(MOTOR_3_B_H_TIM,TIM_EventSource_Update);
		default:
			return FALSE;
	}
}

u8 motor_tensao(u8 motor, s16 valor){
	if(valor>MOTOR_MAXIMO){
		valor=MOTOR_MAXIMO;
	} else if(valor<-MOTOR_MAXIMO){
		valor=-MOTOR_MAXIMO;
	}

	switch(motor){
		case 0:
			GPIO_ResetBits(MOTOR_0_A_L_PORT, MOTOR_0_A_L_PIN);
			GPIO_ResetBits(MOTOR_0_B_L_PORT, MOTOR_0_B_L_PIN);
			if(valor>0){
				MOTOR_0_A_H_REG=valor;
				MOTOR_0_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_0_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_0_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_0_B_H_PORT,MOTOR_0_B_H_PIN));
				GPIO_SetBits(MOTOR_0_B_L_PORT, MOTOR_0_B_L_PIN);
			} else if(valor<0){
				MOTOR_0_B_H_REG=-valor;
				MOTOR_0_A_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_0_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_0_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_0_A_H_PORT,MOTOR_0_A_H_PIN));
				GPIO_SetBits(MOTOR_0_A_L_PORT, MOTOR_0_A_L_PIN);
			} else {
				MOTOR_0_A_H_REG=MOTOR_MINIMO;
				MOTOR_0_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_0_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_0_B_H_TIM,TIM_EventSource_Update);
			}
			return TRUE;
		case 1:
			GPIO_ResetBits(MOTOR_1_A_L_PORT, MOTOR_1_A_L_PIN);
			GPIO_ResetBits(MOTOR_1_B_L_PORT, MOTOR_1_B_L_PIN);
			if(valor>0){
				MOTOR_1_A_H_REG=valor;
				MOTOR_1_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_1_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_1_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_1_B_H_PORT, MOTOR_1_B_H_PIN));
				GPIO_SetBits(MOTOR_1_B_L_PORT, MOTOR_1_B_L_PIN);
			} else if(valor<0){
				MOTOR_1_B_H_REG=-valor;
				MOTOR_1_A_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_1_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_1_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_1_A_H_PORT, MOTOR_1_A_H_PIN));
				GPIO_SetBits(MOTOR_1_A_L_PORT, MOTOR_1_A_L_PIN);
			} else {
				MOTOR_1_A_H_REG=MOTOR_MINIMO;
				MOTOR_1_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_1_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_1_B_H_TIM,TIM_EventSource_Update);
			}
			return TRUE;
		case 2:/* Inserido Navarro */
			GPIO_ResetBits(MOTOR_2_A_L_PORT, MOTOR_2_A_L_PIN);
			GPIO_ResetBits(MOTOR_2_B_L_PORT, MOTOR_2_B_L_PIN);
			if(valor>0){
				MOTOR_2_A_H_REG=valor;
				MOTOR_2_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_2_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_2_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_2_B_H_PORT, MOTOR_2_B_H_PIN));
				GPIO_SetBits(MOTOR_2_B_L_PORT, MOTOR_2_B_L_PIN);
			} else if(valor<0){
				MOTOR_2_B_H_REG=-valor;
				MOTOR_2_A_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_2_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_2_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_2_A_H_PORT, MOTOR_2_A_H_PIN));
				GPIO_SetBits(MOTOR_2_A_L_PORT, MOTOR_2_A_L_PIN);
			} else {
				MOTOR_2_A_H_REG=MOTOR_MINIMO;
				MOTOR_2_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_2_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_2_B_H_TIM,TIM_EventSource_Update);
			}
			return TRUE;
		case 3:/* Inserido Navarro */
			GPIO_ResetBits(MOTOR_3_A_L_PORT, MOTOR_3_A_L_PIN);
			GPIO_ResetBits(MOTOR_3_B_L_PORT, MOTOR_3_B_L_PIN);
			if(valor>0){
				MOTOR_3_A_H_REG=valor;
				MOTOR_3_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_3_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_3_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_3_B_H_PORT, MOTOR_3_B_H_PIN));
				GPIO_SetBits(MOTOR_3_B_L_PORT, MOTOR_3_B_L_PIN);
			} else if(valor<0){
				MOTOR_3_B_H_REG=-valor;
				MOTOR_3_A_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_3_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_3_B_H_TIM,TIM_EventSource_Update);
				while(!GPIO_ReadInputDataBit(MOTOR_3_A_H_PORT, MOTOR_3_A_H_PIN));
				GPIO_SetBits(MOTOR_3_A_L_PORT, MOTOR_3_A_L_PIN);
			} else {
				MOTOR_3_A_H_REG=MOTOR_MINIMO;
				MOTOR_3_B_H_REG=MOTOR_MINIMO;
				TIM_GenerateEvent(MOTOR_3_A_H_TIM,TIM_EventSource_Update);
				TIM_GenerateEvent(MOTOR_3_B_H_TIM,TIM_EventSource_Update);
			}
			return TRUE;
		default:
			return FALSE;
	}
}


u16 motor_get_posicao(u8 motor){
	switch(motor){
		case 0:
			return MOTOR_0_POSICAO;/* Dúvida */
		case 1:
			return MOTOR_1_POSICAO;/* Dúvida */
		default:
			return 0;
	}
}

s16 motor_get_tensao(u8 motor){
	switch(motor){
		case 0:
			if(MOTOR_0_B_H_REG>0) return MOTOR_0_B_H_REG;
			else if(MOTOR_0_A_H_REG>0) return -MOTOR_0_A_H_REG;
			else return 0;
		case 1:
			if(MOTOR_1_B_H_REG>0) return MOTOR_1_B_H_REG;
			else if(MOTOR_1_A_H_REG>0) return -MOTOR_1_A_H_REG;
			else return 0;
		case 2:/* Inserido Navarro */
			if(MOTOR_2_B_H_REG>0) return MOTOR_2_B_H_REG;
			else if(MOTOR_2_A_H_REG>0) return -MOTOR_2_A_H_REG;
			else return 0;
		case 3:/* Inserido Navarro */
			if(MOTOR_3_B_H_REG>0) return MOTOR_3_B_H_REG;
			else if(MOTOR_3_A_H_REG>0) return -MOTOR_3_A_H_REG;
			else return 0;
		default:
			return 0;
	}
}

/* Modificado Navarro */
void motor_inicializar(){
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);

	/*
#ifndef DEBUG_JTAG
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);
#endif

	GPIO_PinRemapConfig(GPIO_PartialRemap_TIM1,ENABLE);
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2,ENABLE);
(Bramigk) I think that GPIO_PinRemapConfig dont exist anymore because of the following:

	Note:
The locked registers are GPIOx_MODER, GPIOx_OTYPER, GPIOx_OSPEEDR, GPIOx_PUPDR, GPIOx_AFRL and GPIOx_AFRH.
The configuration of the locked GPIO pins can no longer be modified until the next reset.


*/

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);




	/* Encoder Motor 0*/
	TIM_EncoderInterfaceConfig(TIM2,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM2,ENABLE);

	/* Encoder Motor 1*/
	TIM_EncoderInterfaceConfig(TIM3,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM3,ENABLE);

	/* Encoder Motor 2*/
	TIM_EncoderInterfaceConfig(TIM4,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM4,ENABLE);

	/* Encoder Motor 3*/
	TIM_EncoderInterfaceConfig(TIM5,TIM_EncoderMode_TI12,TIM_ICPolarity_Rising,TIM_ICPolarity_Rising);
	TIM_Cmd(TIM5,ENABLE);



	/* Configuração Motor 0 */
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_A_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_0_A_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_B_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_0_B_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_A_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_0_A_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_0_A_L_PORT, MOTOR_0_A_L_PIN);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_B_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_0_B_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_0_B_L_PORT, MOTOR_0_B_L_PIN);

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/72000000)-1;
	TIM_TimeBaseStructure.TIM_Period=1000;

	TIM_TimeBaseInit(MOTOR_0_A_H_TIM,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(MOTOR_0_B_H_TIM,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;

	/* Configuração do Canais do Motor 0*/
	TIM_OC1Init(MOTOR_0_A_H_TIM,&TIM_OCInitStructure);
	TIM_OC4Init(MOTOR_0_B_H_TIM,&TIM_OCInitStructure);

	TIM_OC1PreloadConfig(MOTOR_0_A_H_TIM,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(MOTOR_0_B_H_TIM,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(MOTOR_0_A_H_TIM, ENABLE);
	TIM_ARRPreloadConfig(MOTOR_0_B_H_TIM, ENABLE);

	TIM_Cmd(MOTOR_0_A_H_TIM,ENABLE);
	TIM_Cmd(MOTOR_0_B_H_TIM,ENABLE);

	TIM_CtrlPWMOutputs(MOTOR_0_A_H_TIM,ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_0_B_H_TIM,ENABLE);


	/* Configuração Motor 1 */

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_1_A_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_1_A_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_1_B_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_1_B_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_1_A_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_1_A_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_1_A_L_PORT, MOTOR_1_A_L_PIN);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_1_B_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_1_B_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_1_B_L_PORT, MOTOR_1_B_L_PIN);

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/24000000)-1;
	TIM_TimeBaseStructure.TIM_Period=1000;

	TIM_TimeBaseInit(MOTOR_1_A_H_TIM, &TIM_TimeBaseStructure);
	TIM_TimeBaseInit(MOTOR_1_B_H_TIM, &TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCNPolarity=TIM_OCNPolarity_Low;
	TIM_OCInitStructure.TIM_OCNIdleState=TIM_OCNIdleState_Set;

	//TODO Criar define para selecionar o canal
	/* Configuração dos Canais do Motor 1*/
	TIM_OC2Init(MOTOR_1_A_H_TIM, &TIM_OCInitStructure);
	TIM_OC3Init(MOTOR_1_B_H_TIM, &TIM_OCInitStructure);

	TIM_OC2PreloadConfig(MOTOR_1_A_H_TIM,TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(MOTOR_1_B_H_TIM,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(MOTOR_1_A_H_TIM, ENABLE);
	TIM_ARRPreloadConfig(MOTOR_1_B_H_TIM, ENABLE);

	TIM_Cmd(MOTOR_1_A_H_TIM,ENABLE);
	TIM_Cmd(MOTOR_1_B_H_TIM,ENABLE);

	TIM_CtrlPWMOutputs(MOTOR_1_A_H_TIM,ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_1_B_H_TIM,ENABLE);


	/* Configuração Motor 2 */
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_2_A_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_2_A_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_2_B_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_2_B_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_2_A_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_2_A_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_2_A_L_PORT, MOTOR_2_A_L_PIN);

// GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_2_B_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_2_B_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_2_B_L_PORT, MOTOR_2_B_L_PIN);

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/72000000)-1;
	TIM_TimeBaseStructure.TIM_Period=1000;

	TIM_TimeBaseInit(MOTOR_2_A_H_TIM,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(MOTOR_2_B_H_TIM,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;

	/* Configuração do Canais do Motor 2*/
	TIM_OC1Init(MOTOR_2_A_H_TIM,&TIM_OCInitStructure);
	TIM_OC2Init(MOTOR_2_B_H_TIM,&TIM_OCInitStructure);

	TIM_OC1PreloadConfig(MOTOR_2_A_H_TIM,TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(MOTOR_2_B_H_TIM,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(MOTOR_2_A_H_TIM, ENABLE);
	TIM_ARRPreloadConfig(MOTOR_2_B_H_TIM, ENABLE);

	TIM_Cmd(MOTOR_2_A_H_TIM,ENABLE);
	TIM_Cmd(MOTOR_2_B_H_TIM,ENABLE);

	TIM_CtrlPWMOutputs(MOTOR_2_A_H_TIM,ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_2_B_H_TIM,ENABLE);

	/* Configuração Motor 3 */
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_3_A_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_3_A_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_3_B_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_3_B_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_3_A_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_3_A_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_3_A_L_PORT, MOTOR_3_A_L_PIN);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_3_B_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_3_B_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_3_B_L_PORT, MOTOR_3_B_L_PIN);

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/72000000)-1;
	TIM_TimeBaseStructure.TIM_Period=1000;

	TIM_TimeBaseInit(MOTOR_3_A_H_TIM,&TIM_TimeBaseStructure);
	TIM_TimeBaseInit(MOTOR_3_B_H_TIM,&TIM_TimeBaseStructure);

	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState=TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCIdleState=TIM_OCIdleState_Set;

	/* Configuração do Canais do Motor 3*/
	TIM_OC3Init(MOTOR_3_A_H_TIM,&TIM_OCInitStructure);
	TIM_OC4Init(MOTOR_3_B_H_TIM,&TIM_OCInitStructure);

	TIM_OC3PreloadConfig(MOTOR_3_A_H_TIM,TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(MOTOR_3_B_H_TIM,TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(MOTOR_3_A_H_TIM, ENABLE);
	TIM_ARRPreloadConfig(MOTOR_3_B_H_TIM, ENABLE);

	TIM_Cmd(MOTOR_3_A_H_TIM,ENABLE);
	TIM_Cmd(MOTOR_3_B_H_TIM,ENABLE);

	TIM_CtrlPWMOutputs(MOTOR_3_A_H_TIM,ENABLE);
	TIM_CtrlPWMOutputs(MOTOR_3_B_H_TIM,ENABLE);

	pidInit(&cont_m0,10,2,0,MOTOR_MAXIMO,MOTOR_MAXIMO);
	pidInit(&cont_m1,10,2,0,MOTOR_MAXIMO,MOTOR_MAXIMO);
	pidInit(&cont_m2,10,2,0,MOTOR_MAXIMO,MOTOR_MAXIMO);
	pidInit(&cont_m3,10,2,0,MOTOR_MAXIMO,MOTOR_MAXIMO);

}
