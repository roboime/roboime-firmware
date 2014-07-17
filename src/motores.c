#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"
#include <stdio.h>
#include <math.h>
#include "arm_math.h"

#include "config.h"
#include "motores.h"
#include "pid.h"
#include "timer.h"
#include "leds.h"



//Define Motor 0
#define MOTOR_0_TIM TIM1

#define MOTOR_0_A_H_PORT GPIOA
#define MOTOR_0_A_L_PORT GPIOE
#define MOTOR_0_B_H_PORT GPIOE
#define MOTOR_0_B_L_PORT GPIOE

#define MOTOR_0_A_H_PIN GPIO_Pin_8
#define MOTOR_0_A_L_PIN GPIO_Pin_0
#define MOTOR_0_B_H_PIN GPIO_Pin_14
#define MOTOR_0_B_L_PIN GPIO_Pin_1

#define MOTOR_0_A_H_TIM TIM1
#define MOTOR_0_B_H_TIM TIM1

#define MOTOR_0_A_H_REG TIM1->CCR1
#define MOTOR_0_B_H_REG TIM1->CCR4

#define MOTOR_0_A_H_AF_PIN GPIO_PinSource8
#define MOTOR_0_A_H_AF GPIO_AF_TIM1
#define MOTOR_0_B_H_AF_PIN GPIO_PinSource14
#define MOTOR_0_B_H_AF GPIO_AF_TIM1

#define MOTOR_0_ENC_A_PIN GPIO_Pin_15
#define MOTOR_0_ENC_B_PIN GPIO_Pin_3
#define MOTOR_0_ENC_A_PORT GPIOA
#define MOTOR_0_ENC_B_PORT GPIOB

#define MOTOR_0_ENC_A_AF_PIN GPIO_PinSource15
#define MOTOR_0_ENC_A_AF GPIO_AF_TIM2
#define MOTOR_0_ENC_B_AF_PIN GPIO_PinSource3
#define MOTOR_0_ENC_B_AF GPIO_AF_TIM2






//Define Motor 1
#define MOTOR_1_TIM TIM1

#define MOTOR_1_A_H_PORT GPIOB
#define MOTOR_1_A_L_PORT GPIOE
#define MOTOR_1_B_H_PORT GPIOB
#define MOTOR_1_B_L_PORT GPIOB

#define MOTOR_1_A_H_PIN GPIO_Pin_0
#define MOTOR_1_A_L_PIN GPIO_Pin_2
#define MOTOR_1_B_H_PIN GPIO_Pin_1
#define MOTOR_1_B_L_PIN GPIO_Pin_2

#define MOTOR_1_A_H_TIM TIM1
#define MOTOR_1_B_H_TIM TIM1

#define MOTOR_1_A_H_REG TIM1->CCR2
#define MOTOR_1_B_H_REG TIM1->CCR3

#define MOTOR_1_A_H_AF_PIN GPIO_PinSource0
#define MOTOR_1_A_H_AF GPIO_AF_TIM1
#define MOTOR_1_B_H_AF_PIN GPIO_PinSource1
#define MOTOR_1_B_H_AF GPIO_AF_TIM1

#define MOTOR_1_ENC_A_PIN GPIO_Pin_4
#define MOTOR_1_ENC_B_PIN GPIO_Pin_5
#define MOTOR_1_ENC_A_PORT GPIOB
#define MOTOR_1_ENC_B_PORT GPIOB

#define MOTOR_1_ENC_A_AF_PIN GPIO_PinSource4
#define MOTOR_1_ENC_A_AF GPIO_AF_TIM3
#define MOTOR_1_ENC_B_AF_PIN GPIO_PinSource5
#define MOTOR_1_ENC_B_AF GPIO_AF_TIM3



//Define Motor 2
#define MOTOR_2_TIM TIM8

#define MOTOR_2_A_H_PORT GPIOC
#define MOTOR_2_A_L_PORT GPIOE
#define MOTOR_2_B_H_PORT GPIOC
#define MOTOR_2_B_L_PORT GPIOE

#define MOTOR_2_A_H_PIN GPIO_Pin_6
#define MOTOR_2_A_L_PIN GPIO_Pin_8
#define MOTOR_2_B_H_PIN GPIO_Pin_7
#define MOTOR_2_B_L_PIN GPIO_Pin_9

#define MOTOR_2_A_H_TIM TIM8
#define MOTOR_2_B_H_TIM TIM8

