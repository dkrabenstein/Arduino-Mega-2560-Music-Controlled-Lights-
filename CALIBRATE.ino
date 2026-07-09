#include "arduinoFFT.h"
#include <string.h> // Required for memset

#define MIC_PIN A0          
#define SAMPLES 64         // Kept at 128 for memory safety on Uno/Nano
#define SAMPLING_FREQ 9000  // Approximate max speed of a standard analogRead loop
#define ROWS_LIGHTS 8
#define DATA_POINTS 100

double vReal[SAMPLES];
double vImag[SAMPLES];
double bandAverage[ROWS_LIGHTS];
double bandMax[ROWS_LIGHTS];

int bins = (SAMPLES / 2) / ROWS_LIGHTS;

ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQ);

void setup() {
  Serial.begin(115200); 
  pinMode(MIC_PIN, INPUT);
}

void loop() {
  memset(bandAverage, 0, sizeof(bandAverage));
  memset(bandMax, 0, sizeof(bandMax));

  for(int dataPoint = 0; dataPoint < DATA_POINTS; dataPoint++) {
    // 1. RAW SPEED SAMPLING 
    // We sample as fast as the ADC allows to ensure we capture higher frequencies
    for (int i = 0; i < SAMPLES; i++) {
      vReal[i] = analogRead(MIC_PIN); 
      vImag[i] = 0.0;                 
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
      double averageEnergy = abs(bandEnergy / bins);
      bandAverage[band] += averageEnergy;
      if(averageEnergy > bandMax[band])
        bandMax[band] = averageEnergy;
    }
  }

  for (int band = 0; band < ROWS_LIGHTS; band++) {

    Serial.print("Band ");
    Serial.print(band);
    Serial.print(" Average: ");
    Serial.println(bandAverage[band] / DATA_POINTS);

    Serial.print("Band ");
    Serial.print(band);
    Serial.print(" Max: ");
    Serial.println(bandMax[band]);
  }
}