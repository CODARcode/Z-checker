#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "zc.h"
#include "iniparser.h"

void freeCompareResult(ZC_CompareData* compareData)
{
	//free(compareData->property);
	free(compareData->autoCorrAbsErr);
	free(compareData->absErrPDF);
	free(compareData->fftCoeff);
	free(compareData);
}

ZC_CompareData* ZC_constructCompareResult(char* varName, double compressTime, double compressRate, double compressRatio, double rate,
int compressSize, double decompressTime, double decompressRate, double minAbsErr, double avgAbsErr, double maxAbsErr, 
double minRelErr, double avgRelErr, double maxRelErr, double rmse, double nrmse, double psnr, double snr, double pearsonCorr, 
double* autoCorrAbsErr, double* absErrPDF)
{
	ZC_CompareData* this = (ZC_CompareData*)malloc(sizeof(ZC_CompareData));

	//TODO: get the dataProperty based on varName from the hashtable.
	this->property = ht_get(ecPropertyTable, varName);
	
	this->compressTime = compressTime;
	this->compressRate = compressRate;
	this->compressRatio = compressRatio;
	this->rate = rate;
	this->compressSize = compressSize;
	this->decompressTime = decompressTime;
	this->decompressRate = decompressRate;
	this->minAbsErr = minAbsErr;
	this->avgAbsErr = avgAbsErr;
	this->maxAbsErr = maxAbsErr;
	this->minRelErr = minRelErr;
	this->avgRelErr = avgRelErr;
	this->maxRelErr = maxRelErr;
	this->rmse = rmse;
	this->nrmse = nrmse;
	this->psnr = psnr;
	this->snr = snr;
	this->pearsonCorr = pearsonCorr;
	this->autoCorrAbsErr = autoCorrAbsErr;
	this->absErrPDF = absErrPDF;
	return this;
}

