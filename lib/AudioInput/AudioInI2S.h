#ifndef _AUDIO_IN_I2S_CLASS_H_INCLUDED
#define _AUDIO_IN_I2S_CLASS_H_INCLUDED

#include <I2S.h>

class AudioInI2S
{
public:

  AudioInI2S();
  ~AudioInI2S();

  bool begin(long sampleRate, int bitsPerSample);
  void end();

  long sampleRate();
  int bitsPerSample();
  int datasize();
  bool bufferI2SAvailable();
  int readBuffer(void* buffer, int bufferReadSize);

private:

  void onReceive();
  
  static void onI2SReceive();

private:
  //INIT I2S
  long _sampleRate;
  int _bitsPerSample;
  //I2S CALLBACK
  bool _callbackTriggered;
  bool _bufferI2SAvailable;
  //I2S PARAM
  int _datasize;
};

extern AudioInI2S audioInI2SObject;

#endif