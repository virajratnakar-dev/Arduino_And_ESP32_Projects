#include <ArduinoBLE.h>
#include <IRremote.h>
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Built-in character frames from the R4 firmware
byte frame3[8][12] = {
 {0,1,1,1,1,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,0,0,0},
 {0,0,1,1,1,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,0,0,0},
 {0,1,1,1,1,1,1,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0}
};

byte frame2[8][12] = {
 {0,1,1,1,1,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,0,0,0},
 {0,0,0,0,0,1,1,0,0,0,0,0},
 {0,1,1,1,1,1,1,0,0,0,0,0},
 {0,1,1,0,0,0,0,0,0,0,0,0},
 {0,1,1,0,0,0,0,0,0,0,0,0},
 {0,1,1,1,1,1,1,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0}
};

byte frame1[8][12] = {
 {0,0,0,1,1,0,0,0,0,0,0,0},
 {0,0,1,1,1,0,0,0,0,0,0,0},
 {0,0,0,1,1,0,0,0,0,0,0,0},
 {0,0,0,1,1,0,0,0,0,0,0,0},
 {0,0,0,1,1,0,0,0,0,0,0,0},
 {0,0,0,1,1,0,0,0,0,0,0,0},
 {0,1,1,1,1,1,1,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0,0,0,0}
};

byte frameGO[8][12] = {
 {1,1,1,1,1,1,  0,1,1,1,1,0},
 {1,0,0,0,0,0,  0,1,0,0,1,0},
 {1,0,1,1,1,0,  0,1,0,0,1,0},
 {1,0,1,1,1,0,  0,1,0,0,1,0},
 {1,0,0,0,1,0,  0,1,0,0,1,0},
 {1,0,0,0,1,0,  0,1,0,0,1,0},
 {1,1,1,1,1,0,  0,1,1,1,1,0},
 {0,0,0,0,0,0,  0,0,0,0,0,0}
};

// ===== PINS =====
#define IR_PIN 2
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6

const int trigPin = 12;
const int echoPin = 11;

// ===== BLE =====
// ===== BLE =====
BLEService carService("19b10000-e8f2-537e-4f6c-d104768a1214");

BLEStringCharacteristic cmdChar(
  "19b10001-e8f2-537e-4f6c-d104768a1214",
  BLERead | BLEWrite,
  8
);



// ===== SYSTEM =====
int driveMode = 1;   // 1 = remote, 2 = self

unsigned long lastSignalTime = 0;
const unsigned long timeout = 200;

#define MAX_DISTANCE 200
unsigned long timeOut = MAX_DISTANCE * 58UL;

void setup() {
  matrix.begin();
  Serial.begin(9600);
  matrix.renderBitmap(frame3, 8, 12);
  delay(1000);

  matrix.renderBitmap(frame2, 8, 12);
  delay(1000);

  matrix.renderBitmap(frame1, 8, 12);
  delay(1000);

  matrix.renderBitmap(frameGO, 8, 12);
  delay(1500);

  matrix.clear();
  delay(800);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  stopMotors();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  // ---- BLUETOOTH ----
  BLE.begin();
  BLE.setLocalName("RobotCar");

  BLE.setAdvertisedService(carService);
  carService.addCharacteristic(cmdChar);
  BLE.addService(carService);
  BLE.advertise();

  Serial.println("Bluetooth Ready!");
}

void loop() {

  float distance = getSonar();
  bool blocked = (distance > 0 && distance < 15);

  // ===== BLUETOOTH =====
  BLEDevice central = BLE.central();

  if (central && cmdChar.written()) {

    String c = cmdChar.value();

    if (c == "M1") driveMode = 1;
    if (c == "M2") driveMode = 2;

    if (driveMode == 1 && c.length() == 1) {
      handleCommand(c[0], blocked);
    }
  }

  // ===== IR ALWAYS WORKS =====
  if (IrReceiver.decode()) {
    int command = IrReceiver.decodedIRData.command;
    lastSignalTime = millis();

    handleIR(command, blocked);
    IrReceiver.resume();
  }

  // ===== SELF DRIVING =====
  if (driveMode == 2) {

    if (blocked) {

      stopMotors();
      delay(300);

      // ~160 degree turn
      spinRight();
      delay(620);

      stopMotors();
      delay(200);
    }
    else {
      forward();
    }
  }

  if (millis() - lastSignalTime > timeout) {
    if (driveMode == 1) stopMotors();
  }
}

// ===== COMMANDS =====

void handleCommand(char c, bool blocked) {

  lastSignalTime = millis();

  switch(c) {

    case 'F': if(!blocked) forward(); break;
    case 'B': backward(); break;

    case 'L': if(!blocked) leftOnly(); break;
    case 'R': if(!blocked) rightOnly(); break;

    case 'Q': if(!blocked) forwardLeft(); break;
    case 'E': if(!blocked) forwardRight(); break;

    case 'Z': backwardLeft(); break;
    case 'X': backwardRight(); break;

    case 'S': stopMotors(); break;
  }
}

void handleIR(int command, bool blocked) {

  switch (command) {

    case 7: backward(); break;

    case 21: if(!blocked) forward(); break;

    case 64: if(!blocked) rightOnly(); break;
    case 68: if(!blocked) leftOnly(); break;

    case 24: spinLeft(); break;
    case 12: spinRight(); break;

    case 69: if(!blocked) forwardRight(); break;
    case 71: if(!blocked) forwardLeft(); break;

    case 25: backwardRight(); break;
    case 13: backwardLeft(); break;
  }
}

// ===== MOTORS =====

void forward(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
}

void backward(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
}

void rightOnly(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,LOW);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
}

void leftOnly(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,LOW);
}

void spinLeft(){
  digitalWrite(IN1,LOW); digitalWrite(IN2,HIGH);
  digitalWrite(IN3,HIGH); digitalWrite(IN4,LOW);
}

void spinRight(){
  digitalWrite(IN1,HIGH); digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW); digitalWrite(IN4,HIGH);
}

// ===== DIAGONALS =====

void forwardRight(){ forward(); delay(40); stopMotors(); }
void forwardLeft(){ forward(); delay(40); stopMotors(); }

void backwardRight(){ backward(); delay(40); stopMotors(); }
void backwardLeft(){ backward(); delay(40); stopMotors(); }

void stopMotors(){
  digitalWrite(IN1,LOW);
  digitalWrite(IN2,LOW);
  digitalWrite(IN3,LOW);
  digitalWrite(IN4,LOW);
}

// ===== ULTRASONIC =====

float getSonar(){

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long pingTime = pulseIn(echoPin, HIGH, timeOut);
  if(pingTime == 0) return -1;

  return pingTime * 0.0343 / 2.0;
}
