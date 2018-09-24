/**
 *  @file zc.c
 *  @author Sheng Di and Dingwen Tao
 *  @date Aug, 2016
 *  @brief ZC_Init, Compression and Decompression functions
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "ZC_util.h"
#include "zc.h"
#include "ZC_rw.h"
#include "ZC_Hashtable.h"
#include "ZC_DataSetHandler.h"
#include "ZC_ReportGenerator.h"
#ifdef HAVE_MPI
#include <mpi.h>
#endif
#ifdef HAVE_R
#include "ZC_callR.h"
#endif
#ifdef HAVE_ONLINEVIS
#include "zserver.h"
#endif

char rscriptPath[MAX_MSG_LENGTH];

int sysEndianType; //endian type of the system
int dataEndianType; //endian type of the data

int checkingStatus; //0 refers to probe_compressor, 1 refers to analyze_data, and 2 indicates compare_compressor
int executionMode = 0;
//char *ZC_workspaceDir;

int errorBoundMode; //ABS, REL, ABS_AND_REL, or ABS_OR_REL

char *zc_cfgFile;

double absErrBound;
double relBoundRatio;

int minValueFlag = 1;
int maxValueFlag = 1;
int valueRangeFlag = 1;
int avgValueFlag = 1;
int entropyFlag = 1;
int autocorrFlag = 1;
int autocorr3DFlag = 1;
int fftFlag = 1;
int lapFlag = 0;

int compressTimeFlag = 1;
int decompressTimeFlag = 1;
int compressSizeFlag = 1;

int minAbsErrFlag = 1;
int avgAbsErrFlag = 1;
int maxAbsErrFlag = 1;
int errAutoCorrFlag = 1;
int errAutoCorr3DFlag = 1;
int absErrPDFFlag = 1;
int pwrErrPDFFlag = 1;

int minRelErrFlag = 1;
int avgRelErrFlag = 1;
int maxRelErrFlag = 1;

int rmseFlag = 1;
int nrmseFlag = 1;
int snrFlag = 1;
int psnrFlag = 1;
int valErrCorrFlag = 1;
int pearsonCorrFlag = 1;

int KS_testFlag = 1;
int SSIMFlag = 1;

int plotAutoCorrFlag = 1;
int plotAbsErrPDFFlag = 1;
int plotErrAutoCorrFlag = 1;
int plotFFTAmpFlag = 1;
int plotEntropyFlag = 1;

int checkCompressorsFlag = 1; //corresponding to plotCompressionResult flag in ec.config

int generateReportFlag = 1;
char *reportTemplateDir;

int ZC_versionNumber[3];

struct timeval startCmprTime;
struct timeval endCmprTime;  /* Start and end times */
struct timeval startDecTime;
struct timeval endDecTime;
double totalCmprCost = 0;
double totalDecCost = 0;

double initTime = 0;
double endTime = 0;

long globalCmprSize = 0;

int compressors_count = 0;
char* compressors[20];
char* compressors_dir[20];
char* compareData_dir[20];
char* properties_dir[20];

char* comparisonCases;

int numOfErrorBoundCases;

int allCompressorCount = 0;
CmprsorErrBound allCompressors[CMPR_MAX_LEN];

//char* allCompressors[20];
//int allErrorBoundCount = 0;
//char* allErrorBounds[20];
int allVarCaseCount = 0;
char* allVarCases[20];

#ifdef HAVE_MPI
MPI_Comm ZC_COMM_WORLD; 
#endif

int myRank = 0;
int nbProc = 1;

size_t globalDataLength = 0;

int initStatus = 0; //0 means no initialization yet or already ZC_Finalize(), 1 means already ZC_Init();

int visMode = 0;
int ZSERVER_PORT = 9091;

void cost_startCmpr()
{
	gettimeofday(&startCmprTime, NULL);
}

double cost_endCmpr()
{
	struct timeval endCmprTime;
	gettimeofday(&endCmprTime, NULL);
	double elapsed = ((endCmprTime.tv_sec*1000000+endCmprTime.tv_usec)-(startCmprTime.tv_sec*1000000+startCmprTime.tv_usec))/1000000.0;
	return elapsed;
}

void cost_startDec()
{
	gettimeofday(&startDecTime, NULL);
}

double cost_endDec()
{
	struct timeval endDecTime;
	gettimeofday(&endDecTime, NULL);
	double elapsed = ((endDecTime.tv_sec*1000000+endDecTime.tv_usec)-(startDecTime.tv_sec*1000000+startDecTime.tv_usec))/1000000.0;
	return elapsed;
}

int ZC_Init_NULL()
{
	initStatus = 1; 
	ecPropertyTable = ht_create( HASHTABLE_SIZE );			
	ecCompareDataTable = ht_create(HASHTABLE_SIZE);	
#ifdef HAVE_ONLINEVIS
	if(visMode && myRank == 0)
		zserver_start(ZSERVER_PORT);
#endif
}

int ZC_Init(char *configFilePath)
{
	initStatus = 1; 
	
	char str[512]="", str2[512]="", str3[512]="";
	zc_cfgFile = configFilePath;
	int loadFileResult = ZC_LoadConf();

#ifdef HAVE_R
	// Intialize the R environment.
	char *r_argv[] = { "R", "--no-restore", "--no-save", "--gui=none", "--silent", "--vanilla"};
	
	int r_argc = sizeof(r_argv) / sizeof(r_argv[0]);
		
	//printf("r_argc = %d, r_argv[*]=%s %s %s\n", r_argc, r_argv[0], r_argv[1], r_argv[2]);
	Rf_initEmbeddedR(r_argc, r_argv);
	source(rscriptPath);
#endif	
	
	if(loadFileResult==ZC_NSCS)
		exit(0);
	
	memset(allCompressors, 0, sizeof(CmprsorErrBound)*20);

#ifdef HAVE_MPI
	int mpi_init = 0;
	MPI_Initialized(&mpi_init);
	if(!mpi_init)
	{
		MPI_Init(NULL, NULL);
	}

	MPI_Comm_dup(MPI_COMM_WORLD, &ZC_COMM_WORLD); 
	MPI_Comm_size(ZC_COMM_WORLD, &nbProc);
	MPI_Comm_rank(ZC_COMM_WORLD, &myRank);
	if(nbProc>1 && mpi_init==0)
	{
		if(myRank==0)
		{
			printf("Error: You are running the program with multiple ranks/processes but missing MPI_Init() before ZC_Init().\n");
			printf("Solution: Please add/put the MPI_Init() to be ahead of ZC_Init().\n");
		}
		exit(0);
		return ZC_NSCS;		
	}
#endif

#ifdef HAVE_ONLINEVIS
	if(visMode && myRank == 0)
		zserver_start(ZSERVER_PORT);
	printf("visMode=%d\n", visMode);
#endif
	
	return ZC_SCES;
}

long ZC_computeDataLength(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	long dataLength;
	if(r1==0) 
	{
		dataLength = 0;
	}
	else if(r2==0) 
	{
		dataLength = r1;
	}
	else if(r3==0) 
	{
		dataLength = r1*r2;
	}
	else if(r4==0) 
	{
		dataLength = r1*r2*r3;
	}
	else if(r5==0) 
	{
		dataLength = r1*r2*r3*r4;
	}
	else 
	{
		dataLength = r1*r2*r3*r4*r5;
	}
	return dataLength;
}

