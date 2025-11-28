#include "GamepadController.h"

// Static instance pointer for callback access
GamepadController* GamepadController::instance = nullptr;

/**
 * GamepadController Constructor
 */
GamepadController::GamepadController() 
  : pClient(nullptr)
  , pRemoteCharacteristic(nullptr)
  , deviceConnected(false)
  , oldDeviceConnected(false)
{
  // Initialize gamepad data
  gamepadData = {0, 0, 0, 0, false};
  
  // Set static instance for callback access
  instance = this;
}

/**
 * GamepadController Destructor
 */
GamepadController::~GamepadController() {
  disconnect();
  instance = nullptr;
}

/**
 * Initialize BLE and attempt connection
 */
bool GamepadController::begin() {
  Serial.println("\n╔════════════════════════════════════════╗");
  Serial.println("║   GamepadController Initializing...  ║");
  Serial.println("║   Linking to Controller-PC-Mobile    ║");
  Serial.println("╚════════════════════════════════════════╝\n");
  
  // Initialize NimBLE
  NimBLEDevice::init("");
  
  Serial.print("Looking for controller: ");
  Serial.println(BLE_GAMEPAD_NAME);
  Serial.println("(From Controller-PC-Mobile project)\n");
  
  // Attempt initial connection
  if (connect()) {
    Serial.println("\n✓✓✓ Successfully linked to gamepad! ✓✓✓\n");
    return true;
  } else {
    Serial.println("\n✗ Initial connection failed. Will retry in loop()\n");
    return false;
  }
}

/**
 * Update controller state - call in loop()
 */
void GamepadController::update() {
  // Handle disconnection
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // Give the Bluetooth stack time
    pClient = nullptr;
    oldDeviceConnected = deviceConnected;
    gamepadData.connected = false;
    Serial.println("Gamepad disconnected - motors stopped");
  }
  
  // Handle new connection
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  
  // Attempt reconnection if disconnected
  if (!deviceConnected) {
    static unsigned long lastReconnectAttempt = 0;
    if (millis() - lastReconnectAttempt > 2000) {
      lastReconnectAttempt = millis();
      Serial.println("Attempting to reconnect to gamepad...");
      connect();
    }
  }
}

/**
 * Check if connected
 */
bool GamepadController::isConnected() const {
  return deviceConnected && gamepadData.connected;
}

/**
 * Get gamepad data
 */
GamepadData GamepadController::getData() const {
  return gamepadData;
}

/**
 * Get connection status (backward compatibility)
 */
bool GamepadController::getConnectionStatus() const {
  return isConnected();
}

/**
 * Scan for and connect to gamepad
 */
bool GamepadController::scanAndConnect() {
  Serial.print("Scanning for: ");
  Serial.println(BLE_GAMEPAD_NAME);
  
  NimBLEScan* pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);
  
  // Scan for 5 seconds
  NimBLEScanResults foundDevices = pBLEScan->start(5, false);
  
  Serial.print("Found ");
  Serial.print(foundDevices.getCount());
  Serial.println(" BLE devices");
  
  for (int i = 0; i < foundDevices.getCount(); i++) {
    NimBLEAdvertisedDevice device = foundDevices.getDevice(i);
    
    Serial.print("Device ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(device.toString().c_str());
    
    // Check if this is our gamepad
    if (device.getName() == BLE_GAMEPAD_NAME) {
      Serial.println("✓ Found Controller-PC-Mobile gamepad! Connecting...");
      
      // Create client
      if (pClient != nullptr) {
        pClient->disconnect();
        delete pClient;
      }
      
      pClient = NimBLEDevice::createClient();
      pClient->setClientCallbacks(new ClientCallback(this));
      
      // Connect to device
      if (pClient->connect(&device)) {
        Serial.println("✓ Connected to gamepad device");
        
        // Get HID Service
        NimBLERemoteService* pRemoteService = pClient->getService(NimBLEUUID(SERVICE_UUID_HID));
        if (pRemoteService == nullptr) {
          Serial.println("✗ Failed to find HID service");
          pClient->disconnect();
          return false;
        }
        
        Serial.println("✓ Found HID service");
        
        // Get Input Characteristic
        pRemoteCharacteristic = pRemoteService->getCharacteristic(NimBLEUUID(CHARACTERISTIC_UUID_INPUT));
        if (pRemoteCharacteristic == nullptr) {
          Serial.println("✗ Failed to find input characteristic");
          pClient->disconnect();
          return false;
        }
        
        Serial.println("✓ Found input characteristic");
        
        // Register for notifications
        if (pRemoteCharacteristic->canNotify()) {
          pRemoteCharacteristic->subscribe(true, notifyCallback);
          Serial.println("✓ Registered for gamepad notifications");
          
          return true;
        } else {
          Serial.println("✗ Characteristic does not support notifications");
          pClient->disconnect();
          return false;
        }
      } else {
        Serial.println("✗ Failed to connect to device");
      }
    }
  }
  
  Serial.println("✗ Gamepad not found in scan");
  pBLEScan->clearResults();
  return false;
}

