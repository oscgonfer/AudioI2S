#include "AudioI2S_SCK.h"

AudioI2S_SCK::AudioI2S_SCK(uint32_t fftSize) :
  //BUFFER Sizes
  _fftSize(fftSize),
  _bufferSize(0), //Already usable buffersize
  //BUFFERs
  _sampleBuffer(NULL),
  _sampleBufferWin(NULL),
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  _AspectrumBuffer(NULL),
  _fftBufferDB(NULL),
  _spectrumBufferDB(NULL),
  _AspectrumBufferDB(NULL),
  //RMS
  _rms_specB(0),
  _rms_AspecB(0),
  _rms_specBDB(0),
  _rms_AspecBDB(0),
  //EXTRAS
  _bitsPerSample(-1),
  _channels(-1),
  _SpectrumAvailable(1),
  _bufferAvailable(1),
  _RMSAvailable(1),
  _available(1),
  _sampleRate(0)
{
}

AudioI2S_SCK::~AudioI2S_SCK(){
  if (_sampleBuffer) {
    free(_sampleBuffer);
  }

  if (_fftBuffer) {
    free(_fftBuffer);
  }

  if (_spectrumBuffer) {
    free(_spectrumBuffer);
  }

  if (_AspectrumBuffer) {
    free(_AspectrumBuffer);
  }
    if (_fftBufferDB) {
    free(_fftBufferDB);
  }

  if (_spectrumBufferDB) {
    free(_spectrumBufferDB);
  }

  if (_AspectrumBufferDB) {
    free(_AspectrumBufferDB);
  }
}

int AudioI2S_SCK::available(){
  _available = _SpectrumAvailable*_bufferAvailable;
  return _available;
}

int AudioI2S_SCK::Configure(int bitsPerSample,int channels, int bufferSize, int sampleRate){
    _bitsPerSample = bitsPerSample;
    _channels = channels;
    _bufferSize = bufferSize;
    _sampleRate = sampleRate;

    //General fool-proof checks
    if (_bitsPerSample != 16 && _bitsPerSample != 32) {
      return 0;
    }

    if (_channels != 1 && _channels != 2) {
      return 0;
    }

    //Initialize fft
    if (_bitsPerSample == 16) {
      if (ARM_MATH_SUCCESS != arm_rfft_init_q15(&_S15, _fftSize, 0, 1)) {
        return 0;
      }
    } else {
      if (ARM_MATH_SUCCESS != arm_rfft_init_q31(&_S31, _fftSize, 0, 1)) {
        return 0;
      }
    }

    //Allocate memory for buffers
    if (bitsPerSample == 16) {
      //Allocate time buffer
      _sampleBuffer = calloc(_bufferSize, sizeof(q15_t));
      _sampleBufferWin = calloc(_bufferSize, sizeof(q15_t));
      
      //Allocate frequency buffers
      _fftBuffer = calloc(_fftSize, sizeof(q15_t));
      _spectrumBuffer = calloc(_fftSize/2, sizeof(q15_t));
      _AspectrumBuffer = calloc(_fftSize/2,sizeof(q15_t));
      
      //Allocate frecuency dB buffers
      _spectrumBufferDB = calloc(_fftSize/2, sizeof(q15_t));
      _AspectrumBufferDB = calloc(_fftSize/2,sizeof(q15_t));
    } else {
      //Allocate time buffer
      _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
      _sampleBufferWin = calloc(_bufferSize, sizeof(q31_t));
      
      //Allocate frequency buffers
      _fftBuffer = calloc(_fftSize, sizeof(q31_t));
      _spectrumBuffer = calloc(_fftSize/2, sizeof(q31_t));
      _AspectrumBuffer = calloc(_fftSize/2,sizeof(q31_t));

      //Allocate frecuency dB buffers
      _spectrumBufferDB = calloc(_fftSize/2, sizeof(q31_t));
      _AspectrumBufferDB = calloc(_fftSize/2,sizeof(q31_t));
    }

    //Free all buffers in case of bad allocation
    if (_sampleBuffer == NULL || _sampleBufferWin == NULL || _fftBuffer == NULL || _spectrumBuffer == NULL || _AspectrumBuffer == NULL || _spectrumBufferDB == NULL || _AspectrumBufferDB == NULL) {

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_sampleBufferWin) {
        free(_sampleBufferWin);
        _sampleBufferWin = NULL;
      }

      if (_fftBuffer) {
        free(_fftBuffer);
        _fftBuffer = NULL;
      }

      if (_spectrumBuffer) {
        free(_spectrumBuffer);
        _spectrumBuffer = NULL;
      }

      if (_AspectrumBuffer) {
        free(_AspectrumBuffer);
        _AspectrumBuffer = NULL;
      }

      if (_spectrumBufferDB) {
        free(_spectrumBufferDB);
        _spectrumBufferDB = NULL;
      }

      if (_AspectrumBufferDB) {
        free(_AspectrumBufferDB);
        _AspectrumBufferDB = NULL;
      }

      return 0;
    }
    SerialPrint("Configure Successful",6,true);  
    return 1;
}

