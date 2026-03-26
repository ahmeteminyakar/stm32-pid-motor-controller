#include "encoder.h"

static TIM_HandleTypeDef *enc_htim;
static int32_t last_count;

void Encoder_Init(TIM_HandleTypeDef *htim)
{
    enc_htim   = htim;
    last_count = 0;
    __HAL_TIM_SET_COUNTER(htim, 0);
    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
}

int32_t Encoder_GetCount(void)
{
    return (int32_t)__HAL_TIM_GET_COUNTER(enc_htim);
}

void Encoder_ResetCount(void)
{
    __HAL_TIM_SET_COUNTER(enc_htim, 0);
    last_count = 0;
}

float Encoder_GetRPM(float dt, uint16_t cpr)
{
    int32_t current = (int32_t)__HAL_TIM_GET_COUNTER(enc_htim);
    int32_t delta   = current - last_count;
    last_count      = current;

    float rps = (float)delta / ((float)cpr * dt);
    return rps * 60.0f;
}
