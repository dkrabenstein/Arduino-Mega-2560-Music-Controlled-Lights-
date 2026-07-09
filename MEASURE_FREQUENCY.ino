#include "arduinoFFT.h"

#define MIC_PIN A0          
#define SAMPLES 64         
#define SAMPLING_FREQ 9000  

double vReal[SAMPLES];
double vImag[SAMPLES];
double peak;

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void setup() {
  Serial.begin(115200); 
  pinMode(MIC_PIN, INPUT);
}

void loop() {
  // 1. DATA SAMPLING
  for (int i = 0; i < SAMPLES; i++) {
    vReal[i] = analogRead(MIC_PIN); 
    vImag[i] = 0.0;                 
  }

  // 2. FFT CALCULATIONS
  FFT.dcRemoval();                     
  FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD); 
  FFT.compute(FFT_FORWARD);            
  
  // FIXED: Converts raw complex numbers into physical amplitudes 
  FFT.complexToMagnitude();            
  
  // 3. IDENTIFY AND PRINT THE MASTER PITCH
  peak = FFT.majorPeak();
  Serial.println(peak);
  
  delay(100); // Optional: keeps the Serial Monitor readable instead of a blur
}