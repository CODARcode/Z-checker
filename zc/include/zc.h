/**
 *  @file ec.h
 *  @author Sheng Di, Dingwen Tao
 *  @date April, 2015
 *  @brief Header file for the whole detector.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_H
#define _ZC_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>      /* For gettimeofday(), in microseconds */
#include <time.h>          /* For time(), in seconds */
#include "iniparser.h"
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "ZC_Hashtable.h"
#include "ZC_DataSetHandler.h"
#include "DynamicFloatArray.h"
#include "DynamicDoubleArray.h"
#include "DynamicIntArray.h"
//#include "ZC_util.h"
#include "ZC_rw.h"
//#include "ZC_ReportGenerator.h"
#include "ZC_gnuplot.h"
#include "ZC_latex.h"
#include "ZC_ByteToolkit.h"
#include "ZC_conf.h"
#ifdef HAVE_MPI
#include <mpi.h>
#endif

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif

#define ZC_SCES 0  /*successful*/
#define ZC_NSCS -1 /* not successful*/
#define ZC_CMDE -2 /*command execution error*/
#define ZC_FOPF -3 /*Failed to open input file*/

#define max(a,b) ((a) > (b) ? (a) : (b))
#define min(a,b) ((a) < (b) ? (a) : (b))

#define ZC_BUFS 128
#define ZC_BUFS_LONG 1024
#define ZC_BUFS_SUPER_LONG 131072

#define ZC_VERNUM 0x0010
#define ZC_VER_MAJOR 0
#define ZC_VER_MINOR 1
#define ZC_VER_BUILD 3
#define ZC_VER_REVISION 0

#define PDF_INTERVALS 2000
#define PDF_INTERVALS_REL 50000
#define AUTOCORR_SIZE 100
#define FFT_SIZE 128
#define ENTROPY_BLOCK_SIZE 100

#define ABS 0
#define REL 1
#define ABS_AND_REL 2
#define ABS_OR_REL 3

#define ZC_FLOAT 0
#define ZC_DOUBLE 1
#define ZC_INT32 2 //only used in ZC_DataSet to represent the analysis results instead of the original data
#define ZC_INT16 3 
#define ZC_INT8 4

#define LITTLE_ENDIAN_DATA 0
#define BIG_ENDIAN_DATA 1 /*big_endian (ppc, max, etc.) ; little_endian (x86, x64, etc.)*/

#define LITTLE_ENDIAN_SYSTEM 0
#define BIG_ENDIAN_SYSTEM 1

#define DynArrayInitLen 1024

#define MAX_MSG_LENGTH 1024

#define HASHTABLE_SIZE 65536

#define CMD_OUTPUT_BUF 200
#define DynamicArrayInitLength 1024

#define PROBE_COMPRESSOR 0
#define ANALYZE_DATA 1
#define COMPARE_COMPRESSOR 2

#define PWR_DIS_RNG_BOUND 1000 /*the upper bound of the value range for computing the distribution of pwr error*/

#define GNUPLOT_FONT 22

#define CMPR_MAX_LEN 20
#define PROP_MAX_LEN 20
#define CMPRNAME_STR_BUFSIZE 50
#define ERRBOUND_MAX_LEN 50
#define ERRBOUND_STR_BUFSIZE 50

#define ZC_OFFLINE 0
#define ZC_ONLINE 1

extern char rscriptPath[MAX_MSG_LENGTH];

extern int sysEndianType; /*endian type of the system*/
extern int dataEndianType; /*endian type of the data*/

extern int checkingStatus;
extern int executionMode;

extern char *zc_cfgFile;

/*extern char* ZC_workspaceDir;*/

extern int errorBoundMode;
extern double absErrBound;
extern double relBoundRatio;

extern int compressTimeFlag;
extern int decompressTimeFlag;
extern int compressSizeFlag;

extern int minValueFlag;
extern int maxValueFlag;
extern int valueRangeFlag;
extern int avgValueFlag;
extern int entropyFlag;
extern int autocorrFlag;
extern int autocorr3DFlag;
extern int fftFlag;
extern int lapFlag;

extern int minAbsErrFlag;
extern int avgAbsErrFlag;
extern int maxAbsErrFlag;
extern int autoCorrAbsErrFlag;
extern int autoCorrAbsErr3DFlag;
extern int absErrPDFFlag;
extern int pwrErrPDFFlag;

extern int minRelErrFlag;
extern int avgRelErrFlag;
extern int maxRelErrFlag;

extern int rmseFlag;
extern int nrmseFlag;
extern int snrFlag;
extern int psnrFlag;
extern int valErrCorrFlag;
extern int pearsonCorrFlag;

extern int KS_testFlag;
extern int SSIMFlag;

extern int plotAbsErrPDFFlag;
extern int plotAutoCorrFlag;
extern int plotFFTAmpFlag;
extern int plotEntropyFlag;

extern int checkCompressorsFlag;

extern int generateReportFlag;
extern char* reportTemplateDir;

extern int ZC_versionNumber[3];

extern struct timeval startCmprTime;
extern struct timeval endCmprTime;  /* Start and end times */
extern struct timeval startDecTime;
extern struct timeval endDecTime;
extern double totalCmprCost;
extern double totalDecCost;

