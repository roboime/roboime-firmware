/*
 * Robo.cpp
 *
 *  Created on: Mar 12, 2016
 *      Author: lenovoi7
 */
#include "Robo.h"
#include "pins.h"

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
	batAdc->ADC_Config(10, GPIOC, GPIO_Pin_0);
}
void Robo::control_pos(){
	for(int i=0; i<4; i++){
		motors[i]->Control_Pos(pos[i]);
	}
}
void Robo::control_speed(){
  vBat = 4.3*batAdc->adc_getConversion();
  if(vBat>7400){
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
	speed[0] = -v_r*0.5 + v_t*0.86603 + w*R;
	speed[1] = -v_r*0.5 - v_t*0.86603 + w*R;
	speed[2] = v_r*0.70711 + v_t*0.70711 + w*R;
	speed[3] = v_r*0.70711 - v_t*0.70711 + w*R;
	return;
}
