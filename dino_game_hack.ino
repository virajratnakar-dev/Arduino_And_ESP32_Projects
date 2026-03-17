#include <Servo.h>

Servo myServo; 
int ldrPin = 6;
int anPin = 5;

void setup() {
  Serial.begin(9600);
  myServo.attach(9); 
  //pinMode(ldrPin, INPUT);
  pinMode(anPin, INPUT);
  myServo.write(0); // Set initial position
}

void loop() {
  // Corrected the syntax error and extra argument here
  //digread = digitalRead(ldrPin);
  int anread = analogRead(anPin);
  Serial.println(anread);
  if (anread > 800) { 
    for (int pos = 0; pos <= 50; pos += 4) {
      myServo.write(pos);
      delay(15);
    } 
    for (int pos = 50; pos >= 0; pos -= 4) {
      myServo.write(pos);
      delay(15);
    }
  }
}
                               
