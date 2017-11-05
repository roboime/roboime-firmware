/*
 * Motor.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "Motor.h"

/*
 * Usou-se ziegler-nichols frequency response method
 * Kc=3.2
 * Tc =0.7s
 */
float Motor::cp=8000.0f;
float Motor::cd=1000.0f;
float Motor::ci=10.0f;


Motor::Motor(Pwm *A_High,
		GPIO *A_Low,
		Pwm *B_High,
		GPIO *B_Low,
		Encoder *Enc,
		INA220 *ina220)
{
	Motor_A_High = A_High;
	Motor_A_Low = A_Low;
	Motor_B_High = B_High;
	Motor_B_Low = B_Low;
	Motor_Enc = Enc;
	mina22 = ina220;
}
void Motor::Control_Pos(float  hold_position){
	uint32_t position;
	int16_t answer;
	position = Motor_Enc->get_position();
	answer = this->Pos_Calc_Answer(position, hold_position);	//this eh opcional por estar
																//pos_calc_answer estar dentro
																//do objeto
	this->SetDutyCycle(answer);
	return;
};

//será chamada pelo handler da interrupção gerada pelo TIM6(a cada 1 ms)
//hold_speed é a velocidade da RODA em m/s
void Motor::Control_Speed(float hold_speed){
	//position é medida em divisões de encoder (ou 1/400 de volta)
	int16_t position = (int16_t)Motor_Enc->get_position();
	Motor_Enc->set_position(20000);

	int16_t distance=position-20000;

	float speed=(float)distance*CONVERSION; //converte da unidade da roda para m/s (vel do centro da roda)
	                                     //talvez seja melhor converter de m/s pra unidade da roda
	real_wheel_speed=speed;

	error=hold_speed-speed;
	ierror = 0;
	for(int j = 18; j > 0; j--){
		last_error[j+1]=last_error[j];
		ierror = ierror + last_error[j+1];
	}
	last_error[0]=error;
	ierror = ierror + last_error[0];
	//if(ierror > 1000) ierror = 1000;
	//if(ierror < -1000) ierror = -1000;

	derror=error-last_error[1];

	float out=cp*error + ci * ierror + cd * derror;

	int16_t dutycycle = (int16_t)out;

	if(dutycycle>1000) dutycycle=1000;
	if(dutycycle<-1000) dutycycle=-1000;
	SetDutyCycle(dutycycle);

};

void Motor::SetDutyCycle(int16_t answer)
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

void Motor::SetPID(float p, float i, float d) {
	cp=p;
	ci=i;
	cd=d;
}

void Motor::GetPID(float c[]){
	c[0]=cp;
	c[1]=ci;
	c[2]=cd;
}
