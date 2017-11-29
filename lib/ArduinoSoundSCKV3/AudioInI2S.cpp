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

#include "I2S_SCK.h"

#include "AudioInI2S.h"

AudioInI2SClass::AudioInI2SClass() :
  _sampleRate(-1),
  _bitsPerSample(-1),
  _callbackTriggered(true),
  _timer_init(-1),
  _datasize(-1),
  _total_timer(-1),
  _fftDone(1),
  _mic_type(-1)
{
}

AudioInI2SClass::~AudioInI2SClass()
{
}

int AudioInI2SClass::begin(long sampleRate, int bitsPerSample, int timer_init, int total_sampling,int mic_type)
{
  if (!I2S_SCK.begin(I2S_PHILIPS_MODE, sampleRate, bitsPerSample)) {
    return 0;
  }

  _sampleRate = sampleRate;
  _bitsPerSample = bitsPerSample;
  _timer_init = timer_init;
  _total_timer = total_sampling;
  _mic_type = mic_type;

  // add the receiver callback
  I2S_SCK.onReceive(AudioInI2SClass::onI2SReceive);
  _datasize = datasize();

  // trigger a read to kick things off
  I2S_SCK.read();

  //delay(1000);

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

int AudioInI2SClass::mic_type()
{
  if (_mic_type) {
    return 1; // ADAFRUIT
  } else {
    return 0; //INVENSENSE
  }
}

int AudioInI2SClass::begin()
{
  _callbackTriggered = false;

  return 0;
}

void AudioInI2SClass::fftDone(int flag)
{
  _fftDone = flag;
}

int AudioInI2SClass::read(void* buffer, size_t size)
{
  
  long _timer = millis();
  int read = 0;

  if ((_timer-_timer_init)<_total_timer){
    if (_timer >_timer_init) {
      SerialPrint("*continue*",6,true);
      SerialPrint("fftDone\t" + String(_fftDone),5,true);
      if (_fftDone) {
        _fftDone = 0;
        read = I2S_SCK.read(buffer, size);
        if (read) {
          samplesRead(buffer, read);  
        }
      }
    } else {
      read = I2S_SCK.read(buffer,size);
    }
  } else {
    SerialPrint("Analysis performed for: " + String(_total_timer),6,true);
    SerialPrint("*acquisition_end*",6,true);
  }
  

  //Serial.println(String(_timer) + " - AudioInI2SClass::read / read " + String (read));
  
  return read;
}


int AudioInI2SClass::reset()
{
  return 0;
}

void AudioInI2SClass::onReceive()
{
  if (_callbackTriggered) {
    //Serial.println("Data size = " +String(_datasize));
    uint8_t data[_datasize];

    read(data, sizeof(data));
  }
}

void AudioInI2SClass::onI2SReceive()
{
  AudioInI2S.onReceive();
}

//ONLY FOR PRINTING 
void AudioInI2SClass::SerialPrint(String _to_print, int _prio_fac, bool _line)
{
  if (_prio_fac-PRIORITY>0) {
      if (!_line) {
        Serial.print(_to_print);
      } else Serial.println(_to_print);
  }
}

AudioInI2SClass AudioInI2S;
