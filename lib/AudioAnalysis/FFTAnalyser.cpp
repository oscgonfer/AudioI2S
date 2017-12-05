#include "FFTAnalyser.h"

FFTAnalyser::FFTAnalyser(int bufferSize, int fftSize, WeightingType weighting_type) :
  //BUFFER Sizes
  _fftSize(fftSize),
  _bufferSize(bufferSize),
  //BUFFERs
  _sampleBuffer(NULL),
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  _spectrumBufferDB(NULL),
  //RMS
  _rmsSpecB(0),
  //EXTRAS
  _weighting_type(weighting_type)
{
}

FFTAnalyser::~FFTAnalyser()
{
  if (_sampleBuffer){
    free(_sampleBuffer);
  }

  if (_fftBuffer) {
    free(_fftBuffer);
  }

  if (_spectrumBuffer) {
    free(_spectrumBuffer);
  }

}

bool FFTAnalyser::configure(AudioInI2S& input){

  //Initialize fft
  if (ARM_MATH_SUCCESS != arm_rfft_init_q31(&_S31, _fftSize, 0, 1)) {
    return false;
  }
  //Allocate time buffer
  _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
  _fftBuffer = calloc(_fftSize, sizeof(q31_t));
  //Allocate frequency buffers
  _spectrumBuffer = calloc(_fftSize/2, sizeof(q31_t));
  _spectrumBufferDB = calloc(_fftSize/2, sizeof(q31_t));

  //Free all buffers in case of bad allocation
  if (_sampleBuffer == NULL || _fftBuffer == NULL || _spectrumBuffer == NULL) {

    if (_sampleBuffer) {
      free(_sampleBuffer);
      _sampleBuffer = NULL;
    }

    if (_fftBuffer) {
      free(_fftBuffer);
      _fftBuffer = NULL;
    }

    if (_spectrumBuffer) {
      free(_spectrumBuffer);
      _spectrumBuffer = NULL;
    }
    return false;
  }
  return true;
}

double FFTAnalyser::sensorRead(int spectrum[]){

  if (audioInI2SObject.readBuffer(_sampleBuffer,_bufferSize)){
    uint32_t time_after = micros();

    // Downscale the sample buffer for proper functioning
    scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

    // Apply Hann Window
    window(_sampleBuffer,_bufferSize);
  
    // FFT - EQUALIZATION and WEIGHTING
    fft(_sampleBuffer, _spectrumBuffer, _fftSize);
    equalising(_spectrumBuffer, _fftSize/2);

    switch (_weighting_type) {

      case A_WEIGHTING:
      case C_WEIGHTING:
        weighting(_spectrumBuffer, _fftSize/2);
        _rmsSpecB = rms(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
        convert2DB(_spectrumBuffer, _spectrumBufferDB, _fftSize/2);
        memcpy(spectrum, _spectrumBufferDB, sizeof(int) * _fftSize/2);
        break;

      case Z_WEIGHTING:
        _rmsSpecB = rms(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR);
        convert2DB(_spectrumBuffer, _spectrumBufferDB, _fftSize/2);
        memcpy(spectrum, _spectrumBufferDB, sizeof(int) * _fftSize/2);
        break;
    }

    _rmsSpecB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rmsSpecB));
    SerialUSB.println(micros()-time_after);

    return _rmsSpecB;
  } else {
    return 0;
  }
}

double FFTAnalyser::sensorRead(){

  if (audioInI2SObject.readBuffer(_sampleBuffer,_bufferSize)){
    uint32_t time_after = micros();

    // Downscale the sample buffer for proper functioning
    scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

    // Apply Hann Window
    window(_sampleBuffer,_bufferSize);
  
    // FFT - EQUALIZATION and WEIGHTING
    fft(_sampleBuffer, _spectrumBuffer, _fftSize);
    equalising(_spectrumBuffer, _fftSize/2);

    switch (_weighting_type) {

      case A_WEIGHTING:
      case C_WEIGHTING:
        weighting(_spectrumBuffer, _fftSize/2);
        _rmsSpecB = rms(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
        break;

      case Z_WEIGHTING:
        _rmsSpecB = rms(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
        break;
    }
  
    _rmsSpecB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rmsSpecB));
    SerialUSB.println(micros()-time_after);
  }
  return _rmsSpecB;
}

void FFTAnalyser::fft(void *_inputBuffer, void* _outputBuffer, int _fftBufferSize){
  //_inputBuffer is already treated for FFT (usable samples, averaged, windowed)

  // Calculate FFTBuffer ((r-i,r-i...))
  arm_rfft_q31(&_S31, (q31_t*)_inputBuffer, (q31_t*)_fftBuffer);

  //Calculate spectrumBuffer and normalize
  const q31_t* _pfftBuffer = (const q31_t*)_fftBuffer;
  q31_t* _pspectrumBuffer = (q31_t*) _outputBuffer;

  for (int i = 0; i < _fftBufferSize; i +=2) {
    *_pspectrumBuffer = (*_pfftBuffer) * (*_pfftBuffer);
    _pfftBuffer++;
    
    *_pspectrumBuffer += (*_pfftBuffer) * (*_pfftBuffer);
    *_pspectrumBuffer = sqrt(*_pspectrumBuffer);

    //Normalize SpectrumBuffer
    if (i) {
      *_pspectrumBuffer = 2 * (*_pspectrumBuffer);
    }

    _pfftBuffer++;
    _pspectrumBuffer++;
  }
}

void FFTAnalyser::equalising(void *inputBuffer, int inputSize){
  //Deconvolution of the spectrumBuffer by division of the microphone frequency response

  q31_t* spBE = (q31_t*)inputBuffer;

  for (int i = 0; i < inputSize; i +=8) {
    double equalfactor = EQUALTAB[i];
    *spBE /= equalfactor;
    spBE++;
  }
}

void FFTAnalyser::weighting(void *inputBuffer, int inputSize){
  //Apply weighting to Buffer
  q31_t* spB = (q31_t*)inputBuffer;
  double weighingfactor = 0;
  
  for (int i = 0; i < inputSize; i +=8) {
    
    //Apply weighting
    switch (_weighting_type) {

      case A_WEIGHTING: //A_WEIGHTING
        weighingfactor = A_WEIGHTINGTAB[i];
        break;

      case C_WEIGHTING: //C_WEIGHTING
        weighingfactor = C_WEIGHTINGTAB[i];
        break;
    }

    *spB *= weighingfactor;
    spB++;
  }
}