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
const int fftSize = 512; //IF THIS CHANGES EQUALISATION TABLE CHANGE

//// AUDIO I2S Parameters
const int bitsPerSample = 32; //CONSTANT FOR ICS43432 (24bit)
const int bufferSize = 512;
const int sampleRate = 44100; //Hz

///// OUTPUT
int spectrum[fftSize/2];
double resultDB = 0;

///// Analyser
FFTAnalyser fftAnalyser(bufferSize, fftSize, A_WEIGHTING, HANN);
//FIRAnalysis FIRAnalysis(bufferSize, HANN);

void setup() {
	// Open serial communications
	SerialUSB.begin(115200);

 	// Configure Audio Input
    if(!audioInI2SObject.begin(sampleRate, bitsPerSample)){
        SerialUSB.println("Failed to init I2S");
    }

    // Configure Analyser
    if(!fftAnalyser.configure(audioInI2SObject)){
        SerialUSB.println("Failed to init Analyser");
    }

    SerialUSB.println("-------------");
    SerialUSB.println("Init Audio OK");

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
	// resultDB = fftAnalyser.sensorRead(spectrum);
    //READ ONLY RMS
    resultDB = fftAnalyser.sensorRead();

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

    SerialUSB.println(resultDB);
    SerialUSB.println("FreeRamMem\t" + String(FreeRamMem()));
    SerialUSB.println("--");
}


