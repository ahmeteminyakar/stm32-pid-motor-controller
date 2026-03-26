#include "motor.h"
#include "main.h"

static TIM_HandleTypeDef *motor_htim;
static uint32_t           motor_channel;

void Motor_Init(TIM_HandleTypeDef *htim, uint32_t channel)
{
    motor_htim    = htim;
    motor_channel = channel;
    HAL_TIM_PWM_Start(htim, channel);
    Motor_Stop();
}

void Motor_SetSpeed(float duty)
{
    if (duty >= 0.0f) {
        HAL_GPIO_WritePin(DIR_IN1_PORT, DIR_IN1_PIN, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DIR_IN2_PORT, DIR_IN2_PIN, GPIO_PIN_RESET);
    } else {
        HAL_GPIO_WritePin(DIR_IN1_PORT, DIR_IN1_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DIR_IN2_PORT, DIR_IN2_PIN, GPIO_PIN_SET);
        duty = -duty;
    }

    if (duty > 100.0f) duty = 100.0f;

    uint32_t arr     = __HAL_TIM_GET_AUTORELOAD(motor_htim);
    uint32_t compare = (uint32_t)((duty / 100.0f) * (float)arr);
    __HAL_TIM_SET_COMPARE(motor_htim, motor_channel, compare);
}

void Motor_Stop(void)
{
    HAL_GPIO_WritePin(DIR_IN1_PORT, DIR_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIR_IN2_PORT, DIR_IN2_PIN, GPIO_PIN_RESET);
    __HAL_TIM_SET_COMPARE(motor_htim, motor_channel, 0);
}