#define MOTOR_2_A_H_REG TIM8->CCR1
#define MOTOR_2_B_H_REG TIM8->CCR2


#define MOTOR_2_A_H_AF_PIN GPIO_PinSource6
#define MOTOR_2_A_H_AF GPIO_AF_TIM8
#define MOTOR_2_B_H_AF_PIN GPIO_PinSource7
#define MOTOR_2_B_H_AF GPIO_AF_TIM8

#define MOTOR_2_ENC_A_PIN GPIO_Pin_6
#define MOTOR_2_ENC_B_PIN GPIO_Pin_7
#define MOTOR_2_ENC_A_PORT GPIOB
#define MOTOR_2_ENC_B_PORT GPIOB

#define MOTOR_2_ENC_A_AF_PIN GPIO_PinSource6
#define MOTOR_2_ENC_A_AF GPIO_AF_TIM4
#define MOTOR_2_ENC_B_AF_PIN GPIO_PinSource7
#define MOTOR_2_ENC_B_AF GPIO_AF_TIM4


//Define Motor 3
#define MOTOR_3_TIM TIM8

#define MOTOR_3_A_H_PORT GPIOC
#define MOTOR_3_A_L_PORT GPIOE
#define MOTOR_3_B_H_PORT GPIOC
#define MOTOR_3_B_L_PORT GPIOE

#define MOTOR_3_A_H_PIN GPIO_Pin_8
#define MOTOR_3_A_L_PIN GPIO_Pin_3
#define MOTOR_3_B_H_PIN GPIO_Pin_9
#define MOTOR_3_B_L_PIN GPIO_Pin_12

#define MOTOR_3_A_H_TIM TIM8
#define MOTOR_3_B_H_TIM TIM8

#define MOTOR_3_A_H_REG TIM8->CCR3
#define MOTOR_3_B_H_REG TIM8->CCR4

#define MOTOR_3_A_H_AF_PIN GPIO_PinSource8
#define MOTOR_3_A_H_AF GPIO_AF_TIM8
#define MOTOR_3_B_H_AF_PIN GPIO_PinSource9
#define MOTOR_3_B_H_AF GPIO_AF_TIM8

#define MOTOR_3_ENC_A_PIN GPIO_Pin_0
#define MOTOR_3_ENC_B_PIN GPIO_Pin_1
#define MOTOR_3_ENC_A_PORT GPIOA
#define MOTOR_3_ENC_B_PORT GPIOA

#define MOTOR_3_ENC_A_AF_PIN GPIO_PinSource0
#define MOTOR_3_ENC_A_AF GPIO_AF_TIM5
#define MOTOR_3_ENC_B_AF_PIN GPIO_PinSource1
#define MOTOR_3_ENC_B_AF GPIO_AF_TIM5


#define MOTOR_MINIMO 0
#define MOTOR_MAXIMO 1000

#define VEL_LINEAR_MAXIMA 1.0f
#define VEL_ANGULAR_MAXIMA 8.0f
#define TETA0  60*2*PI/360
#define TETA1  135*2*PI/360
#define TETA2 -135*2*PI/360
#define TETA3 -60*2*PI/360

//TODO Remover
#define MOTOR_0_POSICAO TIM_GetCounter(TIM2)
#define MOTOR_1_POSICAO TIM_GetCounter(TIM3)
#define MOTOR_2_POSICAO TIM_GetCounter(TIM4)
#define MOTOR_3_POSICAO TIM_GetCounter(TIM5)


#define TRUE 1
#define FALSE 0

CONTROLADOR_S cont_m1, cont_m0, cont_m2, cont_m3;

CONTROLADOR_S pid_x, pid_y, pid_teta;

POSICAO_S P;

