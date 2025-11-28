# ESP32 RC Car Circuit Diagram

## Components Required

1. **ESP32 Development Board** (ESP32-WROOM-32 or similar)
2. **L298N Motor Driver Module** (or TB6612FNG)
3. **2x DC Motors** (6V-12V, suitable for your car chassis)
4. **Battery Pack** (7.4V-12V for motors, separate from ESP32 power)
5. **Power Supply for ESP32** (5V via USB or voltage regulator)
6. **Jumper Wires** (Male-to-Male, Male-to-Female)
7. **Breadboard** (optional, for prototyping)
8. **RC Car Chassis** (with wheels and mounting)

## Circuit Connections

### ESP32 to L298N Motor Driver

```
ESP32 GPIO    →    L298N Module
─────────────────────────────────
GPIO 16       →    IN1 (Left Motor Direction 1)
GPIO 17       →    IN2 (Left Motor Direction 2)
GPIO 18       →    ENA (Left Motor PWM Enable)
GPIO 19       →    IN3 (Right Motor Direction 1)
GPIO 21       →    IN4 (Right Motor Direction 2)
GPIO 22       →    ENB (Right Motor PWM Enable)
GND           →    GND (Common Ground)
```

### L298N Motor Driver to Motors

```
L298N Module          →    Motors
─────────────────────────────────────
OUT1                  →    Left Motor Terminal 1
OUT2                  →    Left Motor Terminal 2
OUT3                  →    Right Motor Terminal 1
OUT4                  →    Right Motor Terminal 2
```

### Power Connections

```
Power Source          →    Component
─────────────────────────────────────
Battery Pack (+)      →    L298N VCC (7.4V-12V)
Battery Pack (-)      →    L298N GND
Battery Pack (-)      →    ESP32 GND (Common Ground)
USB 5V or Regulator   →    ESP32 5V/VIN
```

## Visual Circuit Diagram (ASCII)

```
                    ┌─────────────────┐
                    │     ESP32       │
                    │                 │
                    │  GPIO 16 ───────┼───┐
                    │  GPIO 17 ───────┼───┤
                    │  GPIO 18 ───────┼───┤
                    │  GPIO 19 ───────┼───┤
                    │  GPIO 21 ───────┼───┤
                    │  GPIO 22 ───────┼───┤
                    │  GND      ──────┼───┘
                    └─────────────────┘
                           │
                           │
        ┌──────────────────┴──────────────────┐
        │                                     │
        ▼                                     ▼
┌──────────────┐                    ┌──────────────┐
│   L298N      │                    │   Battery    │
│  Motor       │                    │   Pack      │
│  Driver      │                    │  (7.4-12V)  │
│              │                    │             │
│ IN1  ←───────┼────────────────────┘             │
│ IN2  ←───────┼──────────────────────────────────┘
│ ENA  ←───────┼───┐                              │
│ IN3  ←───────┼───┤                              │
│ IN4  ←───────┼───┤                              │
│ ENB  ←───────┼───┘                              │
│ GND  ←───────┼──────────────────────────────────┘
│ VCC  ←───────┼──────────────────────────────────┐
│              │                                   │
│ OUT1 ────────┼───┐                              │
│ OUT2 ────────┼───┤                              │
│ OUT3 ────────┼───┤                              │
│ OUT4 ────────┼───┘                              │
└──────────────┘                                  │
        │                                         │
        │                                         │
        ▼                                         │
┌──────────────┐                    ┌──────────────┐
│  Left Motor  │                    │ Right Motor  │
│              │                    │              │
│  Terminal1   │                    │  Terminal1   │
│  Terminal2   │                    │  Terminal2    │
└──────────────┘                    └──────────────┘
```

## Detailed Pin Connections

### L298N Motor Driver Module

The L298N module typically has these pins:

**Control Pins:**
- `IN1`, `IN2`: Control left motor direction
- `IN3`, `IN4`: Control right motor direction
- `ENA`: Enable/PWM for left motor
- `ENB`: Enable/PWM for right motor

**Power Pins:**
- `VCC`: Motor power supply (7.4V-12V)
- `GND`: Ground
- `+5V`: Logic power (usually not needed if VCC is connected)

**Output Pins:**
- `OUT1`, `OUT2`: Left motor connections
- `OUT3`, `OUT4`: Right motor connections

## Important Notes

1. **Common Ground**: Always connect ESP32 GND to L298N GND for proper signal reference.

2. **Power Separation**: 
   - ESP32 can be powered via USB (5V) or through VIN pin
   - Motors should be powered by a separate battery pack (7.4V-12V)
   - Do NOT power ESP32 from the motor battery directly without a voltage regulator

3. **Motor Direction**: If motors spin in wrong direction, swap the motor wires (OUT1↔OUT2 or OUT3↔OUT4).

4. **Enable Jumpers**: Some L298N modules have jumpers on ENA/ENB. Remove them to use PWM control.

5. **Current Rating**: Ensure your battery and L298N can handle the current draw of your motors.

## Alternative: TB6612FNG Motor Driver

If using TB6612FNG instead of L298N:

```
ESP32 GPIO    →    TB6612FNG
─────────────────────────────────
GPIO 16       →    AIN1
GPIO 17       →    AIN2
GPIO 18       →    PWMA
GPIO 19       →    BIN1
GPIO 21       →    BIN2
GPIO 22       →    PWMB
GND           →    GND
```

TB6612FNG is more efficient and smaller than L298N, but L298N is more commonly available.

