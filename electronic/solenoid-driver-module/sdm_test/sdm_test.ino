

#define CLK 4
#define OE 5
#define DIN 6
#define REGCLK 7

#define BUFSZ 100

char buf[BUFSZ];
char bufidx;

unsigned int readInt(char* b) {
  unsigned int i = 0;
  while (*b != 0)  {
    i = i * 10;
    i = i + (*b - '0');
    b++;
  }
  return i;
}

void setup() {
  digitalWrite(CLK, LOW);
  digitalWrite(OE, HIGH);
  digitalWrite(DIN, LOW);
  digitalWrite(REGCLK, LOW);
  pinMode(CLK, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(DIN, OUTPUT);
  pinMode(REGCLK, OUTPUT);
  Serial.begin(19200);
  Serial.println("*** SOLENOID BOARD TEST ***");
  Serial.println("R[NN] to load number NN into shift registers.");
  Serial.println("ON to turn on outputs.");
  Serial.println("OFF to turn off outputs.");
  Serial.println("T[NN] to test solenoid NN for 1 second.");
  Serial.println("Ready to test.");
  bufidx = 0;
}

void clockPin(unsigned int pin) {
  delay(1);
  digitalWrite(pin, HIGH);
  delay(1);
  digitalWrite(pin, LOW);
}

void enable() {
 digitalWrite(OE, LOW);
}

void disable() {
  digitalWrite(OE, HIGH);
}

void loadNumber(unsigned int number) {
  unsigned int mask = 1 << 15;
  for (int i = 0; i < 16; i++) {
    digitalWrite(DIN,((number & mask)==0)?LOW:HIGH);
    clockPin(CLK);
    mask = mask >> 1;
  }
  clockPin(REGCLK);
}

void loop() {
  while (Serial.available() <= 0) delay(10);
  int b = Serial.read();
  if (b == '\n' || b=='\r') {
    buf[bufidx] = 0;
    if (strcmp("ON",buf) == 0) {
      Serial.println("Enabling outputs");
      enable();
    } else if (strcmp("OFF",buf) == 0) {
      Serial.println("Disabling outputs");
    } else if (buf[0] == 'T') {
      unsigned int val = readInt(buf + 1);
      Serial.print("Turning on number ");
      Serial.print(val,DEC);
      Serial.println("");
      loadNumber(1 << val);
      enable();
      delay(1000);
      disable();
    } else if (buf[0] == 'R') {
      unsigned int val = readInt(buf + 1);
      Serial.print("Loading number ");
      Serial.print(val,DEC);
      Serial.println("");
      loadNumber(val);
    } else if (bufidx != 0) {
      Serial.println("Unrecognized command.");
    }
    bufidx = 0;
  } else if (b >=0 && b < 128) {
    buf[bufidx++] = b;
  }
}
