
//#include <mathFunctions.h>

#include <stdio.h>

void Timer0_Init(void);

const int NUM_OF_BEATS = 5;
const int NUM_SAMPLES = 2000;
int input = A0;
//float time;
unsigned long count = 0;
int inputs[NUM_SAMPLES];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Timer0_Init();

  //set A0 as our pin to read
  //int input = A0;
}
void loop() {
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    int in = analogRead(input);
    inputs[i] = in;
  }


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

  //Populate the array
  
  //Sort the array and make a copy
  //inputs2 = inputs;
  //quickSort(&inputs, 0, 999);

  //Find max values (rudimentary, but it should work)
  //int copy[NUM_SAMPLES] = inputs;
  int peaks[] = {-1, -1, -1, -1, -1};
  findMaxVals(inputs, peaks);
  
  //once the indices of the peaks are found, we have the times of the peaks!
  
  //We need to get the values and the time span between them.
  double hr = calcHeartRate(peaks, peaks[NUM_SAMPLES - 1]);

  Serial.println("THE HEARTRATE IS: ");
  Serial.println(hr);

}

float calcHeartRate(int* indices, int lastPeak){
  
  int size_ = 0;
  
  while(indices[size_] != -1 && size_ < NUM_OF_BEATS){
    size_++;
  }
  
  if(size_ == 1){
    return size_/(indices[0] + NUM_SAMPLES - lastPeak)*30000;
  }
  
  //(else)
  return size_/(indices[size_ - 1] - indices[0])*30000;
  
}

void findMaxVals(int* arr, int* peakIndices) {

  //int[] peakIndices = {-1,-1,-1,-1,-1};
  int peakIn = 0;

  bool isPeak = false;

  //need to check up to the second to last as "i"
  int peak;
  int curNum;

  for (int i = 0; i <= NUM_SAMPLES - 1; i++) {
    while (peakIn < NUM_OF_BEATS) {
      curNum = arr[i];
      peak = curNum;

      if (arr[i + 1] > curNum) {
        peak = arr[i + 1];
        peak = i + 1;
      }
      //check for a drop below 20 pts (rudimentary)
      else if (arr[i + 1] + 20 < curNum) {
        isPeak = true;
      }

      if (isPeak){
        peakIndices[peakIn] = i;
        peak = false;
        peakIn ++;
      }
    }
  }

}

