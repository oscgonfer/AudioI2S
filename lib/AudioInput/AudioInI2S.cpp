#include "AudioInI2S.h"

AudioInI2S::AudioInI2S() :
  _sampleRate(-1),
  _bitsPerSample(-1)
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

int AudioInI2S::readBuffer(void *buffer, int bufferSize) {
  int32_t sample = 0;
  int32_t counter = 0;
  int32_t *buff = (int32_t*) buffer;

  //FILL BUFFER HERE
  while (counter < bufferSize) {
    sample = I2S.read();
    if (sample) {
      *buff = sample>>7; //BIT CORRRECTION (/128)
      buff++;
      counter++;
    }
  }

  return counter;
}


AudioInI2S audioInI2SObject;
