/*
 * Robo.h
 *
 *  Created on: Jul 9, 2016
 *      Author: lenovoi7
 */
#ifndef ROBO_H_
#define ROBO_H_

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "GPIO.h"
#include "Pwm.h"
#include "Encoder.h"
#include "TimerTime.h"
#include "Motor.h"

class Robo {
public:
	Robo();
	void set_pwms(int dutys[4]);
	Timer_Time *Tempo;
	Pwm *ahpwms[4];
	GPIO *algpios[4];
    Pwm *bhpwms[4];
    GPIO *blgpios[4];
    Encoder *encoders[4];
    Motor *motors[4];
private:
};
#endif /* ROBO_H_ */
