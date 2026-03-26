#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    float kp;
    float ki;
    float kd;
    float setpoint;
    float integral;
    float prev_error;
    float output_min;
    float output_max;
    float dt;
} PID_Controller;

void  PID_Init(PID_Controller *pid, float kp, float ki, float kd,
               float dt, float out_min, float out_max);
float PID_Compute(PID_Controller *pid, float measurement);
void  PID_Reset(PID_Controller *pid);
void  PID_SetTunings(PID_Controller *pid, float kp, float ki, float kd);
void  PID_SetSetpoint(PID_Controller *pid, float setpoint);

#endif
