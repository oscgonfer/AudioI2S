#ifndef _FIR_ANALYSER_H_INCLUDED
#define _FIR_ANALYSER_H_INCLUDED

#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>
#define __FPU_PRESENT 0

#include <AudioInI2S.h>

#include "AudioAnalyser.h"
#include "ConstantsSound.h"
#include "fir_coeffs.h"

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
  FIRAnalysis(uint32_t bufferSize); //
  virtual ~FIRAnalysis(); //

  double FIRRMSRead_dB();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int Available();

protected:
  virtual int Configure(AudioIn* input);
  virtual void Update(const void* buffer, size_t bufferSize);

  virtual filterType32 *FilterCreate(void);
  virtual void FilterReset(filterType32 * pThis);
  virtual void FilterDestroy(filterType32 *pObject);
  virtual void FilterInit(filterType32 * pThis);  
  virtual int FilterConv(filterType32 * pThis, q31_t * pInput, q31_t * pOutput, unsigned int count );
  virtual int FilterInChunks(filterType32 * pThis, void * pInput, void * pOutput, int length);

private:
  //BUFFER Sizes
  int _bufferSize; //Already usable bufferSize
  //PARAMETERS
  int _bitsPerSample;
  int _channels;
  int _sampleRate;
  //RMS Results
  double _rmsFilterA;
  double _rmsFilterADB;
  //BUFFERS
  void* _sampleBuffer;
    //FILTER
    void* _sampleBufferFilt;    
  int _FIRAvailable;
};

#endif