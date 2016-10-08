/*
 * Robo.h
 *
 *  Created on: Jul 9, 2016
 *      Author: lenovoi7
 */
#ifndef ROBO_H_
#define ROBO_H_

#include <cstring>
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "GPIO.h"
#include "Pwm.h"
#include "Encoder.h"
#include "TimerTime2.h"
#include "Motor.h"
#include "adc.h"
#include "dibre.h"
#include "NRF24.h"

class Robo {
public:
	Robo();
	GPIO *high_kick;
	GPIO *chute_baixo;
	void HighKick();
	void ChuteBaixo();
	void control_speed();
	void control_pos();
	dibre *drible;
	int pos[4];
	int speed[4];
	Timer_Time2 *robo_timer;
	Pwm *ahpwms[4];
	GPIO *algpios[4];
    Pwm *bhpwms[4];
    GPIO *blgpios[4];
    Encoder *encoders[4];
    Motor *motors[4];
    adc *batAdc;
    float vBat;
    int nVerifyPacket;
    int nPacketReceived;
    void Receive();
    void procPacket(uint8_t *dataPacket);
    NRF24 *radio;
    void set_speed(float v_r, float v_t, float w);
    void set_speed(float v[]);
    void set_motor_speed(uint8_t motnr, float vel);

private:
};

extern Robo robo;
#endif /* ROBO_H_ */
