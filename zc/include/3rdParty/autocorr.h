#ifndef AUTOCORR_H
#define AUTOCORR_H

// Function: autocorrelated3d()--autocorrelation of 3D arrays
// Author: Peter Lindstrom, Lawrence Livermore National Laboratory
// Date: January 10, 2018
// Dependencies: FFTW3 (http://www.fftw.org)
// 
// Compute autocorrelation function of a numerical array input[nz][ny][nx]
// of template type Real.  The autocorrelation is the self-convolution of
// the array after being normalized to have zero mean and unit variance.
// The output has the same dimensions as the input, with maximal correlation
// (zero lag) occurring at output[nz / 2][ny / 2][nx / 2] = 1.
//
// The algorithm exploits the convolution theorem, i.e., that convolution
// in the time domain is multiplication in the Fourier domain, and uses the
// FFTW3 implementation of the discrete Fourier transform.
//
// At a high level, the following steps are perfomed on the input, f:
//
//   g = (f - u(f)) / s(f)   normalize by subtracting mean, dividing by stddev
//   h = FFT(g)              forward Fourier transform (uses zero padding)
//   s = |h|^2               square magnitude
//   r = IFT(s) = g * g      inverse Fourier transform (self-convolution of g)
//
// The computation can be done in place by passing output = input.  When the
// macro DEBUG is defined, progress is reported to std::cerr.

#include <math.h>
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <fftw3.h>

void autocorrelate3d(double* output, double* input, size_t nx, size_t ny, size_t nz)
{
  // allocate scratch array, f, of dimensions (2 nx) * (2 ny) * (2 nz)
  
	size_t n = nx * ny * nz;
	double* f = (double*)malloc(sizeof(double)*n*8);
	memset(f, 0, sizeof(double)*n);
	size_t x, y, z, i, j, k;
	// compute mean, u, and variance, s
	double u = 0;
	double s = 0;
	double val = 0;
	double q;
	for (z = 0; z < nz; z++)
		for (y = 0; y < ny; y++)
			for (x = 0; x < nx; x++) 
			{
				val = input[x + nx * (y + ny * z)];
				f[x + 2 * nx * (y + 2 * ny * z)] = val;
				u += val;
				s += val * val;
			}
	u /= n;
	s /= n;
	s -= u * u;
	// guard against round-off error
	if (s < 0)
		s = 0;

	// normalize data
	q = sqrt(n * s);
	for (z = 0; z < nz; z++)
		for (y = 0; y < ny; y++)
			for (x = 0; x < nx; x++) 
			{
				i = x + 2 * nx * (y + 2 * ny * z);
				f[i] = (f[i] - u) / q;
			}

	// perform forward Fourier transform
	fftw_complex* g = (fftw_complex*)malloc(sizeof(fftw_complex)*(4 * (nx + 1) * ny * nz));
	fftw_plan plan = fftw_plan_dft_r2c_3d(2 * nz, 2 * ny, 2 * nx, f, g, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);		

	// square complex Fourier coefficients
	for (z = 0; z < 2 * nz; z++)
		for (y = 0; y < 2 * ny; y++)
			for (x = 0; x < nx + 1; x++) 
			{
				fftw_complex* p = &(g[x + (nx + 1) * (y + 2 * ny * z)]);
				(*p)[0] = (*p)[0] * (*p)[0] + (*p)[1] * (*p)[1];
				(*p)[1] = 0;
			}

	// perform inverse Fourier transform
	plan = fftw_plan_dft_c2r_3d(2 * nz, 2 * ny, 2 * nx, g, f, FFTW_ESTIMATE);
	fftw_execute(plan);
	fftw_destroy_plan(plan);
	fftw_cleanup();
	free(g);

	// copy result to output field
	for (k = 0; k < nz; k++) {
		z = (2 * nz + k - nz / 2) % (2 * nz);
		for (j = 0; j < ny; j++) {
			y = (2 * ny + j - ny / 2) % (2 * ny);
			for (i = 0; i < nx; i++) {
				x = (2 * nx + i - nx / 2) % (2 * nx);
				val = f[x + 2 * nx * (y + 2 * ny * z)] / (8 * n);
				output[i + nx * (j + ny * k)] = val;
			}
		}
	}
	free(f);
}

#endif
