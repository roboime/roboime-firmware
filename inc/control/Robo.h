/*
 * Robo.h
 *
 *  Created on: Jul 9, 2016
 *      Author: lenovoi7
 */
#ifndef ROBO_H_
#define ROBO_H_

#include <cstring>
#include <utils/time_functions.h>
#include "control/GPIO.h"
#include "control/Motor.h"
#include "control/switch.h"
#include "control/adc.h"

class Robo {
public:
	Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, Switch *_Switch, adc *sensorAdc);
	GPIO *high_kick;
	GPIO *chute_baixo;
    adc *roboAdc;
	void HighKick(float power);
	void ChuteBaixo(float power);
	void control_speed();
	void control_pos();
	int pos[4];
	float speed[4];
    Motor *motors[4];
    float vBat;
    void set_speed(float v_r, float v_t, float w);
    void set_speed(float v[]);
    void set_motor_speed(uint8_t motnr, float vel);
    uint8_t GetId(){return _id;}

   	uint32_t last_kick_time=0;
protected:
    uint8_t _id;
};

extern Robo robo;
#endif /* ROBO_H_ */
