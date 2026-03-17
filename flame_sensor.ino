int Flame = A0; // Connect to A0
int Buzzer = 8;

void setup() {
  pinMode(Flame, INPUT);
  pinMode(Buzzer, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  int Flame_state = analogRead(Flame);
  Serial.println(Flame_state);]

  if (Flame_state < 1000) { 
    tone(Buzzer, 1000);
  } else {
    noTone(Buzzer);
  }
  delay(100);
}