int ponto_alcancado()
{
	float raio=sqrt((P.Xreal-P.Xesperado)*(P.Xreal-P.Xesperado) + (P.Yreal-P.Yesperado)*(P.Yreal-P.Yesperado));
	if(raio<0.01 && abs(P.TETAesperado-P.TETAreal)<0.01) return 1;
	else return 0;

}


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
//-160 até 160
u8 motor_velocidade(u8 motor, float vel){
	vel=vel*160/128;
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

	/* Tentando melhorar as coisas...
	 * Uma volta do motor possui 400 passos no encoder...
	 * A relação entre a engrenagem do motor e a engrenagem da roda é 1:8
	 * logo:
	 * Velocidade Angular=delta omega / delta tempo = (2*PI/400*8)/(1/freq) = 2*PI*freq/3200
	 *
	 * Matriz Omni
	 *
	 * |    -sin(60)/r    cos(60)/r      d/r   |
	 * |   -sin(135)/r    cos(135)/r      d/r   |
	 * |   -sin(-135)/r  cos(-135)/r     d/r   |
	 * |   -sin(-60)/r    cos(-60)/r     d/r   |
	 *
	 * Onde 'd' é a distância do centro do robô para a roda e 'r' é o raio da roda
	 *
	 * d=0.085 metros
	 * r=0.0289 metros
	 *
	 * Entrada: Velocidade x,y lineares e teta
	 * Saida: Velocidade Angular de cada roda
	 *
	 * Matriz Omni Inversa (levando em conta v0,v1 e v2)
	 *
	 * |Vx|   | 0         -0.020435   0.020435 | |v0|
	 * |Vy| = | 0.023941  -0.026632   0.002691 |*|v1|
	 * |Vo|   | 0.199167  -0.051550   0.192383 | |v2|
	 *
	 *
	 */


	//Comentando para tentar tornar os parâmetros mais exatos

/*
	float veloc0=(float)velocidade[0]*Get_Freq()/3920.0;
	cont_m0.realimentacao=(s16)(veloc0*10);

	float veloc1=(float)velocidade[1]*Get_Freq()/3920.0;
	cont_m1.realimentacao=(s16)(veloc1*10);

	float veloc2=(float)velocidade[2]*Get_Freq()/3920.0;
	cont_m2.realimentacao=(s16)(veloc2*10);

	float veloc3=(float)velocidade[3]*Get_Freq()/3920.0;
	cont_m3.realimentacao=(s16)(veloc3*10);*/


	//calculo das velocidades lidas de cada roda em rad/s

	float veloc0=(float)velocidade[0]*Get_Freq()*2*PI/3200.0;
	cont_m0.realimentacao=(s16)(veloc0);

	float veloc1=(float)velocidade[1]*Get_Freq()*2*PI/3200.0;
	cont_m1.realimentacao=(s16)(veloc1);

	float veloc2=(float)velocidade[2]*Get_Freq()*2*PI/3200.0;
	cont_m2.realimentacao=(s16)(veloc2);

	float veloc3=(float)velocidade[3]*Get_Freq()*2*PI/3200.0;
	cont_m3.realimentacao=(s16)(veloc3);


	//calculo de vx, vy e vteta em função das velocidades lidas




	P.Vx=      -(-0.0100*cont_m0.realimentacao     -    0.0082*cont_m1.realimentacao    +         0.0082*cont_m2.realimentacao   +         0.0100*cont_m3.realimentacao);
	P.Vy=      -(0.0120*cont_m0.realimentacao     -    0.0120*cont_m1.realimentacao    -         0.0120*cont_m2.realimentacao   +         0.0120*cont_m3.realimentacao);
	P.Vteta =  -(0.0996*cont_m0.realimentacao     +    0.0704*cont_m1.realimentacao    +         0.0704*cont_m2.realimentacao   +         0.0996*cont_m3.realimentacao);




	//Atualiza posição angular integrando velocidade angular

	P.TETAreal = P.TETAreal +    P.Vteta*(1.0/(float)Get_Freq());

	//while(P.TETAreal>3.14) P.TETAreal-=2*PI;
	//while(P.TETAreal<-3.14) P.TETAreal+=2*PI;

	//encontra sin e cos de teta uma vez para não ter a necessidade de recalcular em cada calculo

	P.TETAreal_rad  = (float32_t)P.TETAreal;
	P.cos_teta_real = arm_cos_f32(P.TETAreal_rad);
	P.sin_teta_real = arm_sin_f32(P.TETAreal_rad);


	//Rotaciona a velocidade do robo para o refencial do campo

	P.Vx_Ref_Campo =  P.Vx*P.cos_teta_real  -   P.Vy*P.sin_teta_real;
	P.Vy_Ref_Campo =  P.Vx*P.sin_teta_real  +   P.Vy*P.cos_teta_real;

	//integra velocidade para atualizar posição do robo

	P.Xreal    = P.Xreal    +    P.Vx_Ref_Campo*(1/(float)Get_Freq());
	P.Yreal    = P.Yreal    +    P.Vy_Ref_Campo*(1/(float)Get_Freq());


	//entradas dos PIDs

	pid_x.entrada = P.Xesperado;
	pid_x.realimentacao = P.Xreal;

	pid_y.entrada = P.Yesperado;
	pid_y.realimentacao = P.Yreal;

	pid_teta.entrada = P.TETAesperado;
	pid_teta.realimentacao = P.TETAreal;

	//calculo de saídas dos PIDs de posição

	pidService(&pid_x);
	pidService(&pid_y);
	pidServiceAngulo(&pid_teta);



	P.Vx_Ref_Robo =     pid_x.saida * P.cos_teta_real + pid_y.saida * P.sin_teta_real;
	P.Vy_Ref_Robo =   - pid_x.saida * P.sin_teta_real + pid_y.saida * P.cos_teta_real;

	//saida dos PIDs de posicao como entrada dos PIDs de velocidade

	cont_m0.entrada     =     - (-29.96628 * P.Vx_Ref_Robo   +   17.30104 * P.Vy_Ref_Robo  +  2.94118 * pid_teta.saida);
	cont_m1.entrada     =     - (-24.46736 * P.Vx_Ref_Robo   -   24.46736 * P.Vy_Ref_Robo  +  2.94118 * pid_teta.saida);
	cont_m2.entrada     =     - ( 24.46736 * P.Vx_Ref_Robo   -   24.46736 * P.Vy_Ref_Robo  +  2.94118 * pid_teta.saida);
	cont_m3.entrada     =     - ( 29.96628 * P.Vx_Ref_Robo   +   17.30104 * P.Vy_Ref_Robo  +  2.94118 * pid_teta.saida);

	pidService(&cont_m0);
	pidService(&cont_m1);
	pidService(&cont_m2);
	pidService(&cont_m3);

	motor_tensao(0,(s16)cont_m0.saida);
	motor_tensao(1,(s16)cont_m1.saida);
	motor_tensao(2,(s16)cont_m2.saida);
	motor_tensao(3,(s16)cont_m3.saida);



	//static int temp=0;
	//if((temp++)%50==0){
	//	display_matlab(veloc0, veloc1, veloc2, veloc3);
	//}

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
			return TRUE;
		case 3:/* Inserido Navarro */
			GPIO_ResetBits(MOTOR_3_A_L_PORT, MOTOR_3_A_L_PIN);
			GPIO_ResetBits(MOTOR_3_B_L_PORT, MOTOR_3_B_L_PIN);
			MOTOR_3_A_H_REG=MOTOR_3_B_H_REG=MOTOR_MAXIMO;
			TIM_GenerateEvent(MOTOR_3_A_H_TIM,TIM_EventSource_Update);
			TIM_GenerateEvent(MOTOR_3_B_H_TIM,TIM_EventSource_Update);
			return TRUE;
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
				MOTOR_2_B_H_REG=(-1)*valor;
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
			return MOTOR_0_POSICAO;
		case 1:
			return MOTOR_1_POSICAO;
		case 2:
			return MOTOR_2_POSICAO;
		case 3:
			return MOTOR_3_POSICAO;

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

	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE); pensar nisso!!!!!
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8,ENABLE);



