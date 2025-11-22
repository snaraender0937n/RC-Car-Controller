# ESP32 RC Car with BLE Gamepad Controller

This project creates an RC car controlled by an ESP32 that connects to your BLE Gamepad controller via Bluetooth Low Energy.

## 🚗 Features

- **Wireless Control**: Control your RC car via Bluetooth Low Energy (BLE)
- **Differential Drive**: Smooth steering and movement using two motors
- **Joystick Control**: Use the left joystick on your gamepad to drive
- **Auto-Reconnect**: Automatically reconnects if connection is lost
- **Safety Features**: Motors stop automatically when disconnected

## 📋 Components Required

### Electronics
- ESP32 Development Board (ESP32-WROOM-32 or similar)
- L298N Motor Driver Module (or TB6612FNG)
- 2x DC Motors (6V-12V, suitable for your car chassis)
- Battery Pack (7.4V-12V for motors)
- Power Supply for ESP32 (USB cable or 5V regulator)
- Jumper Wires (Male-to-Male, Male-to-Female)
- Breadboard (optional, for prototyping)

### Mechanical
- RC Car Chassis (with wheels and mounting)
- Motor mounting hardware
- Battery holder

## 🔌 Circuit Connections

See [circuit_diagram.md](circuit_diagram.md) for detailed wiring instructions.

### Quick Connection Guide

**ESP32 to L298N:**
- GPIO 16 → IN1
- GPIO 17 → IN2
- GPIO 18 → ENA
- GPIO 19 → IN3
- GPIO 21 → IN4
- GPIO 22 → ENB
- GND → GND

**L298N to Motors:**
- OUT1, OUT2 → Left Motor
- OUT3, OUT4 → Right Motor

**Power:**
- Battery Pack (7.4V-12V) → L298N VCC
- USB or 5V → ESP32 (for power)

## 💻 Software Setup

### Prerequisites

1. **Arduino IDE** with ESP32 board support
   - Install ESP32 board package: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
   - Select board: `Tools > Board > ESP32 Arduino > ESP32 Dev Module`

2. **Required Libraries**
   - **NimBLE-Arduino** (install via Arduino Library Manager)
     - Go to: `Sketch > Include Library > Manage Libraries`
     - Search for "NimBLE-Arduino" by h2zero
     - Install the library
   - ESP32 board package (includes basic ESP32 support)

### Installation Steps

1. **Open the Code**
   - Open `rc_car_esp32.ino` in Arduino IDE

2. **Install NimBLE Library**
   - Open Arduino IDE
   - Go to: `Sketch > Include Library > Manage Libraries`
   - Search for "NimBLE-Arduino" by h2zero
   - Click Install
   - This library is required for BLE communication with your controller

3. **Configure Controller Name**
   - Edit line 56 in the code:
   ```cpp
   #define BLE_GAMEPAD_NAME  "BLE Gamepad"
   ```
   - Make sure this matches exactly with your controller's BLE name

4. **Adjust Motor Settings (Optional)**
   - `MAX_SPEED` (line 48): Maximum motor speed (0-255)
   - `MIN_SPEED` (line 49): Minimum speed to overcome friction
   - `DEAD_ZONE` (line 50): Joystick dead zone threshold

5. **Upload to ESP32**
   - Connect ESP32 via USB
   - Select correct COM port: `Tools > Port`
   - Click Upload button

6. **Open Serial Monitor**
   - Set baud rate to 115200
   - You should see connection status messages
   - Look for "(NimBLE Version)" in the startup message

## 🎮 How to Use

### Initial Setup

