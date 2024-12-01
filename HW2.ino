#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 16, 2); // Change the address if needed (e.g., 0x3F)

// Motor control pins
const int motorIn1 = 7;  // L298N IN1
const int motorIn2 = 8;  // L298N IN2
const int motorPWM = 9;  // L298N ENA (PWM)

// Encoder pin
const int encoderA = 3;  // Encoder A connected to pin 3 (interrupt pin)

// Potentiometer pin
const int potPin = A0;   // Potentiometer connected to A0

// Variables for motor control and position tracking
volatile int encoderCount = 0;  // Tracks encoder pulses
int motorSpeed = 0;             // Motor speed (0-255)
String motorDirection = "STOP"; // Stores motor direction
int potValue = 0;               // Potentiometer value

// Constants for encoder calculations
const int pulsesPerRevolution = 140;  // Adjust based on your encoder's specification
const int degreesPerPulse = 360 / pulsesPerRevolution;  // Degrees moved per pulse

void setup() {
  // Serial communication for debugging
  Serial.begin(9600);

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Set motor pins as output
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorPWM, OUTPUT);

  // Set encoder pin as input
  pinMode(encoderA, INPUT_PULLUP);

  // Attach interrupt to encoder pin
  attachInterrupt(digitalPinToInterrupt(encoderA), updateEncoder, CHANGE);

  // Initial motor setup
  motorSpeed = 150;  // Set default speed
}

void loop() {
  // Read potentiometer value
  potValue = analogRead(potPin);

  // Print potentiometer value for debugging
  Serial.print("Potentiometer Value: ");
  Serial.println(potValue);

  // Calculate motor position in degrees
  int position =(encoderCount * degreesPerPulse) % 360;
  if (position < 0) {
  position += 360;  // Ensure position is always positive
}

  // Determine motor direction based on potentiometer value
  if (potValue < 490) {
    setMotorDirection("CCW");
  } else if (potValue > 530) {
    setMotorDirection("CW");
  } else {
    setMotorDirection("STOP");
  }

  // Display motor direction and position on LCD
  lcd.setCursor(0, 0);
  lcd.print("Dir: " + motorDirection + "   "); // Clear remaining characters
  lcd.setCursor(0, 1);
  lcd.print("Pos: " + String(position) + "°   ");

  // Delay for LCD update
  delay(200);
}

// Function to set motor direction
void setMotorDirection(String direction) {
  motorDirection = direction;
  if (direction == "CW") {
    digitalWrite(motorIn1, HIGH);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorPWM, motorSpeed);
  } else if (direction == "CCW") {
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, HIGH);
    analogWrite(motorPWM, motorSpeed);
  } else {
    digitalWrite(motorIn1, LOW);
    digitalWrite(motorIn2, LOW);
    analogWrite(motorPWM, 0);
  }
}

// Interrupt service routine for encoder
void updateEncoder() {
  // Check motor direction and update encoder count
  if (digitalRead(motorIn1) == HIGH && digitalRead(motorIn2) == LOW) {
    encoderCount++;  // CW direction
  } else if (digitalRead(motorIn1) == LOW && digitalRead(motorIn2) == HIGH) {
    encoderCount--;  // CCW direction
  }
}