ZC_DataProperty* ZC_startCmpr_offline(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i;
	double min,max,valueRange,sum = 0,avg;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));
	
	property->varName = (char*)malloc(sizeof(char)*100);
	char* varN = rmFileExtension(varName); //remove the final "." if any
	strcpy(property->varName, varN);
	free(varN);
	property->numOfElem = ZC_computeDataLength(r5,r4,r3,r2,r1);
	property->dataType = dataType;
	property->data = oriData;
	property->r5 = r5;
	property->r4 = r4;
	property->r3 = r3;
	property->r2 = r2;
	property->r1 = r1;
	
	if(dataType == ZC_FLOAT)
	{
		float* data = (float*)oriData;
		min = data[0];
		max = data[0];
		for(i=0;i<property->numOfElem;i++)
		{
			if(min>data[i]) min = data[i];
			if(max<data[i]) max = data[i];
			sum += data[i];
		}
		double med = min+(max-min)/2;
		double sum_of_square = 0;
		for(i=0;i<property->numOfElem;i++)
			sum_of_square += (data[i] - med)*(data[i] - med);
		property->zeromean_variance = sum_of_square/property->numOfElem;		
	}
	else
	{
		double* data = (double*)oriData;
		min = data[0];
		max = data[0];
		for(i=0;i<property->numOfElem;i++)
		{
			if(min>data[i]) min = data[i];
			if(max<data[i]) max = data[i];
			sum += data[i];
		}			
		double med = min+(max-min)/2;
		double sum_of_square = 0;
		for(i=0;i<property->numOfElem;i++)
			sum_of_square += (data[i] - med)*(data[i] - med);
		property->zeromean_variance = sum_of_square/property->numOfElem;			
	}
	property->minValue = min;
	property->maxValue = max;
	property->valueRange = max - min;
	property->avgValue = sum/property->numOfElem;
//	printf("property->avgValue=%f, sum=%f, property->numOfElem=%zu\n", property->avgValue, sum, property->numOfElem);
	
	if(compressTimeFlag)
		cost_startCmpr();
	return property;
}

ZC_DataProperty* ZC_startCmpr_offline_withDataAnalysis(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	ZC_DataProperty* property = ZC_genProperties(varName, dataType, oriData, r5, r4, r3, r2, r1);
	char tgtWorkspaceDir[ZC_BUFS];
	sprintf(tgtWorkspaceDir, "dataProperties");
	ZC_writeDataProperty(property, tgtWorkspaceDir);
	if(compressTimeFlag)
		cost_startCmpr();
	return property;
}

ZC_CompareData* ZC_endCmpr_offline(ZC_DataProperty* dataProperty, char* solution, long cmprSize)
{
	double cmprTime;
	if(compressTimeFlag)
		cmprTime = cost_endCmpr();

	ZC_CompareData* compareResult = ht_get(ecCompareDataTable, solution);
	if(compareResult==NULL)
	{
		compareResult = (ZC_CompareData*)malloc(sizeof(ZC_CompareData));		
		memset(compareResult, 0, sizeof(ZC_CompareData));	
		ht_set(ecCompareDataTable, solution, compareResult);
	}
	else
		free(compareResult->solution);
	
	int elemSize = dataProperty->dataType==ZC_FLOAT? 4: 8;	
	
	if(compressTimeFlag)
	{
		compareResult->compressTime = cmprTime;
		compareResult->compressRate = dataProperty->numOfElem*elemSize/cmprTime; //in B/s		
	}

	if(compressSizeFlag)
	{
		compareResult->compressSize = cmprSize;
		compareResult->compressRatio = (double)(dataProperty->numOfElem*elemSize)/cmprSize;
		if(dataProperty->dataType == ZC_DOUBLE)
			compareResult->rate = 64.0/compareResult->compressRatio;
		else
			compareResult->rate = 32.0/compareResult->compressRatio;
	}
	
	compareResult->property = dataProperty;
	
	compareResult->solution = (char*)malloc(strlen(solution)+1);	
	strcpy(compareResult->solution, solution);	
	
	return compareResult;
}

void ZC_startDec_offline()
{
	if(decompressTimeFlag)
		cost_startDec();
}

void ZC_endDec_offline(ZC_CompareData* compareResult, void *decData)
{
	char* solution = compareResult->solution;
	int elemSize = compareResult->property->dataType==ZC_FLOAT? 4: 8;	
	if(decompressTimeFlag)
	{
		double decTime = cost_endDec();
		compareResult->decompressTime = decTime;  //in seconds
		compareResult->decompressRate = compareResult->property->numOfElem*elemSize/decTime; // in B/s		
	}

	if(compareResult==NULL)
	{
		printf("Error: compressionResults==NULL. \nPlease construct ZC_CompareData* compareResult using ZC_compareData() or ZC_endCmpr().\n");
		exit(0);
	}
	
	//MPI_Barrier(MPI_COMM_WORLD);
	//if(myRank==0)
	//	printf("before ZC_compareData_dec\n");
	ZC_compareData_dec(compareResult, decData);
	//MPI_Barrier(MPI_COMM_WORLD);
	//if(myRank==0)
	//	printf("after ZC_compareData_dec\n");
	ZC_writeCompressionResult(compareResult, solution, compareResult->property->varName, "compressionResults");
}

