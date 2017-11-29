/*
 This example reads audio data from an Invensense's ICS43432 I2S microphone
 breakout board, and prints out the spectrum to the Serial console. The
 Serial Plotter built into the Arduino IDE can be used to plot the audio
 amplitude data (Tools -> Serial Plotter)
 
 Circuit:
 * Arduino/Genuino Zero, MKRZero or MKR1000 board
 * ICS43432:
 * GND connected GND
 * 3.3V connected 3.3V (Zero) or VCC (MKR1000, MKRZero)
 * WS connected to pin 0 (Zero) or pin 3 (MKR1000, MKRZero)
 * CLK connected to pin 1 (Zero) or pin 2 (MKR1000, MKRZero)
 * SD connected to pin 9 (Zero) or pin A6 (MKR1000, MKRZero)
 
 created 21 November 2016
 by Sandeep Mistry
 */
#include <ArduinoSoundSCKV3.h>
#include <I2S_SCK.h>

//#define Serial SerialUSB

int mic_type = 0;
// sample rate for the input
const int sampleRate = 44100;

// size of the FFT to compute
const int fftSize = 512;

// size of the spectrum output, half of FFT size
const int spectrumSize = fftSize / 2;

// array to store spectrum output //to move to all-q31_t?
int spectrum[spectrumSize];
int Aspectrum[spectrumSize];

// SENSOR OUTPUT
int RMS_DBFS = 0;
int RMS_DBSPL = 0;
int RMS_DBFS_A = 0;
int RMS_DBSPL_A = 0;
int RMS_DBSPL_A_avg = 0;

// create an FFT analyzer to be used with the I2S input
FFTAnalyzer fftAnalyzer(fftSize);
int bitsPerSample = 32;
int timer_init = 1000; //ms
int counter = 0;
float weighting_factor = 0.5;
bool printed = false;
int total_sampling = 50000;

void setup() {
    // Open serial communications and wait for port to open:
    // A baud rate of 115200 is used instead of 9600 for a faster data rate
    // on non-native USB ports
    Serial.begin(115200);
    
    // setup the I2S audio input for the sample rate with 32-bits per sample
    if (!AudioInI2S.begin(sampleRate, bitsPerSample, timer_init, total_sampling, mic_type)) {
        Serial.println("Failed to initialize I2S input!");
        while (1); // do nothing
    }
    
    // configure the I2S input as the input for the FFT analyzer
    if (!fftAnalyzer.input(AudioInI2S)) {
        Serial.println("Failed to set FFT analyzer input!");
        while (1); // do nothing
    }
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
    // check if a new analysis is available
    if (fftAnalyzer.available()) {
        // read the new spectrum
        fftAnalyzer.read (spectrum, Aspectrum, spectrumSize);
        
        /*
         RMS_DBFS = fftAnalyzer.read_amp();
         RMS_DBFS_A = fftAnalyzer.read_amp_a_weight();
         
         RMS_DBSPL = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-log10(RMS_DBFS)*20);
         RMS_DBSPL_A = FULL_SCALE_DBSPL-(FULL_SCALE_DBFS-log10(RMS_DBFS_A)*20);
         */
        
        //Serial.println("*****");
        // print out the spectrum
        /*
         for (int i = 0; i < spectrumSize; i++) {
         //Serial.print(millis());
         //Serial.print("\t"); //
         Serial.print((i * sampleRate) / fftSize); // the starting frequency
         Serial.print("\t"); //
         Serial.print(spectrum[i]); // the spectrum value
         Serial.print("\t"); //
         Serial.println(Aspectrum[i]); // the spectrum value
         }
         
         Serial.println("*****");
         
         Serial.println("Amplitude DBFS: " +String(RMS_DBFS) + " dB FS" );
         Serial.println("Amplitude DBFS A-weighted: " +String(RMS_DBFS_A) + " dBA FS" );
         Serial.println("RMS Value in DB SPL : " + String(RMS_DBSPL) + " dB SPL" );
         Serial.println("RMS Value in DB SPL w Aweighting " + String(RMS_DBSPL_A) +  + " dBA SPL" );
         Serial.println('-');
         */
        
        /*
         if(!(RMS_DBSPL_A < -2014748364)) {
         counter = counter + 1;
         if (counter == 1) {
         //Serial.println("/counter = " + String(counter));
         RMS_DBSPL_A_avg = RMS_DBSPL_A;
         //Serial.println("/RMS_DBSPL_A_avg = " + String(RMS_DBSPL_A_avg));
         } else {
         //Serial.println("//counter = " + String(counter));
         RMS_DBSPL_A_avg = RMS_DBSPL_A*weighting_factor+RMS_DBSPL_A_avg*(1-weighting_factor);
         //Serial.println("//RMS_DBSPL_A_avg = " + String(RMS_DBSPL_A_avg));
         }
         }
         */
        
        /*
         if (!printed) {
         Serial.println("Amplitude A-weighted: " + String(RMS_DBSPL_A_avg) + " dBA SPL" );
         Serial.println("FreeRamMem : " + String(FreeRamMem()));
         printed = true;
         }
         
         Serial.println("*****");*/
    }
}
