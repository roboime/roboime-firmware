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

Robo::Robo()
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
  if(vBat>7.4){
    for(int i=0; i<4; i++){
	  motors[i]->Control_Speed(speed[i]);
    }
  }
  else{
    for(int i=0; i<4; i++){
	  motors[i]->Answer(0);
    }
  }
}
void Robo::set_speed(int v_r, int v_t, int w){
	uint8_t R = 1; //TODO valor temporario

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
