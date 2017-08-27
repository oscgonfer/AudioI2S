#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>

#include "ConstantsSound.h"
#include "RawData.h"

class AudioI2S_SCK
{
public:
  AudioI2S_SCK(uint32_t fftSize); //
  virtual ~AudioI2S_SCK(); //

  int Configure(int bitsPerSample,int channels, int bufferSize, int sampleRate);
  double AudioSpectrumRead(int spectrum[], int Aspectrum [], int spectrumDB[], int AspectrumDB[], int fftSize);
  double AudioTimeFilter();
  double AudioRMSRead_dB();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int available();

protected:
  virtual void GetBuffer();
  virtual void Window();
  virtual void FilterConv();
  virtual void FFT();
  virtual void A_WEIGHTING();
  virtual double RMSG(void *inputBuffer, int inputSize, int typeRMS);
  virtual void UpScaling(void *vector, int vectorSize, int factor);
  virtual void DownScaling(void *vector, int vectorSize, int factor);
  virtual void EQUALIZING();
  virtual void Convert2DB(void *vectorSource, void *vectorDest, int vectorSize);

private:
  //BUFFER Sizes
  uint32_t _fftSize;
  int _bufferSize; //Already usable bufferSize
  //PARAMETERS
  int _bitsPerSample;
  int _channels;
  int _sampleRate;
  //RMS Results
  double _rms_time;
  double _rms_specB;
  double _rms_AspecB;
  double _rms_timeDB;
  double _rms_specBDB;
  double _rms_AspecBDB;
  double _rmsFilterA;
  double _rmsFilterADB;
  //BUFFERS
  void* _sampleBuffer;
  void* _sampleBufferWin;
  void* _fftBuffer;
  void* _spectrumBuffer;
  void* _AspectrumBuffer;
  void* _fftBufferDB;
  void* _spectrumBufferDB;
  void* _AspectrumBufferDB;
  void* _filterBufferR;
  void* _filterBufferI;
  void* _sampleBufferFilt;
  //EXTRAS
  int _SpectrumAvailable;
  int _RMSAvailable;
  int _bufferAvailable;
  int _available;

  arm_rfft_instance_q15 _S15;
  arm_rfft_instance_q31 _S31;
};
