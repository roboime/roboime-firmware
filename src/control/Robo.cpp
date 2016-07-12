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
	Tempo = new Timer_Time(); //valores ja setupados pro timer_time

	for(int i=0; i<4; i++){
	  ahpwms[i]     = new Pwm(MAH_Port[i], MAH_Pin[i], MAH_Tim[i], MAH_Af_Pin[i], MAH_Af[i], MAH_Ch[i], MAH_nState[i]);
	  algpios[i]    = new GPIO(MAL_Port[i], MAL_Pin[i]);
	  bhpwms[i]     = new Pwm(MBH_Port[i], MBH_Pin[i], MBH_Tim[i], MBH_Af_Pin[i], MBH_Af[i], MBH_Ch[i], MBH_nState[i]);
	  blgpios[i]    = new GPIO(MBL_Port[i], MBL_Pin[i]);
	  encoders[i]   = new Encoder(M_EncA_Port[i], M_EncB_Port[i], M_EncA_Pin[i], M_EncB_Pin[i], M_Enc_Tim[i], M_EncA_Af_Pin[i], M_EncB_Af_Pin[i], M_Enc_Af[i]);
	  motors[i]     = new Motor(ahpwms[i], algpios[i], bhpwms[i], blgpios[i], encoders[i], Tempo);
	}
}
void Robo::set_pwms(int dutys[4]){
	for(int i=0; i<4; i++){
		motors[i]->Answer(dutys[i]);
	}
}
