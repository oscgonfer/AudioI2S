
#ifndef _AUDIO_ANALYZER_H_INCLUDED
#define _AUDIO_ANALYZER_H_INCLUDED

#include <Arduino.h>


#define ARM_MATH_CM0PLUS
#include <arm_math.h>
#include <stddef.h>

#include "AudioIn.h"
#include "ConstantsSound.h"

//CLASS
class AudioAnalyser
{
public:
  int input(AudioIn& input);

  double RMSG(void *inputBuffer, int inputSize, int typeRMS, int factor);
  void Scaling(void *vector, int vectorSize, double factor, bool multDiv);
  void Convert2DB(void *vectorSource, void *vectorDest, int vectorSize);
  void SerialPrint(String _to_print, int _prio_fac, bool _line);

protected:
  friend class AudioIn;

  virtual int Configure(AudioIn* input) = 0;
  virtual void Update (const void* buffer, size_t bufferUpdateSize) = 0;
};

#endif