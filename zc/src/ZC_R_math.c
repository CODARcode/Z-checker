#include "ZC_callR.h"
#include "zc.h"

void ZC_R_process(char* funName, int dim, int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outLen, double** out)
{
	switch(dim)
	{
	case 1:
		switch(dataType)
		{
		case ZC_FLOAT:
			ZC_callR_2_1d(funName, dataType, r1, (float*)oriData, r1, (float*)decData, outLen, out);
			break;
		case ZC_DOUBLE:
			ZC_callR_2_1d(funName, dataType, r1, (double*)oriData, r1, (double*)decData, outLen, out);
			break;
		}
		break;
	case 2:
		switch(dataType)
		{
		case ZC_FLOAT:
			ZC_callR_2_2d(funName, dataType, r2, r1, (float*)oriData, r2, r1, (float*)decData, outLen, out);
			break;
		case ZC_DOUBLE:
			ZC_callR_2_2d(funName, dataType, r2, r1, (double*)oriData, r2, r1, (double*)decData, outLen, out);
			break;
		}		
		break;
	case 3:
		switch(dataType)
		{
		case ZC_FLOAT:
			ZC_callR_2_3d(funName, dataType, r3, r2, r1, (float*)oriData, r3, r2, r1, (float*)decData, outLen, out);
			break;
		case ZC_DOUBLE:
			ZC_callR_2_3d(funName, dataType, r3, r2, r1, (double*)oriData, r3, r2, r1, (double*)decData, outLen, out);
			break;
		}				
		break;
	case 4:
		switch(dataType)
		{
		case ZC_FLOAT:
			ZC_callR_2_3d(funName, dataType, r4*r3, r2, r1, (float*)oriData, r4*r3, r2, r1, (float*)decData, outLen, out);
			break;
		case ZC_DOUBLE:
			ZC_callR_2_3d(funName, dataType, r4*r3, r2, r1, (double*)oriData, r4*r3, r2, r1, (double*)decData, outLen, out);
			break;
		}				
		break;
	case 5:
		switch(dataType)
		{
		case ZC_FLOAT:
			ZC_callR_2_3d(funName, dataType, r5*r4*r3, r2, r1, (float*)oriData, r5*r4*r3, r2, r1, (float*)decData, outLen, out);
			break;
		case ZC_DOUBLE:
			ZC_callR_2_3d(funName, dataType, r5*r4*r3, r2, r1, (double*)oriData, r5*r4*r3, r2, r1, (double*)decData, outLen, out);
			break;
		}				
		break;	
	}	
}

double KS_test(int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t outLen = 0;
	double *out = NULL;
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	ZC_R_process("KS_test", dim, dataType, oriData, decData, r5, r4, r3, r2, r1, &outLen, &out);
	double result = 0;
	if(out!=NULL)	
		result = *out;
	else
		result = -1; //error
	free(out);
	return result;
}

/**
 * Compute SSIM for two data sets (original data and decompressed data)
 * 
 * @return an array with 4 elements: lum,cont,struc,ssim
 * 
 * 
 * */
double* SSIM(int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t outLen = 0;
	double *out = NULL;
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	ZC_R_process("SSIM", dim, dataType, oriData, decData, r5, r4, r3, r2, r1, &outLen, &out);
	return out;	
}

/**
 * Compute SSIM for two data sets (original data and decompressed data)
 * 
 * @return an array with 4 elements: lum,cont,struc,ssim
 * 
 * 
 * */
double* SSIM3(int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t outLen = 0;
	double *out = NULL;
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	ZC_R_process("SSIM3", dim, dataType, oriData, decData, r5, r4, r3, r2, r1, &outLen, &out);
	return out;	
}
