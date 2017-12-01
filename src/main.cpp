/*
Circuit:
 * Arduino/Genuino Zero
 * ICS43432:
   * GND connected GND
   * 3.3V connected 3.3V
   * WS connected to pin 0
   * CLK connected to pin 1
   * SD connected to pin 9
*/

#include "FFTAnalyser.h"
#include "AudioInI2S.h"

///// FFT Parameters
const int fftSize = 512; //IF THIS CHANGES, WEIGHTING FUNCTION AND EQUALISATION TABLE CHANGE

//// AUDIO I2S Parameters
const int bitsPerSample = 32; // FIXED FOR ICS43432 (24bit)
const int channels = 2; //FIXED
const int bufferSize = 512;  //IF THIS CHANGES, HANN WINDOW FUNCTION CHANGES
const int sampleRate = 8000; //Hz

///// OUTPUT
int spectrum[fftSize/2];
double resultDB = 0;

///// Analyser
FFTAnalyser fftAnalyser(bufferSize, fftSize, A_WEIGHTING);
//FIRAnalysis FIRAnalysis(bufferSize);

void setup() {
	// Open serial communications
	Serial.begin(115200);

 	// Configure Audio Input
    if(!audioInI2SObject.begin(sampleRate, bitsPerSample)){
        Serial.println("Failed to init I2S");
    }

    // Configure Analyser
    if(!fftAnalyser.configure(audioInI2SObject)){
        Serial.println("Failed to init Analyser");
    }

    Serial.println("-------------");
    Serial.println("Init Audio OK");

}

uint32_t FreeRamMem() {
    uint32_t stackTop;
    uint32_t heapTop;

    // Current position of the stack
    stackTop = (uint32_t) &stackTop;

    // Current position of heap
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram
    return stackTop - heapTop;
}

void loop() {

    //READ RMS AND SPECTRUM
	resultDB = fftAnalyser.sensorRead(spectrum);
    //READ ONLY RMS
    //RMS = fftAnalyser.sensorRead();

    /*
    //PRINT BUFFER RESULTS
    Serial.println("Buffer Results (arduino)");    
    for (int i = 0; i < fftSize/2; i++) {
        Serial.print((i * sampleRate) / fftSize);
        Serial.print("\t");
        Serial.print(spectrum[i]);
        Serial.println("");
    }
    Serial.println("*******");
    */

    Serial.println(resultDB);
    Serial.println("FreeRamMem\t" + String(FreeRamMem()));
}


