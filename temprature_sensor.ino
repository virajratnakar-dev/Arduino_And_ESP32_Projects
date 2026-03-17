
const uint8_t tempPin = A0;
const uint8_t samples = 8;

long readVcc_mV() {
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); 
  ADCSRA |= _BV(ADSC); 
  while (ADCSRA & _BV(ADSC)); 
  uint16_t result = ADC;
  long vcc = (1125300L) / result; 
  return vcc;
}

void setup() {
  Serial.begin(9600);
  delay(200);
  Serial.println(F("LM35 improved read"));
}

void loop() {
  uint32_t acc = 0;
  for (uint8_t i = 0; i < samples; ++i) {
    acc += analogRead(tempPin);
    delay(5);
  }
  float adc = (float)acc / samples; 
  long vcc_mV = readVcc_mV(); 
  float vcc = vcc_mV / 1000.0; 

  float voltage = (adc / 1023.0) * vcc; 

  float tempC = voltage * 100.0;

  Serial.print(F("ADC(avg)="));
  Serial.print((int)adc);
  Serial.print(F("  Vcc(mV)="));
  Serial.print(vcc_mV);
  Serial.print(F("  Vout="));
  Serial.print(voltage, 3);
  Serial.print(F(" V  TEMP="));
  Serial.print(tempC, 2);
  Serial.println(F(" °C"));

  delay(1000);
}
