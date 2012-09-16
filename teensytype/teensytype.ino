/*
 * This project is targeted for the Teensy 2.0
 */

/* Pin assignments
 *
 * Atmega   Arduino   Function
 * B7       4         Shift Clock
 * D0       5         Register Clock
 * D1       6         Output Enable'
 * D2       7         RX
 * D3       8         TX
 * D4       22        Data 1
 * D5       23        Data 2
 * D6       11        Data 3
 * D7       12        Data 4
 * B6       15        Motor
 * B5       14        Brake
 * B4       13        Switch 1
 * B3       3         Switch 2
 * B2       2         Switch 3
 * B1       1         Switch 4
 */

const unsigned int pin_shift_clock = 4;
const unsigned int pin_register_clock = 5;
const unsigned int pin_output_enable = 6;
const unsigned int pin_rx = 7;
const unsigned int pin_tx = 8;
const unsigned int pin_data1 = 22;
const unsigned int pin_data2 = 23;
const unsigned int pin_data3 = 11;
const unsigned int pin_data4 = 12;
const unsigned int pin_motor = 15;
const unsigned int pin_brake = 14;
const unsigned int pin_switch1 = 13;
const unsigned int pin_switch2 = 3;
const unsigned int pin_switch3 = 2;
const unsigned int pin_switch4 = 1;

inline void pinMode(int pin, int mode, int value) {
  digitalWrite(pin,value);
  pinMode(pin,mode);
  digitalWrite(pin,value);
}

void setPinDirections() {
  pinMode(pin_shift_clock, OUTPUT, LOW);
  pinMode(pin_register_clock, OUTPUT, LOW);
  pinMode(pin_output_enable, OUTPUT, HIGH);
  pinMode(pin_rx, OUTPUT, LOW);
  pinMode(pin_tx, OUTPUT, LOW);
  pinMode(pin_data1, OUTPUT, LOW);
  pinMode(pin_data2, OUTPUT, LOW);
  pinMode(pin_data3, OUTPUT, LOW);
  pinMode(pin_data4, OUTPUT, LOW);
  pinMode(pin_motor, OUTPUT, LOW);
  pinMode(pin_brake, OUTPUT, LOW);
  pinMode(pin_switch1, INPUT, HIGH);
  pinMode(pin_switch2, INPUT, HIGH);
  pinMode(pin_switch3, INPUT, HIGH);
  pinMode(pin_switch4, INPUT, HIGH);
}

HardwareSerial Uart = HardwareSerial();

typedef enum {
  MOTOR_OFF = 0,
  MOTOR_ON,
  MOTOR_BRAKE
} MotorState;

void setMotor(MotorState state = MOTOR_OFF) {
  switch (state) {
  case MOTOR_ON:
    digitalWrite(pin_brake, LOW);
    delay(10);
    digitalWrite(pin_motor, HIGH);
    break;
  case MOTOR_BRAKE:
    digitalWrite(pin_motor, LOW);
    digitalWrite(pin_brake, HIGH);
    break;
  case MOTOR_OFF:
  default:
    digitalWrite(pin_motor, LOW);
    digitalWrite(pin_brake, LOW);
  }
}

enum {
  SW_1 = 0x01,
  SW_2 = 0x02,
  SW_3 = 0x04,
  SW_4 = 0x08
};

uint8_t getSwitchState() {
  uint8_t state = 0;
  if (digitalRead(pin_switch1) == HIGH) state |= SW_1;
  if (digitalRead(pin_switch2) == HIGH) state |= SW_2;
  if (digitalRead(pin_switch3) == HIGH) state |= SW_3;
  if (digitalRead(pin_switch4) == HIGH) state |= SW_4;
  return state;
}

/*
 * Bank -> key mapping
 * This chart gives the function of each bank/solenoid pair.
 * Unused values are indicated by a blank. Shift is SH, space
 * is SP.
 *
 *             Solenoid
 *        0  1  2  3  4  5  6  7  8  9  A  B  
 *       _____________________________________
 * B  0 | 0  8  6  4  2  3  5  7  9  -
 * a  1 | p  i  y  r  w  q  e  t  u  o
 * n  2 | ;  k  h  f  s  a  d  g  j  l
 * k  3 | x  v  n  ,  /  z  c  b  m  . SP SH
 *
 */

