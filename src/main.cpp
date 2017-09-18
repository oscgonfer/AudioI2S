#include <AudioI2S_SCK.h>

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

///// DEFINE OBJECT
AudioI2S_SCK AudioI2S_SCK(fftSize);

void setup() {
	// Open serial communications and wait for port to open:
	Serial.begin(115200);

 	// configure the I2S input as the input for the FFT analyzer
	if (!AudioI2S_SCK.ConfigureFilter(bitsPerSample, channels, bufferSize, sampleRate)){
		Serial.println("Failed to init the Audio!");
		while (1); // do nothing
	}

  Serial.println("Init Audio OK");
}

uint32_t FreeRamMem() {
    uint32_t stackTop;
    uint32_t heapTop;

    // current position of the stack.
    stackTop = (uint32_t) &stackTop;

    // current position of heap.
    void* hTop = malloc(1);
    heapTop = (uint32_t) hTop;
    free(hTop);

    // The difference is the free, available ram.
    return stackTop - heapTop;
}

void loop() {
	if (AudioI2S_SCK.available()){
		//rms_AspecBDB = AudioI2S_SCK.AudioSpectrumRead(spectrum, Aspectrum, spectrumDB, AspectrumDB, fftSize);
 		//rms_specBDB = AudioI2S_SCK.AudioRMSRead_dB();
    rms_FilterADB = AudioI2S_SCK.AudioTimeFilter();

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
    
    //Serial.println("rms_specBDB\t" + String(rms_specBDB));
    //Serial.println("rms_AspecBDB\t" + String(rms_AspecBDB));
    

    Serial.println("rms_FilterADB\t" + String(rms_FilterADB));
    
    Serial.println("*******");
    
    Serial.println("FreeRamMem\t" + String(FreeRamMem()));
	}
}