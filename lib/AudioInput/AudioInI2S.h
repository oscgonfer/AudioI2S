#ifndef _AUDIO_IN_I2S_CLASS_H_INCLUDED
#define _AUDIO_IN_I2S_CLASS_H_INCLUDED

#include "I2S.h"

class AudioInI2S
{
public:

  AudioInI2S();
  ~AudioInI2S();

  bool begin(long sampleRate, int bitsPerSample);
  void end();
  bool readBuffer(void* buffer, int bufferReadSize);
};

extern AudioInI2S audioInI2SObject;

#endif