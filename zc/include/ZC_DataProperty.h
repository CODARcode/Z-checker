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
typedef struct{real Re; real Im; real Amp;} complex;

typedef struct ZC_DataProperty
{
	char* varName;
	int dataType; /*ZC_DOUBLE or ZC_FLOAT*/
	int r5;
	int r4;
	int r3;
	int r2;
	int r1;
	
	void *data;
	
	int numOfElem;
	double minValue;
	double maxValue;
	double valueRange;
	double avgValue;
	double entropy;
	double zeromean_variance;
	double* autocorr; /*array of autocorrelation coefficients*/
	complex* fftCoeff; /*array of fft coefficients*/
	double* lap;
} ZC_DataProperty;

void hash_init(HashEntry *table, int table_size);
int hash_get(HashEntry *table, unsigned long key, int table_size);
void hash_put(HashEntry *table, unsigned long key, int table_size);

void fft(complex *v, int n, complex *tmp);
void ifft(complex *v, int n, complex *tmp);

void computeLap(double *data, double *lap, int r5, int r4, int r3, int r2, int r1);

void freeDataProperty(ZC_DataProperty* dataProperty);
ZC_DataProperty* ZC_constructDataProperty(char* varName, int dataType, int r5, int r4, int r3, int r2, int r1, 
int numOfElem, double minValue, double maxValue, double valueRange, double avgValue, 
double entropy, double* autocorr, complex* fftCoeff);

complex* ZC_computeFFT(void* data, int n, int dataType);
ZC_DataProperty* ZC_genProperties_float(char* varName, float *data, int numOfElem, int r5, int r4, int r3, int r2, int r1);
ZC_DataProperty* ZC_genProperties_double(char* varName, double *data, int numOfElem, int r5, int r4, int r3, int r2, int r1);
ZC_DataProperty* ZC_genProperties(char* varName, int dataType, void *oriData, int r5, int r4, int r3, int r2, int r1);
void ZC_printDataProperty(ZC_DataProperty* property);
char** constructDataPropertyString(ZC_DataProperty* property);

void ZC_writeFFTResults(char* varName, complex* fftCoeff, char* tgtWorkspaceDir);
void ZC_writeDataProperty(ZC_DataProperty* property, char* tgtWorkspaceDir);
ZC_DataProperty* ZC_loadDataProperty(char* propResultFile);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_DataProperty_H  ----- */
