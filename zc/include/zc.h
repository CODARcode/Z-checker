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

#include <math.h>
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

#define ZC_BUFS 128
#define ZC_BUFS_LONG 1024
#define ZC_BUFS_SUPER_LONG 131072

#define ZC_VERNUM 0x0040
#define ZC_VER_MAJOR 0
#define ZC_VER_MINOR 6
#define ZC_VER_BUILD 0
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

#define HASHTABLE_SIZE_SMALL 64
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

#define COMPRESSOR_SZ 101
#define COMPRESSOR_ZFP 102
#define COMPRESSOR_MGARD 103
#define COMPRESSOR_FPZIP 104
#define COMPRESSOR_BITGROOMING 105
#define COMPRESSOR_DIGITROUNDING 106

#define CMD_SZ_ZC_VIS_ABS_1D "./sz-zc-vis DATATYPE -i INPUTDATAPATH -1 R1 -v VARNAME -k \"SOLUTION\" -C zc.config -M ABS -A ERRORBOUND"
#define CMD_SZ_ZC_VIS_ABS_2D "./sz-zc-vis DATATYPE -i INPUTDATAPATH -2 R1 R2 -v VARNAME -k \"SOLUTION\" -C zc.config -M ABS -A ERRORBOUND"
#define CMD_SZ_ZC_VIS_ABS_3D "./sz-zc-vis DATATYPE -i INPUTDATAPATH -3 R1 R2 R3 -v VARNAME -k \"SOLUTION\" -C zc.config -M ABS -A ERRORBOUND"
#define CMD_SZ_ZC_VIS_REL_1D "./sz-zc-vis DATATYPE -i INPUTDATAPATH -1 R1 -v VARNAME -k \"SOLUTION\" -C zc.config -M REL -R ERRORBOUND"
#define CMD_SZ_ZC_VIS_REL_2D "./sz-zc-vis DATATYPE -i INPUTDATAPATH -2 R1 R2 -v VARNAME -k \"SOLUTION\" -C zc.config -M REL -R ERRORBOUND"
#define CMD_SZ_ZC_VIS_REL_3D "./sz-zc-vis DATATYPE -i INPUTDATAPATH -3 R1 R2 R3 -v VARNAME -k \"SOLUTION\" -C zc.config -M REL -R ERRORBOUND"
#define CMD_ZFP_ZC_VIS_ABS_1D "./zfp-zc-vis -s DATATYPE -a ERRORBOUND -1 R1 -i INPUTDATAPATH -k \"SOLUTION\" -v VARNAME"
#define CMD_ZFP_ZC_VIS_ABS_2D "./zfp-zc-vis -s DATATYPE -a ERRORBOUND -2 R1 R2 -i INPUTDATAPATH -k \"SOLUTION\" -v VARNAME"
#define CMD_ZFP_ZC_VIS_ABS_3D "./zfp-zc-vis -s DATATYPE -a ERRORBOUND -3 R1 R2 R3 -i INPUTDATAPATH -k \"SOLUTION\" -v VARNAME"
#define CMD_ZFP_ZC_VIS_REL_1D "./zfp-zc-vis -s DATATYPE -a ERRORBOUND -1 R1 -i INPUTDATAPATH -k \"SOLUTION\" -v VARNAME -l"
#define CMD_ZFP_ZC_VIS_REL_2D "./zfp-zc-vis -s DATATYPE -a ERRORBOUND -2 R1 R2 -i INPUTDATAPATH -k \"SOLUTION\" -v VARNAME -l"
#define CMD_ZFP_ZC_VIS_REL_3D "./zfp-zc-vis -s DATATYPE -a ERRORBOUND -3 R1 R2 R3 -i INPUTDATAPATH -k \"SOLUTION\" -v VARNAME -l"


extern char *rscriptPath;

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
extern int plotImageFlag;
extern int plotDecImageFlag;

extern int plotDecSliceMode;
extern int nbPlotCRs;
extern char *plotCRs_str[MAX_VIS_DEC_CRS];
extern float plotCRs[MAX_VIS_DEC_CRS]; //at most 32 compression ratios specified by users.
extern int nbPlotCompressors;
extern int plotCompressors[MAX_NB_CMPR_CASES];

extern int minAbsErrFlag;
extern int avgAbsErrFlag;
extern int maxAbsErrFlag;
extern int errAutoCorrFlag;
extern int errAutoCorr3DFlag;
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
extern int SSIMIMAGE2DFlag;

extern int derivativeOrder1_sep_maxDiffFlag; //separate calculation for three matrices
extern int derivativeOrder1_sep_psnrFlag; //separate calculation for three matrices
extern int derivativeOrder1_sep_ssimFlag; //separate calculation for three matrices

extern int derivativeOrder1_psnrFlag; //aggregated calculation
extern int derivativeOrder2_psnrFlag; //aggregated calculation
extern int derivativeOrder1_ssimFlag; //aggregated calculation
extern int derivativeOrder2_ssimFlag; //aggregated calculation

extern int derivativeOrder1_sobolevFlag;

extern int plotAutoCorrFlag;