/*
 * The character map. Entries are packed 8 bit values
 * arranged as such:
 * __________________________________________________
 * |   7     6     5     4     3     2     1     0  |
 * | shift | 0 |   b a n k  |    s o l e n o i d    |
 */

inline const uint8_t entry(
  const bool shift,
  const uint8_t bank,
  const uint8_t solenoid)
{
  return (shift?0x80:0x00) | (bank << 4) | solenoid;
}

uint8_t keymap[128];

void buildMap() {
  for (uint8_t i = 0; i < 128; i++) {
    keymap[i] = 0xFF;
  }

  keymap['0'] = entry(false,0,0);
  keymap['1'] = entry(false,2,9); // 1 is represented by 'l'
  keymap['2'] = entry(false,0,4);
  keymap['3'] = entry(false,0,5);
  keymap['4'] = entry(false,0,3);
  keymap['5'] = entry(false,0,6);
  keymap['6'] = entry(false,0,2);
  keymap['7'] = entry(false,0,7);
  keymap['8'] = entry(false,0,1);
  keymap['9'] = entry(false,0,8);

  keymap['-'] = entry(false,0,9);
  keymap[';'] = entry(false,2,0);
  keymap[','] = entry(false,3,3);
  keymap['/'] = entry(false,3,4);
  keymap['.'] = entry(false,3,9);

  keymap[' '] = entry(false,3,0xA);

  keymap['a'] = entry(false,2,5);
  keymap['b'] = entry(false,3,7);
  keymap['c'] = entry(false,3,6);
  keymap['d'] = entry(false,2,6);
  keymap['e'] = entry(false,1,6);
  keymap['f'] = entry(false,2,3);
  keymap['g'] = entry(false,2,7);
  keymap['h'] = entry(false,2,2);
  keymap['i'] = entry(false,1,1);
  keymap['j'] = entry(false,2,8);
  keymap['k'] = entry(false,2,1);
  keymap['l'] = entry(false,2,9);
  keymap['m'] = entry(false,3,8);
  keymap['n'] = entry(false,3,2);
  keymap['o'] = entry(false,1,9);
  keymap['p'] = entry(false,1,0);
  keymap['q'] = entry(false,1,5);
  keymap['r'] = entry(false,1,3);
  keymap['s'] = entry(false,2,4);
  keymap['t'] = entry(false,1,7);
  keymap['u'] = entry(false,1,8);
  keymap['v'] = entry(false,3,1);
  keymap['w'] = entry(false,1,4);
  keymap['x'] = entry(false,3,0);
  keymap['y'] = entry(false,1,2);
  keymap['z'] = entry(false,3,5);
  // Set upper case
  for (char c = 0; c < 26; c++) {
    keymap[c+'A'] = keymap[c+'a'] | 0x80;
  }

  keymap['"'] = keymap['2'] | 0x80;
  keymap['#'] = keymap['3'] | 0x80;
  keymap['$'] = keymap['4'] | 0x80;
  keymap['%'] = keymap['5'] | 0x80;
  keymap['_'] = keymap['6'] | 0x80;
  keymap['&'] = keymap['7'] | 0x80;
  keymap['\''] = keymap['8'] | 0x80;
  keymap['('] = keymap['9'] | 0x80;
  keymap[')'] = keymap['0'] | 0x80;
  keymap['*'] = keymap['-'] | 0x80;
  keymap[':'] = keymap[';'] | 0x80;
  keymap['?'] = keymap[','] | 0x80;
}

/*
 * The Time Kerning table: this is the order of the
 * keybars from left to right on the typewriter. If
 * characters are close together, they will require
 * extra time between keystrokes to avoid jamming.
 */

