#include "FIRAanalysis.h"
#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

FIRAanalysis::FIRAanalysis(uint32_t bufferSize) :
  //BUFFER Sizes
  _bufferSize(bufferSize), //Already usable buffersize
  //BUFFERs
  _sampleBuffer(NULL),
  _sampleBufferWin(NULL),
  _sampleBufferFilt(NULL),
  _rmsFilterA(0),
  _rmsFilterADB(0),
  //EXTRAS
  _bitsPerSample(-1),
  _channels(-1),
  _bufferAvailable(1),
  _RMSAvailable(1),
  _available(1),
  _sampleRate(0)
{
}

FIRAanalysis::~FIRAanalysis(){
  if (_sampleBuffer) {
    free(_sampleBuffer);
  }

  if (_sampleBufferFilt) {
    free(_sampleBufferFilt);
  }
}

int FIRAanalysis::available(){
  _available = _FIRAvailable;
  return _available;
}

int FIRAanalysis::ConfigureFilter(int bitsPerSample,int channels, int bufferSize, int sampleRate){
    _bitsPerSample = bitsPerSample;
    _channels = channels;
    _bufferSize = bufferSize;
    _sampleRate = sampleRate;

    //General fool-proof checks
    if (_bitsPerSample != 32) {
      return 0;
    }

    if (_channels != 1 && _channels != 2) {
      return 0;
    }

    //Allocate memory for buffers

      //Allocate time buffer
      _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
      
      //Allocate results buffer
      _sampleBufferFilt = calloc(_bufferSize, sizeof(q31_t));
      
    //Free all buffers in case of bad allocation
    if (_sampleBuffer == NULL || _sampleBufferWin == NULL || _sampleBufferFilt == NULL){

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_sampleBufferWin) {
        free(_sampleBufferWin);
        _sampleBufferWin = NULL;
      }

      if (_sampleBufferFilt) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      return 0;
    }
    return 1;
}

double FIRAanalysis::AudioTimeFilter(){ 
   
  filterType32 *filter = FilterCreate(); // Create an instance of the filter

  if (!_bufferAvailable) { 
    return 0; 
  } 
 
  // Get buffer (currently hardcoded) 
  GetBuffer(); 
 
  // Downscale the sample buffer for proper functioning 
  int _time_before_Down = millis();
  Scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false); 
  int _time_after_Down = millis();
  int time_delta_Down = _time_after_Down-_time_before_Down;

  // Filter by convolution - applies a-weighting + equalization + window
  FilterReset(filter);
  int _time_before_FIR = millis();
  int samplesProcessed = FilterInChunks(filter, _sampleBuffer, _sampleBufferFilt, _bufferSize);
  int _time_after_FIR = millis();
  int time_delta_FIR = _time_after_FIR-_time_before_FIR;
  
  // RMS CALCULATION 
  int _time_before_RMS = millis();
  _rmsFilterA = RMSG(_sampleBufferFilt, _bufferSize, 3, CONST_FACTOR); 
  _rmsFilterADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rmsFilterA)); 
  int _time_after_RMS = millis();
  int time_delta_RMS = _time_after_RMS-_time_before_RMS;
  
  SerialPrint("samplesProcessed = \t"+ String(samplesProcessed),6,true);
  SerialPrint("time_delta_Down : \t" + String(time_delta_Down),6,true);
  SerialPrint("time_delta_FIR : \t" + String(time_delta_FIR),6,true);
  SerialPrint("time_delta_RMS : \t" + String(time_delta_RMS),6,true);
  
  FilterDestroy(filter);
  // Set available to 0 to wait for new process 
  _bufferAvailable = 0; 

  return _rmsFilterADB; 
} 

int FIRAanalysis::FilterInChunks(filterType32* pThis, void* pInput, void* pOutput, int length){
  int processedLength = 0;
  unsigned int chunkLength, outLength;

  q31_t* _pInpBuffer = (q31_t*) pInput;
  q31_t* _pOutBuffer = (q31_t*) pOutput;

  while( length > 0 )
  {
    // Choose chunkLength from 0-30
    chunkLength = 30;
    // Limit chunk length to the number of remaining samples                                  
    if( chunkLength > length ) chunkLength = length;  
    // Filter the block and determine the number of returned samples   
    outLength = FilterConv(pThis, _pInpBuffer, _pOutBuffer, chunkLength);
    //SerialPrint("outLength\t" + String(outLength),6,true);
    // Update the output pointer
    _pOutBuffer += outLength;            
    // Update the total number of samples output           
    processedLength += outLength;
    // Update the input pointer                  
    _pInpBuffer += chunkLength;
    // Update the number of samples remaining                        
    length -= chunkLength;
  }
  // Return the number of samples processed
  return processedLength;                         
}

int FIRAanalysis::FilterConv(filterType32 * pThis, q31_t* pInputChunk, q31_t* pOutputChunk, unsigned int count) {
  arm_fir_q31( &pThis->_Q31, pInputChunk, pOutputChunk, count);

  return count;
}

filterType32 *FIRAanalysis::FilterCreate(void){
  filterType32 *result = (filterType32 *)malloc( sizeof(filterType32) ); // Allocate memory for the object
  FilterInit(result);                     // Initialize it
  return result;                                // Return the result
}

void FIRAanalysis::FilterInit(filterType32 * pThis){
  arm_float_to_q31(firCoeffs, pThis->_CoeffsQ31, FILTERSIZE);
  arm_fir_init_q31(&pThis->_Q31, FILTERSIZE, pThis->_CoeffsQ31, pThis->_FstateQ31, FILTERBLOCKSIZE);
  FilterReset( pThis );
}

void FIRAanalysis::FilterDestroy(filterType32 *pObject){
  free(pObject);
  free(_sampleBufferFilt);
}

void FIRAanalysis::FilterReset(filterType32 * pThis) {
  // Reset state to 0
  memset(&pThis->_FstateQ31, 0, sizeof(pThis->_FstateQ31) );
  // Reset output
  memset(_sampleBufferFilt, 0, sizeof(_sampleBufferFilt));
}

void FIRAanalysis::SerialPrint(String ToPrint, int PrioFac, bool NewLine){
  // ONLY FOR PRINTING
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}