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

uint16_t bank[4];

void clearBanks() {
  bank[0] = bank[1] = bank[2] = bank[3] = 0;
}

void setup() 
{ 
  clearBanks();
  setPinDirections();
  writeSolenoids();  
  Serial.begin(9600); 
  Serial.println("USB connection online."); 
  Uart.begin(9600); 
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

char command_buffer[128];

/*
 * Command reference
 * All commands are terminated by a newline ('\n') character. All responses
 * are terminated by a newline character.
 * Carriage return characters ('\r') are ignored.
 * ping   - Ping the board to see if we're connected.
 *          Returns "OK version" where "version" is an arbitrary version
 *          string.
 * tBR    - Type character. 
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
  } else if (cmd.equals("read")) {
    Uart.println(String(getSwitchState(),HEX));
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
