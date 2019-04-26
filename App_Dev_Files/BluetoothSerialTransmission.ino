#include <SoftwareSerial.h>
#include <math.h>

#define BP 0b00
#define ECG 0b01
#define PO 0b10
#define TEMP 0b11

// change baud rate to allow for faster data transmission???

SoftwareSerial Bluetooth(10, 11); // RX | TX

void setup() {
  Bluetooth.begin(38400);  //Default Baud for comm
  Serial.begin(9600);
  Serial.println("The bluetooth gates are open.\nConnect to HC-05 from any other bluetooth device with 1234 as pairing key!");
}

void loop() {
  // Output any data from bluetooth to Serial Monitor
  if (Bluetooth.available()) {
    Serial.write(Bluetooth.read());
  }

  static int i = 0;

  // bp test
  int systolicTest = serializeBP(123);
  byte systolicArray[2];
  int diastolicTest = serializeBP(90);
  byte diastolicArray[2];
  
  systolicArray[0] = (systolicTest & 0xFF00) >> 8;
  systolicArray[1] = systolicTest & 0x00FF;
  diastolicArray[0] = (diastolicTest & 0xFF00) >> 8;
  diastolicArray[1] = diastolicTest & 0x00FF;
  // ecg test
  int ecgTest = serializeECG(200 * sin(i) + 200);
  byte ecgArray[2];
  ecgArray[0] = (ecgTest & 0xFF00) >> 8;
  ecgArray[1] = ecgTest & 0x00FF;
  // po test
  int poTest = serializePO(200 * sin(i) + 200);
  byte poArray[2];
  poArray[0] = (poTest & 0xFF00) >> 8;
  poArray[1] = poTest & 0x00FF;
  // temp test
  int tempTest = serializeTemp(readTemp());
  byte tempArray[2];
  tempArray[0] = (tempTest & 0xFF00) >> 8;
  tempArray[1] = tempTest & 0x00FF;

  if (i > 100) {
    i = 0;
  } else {
    i++;
  }
  
  Bluetooth.write(systolicArray, 2);
  //Bluetooth.flush();
  delay(10);
  Bluetooth.write(diastolicArray, 2);
  delay(10);
  Bluetooth.write(ecgArray, 2);
  //Bluetooth.flush();
  delay(10);
  Bluetooth.write(poArray, 2);
  //Bluetooth.flush();
  delay(10);
  Bluetooth.write(tempArray, 2);
  //Bluetooth.flush();
  delay(10);
}

float readTemp() {
  // Set up variables to use to measure resistance and calculate temperature
  const int seriesResistance = 10000;
  const int nominalTemp = 25;
  const double cToK = 273.15;
  const int bCoeff = 3950;
  float thermistor_reading;
  float thermistor_resistance;
  float inverse_temp;
  float thermistor_temp;

  // Read voltage across thermistor on analog pin
  thermistor_reading = analogRead(A0);

  // Convert voltage reading to resistance of thermistor using known series resistor value
  thermistor_resistance = seriesResistance / ((1023.0 / thermistor_reading) - 1.0);
  
  // Use one parameter B equaton to convert from thermistor resistance to temp
  // 1/T = 1/T0 + 1/B*ln(R/R0)
  inverse_temp = (1.0 / (nominalTemp + cToK)) + (1.0 / bCoeff) * log(thermistor_resistance / seriesResistance);

  // Convert to Celcius from Kelvin
  thermistor_temp = (1.0 / inverse_temp) - cToK;
  thermistor_temp = thermistor_temp * 9 / 5 + 32;
  Serial.println(thermistor_temp);
  return thermistor_temp;
}

float readBP() {
  float PressureMin=0;
  float PressureMax=5.8;
  float Vsupply=5;
  int analogPin=A0;
  float volta =0;
  float maxvolt =0;
  float volt =0;
  float pressure =0;
  float MAP =0;
  float maxv=0;
  int air = 7;
  int val =8;

  pinMode(3, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(air, OUTPUT);
  pinMode(val, OUTPUT);

  digitalWrite(air, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(val, LOW);   // turn the LED on (HIGH is the voltage level)

  for(int i=0; i <50; i=i+1) {
    volta= analogRead(analogPin);
    volt= (volta*Vsupply)/(pow(2,10));
    maxv = max(abs(volt-.05),maxvolt);
    maxvolt = abs(maxv-0.05);
    delay(250);
  }
  
  delay(15000);               // wait for a second
  digitalWrite(air, HIGH);    // turn the LED on (HIGH is the voltage level)
  digitalWrite(val, HIGH);    // turn the LED on (HIGH is the voltage level)
  pressure = (((maxvolt)-.1*Vsupply)/((.8*Vsupply)/(PressureMax-PressureMin)))+PressureMin;

  return pressure;
}

/*
  Serial Protocol (16 bits) for transmitting data
  max value = 2^11 - 1 = 2047

  bit 15 (MSB) = start bit = 0;
  bits 14-13 = data type (BP = 00; ECG = 01; PO = 10; temp = 11)
  bits 12-2 = data
  bit 1 = odd parity bit (gets set to either 0 or 1 to always make serialized data an odd amount of ones; used for error checking)
  bit 0 (LSB) = stop bit = 1
*/
int serializeBP(int bp) {
  // mask with 0b000...01
  int lowMask = ~0xE002; // 1110 0000 0000 0010
  int highMask = 0x0001; // 0000 0000 0000 0001
  // to encode:
  // systolic, diastolic

  // to decode:
  // for temp, divide by 25 and add 95
  // if temp is 0, add note "temp is below 95"

  // format to serial protocol
  int data = ((bp & 0x07FF) << 2) & lowMask | highMask;

  // return serial data with odd pairity bit
  return setOddParity(data);
}

int serializeECG(int ecg) {
  // mask with 0b001...01
  int lowMask = ~0xC002; // 1100 0000 0000 0010
  int highMask = 0x2001; // 0010 0000 0000 0001
  // format to serial protocol
  int data = ((ecg & 0x07FF) << 2) & lowMask | highMask;

  // return serial data with odd pairity bit
  return setOddParity(data);
}

int serializePO(int po) {
  // mask with 0b010...01
  int lowMask = ~0xA002; // 1010 0000 0000 0010
  int highMask = 0x4001; // 0100 0000 0000 0001
  // format to serial protocol
  int data = ((po & 0x07FF) << 2) & lowMask | highMask;

  // return serial data with odd pairity bit
  return setOddParity(data);
}

int serializeTemp(int temp) {
  // mask with 0b011...01
  int lowMask = ~0x8002; // 1000 0000 0000 0010
  int highMask = 0x6001; // 0110 0000 0000 0001
  // format to serial protocol
  int data = ((temp & 0x07FF) << 2) & lowMask | highMask;

  // return serial data with odd pairity bit
  return setOddParity(data);
}

// returns 0 if even or 1 if odd number of ones appear in serial data
// data is corrupted if this returns 0 on receiving end
int isDataOdd(int serialData) {
  int onesCounter = 0;

  for (int i = 0; i < 16; i++) {
    onesCounter += (serialData >> i) & 0x0001;
  }

  return onesCounter % 2;
}

// returns serial data with odd pairity (odd number of ones) by setting pairity bit (bit 1) to 0 if odd or 1 if even
// makes result always have an odd amount of ones for odd pairity check on receiving end
int setOddParity(int serialData) {
  return serialData | (~isDataOdd(serialData) & 0x01) << 1;
}
