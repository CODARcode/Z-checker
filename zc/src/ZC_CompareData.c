#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "ZC_util.h"
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "ZC_quicksort.h"
#include "zc.h"
#include "iniparser.h"
#ifdef HAVE_ONLINEVIS
#include "zserver.h"
#endif
#ifdef HAVE_LIBPRESSIOOPT
#include "ZC_LibpressioOpt.h"
#endif

void freeCompareResult_internal(ZC_CompareData* compareData)
{
	//free(compareData->property);
	if(compareData==NULL)
		return;
	if(compareData->solution!=NULL)
		free(compareData->solution);
	if(compareData->autoCorrAbsErr!=NULL)
		free(compareData->autoCorrAbsErr);
	if(compareData->autoCorrAbsErr3D!=NULL)
		free(compareData->autoCorrAbsErr3D);
	if(compareData->absErrPDF!=NULL)
		free(compareData->absErrPDF);
	if(compareData->pwrErrPDF!=NULL)
		free(compareData->pwrErrPDF);
	if(compareData->fftCoeff!=NULL)
		free(compareData->fftCoeff);
	free(compareData);
}

int freeCompareResult(ZC_CompareData* compareData)
{
	if(compareData==NULL)
		return 0;
	char* key = compareData->solution;
	if(key==NULL)
		return 0;
	ZC_CompareData* found = NULL;
	if(ecCompareDataTable!=NULL)
		found = (ZC_CompareData*)ht_freePairEntry(ecCompareDataTable, key);
	if(found==NULL)
	{
		freeCompareResult_internal(compareData);
		return 0;
	}
	else
	{
		freeCompareResult_internal(found);
		return 1;
	}
}

ZC_CompareData* ZC_constructCompareResult(const char* varName, double compressTime, double compressRate, double compressRatio, double rate,
size_t compressSize, double decompressTime, double decompressRate, double minAbsErr, double avgAbsErr, double maxAbsErr, 
double minRelErr, double avgRelErr, double maxRelErr, double rmse, double nrmse, double psnr, double snr, double valErrCorr, double pearsonCorr,
double* autoCorrAbsErr, double* absErrPDF, int compressionMode, 
double ssimImage2D_min, double ssimImage2D_avg, double ssimImage2D_max, 
double maxDx, double maxDy, double maxDz, double maxDt,
double maxErrDx, double maxErrDy, double maxErrDz, double maxErrDt, double psnrDx, double psnrDy, double psnrDz, double psnrDt, 
double ssimDx, double ssimDy, double ssimDz, double ssimDt, 
double derivativeOrder1_psnr, double derivativeOrder2_psnr, double derivativeOrder1_ssim, double derivativeOrder2_ssim, double derivative1_sobolev)
{
	ZC_CompareData* result = (ZC_CompareData*)malloc(sizeof(struct ZC_CompareData));
	memset(result, 0, sizeof(struct ZC_CompareData));

	//TODO: get the dataProperty based on varName from the hashtable.
	result->property = (ZC_DataProperty*)ht_get(ecPropertyTable, varName);
	result->compressionMode = compressionMode;
	result->dec_data = NULL;
	result->compressTime = compressTime;
	result->compressRate = compressRate;
	result->compressRatio = compressRatio;
	result->rate = rate;
	result->compressSize = compressSize;
	result->decompressTime = decompressTime;
	result->decompressRate = decompressRate;
	result->minAbsErr = minAbsErr;
	result->avgAbsErr = avgAbsErr;
	result->maxAbsErr = maxAbsErr;
	result->minRelErr = minRelErr;
	result->avgRelErr = avgRelErr;
	result->maxRelErr = maxRelErr;
	result->rmse = rmse;
	result->nrmse = nrmse;
	result->psnr = psnr;
	result->snr = snr;
	result->valErrCorr = valErrCorr;
	result->pearsonCorr = pearsonCorr;
	result->autoCorrAbsErr = autoCorrAbsErr;
	result->autoCorrAbsErr3D = NULL;
	result->absErrPDF = absErrPDF;
	result->pwrErrPDF = NULL;
	result->fftCoeff = NULL;
	result->ssimImage2D_min = ssimImage2D_min;
	result->ssimImage2D_avg = ssimImage2D_avg;
	result->ssimImage2D_max = ssimImage2D_max;
	
	result->maxDx = maxDx;
	result->maxDy = maxDy;
	result->maxDz = maxDz;
	result->maxDt = maxDt;
	
	result->maxErrDx = maxErrDx;
	result->maxErrDy= maxErrDy;
	result->maxErrDz = maxErrDz;
	result->maxErrDt = maxErrDt;
	result->psnrDx = psnrDx;
	result->psnrDy= psnrDy;
	result->psnrDz = psnrDz;
	result->psnrDt = psnrDt;
	result->ssimDx = ssimDx;
	result->ssimDy = ssimDy;
	result->ssimDz = ssimDz;
	result->ssimDt = ssimDt;		
	
	result->derivativeOrder1_psnr = derivativeOrder1_psnr;
	result->derivativeOrder2_psnr = derivativeOrder2_psnr;
	result->derivativeOrder1_ssim = derivativeOrder1_ssim;
	result->derivativeOrder2_ssim = derivativeOrder2_ssim;
	
	result->derivative1_sobolev = derivative1_sobolev;
	return result;
}

void ZC_computeFFT_float_offline(ZC_CompareData* compareResult,float* data1, float* data2, size_t numOfElem)
{
	size_t fft_size = pow(2,(int)log2(numOfElem));
	 
	ZC_Complex* fftCoeff1 = ZC_computeFFT(data1, fft_size, ZC_FLOAT);
	ZC_Complex* fftCoeff2 = ZC_computeFFT(data2, fft_size, ZC_FLOAT);
	ZC_Complex* fftCoeffRelDiff = (ZC_Complex*)malloc(FFT_SIZE*sizeof(ZC_Complex));
	size_t i;
	fftCoeffRelDiff[0].Re = fabs((fftCoeff2[0].Re - fftCoeff1[0].Re)/fftCoeff1[0].Re);
	fftCoeffRelDiff[0].Im = 0;
	fftCoeffRelDiff[0].Amp= fabs((fftCoeff2[0].Amp - fftCoeff1[0].Amp)/fftCoeff1[0].Amp);
	for (i = 1; i < FFT_SIZE; i++)
	{
		fftCoeffRelDiff[i].Re = fabs((fftCoeff2[i].Re - fftCoeff1[i].Re)/fftCoeff1[i].Re);
		fftCoeffRelDiff[i].Im = fabs((fftCoeff2[i].Im - fftCoeff1[i].Im)/fftCoeff1[i].Im);
		fftCoeffRelDiff[i].Amp= fabs((fftCoeff2[i].Amp - fftCoeff1[i].Amp)/fftCoeff1[i].Amp);
	}			
	compareResult->fftCoeff = fftCoeffRelDiff;
	free(fftCoeff1);
	free(fftCoeff2);	
}

