#include "LedControl.h"
#include "arduinoFFT.h"

// --- LED Matrix Configuration ---
LedControl lc = LedControl(12, 10, 11, 1);

// --- FFT Configuration ---
#define SAMPLES 64              
#define SAMPLING_FREQUENCY 9000 
#define MIC_PIN A0
#define ROWS_LIGHTS 8

const int sensorAnalogPin = A0;
int bins = (SAMPLES / 2) / ROWS_LIGHTS;

int averageEnergy = 0;
double vReal[SAMPLES];
double vImag[SAMPLES];

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQUENCY);

void setup() {
  Serial.begin(115200); 
  
  lc.shutdown(0, false);       
  lc.setIntensity(0, 5);       
  lc.clearDisplay(0);          
}

void loop() {
    // 1. RAW SPEED SAMPLING 
    // We sample as fast as the ADC allows to ensure we capture higher frequencies
    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = analogRead(MIC_PIN); 
      vIm
      ag[i] = 0.0;                 
    }

    // 2. FFT CALCULATIONS
    FFT.dcRemoval();                     
    FFT.windowing(FFT_WIN_TYP_HAMMING, FFT_FORWARD); 
    FFT.compute(FFT_FORWARD);            
    FFT.complexToMagnitude();            
    // Example: Combine 64 bins into 8 wide custom frequency bands

    for (int band = 0; band < ROWS_LIGHTS; band++) {
      double bandEnergy = 0;
      
      // Sum up 8 consecutive bins per band
      for (int bin = 0; bin < bins; bin++) {
        bandEnergy += vReal[(band * bins) + bin];
      }
      averageEnergy = abs(bandEnergy / bins);
      if(averageEnergy > 8)
        averageEnergy = 8;
      for (int i = 0; i < averageEnergy; i++)
        lc.setLed(0, band, i, true);
    }
  delay(50);
  lc.clearDisplay(0);
}