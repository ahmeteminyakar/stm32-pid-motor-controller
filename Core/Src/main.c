#include "main.h"
#include "pid.h"
#include "encoder.h"
#include "motor.h"
#include "uart_comm.h"

TIM_HandleTypeDef  htim2;
TIM_HandleTypeDef  htim3;
TIM_HandleTypeDef  htim6;
UART_HandleTypeDef huart2;

PID_Controller pid;
volatile uint8_t pid_tick_flag = 0;
static float current_duty = 0.0f;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM6_Init(void);
static void MX_USART2_UART_Init(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == PID_TIM)
        pid_tick_flag = 1;
}

extern void UART_RxByteCallback(void);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == COMM_UART)
        UART_RxByteCallback();
}

int main(void)
{
    HAL_Init();
    SystemClock_Config();

    MX_GPIO_Init();
    MX_TIM2_Init();
    MX_TIM3_Init();
    MX_TIM6_Init();
    MX_USART2_UART_Init();

    Encoder_Init(&htim2);
    Motor_Init(&htim3, MOTOR_TIM_CHANNEL);
    UART_Init(&huart2);

    PID_Init(&pid, DEFAULT_KP, DEFAULT_KI, DEFAULT_KD,
             PID_DT, -100.0f, 100.0f);
    PID_SetSetpoint(&pid, DEFAULT_SETPOINT);

    HAL_TIM_Base_Start_IT(&htim6);

    const char *header = "time_ms,setpoint,rpm,duty,kp,ki,kd\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t *)header, strlen(header), 100);

    while (1) {
        if (pid_tick_flag) {
            pid_tick_flag = 0;

            float rpm = Encoder_GetRPM(PID_DT, ENCODER_CPR);
            current_duty = PID_Compute(&pid, rpm);
            Motor_SetSpeed(current_duty);

            UART_SendTelemetry(HAL_GetTick(), pid.setpoint, rpm,
                               current_duty, pid.kp, pid.ki, pid.kd);
        }
    }
}

/* Peripheral init — CubeMX generates these, shown here as reference */

void SystemClock_Config(void)
{
    /* 180 MHz via PLL — CubeMX generates full implementation */
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin   = DIR_IN1_PIN | DIR_IN2_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_GPIO_WritePin(DIR_IN1_PORT, DIR_IN1_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DIR_IN2_PORT, DIR_IN2_PIN, GPIO_PIN_RESET);
}

static void MX_TIM2_Init(void)
{
    TIM_Encoder_InitTypeDef sConfig = {0};

    __HAL_RCC_TIM2_CLK_ENABLE();

    htim2.Instance               = TIM2;
    htim2.Init.Prescaler         = 0;
    htim2.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim2.Init.Period            = 0xFFFFFFFF;
    htim2.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    sConfig.EncoderMode  = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity  = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter    = 0x0F;
    sConfig.IC2Polarity  = TIM_ICPOLARITY_RISING;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Filter    = 0x0F;

    HAL_TIM_Encoder_Init(&htim2, &sConfig);
}

static void MX_TIM3_Init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};

    __HAL_RCC_TIM3_CLK_ENABLE();

    htim3.Instance               = TIM3;
    htim3.Init.Prescaler         = 0;
    htim3.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim3.Init.Period            = 999;     /* 20 kHz PWM @ 90 MHz APB1 */
    htim3.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    HAL_TIM_PWM_Init(&htim3);

    sConfigOC.OCMode     = TIM_OCMODE_PWM1;
    sConfigOC.Pulse      = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1);
}

static void MX_TIM6_Init(void)
{
    __HAL_RCC_TIM6_CLK_ENABLE();

    htim6.Instance               = TIM6;
    htim6.Init.Prescaler         = 8999;    /* 90 MHz / 9000 = 10 kHz */
    htim6.Init.CounterMode       = TIM_COUNTERMODE_UP;
    htim6.Init.Period            = 99;      /* 10 kHz / 100 = 100 Hz */
    htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    HAL_TIM_Base_Init(&htim6);

    HAL_NVIC_SetPriority(TIM6_DAC_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

static void MX_USART2_UART_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();

    huart2.Instance          = USART2;
    huart2.Init.BaudRate     = 115200;
    huart2.Init.WordLength   = UART_WORDLENGTH_8B;
    huart2.Init.StopBits     = UART_STOPBITS_1;
    huart2.Init.Parity       = UART_PARITY_NONE;
    huart2.Init.Mode         = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    HAL_UART_Init(&huart2);

    HAL_NVIC_SetPriority(USART2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}
