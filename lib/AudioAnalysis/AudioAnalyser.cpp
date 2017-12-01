#include "AudioAnalyser.h"

void AudioAnalyser::scaling(void *vector, int vectorSize, double factor, bool multDiv){
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

double AudioAnalyser::rms(void *inputBuffer, int inputSize, RMSType typeRMS, int FACTOR){ 
  
  double _rmsOut = 0;
  const q31_t* _pBuffer = (const q31_t*) inputBuffer; 
 
  for (int i = 0; i < inputSize; i ++) { 
    _rmsOut += (*_pBuffer) * (*_pBuffer); 
    _pBuffer++; 
  } 
  _rmsOut = sqrt(_rmsOut/inputSize); 
  
  switch (typeRMS) {
    case TIME_W_WIN: //TIME DOMAIN SIGNAL W/ WINDOW
      _rmsOut = _rmsOut * 1/RMS_HANN* FACTOR; 
      break;
    case FREQ: //SPECTRUM IN FREQ DOMAIN
      _rmsOut = _rmsOut * 1/RMS_HANN * FACTOR * sqrt(inputSize) / sqrt(2); 
      break;
    case TIME_WO_WIN: //TIME DOMAIN SIGNAL W/O WINDOW
      _rmsOut = _rmsOut * FACTOR; 
  }
  
  return _rmsOut;
} 

void AudioAnalyser::convert2DB(void *vector, int vectorSize){

    q31_t* _vect = (q31_t*) vector;

    for (int i = 0; i<vectorSize;i++){
      if (*_vect>0){ 
        *_vect = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2) * (*_vect)));
      } else {
        *_vect = 0;
      }
      _vect++;
    }
}

void AudioAnalyser::window(void *vector, int vectorSize){
  q31_t* srcW = (q31_t*)vector;

  //Apply hann window in time-domain
  for (int i = 0; i < vectorSize; i ++) {
    double window = HANN[i];
    (*srcW) *= window;
    srcW++;
  }
}