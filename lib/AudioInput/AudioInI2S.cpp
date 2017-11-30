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

bool AudioInI2S::begin(long sampleRate, int bitsPerSample)
{
  if (!I2S_SCK.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample)) {
    return 0;
  }

  _sampleRate = sampleRate;
  _bitsPerSample = bitsPerSample;

  // add the receiver callback
  //I2S_SCK.onReceive(AudioInI2S::onI2SReceive);

  int _delay = 263000;
  //Initialisation
  for (int i = 0; i< _delay; i++) {
      // Trigger a read to kick things off
      I2S_SCK.read();
  }

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

bool AudioInI2S::bufferI2SAvailable(){

  return _bufferI2SAvailable;
}

bool AudioInI2S::readBuffer(void *buffer, int bufferSize) {
  int32_t sample = 0;
  int32_t counter = 0;
  int32_t *buff = (int32_t*) buffer;

  //if (_bufferI2SAvailable == true) {

    //FILL BUFFER HERE
    while (counter < bufferSize) {
      sample = I2S_SCK.read();
      if (sample) {
        *buff = sample;
        buff++;
        counter++;
      }
    }

    return true;

  /*} else {
    return false;
  }*/

  //_bufferI2SAvailable = false;
}

void AudioInI2S::onReceive()
{
  //I2S_SCK.read();
  _bufferI2SAvailable = true;
}

void AudioInI2S::onI2SReceive()
{
  audioInI2SObject.onReceive();
}

AudioInI2S audioInI2SObject;
