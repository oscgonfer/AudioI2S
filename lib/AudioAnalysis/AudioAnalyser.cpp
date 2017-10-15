#include "AudioAnalyser.h"

int AudioAnalyser::input(AudioIn& input)
{
  return input.setAnalyser(this);
}

void AudioAnalyser::Scaling(void *vector, int vectorSize, double factor, bool multDiv){
    // SCALE signal by factor
    q31_t* _vectDW = (q31_t*) vector;
    for (int i = 0; i<vectorSize;i++){
      if (multDiv) {
        *_vectDW *= factor;
      }
      else {
        *_vectDW /= factor;
      }
      _vectDW++;
    }
}

double AudioAnalyser::RMSG(void *inputBuffer, int inputSize, int typeRMS, int FACTOR){ 
  //typeRMS = 1 if time domain -- typeRMS = 2 if spectrum domain
  double _rmsOut = 0;
  const q31_t* _pBuffer = (const q31_t*) inputBuffer; 
 
  for (int i = 0; i < inputSize; i ++) { 
    _rmsOut += (*_pBuffer) * (*_pBuffer); 
    _pBuffer++; 
  } 
  _rmsOut = sqrt(_rmsOut/inputSize); 
  
  switch (typeRMS) {
    case 1: //TIME DOMAIN SIGNAL
      _rmsOut = _rmsOut * 1/RMS_HANN* FACTOR; 
      break;
    case 2: //SPECTRUM IN FREQ DOMAIN
      _rmsOut = _rmsOut * 1/RMS_HANN * FACTOR * sqrt(inputSize) / sqrt(2); 
      break;
    case 3:
      _rmsOut = _rmsOut * FACTOR; 
  }
  
  return _rmsOut;
} 

void AudioAnalyser::Convert2DB(void *vectorSource, void *vectorDest, int vectorSize){

    q31_t* _vectDB = (q31_t*) vectorDest;
    const q31_t* _vect = (const q31_t*) vectorSource;

    for (int i = 0; i<vectorSize;i++){
      if (*_vect>0){ 
        *_vectDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2) * (*_vect)));
      } else {
        *_vectDB = 0;
      }
      _vectDB++;
      _vect++;
    }
}

void AudioAnalyser::SerialPrint(String ToPrint, int PrioFac, bool NewLine){
  // ONLY FOR PRINTING
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}