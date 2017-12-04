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
// #include "FIRAnalyser.h"
#include "AudioInI2S.h"

///// FFT Parameters
const int fftSize = 512;
const int bitsPerSample = 32;
const int channels = 2;
const int bufferSize = 512;
const int sampleRate = 44100;

///// OUTPUT
int spectrum[fftSize/2];
double DB = 0;
int timer = 0;

///// DEFINE ANALYSER
FFTAnalyser fftAnalyser(bufferSize, fftSize, A_WEIGHTING);
// FIRAnalyser firAnalyser(bufferSize);

void setup() {
	// Open serial communications
	SerialUSB.begin(115200);

    // BLINK LED
    pinMode(LED_BUILTIN, OUTPUT);

 	// Configure Analysis
    if(!audioInI2SObject.begin(sampleRate, bitsPerSample)){
        SerialUSB.println("Failed to init I2S");
    }

    if(!fftAnalyser.configure(audioInI2SObject)){
        SerialUSB.println("Failed to init Analyser");
    }

    // if(!firAnalyser.configure(audioInI2SObject)){
    //     SerialUSB.println("Failed to init Analyser");
    // }

    SerialUSB.println("*******");
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
	//if (fftAnalyser.analyserAvailable()){

    while (timer < 30000000){
        timer++;
    }
    timer = 0;

    //READ RMS AND SPECTRUM
	DB = fftAnalyser.sensorRead(spectrum);
    //READ ONLY RMS
    // DB = fftAnalyser.sensorRead();

    // DB = firAnalyser.sensorRead();

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(15);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(15);                       // wait for a second
    
    /*
    SerialUSB.println("Buffer Results (arduino)");    
    for (int i = 0; i < fftSize/2; i++) {
        SerialUSB.print((i * sampleRate) / fftSize);
        SerialUSB.print("\t");
        SerialUSB.print(spectrum[i]);
        SerialUSB.println("");
    }

    SerialUSB.println("--");*/

    SerialUSB.println(DB);
    SerialUSB.println(FreeRamMem());
    SerialUSB.println("--");

}


