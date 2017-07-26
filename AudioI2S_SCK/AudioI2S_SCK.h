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
  int AudioSpectrumRead(int spectrum[], int Aspectrum [], int spectrumDB[], int AspectrumDB[], int fftSize);
  //double AudioRMSRead(double rms_specB, double rms_AspecB, double rms_specBDB, double rms_AspecBDB);
  double AudioRMSRead_dB();
  double AudioRMSRead_dBA();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int available();

protected:
  virtual void GetBuffer();
  virtual void Window();
  virtual void FFT();
  virtual void A_WEIGHTING();
  virtual void RMS();
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
  //BUFFERS
  void* _sampleBuffer;
  void* _sampleBufferWin;
  void* _fftBuffer;
  void* _spectrumBuffer;
  void* _AspectrumBuffer;
  void* _fftBufferDB;
  void* _spectrumBufferDB;
  void* _AspectrumBufferDB;
  //EXTRAS
  String _ToPrint;
  bool _NewLine;
  int _PrioFac;
  int _SpectrumAvailable;
  int _RMSAvailable;
  int _available;

  arm_rfft_instance_q15 _S15;
  arm_rfft_instance_q31 _S31;
};