void ZC_compareData_float(ZC_CompareData* compareResult, float* data1, float* data2, 
int r5, int r4, int r3, int r2, int r1)
{
	int i = 0;
	double minDiff = data2[0]-data1[0];
	double maxDiff = minDiff;
	double minErr = fabs(minDiff);
	double maxErr = minErr;
	double sum1 = 0, sum2 = 0, sumDiff = 0, sumErr = 0, sumErrSqr = 0;
	
	//int numOfElem = ZC_computeDataLength(r5, r4, r3, r2, r1);
	int numOfElem = compareResult->property->numOfElem;
	double sumOfDiffSquare = 0;

	for (i = 0; i < numOfElem; i++)
	{
		sum1 += data1[i];
		sum2 += data2[i];
		
		double diff = data2[i]-data1[i];
		if(minDiff > diff) minDiff = diff;
		if(maxDiff < diff) maxDiff = diff;
		sumDiff += diff;
		sumOfDiffSquare += diff*diff;

		double err = fabs(diff);
		if(minErr>err) minErr = err;
		if(maxErr<err) maxErr = err;
		sumErr += err;

		sumErrSqr += err*err; //used for mse, nrmse, psnr
	}
	
	ZC_DataProperty* property = compareResult->property;
	//double minValue = property->minValue;
	//double maxValue = property->maxValue;
	//double medValue = minValue+(maxVal-minVal)/2;
	
	double zeromean_variance = property->zeromean_variance;
	double valRange = property->valueRange;
	double mean1 = sum1/numOfElem;
	double mean2 = sum2/numOfElem;
	
	double avgDiff = sumDiff/numOfElem;
	double avgErr = sumErr/numOfElem;
	double diffRange = maxDiff - minDiff;
	double mse = sumErrSqr/numOfElem;
	double diff; 
	int index;
	
	if (minAbsErrFlag)
		compareResult->minAbsErr = minErr;

	if (minRelErrFlag)
		compareResult->minRelErr = minErr/valRange;

	if (maxAbsErrFlag)
		compareResult->maxAbsErr = maxErr;

	if (maxRelErrFlag)
		compareResult->maxRelErr = maxErr/valRange;

	if (avgAbsErrFlag)
		compareResult->avgAbsErr = avgErr;

	if (avgRelErrFlag)
		compareResult->avgRelErr = avgErr/valRange;

	if (absErrPDFFlag)
	{
		double interval = diffRange/PDF_INTERVALS;
		double *absErrPDF = NULL;
		if(interval==0)
		{
			double* absErrPDF = (double*)malloc(sizeof(double));
			*absErrPDF = 0;
		}
		else
		{
			absErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS);
			memset(absErrPDF, 0, PDF_INTERVALS*sizeof(double));

			for (i = 0; i < numOfElem; i++)
			{
				diff = data2[i] - data1[i];
				index = (int)((diff-minDiff)/interval);
				if(index==PDF_INTERVALS)
					index = PDF_INTERVALS-1;
				absErrPDF[index] += 1;
			}

			for (i = 0; i < PDF_INTERVALS; i++)
				absErrPDF[i]/=numOfElem;			
		}
		compareResult->absErrPDF = absErrPDF;
		compareResult->err_interval = interval;
		compareResult->err_minValue = minDiff;		
	}

	if (autoCorrAbsErrFlag)
	{
		double *autoCorrAbsErr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));
		double *absDiff = (double*)malloc(numOfElem*sizeof(double));

		double covDiff = 0;
		for (i = 0; i < numOfElem; i++)
		{
			absDiff[i] = data2[i] - data1[i];
			covDiff += (absDiff[i] - avgDiff)*(absDiff[i] - avgDiff);
		}

		covDiff = covDiff/numOfElem;
		int delta;
		for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
		{
			double sum = 0;

			for (i = 0; i < numOfElem-delta; i++)
			{
				sum += (absDiff[i]-avgDiff)*(absDiff[i+delta]-avgDiff);
			}

			autoCorrAbsErr[delta] = sum/(numOfElem-delta)/covDiff;
		}

		autoCorrAbsErr[0] = 1;
		compareResult->autoCorrAbsErr = autoCorrAbsErr;
		free(absDiff);
	}

	if (pearsonCorrFlag)
	{
		double prodSum = 0, sum1 = 0, sum2 = 0;
    	for (i = 0; i < numOfElem; i++)
    	{
    		prodSum += (data1[i]-mean1)*(data2[i]-mean2);
    		sum1 += (data1[i]-mean1)*(data1[i]-mean1);
    		sum2 += (data2[i]-mean2)*(data2[i]-mean2);
    	}

    	double std1 = sqrt(sum1/numOfElem);
    	double std2 = sqrt(sum2/numOfElem);
    	double ee = prodSum/numOfElem;
    	double pearsonCorr = ee/std1/std2;

    	compareResult->pearsonCorr = pearsonCorr;
	}

	if (rmseFlag)
	{
		double rmse = sqrt(mse);
		compareResult->rmse = rmse;
	}

	if (nrmseFlag)
	{
		double nrmse = sqrt(mse)/valRange;
		compareResult->nrmse = nrmse;
	}

	if(snrFlag)
	{
		compareResult->snr = 10*log10(zeromean_variance/mse);
		//printf("compareResult->snr=%f ccompareResult->snr_db=%f", compareResult->snr, compareResult->snr_db); 		
	}

	if (psnrFlag)
	{
		double psnr = -20.0*log10(sqrt(mse)/valRange);
		compareResult->psnr = psnr;
	}
	
}

