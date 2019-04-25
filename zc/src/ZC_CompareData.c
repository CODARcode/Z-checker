#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "ZC_util.h"
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "zc.h"
#include "iniparser.h"
#ifdef HAVE_ONLINEVIS
#include "zserver.h"
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
	ZC_CompareData* found = (ZC_CompareData*)ht_freePairEntry(ecCompareDataTable, key);
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

ZC_CompareData* ZC_constructCompareResult(char* varName, double compressTime, double compressRate, double compressRatio, double rate,
size_t compressSize, double decompressTime, double decompressRate, double minAbsErr, double avgAbsErr, double maxAbsErr, 
double minRelErr, double avgRelErr, double maxRelErr, double rmse, double nrmse, double psnr, double snr, double valErrCorr, double pearsonCorr,
double* autoCorrAbsErr, double* absErrPDF)
{
	ZC_CompareData* result = (ZC_CompareData*)malloc(sizeof(struct ZC_CompareData));
	memset(result, 0, sizeof(struct ZC_CompareData));

	//TODO: get the dataProperty based on varName from the hashtable.
	result->property = (ZC_DataProperty*)ht_get(ecPropertyTable, varName);
	
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
	return result;
}

void ZC_computeFFT_float_offline(ZC_CompareData* compareResult,float* data1, float* data2, size_t numOfElem)
{
	size_t fft_size = pow(2,(int)log2(numOfElem));
	 
	complex* fftCoeff1 = ZC_computeFFT(data1, fft_size, ZC_FLOAT);
	complex* fftCoeff2 = ZC_computeFFT(data2, fft_size, ZC_FLOAT);
	complex* fftCoeffRelDiff = (complex*)malloc(FFT_SIZE*sizeof(complex));
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
	 
	complex* fftCoeff1 = ZC_computeFFT(data1, fft_size, ZC_FLOAT);
	complex* fftCoeff2 = ZC_computeFFT(data2, fft_size, ZC_FLOAT);
	complex* fftCoeffRelDiff = (complex*)malloc(FFT_SIZE*sizeof(complex));
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

char** constructCompareDataString(ZC_CompareData* compareResult)
{
	char** s = (char**)malloc(33*sizeof(char*));
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
	
#ifdef HAVE_R
	s[25] = (char*)malloc(100*sizeof(char));
	sprintf(s[25], "KS_test = %.10G\n", compareResult->ksValue);
	s[26] = (char*)malloc(100*sizeof(char));
	sprintf(s[26], "lum = %.10G\n", compareResult->lum);
	s[27] = (char*)malloc(100*sizeof(char));
	sprintf(s[27], "cont = %.10G\n", compareResult->cont);
	s[28] = (char*)malloc(100*sizeof(char));
	sprintf(s[28], "struc = %.10G\n", compareResult->struc);
	s[29] = (char*)malloc(100*sizeof(char));
	sprintf(s[29], "ssim = %.10G\n", compareResult->ssim);					
#else
	s[25] = (char*)malloc(100*sizeof(char));
	strcpy(s[25], "KS_test = -\n");
	s[26] = (char*)malloc(100*sizeof(char));
	strcpy(s[26], "lum = -\n");
	s[27] = (char*)malloc(100*sizeof(char));
	strcpy(s[27], "cont = -\n");
	s[28] = (char*)malloc(100*sizeof(char));
	strcpy(s[28], "struc = -\n");
	s[29] = (char*)malloc(100*sizeof(char));
	strcpy(s[29], "ssim = -\n");
#endif

	s[30] = (char*)malloc(100*sizeof(char));
	s[31] = (char*)malloc(100*sizeof(char));
	s[32] = (char*)malloc(100*sizeof(char));
	if(compareResult->property->r2==0)
	{
		strcpy(s[30], "ssimImage2D_min = -\n");
		strcpy(s[31], "ssimImage2D_avg = -\n");
		strcpy(s[32], "ssimImage2D_max = -\n");
	}
	else if(compareResult->property->r3==0)
	{
		strcpy(s[30], "ssimImage2D_min = -\n");		
		sprintf(s[31], "ssimImage2D_avg = %.10G\n", compareResult->ssimImage2D_avg);
		strcpy(s[32], "ssimImage2D_max = -\n");
	}
	else
	{
		sprintf(s[30], "ssimImage2D_min = %.10G\n", compareResult->ssimImage2D_min);
		sprintf(s[31], "ssimImage2D_avg = %.10G\n", compareResult->ssimImage2D_avg);
		sprintf(s[32], "ssimImage2D_max = %.10G\n", compareResult->ssimImage2D_max);						
	}

	return s;
}

void ZC_writeCompressionResult(ZC_CompareData* compareResult, char* solution, char* varName, char* tgtWorkspaceDir)
{
#if HAVE_ONLINEVIS
  return;
#endif
	char** s = constructCompareDataString(compareResult);
	char varName_[ZC_BUFS];
	strcpy(varName_, varName);
	ZC_ReplaceStr2(varName_, "_", "\\\\_");
	DIR *dir = opendir(tgtWorkspaceDir);
	if(dir==NULL)
		mkdir(tgtWorkspaceDir,0775);
	
	char tgtFilePath[ZC_BUFS_LONG];
	sprintf(tgtFilePath, "%s/%s:%s.cmp", tgtWorkspaceDir, solution, varName); 
	ZC_writeStrings(33, s, tgtFilePath);
	
	int i;
	for(i=0;i<=32;i++)
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
			ss[1] = "0 1\n";
			ZC_writeStrings(2, ss, tgtFilePath);
			free(ss[0]);	
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
			ss[1] = "0 1\n";
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
		
	ZC_CompareData* compareResult = ZC_constructCompareResult(var, 
	compressTime, compressRate, compressRatio, rate, 
	compressSize, decompressTime, decompressRate, minAbsErr, avgAbsErr, maxAbsErr, minRelErr, avgRelErr, maxRelErr, 
	rmse, nrmse, psnr, snr, valErrCorr, pearsonCorr, autoCorrAbsErr, absErrPDF);
	
	iniparser_freedict(ini);
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
			compressDataList[j++] = t->value;
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
