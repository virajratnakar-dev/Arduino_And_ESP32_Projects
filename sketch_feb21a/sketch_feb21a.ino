int ledPin = 4;
int ldrPin = 5;
int lightlevel = 0;
void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  lightlevel = digitalRead(ldrPin);
  Serial.print(lightlevel);
  if (lightlevel == HIGH) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}
