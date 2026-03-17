#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <time.h>

const char* routerSSID = "jackeis";
const char* routerPASS = "Jackjack333.";


const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 19800;   // India
const int daylightOffset_sec = 0;
bool colonState = true;
bool alarmActive = false;
bool alarmTriggeredToday = false;
unsigned long alarmStart = 0;
unsigned long lastClockUpdate = 0;
char timeString[17] = "--:--:--";

// ---------------- LCD ----------------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- WiFi Hotspot ----------------

const char* ssid = "VirajPager";
const char* password = "12345678";

WiFiServer server(80);

// ---------------- Hardware ----------------
const int buzzer = 18;

// ---------------- State Variables ----------------
String receivedMessage = "";
String messageType = "";
unsigned long callTime = 0;
bool callActive = false;
int callCount = 0;
 struct tm timeinfo;


// ---------------- Clock Variables ----------------

// ---------------- Sound Functions ----------------
void soundCall() {
  tone(buzzer, 2000); delay(120); noTone(buzzer); delay(120);
  tone(buzzer, 2000); delay(120); noTone(buzzer); delay(150);
  tone(buzzer, 2000); delay(120); noTone(buzzer); delay(120);
  tone(buzzer, 2000); delay(120); noTone(buzzer);
}

void soundFood() {
  tone(buzzer, 1200); delay(300); noTone(buzzer); delay(200);
  tone(buzzer, 1200); delay(300); noTone(buzzer); delay(300);
  tone(buzzer, 1200); delay(300); noTone(buzzer); delay(200);
  tone(buzzer, 1200); delay(300); noTone(buzzer);
}

void soundEmergency() {
  for (int i = 0; i < 6; i++) {
    tone(buzzer, 2800); delay(100); noTone(buzzer); delay(80);
  }
}

// ---------------- Setup ----------------
void setup() {

  Serial.begin(115200);

  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);

  lcd.init();
  lcd.backlight();

WiFi.mode(WIFI_AP_STA);

WiFi.begin(routerSSID, routerPASS);

lcd.setCursor(0,0);
lcd.print("Connecting WiFi");

while (WiFi.status() != WL_CONNECTED) {
  delay(500);
}

lcd.clear();
lcd.print("WiFi Connected");
delay(1000);

WiFi.softAP(ssid, password);

configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.print("Hotspot IP: ");
  Serial.println(WiFi.softAPIP());

  lcd.setCursor(0,0);
  lcd.print("Hotspot Ready ");
  delay(1000);

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Viraj Pager v2.0");
  delay(1500);

  lcd.clear();

  server.begin();
}

// ---------------- Main Loop ----------------
void loop() {

  // -------- CLOCK UPDATE (once per second) --------
  if (millis() - lastClockUpdate > 1000) {
    
    if (getLocalTime(&timeinfo, 0)) {
char buffer[17];

strftime(buffer, sizeof(buffer), "%I:%M:%S %p", &timeinfo);

if (!colonState) {
  buffer[2] = ' ';
  buffer[5] = ' ';
}

snprintf(timeString, sizeof(timeString), "%s", buffer);

colonState = !colonState;    }

    lastClockUpdate = millis();
  }
  int currentHour = timeinfo.tm_hour;
int currentMinute = timeinfo.tm_min;
int currentSecond = timeinfo.tm_sec;

// Trigger alarm at 6:45 AM
if (currentHour == 9 && currentMinute == 41 && !alarmTriggeredToday) {
  alarmActive = true;
  alarmTriggeredToday = true;
  alarmStart = millis();
}
if (currentHour == 0 && currentMinute == 0) {
  alarmTriggeredToday = false;
}

  // -------- WiFi Client --------
  WiFiClient client = server.available();

  if (client) {

    String request = client.readStringUntil('\r');
    client.flush();

    messageType = "";

    if (request.indexOf("/call") != -1 || request.indexOf("msg=CALL") != -1)
      messageType = "CALL";

    else if (request.indexOf("/food") != -1 || request.indexOf("msg=FOOD") != -1)
      messageType = "FOOD";

    else if (request.indexOf("/emergency") != -1 || request.indexOf("msg=EMERGENCY") != -1)
      messageType = "EMERGENCY";

    if (messageType != "") {

      callCount++;
      callActive = true;
      callTime = millis();

      if (messageType == "CALL")
        receivedMessage = "Viraj Calling!";

      if (messageType == "FOOD")
        receivedMessage = "Food Time!";

      if (messageType == "EMERGENCY")
        receivedMessage = "!!!EMERGENCY!!!";
    }

    // -------- Webpage --------
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println(R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>Viraj Pager</title>
<style>
body {
  font-family: Arial, sans-serif;
  text-align: center;
  background: #111;
  color: white;
  margin-top: 40px;
}

h1 {
  font-size: 32px;
  margin-bottom: 30px;
}

button {
  width: 80%;
  max-width: 300px;
  padding: 20px;
  margin: 15px;
  font-size: 20px;
  border: none;
  border-radius: 15px;
  cursor: pointer;
  transition: 0.2s;
}

.call {
  background: #3498db;
  color: white;
}

.food {
  background: #27ae60;
  color: white;
}

.emergency {
  background: #e74c3c;
  color: white;
}

button:hover {
  transform: scale(1.05);
  opacity: 0.9;
}

.footer {
  margin-top: 30px;
  font-size: 18px;
  opacity: 0.8;
}
</style>
</head>

<body>

<h1>Viraj Pager</h1>

<button class="call" onclick="location.href='/call'">📞 Call</button>
<br>
<button class="food" onclick="location.href='/food'">🍛 Food</button>
<br>
<button class="emergency" onclick="location.href='/emergency'">🚨 Emergency</button>

<div class="footer">
Total Calls: )rawliteral");

    client.stop();
  }
// -------- Alarm Beeping --------
if (alarmActive) {

  tone(buzzer, 1800);

  if (millis() - alarmStart > 60000) {
    alarmActive = false;
    noTone(buzzer);
  }

}
  // -------- LCD DISPLAY --------

  if (!callActive) {
    lcd.setCursor(0,0);
    lcd.print("  ");
    lcd.setCursor(2,0);
    lcd.print(timeString);
    lcd.print("        ");
    lcd.setCursor(0,1);
    lcd.print("   ");
    lcd.setCursor(3,1);
    lcd.print("Calls: ");
    lcd.print(callCount);
    lcd.print("       ");
  }

  if (callActive) {

    lcd.setCursor(0,0);
    lcd.print(receivedMessage);
    lcd.print("    ");

    lcd.setCursor(0,1);
    lcd.print("Calls: ");
    lcd.print(callCount);
    lcd.print("    ");

    if (messageType == "CALL") soundCall();
    if (messageType == "FOOD") soundFood();
    if (messageType == "EMERGENCY") soundEmergency();

    if (millis() - callTime > 3000) {
      callActive = false;
    }
  }
}