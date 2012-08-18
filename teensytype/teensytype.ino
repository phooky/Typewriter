
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
}

void setPinDirections() {
  pinMode(pin_shift_clock, OUTPUT, LOW);
  pinMode(pin_register_clock, OUTPUT, LOW);
  pinMode(pin_output_enable, OUTPUT, LOW);
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

void setup() 
{ 
  setPinDirections();
  Serial.begin(9600); 
  Serial.println("USB connection online."); 
  Uart.begin(9600); 
  Uart.println("Serial connection online."); 
}

void loop() 
{
}
