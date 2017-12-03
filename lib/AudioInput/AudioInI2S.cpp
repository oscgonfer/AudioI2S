#include "AudioInI2S.h"

AudioInI2S::AudioInI2S() :
  _sampleRate(-1),
  _bitsPerSample(-1),
  _callbackTriggered(false),
  _bufferI2SAvailable(false)
{
}

AudioInI2S::~AudioInI2S()
{
}

bool AudioInI2S::begin(long sampleRate, int bitsPerSample)
{
  if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample)) {
    return false;
  }

  _sampleRate = sampleRate;
  _bitsPerSample = bitsPerSample;

  // add the receiver callback
  //I2S.onReceive(AudioInI2S::onI2SReceive);

  //Initialisation 
  int _delay = 263000;
  
  for (int i = 0; i< _delay; i++) {
      // Trigger a read to kick things off
      I2S.read();
  }

  return true;
}

void AudioInI2S::end()
{
  _sampleRate = -1;
  _bitsPerSample = -1;
  _datasize = -1;
  _bufferI2SAvailable = false;
  _callbackTriggered = false;

  I2S.end();
}

long AudioInI2S::sampleRate()
{
  return _sampleRate;
}

int AudioInI2S::bitsPerSample()
{
  return _bitsPerSample;
}

bool AudioInI2S::bufferI2SAvailable(){

  return _bufferI2SAvailable;
}

int AudioInI2S::readBuffer(void *buffer, int bufferSize) {
  int32_t sample = 0;
  int32_t counter = 0;
  int32_t *buff = (int32_t*) buffer;

  //FILL BUFFER HERE
  while (counter < bufferSize) {
    sample = I2S.read();
    if (sample) {
      *buff = sample/128; //BIT CORRRECTION
      buff++;
      counter++;
    }
  }

  _bufferI2SAvailable = false;

  return counter;
}

void AudioInI2S::onReceive()
{
  _bufferI2SAvailable = true;
}

void AudioInI2S::onI2SReceive()
{
  audioInI2SObject.onReceive();
}

AudioInI2S audioInI2SObject;
