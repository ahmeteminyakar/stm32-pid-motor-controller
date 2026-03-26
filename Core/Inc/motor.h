#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f4xx_hal.h"

void Motor_Init(TIM_HandleTypeDef *htim, uint32_t channel);
void Motor_SetSpeed(float duty);    /* -100.0 to +100.0 */
void Motor_Stop(void);

#endif
