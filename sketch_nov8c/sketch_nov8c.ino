

#include <Servo.h>

Servo myservo;         // create servo object to control a servo
const int b = 4;       // pin 4 used as output signal (e.g., LED, relay)

void setup()
{
  myservo.attach(9);   // attaches the servo on pin 9
  pinMode(b, OUTPUT);  // set pin 4 as output
  myservo.write(0);    // set initial position to 0 degrees
  delay(1000);         // wait for a second
}

void loop()
{
    myservo.write(0);
  delay(60000);
  // Move servo in 20-degree increments with 60s delay
  myservo.write(36);
  delay(60000);
  
  myservo.write(72);
  delay(60000);
  
  myservo.write(108);
  delay(60000);
  
  myservo.write(144);
  delay(60000);
  
  myservo.write(180);
  // Activate digital pin b (e.g., LED on)
  tone(b, 100);
  delay(10000);
noTone(b);
  // Return servo to 0 degrees
}