void ZC_plotHistogramResults(int cmpCount, char** compressorCases)
{
	if(cmpCount==0)
	{
		printf("Error: compressors_count==0.\n Please run ZC_Init(config) first..\n");
		printf("If you already have ZC_Init(config) in your code, please make sure checkCompressors=1.\n");
		exit(0);
	}
	size_t i, j, count = ecPropertyTable->count;
	char stringBuffer[ZC_BUFS_LONG];
	
	char** keys = ht_getAllKeys(ecPropertyTable);
	char* cmprRatioLines[count+1];
	char* cmprRateLines[count+1];
	char* dcmprRateLines[count+1];
	char* psnrLines[cmpCount*count+1];
	cmprRatioLines[0] = (char*)malloc(2*ZC_BUFS);
	cmprRateLines[0] = (char*)malloc(2*ZC_BUFS);
	dcmprRateLines[0] = (char*)malloc(2*ZC_BUFS);	
	psnrLines[0] = (char*)malloc(2*ZC_BUFS);
	
	//constructing the field line
	sprintf(cmprRatioLines[0], "%s", "cmprRatio");
	sprintf(cmprRateLines[0], "%s", "cmprRate");
	sprintf(dcmprRateLines[0], "%s", "dcmprRate");	
	sprintf(psnrLines[0], "%s", "psnr");
	for(i=0;i<cmpCount;i++)
	{
		
		sprintf(stringBuffer, "%s %s", cmprRatioLines[0],compressorCases[i]);
		strcpy(cmprRatioLines[0], stringBuffer);
		sprintf(stringBuffer, "%s %s", cmprRateLines[0], compressorCases[i]);
		strcpy(cmprRateLines[0], stringBuffer);
		sprintf(stringBuffer, "%s %s", dcmprRateLines[0], compressorCases[i]);	
		strcpy(dcmprRateLines[0], stringBuffer);	
		sprintf(stringBuffer, "%s %s", psnrLines[0], compressorCases[i]);
		strcpy(psnrLines[0], stringBuffer);
	}
	
	strcat(cmprRatioLines[0], "\n");
	strcat(cmprRateLines[0], "\n");
	strcat(dcmprRateLines[0], "\n");
	strcat(psnrLines[0], "\n");	

	double maxCR = 0, maxCRT = 0, maxDCRT = 0, maxPSNR = 0;

	//constructing the data
	for(i=0;i<count;i++)
	{
		char* key = keys[i];
		char key_[ZC_BUFS];
		strcpy(key_, key);
		ZC_ReplaceStr2(key_, "_", "\\\\_");
		
		char* line1 = (char*)malloc(10*ZC_BUFS); //suppose there are 10 compressors
		char* line2 = (char*)malloc(10*ZC_BUFS); //suppose there are 10 compressors
		char* line3 = (char*)malloc(10*ZC_BUFS); //suppose there are 10 compressors
		char* line4 = (char*)malloc(10*ZC_BUFS); //suppose there are 10 compressors		
		char* compVarCase = (char*)malloc(ZC_BUFS);
		sprintf(compVarCase, "%s", key);
		sprintf(line1, "%s", key_);
		sprintf(line2, "%s", key_);
		sprintf(line3, "%s", key_);
		sprintf(line4, "%s", key_);		
		
		for(j=0;j<cmpCount;j++)
		{
			char* compressorName = compressorCases[j];
			sprintf(compVarCase, "%s:%s", compressorName, key);
			ZC_CompareData* compressResult = ht_get(ecCompareDataTable, compVarCase);
			if(compressResult==NULL)
			{
				printf("Error: compressResult==NULL. %s cannot be found in compression result\n", compVarCase);
				strcat(line1, " -");
				strcat(line2, " -");
				strcat(line3, " -");
				strcat(line4, " -");				
			}
			else
			{
				double cr = compressResult->compressRatio;
				if(maxCR<cr)
					maxCR = cr;
				sprintf(stringBuffer, "%s %f", line1, cr);
				strcpy(line1, stringBuffer);
				double crt = compressResult->compressRate;
				if(maxCRT<crt)
					maxCRT = crt;
				sprintf(stringBuffer, "%s %f", line2, crt);
				strcpy(line2, stringBuffer);
				double dcrt = compressResult->decompressRate;
				if(maxDCRT<dcrt)
					maxDCRT = dcrt;
				sprintf(stringBuffer, "%s %f", line3, dcrt);				
				strcpy(line3, stringBuffer);
				double psnr = compressResult->psnr;
				if(maxPSNR<psnr)
					maxPSNR = psnr;
				sprintf(stringBuffer, "%s %f", line4, psnr);
				strcpy(line4, stringBuffer);
			}			
		}
		strcat(line1, "\n");
		cmprRatioLines[i+1] = line1;
		strcat(line2, "\n");
		cmprRateLines[i+1] = line2;
		strcat(line3, "\n");
		dcmprRateLines[i+1] = line3;
		strcat(line4, "\n");
		psnrLines[i+1] = line4;				
		free(compVarCase);
	}
	
	char compreStringKey[ZC_BUFS_LONG];
	strcpy(compreStringKey, compressorCases[0]);
	for(i=1;i<cmpCount;i++)
	{
		strcat(compreStringKey, "_");
		strcat(compreStringKey, compressorCases[i]);
	} 
	
	char cmpRatioKey[ZC_BUFS_LONG], cmpRateKey[ZC_BUFS_LONG], dcmpRateKey[ZC_BUFS_LONG], psnrKey[ZC_BUFS_LONG];
	char cmpRatioDataFile[ZC_BUFS_LONG], cmpRateDataFile[ZC_BUFS_LONG], dcmpRateDataFile[ZC_BUFS_LONG], psnrDataFile[ZC_BUFS_LONG];
	char cmpRatioPlotFile[ZC_BUFS_LONG], cmpRatePlotFile[ZC_BUFS_LONG], dcmpRatePlotFile[ZC_BUFS_LONG], psnrPlotFile[ZC_BUFS_LONG];
	char cmpRatioCmd[ZC_BUFS_LONG], cmpRateCmd[ZC_BUFS_LONG], dcmpRateCmd[ZC_BUFS_LONG], psnrCmd[ZC_BUFS_LONG];
	sprintf(cmpRatioKey, "cratio_%s", compreStringKey);
	sprintf(cmpRateKey, "crate_%s", compreStringKey);
	sprintf(dcmpRateKey, "drate_%s", compreStringKey);
	sprintf(psnrKey, "psnr_%s", compreStringKey);
	sprintf(cmpRatioDataFile, "%s.txt", cmpRatioKey);
	sprintf(cmpRateDataFile, "%s.txt", cmpRateKey);
	sprintf(dcmpRateDataFile, "%s.txt", dcmpRateKey);
	sprintf(psnrDataFile, "%s.txt", psnrKey);
	sprintf(cmpRatioPlotFile, "%s.p", cmpRatioKey);
	sprintf(cmpRatePlotFile, "%s.p", cmpRateKey);
	sprintf(dcmpRatePlotFile, "%s.p", dcmpRateKey);	
	sprintf(psnrPlotFile, "%s.p", psnrKey);
	
	sprintf(cmpRatioCmd, "gnuplot \"%s\"", cmpRatioPlotFile);
	sprintf(cmpRateCmd, "gnuplot \"%s\"", cmpRatePlotFile);
	sprintf(dcmpRateCmd, "gnuplot \"%s\"", dcmpRatePlotFile);	
	sprintf(psnrCmd, "gnuplot \"%s\"", psnrPlotFile);
	//writing the data to file
	ZC_writeStrings(count+1, cmprRatioLines, cmpRatioDataFile);
	ZC_writeStrings(count+1, cmprRateLines, cmpRateDataFile);
	ZC_writeStrings(count+1, dcmprRateLines, dcmpRateDataFile);	
	ZC_writeStrings(count+1, psnrLines, psnrDataFile);
		
	//generate GNUPLOT scripts, and plot the data by running the scripts
	char** scriptLines = genGnuplotScript_histogram(cmpRatioKey, "txt", GNUPLOT_FONT, 1+cmpCount, "Variables", "Compression Ratio", (long)(maxCR*1.3)+1);
	ZC_writeStrings(18, scriptLines, cmpRatioPlotFile);
	system(cmpRatioCmd);
	
	scriptLines = genGnuplotScript_histogram(cmpRateKey, "txt", GNUPLOT_FONT, 1+cmpCount, "Variables", "Compression Rate", (long)(maxCRT*1.3)+1);
	ZC_writeStrings(18, scriptLines, cmpRatePlotFile);
	system(cmpRateCmd);

	scriptLines = genGnuplotScript_histogram(dcmpRateKey, "txt", GNUPLOT_FONT, 1+cmpCount, "Variables", "Decompression Rate", (long)(maxDCRT*1.3)+1);
	ZC_writeStrings(18, scriptLines, dcmpRatePlotFile);
	system(dcmpRateCmd);
	
	scriptLines = genGnuplotScript_histogram(psnrKey, "txt", GNUPLOT_FONT, 1+cmpCount, "Variables", "PSNR", (long)(maxPSNR*1.3)+1);
	ZC_writeStrings(18, scriptLines, psnrPlotFile);
	system(psnrCmd);	
	
	free(scriptLines);
	for(i=0;i<count+1;i++)
	{
		free(cmprRatioLines[i]);
		free(cmprRateLines[i]);
		free(dcmprRateLines[i]);
		free(psnrLines[i]);
	}
}

