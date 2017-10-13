#include <AudioI2S_SCK.h>

///// FFT Parameters
const uint32_t fftSize = 512;
const int bitsPerSample = 32;
const int channels = 2;
const int bufferReadSize = 512;
const int sampleRate = 44100;

///// OUTPUT
int spectrum[fftSize/2];
int Aspectrum[fftSize/2];
int spectrumDB[fftSize/2];
int AspectrumDB[fftSize/2];

double rms_specBDB = 0;
double rms_AspecBDB = 0;
double rms_FilterADB = 0;

bool SpectrumDBOutput = false;

///// DEFINE OBJECT
FFTAnalyser fftAnalyser(fftSize, SpectrumDBOutput);

void setup() {
	// Open serial communications
	Serial.begin(115200);
 	// Configure Analysis
    if (!AudioInI2S.begin(sampleRate, bitsPerSample, bufferReadSize)){
        Serial.println("Failed to init I2S");
    };

    if(!fftAnalyser.input(AudioInI2S)){
        Serial.println("Failed to init FFTAnalyser");
    }
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
		rms_AspecBDB = fftAnalyser.AudioSpectrumRead(spectrum, Aspectrum, spectrumDB, AspectrumDB);
 		rms_specBDB = fftAnalyser.AudioRMSRead_dB();
        //rms_FilterADB = FIRAnalyser.FIRRMSRead_dB();

    /*
        Serial.println("Buffer Results (arduino)");    
	for (int i = 0; i < fftSize/2; i++) {
        Serial.print((i * sampleRate) / fftSize);
        Serial.print("\t");
        Serial.print(spectrum[i]);
    	Serial.print("\t");
    	Serial.print(Aspectrum[i]);
    	Serial.print("\t");
    	Serial.print(spectrumDB[i]);
  		Serial.print("\t");
  		Serial.println(AspectrumDB[i]);
    }
    */

    Serial.println("*******");
    
    Serial.println("rms_specBDB\t" + String(rms_specBDB));
    Serial.println("rms_AspecBDB\t" + String(rms_AspecBDB));
    //Serial.println("rms_FilterADB\t" + String(rms_FilterADB));
    
    Serial.println("*******");
    
    Serial.println("FreeRamMem\t" + String(FreeRamMem()));
	}
}


