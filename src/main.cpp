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

#include "AudioAnalyser.h"
#include "FFTAnalyser.h"
#include "AudioInI2S.h"

///// FFT Parameters
const int fftSize = 64;
const int bitsPerSample = 32;
const int channels = 2;
const int bufferSize = 128;
const int sampleRate = 8000;

///// OUTPUT
int spectrum[fftSize/2];
double DB = 0;
///// DEFINE ANALYSER
FFTAnalyser fftAnalyser(bufferSize, fftSize, A_WEIGHTING);
//FIRAnalysis FIRAnalysis(bufferSize);

void setup() {
	// Open serial communications
	Serial.begin(115200);

 	// Configure Analysis
    if(!audioInI2SObject.begin(sampleRate, bitsPerSample)){
        Serial.println("Failed to init I2S");
    }

    if(!fftAnalyser.configure(audioInI2SObject)){
        Serial.println("Failed to init Analyser");
    }

    Serial.println("*******");
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
	//if (fftAnalyser.analyserAvailable()){

        //READ RMS AND SPECTRUM
		DB = fftAnalyser.sensorRead(spectrum);
        //READ ONLY RMS
        //RMS = fftAnalyser.sensorRead();

        /*
        Serial.println("Buffer Results (arduino)");    
	    for (int i = 0; i < fftSize/2; i++) {
            Serial.print((i * sampleRate) / fftSize);
            Serial.print("\t");
            Serial.print(spectrum[i]);
            Serial.println("");
        }
        
        Serial.println("rms_result\t" + String(DB) + " dB");
        Serial.println("--");
        Serial.println("*******");
        Serial.println("FreeRamMem\t" + String(FreeRamMem()));*/

        Serial.println(DB);
        Serial.println("FreeRamMem\t" + String(FreeRamMem()));
	//}
}


