#ifndef UART_COMM_H
#define UART_COMM_H

#include "stm32f4xx_hal.h"

void UART_Init(UART_HandleTypeDef *huart);
void UART_SendTelemetry(uint32_t tick_ms, float setpoint, float rpm,
                        float pwm_duty, float kp, float ki, float kd);
void UART_ProcessCommand(void);

#endif
