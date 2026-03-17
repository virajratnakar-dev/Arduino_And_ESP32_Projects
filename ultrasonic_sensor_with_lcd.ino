#include <Arduino.h>
#include <LiquidCrystal.h>

// Pin assignments
const int ledPina = 8;
const int ledPinb = 9;
const int buzzerPin = 10;
const int trigPin   = 12;
const int echoPin   = 11;
const int btnPin    = 2;

// LCD wiring (check yours!)
LiquidCrystal lcd(7, 6, 5, 4, 3, A0);

// Ultrasonic settings
#define MAX_DISTANCE 200
unsigned long timeOut = MAX_DISTANCE * 58UL;

// Timers
unsigned long previousMillis = 0;
const long blinkInterval = 250;
bool ledState = false;

// Track display mode
enum Mode { SAFE, ALERT_OBJECT, ALERT_BUTTON };
Mode currentMode = SAFE;

void setup() {
  pinMode(ledPina, OUTPUT);
  pinMode(ledPinb, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(btnPin, INPUT_PULLUP); // button → GND when pressed

  Serial.begin(9600);

  // LCD init
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("System Ready");
  delay(1000);
  lcd.clear();

  // Backlight wiring: pin15 (A) → 5V, pin16 (K) → GND
}

void loop() {
  int buttonState = digitalRead(btnPin);
  float distance = getSonar();
  Serial.println(distance);

  unsigned long currentMillis = millis();
  bool objectClose = (distance > 0 && distance < 15);

  // --- Button pressed ---
  if (buttonState == LOW) {
    // Update LCD only if mode changed
    if (currentMode != ALERT_BUTTON) {
      currentMode = ALERT_BUTTON;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ALERT !!!");
      lcd.setCursor(0, 1);
      lcd.print("Button pressed");
    }

    // Solid LEDs + continuous buzzer
    digitalWrite(ledPina, HIGH);
    digitalWrite(ledPinb, HIGH);
    tone(buzzerPin, 500);
  }

  // --- Object close ---
  else if (objectClose) {
    if (currentMode != ALERT_OBJECT) {
      currentMode = ALERT_OBJECT;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("!!! ALERT !!!");
      lcd.setCursor(0, 1);
      lcd.print("Object close");
    }

    // Blink LEDs and buzzer
    if (currentMillis - previousMillis >= blinkInterval) {
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(ledPina, ledState);
      digitalWrite(ledPinb, ledState);
      if (ledState) tone(buzzerPin, 1000);
      else          noTone(buzzerPin);
    }
  }

  // --- Safe (>=20 cm) ---
  else if (distance >= 20) {
    if (currentMode != SAFE) {
      currentMode = SAFE;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Distance OK");
    }

    // LEDs & buzzer off
    digitalWrite(ledPina, LOW);
    digitalWrite(ledPinb, LOW);
    noTone(buzzerPin);

    lcd.setCursor(0, 1);
    if (distance > 0)
      lcd.print(String(distance, 1) + " cm     ");
    else
      lcd.print("No signal     ");
  }

  delay(20); // small loop delay
}

// --- Ultrasonic function ---
float getSonar() {
  // Send trigger pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo
  unsigned long pingTime = pulseIn(echoPin, HIGH, 20000UL); // 20ms timeout
  if (pingTime == 0) return -1;

  float distance = pingTime * 0.0343 / 2.0;
  return distance;
}
