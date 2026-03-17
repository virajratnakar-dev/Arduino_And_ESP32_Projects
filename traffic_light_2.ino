// RGB module pins (no resistors needed)
// to make this make curcuit of lesson 7 and use rgb led r is pin 2 and green pin 3
const int redLED = 2;
const int greenLED = 3;

// Declare current LED color: true = red, false = green
bool ledRed = true;

// 7-segment functions (from your PDF)
int a=7, b=6, c=4, d=11, e=10, f=8, g=9, dp=5;

void digital_0(){ digitalWrite(a,HIGH); digitalWrite(b,HIGH); digitalWrite(c,HIGH); digitalWrite(d,HIGH); digitalWrite(e,HIGH); digitalWrite(f,HIGH); digitalWrite(g,LOW); digitalWrite(dp,LOW);}
void digital_1(){ digitalWrite(a,LOW); digitalWrite(b,HIGH); digitalWrite(c,HIGH); digitalWrite(d,LOW); digitalWrite(e,LOW); digitalWrite(f,LOW); digitalWrite(g,LOW); digitalWrite(dp,LOW);}
void digital_2(){ digitalWrite(a,HIGH); digitalWrite(b,HIGH); digitalWrite(c,LOW); digitalWrite(d,HIGH); digitalWrite(e,HIGH); digitalWrite(f,LOW); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}
void digital_3(){ digitalWrite(a,HIGH); digitalWrite(b,HIGH); digitalWrite(c,HIGH); digitalWrite(d,HIGH); digitalWrite(e,LOW); digitalWrite(f,LOW); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}
void digital_4(){ digitalWrite(a,LOW); digitalWrite(b,HIGH); digitalWrite(c,HIGH); digitalWrite(d,LOW); digitalWrite(e,LOW); digitalWrite(f,HIGH); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}
void digital_5(){ digitalWrite(a,HIGH); digitalWrite(b,LOW); digitalWrite(c,HIGH); digitalWrite(d,HIGH); digitalWrite(e,LOW); digitalWrite(f,HIGH); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}
void digital_6(){ digitalWrite(a,HIGH); digitalWrite(b,LOW);  digitalWrite(c,HIGH); digitalWrite(d,HIGH); digitalWrite(e,HIGH); digitalWrite(f,HIGH); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}
void digital_7(){ digitalWrite(a,HIGH); digitalWrite(b,HIGH);  digitalWrite(c,HIGH);  digitalWrite(d,LOW); digitalWrite(e,LOW); digitalWrite(f,LOW); digitalWrite(g,LOW); digitalWrite(dp,LOW);}
void digital_8(){ digitalWrite(a,HIGH); digitalWrite(b,HIGH); digitalWrite(c,HIGH); digitalWrite(d,HIGH); digitalWrite(e,HIGH); digitalWrite(f,HIGH); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}
void digital_9(){ digitalWrite(a,HIGH); digitalWrite(b,HIGH); digitalWrite(c,HIGH); digitalWrite(d,HIGH); digitalWrite(e,LOW); digitalWrite(f,HIGH); digitalWrite(g,HIGH); digitalWrite(dp,LOW);}

void setup() {
  // 7-segment pins as output
  int pins[] = {a,b,c,d,e,f,g,dp};
  for(int i=0;i<8;i++){
    pinMode(pins[i], OUTPUT);
  }

  // RGB module pins
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  // Start with red LED
  digitalWrite(redLED, HIGH);
  digitalWrite(greenLED, LOW);
}

void loop() {
  // Countdown 9 → 1
  for(int i=9; i>=1; i--){
    switch(i){
      case 1: digital_1(); break;
      case 2: digital_2(); break;
      case 3: digital_3(); break;
      case 4: digital_4(); break;
      case 5: digital_5(); break;
      case 6: digital_6(); break;
      case 7: digital_7(); break;
      case 8: digital_8(); break;
      case 9: digital_9(); break;
    }

    delay(1000); // 1 second per number

    // When reaching 1, toggle LED for next countdown
    if(i==1){
      if(ledRed){
        digitalWrite(redLED, LOW);
        digitalWrite(greenLED, HIGH);
        ledRed = false;
      } else {
        digitalWrite(greenLED, LOW);
        digitalWrite(redLED, HIGH);
        ledRed = true;
      }
    }
  }
}