void ZC_compareData_double(ZC_CompareData* compareResult, double* data1, double* data2,
int r5, int r4, int r3, int r2, int r1)
{
	int i = 0;
	double minDiff = data2[0]-data1[0];
	double maxDiff = minDiff;
	double minErr = fabs(minDiff);
	double maxErr = minErr;
	double minVal = data1[0];
	double maxVal = minVal;
	double sum1 = 0, sum2 = 0, sumDiff = 0, sumErr = 0, sumErrSqr = 0;

	//int numOfElem = ZC_computeDataLength(r5, r4, r3, r2, r1);
	int numOfElem = compareResult->property->numOfElem;

	for (i = 0; i < numOfElem; i++)
	{
		sum1 += data1[i];
		sum2 += data2[i];

		if (minVal > data1[i]) minVal = data1[i];
		if (maxVal < data1[i]) maxVal = data1[i];

		double diff = data2[i]-data1[i];
		if(minDiff > diff) minDiff = diff;
		if(maxDiff < diff) maxDiff = diff;
		sumDiff += diff;

		double err = fabs(diff);
		if(minErr>err) minErr = err;
		if(maxErr<err) maxErr = err;
		sumErr += err;

		sumErrSqr += err*err; //used for mse, nrmse, psnr
	}

	ZC_DataProperty* property = compareResult->property;

	double zeromean_variance = property->zeromean_variance;
	double valRange = property->valueRange;
	double mean1 = sum1/numOfElem;
	double mean2 = sum2/numOfElem;

	double avgDiff = sumDiff/numOfElem;
	double avgErr = sumErr/numOfElem;
	double diffRange = maxDiff - minDiff;
	double mse = sumErrSqr/numOfElem;
	double diff;
	int index;

	if (minAbsErrFlag)
		compareResult->minAbsErr = minErr;

	if (minRelErrFlag)
		compareResult->minRelErr = minErr/valRange;

	if (maxAbsErrFlag)
		compareResult->maxAbsErr = maxErr;

	if (maxRelErrFlag)
		compareResult->maxRelErr = maxErr/valRange;

	if (avgAbsErrFlag)
		compareResult->avgAbsErr = avgErr;

	if (avgRelErrFlag)
		compareResult->avgRelErr = avgErr/valRange;

	if (absErrPDFFlag)
	{
		double interval = diffRange/PDF_INTERVALS;
		double *absErrPDF = NULL;
		if(interval==0)
		{
			double* absErrPDF = (double*)malloc(sizeof(double));
			*absErrPDF = 0;
		}
		else
		{
			absErrPDF = (double*)malloc(sizeof(double)*PDF_INTERVALS);
			memset(absErrPDF, 0, PDF_INTERVALS*sizeof(double));

			for (i = 0; i < numOfElem; i++)
			{
				double diff = data2[i] - data1[i];
				int index = (int)((diff-minDiff)/interval);
				absErrPDF[index] += 1;
			}

			for (i = 0; i < PDF_INTERVALS; i++)
				absErrPDF[i]/=numOfElem;
		}
		compareResult->absErrPDF = absErrPDF;
		compareResult->err_interval = interval;
		compareResult->err_minValue = minDiff;			
	}

	if (autoCorrAbsErrFlag)
	{
		double *autoCorrAbsErr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));
		double *absDiff = (double*)malloc(numOfElem*sizeof(double));

		double covDiff = 0;
		for (i = 0; i < numOfElem; i++)
		{
			absDiff[i] = data2[i] - data1[i];
			covDiff += (absDiff[i] - avgDiff)*(absDiff[i] - avgDiff);
		}

		covDiff = covDiff/numOfElem;
		int delta;
		for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
		{
			double sum = 0;

			for (i = 0; i < numOfElem-delta; i++)
			{
				sum += (absDiff[i]-avgDiff)*(absDiff[i+delta]-avgDiff);
			}

			autoCorrAbsErr[delta] = sum/(numOfElem-delta)/covDiff;
		}
		
		autoCorrAbsErr[0] = 1;
		compareResult->autoCorrAbsErr = autoCorrAbsErr;
	}

	if (pearsonCorrFlag)
	{
		double prodSum = 0, sum1 = 0, sum2 = 0;
    	for (i = 0; i < numOfElem; i++)
    	{
    		prodSum += (data1[i]-mean1)*(data2[i]-mean2);
    		sum1 += (data1[i]-mean1)*(data1[i]-mean1);
    		sum2 += (data2[i]-mean2)*(data2[i]-mean2);
    	}

    	double std1 = sqrt(sum1/numOfElem);
    	double std2 = sqrt(sum2/numOfElem);
    	double ee = prodSum/numOfElem;
    	double pearsonCorr = ee/std1/std2;

    	compareResult->pearsonCorr = pearsonCorr;
	}

	if (rmseFlag)
	{
		double rmse = sqrt(mse);
		compareResult->rmse = rmse;
	}

	if (nrmseFlag)
	{
		double nrmse = sqrt(mse)/valRange;
		compareResult->nrmse = nrmse;
	}
	
	if(snrFlag)
	{
		compareResult->snr = 10*log10(zeromean_variance/mse);
	}

	if (psnrFlag)
	{
		double psnr = -20.0*log10(sqrt(mse)/valRange);
		compareResult->psnr = psnr;
	}
}

