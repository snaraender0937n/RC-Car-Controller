/*
 * ESP32 RC Car Receiver (NimBLE Version)
 * Connects to BLE Gamepad Controller and controls RC car motors
 * 
 * This code connects to the ESP32 BLE Gamepad controller and uses
 * the left joystick to control a differential drive RC car.
 * 
 * Uses NimBLE library for compatibility with the controller.
 * 
 * Hardware Requirements:
 * - ESP32 Development Board
 * - Motor Driver (L298N, TB6612FNG, or similar)
 * - 2x DC Motors (for differential drive)
 * - Battery pack (7.4V - 12V recommended for motors)
 * - 5V power supply for ESP32 (can use USB or regulator)
 * 
 * Motor Control:
 * - Left joystick X-axis: Steering (left/right)
 * - Left joystick Y-axis: Forward/Backward
 * 
 * Wiring Configuration (L298N Motor Driver):
 * - Motor Driver IN1 -> ESP32 GPIO 16
 * - Motor Driver IN2 -> ESP32 GPIO 17
 * - Motor Driver ENA -> ESP32 GPIO 18 (PWM)
 * - Motor Driver IN3 -> ESP32 GPIO 19
 * - Motor Driver IN4 -> ESP32 GPIO 21
 * - Motor Driver ENB -> ESP32 GPIO 22 (PWM)
 * 
 * Library Required:
 * - NimBLE-Arduino (install via Arduino Library Manager)
 */

#include <NimBLEDevice.h>
#include <NimBLEClient.h>
#include <NimBLEUtils.h>

// ========== MOTOR CONTROL CONFIGURATION ==========
// Motor Driver Pins (L298N - adjust for your motor driver)
#define MOTOR_LEFT_IN1    16    // Left motor direction pin 1
#define MOTOR_LEFT_IN2    17    // Left motor direction pin 2
#define MOTOR_LEFT_PWM    18    // Left motor PWM pin (ENA)

#define MOTOR_RIGHT_IN1   19    // Right motor direction pin 1
#define MOTOR_RIGHT_IN2   21    // Right motor direction pin 2
#define MOTOR_RIGHT_PWM   22    // Right motor PWM pin (ENB)

// PWM Configuration
#define PWM_FREQUENCY     5000  // PWM frequency in Hz
#define PWM_RESOLUTION    8     // 8-bit resolution (0-255)
#define PWM_CHANNEL_LEFT  0
#define PWM_CHANNEL_RIGHT 1

// Motor speed limits (0-255)
#define MAX_SPEED         200   // Maximum motor speed (reduce if too fast)
#define MIN_SPEED         50    // Minimum speed to overcome friction
#define DEAD_ZONE         500   // Joystick dead zone (out of 4095)

// ========== BLE CONFIGURATION ==========
// The name of your BLE Gamepad controller (must match exactly)
#define BLE_GAMEPAD_NAME  "BLE Gamepad"

// BLE HID Service and Characteristic UUIDs
#define SERVICE_UUID_HID          "1812"  // HID Service
#define CHARACTERISTIC_UUID_INPUT "2A4D"  // HID Input Report

NimBLEClient* pClient = nullptr;
NimBLERemoteCharacteristic* pRemoteCharacteristic = nullptr;
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Gamepad data structure
struct GamepadData {
  int16_t leftX;      // Left joystick X (-32767 to 32767)
  int16_t leftY;      // Left joystick Y (-32767 to 32767)
  int16_t rightX;     // Right joystick X (-32767 to 32767)
  int16_t rightY;     // Right joystick Y (-32767 to 32767)
  bool connected;
};

GamepadData gamepadData = {0, 0, 0, 0, false};

