#ifndef GAMEPAD_CONTROLLER_H
#define GAMEPAD_CONTROLLER_H

#include <NimBLEDevice.h>
#include <NimBLEClient.h>
#include <NimBLEUtils.h>

/**
 * GamepadController - Links RC Car to BLE Gamepad from Controller-PC-Mobile
 * 
 * This module handles all BLE communication between the RC Car receiver
 * and the ESP32 BLE Gamepad controller from the Controller-PC-Mobile project.
 * 
 * Controller Settings:
 * - Name: "BLE Gamepad"
 * - Manufacturer: "ESP"
 * - Type: Gamepad (Android mode compatible)
 */

// BLE Configuration - Must match Controller-PC-Mobile settings
#define BLE_GAMEPAD_NAME  "BLE Gamepad"    // Must match controller name exactly
#define SERVICE_UUID_HID          "1812"   // HID Service UUID
#define CHARACTERISTIC_UUID_INPUT "2A4D"   // HID Input Report UUID

/**
 * GamepadData structure - Stores joystick and button data
 */
struct GamepadData {
  int16_t leftX;      // Left joystick X (-32767 to 32767)
  int16_t leftY;      // Left joystick Y (-32767 to 32767)
  int16_t rightX;     // Right joystick X (-32767 to 32767)
  int16_t rightY;     // Right joystick Y (-32767 to 32767)
  bool connected;     // Connection status
};

/**
 * GamepadController Class
 * Handles BLE connection and data reception from the gamepad
 */
class GamepadController {
private:
  NimBLEClient* pClient;
  NimBLERemoteCharacteristic* pRemoteCharacteristic;
  bool deviceConnected;
  bool oldDeviceConnected;
  GamepadData gamepadData;
  
  // Internal callback classes
  class ClientCallback : public NimBLEClientCallbacks {
    GamepadController* parent;
  public:
    ClientCallback(GamepadController* p) : parent(p) {}
    void onConnect(NimBLEClient* pclient);
    void onDisconnect(NimBLEClient* pclient);
    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params);
  };
  
  // Static notification callback wrapper
  static void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                              uint8_t* pData, size_t length, bool isNotify);
  
  // Helper functions
  bool scanAndConnect();
  void parseGamepadReport(uint8_t* pData, size_t length);
  
public:
  GamepadController();
  ~GamepadController();
  
  /**
   * Initialize the BLE controller
   * Call this in setup()
   */
  bool begin();
  
  /**
   * Update controller state
   * Call this in loop() to handle connection management
   */
  void update();
  
  /**
   * Check if gamepad is connected
   */
  bool isConnected() const;
  
  /**
   * Get current gamepad data
   */
  GamepadData getData() const;
  
  /**
   * Get connection status (for backward compatibility)
   */
  bool getConnectionStatus() const;
  
  /**
   * Manually attempt to connect
   */
  bool connect();
  
  /**
   * Disconnect from gamepad
   */
  void disconnect();
  
  // Static instance for callback access
  static GamepadController* instance;
};

#endif // GAMEPAD_CONTROLLER_H

