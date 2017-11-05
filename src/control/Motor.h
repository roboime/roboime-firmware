/*
 * PIDControl.h
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "stm32f4xx.h"
#include "GPIO.h"
#include "Pwm.h"
#include "Encoder.h"
#include "hal/ina220.h"

#ifndef MOTOR_H_
#define MOTOR_H_

//valor aproximado do raio da roda omni, em metros
#define R_roda 0.028
#define PI 3.1415926
//tempo (em s) entre as interrupções geradas pelo TIM6
#define TIM6_delay 0.001
//número de divisões do encoder
#define ENC_DIV 400
//para cada 8 giros do motor, a roda gira 1 vez
#define FT_TRANS 8
#define CONVERSION 2*PI*R_roda/(TIM6_delay*ENC_DIV*FT_TRANS)

class Motor {
public:
	Motor(Pwm *A_High,
			GPIO *A_Low,
			Pwm *B_High,
			GPIO *B_Low,
			Encoder *Enc,
			INA220 *ina220);
	void Control_Pos(float hold_position);
	void Control_Speed(float hold_speed);
	void SetDutyCycle(int16_t answer);
	static void SetPID(float p, float i, float d);
	static void GetPID(float c[]);
	INA220 *mina22;

	float real_wheel_speed;
private:
	Pwm *Motor_A_High;
	Pwm *Motor_B_High;
	GPIO *Motor_A_Low;
	GPIO *Motor_B_Low;
	Encoder *Motor_Enc;
	int16_t Pos_Calc_Answer(uint32_t, uint32_t);
	int16_t Pos_Last_Error[20];

	float lasterror;
	float error;
	float derror;
	float last_error[20];
	float ierror;
	static float cp;
	static float cd;
	static float ci;


};
#endif /* MOTOR_H_ */
