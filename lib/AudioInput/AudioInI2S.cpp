#include "AudioInI2S.h"

AudioInI2S::AudioInI2S() :
  _sampleRate(-1),
  _bitsPerSample(-1),
  _callbackTriggered(false),
  _bufferI2SAvailable(true)
{
}

AudioInI2S::~AudioInI2S()
{
}

int AudioInI2S::begin(long sampleRate, int bitsPerSample)
{
  if (!I2S_SCK.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample)) {
    return 0;
  }

  _sampleRate = sampleRate;
  _bitsPerSample = bitsPerSample;

  // add the receiver callback
  I2S_SCK.onReceive(AudioInI2S::onI2SReceive);

  // Trigger a read to kick things off
  I2S_SCK.read();

  return 1;
}

void AudioInI2S::end()
{
  _sampleRate = -1;
  _bitsPerSample = -1;
  _datasize = -1;
  _bufferI2SAvailable = false;
  _callbackTriggered = false;

  I2S_SCK.end();
}

long AudioInI2S::sampleRate()
{
  return _sampleRate;
}

int AudioInI2S::bitsPerSample()
{
  return _bitsPerSample;
}

int AudioInI2S::channels()
{
  return 2;
}

bool AudioInI2S::bufferI2SAvailable(void *buffer, size_t bufferReadSize){

  bool bufferI2SAvailable = _bufferI2SAvailable;

  if (_bufferI2SAvailable == true) {

    I2S_SCK.read(buffer, bufferReadSize);

  }

  _bufferI2SAvailable = false;

  return bufferI2SAvailable;
}

void AudioInI2S::onReceive()
{
  if (_callbackTriggered) {
    _bufferI2SAvailable = true;
  }
}

void AudioInI2S::onI2SReceive()
{
  audioInI2SObject.onReceive();
}

AudioInI2S audioInI2SObject;
