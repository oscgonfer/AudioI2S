# AudioI2S/Octave

This folder contains octave scripts in order to auto-generate .h files with custom coefficients for the Audio Analysis.

## FFT Spectrum Equalisation

Depending on the microphone frequency response, it would be necessary to equalise the spectrum:

1. Generate a .csv file with two columns: the frequency vector and the corresponding desired correction. Vector lenght is not fixed. Take into account the correction will be applied by division:

```
    spectrumBuffer /= equalisationTable;
```

2. Save it in this same directory
3. Execute EQUALISATION.m in order to generate corresponding .h file for the analyser.

## FIR Filter Coefficients Generation

Generate the desired filter coefficients for the FIR filter 

1. Generate a .csv file with two columns: the frequency vector and the corresponding desired equalisation. Vector lenght is not fixed. Take into account the correction will be applied by division:

```
    spectrumBuffer /= equalisationTable;
```

2. Save it in this same directory
3. Open FILTER_CREATE.m and choose which weighting function you would like to implement (A, C or Z (no weighting))
4. Execute it and review the filter created with respect to its desired response

## Weighting Functions

These octave functions create a look up table with the desired sample rate and fft bins for A or C weighting, using UNE-EN ISO 61672.

The output in octave looks like this:

![](https://i.imgur.com/UYb5g7E.png)

And they generate a csv file that you can use for implementation or other things.