1. **Power On Controller**
   - Turn on your ESP32 BLE Gamepad controller
   - Wait for it to start advertising (check controller's serial monitor)

2. **Power On RC Car**
   - Power on the ESP32 RC car receiver
   - It will automatically scan and connect to the controller
   - Check Serial Monitor for connection status

3. **Verify Connection**
   - Serial Monitor should show: `✓✓✓ Successfully connected to gamepad! ✓✓✓`

### Control Scheme

- **Left Joystick Y-Axis**: Forward (up) / Backward (down)
- **Left Joystick X-Axis**: Turn Right (right) / Turn Left (left)
- **Right Joystick**: Not used (reserved for future features)

### Driving Tips

- **Forward/Backward**: Push left joystick up or down
- **Turning**: Push left joystick left or right while moving
- **Smooth Control**: Small joystick movements = slow speed, full movement = max speed
- **Stop**: Release joystick to center position

## 🔧 Troubleshooting

### Car Not Connecting to Controller

1. **Check NimBLE Library**
   - Make sure NimBLE-Arduino library is installed
   - Verify it's the correct version (compatible with your ESP32 board package)

2. **Check Controller Name**
   - Verify `BLE_GAMEPAD_NAME` matches exactly (case-sensitive)
   - Check controller's serial output for its BLE name

3. **Check Controller is Advertising**
   - Controller must be powered on and connected to nothing else
   - Some controllers only advertise when not connected

4. **Reset Both Devices**
   - Power cycle both ESP32 boards
   - Wait 10 seconds and try again

5. **Check Serial Monitor**
   - Look for error messages
   - Verify BLE scanning is working
   - Check for NimBLE-specific error messages

### Motors Not Moving

1. **Check Power**
   - Verify motor battery is connected and charged
   - Check L298N power LED (if present)
   - Ensure ESP32 is powered

2. **Check Connections**
   - Verify all GPIO connections are correct
   - Check motor wires are connected to OUT1/OUT2 and OUT3/OUT4

3. **Test Motors Directly**
   - Temporarily connect motors directly to battery to verify they work
   - Check motor polarity

4. **Check PWM Enable**
   - Some L298N modules have jumpers on ENA/ENB
   - Remove jumpers to enable PWM control

### Motors Spinning Wrong Direction

- Swap motor wires: OUT1 ↔ OUT2 (for left motor) or OUT3 ↔ OUT4 (for right motor)
- Or modify code to invert motor direction

### Car Moves Too Fast/Slow

- Adjust `MAX_SPEED` in code (line 42)
- Reduce value for slower speed (e.g., 150 instead of 200)
- Increase value for faster speed (max 255)

### Jerky Movement

- Increase `MIN_SPEED` if motors don't start smoothly
- Adjust `DEAD_ZONE` if joystick center position causes movement

## 📝 Code Customization

### Change Motor Pins

Edit these lines (around line 35-41) if you need different GPIO pins:
```cpp
#define MOTOR_LEFT_IN1    16
#define MOTOR_LEFT_IN2    17
#define MOTOR_LEFT_PWM    18
#define MOTOR_RIGHT_IN1   19
#define MOTOR_RIGHT_IN2   21
#define MOTOR_RIGHT_PWM   22
```

### Use Right Joystick Instead

Change line 327 from:
```cpp
driveDifferential(gamepadData.leftX, gamepadData.leftY);
```
to:
```cpp
driveDifferential(gamepadData.rightX, gamepadData.rightY);
```

### Add Button Controls

You can add button functions by checking `gamepadData.buttons`. For example:
```cpp
// Emergency stop on PS button
if (gamepadData.buttons & (1 << 12)) {
  stopMotors();
}
```

## 🔋 Power Recommendations

- **Motors**: 7.4V (2S LiPo) to 12V battery pack
- **ESP32**: 5V via USB or regulated power supply
- **Current**: Ensure battery can supply enough current for both motors
- **Safety**: Use appropriate battery protection circuits

## 📚 Additional Resources

- [NimBLE-Arduino Library](https://github.com/h2zero/NimBLE-Arduino) - Required BLE library
- [ESP32 BLE Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/index.html)
- [L298N Motor Driver Datasheet](https://www.st.com/resource/en/datasheet/l298.pdf)
- [Arduino ESP32 Board Support](https://github.com/espressif/arduino-esp32)

## ⚠️ Safety Notes

1. **Battery Safety**: Use proper LiPo battery handling practices
2. **Current Protection**: Add fuses if motors draw high current
3. **Heat**: L298N can get hot - add heatsink if needed
4. **Testing**: Test on a stand before running on ground
5. **Range**: BLE range is typically 10-30 meters, test your range

## 🎯 Next Steps / Enhancements

- Add speed control using triggers
- Implement right joystick for camera/servo control
- Add LED indicators for connection status
- Implement battery level monitoring
- Add obstacle avoidance sensors
- Create a mobile app for additional control

## 📄 License

This code is provided as-is for educational purposes. Modify and use as needed.

---

**Happy Driving! 🚗💨**

