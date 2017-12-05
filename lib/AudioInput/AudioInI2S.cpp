#include "AudioInI2S.h"

AudioInI2S::AudioInI2S()
{
}

AudioInI2S::~AudioInI2S()
{
}

bool AudioInI2S::begin(long sampleRate, int bitsPerSample)
{
  if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample)) {
    return 0;
  }

  //Initialisation
  int _delay = 263000;
  for (int i = 0; i< _delay; i++) {
      // Trigger a read to kick things off
      I2S.read();
  }

  return 1;
}

void AudioInI2S::end()
{
  I2S.end();
}


bool AudioInI2S::readBuffer(void *buffer, int bufferSize) {
  int32_t sample = 0;
  int32_t counter = 0;
  int32_t *buff = (int32_t*) buffer;

  //FILL BUFFER HERE
  while (counter < bufferSize) {
    sample = I2S.read();
    if (sample) {
      *buff = sample >> 7; //Bit displacement
      buff++;
      counter++;
    }
  }

  return true;
}

AudioInI2S audioInI2SObject;