/**
 * Connect to gamepad
 */
bool GamepadController::connect() {
  return scanAndConnect();
}

/**
 * Disconnect from gamepad
 */
void GamepadController::disconnect() {
  if (pClient != nullptr && pClient->isConnected()) {
    pClient->disconnect();
  }
  deviceConnected = false;
  gamepadData.connected = false;
}

/**
 * Parse gamepad HID report
 * Compatible with Controller-PC-Mobile Android mode format
 */
void GamepadController::parseGamepadReport(uint8_t* pData, size_t length) {
  // Minimum required length
  if (length < 8) return;
  
  // HID Report format from Controller-PC-Mobile (Android mode):
  // [buttons] [special] [X_L] [X_H] [Y_L] [Y_H] [Z_L] [Z_H] [RX_L] [RX_H] [RY_L] [RY_H]
  // X, Y = left thumb, Z, RX = right thumb
  
  int offset = 1; // Skip button byte
  
  // Check if there are special buttons (usually 1 byte after buttons)
  if (length > 9) {
    offset = 2; // Skip button + special button bytes
  }
  
  // Extract joystick axes (little-endian 16-bit values)
  if (length >= offset + 8) {
    // Left joystick (X, Y)
    gamepadData.leftX = (int16_t)(pData[offset] | (pData[offset + 1] << 8));
    gamepadData.leftY = (int16_t)(pData[offset + 2] | (pData[offset + 3] << 8));
    
    // Right joystick (Z, RX for Android mode)
    gamepadData.rightX = (int16_t)(pData[offset + 4] | (pData[offset + 5] << 8));
    gamepadData.rightY = (int16_t)(pData[offset + 6] | (pData[offset + 7] << 8));
    
    // Convert unsigned to signed if needed
    // HID values are typically 0-65535, center at 32767
    // We need to convert to -32767 to 32767
    if (gamepadData.leftX > 32767) gamepadData.leftX -= 65536;
    if (gamepadData.leftY > 32767) gamepadData.leftY -= 65536;
    if (gamepadData.rightX > 32767) gamepadData.rightX -= 65536;
    if (gamepadData.rightY > 32767) gamepadData.rightY -= 65536;
  }
}

/**
 * Static notification callback
 * This is called when gamepad sends data
 */
void GamepadController::notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                                        uint8_t* pData, size_t length, bool isNotify) {
  // Use static instance to access member functions
  if (instance != nullptr) {
    instance->parseGamepadReport(pData, length);
  }
}

/**
 * ClientCallback::onConnect
 */
void GamepadController::ClientCallback::onConnect(NimBLEClient* pclient) {
  Serial.println("✓ Connected to Controller-PC-Mobile gamepad");
  if (parent) {
    parent->deviceConnected = true;
    parent->gamepadData.connected = true;
  }
}

/**
 * ClientCallback::onDisconnect
 */
void GamepadController::ClientCallback::onDisconnect(NimBLEClient* pclient) {
  Serial.println("✗ Disconnected from gamepad");
  if (parent) {
    parent->deviceConnected = false;
    parent->gamepadData.connected = false;
  }
}

/**
 * ClientCallback::onConnParamsUpdateRequest
 */
bool GamepadController::ClientCallback::onConnParamsUpdateRequest(NimBLEClient* pClient, 
                                                                    const ble_gap_upd_params* params) {
  return true;
}

