# Wiring Diagram

## Target Board
- **STM32F446RE Nucleo-64**

## Encoder (Quadrature)
| Encoder Pin | STM32 Pin | Function       |
|-------------|-----------|----------------|
| A (CH_A)    | PA0       | TIM2_CH1       |
| B (CH_B)    | PA1       | TIM2_CH2       |
| VCC         | 5V        | Power          |
| GND         | GND       | Ground         |

## H-Bridge Motor Driver (e.g., L298N, TB6612)
| Driver Pin | STM32 Pin | Function           |
|------------|-----------|---------------------|
| IN1        | PB0       | Direction control   |
| IN2        | PB1       | Direction control   |
| PWM / ENA  | PA6       | TIM3_CH1 PWM       |
| VCC        | External  | Motor power supply  |
| GND        | GND       | Common ground       |

## UART (via ST-Link VCP)
| Function | STM32 Pin | Notes              |
|----------|-----------|---------------------|
| TX       | PA2       | USART2 TX → PC RX  |
| RX       | PA3       | USART2 RX ← PC TX  |

> USART2 is connected to the ST-Link virtual COM port on Nucleo boards.
> No external USB-UART adapter needed.

## Power Notes
- Motor supply should be separate from the Nucleo board's 5V
- Connect GND between motor driver and Nucleo
- Add decoupling caps near the motor driver

## Pin Summary
```
PA0  → Encoder A (TIM2_CH1)
PA1  → Encoder B (TIM2_CH2)
PA2  → UART TX (USART2)
PA3  → UART RX (USART2)
PA6  → Motor PWM (TIM3_CH1)
PB0  → H-Bridge IN1
PB1  → H-Bridge IN2
```
