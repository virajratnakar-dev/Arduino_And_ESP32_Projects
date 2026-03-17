#include <Wire.h>
#include "RTClib.h"   // Works for both DS3231 and DS1307

#define BUZZER_PIN 3

RTC_DS3231 rtc;
bool buzzerOn = false;
int lastAlarmDay = -1;  // Remember which day the alarm rang
unsigned long buzzerStart = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC module!");
    while (1);
  }

  // Uncomment this once to sync with computer time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  Serial.println("System ready. Waiting for alarm...");
}

void loop() {
  DateTime now = rtc.now();

  // Print time for debugging
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.println(now.second());

  // 🔔 Alarm Time (set your own)
  int alarmHour = 7;     // 7 AM (24-hour format)
  int alarmMinute = 0;   // 0 minutes past

  // If it's alarm time and it hasn't rung today
  if (now.hour() == alarmHour && now.minute() == alarmMinute && now.day() != lastAlarmDay) {
    buzzerOn = true;
    buzzerStart = millis();
    lastAlarmDay = now.day();  // Remember today so it doesn’t repeat
    Serial.println("⏰ Alarm ON!");
    tone(BUZZER_PIN, 1000);  // 1 kHz tone for passive buzzer
    delay(30000);
    noTone(BUZZER_PIN);
    buzzerOn = false;
  }

  // Auto-stop buzzer after 10 seconds

  delay(500);  // Check twice a second
}
