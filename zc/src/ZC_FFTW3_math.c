#include "3rdParty/autocorr.h"

/**
 * Compute 3D auto correlation (calling the function implemented by Peter Linstrom)
 *  
 * */
 
double* autocorr_3d_double(double* input, size_t nx, size_t ny, size_t nz)
{
	double* output = (double*)malloc(sizeof(double)*nx*ny*nz);
	autocorrelate3d(output, input, nx, ny, nz);
	return output;
}

float* autocorr_3d_float(float* input, size_t nx, size_t ny, size_t nz)
{
	size_t i, nbEle = nx*ny*nz;
	double* output = (double*)malloc(sizeof(double)*nbEle);
	double* input2 = (double*)malloc(sizeof(double)*nbEle);
	for(i=0;i<nbEle;i++)
		input2[i] = input[i];
	
	autocorrelate3d(output, input2, nx, ny, nz);
	free(input2);
	float* output2 = (float*)malloc(sizeof(float)*nbEle);
	for(i=0;i<nbEle;i++)
		output2[i] = output[i];
	free(output);
	return output2;
}


