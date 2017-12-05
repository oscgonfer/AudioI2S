# AudioI2S

This is the base library for the [Smart Citizen Kit V2.0](https://github.com/fablabbcn/smartcitizen-kit-20) for performing audio analysis.

It's intented to perform **audio analysis** on a SAMD21 processor of a **MEMS I2S microphone** output, with the idea of easily expanding it's functionality towards different inputs.

Additionally, the analysis can be performed through a configurable AudioAnalyser, containing a FFT analyser or a FIR (Finite Impulse Response) Filter.

## Features:

### FFT Analysis

1. Signal windowing through _configurable_ window method (Hann, Blackman, Hamming)
2. FFT analysis with arm's cortex funtion on q31_t data type
3. Spectrum calculation and _configurable_ weigthing (A, C or Z)
4. Custom Spectrum Equalisation (With auto generated parameters in a octave script)
6. RMS calculation
7. dB scale convertion

### FIR Filtering

1. Signal downscaling
2. Signal filtering (by chunks) with arm_fir custom filter including equalisation and weighting
3. RMS calculation

### A-C Weighting functions

There is an octave script in /OCTAVE/WEIGHTING that generates a custom weighting table as specifies in the actual legislation. It outputs a vector containing the corresponding factors to apply in the spectrum, according to the specifies sampling frequency / fft bins.

---

With kind references to:

 * [IoT_noise by Andre Filliettaz, 2016](https://github.com/andrentaz/iot-noise/tree/4eed813a5d868e7a4ff47a5fa0936d3415a5737d)
 * [SpectrumSerialPlotter by Sandeep Mistry, 2016](https://github.com/arduino-libraries/ArduinoSound/tree/master/examples)
 * [SPL meter - Davide Girone](http://davidegironi.blogspot.com.br/2014/02/a-simple-sound-pressure-level-meter-spl.html#.WCxnlHUrK03)