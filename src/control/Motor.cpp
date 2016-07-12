/*
 * Motor.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "Motor.h"

Motor::Motor(Pwm *A_High,
		GPIO *A_Low,
		Pwm *B_High,
		GPIO *B_Low,
		Encoder *Enc,
		Timer_Time *Time)
{
	Motor_A_High = A_High;
	Motor_A_Low = A_Low;
	Motor_B_High = B_High;
	Motor_B_Low = B_Low;
	Motor_Enc = Enc;
	Motor_Time = Time;
}

void Motor::Control_Pos(uint32_t hold_position){
	uint32_t position;
	int16_t answer;
	position = Motor_Enc->position();
	answer = this->Pos_Calc_Answer(position, hold_position);	//this eh opcional por estar
																//pos_calc_answer estar dentro
																//do objeto
	this->Answer(answer);
	return;
};

void Motor::Control_Speed(int16_t hold_speed){
	int32_t speed;
	int16_t answer;
	uint32_t position = Motor_Enc->position();
	uint32_t time = Motor_Time->current_time();

	speed = (position - last_position)/(time - last_time);
	answer = this->Spe_Calc_Answer(speed, hold_speed);
	this->Answer(answer);
	last_position = position;
	last_time = time;
	return;
};

void Motor::Answer(int16_t answer)
{
	if (answer > 0)
	{
		if (answer>600)
		{
			answer=600;
		}
		Motor_A_Low->Reset();
		Motor_B_High->set_DutyCycle(0);
		Motor_A_High->set_DutyCycle(answer);
		Motor_B_Low->Set();
	}
	else
	{
		answer=-answer;
		if(answer>600)
		{
			answer=600;
		}
		Motor_B_Low->Reset();
		Motor_A_High->set_DutyCycle(0);
		Motor_B_High->set_DutyCycle(answer);
		Motor_A_Low->Set();
	}
	return;
}

int16_t Motor::Pos_Calc_Answer(uint32_t position, uint32_t hold_position)
{
	int16_t integral;
	int16_t error;
	int16_t derivative;
	int i;
//verde oliva
	error = (int16_t) (position - hold_position);
	for(i=0; i<19; i++){
		Pos_Last_Error[i]=Pos_Last_Error[i+1];
	}
	Pos_Last_Error[19] = error;
	integral=0;
	derivative=Pos_Last_Error[19]-Pos_Last_Error[18];

	for(i=0; i<20; i++){
		integral = integral+Pos_Last_Error[i];
	}
	if (integral > 600/0.18) integral = 600/0.18;
	return (int16_t) ((error)*0.81+(integral)*0.18 - derivative*2.25);
	//Kp = 0.81, Ki = 0.36, Kd = 2.25
}

//Falta definir o que é o valor speed, que deve ter sinal
int16_t Motor::Spe_Calc_Answer(int32_t speed, int32_t hold_speed){

	int16_t integral;
	int16_t error;
	int16_t derivative;
	int i;
//verde oliva
	error = (int16_t) (speed - hold_speed);
	for(i=0; i<19; i++){
		Speed_Last_Error[i]=Speed_Last_Error[i+1];
	}
	Speed_Last_Error[19] = error;
	integral=0;
	derivative=Speed_Last_Error[19]-Speed_Last_Error[18];

	for(i=0; i<20; i++){
		integral = integral+Speed_Last_Error[i];
	}
	if (integral > 600/0.18) integral = 600/0.18;
	return (int16_t) ((error)*0.81+(integral)*0.36 - derivative*2.25);
	//Kp = 0.81, Ki = 0.36, Kd = 2.25
}
