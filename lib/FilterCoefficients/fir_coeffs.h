
#ifndef __FIR_COEFFS__
#define __FIR_COEFFS__

#define FIR_COEFFS_LEN    101

const uint16_t FILTERSIZE = FIR_COEFFS_LEN;
const uint32_t FILTERBLOCKSIZE = 15;

extern float32_t firCoeffs[FIR_COEFFS_LEN];

#endif
