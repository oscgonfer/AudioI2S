#ifndef _FFT_ANALYSER_H_INCLUDED
#define _FFT_ANALYSER_H_INCLUDED

#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>

#include <AudioInI2S.h>

#include "AudioAnalyser.h"
#include "ConstantsSound.h"

//CLASS
class FFTAnalyser : public AudioAnalyser
{
public:
  FFTAnalyser(int fftSize, bool SpectrumDBOutput); //
  virtual ~FFTAnalyser(); //

  //int Configure(int bufferSize, bool SpectrumDBOutput);
  double AudioSpectrumRead(int spectrum[], int Aspectrum [], int spectrumDB[], int AspectrumDB[]);
  double AudioRMSRead_dB();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int Available();

protected:
  virtual int Configure(AudioIn* input);
  virtual void Update(const void* buffer, size_t bufferReadSize);

  virtual void Window();
  virtual void FFT(void *inputBuffer, void* outputBuffer, int fftBufferSize);
  virtual void EQUALIZING(void *inputBuffer, int inputSize);
  virtual void A_WEIGHTING(void *inputBuffer, void* outputBuffer, int inputSize);

private:
  //BUFFER Sizes
  uint32_t _fftSize;
  int _bufferSize; //Already usable bufferSize
  //PARAMETERS
  int _bitsPerSample;
  int _channels;
  int _sampleRate;
  bool _SpectrumDBOutput;
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
    //FFT
    void* _fftBuffer;
    void* _spectrumBuffer;
    void* _AspectrumBuffer;
    void* _spectrumBufferDB;
    void* _AspectrumBufferDB;  
  //EXTRAS
  int _SpectrumAvailable;
  int _RMSAvailable;
  int _bufferAvailable;
  int _available;
  //FFT
  arm_rfft_instance_q31 _S31;
};

#endif