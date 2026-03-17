#include <IRremote.h>
#include <Servo.h>

const int IR_PIN = 11;
Servo myservo;      

int command = 0;

void setup() {
  Serial.begin(9600);

  IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);

  myservo.attach(9);
  myservo.write(90);   // start at center
}

void loop() {

  if (IrReceiver.decode()) {

    command = IrReceiver.decodedIRData.command;
    Serial.println(command);

    switch (command) {

      case 21:        // button code 21
        myservo.write(180);
        delay(500);
        myservo.write(90);
        break;

      case 7:        // button code 12
        myservo.write(0);
        delay(500);
        myservo.write(90);
        break;

    }

    IrReceiver.resume(); // receive next signal
  }
}