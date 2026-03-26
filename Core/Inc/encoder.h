#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f4xx_hal.h"

void    Encoder_Init(TIM_HandleTypeDef *htim);
int32_t Encoder_GetCount(void);
void    Encoder_ResetCount(void);
float   Encoder_GetRPM(float dt, uint16_t cpr);

#endif
