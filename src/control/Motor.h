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
#include "TimerTime.h"

#ifndef MOTOR_H_
#define MOTOR_H_

class Motor {
public:
	Motor(Pwm *A_High,
			GPIO *A_Low,
			Pwm *B_High,
			GPIO *B_Low,
			Encoder *Enc,
			Timer_Time *Time);
	void Control_Pos(uint32_t hold_position);
	void Control_Speed(int16_t hold_speed);
	void Answer(int16_t answer);
private:
	Pwm *Motor_A_High;
	Pwm *Motor_B_High;
	GPIO *Motor_A_Low;
	GPIO *Motor_B_Low;
	Encoder *Motor_Enc;
	Timer_Time *Motor_Time;
	int16_t Pos_Calc_Answer(uint32_t, uint32_t);
	int16_t Spe_Calc_Answer(int32_t, int32_t);
	int16_t Pos_Last_Error[20];
	int16_t Speed_Last_Error[20];
	uint32_t last_position;
	uint32_t last_time;
};
#endif /* MOTOR_H_ */
