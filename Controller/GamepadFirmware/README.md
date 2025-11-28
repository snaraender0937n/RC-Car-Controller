# Gamepad Controller Firmware

This is a **clone** of the Controller-PC-Mobile gamepad firmware, copied directly into RC-Car-Controller for convenience.

## 📁 Structure

```
Controller/GamepadFirmware/
├── gamepad_controller.ino    # Main controller firmware
├── library/                  # BLE Gamepad library files
│   ├── BleGamepad.h/.cpp
│   ├── BleGamepadConfiguration.h/.cpp
│   ├── BleConnectionStatus.h/.cpp
│   ├── BleOutputReceiver.h/.cpp
│   └── BleNUS.h/.cpp
└── README.md
```

## 🎮 Purpose

This firmware runs on a **SEPARATE ESP32 board** that acts as the gamepad/controller you hold in your hands. It sends joystick and button data via BLE to the RC Car receiver.

**Important:** 
- This code goes on the **Controller ESP32** (the gamepad)
- `rc_car_esp32.ino` goes on the **RC Car ESP32** (the car)

## 🔌 Hardware Setup

### Required Components:
- ESP32 Development Board
- 2x Analog Joysticks
- 13x Buttons (X, Circle, Triangle, Square, R1, R2, L1, L2, Start, Select, PS, R3, L3)

### Pin Connections:

**Buttons:**
- X Button → GPIO 15
- Circle Button → GPIO 17
- Triangle Button → GPIO 4
- Square Button → GPIO 5
- R1 → GPIO 18
- R2 → GPIO 19
- L1 → GPIO 21
- L2 → GPIO 22
- Start → GPIO 23
- Select → GPIO 26
- PS → GPIO 25
- R3 → GPIO 33
- L3 → GPIO 32

**Joysticks:**
- Left VRX → GPIO 14 (Analog)
- Left VRY → GPIO 27 (Analog)
- Right VRX → GPIO 12 (Analog)
- Right VRY → GPIO 13 (Analog)

## 💻 Upload Instructions

1. Open `gamepad_controller.ino` in Arduino IDE
2. Select board: **ESP32 Dev Module**
3. Install required library: **NimBLE-Arduino** (via Library Manager)
4. Upload to your Controller ESP32 board
5. Open Serial Monitor (115200 baud) to see connection status

## ⚙️ Configuration

### Controller Name
The controller advertises as **"BLE Gamepad"** (line 52 in code). This name must match what the RC Car is looking for.

### Mode Selection
Set the gamepad mode (line 49):
- `ANDROID` - For mobile/Android devices (default, works with RC Car)
- `PC` - For PC/Windows devices

## 🔗 Connection to RC Car

1. Upload this firmware to Controller ESP32
2. Upload `rc_car_esp32.ino` to RC Car ESP32
3. Power on both devices
4. RC Car will automatically scan and connect to "BLE Gamepad"
5. Control the car using the joysticks!

## 📝 Notes

- This is a **clone** - the original is in `Controller-PC-Mobile-main`
- All library files are included locally in the `library/` folder
- Controller name is hardcoded to "BLE Gamepad" to match RC Car linking code
- Default mode is ANDROID (compatible with RC Car receiver)

## 🔧 Troubleshooting

**Controller not found by RC Car:**
- Check Serial Monitor - should show "BLE Gamepad Controller Started"
- Verify controller name is exactly "BLE Gamepad"
- Ensure controller is powered on before RC Car

**Buttons/Joysticks not working:**
- Check wiring connections
- Verify GPIO pins match your hardware
- Test buttons with multimeter/voltmeter

**Connection issues:**
- Keep devices within 10-30 meters
- Ensure no other device is connected to controller
- Power cycle both devices if connection fails

