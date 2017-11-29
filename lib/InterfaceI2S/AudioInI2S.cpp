/*
  Copyright (c) 2016 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "AudioInI2S.h"
#include <I2S_SCK.h>

AudioInI2SClass::AudioInI2SClass() :
  _sampleRate(-1),
  _bitsPerSample(-1),
  _callbackTriggered(true),
  _fftDone(1)
{
}

AudioInI2SClass::~AudioInI2SClass()
{
}

int AudioInI2SClass::begin(long sampleRate, int bitsPerSample)
{
  if (!I2S_SCK.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample)) {
    return 0;
  }

  _sampleRate = sampleRate;
  _bitsPerSample = bitsPerSample;

  // add the receiver callback
  I2S_SCK.onReceive(AudioInI2SClass::onI2SReceive);
  _datasize = datasize();

  // Trigger a read to kick things off
  I2S_SCK.read();

  return 1;
}

void AudioInI2SClass::end()
{
  _sampleRate = -1;
  _bitsPerSample = -1;
  _callbackTriggered = true;

  I2S_SCK.end();
}

long AudioInI2SClass::sampleRate()
{
  return _sampleRate;
}

int AudioInI2SClass::bitsPerSample()
{
  return _bitsPerSample;
}

int AudioInI2SClass::channels()
{
  return 2;
}

int AudioInI2SClass::datasize()
{
  return I2S_SCK.datasize();
}

int AudioInI2SClass::begin()
{
  _callbackTriggered = false;

  return 0;
}


int AudioInI2SClass::read(void* buffer, size_t bufferReadSize)
{
  int readSize = 0;
  if (_fftDone) {
    _fftDone = 0;
    readSize = I2S_SCK.read(buffer, bufferReadSize);

    if (readSize) {
      samplesCalculate(buffer, readSize);    
    }
  }
  return readSize;
}

void AudioInI2SClass::fftDone(int flag)
{
  _fftDone = flag;
}

int AudioInI2SClass::reset()
{
  return 0;
}

void AudioInI2SClass::onReceive()
{
  if (_callbackTriggered) {
    uint8_t data[_datasize];
    read(data, sizeof(data));
  }
}


void AudioInI2SClass::onI2SReceive()
{
  AudioInI2S.onReceive();
}

AudioInI2SClass AudioInI2S;