int getComparisonCases(char* cases[])
{
	size_t i;
	char copyCompCases[ZC_BUFS_LONG];
	strcpy(copyCompCases, comparisonCases);
	char* ccase = strtok(copyCompCases, " ");
	for(i=0;ccase!=NULL;i++)
	{
		if(i>ZC_BUFS) 
		{
			printf("Error: # comparison cases cannot be greater than %d\n", ZC_BUFS);
			exit(0);
		}
		cases[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
		sprintf(cases[i], "%s", ccase);
		ccase = strtok(NULL, " ");
	}
	return i;
}

void ZC_plotComparisonCases()
{
	size_t i=0,n,j;
	char* cases[ZC_BUFS]; //100 cases at most
	n = getComparisonCases(cases);
	
	for(i=0;i<n;i++)
	{
		char* cmprssors[ZC_BUFS]; 
		char* cmprsor = strtok(cases[i], ",");
		for(j=0;cmprsor!=NULL;j++)
		{
			cmprssors[j] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(cmprssors[j], "%s", cmprsor); 
			cmprsor = strtok(NULL, ",");
		}
		size_t m = j;
		ZC_plotHistogramResults(m, cmprssors);
		for(j=0;j<m;j++)
			free(cmprssors[j]);
		free(cases[i]);
	}
}

/**
 * @deprecated 
 * */
void ZC_plotCompressionRatio()
{
	if(compressors_count==0)
	{
		printf("Error: compressors_count==0.\n Please run ZC_Init(config) first.\n");
		exit(0);
	}
	size_t i, j, count = ecPropertyTable->count;
	
	//TODO: Construct the cases by using compressor name and the variable name
	//TODO: traverse all the property-variables through the propertyTable
	char** keys = ht_getAllKeys(ecPropertyTable);
	char* dataLines[count+1];
	dataLines[0] = "data";
	for(i=0;i<compressors_count;i++)
		sprintf(dataLines[0], "%s %s", dataLines[0], compressors[i]);
	sprintf(dataLines[0], "%s\n", dataLines[0]);
	
	double maxCR = 0;
	
	for(i=0;i<count;i++)
	{
		char* key = keys[i];
		char* line = (char*)malloc(10*ZC_BUFS);
		char* compVarCase = (char*)malloc(ZC_BUFS);
		sprintf(compVarCase, "%s", key);
		sprintf(line, "%s", key);
		for(j=0;j<compressors_count;j++)
		{
			char* compressorName = compressors[j];
			sprintf(compVarCase, "%s:%s", compressorName, key);
			ZC_CompareData* compressResult = ht_get(ecCompareDataTable, compVarCase);
			if(compressResult==NULL)
			{
				printf("Error: compressResult==NULL. %s cannot be found in compression result\n", compVarCase);
				sprintf(line, "%s -", line);
			}
			else
			{
				double cr = compressResult->compressRatio;
				if(maxCR<cr)
					maxCR = cr;
				sprintf(line, "%s %f", line, cr);				
			}
		}		
		sprintf(line, "%s\n", line);
		dataLines[i+1] = line;
		free(compVarCase);
	}
	
	ZC_writeStrings(count+1, dataLines, "compressionRatio.txt");
	
	//TODO: generate the GNUPLOT script.
	char** scriptLines = genGnuplotScript_histogram("compressionRatio", "txt", GNUPLOT_FONT, 1+compressors_count, "Variables", "Compression Ratio", (long)(maxCR*1.3));
	ZC_writeStrings(18, scriptLines, "compressionRatio.p");
	system("gnuplot compressionRatio.p");
}

void ZC_plotRateDistortion()
{
	if(compressors_count==0)
	{
		printf("Error: compressors_count==0.\n Please run ZC_Init(config) first.\n");
		exit(0);
	}
	size_t i, j;
	
	//select variables
	char** variables = ht_getAllKeys(ecPropertyTable);
	for(i=0;i<ecPropertyTable->count;i++)
	{
		int count, validLineNum; //count: the number of test-cases
		char** cmpResList = getCompResKeyList(variables[i], &count);
		char** dataLines_psnr = extractRateDistortion_psnr(count, cmpResList, &validLineNum);
		if(dataLines_psnr!=NULL&&validLineNum>0)
		{
			char fileName[ZC_BUFS];
			sprintf(fileName, "rate-distortion_psnr_%s.txt", variables[i]);
			ZC_writeStrings(count+1, dataLines_psnr, fileName);		
			
			//TODO: generate the GNUPLOT script.
			sprintf(fileName, "rate-distortion_psnr_%s", variables[i]);
			char** scriptLines = genGnuplotScript_linespoints(fileName, "txt", GNUPLOT_FONT, 1+compressors_count, "Rate", "PSNR");
			sprintf(fileName, "rate-distortion_psnr_%s.p", variables[i]);
			ZC_writeStrings(24, scriptLines, fileName);
			char cmd[ZC_BUFS];
			sprintf(cmd, "gnuplot %s", fileName);
			system(cmd);
			
			for(j=0;j<count+1;j++)
				if(dataLines_psnr[j]!=NULL) free(dataLines_psnr[j]);						
		}
		
		char** dataLines_snr = extractRateDistortion_snr(count, cmpResList, &validLineNum);
		if(dataLines_snr!=NULL&&validLineNum>0)
		{
			char fileName[ZC_BUFS];
			sprintf(fileName, "rate-distortion_snr_%s.txt", variables[i]);
			ZC_writeStrings(count+1, dataLines_snr, fileName);		
			
			//TODO: generate the GNUPLOT script.
			sprintf(fileName, "rate-distortion_snr_%s", variables[i]);
			char** scriptLines = genGnuplotScript_linespoints(fileName, "txt", GNUPLOT_FONT, 1+compressors_count, "Rate", "Distortion");
			sprintf(fileName, "rate-distortion_snr_%s.p", variables[i]);
			ZC_writeStrings(24, scriptLines, fileName);
			char cmd[ZC_BUFS];
			sprintf(cmd, "gnuplot %s", fileName);
			system(cmd);
			
			for(j=0;j<count+1;j++)
				if(dataLines_snr[j]!=NULL) free(dataLines_snr[j]);						
		}

		char** dataLines_valErrCorr = extractRateCorrelation(count, cmpResList, &validLineNum);
		if(dataLines_valErrCorr!=NULL&&validLineNum>0)
		{
			char fileName[ZC_BUFS];
			sprintf(fileName, "rate-corr_%s.txt", variables[i]);
			ZC_writeStrings(count+1, dataLines_valErrCorr, fileName);

			//TODO: generate the GNUPLOT script.
			sprintf(fileName, "rate-corr_%s", variables[i]);
			char** scriptLines = genGnuplotScript_linespoints(fileName, "txt", GNUPLOT_FONT, 1+compressors_count, "Rate", "Correlation");
			sprintf(fileName, "rate-corr_%s.p", variables[i]);
			ZC_writeStrings(24, scriptLines, fileName);
			char cmd[ZC_BUFS];
			sprintf(cmd, "gnuplot %s", fileName);
			system(cmd);

			for(j=0;j<count+1;j++)
				if(dataLines_valErrCorr[j]!=NULL) free(dataLines_valErrCorr[j]);
		}
		
		free(cmpResList);
	}
}

char** getCompResKeyList(char* var, int* count)
{
	size_t i, j = 0;
	char** selected = (char**)malloc(ecCompareDataTable->count*sizeof(char*));
	char** cmpResKeys = ht_getAllKeys(ecCompareDataTable);
	for(i=0;i < ecCompareDataTable->count;i++)
	{
		char* tmp = (char*)malloc(strlen(cmpResKeys[i])+1);
		sprintf(tmp, "%s",cmpResKeys[i]); 
		char* cmpssor = strtok(tmp, ":");
		char* varCase = strtok(NULL, ":");
		if(strcmp(var, varCase)==0)
			selected[j++] = cmpResKeys[i];
		free(tmp);
	}
	*count = j;
	return selected;
}

char** extractRateDistortion_psnr(int totalCount, char** cmpResList, int* validLineNum)
{
	size_t i, j, p, k, q = 1, t = 0;
	char stringBuffer[ZC_BUFS_LONG];
	
	if(totalCount==0)
		return NULL;
	//construct the dataLines and the field line
	char** dataLines = (char**)malloc((totalCount+1)*sizeof(char*)); //including field file
	memset(dataLines, 0, (totalCount+1)*sizeof(char*));
	
	dataLines[0] = (char*)malloc(ZC_BUFS);
	
	sprintf(dataLines[0], "ratedistortion");
	for(i=0;i<compressors_count;i++)
	{
		sprintf(stringBuffer, "%s %s", dataLines[0], compressors[i]);
		strcpy(dataLines[0], stringBuffer);
	}
	sprintf(stringBuffer, "%s\n", dataLines[0]);
	strcpy(dataLines[0], stringBuffer);
	
	RateDistElem* rdList = (RateDistElem*)malloc(totalCount*sizeof(RateDistElem));
	
	//start checking compressors one by one, constructing the rate distortion curves.	
	for(i=0;i<compressors_count;i++)
	{
		memset(rdList, 0, totalCount*sizeof(RateDistElem));
		p = 0;
		char* compressorName = compressors[i];
		//TODO: scan ecCompareDataTable and select matched records
		for(j=0;j<totalCount;j++)
		{
			char* key = cmpResList[j];
			int ck = checkStartsWith(key, compressorName);
			if(ck)
			{
				ZC_CompareData* compareResult = (ZC_CompareData*)ht_get(ecCompareDataTable, key);
				RateDistElem e = (RateDistElem)malloc(sizeof(struct RateDistElem_t));
				e->rate = compareResult->rate;
				e->psnr = compareResult->psnr;
				e->maxAbsErr = compareResult->maxAbsErr;
				e->compressRate = compareResult->compressRate;
				rdList[p++] = e;
			}
		}
		
		t += p;
		ZC_quick_sort(rdList, 0, p-1);
		
		for(j=0;j<p;j++)
		{
			RateDistElem e = rdList[j];
			char* l = (char*)malloc(ZC_BUFS);
			sprintf(l, "%f", e->rate);
			for(k=0;k<compressors_count;k++)
			{
				if(k==i && e->maxAbsErr!=0)
				{
					sprintf(stringBuffer, "%s %f", l, e->psnr);
					strcpy(l, stringBuffer);
				}
				else
					strcat(l, " -");
			}
			strcat(l, "\n");
			dataLines[q++] = l;
		}
		
		for(j=0;j<totalCount;j++)
			if(rdList[j]!=NULL) free(rdList[j]);
	}

	*validLineNum = t;

	free(rdList);
	return dataLines;
}

char** extractRateDistortion_snr(int totalCount, char** cmpResList, int* validLineNum)
{
	size_t i, j, p, k, q = 1, t = 0;
	char stringBuffer[ZC_BUFS_LONG];
	
	if(totalCount==0)
		return NULL;
	//construct the dataLines and the field line
	char** dataLines = (char**)malloc((totalCount+1)*sizeof(char*)); //including field file
	memset(dataLines, 0, (totalCount+1)*sizeof(char*));
	
	dataLines[0] = (char*)malloc(ZC_BUFS);
	
	sprintf(dataLines[0], "ratedistortion");
	for(i=0;i<compressors_count;i++)
	{
		sprintf(stringBuffer, "%s %s", dataLines[0], compressors[i]);
		strcpy(dataLines[0], stringBuffer);
	}
	sprintf(stringBuffer, "%s\n", dataLines[0]);
	strcpy(dataLines[0], stringBuffer);
	
	RateDistElem* rdList = (RateDistElem*)malloc(totalCount*sizeof(RateDistElem));
	
	//start checking compressors one by one, constructing the rate distortion curves.	
	for(i=0;i<compressors_count;i++)
	{
		memset(rdList, 0, totalCount*sizeof(RateDistElem));
		p = 0;
		char* compressorName = compressors[i];
		//TODO: scan ecCompareDataTable and select matched records
		for(j=0;j<totalCount;j++)
		{
			char* key = cmpResList[j];
			int ck = checkStartsWith(key, compressorName);
			if(ck)
			{
				ZC_CompareData* compareResult = (ZC_CompareData*)ht_get(ecCompareDataTable, key);
				RateDistElem e = (RateDistElem)malloc(sizeof(struct RateDistElem_t));
				e->rate = compareResult->rate;
				e->psnr = compareResult->snr;
				e->maxAbsErr = compareResult->maxAbsErr;
				e->compressRate = compareResult->compressRate;
				rdList[p++] = e;
			}
		}
		
		t += p;
		ZC_quick_sort(rdList, 0, p-1);
		
		for(j=0;j<p;j++)
		{
			RateDistElem e = rdList[j];
			char* l = (char*)malloc(ZC_BUFS);
			sprintf(l, "%f", e->rate);
			for(k=0;k<compressors_count;k++)
			{
				if(k==i && e->maxAbsErr!=0)
				{
					sprintf(stringBuffer, "%s %f", l, e->psnr);
					strcpy(l, stringBuffer);
				}
				else
					strcat(l, " -");
			}
			strcat(l, "\n");
			dataLines[q++] = l;
		}
		
		for(j=0;j<totalCount;j++)
			if(rdList[j]!=NULL) free(rdList[j]);
	}

	*validLineNum = t;

	free(rdList);
	return dataLines;
}

char** extractRateCorrelation(int totalCount, char** cmpResList, int* validLineNum)
{
	size_t i, j, p, k, q = 1, t = 0;
	char stringBuffer[ZC_BUFS_LONG];

	if(totalCount==0)
		return NULL;
	//construct the dataLines and the field line
	char** dataLines = (char**)malloc((totalCount+1)*sizeof(char*)); //including field file
	memset(dataLines, 0, (totalCount+1)*sizeof(char*));

	dataLines[0] = (char*)malloc(ZC_BUFS);

	sprintf(dataLines[0], "ratecorrelation");
	for(i=0;i<compressors_count;i++)
	{
		sprintf(stringBuffer, "%s %s", dataLines[0], compressors[i]);
		strcpy(dataLines[0], stringBuffer);
	}
	sprintf(stringBuffer, "%s\n", dataLines[0]);
	strcpy(dataLines[0], stringBuffer);

	RateDistElem* rdList = (RateDistElem*)malloc(totalCount*sizeof(RateDistElem));

	//start checking compressors one by one, constructing the rate distortion curves.
	for(i=0;i<compressors_count;i++)
	{
		memset(rdList, 0, totalCount*sizeof(RateDistElem));
		p = 0;
		char* compressorName = compressors[i];
		//TODO: scan ecCompareDataTable and select matched records
		for(j=0;j<totalCount;j++)
		{
			char* key = cmpResList[j];
			int ck = checkStartsWith(key, compressorName);
			if(ck)
			{
				ZC_CompareData* compareResult = (ZC_CompareData*)ht_get(ecCompareDataTable, key);
				RateDistElem e = (RateDistElem)malloc(sizeof(struct RateDistElem_t));
				e->rate = compareResult->rate;
				e->psnr = compareResult->valErrCorr;
				e->maxAbsErr = compareResult->maxAbsErr;
				e->compressRate = compareResult->compressRate;
				rdList[p++] = e;
			}
		}

		t += p;
		ZC_quick_sort(rdList, 0, p-1);

		for(j=0;j<p;j++)
		{
			RateDistElem e = rdList[j];
			char* l = (char*)malloc(ZC_BUFS);
			sprintf(l, "%f", e->rate);
			for(k=0;k<compressors_count;k++)
			{
				if(k==i && e->maxAbsErr!=0)
				{
					sprintf(stringBuffer, "%s %f", l, e->psnr);
					strcpy(l, stringBuffer);
				}
				else
					strcat(l, " -");
			}
			strcat(l, "\n");
			dataLines[q++] = l;
		}

		for(j=0;j<totalCount;j++)
			if(rdList[j]!=NULL) free(rdList[j]);
	}

	*validLineNum = t;

	free(rdList);
	return dataLines;
}

void ZC_plotAutoCorr_CompressError()
{
	size_t i,j;
	char acFileName[ZC_BUFS], acPlotFile[ZC_BUFS], acCmd[ZC_BUFS_LONG];
	char** allCampCaseNames = ht_getAllKeys(ecCompareDataTable);
	for(i=0;i<ecCompareDataTable->count;i++)
	{
		sprintf(acFileName, "%s", allCampCaseNames[i]);
		char** scriptLines = genGnuplotScript_fillsteps(acFileName, "autocorr", GNUPLOT_FONT, 2, "Lags", "ACF");
		sprintf(acPlotFile, "compressionResults/%s-autocorr.p", acFileName);
		ZC_writeStrings(19, scriptLines, acPlotFile);
		//execute .p files using system().
		sprintf(acCmd, "cd compressionResults;gnuplot \"%s-autocorr.p\";mv \"%s.autocorr.eps\" \"%s-autocorr.eps\"", acFileName, acFileName, acFileName);
		system(acCmd);	
		for(j=0;j<19;j++)
			free(scriptLines[j]);
		free(scriptLines);	
	}	
}

void ZC_plotAutoCorr_DataProperty()
{
	size_t i,j;
	char acFileName[ZC_BUFS], acPlotFile[ZC_BUFS], acCmd[ZC_BUFS_LONG];
	char** allVarNames = ht_getAllKeys(ecPropertyTable);
	for(i=0;i<ecPropertyTable->count;i++)
	{
		sprintf(acFileName, "%s", allVarNames[i]);
		char** scriptLines = genGnuplotScript_fillsteps(acFileName, "autocorr", GNUPLOT_FONT, 2, "Lags", "ACF");
		sprintf(acPlotFile, "dataProperties/%s-autocorr.p", acFileName);		
		ZC_writeStrings(19, scriptLines, acPlotFile);
		//execute .p files using system().
		sprintf(acCmd, "cd dataProperties;gnuplot %s-autocorr.p;mv %s.autocorr.eps %s-autocorr.eps", acFileName, acFileName, acFileName);
		system(acCmd);
		for(j=0;j<19;j++)
			free(scriptLines[j]);
		free(scriptLines);		
	}
}

void ZC_plotFFTAmplitude_OriginalData()
{
	size_t i,j;
	char ampFileName[ZC_BUFS], ampPlotFile[ZC_BUFS], ampCmd[ZC_BUFS_LONG];
	char** allVarNames = ht_getAllKeys(ecPropertyTable);
	for(i=0;i<ecPropertyTable->count;i++)
	{
		sprintf(ampFileName, "%s.fft", allVarNames[i]);
		sprintf(ampPlotFile, "dataProperties/%s-fft-amp.p", allVarNames[i]);
		char** scriptLines = genGnuplotScript_fillsteps(ampFileName, "amp", GNUPLOT_FONT, 2, "frequency", "amplitude");
		ZC_writeStrings(19, scriptLines, ampPlotFile);
		//execute .p files using system().
		sprintf(ampCmd, "cd dataProperties;gnuplot \"%s-fft-amp.p\";mv \"%s.fft.amp.eps\" \"%s-fft-amp.eps\"", allVarNames[i],allVarNames[i], allVarNames[i]);
		system(ampCmd);
		for(j=0;j<19;j++)
			free(scriptLines[j]);
		free(scriptLines);
	}
}

void ZC_plotFFTAmplitude_DecompressData()
{
	size_t i,j;
	char ampFileName[ZC_BUFS], ampPlotFile[ZC_BUFS], ampCmd[ZC_BUFS_LONG];
	char** allVarNames = ht_getAllKeys(ecCompareDataTable);
	for(i=0;i<ecCompareDataTable->count;i++)
	{
		sprintf(ampFileName, "%s.fft", allVarNames[i]);
		sprintf(ampPlotFile, "compressionResults/%s-fft-amp.p", allVarNames[i]);
		char** scriptLines = genGnuplotScript_fillsteps(ampFileName, "amp", GNUPLOT_FONT, 2, "frequency", "relative difference of amplitude");
		ZC_writeStrings(19, scriptLines, ampPlotFile);
		//execute .p files using system().
		sprintf(ampCmd, "cd compressionResults;gnuplot \"%s-fft-amp.p\";mv \"%s.fft.amp.eps\" \"%s-fft-amp.eps\"", allVarNames[i],allVarNames[i], allVarNames[i]);
		system(ampCmd);
		for(j=0;j<19;j++)
			free(scriptLines[j]);
		free(scriptLines);
	}
}

void ZC_plotErrDistribtion()
{
	size_t i,j;
	char disFileName[ZC_BUFS], disPlotFile[ZC_BUFS], disCmd[ZC_BUFS_LONG];
	char** allVarNames = ht_getAllKeys(ecCompareDataTable);
	
	if(absErrPDFFlag)
	{
		for(i=0;i<ecCompareDataTable->count;i++)
		{
			sprintf(disFileName, "%s", allVarNames[i]);
			sprintf(disPlotFile, "compressionResults/%s-dis.p", allVarNames[i]);
			char** scriptLines = genGnuplotScript_fillsteps(disFileName, "dis", GNUPLOT_FONT, 2, "Error", "PDF");
			ZC_writeStrings(19, scriptLines, disPlotFile);
			//execute .p files using system().
			sprintf(disCmd, "cd compressionResults;gnuplot \"%s-dis.p\";mv \"%s.dis.eps\" \"%s-dis.eps\"", disFileName,disFileName, disFileName);
			system(disCmd);
			for(j=0;j<19;j++)
				free(scriptLines[j]);
			free(scriptLines);	
		}		
	}

	if(pwrErrPDFFlag)
	{
		for(i=0;i<ecCompareDataTable->count;i++)
		{
			sprintf(disFileName, "%s", allVarNames[i]);
			sprintf(disPlotFile, "compressionResults/%s-pds.p", allVarNames[i]);
			char** scriptLines = genGnuplotScript_fillsteps(disFileName, "pds", GNUPLOT_FONT, 2, "Error", "PDF");
			ZC_writeStrings(19, scriptLines, disPlotFile);
			//execute .p files using system().
			sprintf(disCmd, "cd compressionResults;gnuplot \"%s-pds.p\";mv \"%s.pds.eps\" \"%s-pds.eps\"", disFileName,disFileName, disFileName);
			system(disCmd);
			for(j=0;j<19;j++)
				free(scriptLines[j]);
			free(scriptLines);	
		}		
	}	
}

int ZC_analyze_and_generateReport(char* dataSetName)
{
	if(generateReportFlag==0)
	{
		printf("Error: You want to generate the report but generateReportFlag==0\n");
		printf("Solution: Set generateReportFlag to 1 in the zc.config or in the initialization step.\n");
		exit(0);
	}
	
    ZC_plotComparisonCases();
    ZC_plotRateDistortion();

	if(plotAutoCorrFlag)
		ZC_plotAutoCorr_DataProperty();
		
    ZC_plotAutoCorr_CompressError();
    
    if(plotFFTAmpFlag)
    {
		ZC_plotFFTAmplitude_OriginalData();
		ZC_plotFFTAmplitude_DecompressData();
	}

	if(plotAbsErrPDFFlag)
		ZC_plotErrDistribtion();	
    
    ZC_generateOverallReport(dataSetName);

    return ZC_SCES;
}


int ZC_Finalize()
{
	if(initStatus==0)
	{
		printf("Error: ZC_finalize: you cannot perform ZC_Finalize() before ZC_Init().\n");
		printf("Hint: ZC_Finalize() cannot be performed multiple times in a row without corresponding ZC_Init().\n");
		return ZC_NSCS;
	}
	//free hashtable memory
	if(ecPropertyTable!=NULL)
	{
		size_t i, count = ecPropertyTable->count;
		char** keys = ht_getAllKeys(ecPropertyTable);
	
		for(i=0;i<count;i++)
		{
			char* key = keys[i];

			ZC_DataProperty* property = ht_get(ecPropertyTable, key);
			freeDataProperty_internal(property);
			//free(key);
		}		
		ht_freeTable(ecPropertyTable);
		free(keys);
	}

	if(ecCompareDataTable!=NULL)
	{
		size_t i, count = ecCompareDataTable->count;
		char** keys = ht_getAllKeys(ecCompareDataTable);
		for(i=0;i<count;i++)
		{
			char* key = keys[i];
			ZC_CompareData* c = ht_get(ecCompareDataTable, key);
			freeCompareResult_internal(c);
		}
		free(keys);
		ht_freeTable(ecCompareDataTable);
	}
	if(reportTemplateDir!=NULL)
		free(reportTemplateDir);
	//free compressor_errBounds_elements
	size_t i =0, j=0;
	for(i=0;i<allCompressorCount;i++)
	{
		for(j=0;j<allCompressors[i].allErrBoundCount;j++)
		{
			free(allCompressors[i].allErrBounds[j]->str);//Don't need to free selErrBounds[j] because it's pointing allErrBounds[j]
			free(allCompressors[i].allErrBounds[j]);
		}
		free(allCompressors[i].allErrBounds);
		free(allCompressors[i].selErrBounds);	
	}
	
#ifdef HAVE_R
	// Release R environment
	Rf_endEmbeddedR(0);
#endif	
#ifdef HAVE_ONLINEVIS
	if(visMode && myRank == 0)
		zserver_stop();
#endif
		
	initStatus = 0;
	return ZC_SCES;
}

ZC_CompareData* ZC_registerVar(char* name, int dataType, void* oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	ZC_CompareData* zcv = NULL;
	if(ecPropertyTable==NULL)
		ecPropertyTable = ht_create(HASHTABLE_SIZE);
	ZC_DataProperty* property = ht_get(ecPropertyTable, name);
	if(property==NULL)
	{
		property = ZC_genProperties(name, dataType, oriData, r5, r4, r3, r2, r1);
		ht_set(ecPropertyTable, name, property);
	}
	if(ecCompareDataTable==NULL)
		ecCompareDataTable = ht_create(HASHTABLE_SIZE);
	zcv = (ZC_CompareData*)ht_get(ecCompareDataTable, name);
	if(zcv==NULL)
	{
		zcv = ZC_constructCompareResult(name, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);		
		zcv->property = property;
		zcv->dec_data = NULL;
	}
	
	return zcv;
}

ZC_CompareData** loadMultiVars(char* multivarFile, int* nbVars, int* status)
{
	int i, lineCount, nbVars_ = 0;
	StringLine* lines = ZC_readLines(multivarFile, &lineCount);
	char *str, *token;
	char delim[2];
	strcpy(delim, ":"); 
	
	char varName[100];
	char endian[20];
	char type[10];
	char dim[100];
	char filePath[256];
	
	StringLine* p = lines->next;
	while(p!=NULL)
	{
		str = p->str;
		if(str[0]=='#'||str[0]=='\0')
		{
			p = p->next;
			continue;
		}
		else
			nbVars_++;
		p = p->next;
	}
	ZC_CompareData** result = (ZC_CompareData**)malloc(sizeof(ZC_CompareData*)*nbVars_);	
	
	p = lines->next;
	int counter = 0;
	while(p!=NULL)
	{
		str = p->str;
		
		if(str[0]!='#'&&str[0]!='\0')
		{
			printf("Loading data %s\n", str);
			/* get the first token */
			token = strtok(str, delim);	
			strcpy(varName, token);
			
			void* value = ht_get(ecPropertyTable, varName);
			if(value!=NULL)
			{
				printf("Error: duplicated name of variables in the list (%s)\n", varName);
				*status = ZC_NSCS;
				return NULL;
			}
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("Error: not enough information for variable %s in %s\n", varName, multivarFile);
				*status = ZC_NSCS;
				return NULL;
			}
			strcpy(endian, token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("Error: not enough information for variable %s in %s\n", varName, multivarFile);
				*status = ZC_NSCS;
				return NULL;
			}
			strcpy(type, token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("Error: not enough information for variable %s in %s\n", varName, multivarFile);
				*status = ZC_NSCS;
				return NULL;
			}		
			strcpy(dim, token);
			token = strtok(NULL, delim);
			if(token==NULL)
			{
				printf("Error: not enough information for variable %s in %s\n", varName, multivarFile);
				*status = ZC_NSCS;
				return NULL;
			}		
			strcpy(filePath, token);			

			rtrim(filePath); //remove '\n'
			
			result[counter] = ZC_constructOneVariable(varName, endian, type, dim, filePath);
			counter++;
		}
		p = p->next;
	}
	*nbVars = nbVars_;
	ZC_freeLines(lines);
	*status = ZC_SCES;
	return result;
}

