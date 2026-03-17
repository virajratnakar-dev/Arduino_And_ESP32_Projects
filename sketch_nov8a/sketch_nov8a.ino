/***********************************************************
File name: 05_breathingLed.ino
Description: PWM control the LED gradually from dark to 
             brighter, then from brighter to dark
Website: www.quadstore.in
***********************************************************/
int ledpin = 11; // define digital pin 11 as the pin to control the LED

void setup()
{
  pinMode(ledpin, OUTPUT); // set pin 11 as an output
}
 
void loop()
{
  for (int a = 0; a <= 255; a++)  // loop: increase LED brightness
  {
    analogWrite(ledpin, a);   // set LED brightness (0–255)
    delay(15);                // wait 15 ms at each brightness level
  }
  for (int a = 255; a >= 0; a--)  // loop: decrease LED brightness
  {
    analogWrite(ledpin, a);   // set LED brightness (255–0)
    delay(15);                // wait 15 ms at each brightness level
  }
  delay(100);                 // wait 100 ms before restarting
}