char* key_order = "qaz2wsx3edc4rfv5tgb6yhn7ujm8ik,9ol.0p;/-";
// Map of solenoid table entries to key position
uint8_t key_position[127];

void buildPositions() {
  for (int i = 0; i < 127; i++) {
    key_position[i] = 0;
  }
  char* key = key_order;
  int pos = 1;
  while (*key != '\0') {
    key_position[(keymap[*key] & 0x7F)] = pos;
    key++;
    pos++;
  }
}

uint16_t kernTime(uint8_t first, uint8_t second) {
  if (first == ' ' or second == ' ') return 105;
  uint8_t posA = key_position[keymap[first] & 0x7F];
  uint8_t posB = key_position[keymap[second] & 0x7F];
  //Uart.print("KT: A is ");
  //Uart.print(posA, DEC);
  //Uart.print(", B is ");
  //Uart.println(posB, DEC);
  if (posA == 0 or posB == 0) return 0;
  uint8_t diff;
  if (posA > posB) { diff = posA - posB; } else { diff = posB - posA; }
  if (diff > 16) return 0;
  //Uart.print("Diff is ");
  //Uart.println(diff, DEC);
  return (16 - diff) * 6;
}

uint16_t bank[4];

void clearBanks() {
  bank[0] = bank[1] = bank[2] = bank[3] = 0;
}

void setup() 
{ 
  clearBanks();
  setPinDirections();
  writeSolenoids();  
  buildMap();
  buildPositions();
  Serial.begin(19200); 
  Serial.println("USB connection online."); 
  Uart.begin(19200); 
  Uart.println("Serial connection online."); 
}

void setSolenoid(int b, int r) {
  bank[b] |= 0x01 << r;
}

inline void setIfBit(const uint16_t data, const uint8_t offset, const unsigned int pin) {
  digitalWrite(pin, ((data & _BV(offset)) == 0)?LOW:HIGH);
}

void enableSolenoids() {
  digitalWrite(pin_output_enable,LOW);
}

void disableSolenoids() {
  digitalWrite(pin_output_enable,HIGH);
}

void writeSolenoids() {
  for (int i = 15; i >= 0; i--) {
    setIfBit(bank[0],i,pin_data1);
    setIfBit(bank[1],i,pin_data2);
    setIfBit(bank[2],i,pin_data3);
    setIfBit(bank[3],i,pin_data4);
    digitalWrite(pin_shift_clock,HIGH);
    digitalWrite(pin_shift_clock,LOW);
  }
  digitalWrite(pin_register_clock,HIGH);
  digitalWrite(pin_register_clock,LOW);
}

uint8_t previous_char;
bool shift_state;

void setShift(bool shift) {
  if (shift_state != shift) {
    if (shift) {
      setSolenoid(3,0xB);
    }
    writeSolenoids();
    enableSolenoids();
    delay(160);
    if (!shift) {
      // There's a lot of bounce coming out of shift!
      delay(150);
    }
    disableSolenoids();
  }
  if (shift) setSolenoid(3,0xB);
  shift_state = shift;
}

void typeKey(uint8_t c) {
  uint8_t dat = keymap[c & 0x7F];
  if (dat == 0xFF) return;
  bool shift = (dat & 0x80) != 0;
  uint8_t bank = (dat >> 4) & 0x03;
  uint8_t solenoid = dat & 0x0F;
  clearBanks();
  setShift(shift);  
  setSolenoid(bank,solenoid);
  disableSolenoids();
  writeSolenoids();
  enableSolenoids();
  delay(75);
  if (c == ' ') { delay(35); }
  disableSolenoids();
  clearBanks();
  if (shift) setSolenoid(3,0xB);
  writeSolenoids();
  enableSolenoids();
  delay(30);
}

char command_buffer[128];

/*
 * Configurable delay between two characters. Ideally you should have
 * a longer delay between keys that are closer together.
 */
void delayBetween(uint8_t last, uint8_t next) {
  delay(kernTime(last,next) + 55);
}

