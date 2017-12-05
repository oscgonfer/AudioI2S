# AudioI2S

This is a library for the [Smart Citizen Kit V2.0](https://github.com/fablabbcn/smartcitizen-kit-20) for performing audio analysis.

It's intented to perform **audio analysis** on a SAMD21 processor of a **MEMS I2S microphone** output, with the idea of easily expanding it's functionality towards different inputs.

Additionally, the analysis can be performed through a configurable AudioAnalyser, containing a FFT analyser or a FIR (Finite Impulse Response) Filter.

## Features:

### FFT Analysis

1. Signal windowing through _configurable_ hann method
2. FFT analysis with arm's cortex funtion on q31_t data type
3. Spectrum calculation and _configurable_ weigthing through look-up table
4. Custom Spectrum Equalisation
6. RMS calculation
7. dB scale convertion

### FIR Filtering

1. Signal downscaling
2. Signal filtering (by chunks) with arm_fir custom filter including equalisation and a-weighting
3. RMS calculation


With kind references to:

 * [IoT_noise by Andre Filliettaz, 2016](https://github.com/andrentaz/iot-noise/tree/4eed813a5d868e7a4ff47a5fa0936d3415a5737d)
 * [SpectrumSerialPlotter by Sandeep Mistry, 2016](https://github.com/arduino-libraries/ArduinoSound/tree/master/examples)
 * [SPL meter - Davide Girone](http://davidegironi.blogspot.com.br/2014/02/a-simple-sound-pressure-level-meter-spl.html#.WCxnlHUrK03)
 
