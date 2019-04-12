float PressureMin=0;
float PressureMax=5.8;
float Vsupply=5;

int analogPin=A0;
float volta =0;
int i;
float maxvolt =0;
float volt =0;
float pressure =0;
float MAP =0;
float maxv=0;
//
int air = 7;
int val =8;



void setup() {
    Serial.begin(9600);
    pinMode(3,OUTPUT);
    pinMode(A0,INPUT);
    //
    pinMode(air, OUTPUT);
  pinMode(val, OUTPUT);
}
 
void loop(){
   digitalWrite(air, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(val, LOW);   // turn the LED on (HIGH is the voltage level)

  for(i=0; i <50; i=i+1) {
   volta= analogRead(analogPin);
   volt= (volta*Vsupply)/(pow(2,10));
  maxv = max(abs(volt-.05),maxvolt);
  maxvolt = abs(maxv-0.05);
  
  delay(250);
  }
  


delay(15000);                       // wait for a second

  digitalWrite(air, HIGH);   // turn the LED on (HIGH is the voltage level)
  digitalWrite(val, HIGH);   // turn the LED on (HIGH is the voltage level)

  //
  pressure = (((maxvolt)-.1*Vsupply)/((.8*Vsupply)/(PressureMax-PressureMin)))+PressureMin;

Serial.print("Volt:");
Serial.print(volt);
Serial.println();
Serial.print("Pressure:(psi)");
Serial.print(pressure);
Serial.println();


Serial.print("Pressure(mmhg):");
Serial.print(pressure*51.7);
Serial.println();

Serial.print(pressure*1.1*51.7);
Serial.print("/");
Serial.print(pressure*0.8*51.7);
Serial.println();

while(1);

  
}
