#include "FIRAnalyser.h"

FIRAnalysis::FIRAnalysis(uint32_t bufferSize) :
  //BUFFER Sizes
  _bufferSize(bufferSize), //Already usable buffersize
  //BUFFERs
  _sampleBuffer(NULL),
  _sampleBufferFilt(NULL),
  _rmsFilter(0),
  _rmsFilterDB(0),
  //EXTRAS
  _bitsPerSample(-1),
  _channels(-1),
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

bool FIRAnalysis::configure(AudioInI2S& input){

    _bitsPerSample = input.bitsPerSample();
    _channels = input.channels();

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

double FIRAnalysis::sensorRead(){

  //ASK I2S about BUFFERSIZE?
  //uint8_t _buffer[_datasize];
  int32_t _buffer[_bufferSize]; //Or using directly the _sampleBuffer?

  if (!audioInI2SObject.bufferI2SAvailable(_buffer,_bufferSize)){
    free(_buffer);
  } else {
    filterType32 *_filter = filterCreate(); // Create an instance of the filter

    available(false);

    //------------------------------
    // PRE-TREAT BUFFER - NECESSARY?
    //------------------------------
    int32_t* newSamples = ((int32_t*)_sampleBuffer);
    int samples = _bufferSize / (_bitsPerSample / 8); //NEEDED?

    const int32_t *src = (const int32_t*)_buffer;
    int32_t* dst = (int32_t*)newSamples;

    for (int i = 0; i < samples; i += 2) {
      *dst = *src / 2;
      src++;
      *dst += *src / 2;
      *dst/=64; //CORRECT THE BIT NUMBER
      src++;
      dst++;
    }
    //------------------------------
  
    // Downscale the sample buffer for proper functioning
    scaling(_sampleBuffer, _bufferSize, CONST_FACTOR, false);

    window(_sampleBuffer,_bufferSize);

    // Filter by convolution - applies a-weighting + equalization + window
    filterReset(_filter);
    int samplesProcessed = filterInChunks(_filter, _sampleBuffer, _sampleBufferFilt, _bufferSize);

    // RMS CALCULATION 
    _rmsFilter = rms(_sampleBufferFilt, _bufferSize, 3, CONST_FACTOR); 
    _rmsFilterDB = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-20*log10(sqrt(2)*_rmsFilter)); 
    filterDestroy(_filter);
  }


  free(_sampleBuffer);
  free(_sampleBufferFilt);

  // Set available to true
  available(true);

  return _rmsFilterDB;
}


int FIRAnalysis::filterInChunks(filterType32* pThis, void* pInput, void* pOutput, int length){
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
    outLength = filterConv(pThis, _pInpBuffer, _pOutBuffer, chunkLength);
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

int FIRAnalysis::filterConv(filterType32 * pThis, q31_t* pInputChunk, q31_t* pOutputChunk, unsigned int count) {
  arm_fir_q31( &pThis->_Q31, pInputChunk, pOutputChunk, count);

  return count;
}

filterType32 *FIRAnalysis::filterCreate(){
  filterType32 *result = (filterType32 *)malloc( sizeof(filterType32) ); // Allocate memory for the object
  filterInit(result); // Initialize it
  return result; // Return the result
}

void FIRAnalysis::filterInit(filterType32 * pThis){
  arm_float_to_q31(firCoeffs, pThis->_CoeffsQ31, FILTERSIZE);
  arm_fir_init_q31(&pThis->_Q31, FILTERSIZE, pThis->_CoeffsQ31, pThis->_FstateQ31, FILTERBLOCKSIZE);
  filterReset(pThis);
}

void FIRAnalysis::filterDestroy(filterType32 *pObject){
  free(pObject);
}

void FIRAnalysis::filterReset(filterType32 * pThis) {
  // Reset state to 0
  memset(&pThis->_FstateQ31, 0, sizeof(pThis->_FstateQ31) );
  // Reset output
  memset(_sampleBufferFilt, 0, sizeof(_sampleBufferFilt));
}