# AudioAnalyser

Audio treatment of a I2S MEMs microphone for the Smart Citizen Kit 2.0

This library performs the audio analysis on a SAMD21 processor of a MEMS I2S microphone output.

# Features:

FFT Analysis

1. Signal downscaling
2. Signal windowing through hann method
3. FFT analysis with arm's cortex funtion on q31_t data type
4. Spectrum calculation and a-weigthing through look-up table
5. Spectrum equalisation
6. RMS calculation
7. dB scale convertion

FIR Filtering

1. Signal downscaling
2. Signal filtering (by chunks) with arm_fir custom filter including equalisation and a-weighting
3. RMS calculation

With references to
 * IoT_noise by Andre Filliettaz, 2016 https://github.com/andrentaz/iot-noise/tree/4eed813a5d868e7a4ff47a5fa0936d3415a5737d
 * SpectrumSerialPlotter by Sandeep Mistry, 2016 https://github.com/arduino-libraries/ArduinoSound/tree/master/examples
 * SPL meter - Davide Girone: http://davidegironi.blogspot.com.br/2014/02/a-simple-sound-pressure-level-meter-spl.html#.WCxnlHUrK03
 
