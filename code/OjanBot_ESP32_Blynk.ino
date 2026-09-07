#define BLYNK_TEMPLATE_ID "TMPL6whG54-IM"
#define BLYNK_TEMPLATE_NAME "OjanBot ESP32"
#define BLYNK_AUTH_TOKEN "9h2C"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include "CytronMotorDriver.h"

// WiFi
char ssid[] = "QEatron";
char pass[] = "f5f4f3ff5";

// =========================
// Motor Driver
// =========================
// M1 = Left Motor
// M2 = Right Motor
CytronMD motor1(PWM_PWM, 12, 13);
CytronMD motor2(PWM_PWM, 14, 27);

// =========================
// Servo Pins
// =========================
#define RIGHT_SERVO_PIN 4
#define LEFT_SERVO_PIN 5

Servo rightServo;
Servo leftServo;

// =========================
// Motor Commands
// =========================
#define STOP     0
#define FORWARD  1
#define BACKWARD 2
#define LEFT     3
#define RIGHT    4

// =========================
// Variables
// =========================
int motorSpeed = 150;
int currentCommand = STOP;

// =========================
// Robot Movement
// =========================
void moveRobot(int command)
{
  currentCommand = command;

  Serial.print("Motor command: ");
  Serial.println(command);

  switch (command)
  {
    case FORWARD:
      // Left motor forward
      // Right motor forward
      motor1.setSpeed(+motorSpeed);
      motor2.setSpeed(-motorSpeed);
      break;

    case BACKWARD:
      // Left motor backward
      // Right motor backward
      motor1.setSpeed(-motorSpeed);
      motor2.setSpeed(+motorSpeed);
      break;

    case LEFT:
      // Left motor backward
      // Right motor forward
      motor1.setSpeed(-motorSpeed);
      motor2.setSpeed(-motorSpeed);
      break;

    case RIGHT:
      // Left motor forward
      // Right motor backward
      motor1.setSpeed(+motorSpeed);
      motor2.setSpeed(+motorSpeed);
      break;

    case STOP:
    default:
      motor1.setSpeed(0);
      motor2.setSpeed(0);
      break;
  }
}

// =========================
// Blynk - Motor Control
// V0:
// 0 = STOP
// 1 = FORWARD
// 2 = BACKWARD
// 3 = LEFT
// 4 = RIGHT
// =========================
BLYNK_WRITE(V0)
{
  int command = param.asInt();

  Serial.print("Blynk V0 = ");
  Serial.println(command);

  moveRobot(command);
}

// =========================
// Blynk - Right Arm
// V1 = 0 to 180
// =========================
BLYNK_WRITE(V1)
{
  int angle = constrain(param.asInt(), 0, 180);

  Serial.print("Right Arm = ");
  Serial.println(angle);

  rightServo.write(angle);
}

// =========================
// Blynk - Left Arm
// V2 = 0 to 180
// =========================
BLYNK_WRITE(V2)
{
  int angle = constrain(param.asInt(), 0, 180);

  Serial.print("Left Arm = ");
  Serial.println(angle);

  leftServo.write(angle);
}

// =========================
// Blynk - Motor Speed
// V4 = 0 to 255
// =========================
BLYNK_WRITE(V4)
{
  motorSpeed = constrain(param.asInt(), 0, 255);

  Serial.print("Motor Speed = ");
  Serial.println(motorSpeed);

  // Apply new speed to the current movement
  if (currentCommand != STOP)
  {
    moveRobot(currentCommand);
  }
}

// =========================
// When Blynk connects
// =========================
BLYNK_CONNECTED()
{
  Serial.println("Blynk connected");

  // Set servos to center position
  rightServo.write(90);
  leftServo.write(90);

  // Update Blynk dashboard
  Blynk.virtualWrite(V1, 90);
  Blynk.virtualWrite(V2, 90);

  // Set default motor speed
  motorSpeed = 150;
  Blynk.virtualWrite(V4, 150);

  // Stop motors
  moveRobot(STOP);
}

// =========================
// Setup
// =========================
void setup()
{
  Serial.begin(115200);

  // -------------------------
  // Servo setup
  // -------------------------
  rightServo.setPeriodHertz(50);
  leftServo.setPeriodHertz(50);

  rightServo.attach(RIGHT_SERVO_PIN, 500, 2400);
  leftServo.attach(LEFT_SERVO_PIN, 500, 2400);

  rightServo.write(90);
  leftServo.write(90);

  // -------------------------
  // Stop motors at startup
  // -------------------------
  motor1.setSpeed(0);
  motor2.setSpeed(0);

  // -------------------------
  // Connect to Blynk
  // -------------------------
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Serial.println("OjanBot ESP32 started");
}

// =========================
// Main Loop
// =========================
void loop()
{
  Blynk.run();
}