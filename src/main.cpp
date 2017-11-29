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

#include <AudioI2S_SCK.h>

///// FFT Parameters
const int fftSize = 512;
const int bitsPerSample = 32;
const int channels = 2;
const int bufferReadSize = 512;
const int sampleRate = 44100;

///// OUTPUT
int spectrum[fftSize/2];
int spectrumDB[fftSize/2];

double rms_result = 0;
double rms_FilterADB = 0;

bool SpectrumDBOutput = false;
int type_weighting = 0; // 1 = A ; 0 = Z WEIGHTING
bool ScalingOutput = false;

///// DEFINE ANALYSER
FFTAnalyser fftAnalyser(fftSize, SpectrumDBOutput, type_weighting, ScalingOutput);
//FIRAnalysis FIRAnalysis(bufferReadSize);

void setup() {
	// Open serial communications
	Serial.begin(115200);
 	// Configure Analysis
    if (!AudioInI2S.begin(sampleRate, bitsPerSample)){
        Serial.println("Failed to init I2S");
    }

    if(!fftAnalyser.input(AudioInI2S)){
        Serial.println("Failed to init FFTAnalyser");
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
	if (fftAnalyser.Available()){

		rms_result = fftAnalyser.AudioSpectrumRead(spectrum, spectrumDB);
        //rms_FilterADB = FIRAnalysis.FIRRMSRead_dB();
        double read_done = millis()/1000;
        double update_called = fftAnalyser.UPDATE_TIME()/1000;
        double read_called = fftAnalyser.READ_TIME()/1000;

        /*
        Serial.println("Buffer Results (arduino)");    
	    for (int i = 0; i < fftSize/2; i++) {
            Serial.print((i * sampleRate) / fftSize);
            Serial.print("\t");
            Serial.print(spectrum[i]);
            if (SpectrumDBOutput){
                Serial.print("\t");
                Serial.print(spectrumDB[i]);
            }
            Serial.println("");
        }
        */
        Serial.println("rms_result\t" + String(rms_result) + " dBA");
        Serial.println("--");
        Serial.println("update_called\t" + String(update_called) + " s");
        Serial.println("read_called\t" + String(read_called) + " s");
        Serial.println("read_done\t" + String(read_done) + " s");

        //Serial.println("rms_FilterADB\t" + String(rms_FilterADB));
    
        Serial.println("*******");
    
        //Serial.println("FreeRamMem\t" + String(FreeRamMem()));
	}
}


