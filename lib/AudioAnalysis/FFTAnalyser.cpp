#include "FFTAnalyser.h"
#include "AudioIn.h"

FFTAnalyser::FFTAnalyser(int fftSize, bool SpectrumDBOutput) :
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
  _rms_AspecB(0),
  _rms_specBDB(0),
  _rms_AspecBDB(0),
  //EXTRAS
  _SpectrumAvailable(0),
  _bitsPerSample(0),
  _channels(0),
  _bufferSize(0),
  _sampleRate(0),
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

double FFTAnalyser::AudioSpectrumRead(int spectrum[], int Aspectrum [],int spectrumDB[], int AspectrumDB[]){
  
  if (!_SpectrumAvailable) {
    return 0;
  }

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

  _SpectrumAvailable = 0;

  return _rms_AspecBDB; 
}

double FFTAnalyser::AudioRMSRead_dB(){
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
      _bufferSize = _fftSize; //ONLY because the ARM_FFT function works
      _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
      _sampleBufferWin = calloc(_bufferSize, sizeof(q31_t));
      _fftBuffer = calloc(_fftSize, sizeof(q31_t));
      //Allocate frequency buffers
      _spectrumBuffer = calloc(_fftSize/2, sizeof(q31_t));
      _AspectrumBuffer = calloc(_fftSize/2,sizeof(q31_t));

      if (_SpectrumDBOutput){
        //Allocate frecuency dB buffers
        _spectrumBufferDB = calloc(_fftSize/2, sizeof(q31_t));
        _AspectrumBufferDB = calloc(_fftSize/2,sizeof(q31_t));
      }
      

    //Free all buffers in case of bad allocation
    if (_sampleBuffer == NULL || _sampleBufferWin == NULL || _spectrumBuffer == NULL || _AspectrumBuffer == NULL) {

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_sampleBufferWin) {
        free(_sampleBufferWin);
        _sampleBufferWin = NULL;
      }

      if (_spectrumBuffer) {
        free(_spectrumBuffer);
        _spectrumBuffer = NULL;
      }

      if (_AspectrumBuffer) {
        free(_AspectrumBuffer);
        _AspectrumBuffer = NULL;
      }
      if (_SpectrumDBOutput){
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

void FFTAnalyser::Update(const void*buffer, size_t bufferUpdateSize){

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
  
  // Downscale the sample buffer for proper functioning
  Scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

  // Apply Hann Window
  Window();
  
  // FFT - EQUALIZATION and A-WEIGHTING
  FFT(_sampleBufferWin, _spectrumBuffer, _fftSize);
  EQUALIZING(_spectrumBuffer, _fftSize/2);
  A_WEIGHTING(_spectrumBuffer, _AspectrumBuffer, _fftSize/2);

  q31_t* printW = (q31_t*)_spectrumBuffer;
  q31_t* printWA = (q31_t*)_AspectrumBuffer;

  /*
  //Apply hann window in time-domain
  for (int i = 0; i < _fftSize/2; i ++) {
    SerialPrint(String(i)+"\t"+String(*printW) +"\t"+String(*printWA), 6, true);
    printW++;
    printWA++;
  }*/

  // RMS CALCULATION  
  _rms_specB = RMSG(_spectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
  _rms_AspecB = RMSG(_AspectrumBuffer, _fftSize/2, 2, CONST_FACTOR); 
  
  _rms_specBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_specB)); 
  _rms_AspecBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_AspecB)); 
 
  // UPSCALING THE BUFFERS
  Scaling(_sampleBuffer,_bufferSize,CONST_FACTOR,true);
  Scaling(_spectrumBuffer,_fftSize/2,CONST_FACTOR,true);
  Scaling(_AspectrumBuffer,_fftSize/2,CONST_FACTOR,true);

  // Set available to 0 to wait for new process
  _SpectrumAvailable = 1;

  interrupts();
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
  const q31_t* srcW = (const q31_t*)_sampleBuffer;
  q31_t* dstW = (q31_t*)_sampleBufferWin;

  //Apply hann window in time-domain
  for (int i = 0; i < _bufferSize; i ++) {
    double window = HANN[i];
    *dstW = (*srcW) * window;
    dstW++;
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