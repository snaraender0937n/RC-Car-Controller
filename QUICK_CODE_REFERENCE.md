# 🚀 Quick Code Reference - Where Is Everything?

## 📍 All 3 Code Sections - Exact Locations

### 1️⃣ RC CAR CODE
```
📄 rc_car_esp32.ino
```
**Full Path:** `E:\RC-Car-Controller-main\rc_car_esp32.ino`

---

### 2️⃣ CONTROLLER CODE  
```
📁 Controller/
   └── 📁 GamepadFirmware/
       └── 📄 gamepad_controller.ino
```
**Full Path:** `E:\RC-Car-Controller-main\Controller\GamepadFirmware\gamepad_controller.ino`

---

### 3️⃣ LINKING CODE
```
📁 Controller/
   ├── 📄 GamepadController.h
   └── 📄 GamepadController.cpp
```
**Full Paths:**
- `E:\RC-Car-Controller-main\Controller\GamepadController.h`
- `E:\RC-Car-Controller-main\Controller\GamepadController.cpp`

---

## 📋 Summary Table

| Code | File Name | Folder | Board |
|------|-----------|--------|-------|
| **RC Car** | `rc_car_esp32.ino` | Root | RC Car ESP32 |
| **Controller** | `gamepad_controller.ino` | `Controller/GamepadFirmware/` | Controller ESP32 |
| **Linking** | `GamepadController.h/.cpp` | `Controller/` | Used by RC Car |

---

## 🔗 How They Connect

```
Controller ESP32                    RC Car ESP32
     │                                    │
     │                                    │
[gamepad_controller.ino]         [rc_car_esp32.ino]
     │                                    │
     │         BLE Connection             │
     ├────────────────────────────────────┤
     │                                    │
     │                            [GamepadController.*]
     │                                    │
     ▼                                    ▼
  Motors Controlled
```

---

**For detailed information, see:** `CODE_LOCATIONS.md`




