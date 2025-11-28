# Troubleshooting Guide

## BLE Not Connecting
**Possible Causes**
- Controller name mismatch
- Already connected to phone/PC
- BLE library mismatch

**Fix**
- Ensure name is "BLE Gamepad"
- Use NimBLE on both ESP32s
- Restart both devices

## Motors Not Moving
**Possible Causes**
- Wrong wiring
- Insufficient power
- ENA/ENB jumpers not removed

**Fix**
- Double-check connections
- Check motor battery voltage
- Test motors directly with battery

## Jerky Movement
**Possible Causes**
- Dead zone too small
- Battery voltage drop

**Fix**
- Increase DEAD_ZONE in code
- Replace or charge battery

## ESP32 Resetting
- Motors drawing too much current
- Shared power supply

**Fix**
- Use separate motor power
- Add decoupling capacitors