double AudioI2S_SCK::AudioSpectrumRead(int spectrum[], int Aspectrum [],int spectrumDB[], int AspectrumDB[], int fftSize){
  
  if (!_SpectrumAvailable) {
    return 0;
  }

  // Get buffer (currently hardcoded)
  GetBuffer();

  // Downscale the sample buffer for proper functioning
  DownScaling(_sampleBuffer, _bufferSize, CONST_FACTOR);

  // Apply Hann Window
  Window();
  
  // FFT - EQUALIZATION and A-WEIGHTING
  FFT();
  EQUALIZING();
  A_WEIGHTING();

  // RMS CALCULATION  
  _rms_specB = RMSG(_spectrumBuffer, _fftSize/2, 2); 
  _rms_AspecB = RMSG(_AspectrumBuffer, _fftSize/2, 2); 
  
  _rms_specBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_specB)); 
  _rms_AspecBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_AspecB)); 
 
  // UPSCALING THE BUFFERS
  UpScaling(_sampleBuffer,_bufferSize,CONST_FACTOR);
  UpScaling(_sampleBufferWin,_bufferSize,CONST_FACTOR);
  UpScaling(_spectrumBuffer,fftSize/2,CONST_FACTOR);
  UpScaling(_AspectrumBuffer,fftSize/2,CONST_FACTOR);

  // CONVERT 2 DB THE BUFFERS
  Convert2DB(_spectrumBuffer, _spectrumBufferDB, _fftSize/2);
  Convert2DB(_AspectrumBuffer, _AspectrumBufferDB, _fftSize/2);

  // COPY SPECTRUMS TO MAIN
  memcpy(spectrum, _spectrumBuffer, sizeof(int) * _fftSize/2);
  memcpy(Aspectrum, _AspectrumBuffer, sizeof(int) * _fftSize/2);
  memcpy(spectrumDB, _spectrumBufferDB, sizeof(int) * _fftSize/2);
  memcpy(AspectrumDB, _AspectrumBufferDB, sizeof(int) * _fftSize/2);
  
  // Set available to 0 to wait for new process
  _SpectrumAvailable = 0;

  return _rms_AspecBDB; 
} 

double AudioI2S_SCK::AudioRMSRead_dB(){
  return _rms_specBDB;
}

void AudioI2S_SCK::GetBuffer(){

  //Get the hardcoded buffer
  //buffer is already treated (usable samples, averaged)
  q31_t* dstG = (q31_t*)_sampleBuffer;
  for (int i = 0; i < _bufferSize; i ++) {
    int value = buffer[i];
    *dstG = value;
    dstG++;
  }
}

void AudioI2S_SCK::Window(){
  const q31_t* srcW = (const q31_t*)_sampleBuffer;
  q31_t* dstW = (q31_t*)_sampleBufferWin;

  //Apply hann window in time-domain
  for (int i = 0; i < _bufferSize; i ++) {
    double window = HANN[i];

    *dstW = (*srcW) * window;
    //SerialPrint(String(i) + "\t" + String(*srcW) + "\t" +  String(window) + "\t" +  String(*dstW),6,true);
    dstW++;
    srcW++;
  }
}

