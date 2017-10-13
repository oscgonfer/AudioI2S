#ifndef _AUDIO_IN_I2S_CLASS_H_INCLUDED
#define _AUDIO_IN_I2S_CLASS_H_INCLUDED

#include "AudioIn.h"
#include <I2S_SCK.h>
#include "ConstantsSound.h"

class AudioInI2SClass : public AudioIn
{
public:
  AudioInI2SClass();
  virtual ~AudioInI2SClass();

  int begin(long sampleRate, int bitsPerSample, int bufferReadSize);
  virtual void end();

  virtual long sampleRate();
  virtual int bitsPerSample();
  virtual int channels();
  virtual int datasize();

protected:
  virtual int begin();
  virtual int read(void* buffer, size_t size);
  virtual int reset();

private:
  void onReceive();

  static void onI2SReceive();

private:
  long _sampleRate;
  int _bitsPerSample;
  bool _callbackTriggered;
  int _bufferReadSize;
  int _datasize;

};

extern AudioInI2SClass AudioInI2S;

#endif