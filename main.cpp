#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with I2C address 0x27, 16 columns, and 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo objects for controlling the motors and steering
Servo continuousServo1, continuousServo2, steeringServo;

// Pin definitions

#define FLEX_MOVE A1      // Flex sensor to move/stop the wheelchair
#define FLEX_LEFT A3       // Flex sensor to steer left
#define FLEX_RIGHT A2   // Flex sensor to steer right
#define ULTRASONIC_PIN 12  // Ultrasonic sensor pin (combined trigger/echo)
#define BUZZER 3           // Buzzer pin for alerts
#define LDR A0            // Light-dependent resistor pin for day/night detection
#define LED 4              // LED for lighting

// Variables for storing sensor readings
float temperature;
long duration;
int distance;

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();

  // Attach servos to respective pins
  continuousServo1.attach(5);
  continuousServo2.attach(6);
  steeringServo.attach(11);

  // Configure pin modes
  pinMode(ULTRASONIC_PIN, OUTPUT);  // Shared trigger/echo pin for ultrasonic sensor
  pinMode(BUZZER, OUTPUT);
  pinMode(LED, OUTPUT);
  
  steeringServo.write(180);

  // Begin serial communication for debugging
  Serial.begin(9600);
}

void loop() {
  // === Ultrasonic Sensor ===
  // Trigger the ultrasonic sensor
  pinMode(ULTRASONIC_PIN, OUTPUT);
  digitalWrite(ULTRASONIC_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_PIN, LOW);

  // Measure the echo pulse duration
  pinMode(ULTRASONIC_PIN, INPUT);
  duration = pulseIn(ULTRASONIC_PIN, HIGH);

  // Calculate distance in cm
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Alert if an obstacle is nearby
  if (distance < 30) {
    lcd.setCursor(0, 0);
    lcd.print("Obstacle Nearby");
    digitalWrite(BUZZER, HIGH);
  } else {
    lcd.clear();
    digitalWrite(BUZZER, LOW);
  }

  // === LDR Sensor (Light Detection) ===
  int ldrValue = analogRead(LDR);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // Turn on the LED if it's dark
  if(ldrValue<300){
    lcd.setCursor(0,1);
    lcd.print("LED On!");
    digitalWrite(LED, HIGH);}
  else{
    lcd.setCursor(0,1);
    lcd.print("LED Off!");
    digitalWrite(LED, LOW);
  }

  // === Steering Control ===
  int flexLeft = analogRead(FLEX_LEFT);
  int flexRight = analogRead(FLEX_RIGHT);

  Serial.print("Flex Left: ");
  Serial.print(flexLeft);
  Serial.print(", Flex Right: ");
  Serial.println(flexRight);

  // Control steering based on flex sensor input
  if (flexLeft < 100) {
    lcd.setCursor(0,0);
  lcd.print("Turning Left");
  delay(500);
  lcd.clear();
    steeringServo.write(135); // Turn left
  } else if (flexRight < 100) {
    lcd.setCursor(0,0);
  lcd.print("Turning Right");
  delay(500);
  lcd.clear();
    steeringServo.write(45); // Turn right
  } else {
    steeringServo.write(90); // Center position
  }

  // === Movement Control ===
int flexMove = analogRead(FLEX_MOVE);

Serial.print("Flex Move: ");
Serial.println(flexMove);

// Control movement based on flex sensor input
if (flexMove < 100) {
  lcd.setCursor(0,0);
  lcd.print("Stopped");
  delay(500);
  lcd.clear();
      
    continuousServo1.write(93); // Stop continuous servo 1
    continuousServo2.write(93); // Stop continuous servo 2
} else {
  lcd.setCursor(0,0);
  lcd.print("Moving");
  
  delay(500);
  lcd.clear();
    continuousServo1.write(0);  // Move forward (adjust for your servo's direction)
    continuousServo2.write(0);  // Move forward (adjust for your servo's direction)
}

  // Small delay for stability
  delay(500);
