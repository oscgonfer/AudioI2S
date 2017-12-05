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
  //typeRMS = 1 if time domain -- typeRMS = 2 if spectrum domain
  double _rmsOut = 0;
  const q31_t* _pBuffer = (const q31_t*) inputBuffer; 
 
  for (int i = 0; i < inputSize; i ++) { 
    _rmsOut += (*_pBuffer) * (*_pBuffer); 
    _pBuffer++; 
  } 
  _rmsOut = sqrt(_rmsOut/inputSize); 
  
  switch (typeRMS) {
    case TIME_W_WIN: //TIME DOMAIN SIGNAL W/ WINDOW
      _rmsOut = _rmsOut * 1/_factorRMS* FACTOR; 
      break;
    case FREQ: //SPECTRUM IN FREQ DOMAIN
      _rmsOut = _rmsOut * 1/_factorRMS * FACTOR * sqrt(inputSize) / sqrt(2); 
      break;
    case TIME_WO_WIN: //TIME DOMAIN SIGNAL W/O WINDOW
      _rmsOut = _rmsOut * FACTOR; 
  }
  
  return _rmsOut;
} 

void AudioAnalyser::convert2DB(void *inputVector, void *outputVector, int vectorSize){
    const q31_t* _vect = (const q31_t*) inputVector;
    q31_t* _vectDB = (q31_t*) outputVector;

    for (int i = 0; i<vectorSize;i++){
      if (*_vect>0){ 
        *_vectDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2) * (*_vect)));
        if (*_vectDB < 0 ) *_vectDB = 0;      
      } else {
        *_vectDB = 0;
      }
      _vect++;
      _vectDB++;
    }
}

bool AudioAnalyser::window(void *vector, void *windowTable, int vectorSize){
  q31_t* srcW = (q31_t*)vector;
  double* win = (double*) windowTable;

  //Apply window in time-domain
  for (int i = 0; i < vectorSize; i ++) {
    (*srcW) *= *win;
    srcW++;
    win++;
  }
  return true;
}

void AudioAnalyser::createWindow(void *vector, int vectorSize, WindowType windowType){
  double * vect = (double*) vector;

  for (int i = 0; i < vectorSize; i++) {
    switch (windowType) {
      case HANN:
        *vect = 0.5*(1-cos(2*3.14159*i/(vectorSize-1)));
        vect++;
        break;
      case HAMMING:
        *vect = 0.54-0.46*cos(2*3.14159*i/(vectorSize-1));
        vect++;
        break;
      case BLACKMAN:
        *vect = 7938/18608 - 9240/18608*cos(2*3.14159*i/(vectorSize-1))+1430/18608*cos(4*3.14159*i/(vectorSize-1));
        vect++;
        break;
    }
  } 
  
  switch (windowType) {
    case HANN:
      _factorRMS = 0.61207;
      break;
    case HAMMING:
      _factorRMS = 0.63009;
      break;
    case BLACKMAN:
      _factorRMS = 0.55164;
      break;
  }
}