// ========== MOTOR CONTROL FUNCTIONS ==========
void setupMotors() {
  // Configure motor pins
  pinMode(MOTOR_LEFT_IN1, OUTPUT);
  pinMode(MOTOR_LEFT_IN2, OUTPUT);
  pinMode(MOTOR_RIGHT_IN1, OUTPUT);
  pinMode(MOTOR_RIGHT_IN2, OUTPUT);
  
  // Setup PWM channels
  ledcSetup(PWM_CHANNEL_LEFT, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(PWM_CHANNEL_RIGHT, PWM_FREQUENCY, PWM_RESOLUTION);
  
  // Attach PWM channels to pins
  ledcAttachPin(MOTOR_LEFT_PWM, PWM_CHANNEL_LEFT);
  ledcAttachPin(MOTOR_RIGHT_PWM, PWM_CHANNEL_RIGHT);
  
  // Stop motors initially
  stopMotors();
  
  Serial.println("✓ Motors initialized");
}

void stopMotors() {
  digitalWrite(MOTOR_LEFT_IN1, LOW);
  digitalWrite(MOTOR_LEFT_IN2, LOW);
  digitalWrite(MOTOR_RIGHT_IN1, LOW);
  digitalWrite(MOTOR_RIGHT_IN2, LOW);
  ledcWrite(PWM_CHANNEL_LEFT, 0);
  ledcWrite(PWM_CHANNEL_RIGHT, 0);
}

void setMotorSpeed(int motor, int speed) {
  // Clamp speed to valid range
  speed = constrain(speed, -MAX_SPEED, MAX_SPEED);
  
  if (motor == 0) { // Left motor
    if (speed > 0) {
      digitalWrite(MOTOR_LEFT_IN1, HIGH);
      digitalWrite(MOTOR_LEFT_IN2, LOW);
      ledcWrite(PWM_CHANNEL_LEFT, abs(speed));
    } else if (speed < 0) {
      digitalWrite(MOTOR_LEFT_IN1, LOW);
      digitalWrite(MOTOR_LEFT_IN2, HIGH);
      ledcWrite(PWM_CHANNEL_LEFT, abs(speed));
    } else {
      digitalWrite(MOTOR_LEFT_IN1, LOW);
      digitalWrite(MOTOR_LEFT_IN2, LOW);
      ledcWrite(PWM_CHANNEL_LEFT, 0);
    }
  } else { // Right motor
    if (speed > 0) {
      digitalWrite(MOTOR_RIGHT_IN1, HIGH);
      digitalWrite(MOTOR_RIGHT_IN2, LOW);
      ledcWrite(PWM_CHANNEL_RIGHT, abs(speed));
    } else if (speed < 0) {
      digitalWrite(MOTOR_RIGHT_IN1, LOW);
      digitalWrite(MOTOR_RIGHT_IN2, HIGH);
      ledcWrite(PWM_CHANNEL_RIGHT, abs(speed));
    } else {
      digitalWrite(MOTOR_RIGHT_IN1, LOW);
      digitalWrite(MOTOR_RIGHT_IN2, LOW);
      ledcWrite(PWM_CHANNEL_RIGHT, 0);
    }
  }
}

// Differential drive control using left joystick
void driveDifferential(int16_t x, int16_t y) {
  // Convert joystick values (-32767 to 32767) to motor speeds
  // Y-axis: Forward (positive) / Backward (negative)
  // X-axis: Right (positive) / Left (negative)
  
  // Apply dead zone
  if (abs(x) < DEAD_ZONE) x = 0;
  if (abs(y) < DEAD_ZONE) y = 0;
  
  // Normalize to -1.0 to 1.0
  float forward = (float)y / 32767.0;  // -1.0 to 1.0 (forward/backward)
  float turn = (float)x / 32767.0;     // -1.0 to 1.0 (left/right)
  
  // Calculate left and right motor speeds
  // When turning right: right motor slows down
  // When turning left: left motor slows down
  float leftSpeed = forward + turn;   // Add turn to left motor
  float rightSpeed = forward - turn;  // Subtract turn from right motor
  
  // Normalize to prevent exceeding max speed
  float maxSpeed = max(abs(leftSpeed), abs(rightSpeed));
  if (maxSpeed > 1.0) {
    leftSpeed /= maxSpeed;
    rightSpeed /= maxSpeed;
  }
  
  // Convert to motor speed values
  int leftMotor = (int)(leftSpeed * MAX_SPEED);
  int rightMotor = (int)(rightSpeed * MAX_SPEED);
  
  // Apply minimum speed threshold to overcome friction
  if (abs(leftMotor) > 0 && abs(leftMotor) < MIN_SPEED) {
    leftMotor = leftMotor > 0 ? MIN_SPEED : -MIN_SPEED;
  }
  if (abs(rightMotor) > 0 && abs(rightMotor) < MIN_SPEED) {
    rightMotor = rightMotor > 0 ? MIN_SPEED : -MIN_SPEED;
  }
  
  // Apply speeds to motors
  setMotorSpeed(0, leftMotor);
  setMotorSpeed(1, rightMotor);
}

// ========== BLE CALLBACKS ==========
class MyClientCallback : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient* pclient) {
    Serial.println("✓ Connected to gamepad");
    deviceConnected = true;
    gamepadData.connected = true;
  }

  void onDisconnect(NimBLEClient* pclient) {
    Serial.println("✗ Disconnected from gamepad");
    deviceConnected = false;
    gamepadData.connected = false;
    stopMotors(); // Stop car when disconnected
  }

  // NimBLE may call this when connection parameters need updating
  bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) {
    return true;
  }
};

