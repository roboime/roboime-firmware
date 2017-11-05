/*
 * Robo.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */

#include "Robo.h"

#define sin_phi 0.50
#define cos_phi 0.866
#define sin_theta 0.707
#define cos_theta 0.707

/*#define alfa 1
#define Massa 3
#define Raio 0.09*/

Robo::Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, Switch *_Switch)
{
	motors[0]=roboMotor0;
	motors[1]=roboMotor1;
	motors[2]=roboMotor2;
	motors[3]=roboMotor3;

	_id = _Switch->id;

	high_kick = new GPIO(GPIOD, GPIO_Pin_8);
	chute_baixo = new GPIO(GPIOD, GPIO_Pin_10);
}

void Robo::HighKick(float power){
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)){
		high_kick->Set();
		for(int i=0;i<0xeee2;i++);
		high_kick->Reset();
	}
}

void Robo::ChuteBaixo(float power){
	if((GetLocalTime()-last_kick_time)>700){
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)){
			chute_baixo->Set();
			delay_ticks((uint32_t) (power*611)); //611 = Gustavo's magic number
			chute_baixo->Reset();
			last_kick_time = GetLocalTime();
		}
	}
}

void Robo::control_pos(){
	for(int i=0; i<4; i++){
		motors[i]->Control_Pos(pos[i]);
	}
}
void Robo::control_speed(){
  //vBat = 4.3*roboAdc->adc_getConversion();
//testa e corrige eventual deslizamento
	  float v0= motors[0]->real_wheel_speed;
	  float v1= motors[1]->real_wheel_speed;
	  float v2= motors[2]->real_wheel_speed;
	  float v3= motors[3]->real_wheel_speed;
  //float M1 = 0.2*(v0-v3)+0.24495*(v1-v2);
  //float M2 = -0.24495*(v0-v3)+0.3*(v1-v2);
  //float proj=-0.5477*v0+0.4472*v1+0.5477*v2-0.4472*v3;
  //vBat=7;
  //if(vBat>6.2){
    //if(proj<1 && proj>-1){
    	for(int i=0; i<4; i++){
    		motors[i]->Control_Speed(speed[i]); //manda a velocidade speed[i] pro motor[i] na unidade m/s
    	}
    //}
    /*else {
    	//nem o valor de alfa nem a massa interferem no espaço nulo.
    	speed[0]=v0*0.8+0.2*v2+0.245*(v1-v3);
    	speed[2]=v2*0.8+0.2*v0-0.245*(v1-v3);
    	speed[3]=v3*0.7+0.245*(v2-v0)+0.3*v1;
    	speed[1]=v1*0.7-0.245*(v2-v0)+0.3*v3;*/
    	/*speed[0]=v0-(-0.5477*v0+0.4472*v1+0.5477*v2-0.4472*v3)*(-0.5477);
    	speed[1]=v1-(-0.5477*v0+0.4472*v1+0.5477*v2-0.4472*v3)*(0.4472);
    	speed[2]=v2-(-0.5477*v0+0.4472*v1+0.5477*v2-0.4472*v3)*(0.5477);
    	speed[3]=v3-(-0.5477*v0+0.4472*v1+0.5477*v2-0.4472*v3)*(-0.4472);
    	for(int i=0; i<4; i++){
     		motors[i]->Control_Speed(speed[i]); //manda a velocidade speed[i] pro motor[i] na unidade m/s
    	}
    }
  //}
  /*else{//medida de proteção: se a bateria estiver fraca, o robô para
    for(int i=0; i<4; i++){
	  motors[i]->SetDutyCycle(0);
    }
  }
  */
}


//recebe as velocidades radial, tangente em m/s e w em rad/s
//grava em speed[] os valores em m/s da velocidade DAS RODAS
void Robo::set_speed(float v_r, float v_t, float w){
	float R = 0.075; //Raio do robo = 9cm

	speed[0] = v_t*cos_phi - v_r*sin_phi + w*R;
	speed[2] = -v_t*cos_phi - v_r*sin_phi + w*R;
	speed[3] = -v_t*cos_theta + v_r*sin_theta + w*R;
	speed[1] = v_t*cos_theta + v_r*sin_theta + w*R;
	//speed[] = 0.176; //teste: para cada roda girar com período 1s
	//speed[] está em m/s. Cuidado para manter a mesma unidade qnd passar pros motores

}

void Robo::set_speed(float v[]){
	speed[0] = v[0];
	speed[1] = v[1];
	speed[2] = v[2];
	speed[3] = v[3];
}

//  5º dia: ainda estou na classe robo

void Robo::set_motor_speed(uint8_t motnr, float vel) {
	if(motnr<4){
		speed[motnr]=vel;
	}
}
