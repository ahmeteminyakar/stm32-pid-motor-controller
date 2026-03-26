#ifndef MAIN_H
#define MAIN_H

#include "stm32f4xx_hal.h"

/* Pin assignments — Nucleo-F446RE */

#define ENC_TIM             TIM2

#define MOTOR_TIM           TIM3
#define MOTOR_TIM_CHANNEL   TIM_CHANNEL_1

#define DIR_IN1_PORT        GPIOB
#define DIR_IN1_PIN         GPIO_PIN_0
#define DIR_IN2_PORT        GPIOB
#define DIR_IN2_PIN         GPIO_PIN_1

#define COMM_UART           USART2

#define PID_TIM             TIM6
#define PID_LOOP_HZ         100
#define PID_DT              (1.0f / PID_LOOP_HZ)

#define ENCODER_CPR         2048    /* 4x quadrature */

#define DEFAULT_KP          2.0f
#define DEFAULT_KI          0.5f
#define DEFAULT_KD          0.05f
#define DEFAULT_SETPOINT    500.0f

#endif