extern double initTime;
extern double endTime;

extern long globalCmprSize;

extern int compressors_count; //this compressors_count is the number of compressors to be compared, set by zc.config
extern char* compressors[CMPR_MAX_LEN];
extern char* compressors_dir[CMPR_MAX_LEN];
extern char* compareData_dir[CMPR_MAX_LEN];
extern char* properties_dir[PROP_MAX_LEN];

extern char* comparisonCases;

extern int numOfErrorBoundCases;

#ifdef HAVE_MPI
extern MPI_Comm ZC_COMM_WORLD;
#endif

extern int myRank;
extern int nbProc;

extern size_t globalDataLength;

extern int initStatus; 

typedef union eclshort
{
	unsigned short svalue;
	unsigned char byte[2];
} eclshort;

typedef union ecldouble
{
    double value;
    unsigned long lvalue;
    unsigned char byte[8];
} ecldouble;

typedef union eclfloat
{
    float value;
    unsigned int ivalue;
    unsigned char byte[4];
} eclfloat;

typedef struct RateDistElem_t
{
	double rate;
	double psnr;
	double maxAbsErr;
	double compressRate;
} *RateDistElem;

typedef struct StringElem_t
{
	char* str;
	double value;
} *StringElem;

typedef struct CmprsorErrBound
{
	char compressorName[CMPRNAME_STR_BUFSIZE];
	int allErrBoundCount;
	StringElem *allErrBounds;
	int selErrBoundCount;
	StringElem *selErrBounds;
} CmprsorErrBound;

extern int allCompressorCount;
extern CmprsorErrBound allCompressors[CMPR_MAX_LEN];

//extern char* allCompressors[20];
//extern int allErrorBoundCount;
//extern char* allErrorBounds[20];
extern int allVarCaseCount;
extern char* allVarCases[20];

void cost_startCmpr();
double cost_EndCmpr();
void cost_startDec();
double cost_endDec();

/*zc.h*/
int ZC_Init_NULL();
int ZC_Init(char *configFilePath);
long ZC_computeDataLength(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

ZC_DataProperty* ZC_startCmpr_offline(char* varName, int dataType, void* oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_DataProperty* ZC_startCmpr_offline_withDataAnalysis(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_CompareData* ZC_endCmpr_offline(ZC_DataProperty* dataProperty, char* solution, long cmprSize);
void ZC_startDec_offline();
void ZC_endDec_offline(ZC_CompareData* compareResult, void *decData);

void ZC_plotCompressionRatio();
void ZC_plotHistogramResults(int cmpCount, char** compressorCases);

int getComparisonCases(char* cases[]);
void ZC_plotComparisonCases();

char** getCompResKeyList(char* var, int* count);
char** extractRateDistortion_psnr(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_snr(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateCorrelation(int totalCount, char** cmpResList, int* validLineNum);
void ZC_plotRateDistortion();

void ZC_plotAutoCorr_CompressError();
void ZC_plotAutoCorr_DataProperty();
void ZC_plotFFTAmplitude_OriginalData();
void ZC_plotFFTAmplitude_DecompressData();
void ZC_plotErrDistribtion();

void ZC_generateCompressionFactorReport();
void ZC_generateRateDistortionReport();
void ZC_generateRateCorrelationReport();
void ZC_generateErrDistributionReport(CmprsorErrBound *allCompressors, int allCompressorCount);
void ZC_generateErrAutoCorrReport(CmprsorErrBound *allCompressors, int allCompressorCount);
void ZC_generateSpectrumDistortionReport(CmprsorErrBound *allCompressors, int allCompressorCount);

void ZC_updateZCRootTexFile(char* dataSetName);
void ZC_generateOverallReport(char* dataSetName);

int ZC_Finalize();

ZC_CompareData* ZC_registerVar(char* name, int dataType, void* oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

ZC_CompareData** loadMultiVars(char* multivarFile, int* nbVars, int* status);

//The following executeCmd_xxx interfaces are depreated. (Please see [ZC_package]/R/ for how to call R scripts from Z-checker instead. 
int ZC_executeCmd_GfloatVector(char* cmd, int* count, float** data);
int ZC_executeCmd_GdoubleVector(char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatVector(char* cmd, int* count, float** data);
int ZC_executeCmd_RdoubleVector(char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatMatrix(char* cmd, int* m, int* n, float** data);
int ZC_executeCmd_RdoubleMatrix(char* cmd, int* m, int* n, double** data);

//online interfaces
long ZC_computeDataLength_online(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_DataProperty* ZC_startCmpr_online(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_CompareData* ZC_endCmpr_online(ZC_DataProperty* dataProperty, char* solution, long cmprSize);
void ZC_startDec_online();
void ZC_endDec_online(ZC_CompareData* compareResult, void *decData);


//overall interfaces for checkingStatus==PROBE_COMPRESSOR
ZC_DataProperty* ZC_startCmpr(char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_CompareData* ZC_endCmpr(ZC_DataProperty* dataProperty, char* solution, long cmprSize);
void ZC_startDec();
void ZC_endDec(ZC_CompareData* compareResult, void *decData);


#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_H  ----- */
