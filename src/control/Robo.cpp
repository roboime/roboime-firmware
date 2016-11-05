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

Robo::Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, adc *sensorAdc, uint8_t id, bool testmode):
	_testmode(testmode),
	_id(id)
{
	motors[0]=roboMotor0;
	motors[1]=roboMotor1;
	motors[2]=roboMotor2;
	motors[3]=roboMotor3;

	roboAdc = sensorAdc;
	roboAdc->ADC_Config();

	drible = new dibre();

	high_kick = new GPIO(GPIOD, GPIO_Pin_8);
	chute_baixo = new GPIO(GPIOD, GPIO_Pin_10);

//    radio = new NRF24();
//    radio->is_rx=true;
//    radio->Config();
//    radio->SetId(0);
//    radio->NRF_CE->Set();
}
/*
Pacote do tx

00 uint8_t[1] tipo de pacote e versão do protocolo
01-07 int16_t[3] posição medida pela câmera
08 uint8_t pos_delay
09 uint8_t kick_duration
10 uint8_t kick_flags
11 uint8_t dribler_speed
12 uint8_t skill_id
13-26 skill_data

Pacote do rx

00 uint8_t tipo de pacote e versão do protocolo
01-06 int16_t[3] posição atual calibrada com os sensores do robo
07-12 int16_t[3] velocidade medida pelo robô
13 uint8_t battery level
14 uint8_t kick capacitors level
15 uint8_t flags como os sensor de posse da bola
16 uint8_t hardware id
 */
//void Robo::Receive(){
//  if(radio->DataReady()||(radio->RxEmpty()==0)){
//    radio->NRF_CE->Reset();
//    radio->CleanDataReady();
//    uint8_t data_in[27];
//    radio->ReadPayload(data_in, 27);
//    procPacket(data_in);
//    radio->FlushRx();
//    nPacketReceived++;
//    if(nPacketReceived>10){
//      radio->FlushTx();
//      radio->CleanDataSent();
//  	  uint8_t data_out[17];
//  	  uint8_t batLevel = 10*vBat;
//  	  data_out[13] = batLevel;
//  	  radio->WriteAckPayload(data_out, 17);
//  	  nPacketReceived = 0;
//    }
//	if(radio->MaxRt()){
//	  radio->CleanMaxRt();
//	  radio->FlushTx();
//	}
//    radio->NRF_CE->Set();
//    nVerifyPacket=0;
//  }
//  else{
//    nVerifyPacket++;
//  }
//  if(nVerifyPacket>0x3ee2){
//    set_speed(0, 0, 0);
//    nVerifyPacket=0;
//  }
//}
//void Robo::procPacket(uint8_t *dataPacket){
//  if(dataPacket[0]=='a'){
//	if((dataPacket[10]&0b00000001)) ChuteBaixo();
//	if((dataPacket[10]&0b00000010)) HighKick();
//	int drible_vel;
//	drible_vel = 10*dataPacket[11];
//	drible->Set_Vel(drible_vel);
//	if(dataPacket[12]==0){
//      int16_t v[3];
//      memcpy(v, (dataPacket+13), 6);
//      set_speed(v[0], v[1], v[2]);
//	}
//  }
//}
void Robo::HighKick(){
	float sensorValue2 = roboAdc->readSensor(3);
	if(sensorValue2>0.100){
		high_kick->Set();
		for(int i=0;i<0xeee2;i++);
		high_kick->Reset();
	}
}

void Robo::ChuteBaixo(){
	float sensorValue2 = roboAdc->readSensor(3);
	if(sensorValue2>0.100){
		chute_baixo->Set();
		for(int i=0;i<0xeee2;i++);
		chute_baixo->Reset();
	}
}

void Robo::control_pos(){
	for(int i=0; i<4; i++){
		motors[i]->Control_Pos(pos[i]);
	}
}
void Robo::control_speed(){
  vBat = 4.3*roboAdc->adc_getConversion();
  if(vBat>6){
    for(int i=0; i<4; i++){
	  motors[i]->Control_Speed(speed[i]); //manda a velocidade speed[i] pro motor[i] na unidade m/s
    }
  }
  else{//medida de proteção: se a bateria estiver fraca, o robô para
    for(int i=0; i<4; i++){
	  motors[i]->SetDutyCycle(0);
    }
  }
}

//armazena as velocidades lineares dos centros das RODAS em ptr
void Robo::get_wheel_speeds(float ptr[]){
	for(int i=0; i<4; i++){
		ptr[i]=motors[i]->real_wheel_speed;
	}
	return;
}

//recebe as velocidades radial, tangente em m/s e w em rad/s
//grava em speed[] os valores em m/s da velocidade DAS RODAS
void Robo::set_speed(float v_r, float v_t, float w){
	float R = 0.09; //Raio do robo = 9cm

	speed[0] = -v_r*cos_phi + v_t*sin_phi + w*R;
	speed[2] = -v_r*cos_phi - v_t*sin_phi + w*R;
	speed[3] = -v_r*cos_theta - v_t*sin_theta + w*R;
	speed[1] = -v_r*cos_theta + v_t*sin_theta + w*R;
/*		speed[0] = 0.176; //teste: para cada roda girar com período 1s
		speed[2] = 0.176;
		speed[3] = 0.176;
		speed[1] = 0.176;*/
	//speed[] está em m/s. Cuidado para manter a mesma unidade qnd passar pros motores

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
