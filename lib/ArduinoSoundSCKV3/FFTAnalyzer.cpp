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

#include "AudioIn.h"

#include "FFTAnalyzer.h"
//#define Serial SerialUSB

FFTAnalyzer::FFTAnalyzer(int length) :
  _length(length),
  _bitsPerSample(-1),
  _channels(-1),
  _available(0),
  _fftDone(0),
  _sampleBuffer(NULL),
  _fftBuffer(NULL),
  _spectrumBuffer(NULL),
  //NEW
  _rms_time(0),
  _rms_fftB (0),
  _rms_specB (0),
  _rms_AspecB (0),
  _AspectrumBuffer(NULL),
  _datasize(0),
  _iteration(0),
  _restart(false),
  _mic_type(-1)
{
}

FFTAnalyzer::~FFTAnalyzer()
{
  if (_sampleBuffer) {
    free(_sampleBuffer);
  }

  if (_fftBuffer) {
    free(_fftBuffer);
  }

  if (_spectrumBuffer) {
    free(_spectrumBuffer);
  }

  if (_AspectrumBuffer) {
    free(_AspectrumBuffer);
  }
}

int FFTAnalyzer::available()
{
  return _available;
}

int FFTAnalyzer::read_amp_a_weight()
{
  return _rms_AspecB;
}

int FFTAnalyzer::read_amp()
{
  return _rms_specB;
}

int FFTAnalyzer::read(int spectrum[], int Aspectrum[], int spectrumSize) //to move to all-q31_t?
{
  if (!_available) {
    return 0;
  }

  if (spectrumSize > (_length / 2)) {
    spectrumSize = _length / 2;
  }

  if (_bitsPerSample == 16) {
    arm_q15_to_q31((q15_t*)_spectrumBuffer, (q31_t*)spectrum, spectrumSize);
    //A_WEIGHTING(spectrum,_length/2);
    arm_q15_to_q31((q15_t*)_AspectrumBuffer, (q31_t*)Aspectrum, spectrumSize);
  } else {
    memcpy(spectrum, _spectrumBuffer, sizeof(int) * spectrumSize);
    memcpy(Aspectrum, _AspectrumBuffer, sizeof(int) * spectrumSize);
  }
  _available = 0;

  return spectrumSize;
}

int FFTAnalyzer::configure(AudioIn* input)
{

    int bitsPerSample = input->bitsPerSample();
    int channels = input->channels();
    int datasize = input->datasize();
    int mic_type = input->mic_type(); //mic_type = 0 if INVENSENSE // = 1 if ADAFRUIT

    if (bitsPerSample != 16 && bitsPerSample != 32) {
      return 0;
    }

    if (channels != 1 && channels != 2) {
      return 0;
    }

    if (bitsPerSample == 16) {
      if (ARM_MATH_SUCCESS != arm_rfft_init_q15(&_S15, _length, 0, 1)) {
        return 0;
      }
    } else {
      if (ARM_MATH_SUCCESS != arm_rfft_init_q31(&_S31, _length, 0, 1)) {
        return 0;
      }
    }

    _bitsPerSample = bitsPerSample;
    _channels = channels;
    _datasize = datasize;
    _mic_type = mic_type;
    _sampleBufferSize = (_datasize/(_bitsPerSample/8))/_channels;
    //_sampleBufferSize = 256;

    SerialPrint("_datasize "+String(_datasize),5,true);
    SerialPrint("_sampleBufferSize " + String(_sampleBufferSize),5,true);

    if (bitsPerSample == 16) {
      //_sampleBufferSize = _length * sizeof(q15_t);
      _sampleBuffer = calloc(_sampleBufferSize, sizeof(q15_t));
      _spectrumBuffer = calloc(_length/2, sizeof(q15_t));

      //NEW
      _AspectrumBuffer = calloc(_length/2,sizeof(q15_t));
    } else {
      //_sampleBufferSize = _length * sizeof(q31_t);
      _sampleBuffer = calloc(_sampleBufferSize, sizeof(q31_t));
      _spectrumBuffer = calloc(_length/2, sizeof(q31_t));

      //NEW
      _AspectrumBuffer = calloc(_length/2,sizeof(q31_t));
    }

    _fftBuffer = calloc(_length, sizeof(q31_t));
  
    if (_sampleBuffer == NULL || _fftBuffer == NULL || _spectrumBuffer == NULL || _AspectrumBuffer == NULL) {

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_fftBuffer) {
        free(_fftBuffer);
        _fftBuffer = NULL;
      }

      if (_spectrumBuffer) {
        free(_spectrumBuffer);
        _spectrumBuffer = NULL;
      }

      if (_AspectrumBuffer) {
        free(_AspectrumBuffer);
        _AspectrumBuffer = NULL;
      }
      return 0;
    }
  return 1;
}

