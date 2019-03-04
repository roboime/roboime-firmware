/*
 * pins.h
 *
 *  Created on: Mar 14, 2016
 *      Author: lenovoi7
 */

#ifndef PINS_H_
#define PINS_H_

GPIO_TypeDef* MAH_Port[] = {GPIOC, GPIOA, GPIOC, GPIOE};//ok
//GPIO_TypeDef* MAL_Port[] = {GPIOE, GPIOE, GPIOC, GPIOB};
GPIO_TypeDef* MAL_Port[] = {GPIOD, GPIOE, GPIOC, GPIOB};
GPIO_TypeDef* MBH_Port[] = {GPIOC, GPIOC, GPIOE, GPIOE};
GPIO_TypeDef* MBL_Port[] = {GPIOC, GPIOE, GPIOB, GPIOB};

uint32_t MAH_Pin[] = {GPIO_Pin_9, GPIO_Pin_8, GPIO_Pin_6, GPIO_Pin_14};//ok
//uint32_t MAL_Pin[] = {GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_2, GPIO_Pin_12}; é necessario trocar o primeiro, ficando como mostrado embaixo
uint32_t MAL_Pin[] = {GPIO_Pin_7, GPIO_Pin_6, GPIO_Pin_2, GPIO_Pin_12};//ok
uint32_t MBH_Pin[] = {GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_11, GPIO_Pin_13};//ok
uint32_t MBL_Pin[] = {GPIO_Pin_13, GPIO_Pin_4, GPIO_Pin_1, GPIO_Pin_11};//ok

TIM_TypeDef * MAH_Tim[] = {TIM8, TIM1, TIM8, TIM1};
TIM_TypeDef * MBH_Tim[] = {TIM8, TIM8, TIM1, TIM1};

uint8_t MAH_Ch[] = {4, 1, 1, 4};
uint8_t MBH_Ch[] = {2, 3, 2, 3};

bool MAH_nState[] = {false, false, false, false};
bool MBH_nState[] = {false, false, false, false};

uint8_t MAH_Af[]= {GPIO_AF_TIM8, GPIO_AF_TIM1, GPIO_AF_TIM8, GPIO_AF_TIM1};
uint8_t MBH_Af[]= {GPIO_AF_TIM8, GPIO_AF_TIM8, GPIO_AF_TIM1, GPIO_AF_TIM1};

uint8_t MAH_Af_Pin[] = {GPIO_PinSource9, GPIO_PinSource8, GPIO_PinSource6, GPIO_PinSource14};
uint8_t MBH_Af_Pin[] = {GPIO_PinSource7, GPIO_PinSource8, GPIO_PinSource11, GPIO_PinSource13};

GPIO_TypeDef* M_EncA_Port[] = {GPIOB, GPIOA, GPIOA, GPIOB};//ok
GPIO_TypeDef* M_EncB_Port[] = {GPIOB, GPIOB, GPIOA, GPIOB};//ok

uint32_t M_EncA_Pin[] = {GPIO_Pin_4, GPIO_Pin_15, GPIO_Pin_0, GPIO_Pin_6};//ok
uint32_t M_EncB_Pin[] = {GPIO_Pin_5, GPIO_Pin_3, GPIO_Pin_1, GPIO_Pin_7};//ok

TIM_TypeDef * M_Enc_Tim[] = {TIM3, TIM2, TIM5, TIM4};

uint8_t M_Enc_Af[]= {GPIO_AF_TIM3, GPIO_AF_TIM2, GPIO_AF_TIM5, GPIO_AF_TIM4};

uint8_t M_EncA_Af_Pin[] = {GPIO_PinSource4, GPIO_PinSource15, GPIO_PinSource0, GPIO_PinSource6};
uint8_t M_EncB_Af_Pin[] = {GPIO_PinSource5, GPIO_PinSource3, GPIO_PinSource1, GPIO_PinSource7};


/*
 * CON-MC
 * 1-MBL
 * 2-MBH
 * 3-MAL
 * 4-MAH
 *
 * CON-MOTOR
 * 1-GND
 * 2-V+
 * 3-MB
 * 4-MA
 *
 * MOTOR
 * 1-PRETO-MA
 * 2-MB
 * 3-GND
 * 4-VCC
 * 5-ENC-B
 * 6-ENC-A
 *
 */

#endif /* PINS_H_ */
