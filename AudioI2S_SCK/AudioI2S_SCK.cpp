/*
  Copyright (c) 2016 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "AudioI2S_SCK.h"

AudioI2S_SCK::AudioI2S_SCK(uint32_t fftSize) :
  //BUFFER Sizes
  _fftSize(fftSize),
  _bufferSize(0), //Already usable buffersize
  //BUFFER
  _sampleBuffer(NULL),
  _sampleBufferWin(NULL),
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  _AspectrumBuffer(NULL),
  _fftBufferDB(NULL),
  _spectrumBufferDB(NULL),
  _AspectrumBufferDB(NULL),
  //RMS
  //_rms_time(0),
  _rms_specB(0),
  _rms_AspecB(0),
  //_rms_timeDB(0),
  _rms_specBDB(0),
  _rms_AspecBDB(0),
  //EXTRAS
  _bitsPerSample(-1),
  _channels(-1),
  _SpectrumAvailable(1),
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
  _available = _SpectrumAvailable*_RMSAvailable;
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

int AudioI2S_SCK::AudioSpectrumRead(int spectrum[], int Aspectrum [],int spectrumDB[], int AspectrumDB[], int fftSize){
  if (!_SpectrumAvailable) {
    return 0;
  }
  SerialPrint("Before GetBuffer time\t" + String(millis()),7,true);

  GetBuffer();
  DownScaling(_sampleBuffer, _bufferSize, CONST_FACTOR);

  SerialPrint("After GetBuffer time\t" + String(millis()),7,true);

  Window();
  FFT();
  EQUALIZING();
  SerialPrint("After FFT time\t" + String(millis()),7,true);
  A_WEIGHTING();

  SerialPrint("After A-w time\t" + String(millis()),7,true);
  RMS();

  UpScaling(_sampleBuffer,_bufferSize,CONST_FACTOR);
  UpScaling(_sampleBufferWin,_bufferSize,CONST_FACTOR);
  UpScaling(_spectrumBuffer,fftSize/2,CONST_FACTOR);
  UpScaling(_AspectrumBuffer,fftSize/2,CONST_FACTOR);

  SerialPrint("After RMS time\t" + String(millis()),7,true);

  Convert2DB(_spectrumBuffer, _spectrumBufferDB, _fftSize/2);
  Convert2DB(_AspectrumBuffer, _AspectrumBufferDB, _fftSize/2);

  //Copy buffers
  memcpy(spectrum, _spectrumBuffer, sizeof(int) * _fftSize/2);
  memcpy(Aspectrum, _AspectrumBuffer, sizeof(int) * _fftSize/2);
  memcpy(spectrumDB, _spectrumBufferDB, sizeof(int) * _fftSize/2);
  memcpy(AspectrumDB, _AspectrumBufferDB, sizeof(int) * _fftSize/2);
  
  //Set available to 0 to wait for new process
  _SpectrumAvailable = 0;

  return 1;
}

int AudioI2S_SCK::AudioRMSRead(int rms_time, int rms_specB,int rms_AspecB, int rms_timeDB, int rms_specBDB, int rms_AspecBDB){
  //Set available to 0 to wait for new process
  // 

  SerialPrint("Full Scale DBFS\t" + String(FULL_SCALE_DBFS),7,true);

  if (!_RMSAvailable) {
    return 0;
  }

  //rms_time = _rms_time;
  //SerialPrint(String(rms_time) + "\t",7,false);

  rms_specB = _rms_specB;
  SerialPrint(String(rms_specB) + "\t",7,false);

  rms_AspecB = _rms_AspecB;
  SerialPrint(String(rms_AspecB) + "\t",7,true);

  //rms_timeDB = _rms_timeDB;
  //SerialPrint(String(rms_timeDB) + "\t",7,false);

  rms_specBDB = _rms_specBDB;
  SerialPrint(String(rms_specBDB) + "\t",7,false); 

  rms_AspecBDB = _rms_AspecBDB;
  SerialPrint(String(rms_AspecBDB) + "\t",7,true);

  _RMSAvailable = 0;

  return 1;
}

void AudioI2S_SCK::GetBuffer(){

  //Get the hardcoded buffer
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
  SerialPrint("Begin time for module calculation\t"+String(millis()),7,true);
  
  const q31_t* _pfftBuffer = (const q31_t*)_fftBuffer;
  q31_t* _pspectrumBuffer = (q31_t*) _spectrumBuffer;

  for (int i = 0; i < _fftSize; i +=2) {

    SerialPrint(String(i) + "\t",6,false);
    
    //SerialPrint(String(*_pfftBuffer) + "\t",7,false);
    
    *_pspectrumBuffer = (*_pfftBuffer) * (*_pfftBuffer);
    _pfftBuffer++;
    
    //SerialPrint(String(*_pfftBuffer) + "\t",7,false);
    
    *_pspectrumBuffer += (*_pfftBuffer) * (*_pfftBuffer);
    *_pspectrumBuffer = sqrt(*_pspectrumBuffer);

    //Normalize SpectrumBuffer
    if (i) {
      *_pspectrumBuffer = 2 * (*_pspectrumBuffer);
    }

    //*_pspectrumBuffer = CONST_FACTOR * (*_pspectrumBuffer);
    
    SerialPrint(String(*_pspectrumBuffer),6,true);
    _pfftBuffer++;
    _pspectrumBuffer++;
  }
  SerialPrint("Finish time for module calculation\t"+String(millis()),6,true);
    
  /*
    //Calculate spectrumBuffer (r,r,r,...)
    //arm_cmplx_mag_q31((q31_t*)_fftBuffer, (q31_t*) _spectrumBuffer, _fftSize);

  
    SerialPrint("_spectrumBuffer print",7,true);
    //Normalize spectrumBuffer
    q31_t* spBF = (q31_t*)_spectrumBuffer;
    for (int i = 0; i < _fftSize/2; i ++) {
    SerialPrint(String(i) + "\t" + String(*spBF),7,false);
    //Multiply by 2 every component except the DC (1st)
    if (i) {
      *spBF = 2*(*spBF);
    }
    SerialPrint( "\t" + String(*spBF),7,true);
    spBF++;

    }
  */  
}

