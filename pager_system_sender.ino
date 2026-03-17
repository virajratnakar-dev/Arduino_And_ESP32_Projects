#include <WiFiS3.h>

const char* ssid = "VirajPager";
const char* password = "12345678";
const char* serverIP = "192.168.4.1";
int port = 80;

const int btnCall = 12;

WiFiClient client;

int pressCount = 0;
unsigned long firstPressTime = 0;
const unsigned long pressWindow = 800;  // 800ms window

void setup() {
  Serial.begin(115200);
  pinMode(btnCall, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void sendMessage(String msg) {
  if (client.connect(serverIP, port)) {
    client.print("GET /?msg=");
    client.print(msg);
    client.println(" HTTP/1.1");
    client.print("Host: ");
    client.println(serverIP);
    client.println("Connection: close");
    client.println();
    client.stop();
  }
}

void loop() {

  if (digitalRead(btnCall) == LOW) {
    delay(50);  // debounce
    if (digitalRead(btnCall) == LOW) {

      if (pressCount == 0) {
        firstPressTime = millis();
      }

      pressCount++;

      while (digitalRead(btnCall) == LOW);  // wait release
      delay(50);
    }
  }

  if (pressCount > 0 && millis() - firstPressTime > pressWindow) {

    if (pressCount == 1) {
      sendMessage("CALL");
    } 
    else if (pressCount == 2) {
      sendMessage("FOOD");
    } 
    else if (pressCount >= 3) {
      sendMessage("EMERGENCY");
    }

    pressCount = 0;
  }
}