//Configurando GPIO's que funcionarão lendo o ENCODER


	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_ENC_A_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(MOTOR_0_ENC_A_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=MOTOR_0_ENC_B_PIN;
	GPIO_Init(MOTOR_0_ENC_B_PORT,&GPIO_InitStructure);

	GPIO_PinAFConfig(MOTOR_0_ENC_A_PORT, MOTOR_0_ENC_A_AF_PIN, MOTOR_0_ENC_A_AF);
	GPIO_PinAFConfig(MOTOR_0_ENC_B_PORT, MOTOR_0_ENC_B_AF_PIN, MOTOR_0_ENC_B_AF);

	GPIO_InitStructure.GPIO_Pin=MOTOR_1_ENC_A_PIN;
	GPIO_Init(MOTOR_1_ENC_A_PORT,&GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin=MOTOR_1_ENC_B_PIN;
	GPIO_Init(MOTOR_1_ENC_B_PORT,&GPIO_InitStructure);

	GPIO_PinAFConfig(MOTOR_1_ENC_A_PORT, MOTOR_1_ENC_A_AF_PIN, MOTOR_1_ENC_A_AF);
	GPIO_PinAFConfig(MOTOR_1_ENC_B_PORT, MOTOR_1_ENC_B_AF_PIN, MOTOR_1_ENC_B_AF);


	GPIO_InitStructure.GPIO_Pin=MOTOR_2_ENC_A_PIN;
	GPIO_Init(MOTOR_2_ENC_A_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=MOTOR_2_ENC_B_PIN;
	GPIO_Init(MOTOR_2_ENC_B_PORT,&GPIO_InitStructure);

	GPIO_PinAFConfig(MOTOR_2_ENC_A_PORT, MOTOR_2_ENC_A_AF_PIN, MOTOR_2_ENC_A_AF);
	GPIO_PinAFConfig(MOTOR_2_ENC_B_PORT, MOTOR_2_ENC_B_AF_PIN, MOTOR_2_ENC_B_AF);

	GPIO_InitStructure.GPIO_Pin=MOTOR_3_ENC_A_PIN;
	GPIO_Init(MOTOR_3_ENC_A_PORT,&GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin=MOTOR_3_ENC_B_PIN;
	GPIO_Init(MOTOR_3_ENC_B_PORT,&GPIO_InitStructure);

	GPIO_PinAFConfig(MOTOR_3_ENC_A_PORT, MOTOR_3_ENC_A_AF_PIN, MOTOR_3_ENC_A_AF);
	GPIO_PinAFConfig(MOTOR_3_ENC_B_PORT, MOTOR_3_ENC_B_AF_PIN, MOTOR_3_ENC_B_AF);











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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MOTOR_0_A_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_B_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MOTOR_0_B_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_A_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(MOTOR_0_A_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_0_A_L_PORT, MOTOR_0_A_L_PIN);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_0_B_L_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(MOTOR_0_B_L_PORT,&GPIO_InitStructure);
	GPIO_ResetBits(MOTOR_0_B_L_PORT, MOTOR_0_B_L_PIN);

	GPIO_PinAFConfig(MOTOR_0_A_H_PORT, MOTOR_0_A_H_AF_PIN, MOTOR_0_A_H_AF);
	GPIO_PinAFConfig(MOTOR_0_B_H_PORT, MOTOR_0_B_H_AF_PIN, MOTOR_0_B_H_AF);





	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/168000000)-1;
	TIM_TimeBaseStructure.TIM_Period=168000000/168000;

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
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(MOTOR_1_A_H_PORT,&GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin=MOTOR_1_B_H_PIN;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
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


	GPIO_PinAFConfig(MOTOR_1_A_H_PORT, MOTOR_1_A_H_AF_PIN, MOTOR_1_A_H_AF);
	GPIO_PinAFConfig(MOTOR_1_B_H_PORT, MOTOR_1_B_H_AF_PIN, MOTOR_1_B_H_AF);


	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/168000000)-1;
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

	GPIO_PinAFConfig(MOTOR_2_A_H_PORT, MOTOR_2_A_H_AF_PIN, MOTOR_2_A_H_AF);
	GPIO_PinAFConfig(MOTOR_2_B_H_PORT, MOTOR_2_B_H_AF_PIN, MOTOR_2_B_H_AF);

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/168000000)-1;
	TIM_TimeBaseStructure.TIM_Period=168000000/168000;

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

	GPIO_PinAFConfig(MOTOR_3_A_H_PORT, MOTOR_3_A_H_AF_PIN, MOTOR_3_A_H_AF);
	GPIO_PinAFConfig(MOTOR_3_B_H_PORT, MOTOR_3_B_H_AF_PIN, MOTOR_3_B_H_AF);

	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Prescaler=(SystemCoreClock/168000000)-1;
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

	posInit(&P);

	pidInit(&cont_m0,20,0.4,0,MOTOR_MAXIMO,MOTOR_MAXIMO);
	pidInit(&cont_m1,20,0.4,0,MOTOR_MAXIMO,MOTOR_MAXIMO);
	pidInit(&cont_m2,20,0.4,0,MOTOR_MAXIMO,MOTOR_MAXIMO);
	pidInit(&cont_m3,20,0.4,0,MOTOR_MAXIMO,MOTOR_MAXIMO);

	pidInit(&pid_x,30,0.0001,2,VEL_LINEAR_MAXIMA,VEL_LINEAR_MAXIMA);
	pidInit(&pid_y,30,0.0001,2,VEL_LINEAR_MAXIMA,VEL_LINEAR_MAXIMA);
	pidInit(&pid_teta,60,0,0,VEL_ANGULAR_MAXIMA,VEL_ANGULAR_MAXIMA);



	//NewExpectedPosition(&P, 1, 1,10);

}

void NewExpectedPosition(float x, float y, float teta)
{
	P.Xesperado = x;
	P.Yesperado = y;
	P.TETAesperado = teta;
}

