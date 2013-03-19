#ifndef PID_H
#define PID_H

#include <inttypes.h>

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

void pidInit(CONTROLADOR_S *Controlador, float Kp, float Ki, float Kd, float saidaMax, float integMax);
void pidService(CONTROLADOR_S *Controller);


#endif