void FFTAnalyzer::update(const void* buffer, size_t size)
{
  q31_t rms_time = 0; //TIME DOMAIN RMS with original signal
  q31_t rms_fftB = 0; //FREQ DOMAIN RMS with NON-normalised FFT buffer
  q31_t rms_specB = 0; //FREQ DOMAIN RMS with normalised spectrumBuffer
  q31_t rms_AspecB = 0; //FREQ DOMAIN RMS with normalised AspectrumBuffer
  int newSamplesSize = (size / _channels);

  if (newSamplesSize > _sampleBufferSize) {
    // more samples than buffer size, cap
    newSamplesSize = _sampleBufferSize;
  }

  uint8_t* newSamples = ((uint8_t*)_sampleBuffer); // + newSamplesOffset;
  int samples = size / (_bitsPerSample / 8);
  SerialPrint("FFTAnalyzer buffer size " + String(size),5,true);
  SerialPrint("FFTAnalyzer samples " + String(samples),5,true);

  if (_channels == 2) {
    // average the stereo samples to mono
    if (_bitsPerSample == 16) {
      const int16_t *src = (const int16_t*)buffer;
      int16_t* dst = (int16_t*)newSamples;

      for (int i = 0; i < samples; i += 2) {
        *dst = (*src++) / 2;
        *dst += (*src++) / 2;
        dst++;
      }
    } else {
      const int32_t *src = (const int32_t*)buffer;
      int32_t* dst = (int32_t*)newSamples;

      for (int i = 0; i < samples; i += 2) {
        *dst = *src / 2;
        src++;
        *dst += *src / 2;
        src++;
        
        if (_mic_type == 1) {
          *dst/=4096;
        }
        else {
          if (_mic_type ==0){
            *dst/=64;           
          }
        }
        dst++;
      }
    }
  } else {
    memcpy(newSamples, buffer, size);
  }

  SerialPrint("Buffer treatment done",5,true);

  if (_bitsPerSample == 16) {
    /*//CALCULATE FFT-buffer ((r-i,r-i...))
    arm_rfft_q15(&_S15, (q15_t*) _sampleBuffer, (q15_t*)_fftBuffer);
    //INCLUDE A-WEIGHTING HERE

    //CALCULATE COMPLEX MAGNITUDE
    arm_cmplx_mag_q15((q15_t*)_fftBuffer, (q15_t*)_spectrumBuffer, _length);

    //NORMALISE SPECTRUM BY SAMPLES LENGTH

    int32_t* spc = (int32_t*)_spectrumBuffer;
    for (int i = 0; i<_length/2;i++) {
      if (i=0) {
        *spc = (*spc  / _sampleBufferSize);
      } else {
        *spc = 2*(*spc  / _sampleBufferSize);      
      }
      spc++;
    }

    //Computes RMS of the FFT spectrum
    arm_rms_q15((q15_t*)_spectrumBuffer, _length/2, (q15_t*)&rms);
    //Perform A-Weighting
    //memcpy(spectrum, _spectrumBuffer, sizeof(int) * _length/2);
    A_WEIGHTING((q31_t*) _spectrumBuffer,_length/2);
    */
  } else {

    _iteration ++;
    //PRINT FILE INDEXING PARAMETERS
    
    SerialPrint("sampleBufferSize\t",6,false);
    SerialPrint("spectrumSize\t",6,false);
    SerialPrint("iteration\t",6,false);
    SerialPrint("micType",6,true);
    SerialPrint(String(_sampleBufferSize) + '\t',6,false);
    SerialPrint(String(_length/2) + '\t',6,false);
    SerialPrint(String(_iteration) + '\t',6,false);
    SerialPrint(String(_mic_type) + '\t',6,true);
    SerialPrint("*header_end*",6,true);
    
    
    //PRINT RAW SIGNAL
    SerialPrint("_sampleBuffer",5,true);

    int32_t* sBprint = (int32_t*)_sampleBuffer;
    for (int i = 0; i < _sampleBufferSize; i ++) {
      SerialPrint(String(i) + "\t" + String(*sBprint),6,true);
      sBprint++;
    }

    SerialPrint("*time_end*",6,true);
    
    /*
    //CALCULATE THE SINGLE-SIDE NORMALISED SPECTRUM - spectrumBuffer
    //"index" + "\t" + "_fftBuffer Real" + "\t" + "_fftBuffer Imag" + "\t" + "spectrumBuffer"
    q31_t* fBprint = (q31_t*)_fftBuffer;
    q31_t* spBprint = (q31_t*)_spectrumBuffer;
    for (int i = 0; i < _length/2; i ++) {
      //*spBprint += (*fBprint)*(*fBprint);
      SerialPrint(String(i) + "\t" + String(*fBprint),6,false);
      fBprint++;
      //*spBprint += (*fBprint)*(*fBprint);
      SerialPrint("\t" + String(*fBprint),6,false);
      SerialPrint("\t" + String(*spBprint),6,true);
      fBprint++;
      spBprint++;
    }
    */

    //CALCULATE RMS from the FFT buffer (non-normalised)
    /*const int32_t *fftBp = (const int32_t*)_fftBuffer;
    for (int i = 0; i<_length;i++) {
      rms_fftB += *fftBp^2;      
      fftBp++;
    }
    rms_fftB = 1/sqrt(_length)*sqrt(rms_fftB/_length);*/
    //rms_fftB = rms_fftB*sqrt(2);

    /*SerialPrint("_spectrumBuffer treated",6,true);
    int32_t* spBp = (int32_t*)_spectrumBuffer;
    //*spBp = (*spBp  / _sampleBufferSize); //DC component has both spectrum - no need to normalise energy
    rms_specB = *spBp^2;
    for (int i = 1; i<_length/2-1;i++) {
      SerialPrint(String(i) + "\t" + String(*spBp),6,true);
      rms_specB += (*spBp)^2;
      spBp++;
      //*spBp = 2*(*spBp/_sampleBufferSize);      
    }

    rms_specB = sqrt(rms_specB)/sqrt(2);*/

    /*
    int32_t* AspBp = (int32_t*)_AspectrumBuffer;
    //*AspBp = (*AspBp / _sampleBufferSize); //DC component has both spectrum - no need to normalise energy
    rms_AspecB = *AspBp^2;
    for (int i = 1; i<_length/2-1;i++) {
      rms_AspecB += *AspBp^2;
      AspBp++;
      //*AspBp = 2*(*AspBp/_sampleBufferSize);      
    }
    rms_AspecB = sqrt(rms_AspecB)/sqrt(2);*/

    //CALCULATE FFT-buffer ((r-i,r-i...))
    arm_rfft_q31(&_S31, (q31_t*)_sampleBuffer, (q31_t*)_fftBuffer);
    SerialPrint("FFT done",6,true);
    
    //CALCULATE FFT spectrum (r,r,r,...)
    arm_cmplx_mag_q31((q31_t*)_fftBuffer, (q31_t*) _spectrumBuffer, _length); //This is not being calculated properly
    
    /*
    //MULTIPLY BY 2 ALL COMPONENTS EXCEPT DC
    SerialPrint("Complex mag done",6,true);

    int32_t* spBcor = (int32_t*)_spectrumBuffer;//q31_t
    spBcor++;
    for (int i = 1; i < _length/2; i ++) {
      *spBcor*2;
      spBcor++;
    }
    */
    //SerialPrint("Normalisation mag done",6,true);

    //A-WEIGHTING OF _spectrumBuffer
    //A_WEIGHTING((q31_t*)_spectrumBuffer,_length/2);
    /*
    int32_t* _AspecBp = ((int32_t*)_AspectrumBuffer);
    const int32_t* _specBp = (const int32_t*)_spectrumBuffer;

    for (int i = 0; i < _length/2; i++) {
      double weighingfactor = weightingtab[i];
      SerialPrint(String(i) + + "\t"+ String(weighingfactor),5,false);

      //Apply Real FFT
      *_AspecBp = (*_specBp) * weighingfactor;
      SerialPrint("\t"+ String(*_specBp),5,false);
      SerialPrint("\t"+ String(*_AspecBp),5,false);

      _AspecBp++;
      _specBp++;
    }
    */

    //PROCESS AND PRINT-OUT ALL THE BUFFERS
    const q31_t* fBprint = (const q31_t*)_fftBuffer;
    q31_t* spBprint = (q31_t*)_spectrumBuffer;
    //const q31_t* AspBprint= (const q31_t*)_AspectrumBuffer;
    q31_t* AspBprint= (q31_t*)_AspectrumBuffer;
    //double* weighingfactor = (double*) weightingtab;

    for (int i = 0; i < _length/2; i ++) {

      //MULTIPLY BY 2 ALL COMPONENTS EXCEPT DC
      if (i) {
        *spBprint*2;
      }
      
      //A-WEIGHTING OF _spectrumBuffer
      double weighingfactor = weightingtab[i];
      *AspBprint = (*spBprint) * (weighingfactor);

      //PRINT OUT EVERYTHING
      SerialPrint(String(i) + "\t" + String(*fBprint),6,false);
      fBprint++;
      SerialPrint("\t" + String(*fBprint),6,false);
      fBprint++;
      SerialPrint("\t" + String(*spBprint),6,false);
      spBprint++;
      SerialPrint("\t" + String(*AspBprint),6,true);
      AspBprint++;
    }

    SerialPrint("*freq_end*",6,true);

    //CALCULATE RMSs
    arm_rms_q31((q31_t*)_sampleBuffer,_sampleBufferSize,(q31_t*)&rms_time);

    arm_rms_q31((q31_t*)_fftBuffer,_length, (q31_t*)&rms_fftB);
    
    arm_rms_q31((q31_t*)_spectrumBuffer,_length/2, (q31_t*)&rms_specB);

    arm_rms_q31((q31_t*)_AspectrumBuffer,_length/2, (q31_t*)&rms_AspecB);

    /*
    //Convert to dB
    Convert2DB((q31_t*)_spectrumBuffer,(q31_t*)_spectrumBufferDB,_length/2);
    Convert2DB((q31_t*)_AspectrumBuffer,(q31_t*)_AspectrumBufferDB,_length/2);
    Convert2DB((q31_t*)rms_specB,(q31_t*)_rms_specBDB,1);
    Convert2DB((q31_t*)rms_AspecB,(q31_t*)_rms_AspecBDB,1);
    */
    //TO-DO:
    // Add target variables
    // Allocate memory
    // Create function in .h

    //Print-out Scalars
    SerialPrint("_datasize\t_rms_time\t_rms_fftB\trms_specB\trms_AspecB",6,true);
    SerialPrint(String(_datasize)+"\t"+String(rms_time)+"\t"+String(rms_fftB)+"\t"+String(rms_specB)+"\t"+String(rms_AspecB),6,true);
    SerialPrint("*sca_end*",6,true);
  }

  //RMS retrieve
  _rms_time = rms_time;
  _rms_fftB = rms_fftB;
  _rms_specB = rms_specB; //RMS_DBFS
  _rms_AspecB = rms_AspecB; //RMS_DBFS_A

  _available = 1;
  _fftDone = 1;
  AudioInI2S.fftDone(_fftDone);
}

