
//#include <mathFunctions.h>

#include <stdio.h>

void Timer0_Init(void);
void startNewSection();
void countPeaks(int* arr);

//want 5 sec of data
const int NUM_MS = 3000;
const int NUM_POINTS = 3;
int input = A0;
int timeStamp = 0;
//float time;
unsigned long count = 0;
float hr = 0.0;
int curSecond = 0;
static int peakCount = 0;
static int inputs[NUM_POINTS];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Timer0_Init();

  //set A0 as our pin to read
  int input = A0;
  
}
void loop() {
 
  if(timeStamp == 4){
   countPeaks(inputs);
  }
  else if(timeStamp >= NUM_MS){
    calcHr();
	  startNewSection();
  }


}

void calcHr(){
  Serial.print("HR = ");
  Serial.print(peakCount * 20);
  Serial.print(" bpm");
  Serial.println();
}

void startNewSection(){
	timeStamp = 0;
 }


void Timer0_Init(void) {
  // Disable Interrupts
  cli();
  //set timer0 interrupt at 2kHz
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 2khz increments
  OCR0A = 124;// = (16*10^6) / (2000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  // Re-enable interrupts
  sei();
}



ISR(TIMER0_COMPA_vect) {

	/* REPLACE THE CODE IN THIS FUNCTION WITH PERIODIC FUNCTION CALL */
	inputs[timeStamp] = analogRead(A0);
	timeStamp++;
}

void countPeaks(int* arr) {

  bool isPeak = false;

  int curNum;

  for (int i = 0; i < NUM_POINTS - 1; i++){
    curNum = arr[i];
  
    //check for a drop below 20 pts (rudimentary)
    if (arr[i + 1] + 20 < curNum || arr[i] + 20 < curNum){
      isPeak = true;
    }
                      
    //Found a peak!
    if (isPeak){
      isPeak = false;
      peakCount++;
    }

  }
}