extern int plotAbsErrPDFFlag;
extern int plotErrAutoCorrFlag;
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
extern int compressorIDs[CMPR_MAX_LEN];
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

extern int visMode;
extern int ZSERVER_PORT;

typedef union eclint16
{
	short svalue;
	unsigned short usvalue;
	unsigned char byte[2];
} eclint16;

typedef union eclint32
{
	int ivalue;
	unsigned int uivalue;
	unsigned char byte[4];
} eclint32;

typedef union eclint64
{
	long lvalue;
	unsigned long ulvalue;
	unsigned char byte[8];
} eclint64;

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
	double errorBound;
	ZC_CompareData* cmprResult;
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

extern int plot_dec_data;

extern int nbVars;

void cost_startCmpr();
double cost_EndCmpr();
void cost_startDec();
double cost_endDec();

/*zc.h*/
int ZC_Init_NULL();
int ZC_Init(const char *configFilePath);
long ZC_computeDataLength(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

ZC_DataProperty* ZC_startCmpr_offline(const char* varName, int dataType, void* oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_DataProperty* ZC_startCmpr_offline_withDataAnalysis(const char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_CompareData* ZC_endCmpr_offline(ZC_DataProperty* dataProperty, const char* solution, long cmprSize);
void ZC_startDec_offline();
void ZC_endDec_offline(ZC_CompareData* compareResult, void *decData);

void ZC_plotCompressionRatio();

hashtable_t* createCmprsorErrBoundVarMap();

void ZC_plotHistogramResults(int cmpCount, char** compressorCases);

int getComparisonCases(char* cases[]);
void ZC_plotComparisonCases();

char** getCompResKeyList(const char* var, int* count);
char** extractRateDistortion_psnr(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_snr(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateCorrelation(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_psnr(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative2_psnr(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_ssim(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative2_ssim(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_sobolev(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_maxRelErr_dx(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_maxRelErr_dy(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_maxRelErr_dz(int totalCount, char** cmpResList, int* validLineNum);
char** extractRateDistortion_Derivative1_maxRelErr_dt(int totalCount, char** cmpResList, int* validLineNum);

StringLine* ZC_generateOverallRateDistortion_one_compressor(hashtable_t* cmprorErrVarMap, char* compressor);
void freeCmprorErrVarMap(hashtable_t* cmprorErrVarMap);
void ZC_plotOverallRateDistortion_one_compressor(StringLine* lines, char* compressor);
void ZC_plotOverallRateDistortion_all_compressors();

void ZC_plotRateDistortion();

void ZC_plotAutoCorr_CompressError();
void ZC_plotAutoCorr_DataProperty();
void ZC_plotFFTAmplitude_OriginalData();
void ZC_plotFFTAmplitude_DecompressData();
void ZC_plotErrDistribtion();
void ZC_plotSliceImage();

void ZC_generateCompressionFactorReport();
void ZC_generateRateDistortionReport();
void ZC_generateOverallRateDistortionReport();
void ZC_generateRateDerivatives_aggregated_Report();
void ZC_generateRateDerivatives_separated_Report();

void ZC_generateRateCorrelationReport();
void ZC_generateErrDistributionReport(CmprsorErrBound *allCompressors, int allCompressorCount);
void ZC_generateErrAutoCorrReport(CmprsorErrBound *allCompressors, int allCompressorCount);
void ZC_generateSpectrumDistortionReport(CmprsorErrBound *allCompressors, int allCompressorCount);

void ZC_updateZCRootTexFile(const char* dataSetName);
StringLine* ZC_generateDecSliceImageReport();
void ZC_generateOverallReport(const char* dataSetName);

int ZC_analyze_and_generateReport(const char* dataSetName);
int ZC_Finalize();

ZC_CompareData* ZC_registerVar(const char* name, int dataType, void* oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

ZC_CompareData** loadMultiVars(const char* multivarFile, int* nbVars, int* status);

//The following executeCmd_xxx interfaces are depreated. (Please see [ZC_package]/R/ for how to call R scripts from Z-checker instead. 
int ZC_executeCmd_GfloatVector(const char* cmd, int* count, float** data);
int ZC_executeCmd_GdoubleVector(const char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatVector(const char* cmd, int* count, float** data);
int ZC_executeCmd_RdoubleVector(const char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatMatrix(const char* cmd, int* m, int* n, float** data);
int ZC_executeCmd_RdoubleMatrix(const char* cmd, int* m, int* n, double** data);

//online interfaces
long ZC_computeDataLength_online(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_DataProperty* ZC_startCmpr_online(const char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_CompareData* ZC_endCmpr_online(ZC_DataProperty* dataProperty, const char* solution, long cmprSize);
void ZC_startDec_online();
void ZC_endDec_online(ZC_CompareData* compareResult, void *decData);


//overall interfaces for checkingStatus==PROBE_COMPRESSOR
ZC_DataProperty* ZC_startCmpr(const char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_CompareData* ZC_endCmpr(ZC_DataProperty* dataProperty, const char* solution, long cmprSize);
void ZC_startDec();
void ZC_endDec(ZC_CompareData* compareResult, void *decData);


#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_H  ----- */
