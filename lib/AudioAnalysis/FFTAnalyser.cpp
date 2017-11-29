#include "FFTAnalyser.h"
#include "AudioIn.h"

FFTAnalyser::FFTAnalyser(int fftSize, bool SpectrumDBOutput, int typeWeight, bool ScalingOutput) :
  //BUFFER Sizes
  _fftSize(fftSize),
  //BUFFERs
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  _AspectrumBuffer(NULL),
  _spectrumBufferDB(NULL),
  _AspectrumBufferDB(NULL),
  //RMS
  _rms_specB(0),
  _rms_specBDB(0),
  //EXTRAS
  _SpectrumAvailable(0),
  _bufferAvailable(0),
  _bitsPerSample(0),
  _channels(0),
  _bufferSize(0),
  _sampleRate(0),
  _typeWeight(typeWeight),
  _scalingOutput(ScalingOutput),
  _update_called(0),
  _read_called(0),
  _fftDone(0),
  _SpectrumDBOutput(SpectrumDBOutput)
{
}

FFTAnalyser::~FFTAnalyser()
{
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

int FFTAnalyser::Available(){
  return _SpectrumAvailable;
}

int FFTAnalyser::bufferAvailable(){
  return _bufferAvailable;
}

double FFTAnalyser::UPDATE_TIME(){
  return _update_called;
}

double FFTAnalyser::READ_TIME(){
  return _read_called;
}

double FFTAnalyser::AudioSpectrumRead(int spectrum[], int spectrumDB[]){
  double read_called = millis();
  _read_called = read_called;

  if (!_SpectrumAvailable) {
    return 0;
  }
  /*
  switch (_typeWeight) {
    case 0: //Z_WEIGHTING
      _rms_specB = RMSG(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 

      if (_scalingOutput) {
        Scaling(_spectrumBuffer,_fftSize/2,CONST_FACTOR,true);
      } 

      memcpy(spectrum, _spectrumBuffer, sizeof(int) * _fftSize/2);

      if (_SpectrumDBOutput){
        // CONVERT 2 DB THE BUFFERS
        Convert2DB(_spectrumBuffer, _spectrumBufferDB, _fftSize/2);
        // COPY SPECTRUMS TO MAIN
        memcpy(spectrumDB, _spectrumBufferDB, sizeof(int) * _fftSize/2);
      }
      break;

    case 1: //A_WEIGHTING
      A_WEIGHTING(_spectrumBuffer, _AspectrumBuffer, _fftSize/2);
      _rms_specB = RMSG(_AspectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 

      if (_scalingOutput) {
        Scaling(_AspectrumBuffer,_fftSize/2,CONST_FACTOR,true);
      } 

      memcpy(spectrum, _AspectrumBuffer, sizeof(int) * _fftSize/2);

      if (_SpectrumDBOutput){
        // CONVERT 2 DB THE BUFFERS
        Convert2DB(_AspectrumBuffer, _AspectrumBufferDB, _fftSize/2);
        // COPY SPECTRUMS TO MAIN
        memcpy(spectrumDB, _AspectrumBufferDB, sizeof(int) * _fftSize/2);
      }
      break;
  }
  
  if (_scalingOutput) {
    // UPSCALING THE BUFFERS
    Scaling(_sampleBuffer,_bufferSize,CONST_FACTOR,true);
  }
  */
  _SpectrumAvailable = 0;
  _bufferAvailable = 0;
  
  //_rms_specBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_specB));

  return _rms_specBDB; 
}

int FFTAnalyser::Configure(AudioIn* input){

    _bitsPerSample = input->bitsPerSample();
    _channels = input->channels();

    //Initialize fft
    if (ARM_MATH_SUCCESS != arm_rfft_init_q31(&_S31, _fftSize, 0, 1)) {
      return 0;
    }

    //Allocate memory for buffers
      //Allocate time buffer
      _bufferSize = _fftSize; //ONLY because of how the ARM_FFT function works
      _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
      _fftBuffer = calloc(_fftSize, sizeof(q31_t));
      //Allocate frequency buffers
      _spectrumBuffer = calloc(_fftSize/2, sizeof(q31_t));
      if (_typeWeight == 1) {
        _AspectrumBuffer = calloc(_fftSize/2,sizeof(q31_t));
      }

      if (_SpectrumDBOutput){
        //Allocate frecuency dB buffers
        if (_typeWeight==0){
           _spectrumBufferDB = calloc(_fftSize/2, sizeof(q31_t));
        }
        if (_typeWeight == 1) {
          _AspectrumBufferDB = calloc(_fftSize/2,sizeof(q31_t));
        }
      }
      
    //Free all buffers in case of bad allocation
    if (_sampleBuffer == NULL || _spectrumBuffer == NULL || _fftBuffer == NULL) {

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_spectrumBuffer) {
        free(_spectrumBuffer);
        _spectrumBuffer = NULL;
      }

      if (_fftBuffer) {
        free(_fftBuffer);
        _fftBuffer = NULL;
      }

      if (_SpectrumDBOutput){
        if (_spectrumBufferDB) {
          free(_spectrumBufferDB);
          _spectrumBufferDB = NULL;
        }
      }
      return 0;
    }
    return 1;
}

void FFTAnalyser::Update(const void*buffer, size_t bufferUpdateSize){

  double update_called = millis();

  _update_called = update_called;
  uint8_t* newSamples = ((uint8_t*)_sampleBuffer);
  int samples = bufferUpdateSize / (_bitsPerSample / 8);

  const int32_t *src = (const int32_t*)buffer;
  int32_t* dst = (int32_t*)newSamples;

  for (int i = 0; i < samples; i += 2) {
    *dst = *src / 2;
    src++;
    *dst += *src / 2;
    *dst/=64; //CORRECT THE BIT NUMBER
    src++;
    dst++;
  }
  
  _bufferAvailable = 1;

  // Downscale the sample buffer for proper functioning
  Scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

  // Apply Hann Window
  Window();
  
  // FFT - EQUALIZATION and A-WEIGHTING
  FFT(_sampleBuffer, _spectrumBuffer, _fftSize);
  EQUALIZING(_spectrumBuffer, _fftSize/2);

  // Set available to 0 to wait for new process
  _SpectrumAvailable = 1;
  _fftDone = 1;
  AudioInI2S.fftDone(_fftDone);
}

void FFTAnalyser::FFT(void *_inputBuffer, void* _outputBuffer, int _fftBufferSize){
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

void FFTAnalyser::Window(){
  q31_t* srcW = (q31_t*)_sampleBuffer;

  //Apply hann window in time-domain
  for (int i = 0; i < _bufferSize; i ++) {
    double window = HANN[i];
    (*srcW) *= window;
    srcW++;
  }
}

void FFTAnalyser::EQUALIZING(void *inputBuffer, int inputSize){
  q31_t* spBE = (q31_t*)inputBuffer;

  for (int i = 0; i < inputSize; i ++) {
    //Deconvolution of the spectrumBuffer by division of the microphone frequency response
    double equalfactor = EQUALTAB[i];
    *spBE /= equalfactor;
    spBE++;
  }
}

void FFTAnalyser::A_WEIGHTING(void *inputBuffer, void *outputBuffer, int inputSize){
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

void FFTAnalyser::SerialPrint(String ToPrint, int PrioFac, bool NewLine){
  // ONLY FOR PRINTING
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}