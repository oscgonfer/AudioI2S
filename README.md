# AudioI2S_SCK

Audio treatment of a I2S MEMs microphone for SCK

This library performs the audio analysis on a SAMD21 processor of a MEMS I2S microphone output.

# Features:

1. Signal downscaling
2. Signal windowing through hann method
3. FFT analysis with arm's cortex funtion on q31_t data type
4. Spectrum calculation and a-weigthing through look-up table
5. Spectrum equalisation
6. RMS calculation
7. dB scale convertion
