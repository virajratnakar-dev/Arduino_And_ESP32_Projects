int in1 = 13;
int in2 = 12;
int ena = 5;
void setup() {
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(ena, OUTPUT);
}

void loop() {
analogWrite(ena, 100);
digitalWrite(in2, HIGH);
digitalWrite(in1, LOW);
delay(3000);
digitalWrite(in2, LOW);
digitalWrite(in1, HIGH);
delay(3000);
}
