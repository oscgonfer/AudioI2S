#include <AudioAnalysis.h>

///// FFT Parameters
const uint32_t fftSize = 512;
const int bitsPerSample = 32;
const int channels = 2;
const int bufferSize = 512;
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
AudioAnalysis AudioAnalysis(sampleRate, bufferSize, channels, bitsPerSample);

void setup() {
	// Open serial communications
	Serial.begin(115200);
 	// Configure Analysis
    AudioAnalysis.SetAnalysis(FFTAnalysis);
}

void loop() {
	if (AudioAnalysis.available()){
		rms_AspecBDB = AudioAnalysis.AudioSpectrumRead(spectrum, Aspectrum, spectrumDB, AspectrumDB, fftSize);
 		rms_specBDB = AudioAnalysis.AudioRMSRead_dB();
    //rms_FilterADB = AudioAnalysis.AudioTimeFilter();

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