//online interfaces
#ifdef HAVE_MPI
long ZC_computeDataLength_online(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	long localDataLength = ZC_computeDataLength(r5, r4, r3, r2, r1);
	MPI_Allreduce(&localDataLength, &globalDataLength, 1, MPI_LONG, MPI_SUM, ZC_COMM_WORLD);
	return globalDataLength;
}

ZC_DataProperty* ZC_startCmpr_online(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i;
	double sum = 0,avg;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));
	size_t numOfElem = ZC_computeDataLength(r5,r4,r3,r2,r1);	
	
	property->varName = (char*)malloc(sizeof(char)*100);
	char* varN = rmFileExtension(varName); //remove the final "." if any
	strcpy(property->varName, varN);
	free(varN);
	
	property->dataType = dataType;
	property->data = oriData;
	property->r5 = r5;
	property->r4 = r4;
	property->r3 = r3;
	property->r2 = r2;
	property->r1 = r1;
	
	if(dataType == ZC_FLOAT)
	{
		float* data = (float*)oriData;
		ZC_genBasicProperties_float_online(data, numOfElem, property);	
	}
	else
	{
		double* data = (double*)oriData;
		ZC_genBasicProperties_double_online(data, numOfElem, property);
	}
	
	if(compressTimeFlag)
		initTime = MPI_Wtime();
	return property;
}

