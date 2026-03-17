const int btnLeft = 2;
const int btnRight = 3;

void setup() {
  pinMode(btnLeft, INPUT_PULLUP);
  pinMode(btnRight, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int leftState = digitalRead(btnLeft);
  int rightState = digitalRead(btnRight);

  Serial.print(leftState);  // 0 = pressed, 1 = released
  Serial.print(",");
  Serial.println(rightState);

  delay(50);  // Send every 50ms
}