void ZC_computeFFT_double_offline(ZC_CompareData* compareResult, double* data1, double* data2, size_t numOfElem)
{
	size_t fft_size = pow(2,(int)log2(numOfElem));
	 
	ZC_Complex* fftCoeff1 = ZC_computeFFT(data1, fft_size, ZC_FLOAT);
	ZC_Complex* fftCoeff2 = ZC_computeFFT(data2, fft_size, ZC_FLOAT);
	ZC_Complex* fftCoeffRelDiff = (ZC_Complex*)malloc(FFT_SIZE*sizeof(ZC_Complex));
	size_t i;
	fftCoeffRelDiff[0].Re = fabs((fftCoeff2[0].Re - fftCoeff1[0].Re)/fftCoeff1[0].Re);
	fftCoeffRelDiff[0].Im = 0;
	fftCoeffRelDiff[0].Amp= fabs((fftCoeff2[0].Amp - fftCoeff1[0].Amp)/fftCoeff1[0].Amp);
	for (i = 1; i < FFT_SIZE; i++)
	{
		fftCoeffRelDiff[i].Re = fabs((fftCoeff2[i].Re - fftCoeff1[i].Re)/fftCoeff1[i].Re);
		fftCoeffRelDiff[i].Im = fabs((fftCoeff2[i].Im - fftCoeff1[i].Im)/fftCoeff1[i].Im);
		fftCoeffRelDiff[i].Amp= fabs((fftCoeff2[i].Amp - fftCoeff1[i].Amp)/fftCoeff1[i].Amp);
	}			
	compareResult->fftCoeff = fftCoeffRelDiff;
	free(fftCoeff1);
	free(fftCoeff2);	
}

void ZC_compareData_dec(ZC_CompareData* compareResult, void *decData)
{
	if(compareResult==NULL)
	{
		printf("Error: compareResult cannot be NULL\n");
		exit(0);
	}
	int dataType = compareResult->property->dataType;
	void* oriData = compareResult->property->data;	
	size_t r5 = compareResult->property->r5;
	size_t r4 = compareResult->property->r4;
	size_t r3 = compareResult->property->r3;
	size_t r2 = compareResult->property->r2;
	size_t r1 = compareResult->property->r1;
    size_t numOfElem = compareResult->property->numOfElem;

	if(dataType==ZC_FLOAT)
	{
		float* data1 = (float*)oriData;
		float* data2 = (float*)decData;
		
#ifdef HAVE_MPI
		if(executionMode == ZC_OFFLINE)
		{
			if(fftFlag)
				ZC_computeFFT_float_offline(compareResult,data1, data2, numOfElem);
			ZC_compareData_float(compareResult, data1, data2, r5, r4, r3, r2, r1);
		}
		else //ZC_ONLINE
		{
			ZC_compareData_float_online(compareResult, data1, data2, r5, r4, r3, r2, r1);	
		}
#else
		if(fftFlag)
			ZC_computeFFT_float_offline(compareResult,data1, data2, numOfElem);
		ZC_compareData_float(compareResult, data1, data2, r5, r4, r3, r2, r1);			
#endif
	}
	else if(dataType==ZC_DOUBLE)
	{
		double* data1 = (double*)oriData;
		double* data2 = (double*)decData;

#ifdef HAVE_MPI
		if(executionMode == ZC_OFFLINE)
		{
			if(fftFlag)
				ZC_computeFFT_double_offline(compareResult, data1, data2, numOfElem);	
			ZC_compareData_double(compareResult, data1, data2, r5, r4, r3, r2, r1);
		}
		else
		{
			ZC_compareData_double_online(compareResult, data1, data2, r5, r4, r3, r2, r1);	
		}
#else
		if(fftFlag)
			ZC_computeFFT_double_offline(compareResult, data1, data2, numOfElem);
		ZC_compareData_double(compareResult, data1, data2, r5, r4, r3, r2, r1);
#endif
	}
	else
	{
		printf("Error 1: dataType is wrong! (dataType = %d)\n", dataType);
		exit(0);
	}
}

ZC_CompareData* ZC_compareData(char* varName, int dataType, void *oriData, void *decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	ZC_CompareData* compareResult = (ZC_CompareData*)malloc(sizeof(ZC_CompareData));
	memset(compareResult, 0, sizeof(ZC_CompareData));
	//size_t numOfElem = ZC_computeDataLength(r5, r4, r3, r2, r1);
	
	if(dataType==ZC_FLOAT)
	{
		float* data1 = (float*)oriData;
		float* data2 = (float*)decData;
		
#ifdef HAVE_MPI		
		if(executionMode==ZC_OFFLINE)
		{
			//compareResult->property = ZC_genProperties_float(varName, data1, numOfElem, r5, r4, r3, r2, r1);
			compareResult->property = ZC_startCmpr(varName, ZC_FLOAT, data1, r5, r4, r3, r2, r1);
			ZC_compareData_float(compareResult, data1, data2, r5, r4, r3, r2, r1);
		}//ZC_ONLINE
		else
		{
			compareResult->property = ZC_startCmpr_online(varName, ZC_FLOAT, data1, r5, r4, r3, r2, r1);
			ZC_compareData_float_online(compareResult, data1, data2, r5, r4, r3, r2, r1);			
		}
#else
		compareResult->property = ZC_startCmpr(varName, ZC_FLOAT, data1, r5, r4, r3, r2, r1);
		ZC_compareData_float(compareResult, data1, data2, r5, r4, r3, r2, r1);
#endif	
	}
	else if(dataType==ZC_DOUBLE)
	{
		double* data1 = (double*)oriData;
		double* data2 = (double*)decData;
		
#ifdef HAVE_MPI		
		if(executionMode==ZC_OFFLINE)
		{
			//compareResult->property = ZC_genProperties_float(varName, data1, numOfElem, r5, r4, r3, r2, r1);
			compareResult->property = ZC_startCmpr(varName, ZC_DOUBLE, data1, r5, r4, r3, r2, r1);
			ZC_compareData_double(compareResult, data1, data2, r5, r4, r3, r2, r1);
		}//ZC_ONLINE
		else
		{
			compareResult->property = ZC_startCmpr_online(varName, ZC_DOUBLE, data1, r5, r4, r3, r2, r1);
			ZC_compareData_double_online(compareResult, data1, data2, r5, r4, r3, r2, r1);
		}
#else
		compareResult->property = ZC_startCmpr(varName, ZC_DOUBLE, data1, r5, r4, r3, r2, r1);
		ZC_compareData_double(compareResult, data1, data2, r5, r4, r3, r2, r1);
#endif		
	}
	else
	{
		printf("Error 2: dataType is wrong! (dataType == %d)\n", dataType);
		exit(0);
	}
	return compareResult;
}