void AudioI2S_SCK::FFT(){
  //_sampleBufferWin is already treated for FFT (usable samples, averaged, windowed)

  /*
    // Print Time domain signal
    int32_t* sBprint = (int32_t*)_sampleBuffer;
    for (int i = 0; i < _bufferSize; i ++) {
      SerialPrint(String(i) + "\t" + String(*sBprint),6,true);
      sBprint++;
    }
  */
  
  // Calculate FFTBuffer ((r-i,r-i...))
  arm_rfft_q31(&_S31, (q31_t*)_sampleBufferWin, (q31_t*)_fftBuffer);

  //Calculate spectrumBuffer and normalize
  
  const q31_t* _pfftBuffer = (const q31_t*)_fftBuffer;
  q31_t* _pspectrumBuffer = (q31_t*) _spectrumBuffer;

  for (int i = 0; i < _fftSize; i +=2) {

    SerialPrint(String(i) + "\t",6,false);
    
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
  SerialPrint("Finish time for module calculation\t"+String(millis()),6,true);
}

void AudioI2S_SCK::EQUALIZING(){
  //Get spectrumBuffer
  q31_t* spBE = (q31_t*)_spectrumBuffer;

  for (int i = 0; i < _fftSize/2; i ++) {
    
    //Deconvolution of the spectrumBuffer by division of the microphone frequency response
    double equalfactor = EQUALTAB[i];
    *spBE /= equalfactor;
    spBE++;
  }
}

void AudioI2S_SCK::A_WEIGHTING(){
  //Apply a-weighting to spectrumBuffer
  const q31_t* spBA = (const q31_t*)_spectrumBuffer;
  q31_t* AspBA = (q31_t*)_AspectrumBuffer;

  for (int i = 0; i < _fftSize/2; i ++) {
    
    //Apply a-weighting
    double weighingfactor = WEIGHTINGTAB[i];
    *AspBA = weighingfactor * (*spBA);

    spBA++;
    AspBA++;
  }
}

void AudioI2S_SCK::UpScaling(void *vector, int vectorSize, int factor){
  // UPSCALE signal by factor
    q31_t* _vectUP = (q31_t*) vector;

    for (int i = 0; i<vectorSize;i++){
      *_vectUP *= factor;
      _vectUP++;
    }
}

void AudioI2S_SCK::DownScaling(void *vector, int vectorSize, int factor){
    // DOWNSCALE signal by factor
    q31_t* _vectDW = (q31_t*) vector;

    for (int i = 0; i<vectorSize;i++){
      *_vectDW /= factor;
      _vectDW++;
    }
}

double AudioI2S_SCK::RMSG(void *inputBuffer, int inputSize, int typeRMS){ 
  //typeRMS = 1 if time domain -- typeRMS = 2 if spectrum domain
  double _rmsOut = 0;
  const q31_t* _pBuffer = (const q31_t*) inputBuffer; 
 
  for (int i = 0; i < inputSize; i ++) { 
    _rmsOut += (*_pBuffer) * (*_pBuffer); 
    _pBuffer++; 
  } 
  _rmsOut = sqrt(_rmsOut)/sqrt(inputSize); 
  
  switch (typeRMS) {
    case 1: //TIME DOMAIN SIGNAL
      _rmsOut = _rmsOut * 1/RMS_HANN * CONST_FACTOR; 
      break;
    case 2: //SPECTRUM IN FREQ DOMAIN
      _rmsOut = _rmsOut * 1/RMS_HANN * CONST_FACTOR * sqrt(inputSize) / sqrt(2); 
      break;
  }
  
  return _rmsOut;
 
  //_RMSAvailable = 1; 
 } 

void AudioI2S_SCK::Convert2DB(void *vectorSource, void *vectorDest, int vectorSize){

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
    //_SpectrumAvailable = 1;
}

void AudioI2S_SCK::SerialPrint(String ToPrint, int PrioFac, bool NewLine){
  // ONLY FOR PRINTING
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}