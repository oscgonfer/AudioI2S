#include "FFTAnalyser.h"

FFTAnalyser::FFTAnalyser(int bufferSize, int fftSize, WeightingType weightingType, WindowType windowType) :
  //BUFFER Sizes
  _fftSize(fftSize),
  _bufferSize(bufferSize),
  //BUFFERs
  _sampleBuffer(NULL),
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  _weightingTable(NULL),
  _windowTable(NULL),
  _equaliserTable(NULL),
  //RMS
  _rms(0),
  _rmsDB(0),
  //EXTRAS
  _bitsPerSample(0),
  _sampleRate(0),
  _weighting_type(weightingType),
  _window_type(windowType)
{
}

FFTAnalyser::~FFTAnalyser(){
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

  _bitsPerSample = input.bitsPerSample();
  _sampleRate = input.sampleRate();

  //Initialize fft
  if (ARM_MATH_SUCCESS != arm_rfft_init_q31(&_S31, _fftSize, 0, 1)) {
    return false;
  }

  //Allocate time buffer
  _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
  //Allocate frequency buffers
  _fftBuffer = calloc(_fftSize, sizeof(q31_t));
  _spectrumBuffer = calloc(_fftSize/2, sizeof(q31_t));
  //Allocate table for window
  _windowTable = calloc(_bufferSize, sizeof(double));
  //Allocate table for weighting
  _weightingTable = calloc(_fftSize/2, sizeof(double));
  _equaliserTable = calloc(_fftSize/2, sizeof(double));

  createWindow(_windowTable, _bufferSize, _window_type);
  createWeighting(_weightingTable, _sampleRate);
  createEqualiser(_equaliserTable);
   
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

  if (!audioInI2SObject.bufferI2SAvailable()){

    return 0;

  } else {

    if (audioInI2SObject.readBuffer(_sampleBuffer,_bufferSize)){

      // Downscale the sample buffer for proper functioning
      scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

      // Apply Window
      window(_sampleBuffer, _windowTable, _bufferSize);

      // FFT
      fft(_sampleBuffer, _spectrumBuffer, _fftSize);

      // Equalisation (for now only 256 bins)
      equalising(_spectrumBuffer, _equaliserTable, _fftSize/2);

      // Weighting (A, C, or Z)
      switch (_weighting_type) {

        case A_WEIGHTING:
        case C_WEIGHTING:
          weighting(_spectrumBuffer, _fftSize/2);
          break;
        case Z_WEIGHTING:
          break;
      }

      // Calculate RMS
      _rms = rms(_spectrumBuffer, _fftSize/2, FREQ, CONST_FACTOR); 
      _rmsDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms));

      // Upscale the spectrumBuffer
      scaling(_spectrumBuffer, _fftSize/2, CONST_FACTOR, true);

      // Convert it to DB
      convert2DB(_spectrumBuffer, _fftSize/2);

      // Copy it
      memcpy(spectrum, _spectrumBuffer, sizeof(int) * _fftSize/2);
      
      // Free buffer
      // free(_spectrumBuffer);

      return _rmsDB;

    } else {

      return 0;

    }
  }
}

double FFTAnalyser::sensorRead(){

  if (!audioInI2SObject.bufferI2SAvailable()){

    return 0;

  } else {

    if (audioInI2SObject.readBuffer(_sampleBuffer,_bufferSize)){

      // Downscale the sample buffer in order to be able to calculate SQUARES
      scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

      // Apply Window
      window(_sampleBuffer, _windowTable, _bufferSize);
    
      // FFT
      fft(_sampleBuffer, _spectrumBuffer, _fftSize);

      // Equalisation
      equalising(_spectrumBuffer, _equaliserTable, _fftSize/2);

      // Weighting (A, C or Z)
      switch (_weighting_type) {

        case A_WEIGHTING:
        case C_WEIGHTING:
          weighting(_spectrumBuffer, _fftSize/2);
          break;

        case Z_WEIGHTING:
          break;

      }

      // Calculate RMS
      _rms = rms(_spectrumBuffer, _fftSize/2, FREQ, CONST_FACTOR); 
      _rmsDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms));

      // Free buffers
      // free(_spectrumBuffer);
    
      return _rmsDB;

    } else {

      return 0;

    }
  } 
}

void FFTAnalyser::fft(void *_inputBuffer, void* _outputBuffer, int _fftBufferSize){
  //_sampleBufferWin is already treated for FFT (usable samples, averaged, windowed)

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

void FFTAnalyser::equalising(void *inputBuffer, void *equaliserTable, int inputSize){
  q31_t* spBE = (q31_t*)inputBuffer;
  const double* eqTable = (double*) equaliserTable;

  for (int i = 0; i < inputSize; i ++) {
    //Deconvolution of the spectrumBuffer by division of the microphone frequency response
    *spBE /= *eqTable;
    spBE++;
    eqTable++;
  }
}

void FFTAnalyser::createEqualiser(void *equaliserTable){
  //Interpolation of equaliser look up table to adapt it to spectrumlength
  double* eqTable = (double*) equaliserTable;
  double averagedChunk = 0;
  int chunkLength = EQSIZE/(_fftSize/2);
  int counter = 0;

  for (int i = 0; i < _fftSize/2; i++){

    for (int j =  0 ; j <chunkLength ; j++){
      averagedChunk += EQUALISATION_TABLE[counter + j];
    }

    counter += chunkLength;
    averagedChunk /= (chunkLength);
    *eqTable = averagedChunk;
    averagedChunk = 0;
    eqTable++;
  }
}

void FFTAnalyser::createWeighting(void *weightingTable, long sampleRate) {
  // Custom creation of Weighting function based on type, fftSize and sampleRate
  double* wT = (double*) weightingTable;
  double freq = 0;
  double freqSQ = 0;
  double freqS4 = 0;

  switch (_weighting_type) {
      case A_WEIGHTING:
        for (int i = 0; i < _fftSize/2; i++) {
          // Reduce frequency by 1000 in order to achieve max type capacity
          freqSQ = freq*freq;
          freqS4 = freqSQ*freqSQ;
          *wT = ((f4SQ*freqS4)/(((freqSQ+f1SQ)*sqrt(freqSQ+f2SQ))*(sqrt(freqSQ+f3SQ))*(freqSQ+f4SQ)))/AW_A1000INV;
          freq += sampleRate/(1000*_fftSize);
          wT++;
        }
        break;
      case C_WEIGHTING:
        for (int i = 0; i < _fftSize/2; i++) {
          // Reduce frequency by 1000 in order to achieve max type capacity
          freqSQ = freq*freq;
          *wT = (f4SQ*freqSQ)/((freqSQ+f1SQ)*(freqSQ+f4SQ))/CW_C1000INV;
          freq += sampleRate/(1000*_fftSize);
          wT++;
        }
        break;
  }
}

void FFTAnalyser::weighting(void *inputBuffer, int inputSize){
  //Apply weighting to Buffer

  /*
    //NORMAL ACTUAL
    q31_t* spB = (q31_t*)inputBuffer;
    double weighingfactor = 0;
    
    for (int i = 0; i < inputSize; i ++) {
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
  */

  //UPGRADE
  q31_t* spB = (q31_t*)inputBuffer;
  const double* wT = (double*) _weightingTable;
  
  for (int i = 0; i < inputSize; i ++) {
    *spB *= *wT;
    spB++;
    wT++;
  }
}