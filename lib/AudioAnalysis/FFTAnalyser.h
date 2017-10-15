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
  FFTAnalyser(int fftSize, bool SpectrumDBOutput, int type_RMS, bool ScalingOutput); //
  virtual ~FFTAnalyser(); //

  //int Configure(int bufferSize, bool SpectrumDBOutput);
  double AudioSpectrumRead(int spectrum[], int spectrumDB[]);
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int Available();
  int bufferAvailable();
  double UPDATE_TIME();
  double READ_TIME();

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
  int _typeWeight;
  bool _SpectrumDBOutput;
  bool _scalingOutput;
  //RMS Results
  double _rms_specB;
  double _rms_specBDB;
  //BUFFERS
  void* _sampleBuffer;
    //FFT
    void* _fftBuffer;
    void* _spectrumBuffer;
    void* _AspectrumBuffer;
    void* _spectrumBufferDB;
    void* _AspectrumBufferDB;  
  //EXTRAS
  int _SpectrumAvailable;
  int _bufferAvailable;
  double _update_called;
  double _read_called;
  //FFT
  arm_rfft_instance_q31 _S31;
};

#endif