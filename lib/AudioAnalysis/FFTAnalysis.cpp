#include "FFTAnalysis.h"

FFTAnalysis::FFTAnalysis(uint32_t fftSize) :
  //BUFFER Sizes
  _fftSize(fftSize),
  //BUFFERs
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  _AspectrumBuffer(NULL),
  _spectrumBufferDB(NULL),
  _AspectrumBufferDB(NULL),
  _sampleBufferFilt(NULL),
  //RMS
  _rms_specB(0),
  _rms_AspecB(0),
  _rms_specBDB(0),
  _rms_AspecBDB(0),
  //EXTRAS
  _SpectrumAvailable(1),
  _bitsPerSample(0),
  _channels(0),
  _bufferSize(0),
  _sampleRate(0),
  _bufferAvailable(1),
  _available(1),
  _SpectrumDBOutput(false)
{
}

FFTAnalysis::~FFTAnalysis(){

  if (_fftBuffer) {
    free(_fftBuffer);
  }

  if (_spectrumBuffer) {
    free(_spectrumBuffer);
  }

  if (_AspectrumBuffer) {
    free(_AspectrumBuffer);
  }

  if (_spectrumBufferDB) {
    free(_spectrumBufferDB);
  }

  if (_AspectrumBufferDB) {
    free(_AspectrumBufferDB);
  }
}

int FFTAnalysis::ConfigureFFT(int bitsPerSample,int channels, int bufferSize, int sampleRate, bool SpectrumDBOutput){

    _bitsPerSample = bitsPerSample;
    _channels = channels;
    _bufferSize = bufferSize;
    _sampleRate = sampleRate;
    _SpectrumDBOutput = SpectrumDBOutput;

    //General fool-proof checks
    if (_bitsPerSample != 32) {
      return 0;
    }

    if (_channels != 1 && _channels != 2) {
      return 0;
    }

    //Initialize fft
    if (ARM_MATH_SUCCESS != arm_rfft_init_q31(&_S31, _fftSize, 0, 1)) {
      return 0;
    }

    //Allocate memory for buffers
      //Allocate time buffer
      _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
      _sampleBufferWin = calloc(_bufferSize, sizeof(q31_t));
      _fftBuffer = calloc(_fftSize, sizeof(q31_t));
      //Allocate frequency buffers
      _spectrumBuffer = calloc(_fftSize/2, sizeof(q31_t));
      _AspectrumBuffer = calloc(_fftSize/2,sizeof(q31_t));

      if (SpectrumDBOutput){
        //Allocate frecuency dB buffers
        _spectrumBufferDB = calloc(_fftSize/2, sizeof(q31_t));
        _AspectrumBufferDB = calloc(_fftSize/2,sizeof(q31_t));
      }
      

    //Free all buffers in case of bad allocation
    if (_sampleBuffer == NULL || _spectrumBuffer == NULL || _AspectrumBuffer == NULL) {

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_spectrumBuffer) {
        free(_spectrumBuffer);
        _spectrumBuffer = NULL;
      }

      if (_AspectrumBuffer) {
        free(_AspectrumBuffer);
        _AspectrumBuffer = NULL;
      }
      if (SpectrumDBOutput){
        if (_spectrumBufferDB) {
          free(_spectrumBufferDB);
          _spectrumBufferDB = NULL;
        }

        if (_AspectrumBufferDB) {
          free(_AspectrumBufferDB);
          _AspectrumBufferDB = NULL;
        }
      }
      return 0;
    }
    return 1;
}

double AudioAnalysis::AudioSpectrumRead(int fftSize){
  if (!_SpectrumAvailable) {
    return 0;
  }

  // Get buffer (currently hardcoded)
  GetBuffer();

  // Downscale the sample buffer for proper functioning
  Scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

  // Apply Hann Window
  Window();
  
  // FFT - EQUALIZATION and A-WEIGHTING
  FFT(_sampleBufferWin, _spectrumBuffer, _fftSize);
  EQUALIZING(_spectrumBuffer, _fftSize/2);
  A_WEIGHTING(_spectrumBuffer, _AspectrumBuffer, _fftSize/2);

  // RMS CALCULATION  
  _rms_specB = RMSG(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
  _rms_AspecB = RMSG(_AspectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 

  free(_spectrumBuffer);
  free(_AspectrumBuffer);
  
  _rms_specBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_specB)); 
  _rms_AspecBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_AspecB)); 
  
  // Set available to 0 to wait for new process
  _SpectrumAvailable = 0;

  return _rms_AspecBDB; 
}

