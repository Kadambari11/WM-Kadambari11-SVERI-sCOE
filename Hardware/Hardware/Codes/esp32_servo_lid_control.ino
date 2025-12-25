#include <ESP32Servo.h>

// Ultrasonic sensor pins
#define TRIG_PIN 5
#define ECHO_PIN 18

// LED pins
#define YELLOW_LED 14
#define ORANGE_LED 27
#define RED_LED 26

// Servo pin
#define SERVO_PIN 25

Servo lidServo;

// Bin parameters
float binDepth = 30.0;   // Total depth of bin in cm
long duration;
float distance;
float fillPercentage;

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(YELLOW_LED, OUTPUT);
  pinMode(ORANGE_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  lidServo.attach(SERVO_PIN);
  lidServo.write(0);   // Lid closed position

  Serial.println("Smart Waste Bin System Started");
}

// Function to measure distance
float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms
  float dist = duration * 0.034 / 2;
  return dist;
}

void loop() {

  // Take multiple readings for reliability
  float sum = 0;
  for (int i = 0; i < 5; i++) {
    sum += getDistance();
    delay(50);
  }
  distance = sum / 5;

  // Validate distance
  if (distance <= 0 || distance > binDepth) {
    Serial.println("Invalid reading detected");
    delay(2000);
    return;
  }

  // Calculate fill percentage
  fillPercentage = ((binDepth - distance) / binDepth) * 100;
  fillPercentage = constrain(fillPercentage, 0, 100);

  Serial.print("Fill Level: ");
  Serial.print(fillPercentage);
  Serial.println(" %");

  // Reset LEDs
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(ORANGE_LED, LOW);
  digitalWrite(RED_LED, LOW);

  // LED conditions
  if (fillPercentage >= 75 && fillPercentage < 85) {
    digitalWrite(YELLOW_LED, HIGH);
  }
  else if (fillPercentage >= 85 && fillPercentage <= 90) {
    digitalWrite(ORANGE_LED, HIGH);
  }
  else if (fillPercentage >= 98 && fillPercentage <= 100) {
    digitalWrite(RED_LED, HIGH);
  }

  // Automatic lid opening (user proximity)
  if (distance < 12) {        // User detected
    Serial.println("User detected - Opening lid");
    lidServo.write(90);       // Open lid
    delay(3000);
    lidServo.write(0);        // Close lid
  }

  delay(5000);  // Measurement interval
}