boolean doCarriageReturn() {
  const static uint8_t ARM_SW = 1<<2;  
  setMotor(MOTOR_ON);
  delay(50);
  uint32_t i; 
  for (i = 0; i < 160000UL; i++) {
    if ((getSwitchState() & ARM_SW) == 0) {
      break;
    }
  }
  setMotor(MOTOR_OFF);
  delay(5);
  setMotor(MOTOR_BRAKE);
  delay(200);
  setMotor(MOTOR_OFF);
  return i < 160000UL;
}

/*
 * Command reference
 * All commands are terminated by a newline ('\n') character. All responses
 * are terminated by a newline character.
 * Carriage return characters ('\r') are ignored.
 * ping   - Ping the board to see if we're connected.
 *          Returns "OK version" where "version" is an arbitrary version
 *          string.
 * TC     - Type character.
 *          C = the character to type.
 * tBR    - Type character by grid position. 
 *          B = the number of the inductor board [0-3].
 *          R = the number (in hex) of the inductor to use [0-B].
 *          Returns "OK".
 * mon    - Turn motor on.
 *          Returns "OK".
 * moff   - Turn motor off.
 *          Returns "OK".
 * mbrake - Brake motor.
 *          Returns "OK".
 * read   - Reads the value of the switches as a byte.
 *          Returns a single hexidecimal character.
 * Wstr   - Write string. Terminated by a '\r' or '\n' character.
 *          Types the entire string.
 * cr     - Carriage return. Return platen to home position and
 *          advance one line.
 */

void doCommand(char* buf) {
  String cmd(buf);
  cmd.trim(); // In-place in 1.0 and up
  if (cmd.equals("ping")) {
    Uart.println("OK v0.01");
  } else if (cmd.equals("mon")) {
    setMotor(MOTOR_ON); Uart.println("OK");
  } else if (cmd.equals("moff")) {
    setMotor(MOTOR_OFF); Uart.println("OK");
  } else if (cmd.equals("mbrake")) {
    setMotor(MOTOR_BRAKE); Uart.println("OK");
  } else if (cmd.equals("cr")) {
    if (doCarriageReturn()) {
      Uart.println("OK");
    } else {
      Uart.println("ERR2 bad return");
    }
  } else if (cmd.startsWith("W")) {
    char last = 0;
    for (uint8_t i = 1; i < cmd.length(); i++) {
      delayBetween(last,cmd[i]);
      last = cmd[i];
      typeKey(cmd[i]);
    }
    disableSolenoids();
    setShift(false);
    Uart.println("OK");
  } else if (cmd.equals("read")) {
    Uart.println(String(getSwitchState(),HEX));
  } else if (cmd.startsWith("T")) {
    char c = cmd[1];
    typeKey(c);
    Uart.println("OK");
  } else if (cmd.startsWith("t")) {
    char cbank = cmd[1];
    char creg = cmd[2];
    int bank = cbank - '0';
    int reg;
    if (bank < 0 || bank > 3) {
      Uart.println("ERR1 bank out of range in "+cmd);
      return;
    }
    if (creg <= '9' && creg >= '0') {
      reg = creg - '0';
    } else if (creg == 'a' || creg == 'A') {
      reg = 10;
    } else if (creg == 'b' || creg == 'B') {
      reg = 11;
    } else {
      Uart.println("ERR2 register out of range in "+cmd);
      return;
    }
    clearBanks();
    setSolenoid(bank,reg);
    writeSolenoids();
    enableSolenoids();
    delay(100);
    disableSolenoids();
    clearBanks();
    writeSolenoids();
    delay(180);
    Uart.println("OK");
  } else {
    Uart.println("ERR0 unrecognized command " + cmd);
  }
}

void loop() 
{
  int c = '\0';
  int idx = 0;
  do {
    if (Uart.available() > 0) {
      c = Uart.read();
      command_buffer[idx++] = c;
      if (idx > 127) idx = 127;
    }
  } while (c != '\r');
  
  if (idx > 1) {
    command_buffer[idx] = '\0';
    doCommand(command_buffer);
  }
}