ZC_CompareData* ZC_endCmpr_online(ZC_DataProperty* dataProperty, char* solution, long cmprSize)
{
	double cmprTime = 0;
	if(compressTimeFlag)
	{
		endTime = MPI_Wtime();
		cmprTime = endTime - initTime;
	}
	
	ZC_CompareData* compareResult = (ZC_CompareData*)malloc(sizeof(ZC_CompareData));
	memset(compareResult, 0, sizeof(ZC_CompareData));	
	int elemSize = dataProperty->dataType==ZC_FLOAT? 4: 8;	
	
	if(compressTimeFlag)
	{
		compareResult->compressTime = cmprTime;
		compareResult->compressRate = dataProperty->numOfElem*elemSize/cmprTime; //in B/s (dataProperty->numOfElem here means globalDataLength) 
	}

	if(compressSizeFlag)
	{
		//aggregate the compression size among all ranks
		MPI_Allreduce(&cmprSize, &globalCmprSize, 1, MPI_LONG, MPI_SUM, ZC_COMM_WORLD);		
		
		compareResult->compressSize = globalCmprSize;
		compareResult->compressRatio = (double)(dataProperty->numOfElem*elemSize)/globalCmprSize;
		if(dataProperty->dataType == ZC_DOUBLE)
			compareResult->rate = 64.0/compareResult->compressRatio;
		else
			compareResult->rate = 32.0/compareResult->compressRatio;
	}
	compareResult->property = dataProperty;
	compareResult->solution = (char*)malloc(strlen(solution)+1);
	strcpy(compareResult->solution, solution);
	ZC_CompareData* zcc = ht_get(ecCompareDataTable, solution);
	if(zcc==NULL)
	{
		ht_set(ecCompareDataTable, solution, zcc);
		return compareResult;
	}
	else
	{//you compress one variable with the same 'solution' (or key) using the same compressor twice.
	 //the return to be returned will use the one stored in the hash-table
		return zcc;
	}
}

