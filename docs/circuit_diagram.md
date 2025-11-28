# Circuit Diagram – RC Car

## Main Components
- ESP32 (RC Car)
- L298N Motor Driver
- DC Motors (Left & Right)
- Battery Supply

## Connections Overview
ESP32 controls the L298N driver using GPIO pins:
- IN1, IN2 → Left motor direction
- IN3, IN4 → Right motor direction
- ENA, ENB → PWM speed control

All grounds (ESP32, L298N, battery) **must be common**.

## Power Notes
- Motors should be powered separately from ESP32
- Do NOT power motors from ESP32 5V pin
- Remove ENA/ENB jumpers if using PWM

## Reference Diagram
![RC Car Circuit](images/rc_car_circuit.png)

## Alternatives
- TB6612FNG motor driver
- BTS7960 for high-current motors