void AudioI2S_SCK::EQUALIZING(){
  //Normalize spectrumBuffer
  q31_t* spBE = (q31_t*)_spectrumBuffer;

  for (int i = 0; i < _fftSize/2; i ++) {
    
    //Multiply by 2 every component except the DC (1st)
    double equalfactor = EQUALTAB[i];
    *spBE /= equalfactor;

    spBE++;
  }
}

void AudioI2S_SCK::A_WEIGHTING(){
  //Normalize spectrumBuffer
  const q31_t* spBA = (const q31_t*)_spectrumBuffer;
  q31_t* AspBA = (q31_t*)_AspectrumBuffer;

  for (int i = 0; i < _fftSize/2; i ++) {
    
    //Multiply by 2 every component except the DC (1st)
    double weighingfactor = WEIGHTINGTAB[i];
    *AspBA = weighingfactor * (*spBA);

    spBA++;
    AspBA++;
  }
}

void AudioI2S_SCK::UpScaling(void *vector, int vectorSize, int factor){
    q31_t* _vectUP = (q31_t*) vector;

    for (int i = 0; i<vectorSize;i++){
      *_vectUP *= factor;
      _vectUP++;
    }
}

void AudioI2S_SCK::DownScaling(void *vector, int vectorSize, int factor){
    q31_t* _vectDW = (q31_t*) vector;

    for (int i = 0; i<vectorSize;i++){
      *_vectDW /= factor;
      _vectDW++;
    }
}

void AudioI2S_SCK::RMS(){
/*
  //TIME DOMAIN
  //arm_rms_q31((q31_t*)_sampleBufferWin,_bufferSize,(q31_t*)&_rms_time);
  const q31_t* _prsampleBufferWin = (const q31_t*) _sampleBufferWin;

  for (int i = 0; i < _bufferSize; i ++) {
    _rms_time += *_prsampleBufferWin * *_prsampleBufferWin;
    _prsampleBufferWin++;
  }
  _rms_time = sqrt(_rms_time)/sqrt(_bufferSize);
  _rms_time = _rms_time * 1/RMS_HANN * CONST_FACTOR;
*/

  //FREQ DOMAIN
  //arm_rms_q31((q31_t*)_spectrumBuffer,_fftSize/2, (q31_t*)&_rms_specB);
  const q31_t* _prspecBuffer = (const q31_t*) _spectrumBuffer;

  for (int i = 0; i < _fftSize/2; i ++) {
    _rms_specB += *_prspecBuffer * *_prspecBuffer;
    _prspecBuffer++;
  }
  _rms_specB = sqrt(_rms_specB)/sqrt(_fftSize/2);
  _rms_specB = _rms_specB * 1/RMS_HANN * CONST_FACTOR * sqrt(_fftSize/2) / sqrt(2);

  //arm_rms_q31((q31_t*)_AspectrumBuffer,_fftSize/2, (q31_t*)&_rms_AspecB);
  const q31_t* _prAspecBuffer = (const q31_t*) _AspectrumBuffer;

  for (int i = 0; i < _fftSize/2; i ++) {
    _rms_AspecB += *_prAspecBuffer * *_prAspecBuffer;
    _prAspecBuffer++;
  }
  _rms_AspecB = sqrt(_rms_AspecB)/sqrt(_fftSize/2);
  _rms_AspecB =  _rms_AspecB * 1/RMS_HANN * CONST_FACTOR * sqrt(_fftSize/2) / sqrt(2);

  _rms_timeDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_time));
  _rms_specBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_specB));
  _rms_AspecBDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rms_AspecB));

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

//ONLY FOR PRINTING 
void AudioI2S_SCK::SerialPrint(String ToPrint, int PrioFac, bool NewLine)
{
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}