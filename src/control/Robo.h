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
#include "Switch.h"

#include <utils/time_functions.h>


class Robo {
public:
	Robo(Motor *roboMotor0, Motor *roboMotor1, Motor *roboMotor2, Motor *roboMotor3, adc *sensorAdc, Switch *Switch, bool testmode=1);
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
    NRF24 *radio;
    void get_wheel_speeds(float ptr[]);//armazena as velocidades lineares dos centros das RODAS em ptr
    void set_speed(float v_r, float v_t, float w);
    void set_speed(float v[]);
    void set_motor_speed(uint8_t motnr, float vel);
    bool _testmode;
    bool InTestMode(){return _testmode;};
    void SetTestMode(bool testmode) {_testmode=testmode;}

/*
    IO_Pin_STM32 sw1(IO_Pin_Mode mode, GPIO_TypeDef *GPIOx, uint32_t pins, GPIOPuPd_TypeDef pupd=GPIO_PuPd_NOPULL, GPIOOType_TypeDef GPIO_OType=GPIO_OType_PP);
    IO_Pin_STM32 sw2(IO_Pin_Mode mode, GPIO_TypeDef *GPIOx, uint32_t pins, GPIOPuPd_TypeDef pupd=GPIO_PuPd_NOPULL, GPIOOType_TypeDef GPIO_OType=GPIO_OType_PP);
    IO_Pin_STM32 sw3(IO_Pin_Mode mode, GPIO_TypeDef *GPIOx, uint32_t pins, GPIOPuPd_TypeDef pupd=GPIO_PuPd_NOPULL, GPIOOType_TypeDef GPIO_OType=GPIO_OType_PP);
*/

    uint8_t GetId(){return _id;}
protected:
    uint8_t _id;
};

extern Robo robo;
#endif /* ROBO_H_ */
