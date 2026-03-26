#!/usr/bin/env python3
"""
Real-time serial plotter for PID motor controller telemetry.

Usage:
  python serial_plotter.py
  python serial_plotter.py --port COM3
  python serial_plotter.py --log
"""

import argparse
import sys
import time
import csv
from collections import deque

import serial
import serial.tools.list_ports
import matplotlib.pyplot as plt
import matplotlib.animation as animation

BAUD_RATE = 115200
MAX_POINTS = 500


def find_port():
    ports = serial.tools.list_ports.comports()
    for p in ports:
        if "STM" in (p.description or "") or "ST-Link" in (p.description or ""):
            return p.device
    if ports:
        return ports[0].device
    return None


def main():
    parser = argparse.ArgumentParser(description="PID Motor Controller Plotter")
    parser.add_argument("--port", type=str, default=None)
    parser.add_argument("--baud", type=int, default=BAUD_RATE)
    parser.add_argument("--log", action="store_true")
    args = parser.parse_args()

    port = args.port or find_port()
    if not port:
        print("No serial port found. Use --port to specify.")
        sys.exit(1)

    print(f"Connecting to {port} at {args.baud} baud...")
    ser = serial.Serial(port, args.baud, timeout=0.1)
    time.sleep(2)

    t_data   = deque(maxlen=MAX_POINTS)
    sp_data  = deque(maxlen=MAX_POINTS)
    rpm_data = deque(maxlen=MAX_POINTS)
    duty_data = deque(maxlen=MAX_POINTS)

    csv_file = None
    csv_writer = None
    if args.log:
        csv_file = open(f"pid_log_{int(time.time())}.csv", "w", newline="")
        csv_writer = csv.writer(csv_file)
        csv_writer.writerow(["time_ms", "setpoint", "rpm", "duty", "kp", "ki", "kd"])

    ser.readline()

    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 6), sharex=True)
    fig.suptitle("PID Motor Speed Controller")

    line_sp,  = ax1.plot([], [], "r--", label="Setpoint")
    line_rpm, = ax1.plot([], [], "b-",  label="RPM")
    ax1.set_ylabel("Speed (RPM)")
    ax1.legend(loc="upper right")
    ax1.grid(True)

    line_duty, = ax2.plot([], [], "g-", label="PWM Duty %")
    ax2.set_ylabel("Duty (%)")
    ax2.set_xlabel("Time (s)")
    ax2.set_ylim(-110, 110)
    ax2.legend(loc="upper right")
    ax2.grid(True)

    def update(frame):
        while ser.in_waiting:
            try:
                raw = ser.readline().decode("ascii", errors="ignore").strip()
                if not raw:
                    continue
                parts = raw.split(",")
                if len(parts) < 7:
                    continue

                t_data.append(float(parts[0]) / 1000.0)
                sp_data.append(float(parts[1]))
                rpm_data.append(float(parts[2]))
                duty_data.append(float(parts[3]))

                if csv_writer:
                    csv_writer.writerow(parts)

            except (ValueError, IndexError):
                continue

        if len(t_data) > 1:
            line_sp.set_data(list(t_data), list(sp_data))
            line_rpm.set_data(list(t_data), list(rpm_data))
            line_duty.set_data(list(t_data), list(duty_data))

            ax1.set_xlim(t_data[0], t_data[-1])
            ax1.set_ylim(
                min(min(rpm_data), min(sp_data)) - 50,
                max(max(rpm_data), max(sp_data)) + 50
            )
            ax2.set_xlim(t_data[0], t_data[-1])

        return line_sp, line_rpm, line_duty

    ani = animation.FuncAnimation(fig, update, interval=50, blit=False)
    plt.tight_layout()

    try:
        plt.show()
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        if csv_file:
            csv_file.close()


if __name__ == "__main__":
    main()
