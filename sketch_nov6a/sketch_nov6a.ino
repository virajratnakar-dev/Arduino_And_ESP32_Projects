#include <IRremote.hpp>
#include <AccelStepper.h>

// ----- Configuration -----
const int stepsPerRevolution = 512;
AccelStepper myStepper(AccelStepper::FULL4WIRE, 8, 10, 9, 11);

const int RECV_PIN = 5;
const int yay_PIN  = 3;   // buzzer / indicator
const int btn_PIN  = 2;   // physical button

// ----- State Variables -----
bool childLocked   = true;
bool isPaused      = true;
bool moving        = false;
long targetSteps   = 0;
int  codeBuffer[4] = {0, 0, 0, 0};
float myStepperSpeed = 60.0;  // starting RPM

// ----- Speed thresholds -----
const float SPEED_FAST = 4.0;     // ~1 minute/rev
const float SPEED_SLOW = 0.067;   // ~1 hour/rev
const float SPEED_MAX  = 60.0;    // upper limit

// ----- Helpers -----
float rpmToSpeed(float rpm) {
  return (rpm * stepsPerRevolution) / 60.0;  // steps per second
}

// ----- Setup -----
void setup() {
  pinMode(btn_PIN, INPUT_PULLUP);
  pinMode(yay_PIN, OUTPUT);
  digitalWrite(yay_PIN, LOW);

  myStepper.setMaxSpeed(rpmToSpeed(myStepperSpeed));
  myStepper.setAcceleration(rpmToSpeed(30)); // gentle acceleration

  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
  randomSeed(analogRead(0));

  Serial.println("Stepper + IRremote + Non-blocking + Child Lock Ready (RELATIVE)");
}

// ----- Unlock sequence -----
void checkUnlockCode(int command) {
  codeBuffer[0] = codeBuffer[1];
  codeBuffer[1] = codeBuffer[2];
  codeBuffer[2] = codeBuffer[3];
  codeBuffer[3] = command;

  if (codeBuffer[0] == 12 && codeBuffer[1] == 12 &&
      codeBuffer[2] == 94 && codeBuffer[3] == 22) {
    childLocked = false;
    Serial.println("🔓 Child Lock Released");
    digitalWrite(yay_PIN, HIGH);
    delay(300);
    digitalWrite(yay_PIN, LOW);
  }
}

// ----- Main Loop -----
void loop() {
  // Run stepper continuously (non-blocking)
  if (moving && !childLocked && !isPaused) {
    myStepper.run();

    if (myStepper.distanceToGo() == 0) {
      moving = false;
      isPaused = false;
      Serial.println("✅ Move complete");

      // Continuous tone for extreme speeds
      if (myStepperSpeed == 0.067 || myStepperSpeed == 4) {
        digitalWrite(yay_PIN, HIGH);   // keep buzzer ON
      } else {
        // short beep for all other speeds
        digitalWrite(yay_PIN, HIGH);
        delay(100);
        digitalWrite(yay_PIN, LOW);
      }
    }
  }

  // Button stop
  if (digitalRead(btn_PIN) == LOW) {
    digitalWrite(yay_PIN, LOW);
  }

  // ----- Handle IR input -----
  if (IrReceiver.decode()) {
    uint8_t command = IrReceiver.decodedIRData.command;
    Serial.print("Command value: ");
    Serial.println(command);

    checkUnlockCode(command);

    if (childLocked) {
      Serial.println("🚫 Command ignored (child lock active)");
      IrReceiver.resume();
      return;
    }

    long stepsToMove = 0;

    switch (command) {
      // Movement cases
      case 21: stepsToMove =  stepsPerRevolution * 2;  break;
      case 7:  stepsToMove = -stepsPerRevolution * 2;  break;
      case 12: stepsToMove =  stepsPerRevolution * 4;  break;
      case 24: stepsToMove =  stepsPerRevolution * 8;  break;
      case 94: stepsToMove =  stepsPerRevolution * 12; break;
      case 8:  stepsToMove =  stepsPerRevolution * 16; break;
      case 28: stepsToMove =  stepsPerRevolution * 20; break;
      case 90: stepsToMove =  stepsPerRevolution * 24; break;
      case 66: stepsToMove =  stepsPerRevolution * 28; break;
      case 82: stepsToMove =  stepsPerRevolution * 32; break;
      case 74: stepsToMove =  stepsPerRevolution * 36; break;
      case 25: stepsToMove = 100; break;
      case 13: stepsToMove = 200; break;
      case 9:  stepsToMove = random(1, stepsPerRevolution * 4); break;
      case 69: stepsToMove = -10; break;
      case 70: stepsToMove = 10;  break;

      // Buzzer test
      case 22:
        digitalWrite(yay_PIN, HIGH);
        delay(100);
        digitalWrite(yay_PIN, LOW);
        break;

      // Lock
      case 71:
        childLocked = true;
        isPaused = true;
        moving = false;
        myStepper.stop();
        Serial.println("🔒 Child lock engaged");
        digitalWrite(yay_PIN, HIGH);
        delay(200);
        digitalWrite(yay_PIN, LOW);
        break;

      // Play / Pause toggle
      case 67:
        if (moving && !isPaused) {
          isPaused = true;
          myStepper.stop();
          Serial.println("⏸️ Paused movement");
        } else if (moving && isPaused) {
          isPaused = false;
          myStepper.setSpeed(rpmToSpeed(myStepperSpeed));
          Serial.println("▶️ Resuming movement");
        } else {
          Serial.println("⏹️ No active motion to resume");
        }
        digitalWrite(yay_PIN, HIGH);
        delay(100);
        digitalWrite(yay_PIN, LOW);
        break;

      // Volume + (speed up)
      case 64:
        if (myStepperSpeed <= SPEED_SLOW) {
          myStepperSpeed = SPEED_FAST;
        } else {
          myStepperSpeed += 1.0;
          if (myStepperSpeed > SPEED_MAX) myStepperSpeed = SPEED_MAX;
        }
        myStepper.setMaxSpeed(rpmToSpeed(myStepperSpeed));
        Serial.print("🔊 Speed up -> ");
        Serial.println(myStepperSpeed, 3);
        digitalWrite(yay_PIN, HIGH);
        delay(50);
        digitalWrite(yay_PIN, LOW);
        break;

      // Volume - (slow down)
      case 68:
        myStepperSpeed -= 1.0;
        if (myStepperSpeed < SPEED_FAST)
          myStepperSpeed = SPEED_SLOW;
        myStepper.setMaxSpeed(rpmToSpeed(myStepperSpeed));
        Serial.print("🔉 Slow down -> ");
        Serial.println(myStepperSpeed, 3);
        digitalWrite(yay_PIN, HIGH);
        delay(50);
        digitalWrite(yay_PIN, LOW);
        break;

      default:
        Serial.println("Unknown or unassigned command");
        digitalWrite(yay_PIN, HIGH);
        delay(100);
        digitalWrite(yay_PIN, LOW);
        break;
    }
    // ----- Start new movement -----
    if (stepsToMove != 0) {
      myStepper.setMaxSpeed(rpmToSpeed(myStepperSpeed));
      myStepper.move(stepsToMove); // relative move
      moving = true;
      isPaused = false;

      Serial.print("🏁 New move started, steps: ");
      Serial.println(stepsToMove);
      digitalWrite(yay_PIN, HIGH);
      delay(100);
      digitalWrite(yay_PIN, LOW);
    }

    IrReceiver.resume();
  }
}
