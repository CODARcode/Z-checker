/**
 *  @file ZC_DataProperty.h
 *  @author Sheng Di
 *  @date July, 2016
 *  @brief Header file for the ZC_CompareData.c.
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_DataProperty_H
#define _ZC_DataProperty_H

#ifdef __cplusplus
extern "C" {
#endif

struct HashEntry_s{
	int flag;
    unsigned long key;
	int num;
};

typedef struct HashEntry_s HashEntry;

typedef double real;
typedef struct{real Re; real Im; real Amp;} ZC_Complex;

typedef struct ZC_ELEMENT
{
	double value;
	unsigned int counter;
} ZC_ELEMENT;

typedef struct ZC_DataProperty
{
	char* varName;
	char* filePath;
	int dataType; /*ZC_DOUBLE or ZC_FLOAT*/
	size_t r5;
	size_t r4;
	size_t r3;
	size_t r2;
	size_t r1;
	
	void *data;
	void* sliceImage_ori;
	void* sliceImage_log;
	
	long numOfElem;
	double minValue;
	double maxValue;
	double valueRange;
	double avgValue;
	double entropy;
	double entropyFloatpoint;
	double zeromean_variance;
	double* autocorr; /*array of autocorrelation coefficients*/
	void* autocorr3D; //double* or float*, depending on the floating type of the data
	ZC_Complex* fftCoeff; /*array of fft coefficients*/
	double* lap;
	
} ZC_DataProperty;

void hash_init(HashEntry *table, size_t table_size);
size_t hash_get(HashEntry *table, unsigned long key, size_t table_size);
void hash_put(HashEntry *table, unsigned long key, size_t table_size);

double* ZC_compute_autocorrelation1D_float(float* data, size_t numOfElem, double avg);
double* ZC_compute_autocorrelation1D_double(double* data, size_t numOfElem, double avg);

int ZC_computeDimension(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void ZC_constructDimString(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, char* output);
int ZC_constructDimArray(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t *dims);
void fft(ZC_Complex *v, size_t n, ZC_Complex *tmp);
void ifft(ZC_Complex *v, size_t n, ZC_Complex *tmp);

void computeLap(double *data, double *lap, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

void freeDataProperty_internal(ZC_DataProperty* dataProperty);
int freeDataProperty(ZC_DataProperty* dataProperty);

ZC_DataProperty* ZC_constructDataProperty(char* varName, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, 
size_t numOfElem, double minValue, double maxValue, double valueRange, double avgValue, 
double entropy, double* autocorr, ZC_Complex* fftCoeff, char* filePath);

ZC_Complex* ZC_computeFFT(void* data, size_t n, int dataType);
double computeLosslessEntropy_32bits(void* data, size_t nbEle);
ZC_DataProperty* ZC_genProperties_float(const char* varName, float *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

double computeLosslessEntropy_64bits(void* data, size_t nbEle);
ZC_DataProperty* ZC_genProperties_double(const char* varName, double *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_DataProperty* ZC_genProperties(const char* varName, int dataType, void *oriData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

int ZC_moveDataProperty(ZC_DataProperty* target, ZC_DataProperty* source);

void ZC_printDataProperty(ZC_DataProperty* property);
char** constructDataPropertyString(ZC_DataProperty* property);

void ZC_writeFFTResults(const char* varName, ZC_Complex* fftCoeff, const char* tgtWorkspaceDir);
void ZC_writeDataProperty(ZC_DataProperty* property, const char* tgtWorkspaceDir);
ZC_DataProperty* ZC_loadDataProperty(const char* propResultFile);

//online interfaces
void ZC_genBasicProperties_float_online(float* data, size_t numOfElem, ZC_DataProperty* property);
void ZC_genBasicProperties_double_online(double* data, size_t numOfElem, ZC_DataProperty* property);
ZC_DataProperty* ZC_genProperties_float_online(const char* varName, float *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_DataProperty* ZC_genProperties_double_online(const char* varName, double *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_DataProperty_H  ----- */
