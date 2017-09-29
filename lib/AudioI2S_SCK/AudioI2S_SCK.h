#include <Arduino.h>

#define ARM_MATH_CM0PLUS
#include <arm_math.h>
#define __FPU_PRESENT 0

#include "ConstantsSound.h"
#include "RawData.h"
#include "fir_coeffs.h"

//FILTER
typedef struct
{
  arm_fir_instance_q31 _Q31;
  q31_t _FstateQ31[FILTERSIZE+FILTERBLOCKSIZE];
  q31_t _CoeffsQ31[FILTERSIZE];
} filterType32;

//CLASS
class AudioI2S_SCK
{
public:
  AudioI2S_SCK(uint32_t fftSize); //
  virtual ~AudioI2S_SCK(); //

  int ConfigureFFT(int bitsPerSample,int channels, int bufferSize, int sampleRate, bool SpectrumDBOutput);
  int ConfigureFilter(int bitsPerSample,int channels, int bufferSize, int sampleRate);
  double AudioSpectrumRead(int fftSize);
  double AudioSpectrumRead(int spectrum[], int Aspectrum [], int spectrumDB[], int AspectrumDB[], int fftSize);
  double AudioTimeFilter();
  double AudioRMSRead_dB();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int available();

protected:
  virtual void GetBuffer();
  virtual void Window();
  virtual void FFT(void *inputBuffer, void* outputBuffer, int fftBufferSize);
  virtual void A_WEIGHTING(void *inputBuffer, void* outputBuffer, int inputSize);
  virtual double RMSG(void *inputBuffer, int inputSize, int typeRMS, int factor);
  virtual void Scaling(void *vector, int vectorSize, double factor, bool multDiv);
  virtual void EQUALIZING(void *inputBuffer, int inputSize);
  virtual void Convert2DB(void *vectorSource, void *vectorDest, int vectorSize);

  virtual filterType32 *FilterCreate( void );
  virtual void FilterReset(filterType32 * pThis);
  virtual void FilterDestroy(filterType32 *pObject);
  virtual void FilterInit(filterType32 * pThis);  
  virtual int FilterConv(filterType32 * pThis, q31_t * pInput, q31_t * pOutput, unsigned int count );
  virtual int FilterInChunks(filterType32 * pThis, void * pInput, void * pOutput, int length);

private:
  //BUFFER Sizes
  uint32_t _fftSize;
  int _bufferSize; //Already usable bufferSize
  //PARAMETERS
  int _bitsPerSample;
  int _channels;
  int _sampleRate;
  bool _SpectrumDBOutput;
  //RMS Results
  double _rms_time;
  double _rms_specB;
  double _rms_AspecB;
  double _rms_timeDB;
  double _rms_specBDB;
  double _rms_AspecBDB;
  double _rmsFilterA;
  double _rmsFilterADB;
  //BUFFERS
  void* _sampleBuffer;
  void* _sampleBufferWin;
    //FFT
    void* _fftBuffer;
    void* _spectrumBuffer;
    void* _AspectrumBuffer;
    void* _spectrumBufferDB;
    void* _AspectrumBufferDB;
    //FILTER
    void* _sampleBufferFilt;    
  //EXTRAS
  int _SpectrumAvailable;
  int _RMSAvailable;
  int _bufferAvailable;
  int _available;
  //FFT
  arm_rfft_instance_q31 _S31;
};