void ZC_compareData_dec(ZC_CompareData* compareResult, void *decData)
{
	if(compareResult==NULL)
	{
		printf("Error: compareResult cannot be NULL\n");
		exit(0);
	}
	char* varName = compareResult->property->varName;
	int dataType = compareResult->property->dataType;
	void* oriData = compareResult->property->data;	
	int r5 = compareResult->property->r5;
	int r4 = compareResult->property->r4;
	int r3 = compareResult->property->r3;
	int r2 = compareResult->property->r2;
	int r1 = compareResult->property->r1;				

	if(dataType==ZC_FLOAT)
	{
		float* data1 = (float*)oriData;
		float* data2 = (float*)decData;
		compareResult->fftCoeff = ZC_computeFFT(data2, ZC_FLOAT);
		ZC_compareData_float(compareResult, data1, data2, r5, r4, r3, r2, r1);
	}
	else if(dataType==ZC_DOUBLE)
	{
		double* data1 = (double*)oriData;
		double* data2 = (double*)decData;
		
		compareResult->fftCoeff = ZC_computeFFT(data2, ZC_DOUBLE);
		ZC_compareData_double(compareResult, data1, data2, r5, r4, r3, r2, r1);		
	}
	else
	{
		printf("Error: dataType is wrong!\n");
		exit(0);
	}
}

