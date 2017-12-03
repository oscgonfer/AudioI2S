#ifndef _FFT_ANALYSER_H_INCLUDED
#define _FFT_ANALYSER_H_INCLUDED

#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>

#include "EqualisationCoefficients/equalisation_coeffs.h"
#include "AudioAnalyser.h"
#include "ConstantsSound.h"

//CLASS
class FFTAnalyser : public AudioAnalyser
{
public:

  FFTAnalyser(int bufferSize, int fftSize, WeightingType weightingType, WindowType windowType); //
  ~FFTAnalyser(); //

  //int Configure(int bufferSize, bool SpectrumDBOutput);
  double sensorRead(int spectrum[]);
  double sensorRead();
  bool configure(AudioInI2S& input);

private:
  void fft(void *inputBuffer, void* outputBuffer, int fftBufferSize);
  void equalising(void *inputBuffer, void *equaliserTable, int inputSize);
  void weighting(void *inputBuffer, int inputSize);
  void createWeighting(void *weightingTable, long sampleRate);
  void createEqualiser(void *equaliserTable);

  //BUFFER Sizes
  int _fftSize;
  int _bufferSize; //Already usable bufferSize
  //PARAMETERS
  int _bitsPerSample;
  int _sampleRate;
  WeightingType _weighting_type;
  WindowType _window_type;
  //RMS Results
  double _rms;
  double _rmsDB;
  //BUFFERS
  void* _sampleBuffer;
  void* _fftBuffer;
  void* _spectrumBuffer;
  void* _weightingTable;
  void* _windowTable;
  void* _equaliserTable;
  //FFT
  arm_rfft_instance_q31 _S31;
};

#endif