void ZC_printCompressionResult(ZC_CompareData* compareResult)
{
	printf("minAbsErr: %f\n", compareResult->minAbsErr);
	printf("avgAbsErr: %f\n", compareResult->avgAbsErr);
	printf("maxAbsErr: %f\n", compareResult->maxAbsErr);
}

char** constructCompareDataString(ZC_CompareData* compareResult, int* nbLines)
{
	char** s = (char**)malloc(56*sizeof(char*)); //total of 51 lines at most
	//memset(s, NULL, 50);
	
	s[0] = (char*)malloc(100*sizeof(char));
	sprintf(s[0], "[COMPARE]\n");	
	
	s[1] = (char*)malloc(100*sizeof(char));
	if(compareResult->property!=NULL)
		sprintf(s[1], "varName = %s\n", compareResult->property->varName);
	else
		sprintf(s[1], "varName = -\n");
			
	s[2] = (char*)malloc(100*sizeof(char));
	sprintf(s[2], "compressTime = %.10G\n", compareResult->compressTime);
	s[3] = (char*)malloc(100*sizeof(char));
	sprintf(s[3], "compressRate = %.10G\n", compareResult->compressRate);
	s[4] = (char*)malloc(100*sizeof(char));
	sprintf(s[4], "compressRatio = %f\n", compareResult->compressRatio);
	s[5] = (char*)malloc(100*sizeof(char));
	sprintf(s[5], "rate = %f\n", compareResult->rate);			
	s[6] = (char*)malloc(100*sizeof(char));
	sprintf(s[6], "compressSize = %zu\n", compareResult->compressSize);
	
	s[7] = (char*)malloc(100*sizeof(char));
	sprintf(s[7], "decompressTime = %.10G\n", compareResult->decompressTime);
	s[8] = (char*)malloc(100*sizeof(char));
	sprintf(s[8], "decompressRate = %.10G\n", compareResult->decompressRate);			
		
	s[9] = (char*)malloc(100*sizeof(char));
	sprintf(s[9], "minAbsErr = %.10G\n", compareResult->minAbsErr);
	s[10] = (char*)malloc(100*sizeof(char));
	sprintf(s[10], "avgAbsErr = %.10G\n", compareResult->avgAbsErr);
	s[11] = (char*)malloc(100*sizeof(char));
	sprintf(s[11], "maxAbsErr = %.10G\n", compareResult->maxAbsErr);
	
	s[12] = (char*)malloc(100*sizeof(char));
	if(errAutoCorrFlag)
		sprintf(s[12], "errAutoCorr = %.10G\n", (compareResult->autoCorrAbsErr)[1]); //TODO output AUTO_CORR_SIZE coefficients
	else
		sprintf(s[12], "errAutoCorr = -\n");
		
	s[13] = (char*)malloc(100*sizeof(char));
	sprintf(s[13], "minRelErr = %.10G\n", compareResult->minRelErr);
	s[14] = (char*)malloc(100*sizeof(char));
	sprintf(s[14], "avgRelErr = %.10G\n", compareResult->avgRelErr);
	s[15] = (char*)malloc(100*sizeof(char));
	sprintf(s[15], "maxRelErr = %.10G\n", compareResult->maxRelErr);

	s[16] = (char*)malloc(100*sizeof(char));
	sprintf(s[16], "minPWRErr = %.10G\n", compareResult->minPWRErr);
	s[17] = (char*)malloc(100*sizeof(char));
	sprintf(s[17], "avgPWRErr = %.10G\n", compareResult->avgPWRErr);
	s[18] = (char*)malloc(100*sizeof(char));
	sprintf(s[18], "maxPWRErr = %.10G\n", compareResult->maxPWRErr);	

	s[19] = (char*)malloc(100*sizeof(char));
	sprintf(s[19], "rmse = %.10G\n", compareResult->rmse);
	s[20] = (char*)malloc(100*sizeof(char));
	sprintf(s[20], "nrmse = %.10G\n", compareResult->nrmse);
	s[21] = (char*)malloc(100*sizeof(char));
	sprintf(s[21], "psnr = %.10G\n", compareResult->psnr);
	s[22] = (char*)malloc(100*sizeof(char));
	sprintf(s[22], "snr = %.10G\n", compareResult->snr);	

	s[23] = (char*)malloc(100*sizeof(char));
	sprintf(s[23], "valErrCorr = %.10G\n", compareResult->valErrCorr);

	s[24] = (char*)malloc(100*sizeof(char));
	sprintf(s[24], "pearsonCorr = %.10G\n", compareResult->pearsonCorr);


	int index = 25;
#ifdef HAVE_R
	s[index] = (char*)malloc(100*sizeof(char));
	sprintf(s[index], "KS_test = %.10G\n", compareResult->ksValue);
	index++;
	s[index] = (char*)malloc(100*sizeof(char));
	sprintf(s[index], "lum = %.10G\n", compareResult->lum);
	index++;
	s[index] = (char*)malloc(100*sizeof(char));
	sprintf(s[index], "cont = %.10G\n", compareResult->cont);
	index++;
	s[index] = (char*)malloc(100*sizeof(char));
	sprintf(s[index], "struc = %.10G\n", compareResult->struc);
	index++;
	s[index] = (char*)malloc(100*sizeof(char));
	sprintf(s[index], "ssim = %.10G\n", compareResult->ssim);					
	index++;
#else
/*	s[25] = (char*)malloc(100*sizeof(char));
	strcpy(s[25], "KS_test = -\n");
	s[26] = (char*)malloc(100*sizeof(char));
	strcpy(s[26], "lum = -\n");
	s[27] = (char*)malloc(100*sizeof(char));
	strcpy(s[27], "cont = -\n");
	s[28] = (char*)malloc(100*sizeof(char));
	strcpy(s[28], "struc = -\n");
	s[29] = (char*)malloc(100*sizeof(char));
	strcpy(s[29], "ssim = -\n");*/
#endif

	if(compareResult->property->r2==0)
	{
/*		strcpy(s[30], "ssimImage2D_min = -\n");
		strcpy(s[31], "ssimImage2D_avg = -\n");
		strcpy(s[32], "ssimImage2D_max = -\n");*/
	}
	else if(compareResult->property->r3==0)
	{
		//strcpy(s[30], "ssimImage2D_min = -\n");		
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "ssimImage2D_avg = %.10G\n", compareResult->ssimImage2D_avg);
		index++;
		//strcpy(s[32], "ssimImage2D_max = -\n");
	}
	else
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index++], "ssimImage2D_min = %.10G\n", compareResult->ssimImage2D_min);
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index++], "ssimImage2D_avg = %.10G\n", compareResult->ssimImage2D_avg);
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index++], "ssimImage2D_max = %.10G\n", compareResult->ssimImage2D_max);						
	}

	if(derivativeOrder1_sep_maxDiffFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxDx = %.10G\n", compareResult->maxDx);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxDy = %.10G\n", compareResult->maxDy);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxDz = %.10G\n", compareResult->maxDz);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxDt = %.10G\n", compareResult->maxDt);	
		index++;		
		
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxErrDx = %.10G\n", compareResult->maxErrDx);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxErrDy = %.10G\n", compareResult->maxErrDy);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxErrDz = %.10G\n", compareResult->maxErrDz);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "maxErrDt = %.10G\n", compareResult->maxErrDt);	
		index++;							
	}
	if(derivativeOrder1_sep_psnrFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "psnrDx = %.10G\n", compareResult->psnrDx);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "psnrDy = %.10G\n", compareResult->psnrDy);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "psnrDz = %.10G\n", compareResult->psnrDz);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "psnrDt = %.10G\n", compareResult->psnrDt);	
		index++;							
	}
	if(derivativeOrder1_sep_ssimFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "ssimDx = %.10G\n", compareResult->ssimDx);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "ssimDy = %.10G\n", compareResult->ssimDy);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "ddimDz = %.10G\n", compareResult->ssimDz);	
		index++;	
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "ssimDt = %.10G\n", compareResult->ssimDt);	
		index++;							
	}		
	if(derivativeOrder1_psnrFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "derivativeOrder1_psnr = %.10G\n", compareResult->derivativeOrder1_psnr);
		index++;
	}
	
	if(derivativeOrder2_psnrFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));	
		sprintf(s[index], "derivativeOrder2_psnr = %.10G\n", compareResult->derivativeOrder2_psnr);	
		index++;
	}

	if(derivativeOrder1_ssimFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "derivativeOrder1_ssim = %.10G\n", compareResult->derivativeOrder1_ssim);
		index++;
	}
	
	if(derivativeOrder2_ssimFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "derivativeOrder2_ssim = %.10G\n", compareResult->derivativeOrder2_ssim);	
		index++;
	}
	
	if(derivativeOrder1_sobolevFlag)
	{
		s[index] = (char*)malloc(100*sizeof(char));
		sprintf(s[index], "derivative1_sobolev = %.10G\n", compareResult->derivative1_sobolev);	
		index++;		
	}
	s[index] = (char*)malloc(100*sizeof(char));
	sprintf(s[index], "compressionMode = %d\n", compareResult->compressionMode);
	index++;
	*nbLines = index;
	return s;
}

