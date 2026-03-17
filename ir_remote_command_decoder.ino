#include <IRremote.hpp>

const int RECV_PIN = 11;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (IrReceiver.decode()) {
    Serial.print("Command value: ");
    Serial.print(IrReceiver.decodedIRData.command);
    Serial.print("\t | Raw data: 0x");
    Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);

    IrReceiver.resume(); // Ready for next signal
  }
}
