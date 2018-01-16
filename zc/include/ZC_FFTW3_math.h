/**
 *  @file ZC_FFTW3_math.h
 *  @author Sheng Di
 *  @date Jan, 2018
 *  @brief Header file for ZC_R_math.c.
 *  (C) 2018 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_FFTW3_MATH_H
#define _ZC_FFTW3_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

double* autocorr_3d_double(double* input, size_t nx, size_t ny, size_t nz);
float* autocorr_3d_float(float* input, size_t nx, size_t ny, size_t nz);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_FFTW3_MATH_H  ----- */
