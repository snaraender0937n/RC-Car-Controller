# Controller Module - BLE Gamepad Linking

This module provides the interface for linking the RC Car Receiver to the **Controller-PC-Mobile** project's BLE gamepad.

## Overview

The `GamepadController` class encapsulates all BLE communication between the RC Car ESP32 and the gamepad controller ESP32 from the Controller-PC-Mobile project.

## Files

- **GamepadController.h** - Header file with class definition and interface
- **GamepadController.cpp** - Implementation of BLE connection and data parsing

## Connection Details

### Controller Settings (Controller-PC-Mobile)
- **Name**: "BLE Gamepad"
- **Manufacturer**: "ESP"
- **Type**: Gamepad (Android mode compatible)
- **Service UUID**: 1812 (HID Service)
- **Characteristic UUID**: 2A4D (HID Input Report)

### Compatibility

This module is designed to work with the Controller-PC-Mobile project's gamepad firmware:
- Uses NimBLE library (same as controller)
- Parses HID gamepad reports in Android mode format
- Supports automatic reconnection

## Usage

### Basic Setup

```cpp
#include "Controller/GamepadController.h"

GamepadController gamepad;

void setup() {
  // Initialize gamepad controller
  gamepad.begin();
}

void loop() {
  // Update controller state
  gamepad.update();
  
  // Check connection and get data
  if (gamepad.isConnected()) {
    GamepadData data = gamepad.getData();
    // Use data.leftX, data.leftY, etc.
  }
}
```

### API Reference

#### `bool begin()`
Initializes BLE and attempts to connect to the gamepad. Returns `true` if initial connection succeeds.

#### `void update()`
Updates controller state. Call this in `loop()`. Handles reconnection automatically.

#### `bool isConnected()`
Returns `true` if gamepad is currently connected.

#### `GamepadData getData()`
Returns the current gamepad data structure with joystick values.

#### `GamepadData Structure`
```cpp
struct GamepadData {
  int16_t leftX;      // Left joystick X (-32767 to 32767)
  int16_t leftY;      // Left joystick Y (-32767 to 32767)
  int16_t rightX;     // Right joystick X (-32767 to 32767)
  int16_t rightY;     // Right joystick Y (-32767 to 32767)
  bool connected;     // Connection status
};
```

## Connection Flow

1. **Scan**: Searches for "BLE Gamepad" device
2. **Connect**: Establishes BLE connection
3. **Discover Services**: Finds HID service (UUID: 1812)
4. **Subscribe**: Registers for notifications on input characteristic
5. **Receive Data**: Automatically parses incoming joystick data

## Troubleshooting

### Controller Not Found
- Verify controller name matches exactly: "BLE Gamepad"
- Ensure controller is powered on and advertising
- Check that controller is not connected to another device

### Connection Drops
- The module automatically attempts reconnection every 2 seconds
- Check BLE range (typically 10-30 meters)
- Verify both devices have stable power

### Data Not Updating
- Ensure `gamepad.update()` is called in `loop()`
- Check Serial Monitor for connection status messages
- Verify controller is sending joystick data

## Dependencies

- **NimBLE-Arduino** library (install via Arduino Library Manager)
- ESP32 board package

## Integration with Controller-PC-Mobile

This module is specifically designed to work with:
- Controller name: "BLE Gamepad"
- Controller mode: Android (CONTROLLER_TYPE_GAMEPAD)
- HID report format: Android gamepad mode

To ensure compatibility:
1. Upload Controller-PC-Mobile firmware to controller ESP32
2. Set controller name to "BLE Gamepad" (default)
3. Set controller mode to ANDROID (default in sketch.ino)

## Notes

- Connection is handled automatically with auto-reconnect
- Joystick values are converted from HID format (-32767 to 32767)
- Motor control logic is separate and handled in main sketch

