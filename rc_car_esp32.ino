/*
 * ESP32 RC Car Receiver (NimBLE Version)
 * Connects to BLE Gamepad Controller and controls RC car motors
 * 
 * This code connects to the ESP32 BLE Gamepad controller from
 * Controller-PC-Mobile project and uses the left joystick to 
 * control a differential drive RC car.
 * 
 * Uses GamepadController module for BLE communication.
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

#include "Controller/GamepadController.h"

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

// ========== CONTROLLER LINKING ==========
// GamepadController instance - links to Controller-PC-Mobile project
GamepadController gamepad;

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

// ========== CONTROLLER LINKING SECTION ==========
// All BLE gamepad connection logic is now handled by GamepadController class
// See Controller/GamepadController.h and Controller/GamepadController.cpp

// ========== SETUP ==========
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n╔═══════════════════════════════════════╗");
  Serial.println("║   ESP32 RC Car Receiver Starting...  ║");
  Serial.println("║   Linked to Controller-PC-Mobile      ║");
  Serial.println("╚═══════════════════════════════════════╝\n");
  
  // Initialize motors
  setupMotors();
  
  // Initialize and link to Controller-PC-Mobile gamepad
  gamepad.begin();
  
  Serial.println("\nSetup complete. Waiting for joystick input...\n");
}

// ========== MAIN LOOP ==========
void loop() {
  // Update controller connection state (handles reconnect automatically)
  gamepad.update();
  
  // Get current gamepad data
  GamepadData gamepadData = gamepad.getData();
  
  // Control motors based on gamepad input
  if (gamepad.isConnected()) {
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
    // Stop motors when disconnected
    stopMotors();
  }
  
  delay(10); // Small delay to prevent overwhelming the system
}
