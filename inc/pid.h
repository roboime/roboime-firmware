#ifndef PID_H
#define PID_H

#include <inttypes.h>
#include "arm_math.h"

typedef struct
{
    // controls coefficients
    float Kp;             // proportional gain
    float Ki;             // integral gain
    float Kd;             // derivative gain
    // state variables
    volatile float erro;          // error (input-feedback)
    volatile float erro_anterior;     // previous error
    volatile float erro_integ;    // integral of error
    volatile float erro_dif;     // derivative of error
    // limits
    float integMax;      // integral wind-up limit
    float saidaMax;      // motor output clamp (limit)
    // input and output
    volatile float saida;         // motor output
    volatile float entrada;          // user input
    volatile float realimentacao;       // motor feedback
} CONTROLADOR_S;

typedef struct
{
	float Vx;
	float Vx_Ref_Campo;
	float Vx_Ref_Robo;
	float Vy;
	float Vy_Ref_Campo;
	float Vy_Ref_Robo;
	float Vteta;
	float Xreal;
	float Yreal;
	float TETAreal;
	float32_t TETAreal_rad;
	float32_t cos_teta_real;
	float32_t sin_teta_real;
	float Xesperado;
	float Yesperado;
	float TETAesperado;

} POSICAO_S ;


void pidInit(CONTROLADOR_S *Controlador, float Kp, float Ki, float Kd, float saidaMax, float integMax);
void pidService(CONTROLADOR_S *Controller);
void posInit(POSICAO_S *P);
void pidServiceAngulo(CONTROLADOR_S *Controlador);
void NewPosition(POSICAO_S *P, float x, float y, float teta);



#endif
