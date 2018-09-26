/**
 *  @file ZC_CompareData.h
 *  @author Sheng Di
 *  @date July, 2016
 *  @brief Header file for the ZC_CompareData.c.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_CompareData_H
#define _ZC_CompareData_H

#include "ZC_DataProperty.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZC_CompareData
{	
	char* solution; //the key string of the ZC_CompareData
	ZC_DataProperty* property;
	
	void* dec_data;
	double compressTime;
	double compressRate;
	size_t compressSize;
	double compressRatio; /*compression factor = orig_size/compressed_size*/
	double rate; /*# bits to be represented for each data point*/
	
	double decompressTime;
	double decompressRate;
	
	double minAbsErr;
	double avgAbsErr;
	double maxAbsErr;
	double* autoCorrAbsErr;
	double* autoCorrAbsErr3D;
	double* absErrPDF; /*keep the distribution of errors (1000 elements)*/
	double* pwrErrPDF;
	double err_interval;
	double err_interval_rel;
	double err_minValue;
	double err_minValue_rel;
	
	double minRelErr;
	double avgRelErr;
	double maxRelErr;
	
	double minPWRErr;
	double avgPWRErr;
	double maxPWRErr;

	double snr;

	double rmse;
	double nrmse;
	double psnr;

	double valErrCorr;

	double pearsonCorr;
	
	double ksValue; //result of KS_test
	//ssim assessment: lum,cont,struc,ssim
	double lum;
	double cont;
	double struc;
	double ssim;
	
	double ssimImage2D_min;
	double ssimImage2D_avg;
	double ssimImage2D_max;
	
	complex *fftCoeff;	
} ZC_CompareData;

typedef struct ZC_CompareData_Overall
{
	size_t numOfVar;
	
	double originalSize;
	
	double compressTime;
	double compressRate;
	size_t compressSize;
	double compressRatio; /*compression factor = orig_size/compressed_size*/
	double rate; /*# bits to be represented for each data point*/
	
	double decompressTime;
	double decompressRate;
	
	double minAbsErr;
	double avgAbsErr;
	double maxAbsErr;
	//double* autoCorrAbsErr;
	//double* absErrPDF; /*keep the distribution of errors (1000 elements)*/
	//double* pwrErrPDF;
	//double err_interval;
	//double err_interval_rel;
	//double err_minValue;
	//double err_minValue_rel;
	
	double minRelErr;
	double avgRelErr;
	double maxRelErr;
	
	double minPWRErr;
	double avgPWRErr;
	double maxPWRErr;

	//double snr;

	double rmse;
	double nrmse;
	double psnr;

	//double valErrCorr;

	double min_pearsonCorr;
	double avg_pearsonCorr;
	double max_pearsonCorr;
} ZC_CompareData_Overall;

int freeCompareResult(ZC_CompareData* compareData);
void freeCompareResult_internal(ZC_CompareData* compareData);

ZC_CompareData* ZC_constructCompareResult(char* varName, double compressTime, double compressRate, double compressRatio, double rate,
size_t compressSize, double decompressTime, double decompressRate, double minAbsErr, double avgAbsErr, double maxAbsErr, 
double minRelErr, double avgRelErr, double maxRelErr, double rmse, double nrmse, double psnr, double snr, double valErrCorr, double pearsonCorr,
double* autoCorrAbsErr, double* absErrPDF);

void ZC_compareData_float(ZC_CompareData* compareResult, float* data1, float* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_compareData_double(ZC_CompareData* compareResult, double* data1, double* data2,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

void ZC_computeFFT_float_offline(ZC_CompareData* compareResult, float* data1, float* data2, size_t numOfElem);
void ZC_computeFFT_double_offline(ZC_CompareData* compareResult, double* data1, double* data2, size_t numOfElem);

void ZC_compareData_dec(ZC_CompareData* compareResult, void *decData);
ZC_CompareData* ZC_compareData(char* varName, int dataType, void *oriData, void *decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_printCompressionResult(ZC_CompareData* compareResult);
char** constructCompareDataString(ZC_CompareData* compareResult);
void ZC_writeCompressionResult(ZC_CompareData* compareResult, char* solution, char* varName, char* tgtWorkspaceDir);
ZC_CompareData* ZC_loadCompressionResult(char* cmpResultFile);

ZC_CompareData_Overall* ZC_compareData_overall();

//mpi interfaces
void ZC_compareData_float_online(ZC_CompareData* compareResult, float* data1, float* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_compareData_double_online(ZC_CompareData* compareResult, double* data1, double* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_CompareData_H  ----- */
