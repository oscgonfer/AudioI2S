#include <Arduino.h>
#include <InterfaceI2S.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>
#define __FPU_PRESENT 0

#include "ConstantsSound.h"

typedef enum{
  FIRAnalysis,
  FFTAnalysis
} analysis_type;

//CLASS
class AudioAnalysis
{
public:
  AudioAnalysis(int sampleRate, int bufferSize, int channels, int bitsPerSample); //
  virtual ~AudioAnalysis(); //

  int SetAnalysis(int AnalysisType);
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int available();

protected:
  friend class FFTAnalysis;
  friend class FIRAnalysis;

  virtual void GetBuffer();
  virtual double RMSG(void *inputBuffer, int inputSize, int typeRMS, int factor);
  virtual void Scaling(void *vector, int vectorSize, double factor, bool multDiv);
  virtual void Convert2DB(void *vectorSource, void *vectorDest, int vectorSize);
private:
  //PARAMETERS
  int _AnalysisType;
  int _bufferSize; //Already usable bufferSize
  int _bitsPerSample;
  int _channels;
  int _sampleRate;
};
