#ifndef _FIR_ANALYSER_H_INCLUDED
#define _FIR_ANALYSER_H_INCLUDED

#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>
#include <stdlib.h> // For malloc/free
#include <string.h> // For memset
#define __FPU_PRESENT 0


#include "AudioAnalyser.h"
#include "ConstantsSound.h"
#include "FilterCoefficients/fir_coeffs.h"

//FILTER
typedef struct
{
  arm_fir_instance_q31 _Q31;
  q31_t _FstateQ31[FILTERSIZE+FILTERBLOCKSIZE];
  q31_t _CoeffsQ31[FILTERSIZE];
} filterType32;

//CLASS
class FIRAnalysis : public AudioAnalyser
{
public:
  FIRAnalysis(int bufferSize, WindowType windowType); //
  ~FIRAnalysis(); //

  double sensorRead();
  bool configure(AudioInI2S& input);

private:

  filterType32 *filterCreate();
  void filterReset(filterType32 * pThis);
  void filterDestroy(filterType32 *pObject);
  void filterInit(filterType32 * pThis);  
  int filterConv(filterType32 * pThis, q31_t * pInput, q31_t * pOutput, unsigned int count);
  int filterInChunks(filterType32 * pThis, void * pInput, void * pOutput, int length);

  //BUFFER Sizes
  int _bufferSize; //Already usable bufferSize
  //PARAMETERS
  int _bitsPerSample;
  int _sampleRate;
  //RMS Results
  double _rms;
  double _rmsDB;
  //BUFFERS
  void* _sampleBuffer;
  //FILTER
  void* _sampleBufferFilt;
  unsigned int _chunkLength;  
  //WINDOW
  void* _windowTable;
  WindowType _window_type;
};

#endif