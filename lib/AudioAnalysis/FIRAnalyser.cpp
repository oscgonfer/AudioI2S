#include "FIRAnalyser.h"
#include "AudioIn.h"

#include <stdlib.h> // For malloc/free
#include <string.h> // For memset

FIRAnalysis::FIRAnalysis(uint32_t bufferSize) :
  //BUFFER Sizes
  _bufferSize(bufferSize), //Already usable buffersize
  //BUFFERs
  _sampleBuffer(NULL),
  _sampleBufferFilt(NULL),
  _rmsFilterA(0),
  _rmsFilterADB(0),
  //EXTRAS
  _bitsPerSample(-1),
  _channels(-1),
  _FIRAvailable(0),
  _sampleRate(0)
{
}

FIRAnalysis::~FIRAnalysis(){
  if (_sampleBuffer) {
    free(_sampleBuffer);
  }

  if (_sampleBufferFilt) {
    free(_sampleBufferFilt);
  }
}

int FIRAnalysis::Available(){
  return _FIRAvailable;
}

int FIRAnalysis::Configure(AudioIn* input){

    _bitsPerSample = input->bitsPerSample();
    _channels = input->channels();

    //Allocate memory for buffers
      //Allocate time buffer
      _sampleBuffer = calloc(_bufferSize, sizeof(q31_t));
      
      //Allocate results buffer
      _sampleBufferFilt = calloc(_bufferSize, sizeof(q31_t));
      
    //Free all buffers in case of bad allocation
    if (_sampleBuffer == NULL || _sampleBufferFilt == NULL){

      if (_sampleBuffer) {
        free(_sampleBuffer);
        _sampleBuffer = NULL;
      }

      if (_sampleBufferFilt) {
        free(_sampleBufferFilt);
        _sampleBuffer = NULL;
      }

      return 0;
    }
    return 1;
}

double FIRAnalysis::FIRRMSRead_dB(){
  int rmsFilterADB = _rmsFilterADB;
  if (!_FIRAvailable){
    return 0;
  }
  _FIRAvailable=0;
  return rmsFilterADB;
}

void FIRAnalysis::Update(const void* buffer, size_t bufferUpdateSize){ 
   
  filterType32 *filter = FilterCreate(); // Create an instance of the filter

  uint8_t* newSamples = ((uint8_t*)_sampleBuffer);
  int samples = bufferUpdateSize / (_bitsPerSample / 8);

  const int32_t *src = (const int32_t*)buffer;
  int32_t* dst = (int32_t*)newSamples;

  for (int i = 0; i < samples; i += 2) {
    *dst = *src / 2;
    src++;
    *dst += *src / 2;
    *dst/=64; //CORRECT THE BIT NUMBER
    src++;
    dst++;
  }
  
  // Downscale the sample buffer for proper functioning
  Scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

  // Filter by convolution - applies a-weighting + equalization + window
  FilterReset(filter);
  int samplesProcessed = FilterInChunks(filter, _sampleBuffer, _sampleBufferFilt, _bufferSize);

  // RMS CALCULATION 
  _rmsFilterA = RMSG(_sampleBufferFilt, _bufferSize, 3, CONST_FACTOR); 
  _rmsFilterADB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rmsFilterA)); 

  FilterDestroy(filter);
  //free(_sampleBuffer);
  //free(_sampleBufferFilt);

  // Set available to 0 to wait for new process 
  _FIRAvailable = 1; 
} 

int FIRAnalysis::FilterInChunks(filterType32* pThis, void* pInput, void* pOutput, int length){
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

int FIRAnalysis::FilterConv(filterType32 * pThis, q31_t* pInputChunk, q31_t* pOutputChunk, unsigned int count) {
  arm_fir_q31( &pThis->_Q31, pInputChunk, pOutputChunk, count);

  return count;
}

filterType32 *FIRAnalysis::FilterCreate(void){
  filterType32 *result = (filterType32 *)malloc( sizeof(filterType32) ); // Allocate memory for the object
  FilterInit(result); // Initialize it
  return result; // Return the result
}

void FIRAnalysis::FilterInit(filterType32 * pThis){
  arm_float_to_q31(firCoeffs, pThis->_CoeffsQ31, FILTERSIZE);
  arm_fir_init_q31(&pThis->_Q31, FILTERSIZE, pThis->_CoeffsQ31, pThis->_FstateQ31, FILTERBLOCKSIZE);
  FilterReset(pThis);
}

void FIRAnalysis::FilterDestroy(filterType32 *pObject){
  free(pObject);
}

void FIRAnalysis::FilterReset(filterType32 * pThis) {
  // Reset state to 0
  memset(&pThis->_FstateQ31, 0, sizeof(pThis->_FstateQ31) );
  // Reset output
  memset(_sampleBufferFilt, 0, sizeof(_sampleBufferFilt));
}

void FIRAnalysis::SerialPrint(String ToPrint, int PrioFac, bool NewLine){
  // ONLY FOR PRINTING
  if (PrioFac-PRIORITY>0) {
      if (!NewLine) {
        Serial.print(ToPrint);
      } else Serial.println(ToPrint);
  }
}