void ZC_startDec_online()
{
	if(decompressTimeFlag)
		initTime = MPI_Wtime();
}

void ZC_endDec_online(ZC_CompareData* compareResult, void *decData)
{
	char* solution = compareResult->solution;
	int elemSize = compareResult->property->dataType==ZC_FLOAT? 4: 8;	
	if(decompressTimeFlag)
	{
		endTime = MPI_Wtime();
		compareResult->decompressTime = endTime - initTime;  //in seconds
		compareResult->decompressRate = compareResult->property->numOfElem*elemSize/compareResult->decompressTime; // in B/s		
	}

	if(compareResult==NULL)
	{
		printf("Error: compressionResults==NULL. \nPlease construct ZC_CompareData* compareResult using ZC_compareData() or ZC_endCmpr().\n");
		exit(0);
	}
	ZC_compareData_dec(compareResult, decData);
	if(myRank==0)
		ZC_writeCompressionResult(compareResult, solution, compareResult->property->varName, "compressionResults");
}
#endif

//overall interfaces for checkingStatus==PROBE_COMPRESSOR

ZC_DataProperty* ZC_startCmpr(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	ZC_DataProperty* result = (ZC_DataProperty*)ht_get(ecPropertyTable, varName); //note that result->varName is the cleared string of varName.

	if(result!=NULL)
		freeDataProperty(result);

#ifdef HAVE_MPI
	if(executionMode == ZC_ONLINE)
		result = ZC_startCmpr_online(varName, dataType, oriData, r5, r4, r3, r2, r1);
	else
		result = ZC_startCmpr_offline(varName, dataType, oriData, r5, r4, r3, r2, r1);
#else
	result = ZC_startCmpr_offline(varName, dataType, oriData, r5, r4, r3, r2, r1);
#endif
	ht_set(ecPropertyTable, result->varName, result);
	
	return result;	
}

ZC_CompareData* ZC_endCmpr(ZC_DataProperty* dataProperty, char* solution, long cmprSize)
{
	ZC_CompareData* result = NULL;
#ifdef HAVE_MPI
	if(executionMode == ZC_ONLINE)
		result = ZC_endCmpr_online(dataProperty, solution, cmprSize);
	else
		result = ZC_endCmpr_offline(dataProperty, solution, cmprSize);
#else
	result = ZC_endCmpr_offline(dataProperty, solution, cmprSize);
#endif
	return result;
}

void ZC_startDec()
{
#ifdef HAVE_MPI
	if(executionMode == ZC_ONLINE)
		ZC_startDec_online();
	else
		ZC_startDec_offline();
#else
	ZC_startDec_offline();
#endif	
}

void ZC_endDec(ZC_CompareData* compareResult, void *decData)
{
#ifdef HAVE_MPI
	if(executionMode == ZC_ONLINE)
		ZC_endDec_online(compareResult, decData);
	else
		ZC_endDec_offline(compareResult, decData);
#else
	ZC_endDec_offline(compareResult, decData);
#endif
}
