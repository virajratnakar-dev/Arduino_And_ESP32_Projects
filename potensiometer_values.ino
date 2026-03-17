/***********************************************************
File name:   20_Potentiometer_Processing.ino
Description: Arduino and Processing interactive
             The size of the potentiometer voltage controls 
             the red circle on the PC.
Website: www.quadstore.in
***********************************************************/

int potPin = 0; // define UNO board A0 pin connected to the potentiometer

void setup() { 
  // Start the serial port, baud rate 9600
  Serial.begin(9600); 
} 

void loop() { 
  // Read the potentiometer voltage (0–1023)
  int sensorValue = analogRead(potPin);   

  // Processing’s serial.read() only supports 0–255
  // So we map the 10-bit value down to 8 bits
  int Voltage = map(sensorValue, 0, 1023, 0, 255); 

  // Send value to the PC
  Serial.write(Voltage);  

  // Update every 100 ms
  delay(100); 
}

