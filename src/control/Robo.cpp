/*
 * Robo.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "Robo.h"
#include "pins.h"

#define sin_phi 0.8480
#define cos_phi 0.52
#define sin_theta 0.7313
#define cos_theta -0.682

Robo::Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, uint8_t id, bool testmode):
	_id(id),
	_testmode(testmode)
{
	motors[0]=roboMotor0;
	motors[1]=roboMotor1;
	motors[2]=roboMotor2;
	motors[3]=roboMotor3;

	batAdc = new adc();
	batAdc->ADC_Config();

	drible = new dibre();

	high_kick = new GPIO(GPIOD, GPIO_Pin_8);
	chute_baixo = new GPIO(GPIOD, GPIO_Pin_10);
}
void Robo::HighKick(){
	high_kick->Set();
	for(int i=0;i<0xeee2;i++);
	high_kick->Reset();
}

void Robo::ChuteBaixo(){
	chute_baixo->Set();
	for(int i=0;i<0xeee2;i++);
	chute_baixo->Reset();
}

void Robo::control_pos(){
	for(int i=0; i<4; i++){
		motors[i]->Control_Pos(pos[1]);
	}
}
void Robo::control_speed(){
  vBat = 4.3*batAdc->adc_getConversion();
  if(vBat>6 || 1){
    for(int i=0; i<4; i++){
	  motors[i]->Control_Speed(speed[1]);
    }
  }
  else{
    for(int i=0; i<4; i++){
	  motors[i]->SetDutyCycle(0);
    }
  }
}
void Robo::set_speed(float v_r, float v_t, float w){
	float R = 0.06; //TODO valor temporario

	speed[0] = -v_r*cos_phi + v_t*sin_phi + w*R;
	speed[2] = -v_r*cos_phi - v_t*sin_phi + w*R;
	speed[3] = -v_r*cos_theta - v_t*sin_theta + w*R;
	speed[1] = -v_r*cos_theta + v_t*sin_theta + w*R;
	return;
}

void Robo::set_speed(float v[]){
	speed[0] = v[0];
	speed[1] = v[1];
	speed[2] = v[2];
	speed[3] = v[3];
}

void Robo::set_motor_speed(uint8_t motnr, float vel) {
	if(motnr<4){
		speed[motnr]=vel;
	}
}
