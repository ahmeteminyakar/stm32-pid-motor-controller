#include "uart_comm.h"
#include "pid.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static UART_HandleTypeDef *comm_huart;

static uint8_t  rx_byte;
static char     rx_buf[64];
static uint8_t  rx_idx = 0;

extern PID_Controller pid;

void UART_Init(UART_HandleTypeDef *huart)
{
    comm_huart = huart;
    HAL_UART_Receive_IT(huart, &rx_byte, 1);
}

void UART_SendTelemetry(uint32_t tick_ms, float setpoint, float rpm,
                        float pwm_duty, float kp, float ki, float kd)
{
    char buf[128];
    int len = snprintf(buf, sizeof(buf),
                       "%lu,%.1f,%.1f,%.1f,%.2f,%.2f,%.3f\r\n",
                       (unsigned long)tick_ms, setpoint, rpm, pwm_duty,
                       kp, ki, kd);
    HAL_UART_Transmit(comm_huart, (uint8_t *)buf, len, 50);
}

/*
 * Commands (newline-terminated):
 *   S<val>  set target RPM     I<val>  set Ki
 *   P<val>  set Kp             D<val>  set Kd
 *   R       reset integrator
 */
void UART_ProcessCommand(void)
{
    if (rx_idx == 0) return;

    float val;
    switch (rx_buf[0]) {
    case 'S': case 's':
        val = (float)atof(&rx_buf[1]);
        PID_SetSetpoint(&pid, val);
        break;
    case 'P': case 'p':
        val = (float)atof(&rx_buf[1]);
        pid.kp = val;
        break;
    case 'I': case 'i':
        val = (float)atof(&rx_buf[1]);
        pid.ki = val;
        break;
    case 'D': case 'd':
        val = (float)atof(&rx_buf[1]);
        pid.kd = val;
        break;
    case 'R': case 'r':
        PID_Reset(&pid);
        break;
    default:
        break;
    }
    rx_idx = 0;
}

void UART_RxByteCallback(void)
{
    if (rx_byte == '\n' || rx_byte == '\r') {
        rx_buf[rx_idx] = '\0';
        UART_ProcessCommand();
    } else if (rx_idx < sizeof(rx_buf) - 1) {
        rx_buf[rx_idx++] = (char)rx_byte;
    }
    HAL_UART_Receive_IT(comm_huart, &rx_byte, 1);
}
