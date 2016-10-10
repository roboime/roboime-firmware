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


Robo::Robo(uint8_t id, bool testmode):
	_testmode(testmode),
	_id(id)
{


	robo_timer = new Timer_Time2(); //valores ja setupados pro timer_time

	for(int i=0; i<4; i++){
	  ahpwms[i]     = new Pwm(MAH_Port[i], MAH_Pin[i], MAH_Tim[i], MAH_Af_Pin[i], MAH_Af[i], MAH_Ch[i], MAH_nState[i]);
	  algpios[i]    = new GPIO(MAL_Port[i], MAL_Pin[i]);
	  bhpwms[i]     = new Pwm(MBH_Port[i], MBH_Pin[i], MBH_Tim[i], MBH_Af_Pin[i], MBH_Af[i], MBH_Ch[i], MBH_nState[i]);
	  blgpios[i]    = new GPIO(MBL_Port[i], MBL_Pin[i]);
	  encoders[i]   = new Encoder(M_EncA_Port[i], M_EncB_Port[i], M_EncA_Pin[i], M_EncB_Pin[i], M_Enc_Tim[i], M_EncA_Af_Pin[i], M_EncB_Af_Pin[i], M_Enc_Af[i]);
	  motors[i]     = new Motor(ahpwms[i], algpios[i], bhpwms[i], blgpios[i], encoders[i], robo_timer);
	}
	batAdc = new adc();
	batAdc->ADC_Config();

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
void Robo::Receive(){
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
}
void Robo::procPacket(uint8_t *dataPacket){
  if(dataPacket[0]=='a'){
	if((dataPacket[10]&0b00000001)) ChuteBaixo();
	if((dataPacket[10]&0b00000010)) HighKick();
	int drible_vel;
	drible_vel = 10*dataPacket[11];
	drible->Set_Vel(drible_vel);
	if(dataPacket[12]==0){
      int16_t v[3];
      memcpy(v, (dataPacket+13), 6);
      set_speed(v[0], v[1], v[2]);
	}
  }
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
		motors[i]->Control_Pos(pos[i]);
	}
}
void Robo::control_speed(){
  vBat = 4.3*batAdc->adc_getConversion();
  if(vBat>6 || 1){
    for(int i=0; i<4; i++){
	  motors[i]->Control_Speed(speed[i]);
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
	/*
	self.wheel_angles = [
	            +58.0,  # wheel 0
	            +133.0, # wheel 1
	            -133.0, # wheel 2
	            -58.0,  # wheel 3
	        ]

	        # Values in meters.
	        self.wheel_distance = 0.07521
	        self.wheel_radius = 0.0285
	        self.max_speed = 64.0

	    def omniwheel_speeds(self, vx, vy, va):
	        if isnan(vx) or isnan(vy) or isnan(va):
	            return [0.0, 0.0, 0.0, 0.0]
	        speeds = [(vy * cos(a) - vx * sin(a) + va * self.wheel_distance) / self.wheel_radius for a in self.wheel_angles]
	        largest = max(abs(x) for x in speeds)
	        if largest > self.max_speed:
	            speeds = [x * self.max_speed / largest for x in speeds]
	        return speeds
*/
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
