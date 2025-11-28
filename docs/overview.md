# ESP32 RC Car with BLE Gamepad – Overview

This project implements a **Bluetooth Low Energy (BLE) controlled RC Car**
using **two ESP32 boards**:

- One ESP32 acts as a **BLE Gamepad (controller)**
- One ESP32 acts as the **RC Car receiver**

The controller sends joystick data wirelessly using BLE.
The RC car receives this data and controls motors through an **L298N motor driver**
using differential drive logic.

## Key Features
- ESP32 ↔ ESP32 BLE communication (NimBLE)
- Differential motor control
- Auto-reconnect on BLE disconnect
- Safety auto-stop when signal is lost
- Modular and well-documented codebase

## System Architecture
Controller ESP32 → BLE → RC Car ESP32 → L298N → Motors

This repository contains **all firmware, libraries, and documentation**
needed to build and test the system.
