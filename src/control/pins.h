/*
 * pins.h
 *
 *  Created on: Mar 14, 2016
 *      Author: lenovoi7
 */

#ifndef PINS_H_
#define PINS_H_

GPIO_TypeDef* MAH_Port[] = {GPIOA, GPIOB, GPIOC, GPIOC};
GPIO_TypeDef* MAL_Port[] = {GPIOE, GPIOE, GPIOE, GPIOE};
GPIO_TypeDef* MBH_Port[] = {GPIOE, GPIOB, GPIOC, GPIOC};
GPIO_TypeDef* MBL_Port[] = {GPIOE, GPIOB, GPIOE, GPIOE};

uint32_t MAH_Pin[] = {GPIO_Pin_8, GPIO_Pin_0, GPIO_Pin_6, GPIO_Pin_8};
uint32_t MAL_Pin[] = {GPIO_Pin_0, GPIO_Pin_2, GPIO_Pin_8, GPIO_Pin_3};
uint32_t MBH_Pin[] = {GPIO_Pin_14, GPIO_Pin_1, GPIO_Pin_7, GPIO_Pin_9};
uint32_t MBL_Pin[] = {GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_9, GPIO_Pin_12};

TIM_TypeDef * MAH_Tim[] = {TIM1, TIM1, TIM8, TIM8};
TIM_TypeDef * MBH_Tim[] = {TIM1, TIM1, TIM8, TIM8};

uint8_t MAH_Ch[] = {1, 2, 1, 3};
uint8_t MBH_Ch[] = {4, 3, 2, 4};

bool MAH_nState[] = {false, true, false, false};
bool MBH_nState[] = {false, true, false, false};

uint8_t MAH_Af[]= {GPIO_AF_TIM1, GPIO_AF_TIM1, GPIO_AF_TIM8, GPIO_AF_TIM8};
uint8_t MBH_Af[]= {GPIO_AF_TIM1, GPIO_AF_TIM1, GPIO_AF_TIM8, GPIO_AF_TIM8};

uint8_t MAH_Af_Pin[] = {GPIO_PinSource8, GPIO_PinSource0, GPIO_PinSource6, GPIO_PinSource8};
uint8_t MBH_Af_Pin[] = {GPIO_PinSource14, GPIO_PinSource1, GPIO_PinSource7, GPIO_PinSource9};

GPIO_TypeDef* M_EncA_Port[] = {GPIOA, GPIOB, GPIOB, GPIOA};
GPIO_TypeDef* M_EncB_Port[] = {GPIOB, GPIOB, GPIOB, GPIOA};

uint32_t M_EncA_Pin[] = {GPIO_Pin_15, GPIO_Pin_4, GPIO_Pin_6, GPIO_Pin_0};
uint32_t M_EncB_Pin[] = {GPIO_Pin_3, GPIO_Pin_5, GPIO_Pin_7, GPIO_Pin_1};

TIM_TypeDef * M_Enc_Tim[] = {TIM2, TIM3, TIM4, TIM5};

uint8_t M_Enc_Af[]= {GPIO_AF_TIM2, GPIO_AF_TIM3, GPIO_AF_TIM4, GPIO_AF_TIM5};

uint8_t M_EncA_Af_Pin[] = {GPIO_PinSource15, GPIO_PinSource4, GPIO_PinSource6, GPIO_PinSource0};
uint8_t M_EncB_Af_Pin[] = {GPIO_PinSource3, GPIO_PinSource5, GPIO_PinSource7, GPIO_PinSource1};


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
