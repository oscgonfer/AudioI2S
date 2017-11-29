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

#ifndef _FFT_ANALYZER_H_INCLUDED
#define _FFT_ANALYZER_H_INCLUDED

#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>

#include <AudioInI2S.h>

#include "AudioAnalyzer.h"
#include "ConstantsSound.h"

class FFTAnalyzer : public AudioAnalyzer
{
public:
  FFTAnalyzer(int length);
  virtual ~FFTAnalyzer();

  int available();
  int read(int spectrum[], int Aspectrum [], int size);//to move to all-q31_t?
  //NEW
  int read_amp_a_weight();
  int read_amp();
  void SerialPrint(String _to_print, int _prio_fac, bool _line);

protected:
  virtual int configure(AudioIn* input);
  virtual void update(const void* buffer, size_t size);
  //NEW
  virtual void A_WEIGHTING(q31_t spectrum[], size_t size); //to move to all-q31_t?

private:
  int _length;
  int _iteration;
  int _bitsPerSample;
  int _channels;
  bool _enableIRQ; 
  bool _restart;
  int _datasize;
  int _mic_type;
  //NEW
  int _rms;
  int _a_weight_rms;

  arm_rfft_instance_q15 _S15;
  arm_rfft_instance_q31 _S31;

  int _available;
  int _fftDone;
  void* _sampleBuffer;
  int _sampleBufferSize;
  void* _fftBuffer;
  void* _spectrumBuffer;

  //NEW
  void* _AspectrumBuffer;

    //NEW
  int _rms_time;
  int _rms_fftB;
  int _rms_specB;
  int _rms_AspecB;

  //NEW
  String _to_print;
  bool _line;
  int _prio_fac;
};

#endif
