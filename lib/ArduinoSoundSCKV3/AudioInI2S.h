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
#ifndef _AUDIO_IN_I2S_H_INCLUDED
#define _AUDIO_IN_I2S_H_INCLUDED

#include "Arduino.h"
#include "AudioIn.h"
#include "ConstantsSound.h"
#include <stddef.h>

class AudioInI2SClass : public AudioIn
{
public:
  AudioInI2SClass();
  virtual ~AudioInI2SClass();

  int begin(long sampleRate, int bitsPerSample, int timer_init, int total_sampling, int mic_type);
  virtual void end();

  virtual long sampleRate();
  virtual int bitsPerSample();
  virtual int channels();
  virtual int datasize();
  virtual int mic_type();
  void fftDone(int flag);
  void SerialPrint(String _to_print, int _prio_fac, bool _line);

protected:
  virtual int begin();
  virtual int read(void* buffer, size_t size);
  virtual int reset();

private:
  void onReceive();

  static void onI2SReceive();

private:
  long _sampleRate;
  int _fftDone;
  int _bitsPerSample;
  int _timer_init;
  int _total_timer;
  int _datasize;
  bool _callbackTriggered;
  int _mic_type;
};

extern AudioInI2SClass AudioInI2S;

#endif