//NEW
void FFTAnalyzer::A_WEIGHTING(q31_t _spectrumBuffer[], size_t _length) {//to move to all-q31_t?
  if (_bitsPerSample == 16) {

    int16_t* _AspecBp = ((int16_t*)_AspectrumBuffer);
    const int16_t* _specBp = (const int16_t*)_spectrumBuffer;

    for (int i = 0; i < _length/2; i++) {
      double weighingfactor = weightingtab[i];
      SerialPrint(String(i) + "\t"+ String(weighingfactor),5,false);

      //Apply Real FFT
      *_AspecBp = (*_specBp) * weighingfactor;
      SerialPrint("\t"+ String(*_specBp),5,false);
      SerialPrint("\t"+ String(*_AspecBp),5,false);

      _specBp++;
      _AspecBp++;
    }

  } else {
    SerialPrint("A-WEIGHTING called",5,true);
    
    int32_t* _AspecBp = ((int32_t*)_AspectrumBuffer);
    const int32_t* _specBp = (const int32_t*)_spectrumBuffer;

    for (int i = 0; i < _length/2; i++) {
      double weighingfactor = weightingtab[i];
      SerialPrint(String(i) + + "\t"+ String(weighingfactor),5,false);

      //Apply Real FFT
      *_AspecBp = (*_specBp) * weighingfactor;
      SerialPrint("\t"+ String(*_specBp),5,false);
      SerialPrint("\t"+ String(*_AspecBp),5,false);

      _AspecBp++;
      _specBp++;
    }
    SerialPrint("A-WEIGHTING finished",6,true);
  }
}


//ONLY FOR PRINTING 
void FFTAnalyzer::SerialPrint(String _to_print, int _prio_fac, bool _line)
{
  if (_prio_fac-PRIORITY>0) {
      if (!_line) {
        Serial.print(_to_print);
      } else Serial.println(_to_print);
  }
}
