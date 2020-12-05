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
#include "ZC_Hashtable.h"
#include "ZC_rw.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NB_CMPR_CASES 64
#define MAX_VIS_DEC_CRS 64

#define DECVIS_ERROR_SELECT_NEAREST 0
#define DECVIS_ERROR_LINEAR_APPROX 1
#define DECVIS_ERROR_LIBPRESSIO_OPT 2

#define ZC_ABS 0
#define ZC_REL 1

typedef struct ZC_CompareData
{	
	char* solution; //the key string of the ZC_CompareData
	double errorBound;
	ZC_DataProperty* property;
	
	int compressionMode; //ZC_REL or ZC_ABS
	
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
	
	//separate analysis for all the derivative matrices along different dimensions
	
	double maxDx;
	double maxDy;
	double maxDz;
	double maxDt;
	
	double maxErrDx;
	double maxErrDy; 
	double maxErrDz; 
	double maxErrDt;  //Max error of dx, dy, dx, dt
	double psnrDx; 
	double psnrDy; 
	double psnrDz; 
	double psnrDt;  //PSNR of dx, dy, dx, dt
	double ssimDx; 
	double ssimDy; 
	double ssimDz; 
	double ssimDt;	
	
	//aggregated analysis for the derivatives
	double derivativeOrder1_psnr;
	double derivativeOrder2_psnr;
	double derivativeOrder1_ssim;
	double derivativeOrder2_ssim;
	
	double derivative1_sobolev;
	
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

typedef struct ZCVisDecDataElement
{
	char* varName; //filled when initialized
	char* compressorName; //filled when initialized
	int compressorID;
	ZC_DataProperty* dataProperty; //filled when initialized
	
	double errorSetting; //to be filled after analysis
	char* errorSetting_str;
	char* compressionCaseName; //to be filled after analysis
	void* sliceImage_dec_ori; //decompressed data for original domain - to be filled after analysis
	void* sliceImage_dec_log; //decompressed data for log domain - to be filled after analysis
	
	ZC_CompareData* compressionResult; //to be filled after analysis
	
} ZCVisDecDataElement;

typedef struct CompressorCRVisElement
{
	int resultCount;
	char* varName;
	char* compressorName;
	ZC_CompareData *compressionResults[MAX_NB_CMPR_CASES];
	hashtable_t* CRVisDataMap;
} CompressorCRVisElement;

int freeCompareResult(ZC_CompareData* compareData);
void freeCompareResult_internal(ZC_CompareData* compareData);

ZC_CompareData* ZC_constructCompareResult(const char* varName, double compressTime, double compressRate, double compressRatio, double rate,
size_t compressSize, double decompressTime, double decompressRate, double minAbsErr, double avgAbsErr, double maxAbsErr, 
double minRelErr, double avgRelErr, double maxRelErr, double rmse, double nrmse, double psnr, double snr, double valErrCorr, double pearsonCorr,
double* autoCorrAbsErr, double* absErrPDF, int compressionMode, 
double ssimImage2D_min, double ssimImage2D_avg, double ssimImage2D_max, 
double maxDx, double maxDy, double maxDz, double maxDt,
double maxErrDx, double maxErrDy, double maxErrDz, double maxErrDt, double psnrDx, double psnrDy, double psnrDz, double psnrDt, 
double ssimDx, double ssimDy, double ssimDz, double ssimDt, 
double derivativeOrder1_psnr, double derivativeOrder2_psnr, double derivativeOrder1_ssim, double derivativeOrder2_ssim, double derivative1_sobolev);

void ZC_compareData_float(ZC_CompareData* compareResult, float* data1, float* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_compareData_double(ZC_CompareData* compareResult, double* data1, double* data2,
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

void ZC_computeFFT_float_offline(ZC_CompareData* compareResult, float* data1, float* data2, size_t numOfElem);
void ZC_computeFFT_double_offline(ZC_CompareData* compareResult, double* data1, double* data2, size_t numOfElem);

void ZC_compareData_dec(ZC_CompareData* compareResult, void *decData);
ZC_CompareData* ZC_compareData(char* varName, int dataType, void *oriData, void *decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_printCompressionResult(ZC_CompareData* compareResult);
char** constructCompareDataString(ZC_CompareData* compareResult, int* nbLines);
void ZC_writeCompressionResult(ZC_CompareData* compareResult, char* solution, char* varName, const char* tgtWorkspaceDir);
ZC_CompareData* ZC_loadCompressionResult(char* cmpResultFile);

ZC_CompareData_Overall* ZC_compareData_overall();

double computeErrorSetting(const int compressorID, 
const double targetCR, const int nbPoints, double* sortedCompressionRatios, double* errorSettings,
void* data, const int dataType, const size_t r5, const size_t r4, const size_t r3, const size_t r2, const size_t r1, 
const float lower_bound, const float upper_bound);

void print_cmprVisE(CompressorCRVisElement* cmprVisE);
StringLine* write_cmprVisE(CompressorCRVisElement* cmprVisE);
void ZC_itentifyErrorSettingBasedOnCR(CompressorCRVisElement* cmprVisE);
void free_ZCVisDecDataElement(ZCVisDecDataElement* visEle);
void free_CompressorCRVisElement(CompressorCRVisElement* cmprVisE);

//mpi interfaces
void ZC_compareData_float_online(ZC_CompareData* compareResult, float* data1, float* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_compareData_double_online(ZC_CompareData* compareResult, double* data1, double* data2, 
size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_CompareData_H  ----- */
