# Setup Guide – ESP32 RC Car with BLE Gamepad

## Hardware Required
- 2 × ESP32 Dev Boards
- L298N Dual H-Bridge Motor Driver
- 2 × DC Motors
- Battery pack for motors
- Jumper wires
- RC car chassis (optional)

## Software Requirements
- Arduino IDE
- ESP32 Board Package
- NimBLE-Arduino library

## Step 1: Upload Controller Firmware
1. Open:
   src/controller/gamepad_controller.ino
2. Select ESP32 board and correct COM port
3. Upload code to **Controller ESP32**
4. Controller will advertise as **"BLE Gamepad"**

## Step 2: Upload RC Car Firmware
1. Open:
   src/rc_car/rc_car_esp32.ino
2. Confirm motor pin assignments
3. Upload code to **RC Car ESP32**

## Step 3: Power Devices
- Power both ESP32 boards
- Controller will automatically connect
- RC car is ready to move

## Verification
- Open Serial Monitor (115200 baud)
- Look for BLE connection messages
