
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
class FIRAanalysis
{
public:
  FIRAanalysis(uint32_t fftSize); //
  virtual ~FIRAanalysis(); //

  int ConfigureFilter(int bitsPerSample,int channels, int bufferSize, int sampleRate);
  double AudioTimeFilter();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int available();

protected:
  virtual filterType32 *FilterCreate( void );
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