ZC_CompareData* ZC_compareData(char* varName, int dataType, void *oriData, void *decData, int r5, int r4, int r3, int r2, int r1)
{
	ZC_CompareData* compareResult = (ZC_CompareData*)malloc(sizeof(ZC_CompareData));
	memset(compareResult, 0, sizeof(ZC_CompareData));

	if(dataType==ZC_FLOAT)
	{
		float* data1 = (float*)oriData;
		float* data2 = (float*)decData;
		
		int numOfElem = ZC_computeDataLength(r5, r4, r3, r2, r1);
		compareResult->property = ZC_genProperties_float(varName, data1, numOfElem, r5, r4, r3, r2, r1);
		ZC_compareData_float(compareResult, data1, data2, r5, r4, r3, r2, r1);
	}
	else if(dataType==ZC_DOUBLE)
	{
		double* data1 = (double*)oriData;
		double* data2 = (double*)decData;
		
		int numOfElem = ZC_computeDataLength(r5, r4, r3, r2, r1);
		compareResult->property = ZC_genProperties_double(varName, data1, numOfElem, r5, r4, r3, r2, r1);		
		ZC_compareData_double(compareResult, data1, data2, r5, r4, r3, r2, r1);		
	}
	else
	{
		printf("Error: dataType is wrong!\n");
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
	char** s = (char**)malloc(21*sizeof(char*));
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
	sprintf(s[6], "compressSize = %d\n", compareResult->compressSize);
	
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
	sprintf(s[12], "autoCorrAbsErr = %.10G\n", (compareResult->autoCorrAbsErr)[1]); //TODO output AUTO_CORR_SIZE coefficients
	
	s[13] = (char*)malloc(100*sizeof(char));
	sprintf(s[13], "minRelErr = %.10G\n", compareResult->minRelErr);
	s[14] = (char*)malloc(100*sizeof(char));
	sprintf(s[14], "avgRelErr = %.10G\n", compareResult->avgRelErr);
	s[15] = (char*)malloc(100*sizeof(char));
	sprintf(s[15], "maxRelErr = %.10G\n", compareResult->maxRelErr);

	s[16] = (char*)malloc(100*sizeof(char));
	sprintf(s[16], "rmse = %.10G\n", compareResult->rmse);
	s[17] = (char*)malloc(100*sizeof(char));
	sprintf(s[17], "nrmse = %.10G\n", compareResult->nrmse);
	s[18] = (char*)malloc(100*sizeof(char));
	sprintf(s[18], "psnr = %.10G\n", compareResult->psnr);
	s[19] = (char*)malloc(100*sizeof(char));
	sprintf(s[19], "snr = %.10G\n", compareResult->snr);	
	s[20] = (char*)malloc(100*sizeof(char));
	sprintf(s[20], "pearsonCorr = %.10G\n", compareResult->pearsonCorr);
	
	return s;
}

void ZC_writeCompressionResult(ZC_CompareData* compareResult, char* solution, char* varName, char* tgtWorkspaceDir)
{
	char** s = constructCompareDataString(compareResult);
	char varName_[ZC_BUFS];
	strcpy(varName_, varName);
	ZC_ReplaceStr2(varName_, "_", "\\\\_");
	DIR *dir = opendir(tgtWorkspaceDir);
	if(dir==NULL)
		mkdir(tgtWorkspaceDir,0775);
	
	char tgtFilePath[ZC_BUFS_LONG];
	sprintf(tgtFilePath, "%s/%s:%s.cmp", tgtWorkspaceDir, solution, varName); 
	ZC_writeStrings(21, s, tgtFilePath);
	
	int i;
	for(i=0;i<21;i++)
		free(s[i]);
	free(s);
	
	//write the pdf
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
			ss[i+1] = (char*)malloc(sizeof(char)*ZC_BUFS);
			double x = err_minValue+i*err_interval;
			sprintf(ss[i+1], "%.10G %.10G\n", x, compareResult->absErrPDF[i]); 
		}
		
		ZC_writeStrings(PDF_INTERVALS+1, ss, tgtFilePath);
		for(i=0;i<PDF_INTERVALS+1;i++)
			free(ss[i]);			
	}	

	//write auto-correlation coefficients
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
		
	char buf[ZC_BUFS];
	sprintf(buf, "%s:%s", solution, varName);
	ZC_writeFFTResults(buf, compareResult->fftCoeff, tgtWorkspaceDir);
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
	double pearsonCorr = (double)iniparser_getdouble(ini, "COMPARE:pearsonCorr", 0); 
	
	//TODO: Read zfp-test2.autocorr for filling in sol-var.autocorr such as zfp-test2.autocorr
	double* autoCorrAbsErr = NULL;
	
	//TODO: Read more data from distribution-of-err file (zfp-test2.dis) for filling PDF
	double* absErrPDF = NULL;
		
	ZC_CompareData* compareResult = ZC_constructCompareResult(var, 
	compressTime, compressRate, compressRatio, rate, 
	compressSize, decompressTime, decompressRate, minAbsErr, avgAbsErr, maxAbsErr, minRelErr, avgRelErr, maxRelErr, 
	rmse, nrmse, psnr, snr, pearsonCorr, autoCorrAbsErr, absErrPDF);
	
	iniparser_freedict(ini);
	return compareResult;
}
