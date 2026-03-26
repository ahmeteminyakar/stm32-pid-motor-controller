# STM32 PID Motor Speed Controller

Interrupt-driven PID speed controller for a DC motor using quadrature encoder feedback, running on STM32F446RE.

## Features

- **PID control loop** running at 100 Hz in timer interrupt
- **Quadrature encoder** reading via hardware timer (4x decoding)
- **PWM output** to H-bridge motor driver with direction control
- **UART telemetry** — real-time CSV stream of speed, setpoint, and PID parameters
- **Runtime tuning** — change setpoint and PID gains via UART commands
- **Python plotter** — live visualization of speed response

## Hardware

| Component         | Detail                              |
|-------------------|-------------------------------------|
| MCU               | STM32F446RE (Nucleo-64)             |
| Motor             | DC motor with quadrature encoder    |
| Driver            | H-bridge (L298N, TB6612, or similar)|
| Communication     | UART via ST-Link VCP (115200 baud)  |

See [docs/wiring.md](docs/wiring.md) for full pin connections.

## Project Structure

```
stm32-pid-motor-controller/
├── Core/
│   ├── Inc/
│   │   ├── main.h          # Pin definitions, constants
│   │   ├── pid.h           # PID controller interface
│   │   ├── encoder.h       # Encoder reading interface
│   │   ├── motor.h         # Motor PWM + direction interface
│   │   └── uart_comm.h     # UART telemetry & commands
│   └── Src/
│       ├── main.c          # Init, main loop, callbacks
│       ├── pid.c           # PID algorithm with anti-windup
│       ├── encoder.c       # Encoder count → RPM conversion
│       ├── motor.c         # PWM duty + H-bridge direction
│       └── uart_comm.c     # CSV output, command parser
├── tools/
│   ├── serial_plotter.py   # Real-time matplotlib plotter
│   └── requirements.txt
├── docs/
│   └── wiring.md           # Pin connections
└── README.md
```

## Build & Flash

### Using STM32CubeIDE
1. Create a new STM32CubeIDE project for **STM32F446RETx**
2. Copy `Core/Inc/` and `Core/Src/` files into the generated project
3. In CubeMX, configure the peripherals matching the pin assignments in `main.h`
4. Build and flash to the Nucleo board

### Peripheral Configuration (CubeMX)
- **TIM2**: Encoder Mode — CH1 (PA0), CH2 (PA1)
- **TIM3**: PWM Generation CH1 (PA6) — Period: 999, Prescaler: 0
- **TIM6**: Basic timer — Prescaler: 8999, Period: 99 → 100 Hz interrupt
- **USART2**: 115200 baud, 8N1 — TX (PA2), RX (PA3)
- **PB0, PB1**: GPIO Output — H-bridge direction

## UART Telemetry

Output format (CSV, 100 Hz):
```
time_ms,setpoint,rpm,duty,kp,ki,kd
1234,500.0,487.3,24.5,2.00,0.50,0.050
```

## UART Commands

Send via serial terminal (newline-terminated):

| Command   | Description              | Example  |
|-----------|--------------------------|----------|
| `S<val>`  | Set target RPM           | `S800`   |
| `P<val>`  | Set Kp                   | `P3.0`   |
| `I<val>`  | Set Ki                   | `I0.2`   |
| `D<val>`  | Set Kd                   | `D0.01`  |
| `R`       | Reset PID integrator     | `R`      |

## Python Plotter

```bash
cd tools
pip install -r requirements.txt
python serial_plotter.py              # auto-detect port
python serial_plotter.py --port COM3  # specify port
python serial_plotter.py --log        # save to CSV
```

Displays live plots of setpoint vs actual RPM and PWM duty cycle.

## Default PID Parameters

| Parameter  | Value  |
|------------|--------|
| Kp         | 2.0    |
| Ki         | 0.5    |
| Kd         | 0.05   |
| Setpoint   | 500 RPM|
| Loop rate  | 100 Hz |

These are starting values — tune for your specific motor and load.
