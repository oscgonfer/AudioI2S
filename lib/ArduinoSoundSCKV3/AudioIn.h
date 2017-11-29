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
#ifndef _AUDIO_IN_H_INCLUDED
#define _AUDIO_IN_H_INCLUDED

#include <stddef.h>

class AudioAnalyzer;

class AudioIn
{
public:
  AudioIn();
  virtual ~AudioIn();

  virtual long sampleRate() = 0; // Returns the sample rate
  virtual int bitsPerSample() = 0; // returns the bits per sample
  virtual int channels() = 0; // Returns the number of channels
  virtual int datasize() = 0; // Returns the size of the maximum buffer acquired by the I2S (defined at I2S.doublebuffer.h)
  virtual int mic_type() = 0; //Returns the type of the microphone
protected:
  friend class AudioAnalyzer;

  void samplesRead(void* buffer, size_t size);

protected:

  virtual int begin() = 0;
  virtual int read(void* buffer, size_t size) = 0;
  //virtual int once(void* buffer, size_t size, int iter_once) = 0;
  virtual int reset() = 0;
  virtual void end() = 0;

protected:
  friend class AudioAnalyzer;

  int setAnalyzer(AudioAnalyzer* analyzer);

private:
  AudioAnalyzer* _analyzer;
};

#endif