// Notification callback for gamepad data
static void notifyCallback(NimBLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData, size_t length, bool isNotify) {
  // Parse HID gamepad report
  // HID Report format depends on the gamepad configuration
  // For Android mode: [buttons] [special] [X_L] [X_H] [Y_L] [Y_H] [Z_L] [Z_H] [RX_L] [RX_H] [RY_L] [RY_H]
  // X, Y = left thumb, Z, RX = right thumb, RY might not be used
  
  if (length < 8) return; // Need at least 8 bytes
  
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

// ========== BLE CONNECTION FUNCTIONS ==========
bool connectToGamepad() {
  Serial.print("Scanning for: ");
  Serial.println(BLE_GAMEPAD_NAME);
  
  NimBLEScan* pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setActiveScan(true); // Active scan uses more power, but gets results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // Less or equal setInterval value
  
  // Scan for 5 seconds
  NimBLEScanResults foundDevices = pBLEScan->start(5, false);
  
  Serial.print("Found ");
  Serial.print(foundDevices.getCount());
  Serial.println(" devices");
  
  for (int i = 0; i < foundDevices.getCount(); i++) {
    NimBLEAdvertisedDevice device = foundDevices.getDevice(i);
    
    Serial.print("Device ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(device.toString().c_str());
    
    if (device.getName() == BLE_GAMEPAD_NAME) {
      Serial.println("✓ Found gamepad! Connecting...");
      
      pClient = NimBLEDevice::createClient();
      pClient->setClientCallbacks(new MyClientCallback());
      
      if (pClient->connect(&device)) {
        Serial.println("✓ Connected to device");
        
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
          Serial.println("✓ Registered for notifications");
          
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
  
  Serial.println("✗ Gamepad not found");
  pBLEScan->clearResults();
  return false;
}

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔═══════════════════════════════════════╗");
  Serial.println("║   ESP32 RC Car Receiver Starting...  ║");
  Serial.println("║        (NimBLE Version)               ║");
  Serial.println("╚═══════════════════════════════════════╝\n");
  
  // Initialize motors
  setupMotors();
  
  // Initialize NimBLE
  NimBLEDevice::init("");
  
  // Try to connect to gamepad
  Serial.println("Attempting to connect to gamepad...");
  if (connectToGamepad()) {
    Serial.println("\n✓✓✓ Successfully connected to gamepad! ✓✓✓");
  } else {
    Serial.println("\n✗ Failed to connect. Will retry in loop()");
  }
  
  Serial.println("\nSetup complete. Waiting for joystick input...\n");
}

// ========== MAIN LOOP ==========
void loop() {
  // Handle connection state
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // Give the Bluetooth stack time
    pClient = nullptr;
    oldDeviceConnected = deviceConnected;
    stopMotors();
  }
  
  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
  
  // Try to reconnect if disconnected
  if (!deviceConnected) {
    Serial.println("Attempting to reconnect...");
    delay(2000);
    if (connectToGamepad()) {
      Serial.println("✓ Reconnected to gamepad!");
    }
    return;
  }
  
  // Control motors based on gamepad input
  if (gamepadData.connected) {
    // Use left joystick for differential drive control
    driveDifferential(gamepadData.leftX, gamepadData.leftY);
    
    // Debug output (uncomment for debugging)
    static unsigned long lastDebugTime = 0;
    if (millis() - lastDebugTime > 500) { // Print every 500ms
      Serial.printf("LX: %6d | LY: %6d | RX: %6d | RY: %6d\n", 
                    gamepadData.leftX, gamepadData.leftY, 
                    gamepadData.rightX, gamepadData.rightY);
      lastDebugTime = millis();
    }
  } else {
    stopMotors();
  }
  
  delay(10); // Small delay to prevent overwhelming the system
}
