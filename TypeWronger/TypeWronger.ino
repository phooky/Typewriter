char buffer[20];
int offset;
int pulselen;

unsigned int keyTable[128] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
3,0,267,266,265,264,301,303,305,307,309,0,36,53,38,40,
51,39,11,10,9,8,43,45,47,49,297,41,0,0,0,292,
0,279,286,282,283,260,285,287,289,302,291,293,295,290,288,308,
306,262,261,281,300,304,284,263,280,298,278,0,0,0,0,299,
0,23,30,26,27,4,29,31,33,46,35,37,39,34,32,52,
50,6,5,25,44,48,28,7,24,42,22,0,296,0,0,0,
};

#define CR_MOTOR 12
#define CR_SWITCH 14
#define CR_BRAKE 15
#define SHIFT_PIN 2

#define CR_PWM 120
#define CR_POST_TRIGGER 80

unsigned long cr_pwm;
unsigned long cr_post_trigger;

boolean isSwitchOpen() {
  return digitalRead(CR_SWITCH) == HIGH;
}

void shift(boolean on) {
  if (on) {
    digitalWrite(SHIFT_PIN,HIGH);
    delay(100);
    analogWrite(SHIFT_PIN,120);
  } else {
    digitalWrite(SHIFT_PIN,LOW);
    delay(100);
  }
}

void setCrSpeed(unsigned char value) {
  if (value == 0) {
    digitalWrite(CR_MOTOR,LOW);
    delay(20);
    digitalWrite(CR_BRAKE,HIGH);
    delay(300);
    digitalWrite(CR_BRAKE,LOW);
    delay(20);
  }
  else
  {
    digitalWrite(CR_BRAKE,LOW);
    delay(20);
    analogWrite(CR_MOTOR,value);
  }
}

void setup() 
{ 
  cr_pwm=CR_PWM;
  cr_post_trigger=CR_POST_TRIGGER;

  Serial.begin(19200); 
  Serial.println("TYPEWRONGER ready to wrong writes."); 
  offset = 0;
  pulselen = 50;
  for (int i = 2; i <= 53; i++) {
    digitalWrite(i,LOW);
    pinMode(i,OUTPUT);
  }
  digitalWrite(CR_BRAKE,LOW);
  digitalWrite(CR_MOTOR,LOW);
  digitalWrite(CR_SWITCH,HIGH);
  digitalWrite(SHIFT_PIN,LOW);
  pinMode(CR_BRAKE,OUTPUT);
  pinMode(CR_MOTOR,OUTPUT);
  pinMode(CR_SWITCH,INPUT);
  pinMode(SHIFT_PIN,OUTPUT);
}

void carriageReturn() {
  setCrSpeed(cr_pwm);
  { 
    unsigned long startMs = millis();
    while (isSwitchOpen()) {
      if (millis() - startMs > 3000) {
        Serial.println("ERR: carriage return fail");
        break;
      }
    }
    delay(10); // debounce
    startMs = millis();
    while (!isSwitchOpen()) {
      if (millis() - startMs > 3000) {
        Serial.println("ERR: carriage return fail");
        break;
      }
    }
  }
  delay(cr_post_trigger);
  setCrSpeed(0);
  delay(80);
}

void pulse(unsigned int value) {
  if (value > 53 || value < 2) {
    Serial.println("ERR: Pin out of range.");
    return;
  }
  digitalWrite(value,HIGH);
  delay(pulselen);
  digitalWrite(value,LOW);
}

void loop() 
{
  while (Serial.available() <= 0) delay(10);
  int b = Serial.read();
  if (b == '\n' || b=='\r') {
    carriageReturn();
  } else if (b >=0 && b < 128) {
    unsigned int code = keyTable[b];
    if (code != 0) {
      if ((code & 0x100) != 0) {
        shift(true);
        delay(100);
        pulse(code & 0xff);
        delay(100);
        shift(false);
      } else {
        pulse(code);
      }
      delay(200);
    }
  }
  if (Serial.available() <= 0) {
    Serial.println("OK: Buffer clear");
  }
}
