/*
 * PIDControl.h
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "GPIO.h"
#include "Pwm.h"
#include "Encoder.h"
#include "TimerTime2.h"

#ifndef MOTOR_H_
#define MOTOR_H_

class Motor {
public:
	Motor(Pwm *A_High,
			GPIO *A_Low,
			Pwm *B_High,
			GPIO *B_Low,
			Encoder *Enc,
			Timer_Time2 *MTimer);
	void Control_Pos(float hold_position);
	void Control_Speed(float hold_speed);
	void SetDutyCycle(int16_t answer);
	static void SetPID(float p, float i, float d);
private:
	Pwm *Motor_A_High;
	Pwm *Motor_B_High;
	GPIO *Motor_A_Low;
	GPIO *Motor_B_Low;
	Encoder *Motor_Enc;
	Timer_Time2 *Motor_Time;
	int16_t Pos_Calc_Answer(uint32_t, uint32_t);
	int16_t Spe_Calc_Answer(int32_t, int32_t);
	int16_t Pos_Last_Error[20];
	float Speed_Last_Error[10];
	uint32_t last_time;
	double last_vel_answer;
	int16_t last_speed_pos;

	int16_t last_position;
	int16_t dutycycle;
	float lasterror;
	float error;
	float derror;
	float ierror;
	static float cp;
	static float cd;
	static float ci;


};
#endif /* MOTOR_H_ */