void ZC_writeCompressionResult(ZC_CompareData* compareResult, char* solution, char* varName, const char* tgtWorkspaceDir)
{
#if HAVE_ONLINEVIS
  return;
#endif
	int nbLines = 0;
	char** s = constructCompareDataString(compareResult, &nbLines);
	char varName_[ZC_BUFS];
	strcpy(varName_, varName);
	ZC_ReplaceStr2(varName_, "_", "\\\\_");
	DIR *dir = opendir(tgtWorkspaceDir);
	if(dir==NULL)
		mkdir(tgtWorkspaceDir,0775);
	char tgtFilePath[ZC_BUFS_LONG];
	sprintf(tgtFilePath, "%s/%s:%s.cmp", tgtWorkspaceDir, solution, varName); 
	ZC_writeStrings(nbLines, s, tgtFilePath);
	int i;
	for(i=0;i<nbLines;i++)
		free(s[i]);
	free(s);
	//write the pdf
	
	if(absErrPDFFlag)
	{
		double err_interval = compareResult->err_interval;
		double err_minValue = compareResult->err_minValue;		
		memset(tgtFilePath, 0, ZC_BUFS_LONG);
		sprintf(tgtFilePath, "%s/%s:%s.dis", tgtWorkspaceDir, solution, varName);
	
		if(err_interval==0)
		{
			char *ss[2];
			ss[0] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(ss[0], "x %s:%s-PDF\n", solution, varName_);				
			ss[1] = (char*)malloc(sizeof(char)*ZC_BUFS);
			strcpy(ss[1],"0 1\n");
			ZC_writeStrings(2, ss, tgtFilePath);
			free(ss[0]);
			free(ss[1]);
		}
		else
		{
			char *ss[PDF_INTERVALS+1];		
			ss[0] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(ss[0], "x %s:%s-PDF\n", solution, varName_);		
			for(i=0;i<PDF_INTERVALS;i++)
			{
				//printf("%d\n", i);
				ss[i+1] = (char*)malloc(sizeof(char)*ZC_BUFS);
				double x = err_minValue+i*err_interval;
				sprintf(ss[i+1], "%.10G %.10G\n", x, compareResult->absErrPDF[i]); 
			}
			ZC_writeStrings(PDF_INTERVALS+1, ss, tgtFilePath);
			for(i=0;i<PDF_INTERVALS+1;i++)
				free(ss[i]);			
		}	
	}
	if(pwrErrPDFFlag)
	{
		double err_interval = compareResult->err_interval_rel;
		double err_minValue = compareResult->err_minValue_rel;		
		
		memset(tgtFilePath, 0, ZC_BUFS_LONG);
		sprintf(tgtFilePath, "%s/%s:%s.pds", tgtWorkspaceDir, solution, varName);
		if(err_interval==0)
		{
			char *ss[2];
			ss[0] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(ss[0], "x %s:%s-PDF\n", solution, varName_);				
			strcpy(ss[1],"0 1\n");
			ZC_writeStrings(2, ss, tgtFilePath);
			free(ss[0]);	
		}
		else
		{
			char *ss[PDF_INTERVALS_REL+1];		
			ss[0] = (char*)malloc(sizeof(char)*ZC_BUFS_LONG);
			sprintf(ss[0], "x %s:%s-PDF\n", solution, varName_);
			for(i=0;i<PDF_INTERVALS_REL;i++)
			{
				ss[i+1] = (char*)malloc(sizeof(char)*ZC_BUFS);
				double x = err_minValue+i*err_interval;
				sprintf(ss[i+1], "%.10G %.10G\n", x, compareResult->pwrErrPDF[i]); 
			}
			
			ZC_writeStrings(PDF_INTERVALS_REL+1, ss, tgtFilePath);
			for(i=0;i<PDF_INTERVALS_REL+1;i++)
				free(ss[i]);
		}	
	}	
	//write auto-correlation coefficients
	if(errAutoCorrFlag)
	{
		char *autocorr[AUTOCORR_SIZE+2];
		autocorr[0] = (char*)malloc(sizeof(char)*ZC_BUFS);
		sprintf(autocorr[0], "x \"\"\n");
		autocorr[1] = (char*)malloc(sizeof(char)*ZC_BUFS);
		sprintf(autocorr[1], "## %d %.10G\n", 0, (compareResult->autoCorrAbsErr)[0]);	 //don't present autocorr[1] (i.e., x=0), because it's always 1.
		for (i = 2; i < AUTOCORR_SIZE+2; i++)
		{
			autocorr[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(autocorr[i], "%d %.10G\n", i-1, (compareResult->autoCorrAbsErr)[i-1]);
		}
		memset(tgtFilePath, 0, ZC_BUFS_LONG);
		sprintf(tgtFilePath, "%s/%s:%s.autocorr", tgtWorkspaceDir, solution, varName);
		ZC_writeStrings(AUTOCORR_SIZE+2, autocorr, tgtFilePath);
		for (i = 0; i < AUTOCORR_SIZE+2; i++)
			free(autocorr[i]);		
		
	}
#ifdef HAVE_FFTW3	
	if(errAutoCorr3DFlag)
	{
		memset(tgtFilePath, 0, ZC_BUFS);
		sprintf(tgtFilePath, "%s/%s:%s.ac3d", tgtWorkspaceDir, solution, varName);
		ZC_writeDoubleData_inBytes(compareResult->autoCorrAbsErr3D, compareResult->property->numOfElem, tgtFilePath);			
	}
#endif	
	if(fftFlag)
	{
		char buf[ZC_BUFS];
		sprintf(buf, "%s:%s", solution, varName);
		ZC_writeFFTResults(buf, compareResult->fftCoeff, tgtWorkspaceDir);		
	}	
	if(dir!=NULL)
		closedir(dir);
}

ZC_CompareData* ZC_loadCompressionResult(char* cmpResultFile)
{
	//TODO put the information into the hashtable, named ecPropertyTable
	dictionary *ini;
	char* par;
    //printf("[ZC] Reading compareResultFile (%s) ...\n", cmpResultFile);
    if (access(cmpResultFile, F_OK) != 0)
    {
        printf("[ZC] cmpResultFile: %s NOT accessible.\n", cmpResultFile);
        exit(1);
    }
    ini = iniparser_load(cmpResultFile);
    if (ini == NULL)
    {
        printf("[ZC] Iniparser failed to parse the conf. file: %s.\n", cmpResultFile);
        exit(1);
    }
	
	par = iniparser_getstring(ini, "COMPARE:varName", NULL);
	char* var = (char*)malloc(ZC_BUFS);
	snprintf(var, ZC_BUFS,"%s", par);
	
	int compressionMode = (int)iniparser_getint(ini, "COMPARE:compressionMode", 0);
	double compressTime = (double)iniparser_getdouble(ini, "COMPARE:compressTime", 0);
	double compressRate = (double)iniparser_getdouble(ini, "COMPARE:compressRate", 0);
	double compressRatio = (double)iniparser_getdouble(ini, "COMPARE:compressRatio", 0);
	double rate = (double)iniparser_getdouble(ini, "COMPARE:rate", 0);
	int compressSize = (int)iniparser_getint(ini, "COMPARE:compressSize", 0);
	double decompressTime = (double)iniparser_getdouble(ini, "COMPARE:decompressTime", 0);
	double decompressRate = (double)iniparser_getdouble(ini, "COMPARE:decompressRate", 0);
	
	double minAbsErr = (double)iniparser_getdouble(ini, "COMPARE:minAbsErr", 0);
	double avgAbsErr = (double)iniparser_getdouble(ini, "COMPARE:avgAbsErr", 0);
	double maxAbsErr = (double)iniparser_getdouble(ini, "COMPARE:maxAbsErr", 0);
	
	double minRelErr = (double)iniparser_getdouble(ini, "COMPARE:minRelErr", 0);
	double avgRelErr = (double)iniparser_getdouble(ini, "COMPARE:avgRelErr", 0);
	double maxRelErr = (double)iniparser_getdouble(ini, "COMPARE:maxRelErr", 0);
	
	double rmse = (double)iniparser_getdouble(ini, "COMPARE:rmse", 0);
	double nrmse = (double)iniparser_getdouble(ini, "COMPARE:nrmse", 0);
	double psnr = (double)iniparser_getdouble(ini, "COMPARE:psnr", 0);
	double snr = (double)iniparser_getdouble(ini, "COMPARE:snr", 0);

	double valErrCorr = (double)iniparser_getdouble(ini, "COMPARE:valErrCorr", 0);

	double pearsonCorr = (double)iniparser_getdouble(ini, "COMPARE:pearsonCorr", 0); 
	
	//TODO: Read zfp-test2.autocorr for filling in sol-var.autocorr such as zfp-test2.autocorr
	double* autoCorrAbsErr = NULL;
	
	//TODO: Read more data from distribution-of-err file (zfp-test2.dis) for filling PDF
	double* absErrPDF = NULL;
		
	double ssimImage2D_min = (double)iniparser_getdouble(ini, "COMPARE:ssimImage2D_min", 0);	
	double ssimImage2D_avg = (double)iniparser_getdouble(ini, "COMPARE:ssimImage2D_avg", 0);	
	double ssimImage2D_max = (double)iniparser_getdouble(ini, "COMPARE:ssimImage2D_max", 0);

	double maxDx = (double)iniparser_getdouble(ini, "COMPARE:maxDx", -1);
	double maxDy = (double)iniparser_getdouble(ini, "COMPARE:maxDy", -1);
	double maxDz = (double)iniparser_getdouble(ini, "COMPARE:maxDz", -1);
	double maxDt = (double)iniparser_getdouble(ini, "COMPARE:maxDt", -1);  
	
	double maxErrDx = (double)iniparser_getdouble(ini, "COMPARE:maxErrDx", -1);
	double maxErrDy = (double)iniparser_getdouble(ini, "COMPARE:maxErrDy", -1);
	double maxErrDz = (double)iniparser_getdouble(ini, "COMPARE:maxErrDz", -1);
	double maxErrDt = (double)iniparser_getdouble(ini, "COMPARE:maxErrDt", -1);  
	double psnrDx = (double)iniparser_getdouble(ini, "COMPARE:psnrDx", -1);
	double psnrDy = (double)iniparser_getdouble(ini, "COMPARE:psnrDy", -1);
	double psnrDz = (double)iniparser_getdouble(ini, "COMPARE:psnrDz", -1);
	double psnrDt = (double)iniparser_getdouble(ini, "COMPARE:psnrDt", -1);
	double ssimDx = (double)iniparser_getdouble(ini, "COMPARE:ssimDx", -1);
	double ssimDy = (double)iniparser_getdouble(ini, "COMPARE:ssimDy", -1);
	double ssimDz = (double)iniparser_getdouble(ini, "COMPARE:ssimDz", -1);
	double ssimDt = (double)iniparser_getdouble(ini, "COMPARE:ssimDt", -1);
	
	double derivativeOrder1_psnr = (double)iniparser_getdouble(ini, "COMPARE:derivativeOrder1_psnr", -1);	
	double derivativeOrder2_psnr = (double)iniparser_getdouble(ini, "COMPARE:derivativeOrder2_psnr", -1);
	double derivativeOrder1_ssim = (double)iniparser_getdouble(ini, "COMPARE:derivativeOrder1_ssim", -1);
	double derivativeOrder2_ssim = (double)iniparser_getdouble(ini, "COMPARE:derivativeOrder2_ssim", -1);
		
	double derivative1_sobolev = (double)iniparser_getdouble(ini, "COMPARE:derivative1_sobolev", -1);	
		
	ZC_CompareData* compareResult = ZC_constructCompareResult(var, 
	compressTime, compressRate, compressRatio, rate, 
	compressSize, decompressTime, decompressRate, minAbsErr, avgAbsErr, maxAbsErr, minRelErr, avgRelErr, maxRelErr, 
	rmse, nrmse, psnr, snr, valErrCorr, pearsonCorr, autoCorrAbsErr, absErrPDF, compressionMode, 
	ssimImage2D_min, ssimImage2D_avg, ssimImage2D_max, maxDx, maxDy, maxDz, maxDt,
	maxErrDx, maxErrDy, maxErrDz, maxErrDt, psnrDx, psnrDy, psnrDz, psnrDt, ssimDx, ssimDy, ssimDz, ssimDt,
	derivativeOrder1_psnr, derivativeOrder2_psnr, derivativeOrder1_ssim, derivativeOrder2_ssim, derivative1_sobolev);
	
	iniparser_freedict(ini);
	
	free(var);
	return compareResult;
}

ZC_CompareData_Overall* ZC_compareData_overall()
{
	ZC_CompareData_Overall* result = (ZC_CompareData_Overall*)malloc(sizeof(ZC_CompareData_Overall));
	
	if(ecCompareDataTable==NULL || ht_getElemCount(ecCompareDataTable) == 0)
	{
		printf("Error: there are no elements registered. Please use ZC_registerVar() to register variables.\n");
		exit(0);
	}
	
	int count = ecCompareDataTable->count;
	result->numOfVar = count;
	int i, j = 0;
	
	//extract all the compression results from the hashtable
	ZC_CompareData** compressDataList = (ZC_CompareData**)malloc(sizeof(ZC_CompareData*)*count);
	for(i=0;i<ecCompareDataTable->capacity&&j<count;i++)
	{
		entry_t* t = ecCompareDataTable->table[i];
		while(t!=NULL)
		{
			compressDataList[j++] = (ZC_CompareData*)t->value;
			t = t->next;
		}
	}
	
	//Start the overall analysis
	size_t total_OriSize = 0;
	size_t total_CompressSize = 0;
	double overall_ComprsRatio = 0;
	double total_ComprsTime = 0;
	double total_DecmprTime = 0;
	double overall_ComprsRate = 0;
	double overall_DecmprRate = 0;
	
	double overall_Rate = 0;

	double overall_minAbsErr = 1E20;
	double overall_avgAbsErr = 0;
	double overall_maxAbsErr = 0;
	double overall_minRelErr = 1;
	double overall_avgRelErr = 0;
	double overall_maxRelErr = 0;

	double overall_PSNR = 0;

	double overall_rmse = 0;
	double overall_nrmse = 0;
	
	double overall_minPearsonCorr = 1;
	double overall_avgPearsonCorr = 0;
	double overall_maxPearsonCorr = -1;
	

	size_t total_numOfElem = 0;
	double sumSE = 0; //sum of squared error
	double sumNRMSE_Sq = 0;
	double rmse = 0, nrmse = 0;
	int dataType, typeSize = 0;
	
	for(i=0;i<count;i++)
	{
		dataType = compressDataList[i]->property->dataType;
		if(dataType==ZC_FLOAT)
			typeSize = 4;
		else if(dataType==ZC_DOUBLE)
			typeSize = 8;
		else
		{
			printf("Error: No such a data type: %d\n", dataType);
			exit(0);
		}	
		
		total_numOfElem += compressDataList[i]->property->numOfElem;
		total_OriSize += compressDataList[i]->property->numOfElem * typeSize;
		total_CompressSize += compressDataList[i]->compressSize;
		total_ComprsTime += compressDataList[i]->compressTime;
		total_DecmprTime += compressDataList[i]->decompressTime;
		if(overall_minAbsErr > compressDataList[i]->minAbsErr)
			overall_minAbsErr = compressDataList[i]->minAbsErr;
		if(overall_maxAbsErr < compressDataList[i]->maxAbsErr)
			overall_maxAbsErr = compressDataList[i]->maxAbsErr;
		overall_avgAbsErr += compressDataList[i]->avgAbsErr;

		if(overall_minPearsonCorr > compressDataList[i]->minRelErr)
			overall_minPearsonCorr = compressDataList[i]->minRelErr;
		if(overall_maxRelErr < compressDataList[i]->maxRelErr)
			overall_maxRelErr = compressDataList[i]->maxRelErr;
		overall_avgRelErr += compressDataList[i]->avgRelErr;	
		
		if(overall_minRelErr > compressDataList[i]->pearsonCorr)
			overall_minRelErr = compressDataList[i]->pearsonCorr;
		if(overall_maxPearsonCorr < compressDataList[i]->pearsonCorr)
			overall_maxPearsonCorr = compressDataList[i]->pearsonCorr;
		overall_avgPearsonCorr += compressDataList[i]->pearsonCorr;			
		
		rmse = compressDataList[i]->rmse;
		nrmse = compressDataList[i]->nrmse;
		sumSE += rmse*rmse*compressDataList[i]->property->numOfElem;
		
		sumNRMSE_Sq += nrmse*nrmse;
	}
	
	overall_avgAbsErr /= total_numOfElem;
	overall_avgRelErr /= total_numOfElem;
	
	overall_ComprsRatio = ((double)total_OriSize)/((double)total_CompressSize);
	overall_Rate = (total_OriSize/total_numOfElem*8)/overall_ComprsRatio;
	
	overall_ComprsRate = total_OriSize/total_ComprsTime; //in B/s
	overall_DecmprRate = total_OriSize/total_DecmprTime;
	
	overall_rmse = sqrt(sumSE/total_numOfElem);
	overall_nrmse = sqrt(sumNRMSE_Sq/total_numOfElem);
	
	overall_PSNR = 20*log10(1.0/overall_nrmse);
	
	//copy data to result
	result->numOfVar = count;
	result->originalSize = total_OriSize;
	result->compressSize = total_CompressSize;
	result->compressRatio = overall_ComprsRatio;
	
	result->compressTime = total_ComprsTime;
	result->decompressTime = total_DecmprTime;
	result->compressRate = overall_ComprsRate;
	result->decompressRate = overall_DecmprRate;
	result->rate = overall_Rate;
	result->minAbsErr = overall_minAbsErr;
	result->avgAbsErr = overall_avgAbsErr;
	result->maxAbsErr = overall_maxAbsErr;
	result->minRelErr = overall_minRelErr;
	result->avgRelErr = overall_avgRelErr;
	result->maxRelErr = overall_maxRelErr;
	
	result->psnr = overall_PSNR;
	result->rmse = overall_rmse;
	result->nrmse = overall_nrmse;
	
	result->min_pearsonCorr = overall_minPearsonCorr;
	result->avg_pearsonCorr = overall_avgPearsonCorr;	
	result->max_pearsonCorr = overall_maxPearsonCorr;
		
	return result;
}


double computeErrorSetting(const int compressorID, 
const double targetCR, const int nbPoints, double* sortedCompressionRatios, double* errorSettings,
void* data, const int dataType, const size_t r5, const size_t r4, const size_t r3, const size_t r2, const size_t r1, 
const float lower_bound, const float upper_bound)
{
	double result = 0;
	//plotDecSliceMode = DECVIS_ERROR_LINEAR_APPROX;
	if(plotDecSliceMode == DECVIS_ERROR_SELECT_NEAREST)
	{
		printf("Using DECVIS_ERROR_SELECT_NEAREST\n");
		int i = 0;
		int targetIndex = 0;
		
		for(i=0;i<nbPoints;i++)
		{
			if(sortedCompressionRatios[i] >= targetCR)
			{
				targetIndex = i;
				break;
			}
		}
		
		result = errorSettings[targetIndex];
	}
	else if(plotDecSliceMode == DECVIS_ERROR_LINEAR_APPROX)
	{
		printf("Using DECVIS_ERROR_LINEAR_APPROX\n");
		int i = 0;
		int lower = 0, upper = 0;
		for(i=0;i<nbPoints;i++)
		{
			if(sortedCompressionRatios[i] >= targetCR)
			{
				upper = i;
				break;
			}
		}
		if(upper>0)
		{
			lower = upper-1;
			double l = sortedCompressionRatios[lower];
			double u = sortedCompressionRatios[upper];
			double fl = errorSettings[lower];
			double fu = errorSettings[upper];
			double tmp = (targetCR-l)/(u-l)*(fu-fl);
			return fl+tmp;
		}
		else
		{
			lower = upper;
			upper = upper + 1;
			double l = sortedCompressionRatios[lower];
			double u = sortedCompressionRatios[upper];
			double fl = errorSettings[lower];
			double fu = errorSettings[upper];
			double a = (l-targetCR)/(u-targetCR);
			result = (a*fu-fl)/(a-1);						
		}
		if(result<0)
			result = errorSettings[lower];
	}
#ifdef HAVE_LIBPRESSIOOPT	
	else if(plotDecSliceMode == DECVIS_ERROR_LIBPRESSIO_OPT)
	{	
		printf("Using DECVIS_ERROR_LIBPRESSIO_OPT\n");
		ZC_evaluation_result* eresult = search_cmpr_setting_with_CR(compressorID, data, dataType, r5, r4, r3, r2, r1, targetCR, lower_bound, upper_bound);	
		printf("error_setting: %f, compression ratio: %f\n", eresult->error_bound, eresult->metric);
		result = eresult->error_bound;
	}
#endif
	else
	{
		printf("Error: Unrecognized plotDecSliceMode: %d\n", DECVIS_ERROR_LIBPRESSIO_OPT);
		printf("You may need to enable libpressio_opt when compiling your code, if you are trying to use Libpressio_opt\n");
	}
	
	return result;
}

void extractCRandErrorSettings(ZC_CompareData *compressionResults[], int compressionResultCount, double* compressionRatios, double* errorSettings)
{
	int i = 0;
	for(i=0;i<compressionResultCount;i++)
	{
		compressionRatios[i] = compressionResults[i]->compressRatio;
		errorSettings[i] = compressionResults[i]->errorBound;
	}
}

void print_cmprVisE(CompressorCRVisElement* cmprVisE)
{
	printf("varName=%s cmprName=%s\n", cmprVisE->varName, cmprVisE->compressorName);
		
	int i = 0;
	for(i=0;i<cmprVisE->resultCount;i++)
		printf("%f,%f ", cmprVisE->compressionResults[i]->compressRatio, cmprVisE->compressionResults[i]->errorBound);
	
	printf("\n");
	char** keys = ht_getAllKeys(cmprVisE->CRVisDataMap);
	int count = ht_getElemCount(cmprVisE->CRVisDataMap);
	for(i=0;i<count;i++)
	{
		char* compressionRatio = keys[i];
		ZCVisDecDataElement* visEle = (ZCVisDecDataElement*)ht_get(cmprVisE->CRVisDataMap, compressionRatio);
		printf("CR=%s ErrorBound=%s\n", compressionRatio, visEle->errorSetting_str);
	}
	free(keys);
}

/**
 * 
 * This function actually seems not necessary for compression/decompression of specified compressionr ratios (for dec vis)
 * */
StringLine* write_cmprVisE(CompressorCRVisElement* cmprVisE)
{
	char tmpLine[MAX_MSG_LENGTH];	
	StringLine* header = createStringLineHeader();	
	StringLine* p = header; //p always points to the tail
	char* line = NULL;
	
	sprintf(tmpLine, "cmprName=%s\n", cmprVisE->compressorName);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	
	int i = 0;
	for(i=0;i<cmprVisE->resultCount-1;i++)
	{
		sprintf(tmpLine, "%f,%f ", cmprVisE->compressionResults[i]->compressRatio, cmprVisE->compressionResults[i]->errorBound);
		line = createLine(tmpLine); p = appendOneLine(p, line);
	}
	sprintf(tmpLine, "%f,%f\n", cmprVisE->compressionResults[i]->compressRatio, cmprVisE->compressionResults[i]->errorBound);
	line = createLine(tmpLine); p = appendOneLine(p, line);	

	char** keys = ht_getAllKeys(cmprVisE->CRVisDataMap);
	int count = ht_getElemCount(cmprVisE->CRVisDataMap);
	for(i=0;i<count;i++)
	{
		char* compressionRatio = keys[i];
		ZCVisDecDataElement* visEle = (ZCVisDecDataElement*) ht_get(cmprVisE->CRVisDataMap, compressionRatio);
		sprintf(tmpLine, "CR=%s ErrorBound=%s\n", compressionRatio, visEle->errorSetting_str);
		line = createLine(tmpLine); p = appendOneLine(p, line);
	}
	free(keys);	
	
	return header;
}

/**
 * 
 * CompressorCRVisElement* cmprVisE is both input and output. 
 * Specifically, cmprVisE->compressionResults is the input information containing compression results with different error settings
 * cmprVisE->CRVisDataMap contains the output information such as estimated error setting based on different specified compression ratios (CR)
 * see struct ZCVisDecDataElement for details.
 * 
 * */
void ZC_itentifyErrorSettingBasedOnCR(CompressorCRVisElement* cmprVisE)
{
	int i = 0;
	//sorting the compressionResults based on CR (increasing order)
	ZC_quick_sort_vis_CR(cmprVisE->compressionResults, 0, cmprVisE->resultCount-1);
	
	double compressionRatios[MAX_NB_CMPR_CASES];
	double errorSettings[MAX_NB_CMPR_CASES];
	//TODO: extract CRs and errorSettings from the sorted cmprVisE->compressionResults
	extractCRandErrorSettings(cmprVisE->compressionResults, cmprVisE->resultCount, compressionRatios, errorSettings);
	
	//perform analysis 
	hashtable_t* crVisDataMap = cmprVisE->CRVisDataMap;
	int keyCount = ht_getElemCount(crVisDataMap);
	char** cr_str = ht_getAllKeys(crVisDataMap);
	for(i=0;i<keyCount;i++)
	{
		char* targetCR = cr_str[i];
		ZCVisDecDataElement* visEle = (ZCVisDecDataElement*)ht_get(crVisDataMap, targetCR);
		double targetCR_ = atof(targetCR);
		
		int compressorID = visEle->compressorID;
		ZC_DataProperty* dataProperty = visEle->dataProperty;
		int dataType = dataProperty->dataType;
		size_t r5 = dataProperty->r5;
		size_t r4 = dataProperty->r4;
		size_t r3 = dataProperty->r3;
		size_t r2 = dataProperty->r2;
		size_t r1 = dataProperty->r1;
		
		double targetErrorSetting = 0;		
#ifdef HAVE_LIBPRESSIOOPT
		if(plotDecSliceMode == DECVIS_ERROR_LIBPRESSIO_OPT)
		{
			if(dataType == ZC_FLOAT)
			{
				size_t nbEle = 0;	
				float* data = ZC_readFloatData(dataProperty->filePath, &nbEle);
				targetErrorSetting = computeErrorSetting(compressorID, targetCR_, cmprVisE->resultCount, compressionRatios, errorSettings, 
				data, dataType, r5, r4, r3, r2, r1, 1E-6, 1E-1);
			}
			else if(dataType == ZC_DOUBLE)
			{
				size_t nbEle = 0;			
				double* data = ZC_readDoubleData(dataProperty->filePath, &nbEle);
				targetErrorSetting = computeErrorSetting(compressorID, targetCR_, cmprVisE->resultCount, compressionRatios, errorSettings, 
				data, dataType, r5, r4, r3, r2, r1, 1E-6, 1E-1);				
			}
			else
			{
				printf("Error: unrecognized data type %d \n", dataType);
				exit(0);
			}			
		}
		else
		{
#endif			
			targetErrorSetting = computeErrorSetting(compressorID, targetCR_, cmprVisE->resultCount, compressionRatios, errorSettings, 
			NULL, dataType, r5, r4, r3, r2, r1, 1E-6, 1E-1);
#ifdef HAVE_LIBPRESSIOOPT			
		}
#endif
		
		visEle->errorSetting_str = (char*)malloc(sizeof(char)*32);
		sprintf(visEle->errorSetting_str, "%.2G", targetErrorSetting);
		visEle->errorSetting = atof(visEle->errorSetting_str);
		visEle->compressionCaseName = (char*)malloc(sizeof(char)*128);
		sprintf(visEle->compressionCaseName, "%s(%s):%s", visEle->compressorName, visEle->errorSetting_str, visEle->varName);
	}
	free(cr_str);
}

void free_ZCVisDecDataElement(ZCVisDecDataElement* visEle)
{
	if(visEle->errorSetting_str!=NULL)
		free(visEle->errorSetting_str);
	if(visEle->compressionCaseName!=NULL)
		free(visEle->compressionCaseName);
	if(visEle->sliceImage_dec_ori!=NULL)
		free(visEle->sliceImage_dec_ori);
	if(visEle->sliceImage_dec_log!=NULL)
		free(visEle->sliceImage_dec_log);
	if(visEle->compressionResult!=NULL)
		free(visEle->compressionResult);
	free(visEle);
}

void free_CompressorCRVisElement(CompressorCRVisElement* cmprVisE)
{
	//varName and compressorName are not newly allocated memory
/*	if(cmprVisE->varName!=NULL)
		free(cmprVisE->varName);
	if(cmprVisE->compressorName!=NULL)
		free(cmprVisE->compressorName);*/
	int i = 0;
	for(i=0;i<cmprVisE->resultCount;i++)
		freeCompareResult(cmprVisE->compressionResults[i]);
	int capacity = cmprVisE->CRVisDataMap->capacity;
	for(i=0;i<capacity;i++)
	{
		entry_t* entry = cmprVisE->CRVisDataMap->table[i];
		while(entry!=NULL)
		{
			ZCVisDecDataElement* visEle = (ZCVisDecDataElement*)entry->value;
			free_ZCVisDecDataElement(visEle);
			entry = entry->next;
		}
	}
	ht_freeTable(cmprVisE->CRVisDataMap);
	free(cmprVisE);
}
