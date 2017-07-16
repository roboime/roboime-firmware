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
#include "Switch.h"

#include "proto/grSim_Commands.pb.h"
#include "proto/pb_decode.h"
#include "proto/pb_encode.h"
#include "radio/commands.h"
#include <radio/bsp.h>

#include <utils/time_functions.h>


class Robo {
public:
	Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, dibre *drible, adc *sensorAdc, NRF24L01P *mynrf24, Switch *Switch, bool testmode=1);
	GPIO *high_kick;
	GPIO *chute_baixo;
	void HighKick();
	void ChuteBaixo();
	void control_speed();
	void control_pos();
	dibre *drible;
	int pos[4];
	float speed[4];
	float real_wheel_speed[4];//armazenará as velocidades medidas (m/s) das RODAS
    Motor *motors[4];
    adc *roboAdc;
    float vBat;
    int nVerifyPacket;
    int nPacketReceived;
    NRF24L01P *_nrf24;
    void get_wheel_speeds(float ptr[]);//armazena as velocidades lineares dos centros das RODAS em ptr
    void set_speed(float v_r, float v_t, float w);
    void set_speed(float v[]);
    void set_motor_speed(uint8_t motnr, float vel);
    bool _testmode;
    bool InTestMode(){return _testmode;};
    void SetTestMode(bool testmode) {_testmode=testmode;}
    uint8_t GetId(){return _id;}
    void interrupt_control();
    void interruptReceive();
    void interruptTestMode();
    void processPacket();
   	void interruptTransmitter();
   	void init();
   	void interruptAckPayload();

   	bool stepbit;
   	bool printI;
    bool controlbit;
    bool printv;
   	uint8_t channel;
   	uint64_t address;
   	int16_t dutycycles[4];

    uint32_t last_packet_ms = 0;
    grSim_Robot_Command robotcmd;
    grSim_Robot_Command robotcmd_test;
protected:
    uint8_t _id;
};

extern Robo robo;
#endif /* ROBO_H_ */
