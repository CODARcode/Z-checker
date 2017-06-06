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
	ZC_DataProperty* property;
	
	double compressTime;
	double compressRate;
	int compressSize;
	double compressRatio; /*compression factor = orig_size/compressed_size*/
	double rate; /*# bits to be represented for each data point*/
	
	double decompressTime;
	double decompressRate;
	
	double minAbsErr;
	double avgAbsErr;
	double maxAbsErr;
	double* autoCorrAbsErr;
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
	
	complex *fftCoeff;	
} ZC_CompareData;

void freeCompareResult(ZC_CompareData* compareData);
ZC_CompareData* ZC_constructCompareResult(char* varName, double compressTime, double compressRate, double compressRatio, double rate,
int compressSize, double decompressTime, double decompressRate, double minAbsErr, double avgAbsErr, double maxAbsErr,
double minRelErr, double avgRelErr, double maxRelErr, double rmse, double nrmse, double psnr, double snr, double valErrCorr, double pearsonCorr,
double* autoCorrAbsErr, double* absErrPDF);

void ZC_compareData_float(ZC_CompareData* compareResult, float* data1, float* data2, 
int r5, int r4, int r3, int r2, int r1);
void ZC_compareData_double(ZC_CompareData* compareResult, double* data1, double* data2,
int r5, int r4, int r3, int r2, int r1);
void ZC_compareData_dec(ZC_CompareData* compareResult, void *decData);
ZC_CompareData* ZC_compareData(char* varName, int dataType, void *oriData, void *decData, int r5, int r4, int r3, int r2, int r1);
void ZC_printCompressionResult(ZC_CompareData* compareResult);
char** constructCompareDataString(ZC_CompareData* compareResult);
void ZC_writeCompressionResult(ZC_CompareData* compareResult, char* solution, char* varName, char* tgtWorkspaceDir);
ZC_CompareData* ZC_loadCompressionResult(char* cmpResultFile);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_CompareData_H  ----- */
