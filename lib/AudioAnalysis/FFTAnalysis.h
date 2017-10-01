#include "AudioAnalysis.h"
#include "ConstantsSound.h"

//CLASS
class FFTAnalysis
{
public:
  FFTAnalysis(uint32_t fftSize); //
  virtual ~FFTAnalysis(); //

  int ConfigureFFT(int bitsPerSample,int channels, int bufferSize, int sampleRate, bool SpectrumDBOutput);
  double AudioSpectrumRead(int fftSize);
  double AudioSpectrumRead(int spectrum[], int Aspectrum [], int spectrumDB[], int AspectrumDB[], int fftSize);
  double AudioRMSRead_dB();
  void SerialPrint(String ToPrint, int PrioFac, bool NewLine);//
  int available();

protected:
  virtual void GetBuffer();
  virtual void Window();
  virtual void FFT(void *inputBuffer, void* outputBuffer, int fftBufferSize);
  virtual void A_WEIGHTING(void *inputBuffer, void* outputBuffer, int inputSize);
  virtual double RMSG(void *inputBuffer, int inputSize, int typeRMS, int factor);

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
  //BUFFERS
  void* _sampleBuffer;
  void* _sampleBufferWin;
    //FFT
    void* _fftBuffer;
    void* _spectrumBuffer;
    void* _AspectrumBuffer;
    void* _spectrumBufferDB;
    void* _AspectrumBufferDB;  
  //EXTRAS
  int _SpectrumAvailable;
  int _RMSAvailable;
  int _bufferAvailable;
  int _available;
  //FFT
  arm_rfft_instance_q31 _S31;
};
