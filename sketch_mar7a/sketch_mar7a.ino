
void setup() {
pinMode(13, OUTPUT);
pinMode(12, OUTPUT);
pinMode(5, OUTPUT);
}

void loop() {
analogWrite(5, 20);
digitalWrite(12, HIGH);
digitalWrite(13, LOW);
delay(3000);
digitalWrite(12, LOW);
digitalWrite(13, HIGH);
delay(3000);
}
