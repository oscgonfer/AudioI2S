# smartcitizen-kit-audio

This is a library for the [Smart Citizen Kit V2.0](https://github.com/fablabbcn/smartcitizen-kit-20) for performing audio analysis.

It's intented to perform **audio analysis** on a SAMD21 processor of a **MEMS I2S microphone** output. The signal analysis is performed through a FFTAnalyser.

## Features:

### FFT Analysis and Weighting

1. Signal windowing through hann method
2. FFT analysis with arm's cortex funtion on q31_t data type
3. Spectrum calculation and _configurable_ weigthing through look-up table (A, C or Z)
4. Custom Spectrum Equalisation
6. RMS calculation and dB scale Convertion

### A-C Weighting functions

There is an octave script in /OCTAVE/WEIGHTING that generates a custom weighting table as specifies in the actual legislation. It outputs a 

### Communication through sound

WIP

**NB**

For further functionalities, such as configurable Audio Analysis through FFT, FIR filter and so on, visit [here](https://github.com/oscgonfer/AudioI2S).

----

With kind references to:

 * [IoT_noise by Andre Filliettaz, 2016](https://github.com/andrentaz/iot-noise/tree/4eed813a5d868e7a4ff47a5fa0936d3415a5737d)
 * [SpectrumSerialPlotter by Sandeep Mistry, 2016](https://github.com/arduino-libraries/ArduinoSound/tree/master/examples)
 * [SPL meter - Davide Girone](http://davidegironi.blogspot.com.br/2014/02/a-simple-sound-pressure-level-meter-spl.html#.WCxnlHUrK03)
 
