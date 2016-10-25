/*
 * Motor.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "Motor.h"
#define rob2wheel

Motor::Motor(Pwm *A_High,
		GPIO *A_Low,
		Pwm *B_High,
		GPIO *B_Low,
		Encoder *Enc,
		Timer_Time2 *MTimer)
{
	Motor_A_High = A_High;
	Motor_A_Low = A_Low;
	Motor_B_High = B_High;
	Motor_B_Low = B_Low;
	Motor_Enc = Enc;
	Motor_Time = MTimer;
	last_vel_answer = 0;
}
void Motor::Control_Pos(uint32_t hold_position){
	uint32_t position;
	int16_t answer;
	position = Motor_Enc->get_position();
	answer = this->Pos_Calc_Answer(position, hold_position);	//this eh opcional por estar
																//pos_calc_answer estar dentro
																//do objeto
	this->Answer(answer);
	return;
};

void Motor::Control_Speed(int16_t hold_speed){
	//hold_speed em m/s
	int16_t speed;
	int16_t vel_answer;
	uint32_t position = Motor_Enc->get_position();
	Motor_Enc->set_position((uint32_t) 20000);
	speed = 300*((int16_t)position-20000);
	//300 transforma speed em unidade arbritaria da roda em m/s
	//agora tanto speed quando hold_speed estão na mesma unidade
    vel_answer = -Spe_Calc_Answer(speed, hold_speed);
	this->Answer((int16_t)vel_answer);
	return;
};
/*
void Motor::Control_Speed(int16_t hold_speed){
	int16_t speed;
	int16_t vel_answer;
	uint32_t position = Motor_Enc->get_position();
	Motor_Enc->set_position((uint32_t) 20000);
	speed = last_speed_pos -(int16_t)position-20000+hold_speed;
	Control_Pos((uint32_t)(20000+speed));
	last_speed_pos = speed;
	return;
};
*/
void Motor::Answer(int16_t answer)
{
	if (answer > 0)
	{
		if (answer>1000)
		{
			answer=1000;
		}
		Motor_A_Low->Reset();
		Motor_B_High->set_DutyCycle(0);
		while(Motor_A_Low->Status());
		Motor_A_High->set_DutyCycle(answer);
		Motor_B_Low->Set();
	}
	else
	{
		answer=-answer;
		if(answer>1000)
		{
			answer=1000;
		}
		Motor_B_Low->Reset();
		Motor_A_High->set_DutyCycle(0);
		while(Motor_B_Low->Status());
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
	return (int16_t) -((error)*0.81+(integral)*0.18 - derivative*2.25);
	//Kp = 0.81, Ki = 0.36, Kd = 2.25
}

//Falta definir o que é o valor speed, que deve ter sinal
int16_t Motor::Spe_Calc_Answer(int32_t speed, int32_t hold_speed){
	if(hold_speed>1000) //1000 é um numero magico
		hold_speed=1000;
	if(hold_speed<-1000)
		hold_speed=-1000;
	hold_speed = hold_speed*10;
	double vel_answer;
	float error;
	float derivative;
	float integral;
	error=speed-hold_speed;
	derivative = error - Speed_Last_Error[0];
	for(int i = 9; i>0; i--){
		Speed_Last_Error[i] = Speed_Last_Error[i-1];
	}
	Speed_Last_Error[0] = error;
	for(int i=0; i<10; i++){
		integral = integral + Speed_Last_Error[i];
	}
	vel_answer=last_vel_answer + error*0.004 + derivative*0;
	//Kp=0.004, Ki=0, Kd=0;
	if(vel_answer > 1000){
		vel_answer = 1000;
	}
	if(vel_answer < -1000){
		vel_answer = -1000;
	}
	last_vel_answer = vel_answer;
	return (int16_t) vel_answer;
}
