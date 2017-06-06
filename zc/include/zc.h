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
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>      /* For gettimeofday(), in microseconds */
#include <time.h>          /* For time(), in seconds */
#include "iniparser.h"
#include "ZC_DataProperty.h"
#include "ZC_CompareData.h"
#include "ZC_Hashtable.h"
#include "DynamicFloatArray.h"
#include "DynamicDoubleArray.h"
#include "DynamicIntArray.h"
#include "ZC_util.h"
#include "ZC_rw.h"
#include "ZC_ReportGenerator.h"


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
#define ZC_VER_REVISION 1

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

extern int sysEndianType; /*endian type of the system*/
extern int dataEndianType; /*endian type of the data*/

extern int checkingStatus;

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
extern int fftFlag;
extern int lapFlag;

extern int minAbsErrFlag;
extern int avgAbsErrFlag;
extern int maxAbsErrFlag;
extern int autoCorrAbsErrFlag;
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

extern int compressors_count; //this compressors_count is the number of compressors to be compared, set by zc.config
extern char* compressors[20];
extern char* compressors_dir[20];
extern char* compareData_dir[20];
extern char* properties_dir[20];

extern char* comparisonCases;

extern int numOfErrorBoundCases;

extern int allCompressorCount;
extern char* allCompressors[20];
extern int allErrorBoundCount;
extern char* allErrorBounds[20];
extern int allVarCaseCount;
extern char* allVarCases[20];

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

void cost_startCmpr();
double cost_EndCmpr();
void cost_startDec();
double cost_endDec();

/*conf.c*/
void loadProperty(char* property_dir, char* propertyVarName);
int ZC_ReadConf();
int ZC_LoadConf();
int modifyZCConfig(StringLine* confLinesHeader, char* targetAttribute, char* newStringValue);

/*ByteToolkit.c*/
int ZC_bytesToInt_bigEndian(unsigned char* bytes);
void ZC_intToBytes_bigEndian(unsigned char *b, unsigned int num);
long ZC_bytesToLong_bigEndian(unsigned char* b);
void ZC_longToBytes_bigEndian(unsigned char *b, unsigned long num);
long ZC_doubleToOSEndianLong(double value);
int ZC_floatToOSEndianInt(float value);

short ZC_bytesToShort(unsigned char* bytes);
int ZC_bytesToInt(unsigned char* bytes);
long ZC_bytesToLong(unsigned char* bytes);
float ZC_bytesToFloat(unsigned char* bytes);
void ZC_floatToBytes(unsigned char *b, float num);
double ZC_bytesToDouble(unsigned char* bytes);
void ZC_doubleToBytes(unsigned char *b, double num);

/*gnuplot.c*/
char** genGnuplotScript_linespoints(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);
char** genGnuplotScript_histogram(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel, long maxYValue);
char** genGnuplotScript_lines(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);
char** genGnuplotScript_fillsteps(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);

/*quicksort.c*/
int ZC_divide(RateDistElem* list,int start,int end);
void ZC_quick_sort(RateDistElem* list,int start,int end);
int ZC_divide2(StringElem* list,int start,int end);
void ZC_quick_sort2(StringElem* list,int start,int end);

/*ec.h*/
int ZC_Init(char *configFilePath);
int ZC_computeDataLength(int r5, int r4, int r3, int r2, int r1);

ZC_DataProperty* ZC_startCmpr(char* varName, int dataType, void* oriData, int r5, int r4, int r3, int r2, int r1);
ZC_DataProperty* ZC_startCmpr_withDataAnalysis(char* varName, int dataType, void *oriData, int r5, int r4, int r3, int r2, int r1);
ZC_CompareData* ZC_endCmpr(ZC_DataProperty* dataProperty, int cmprSize);
void ZC_startDec();
void ZC_endDec(ZC_CompareData* compareResult, char* solution, void *decData);

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

void ZC_Finalize();

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_H  ----- */
