# 📍 Code Locations - Complete Mapping

This document clearly shows where each code section is located in the final RC-Car-Controller project.

---

## 🎯 Three Main Code Sections

### 1️⃣ RC CAR CODE (Receiver)
**What:** ESP32 firmware for the RC Car that receives commands and controls motors

**Location:** 
```
RC-Car-Controller-main/
└── rc_car_esp32.ino
```

**Full Path:**
```
E:\RC-Car-Controller-main\rc_car_esp32.ino
```

**Purpose:**
- Controls motors via L298N motor driver
- Receives joystick data from gamepad controller
- Implements differential drive control
- Auto-stops when disconnected

**Upload to:** RC Car ESP32 board

---

### 2️⃣ CONTROLLER CODE (Gamepad/Transmitter)
**What:** ESP32 firmware for the gamepad/controller that sends joystick and button data

**Location:**
```
RC-Car-Controller-main/
└── Controller/
    └── GamepadFirmware/
        └── gamepad_controller.ino
```

**Full Path:**
```
E:\RC-Car-Controller-main\Controller\GamepadFirmware\gamepad_controller.ino
```

**Purpose:**
- Reads joystick and button inputs from hardware
- Sends data via BLE to RC Car
- Advertises as "BLE Gamepad"
- Supports Android mode (compatible with RC Car)

**Upload to:** Controller ESP32 board (separate from RC Car)

**Supporting Library Files:**
```
Controller/GamepadFirmware/library/
├── BleGamepad.h
├── BleGamepad.cpp
├── BleGamepadConfiguration.h
├── BleGamepadConfiguration.cpp
├── BleConnectionStatus.h
├── BleConnectionStatus.cpp
├── BleOutputReceiver.h
├── BleOutputReceiver.cpp
├── BleNUS.h
└── BleNUS.cpp
```

---

### 3️⃣ LINKING CODE (BLE Communication Layer)
**What:** BLE communication code that connects RC Car to Controller

**Location:**
```
RC-Car-Controller-main/
└── Controller/
    ├── GamepadController.h
    └── GamepadController.cpp
```

**Full Paths:**
```
E:\RC-Car-Controller-main\Controller\GamepadController.h
E:\RC-Car-Controller-main\Controller\GamepadController.cpp
```

**Purpose:**
- Scans for "BLE Gamepad" controller
- Establishes BLE HID connection
- Parses HID gamepad reports
- Provides clean API to RC Car code
- Handles auto-reconnection

**Used by:** RC Car code (`rc_car_esp32.ino`)

---

## 📁 Complete File Structure Tree

```
RC-Car-Controller-main/
│
├── 📄 rc_car_esp32.ino                          ← 1️⃣ RC CAR CODE
│
├── 📁 Controller/
│   │
│   ├── 📄 GamepadController.h                   ← 3️⃣ LINKING CODE (Header)
│   ├── 📄 GamepadController.cpp                 ← 3️⃣ LINKING CODE (Implementation)
│   ├── 📄 README.md                            ← Linking code documentation
│   │
│   └── 📁 GamepadFirmware/
│       │
│       ├── 📄 gamepad_controller.ino            ← 2️⃣ CONTROLLER CODE
│       ├── 📄 README.md                        ← Controller documentation
│       │
│       └── 📁 library/                          ← Controller library files
│           ├── 📄 BleGamepad.h
│           ├── 📄 BleGamepad.cpp
│           ├── 📄 BleGamepadConfiguration.h
│           ├── 📄 BleGamepadConfiguration.cpp
│           ├── 📄 BleConnectionStatus.h
│           ├── 📄 BleConnectionStatus.cpp
│           ├── 📄 BleOutputReceiver.h
│           ├── 📄 BleOutputReceiver.cpp
│           ├── 📄 BleNUS.h
│           └── 📄 BleNUS.cpp
│
├── 📄 README.md                                 ← Main project documentation
├── 📄 CODE_LOCATIONS.md                         ← This file (code locations)
├── 📄 CODE_VERIFICATION.md                      ← Code verification report
├── 📄 ALL_CODE_SUMMARY.md                       ← Complete summary
└── 📄 circuit_diagram.md                        ← Hardware wiring guide
```

---

## 🔍 Quick Reference Table

| Code Section | Main File | Location | Purpose |
|--------------|-----------|----------|---------|
| **1. RC Car** | `rc_car_esp32.ino` | Root directory | Controls motors, receives commands |
| **2. Controller** | `gamepad_controller.ino` | `Controller/GamepadFirmware/` | Sends joystick data via BLE |
| **3. Linking** | `GamepadController.h/.cpp` | `Controller/` | BLE communication layer |

---

## 📝 File Details

### Main Firmware Files (Upload to ESP32)

1. **RC Car Firmware**
   - **File:** `rc_car_esp32.ino`
   - **Board:** RC Car ESP32
   - **Dependencies:** NimBLE-Arduino library
   - **Uses:** GamepadController class (linking code)

2. **Controller Firmware**
   - **File:** `Controller/GamepadFirmware/gamepad_controller.ino`
   - **Board:** Controller ESP32 (separate board)
   - **Dependencies:** NimBLE-Arduino library + local library files
   - **Uses:** BleGamepad class (from library folder)

### Support Files

3. **Linking Code**
   - **Header:** `Controller/GamepadController.h`
   - **Implementation:** `Controller/GamepadController.cpp`
   - **Used by:** RC Car firmware
   - **Purpose:** BLE client connection to controller

4. **Controller Library**
   - **Location:** `Controller/GamepadFirmware/library/`
   - **Files:** 10 files (5 headers + 5 implementations)
   - **Used by:** Controller firmware
   - **Purpose:** BLE server/gamepad functionality

---

## 🚀 Quick Access Guide

### To Edit RC Car Code:
```
Open: rc_car_esp32.ino
```

### To Edit Controller Code:
```
Open: Controller/GamepadFirmware/gamepad_controller.ino
```

### To Edit Linking Code:
```
Open: Controller/GamepadController.h (or .cpp)
```

---

## ✅ Verification Checklist

- [x] **RC Car Code** → Located at root: `rc_car_esp32.ino`
- [x] **Controller Code** → Located at: `Controller/GamepadFirmware/gamepad_controller.ino`
- [x] **Linking Code** → Located at: `Controller/GamepadController.*`
- [x] **All library files** → Located at: `Controller/GamepadFirmware/library/`

---

## 📍 Absolute Paths (Windows)

1. **RC Car Code:**
   ```
   E:\RC-Car-Controller-main\rc_car_esp32.ino
   ```

2. **Controller Code:**
   ```
   E:\RC-Car-Controller-main\Controller\GamepadFirmware\gamepad_controller.ino
   ```

3. **Linking Code:**
   ```
   E:\RC-Car-Controller-main\Controller\GamepadController.h
   E:\RC-Car-Controller-main\Controller\GamepadController.cpp
   ```

4. **Controller Library:**
   ```
   E:\RC-Car-Controller-main\Controller\GamepadFirmware\library\
   ```

---

**Last Updated:** 2025-11-28  
**All code sections verified and located** ✅