double AudioAnalysis::AudioSpectrumRead(int spectrum[], int Aspectrum [],int spectrumDB[], int AspectrumDB[], int fftSize){
  
  if (!_SpectrumAvailable) {
    return 0;
  }

  // Get buffer (currently hardcoded)
  GetBuffer();
  
  // Downscale the sample buffer for proper functioning
  Scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

  // Apply Hann Window
  Window();
  
  // FFT - EQUALIZATION and A-WEIGHTING
  int _time_before_FFT = millis();
  FFT(_sampleBufferWin, _spectrumBuffer, _fftSize);
  EQUALIZING(_spectrumBuffer, _fftSize/2);
  A_WEIGHTING(_spectrumBuffer, _AspectrumBuffer, _fftSize/2);
  int _time_after_FFT = millis();
  int _time_delta_FFT = _time_after_FFT-_time_before_FFT;
  SerialPrint("FFT Time: \t" + String(_time_delta_FFT),6,true);

  /*
  time_delta_Down :   14
  time_delta_FIR :  173
  time_delta_RMS :  4
  */

  q31_t* printW = (q31_t*)_spectrumBuffer;
  q31_t* printWA = (q31_t*)_AspectrumBuffer;

  //Apply hann window in time-domain
  for (int i = 0; i < _fftSize/2; i ++) {
    SerialPrint(String(i)+"\t"+String(*printW) +"\t"+String(*printWA), 6, true);
    printW++;
    printWA++;
  }

  // RMS CALCULATION  
  int _time_before_RMS = millis();
  _rms_specB = RMSG(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
  _rms_AspecB = RMSG(_AspectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
  
  _rms_specBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_specB)); 
  _rms_AspecBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_AspecB)); 
  int _time_after_RMS = millis();
  int time_delta_RMS = _time_after_RMS-_time_before_RMS;
 
  // UPSCALING THE BUFFERS
  Scaling(_sampleBuffer,_bufferSize,CONST_FACTOR,true);
  Scaling(_spectrumBuffer,fftSize/2,CONST_FACTOR,true);
  Scaling(_AspectrumBuffer,fftSize/2,CONST_FACTOR,true);

  // COPY SPECTRUMS TO MAIN
  memcpy(spectrum, _spectrumBuffer, sizeof(int) * _fftSize/2);
  memcpy(Aspectrum, _AspectrumBuffer, sizeof(int) * _fftSize/2);

  if (_SpectrumDBOutput){
    // CONVERT 2 DB THE BUFFERS
    Convert2DB(_spectrumBuffer, _spectrumBufferDB, _fftSize/2);
    Convert2DB(_AspectrumBuffer, _AspectrumBufferDB, _fftSize/2);
    // COPY SPECTRUMS TO MAIN
    memcpy(spectrumDB, _spectrumBufferDB, sizeof(int) * _fftSize/2);
    memcpy(AspectrumDB, _AspectrumBufferDB, sizeof(int) * _fftSize/2);
  }
  // Set available to 0 to wait for new process
  _SpectrumAvailable = 0;

  return _rms_AspecBDB; 
}

double AudioAnalysis::AudioRMSRead_dB(){
  return _rms_specBDB;
}

void FFTAnalysis::FFT(void *_inputBuffer, void* _outputBuffer, int _fftBufferSize){
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


void FFTAnalysis::EQUALIZING(void *inputBuffer, int inputSize){
  q31_t* spBE = (q31_t*)inputBuffer;

  for (int i = 0; i < inputSize; i ++) {
    //Deconvolution of the spectrumBuffer by division of the microphone frequency response
    double equalfactor = EQUALTAB[i];
    *spBE /= equalfactor;
    spBE++;
  }
}

void FFTAnalysis::A_WEIGHTING(void *inputBuffer, void *outputBuffer, int inputSize){
  //Apply a-weighting to spectrumBuffer
  const q31_t* spBA = (const q31_t*)inputBuffer;
  q31_t* AspBA = (q31_t*)outputBuffer;

  for (int i = 0; i < inputSize; i ++) {
    //Apply a-weighting
    double weighingfactor = WEIGHTINGTAB[i];
    *AspBA = weighingfactor * (*spBA);
    spBA++;
    AspBA++;
  }
}

void AudioAnalysis::SerialPrint(String ToPrint, int PrioFac, bool NewLine){
  // ONLY FOR PRINTING
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}