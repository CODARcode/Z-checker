#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include "ZC_DataProperty.h"
#include "zc.h"
#include "iniparser.h"

/* For entropy calculation */
void hash_init(HashEntry *table, int table_size)
{
	int i;
	for (i = 0; i < table_size; i++)
		table[i].flag = 0;
}

int hash_get(HashEntry *table, unsigned long key, int table_size)
{
      int hash = (key % table_size);
      while (table[hash].flag != 0 && table[hash].key != key)
            hash = (hash + 1) % table_size;
      if (table[hash].flag == 0)
            return -1;
      else
            return table[hash].num;
}

void hash_put(HashEntry *table, unsigned long key, int table_size)
{
      int hash = (key % table_size);
      while (table[hash].flag != 0 && table[hash].key != key)
            hash = (hash + 1) % table_size;
      if (table[hash].flag != 0)
      	table[hash].num++;
      else
      {
      	table[hash].flag = 1;
    	table[hash].key = key;
      	table[hash].num = 1;
      }
}

/* For FFT and iFFT calculation */
void fft(complex *v, int n, complex *tmp)
{
	if(n>1) {			/* otherwise, do nothing and return */
		int k,m;
		complex z, w, *vo, *ve;
		ve = tmp;
		vo = tmp+n/2;
		for(k=0; k<n/2; k++) {
			ve[k] = v[2*k];
			vo[k] = v[2*k+1];
		}
		fft(ve, n/2, v);		/* FFT on even-indexed elements of v[] */
		fft(vo, n/2, v);		/* FFT on odd-indexed elements of v[] */
		for(m = 0; m < n/2; m++) {
			w.Re = cos(2*PI*m/(double)n);
			w.Im = -sin(2*PI*m/(double)n);
			z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
			z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;
			v[m+n/2].Re = ve[m].Re - z.Re;
			v[m+n/2].Im = ve[m].Im - z.Im;
		}
	}
	return;
}

void ifft(complex *v, int n, complex *tmp)
{
	if(n>1) {			/* otherwise, do nothing and return */
		int k,m;
		complex z, w, *vo, *ve;
		ve = tmp;
		vo = tmp+n/2;
		for(k = 0; k < n/2; k++) {
			ve[k] = v[2*k];
			vo[k] = v[2*k+1];
		}
		ifft(ve, n/2, v);		/* FFT on even-indexed elements of v[] */
		ifft(vo, n/2, v);		/* FFT on odd-indexed elements of v[] */
		for(m = 0; m < n/2; m++) {
			w.Re = cos(2*PI*m/(double)n);
			w.Im = sin(2*PI*m/(double)n);
			z.Re = w.Re*vo[m].Re - w.Im*vo[m].Im;	/* Re(w*vo[m]) */
			z.Im = w.Re*vo[m].Im + w.Im*vo[m].Re;	/* Im(w*vo[m]) */
			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;
			v[m+n/2].Re = ve[m].Re - z.Re;
			v[m+n/2].Im = ve[m].Im - z.Im;
		}
	}
	return;
}

void computeLap(double *data, double *lap, int r5, int r4, int r3, int r2, int r1)
{
	if (r2 == 0)		// compute Laplacian of 1D data
	{
		int x, y;
			for (x = 0; x < r1; x++) {
				unsigned long i = max(1u, min(x, r1 - 2));
				double fxx = 1 * data[(i - 1)] - 2 * data[(i + 0)] + 1 * data[(i + 1)];
				double ddf = fxx;
				lap[x] = ddf;
			}
	}
	else if (r3 ==0)	// computer Laplacian of 2D data
	{
		int x, y;
		for (y = 0; y < r2; y++) {
			unsigned long j = max(1u, min(y, r2 - 2));
			for (x = 0; x < r1; x++) {
				unsigned long i = max(1u, min(x, r1 - 2));
				double fxx = +1 * data[(i - 1) + r1 * (j + 0)]
						-2 * data[(i + 0) + r1 * (j + 0)]
								+1 * data[(i + 1) + r1 * (j + 0)];
				double fyy = +1 * data[(i + 0) + r1 * (j - 1)]
						-2 * data[(i + 0) + r1 * (j + 0)]
								+1 * data[(i + 0) + r1 * (j + 1)];
				double ddf = fxx + fyy;
				lap[x + y * r1] = ddf;
			}
		}
	}
	else if (r4 == 0)	/*computer Laplacian of 3D data*/
	{
		int x, y, z;
		for (z = 0; z < r3; z++) {
			unsigned long k = max(1u, min(z, r3 - 2));
			for (y = 0; y < r2; y++) {
				unsigned long j = max(1u, min(y, r2 - 2));
				for (x = 0; x < r1; x++) {
					unsigned long i = max(1u, min(x, r1 - 2));
					double fxx = +1 * data[(i - 1) + r1 * ((j + 0) + r2 * (k + 0))]
											   -2 * data[(i + 0) + r1 * ((j + 0) + r2 * (k + 0))]
															 +1 * data[(i + 1) + r1 * ((j + 0) + r2 * (k + 0))];
					double fyy = +1 * data[(i + 0) + r1 * ((j - 1) + r2 * (k + 0))]
											   -2 * data[(i + 0) + r1 * ((j + 0) + r2 * (k + 0))]
															 +1 * data[(i + 0) + r1 * ((j + 1) + r2 * (k + 0))];
					double fzz = +1 * data[(i + 0) + r1 * ((j + 0) + r2 * (k - 1))]
											   -2 * data[(i + 0) + r1 * ((j + 0) + r2 * (k + 0))]
															 +1 * data[(i + 0) + r1 * ((j + 0) + r2 * (k + 1))];
					double ddf = fxx + fyy + fzz;
					lap[x + y * r1 + z * r1 * r2] = ddf;
				}
			}
		}
	}
	/* TODO: computer Laplacian of 4D and 5D data*/

	return;
}

void freeDataProperty(ZC_DataProperty* dataProperty)
{
	//free(dataProperty->varName);
	if(dataProperty->autocorr!=NULL)
		free(dataProperty->autocorr);
	if(dataProperty->fftCoeff!=NULL)
		free(dataProperty->fftCoeff);
	if(dataProperty->lap!=NULL)
		free(dataProperty->lap);
	free(dataProperty);
}

ZC_DataProperty* ZC_constructDataProperty(char* varName, int dataType, int r5, int r4, int r3, int r2, int r1, 
int numOfElem, double minValue, double maxValue, double valueRange, double avgValue, 
double entropy, double* autocorr, complex* fftCoeff)
{
	ZC_DataProperty* this = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	this->varName = varName;
	this->dataType = dataType;
	this->r5 = r5;
	this->r4 = r4;
	this->r3 = r3;
	this->r2 = r2;
	this->r1 = r1;
	this->numOfElem = numOfElem;
	this->minValue = minValue;
	this->maxValue = maxValue;
	this->valueRange = valueRange;
	this->avgValue = avgValue;
	this->entropy = entropy;
	this->autocorr = autocorr;
	this->fftCoeff = fftCoeff;
	return this;
}

complex* ZC_computeFFT(void* data, int n, int dataType)
{
	int i;
	complex *fftCoeff = (complex*)malloc(n*sizeof(complex));
    complex *scratch  = (complex*)malloc(n*sizeof(complex));

	if(dataType==ZC_FLOAT)
	{
		float* data_ = (float*)data;
		for (i = 0; i < n; i++)
		{
			fftCoeff[i].Re = data_[i];
			fftCoeff[i].Im = 0;
		}		
	}
	else if(dataType==ZC_DOUBLE)
	{
		double* data_ = (double*)data;
		for (i = 0; i < n; i++)
		{
			fftCoeff[i].Re = data_[i];
			fftCoeff[i].Im = 0;
		}
	}
	else
	{
		printf("Error: Wrong data type!\n");
		exit(0);
	}

	fft(fftCoeff, n, scratch);
    
    for (i = 0; i < n; i++)
    {
        fftCoeff[i].Amp = sqrt(fftCoeff[i].Re*fftCoeff[i].Re + fftCoeff[i].Im*fftCoeff[i].Im);
    }
    
    free(scratch);
    
	return fftCoeff;
}

ZC_DataProperty* ZC_genProperties_float(char* varName, float *data, int numOfElem, int r5, int r4, int r3, int r2, int r1)
{
	int i = 0;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));
	
	property->varName = varName;
	property->dataType = ZC_FLOAT;
	property->data = data;
	
	property->numOfElem = numOfElem;
	double min=data[0],max=data[0],sum=0,avg,valueRange;

    for(i=0;i<numOfElem;i++)
	{
		if(min>data[i]) min = data[i];
		if(max<data[i]) max = data[i];
		sum += data[i];
	}

	double med = min+(max-min)/2;
	double sum_of_square = 0;
	for(i=0;i<numOfElem;i++)
		sum_of_square += (data[i] - med)*(data[i] - med);
	property->zeromean_variance = sum_of_square/numOfElem;

	avg = sum/numOfElem;
	valueRange = max - min;

	if (minValueFlag)
		property->minValue = min;

	if (maxValueFlag)
		property->maxValue = max;

	if (avgValueFlag)
		property->avgValue = avg;
	
	if (valueRangeFlag)
		property->valueRange = valueRange;
    
	if(entropyFlag)
	{
		double absErr = 1E-3 * valueRange; /*TODO change fixed value to user input*/
		double entVal = 0.0;
		int table_size;
		if (valueRange/absErr > numOfElem)
			table_size = (int)(valueRange/absErr);
		else
			table_size = numOfElem;
		HashEntry *table = (HashEntry*)malloc(table_size*sizeof(HashEntry));
		hash_init(table,table_size);

		for (i = 0; i < numOfElem; i++)
 			hash_put(table, (unsigned long)((data[i]-min)/absErr), table_size);
 
		for (i = 0; i<table_size; i++)
			if (table[i].flag != 0)
			{
				double prob = (double)table[i].num/table_size;
				entVal -= prob*log(prob)/log(2);
			}

		property->entropy = entVal;
		free(table);
	}

	if(autocorrFlag)
	{
		double *autocorr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));

		int delta;

		if (numOfElem > 4096)
		{
			double cov = 0;
			for (i = 0; i < numOfElem; i++)
				cov += (data[i] - avg)*(data[i] - avg);

			cov = cov/numOfElem;

			if (cov == 0)
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
					autocorr[delta] = 0;
			}
			else
			{
				for(delta = 1; delta <= AUTOCORR_SIZE; delta++)
				{
					double sum = 0;

					for (i = 0; i < numOfElem-delta; i++)
						sum += (data[i]-avg)*(data[i+delta]-avg);

					autocorr[delta] = sum/(numOfElem-delta)/cov;
				}
			}
		}
		else
		{
			for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
			{
				double avg_0 = 0;
				double avg_1 = 0;

				for (i = 0; i < numOfElem-delta; i++)
				{
					avg_0 += data[i];
					avg_1 += data[i+delta];
				}

				avg_0 = avg_0 / (numOfElem-delta);
				avg_1 = avg_1 / (numOfElem-delta);

				double cov_0 = 0;
				double cov_1 = 0;

				for (i = 0; i < numOfElem-delta; i++)
				{
					cov_0 += (data[i]-avg_0)*(data[i]-avg_0);
					cov_1 += (data[i+delta]-avg_1)*(data[i+delta]-avg_1);
				}

				cov_0 = cov_0/(numOfElem-delta);
				cov_1 = cov_1/(numOfElem-delta);

				cov_0 = sqrt(cov_0);
				cov_1 = sqrt(cov_1);

				if (cov_0*cov_1 == 0)
				{
					for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
						autocorr[delta] = 0;
				}
				else
				{
					double sum = 0;

					for (i = 0; i < numOfElem-delta; i++)
						sum += (data[i]-avg_0)*(data[i+delta]-avg_1);

					autocorr[delta] = sum/(numOfElem-delta)/(cov_0*cov_1);
				}
			}
		}

		autocorr[0] = 1;
		property->autocorr = autocorr;
	}
	
	if(fftFlag)
	{
        int fft_size = pow(2, (int)log2(numOfElem));
        property->fftCoeff = ZC_computeFFT(data, fft_size, ZC_FLOAT);
	}
	
	if (lapFlag)
	{
		double *lap = (double*)malloc(numOfElem*sizeof(double));
		double *ddata = (double*)malloc(numOfElem*sizeof(double));
		for(i=0;i<numOfElem;i++)
			ddata[i] = data[i];
		computeLap(ddata, lap, r5, r4, r3, r2, r1);
		free(ddata);
		property->lap = lap;
	}

	return property;
}

ZC_DataProperty* ZC_genProperties_double(char* varName, double *data, int numOfElem, int r5, int r4, int r3, int r2, int r1)
{
	int i = 0;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));

	property->varName = varName;
	property->dataType = ZC_FLOAT;
	property->data = data;

	property->numOfElem = numOfElem;
	double min=data[0],max=data[0],sum=0,avg,valueRange;

	for(i=0;i<numOfElem;i++)
	{
		if(min>data[i]) min = data[i];
		if(max<data[i]) max = data[i];
		sum += data[i];
	}

	avg = sum/numOfElem;
	valueRange = max - min;

	if (minValueFlag)
		property->minValue = min;

	if (maxValueFlag)
		property->maxValue = max;

	if (avgValueFlag)
		property->avgValue = avg;

	if (valueRangeFlag)
		property->valueRange = valueRange;

	if(entropyFlag)
	{
		double absErr = 1E-3 * valueRange; /*TODO change fixed value to user input*/
		double entVal = 0.0;
		int table_size;
		if (valueRange/absErr > numOfElem)
			table_size = (int)(valueRange/absErr);
		else
			table_size = numOfElem;
		printf ("value range = %lf, table_size = %d\n", valueRange, table_size);
		HashEntry *table = (HashEntry*)malloc(table_size*sizeof(HashEntry));
		hash_init(table,table_size);

		for (i = 0; i < numOfElem; i++)
		{
			hash_put(table, (unsigned long)((data[i]-min)/absErr), table_size);
		}

		for (i = 0; i<table_size; i++)
			if (table[i].flag != 0)
			{
				double prob = (double)table[i].num/table_size;
				entVal -= prob*log(prob)/log(2);
			}

		property->entropy = entVal;
	}

	if(autocorrFlag)
	{
		double *autocorr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));

		int delta;

		if (numOfElem > 4096)
		{
			double cov = 0;
			for (i = 0; i < numOfElem; i++)
				cov += (data[i] - avg)*(data[i] - avg);

			cov = cov/numOfElem;

			if (cov == 0)
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
					autocorr[delta] = 0;
			}
			else
			{
				for(delta = 1; delta <= AUTOCORR_SIZE; delta++)
				{
					double sum = 0;

					for (i = 0; i < numOfElem-delta; i++)
						sum += (data[i]-avg)*(data[i+delta]-avg);

					autocorr[delta] = sum/(numOfElem-delta)/cov;
				}
			}
		}
		else
		{
			for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
			{
				double avg_0 = 0;
				double avg_1 = 0;

				for (i = 0; i < numOfElem-delta; i++)
				{
					avg_0 += data[i];
					avg_1 += data[i+delta];
				}

				avg_0 = avg_0 / (numOfElem-delta);
				avg_1 = avg_1 / (numOfElem-delta);

				double cov_0 = 0;
				double cov_1 = 0;

				for (i = 0; i < numOfElem-delta; i++)
				{
					cov_0 += (data[i]-avg_0)*(data[i]-avg_0);
					cov_1 += (data[i+delta]-avg_1)*(data[i+delta]-avg_1);
				}

				cov_0 = cov_0/(numOfElem-delta);
				cov_1 = cov_1/(numOfElem-delta);

				cov_0 = sqrt(cov_0);
				cov_1 = sqrt(cov_1);

				if (cov_0*cov_1 == 0)
				{
					for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
						autocorr[delta] = 0;
				}
				else
				{
					double sum = 0;

					for (i = 0; i < numOfElem-delta; i++)
						sum += (data[i]-avg_0)*(data[i+delta]-avg_1);

					autocorr[delta] = sum/(numOfElem-delta)/(cov_0*cov_1);
				}
			}
		}

		autocorr[0] = 1;
		property->autocorr = autocorr;
	}

	if(fftFlag)
	{
        int fft_size = pow(2, (int)log2(numOfElem));
        property->fftCoeff = ZC_computeFFT(data, fft_size, ZC_DOUBLE);
	}

	if (lapFlag)
	{
		double *lap = (double*)malloc(numOfElem*sizeof(double));
		computeLap(data, lap, r5, r4, r3, r2, r1);
		property->lap = lap;
	}

	return property;
}

ZC_DataProperty* ZC_genProperties(char* varName, int dataType, void *oriData, int r5, int r4, int r3, int r2, int r1)
{
	ZC_DataProperty* property = NULL;
	int numOfElem = ZC_computeDataLength(r5,r4,r3,r2,r1);
	if(dataType==ZC_FLOAT)
	{
		float* data = (float*)oriData;
		property = ZC_genProperties_float(varName, data, numOfElem, r5, r4, r3, r2, r1);
	}
	else if(dataType==ZC_DOUBLE)
	{
		double* data = (double*)oriData;
		property = ZC_genProperties_double(varName, data, numOfElem, r5, r4, r3, r2, r1);
	}
	else
	{
		printf("Error: dataType is wrong!\n");
		exit(0);
	}
	if(property!=NULL)
	{
		property->r5 = r5;
		property->r4 = r4;
		property->r3 = r3;
		property->r2 = r2;
		property->r1 = r1;
	}

	return property;
}

void ZC_printDataProperty(ZC_DataProperty* property)
{
	printf("number of elements: %d\n", property->numOfElem);
	printf("min value: %f\n", property->minValue);
	printf("avg value: %f\n", property->avgValue);
	printf("max value: %f\n", property->maxValue);
	printf("value range: %f\n", property->valueRange);
}

char** constructDataPropertyString(ZC_DataProperty* property)
{
	char** s = (char**)malloc(15*sizeof(char*));
	
	s[0] = (char*)malloc(100*sizeof(char));
	sprintf(s[0], "[PROPERTY]\n");
	
	s[1] = (char*)malloc(100*sizeof(char));
	sprintf(s[1], "varName = %s\n", property->varName);
	s[2] = (char*)malloc(100*sizeof(char));
	sprintf(s[2], "dataType = %d\n", property->dataType);	
	s[3] = (char*)malloc(100*sizeof(char));
	sprintf(s[3], "r5 = %d\n", property->r5);
	s[4] = (char*)malloc(100*sizeof(char));
	sprintf(s[4], "r4 = %d\n", property->r4);
	s[5] = (char*)malloc(100*sizeof(char));
	sprintf(s[5], "r3 = %d\n", property->r3);
	s[6] = (char*)malloc(100*sizeof(char));
	sprintf(s[6], "r2 = %d\n", property->r2);
	s[7] = (char*)malloc(100*sizeof(char));
	sprintf(s[7], "r1 = %d\n", property->r1);	
	
	s[8] = (char*)malloc(100*sizeof(char));
	sprintf(s[8], "numOfElem = %d\n", property->numOfElem);
	s[9] = (char*)malloc(100*sizeof(char));
	sprintf(s[9], "minValue = %.10G\n", property->minValue);
	s[10] = (char*)malloc(100*sizeof(char));
	sprintf(s[10], "maxValue = %.10G\n", property->maxValue);
	
	s[11] = (char*)malloc(100*sizeof(char));
	sprintf(s[11], "valueRange = %.10G\n", property->valueRange);
	
	s[12] = (char*)malloc(100*sizeof(char));
	sprintf(s[12], "avgValue = %.10G\n", property->avgValue);
	s[13] = (char*)malloc(100*sizeof(char));
	sprintf(s[13], "entropy = %.10G\n", property->entropy);
	s[14] = (char*)malloc(100*sizeof(char));
	sprintf(s[14], "autocorr = %.10G\n", (property->autocorr)[1]);
	
	return s;
}

void ZC_writeFFTResults(char* varName, complex* fftCoeff, char* tgtWorkspaceDir)
{
	int i;
	char tgtFilePath[ZC_BUFS] = {0};
	if(fftCoeff!=NULL)
	{
		//write coefficients
		char *ss[FFT_SIZE+1];
		ss[0] = (char*)malloc(sizeof(char)*50);
		sprintf(ss[0], "#Frequency Real Imag\n");

		for(i=0;i<FFT_SIZE;i++)
		{
			ss[i+1] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(ss[i+1], "%d/%d %f %f\n", i, FFT_SIZE, fftCoeff[i].Re, fftCoeff[i].Im);
		}
		memset(tgtFilePath, 0, ZC_BUFS);
		sprintf(tgtFilePath, "%s/%s.fft", tgtWorkspaceDir, varName);
		ZC_writeStrings(FFT_SIZE+1, ss, tgtFilePath);

		//write amplitudes
		sprintf(ss[0], "#Frequency Amplitude\n");
		for(i=0;i<FFT_SIZE;i++)
		{
//			double Re = fftCoeff[i].Re;
//			double Im = fftCoeff[i].Im;
//			double amplitude = sqrt(Re*Re+Im*Im);
			sprintf(ss[i+1], "%d/%d %f\n", i, FFT_SIZE, fftCoeff[i].Amp);
		}
		memset(tgtFilePath, 0, ZC_BUFS);
		sprintf(tgtFilePath, "%s/%s.fft.amp", tgtWorkspaceDir, varName);
		ZC_writeStrings(FFT_SIZE+1, ss, tgtFilePath);

		for(i=0;i<=FFT_SIZE;i++)
			free(ss[i]);
	}
}

void ZC_writeDataProperty(ZC_DataProperty* property, char* tgtWorkspaceDir)
{
	char** s = constructDataPropertyString(property);
	
	DIR *dir = opendir(tgtWorkspaceDir);
	if(dir==NULL)
		mkdir(tgtWorkspaceDir,0775);

	char tgtFilePath[ZC_BUFS];
	sprintf(tgtFilePath, "%s/%s.prop", tgtWorkspaceDir, property->varName); 
	ZC_writeStrings(15, s, tgtFilePath);
	
	int i;
	for(i=0;i<15;i++)
		free(s[i]);
	free(s);

	/*write the fft coefficients and amplitudes*/
	ZC_writeFFTResults(property->varName, property->fftCoeff, tgtWorkspaceDir);

	/*write auto-correlation coefficients*/
	if(property->autocorr!=NULL)
	{
		char *autocorr[AUTOCORR_SIZE+2];
		autocorr[0] = (char*)malloc(sizeof(char)*ZC_BUFS);
		sprintf(autocorr[0], "- \"\"\n");
		for (i = 1; i < AUTOCORR_SIZE+2; i++)
		{
			autocorr[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
			sprintf(autocorr[i], "%d %.10G\n", i-1, (property->autocorr)[i-1]);
		}
		memset(tgtFilePath, 0, ZC_BUFS);
		sprintf(tgtFilePath, "%s/%s.autocorr", tgtWorkspaceDir, property->varName);
		ZC_writeStrings(AUTOCORR_SIZE, autocorr, tgtFilePath);
		for (i = 0; i < AUTOCORR_SIZE+2; i++)
			free(autocorr[i]);
	}

	/*write Laplacian*/
	if(property->lap!=NULL)
	{
		char **lap = (char**)malloc(property->numOfElem*sizeof(char*));

		for (i = 0; i < property->numOfElem; i++)
		{
			lap[i] = (char*)malloc(sizeof(char)*50);
			sprintf(lap[i], "%f\n", (property->lap)[i]);
		}
		memset(tgtFilePath, 0, ZC_BUFS);
		sprintf(tgtFilePath, "%s/%s.lap", tgtWorkspaceDir, property->varName);
		ZC_writeStrings(property->numOfElem, lap, tgtFilePath);
		for (i = 0; i < property->numOfElem; i++)
			free(lap[i]);
		free(lap);
	}
	if(dir!=NULL)
		closedir(dir);
}

ZC_DataProperty* ZC_loadDataProperty(char* propResultFile)
{
	/*TODO put the information into the hashtable, named ecPropertyTable*/
	dictionary *ini;
	char* par;
    /*printf("[ZC] Reading propResultFile (%s) ...\n", propResultFile);*/
    if (access(propResultFile, F_OK) != 0)
    {
        printf("[ZC] propResultFile: %s NOT accessible.\n", propResultFile);
        exit(1);
    }
    ini = iniparser_load(propResultFile);
    if (ini == NULL)
    {
        printf("[ZC] Iniparser failed to parse the conf. file: %s.\n", propResultFile);
        exit(1);
    }
	
	par = iniparser_getstring(ini, "PROPERTY:varName", NULL);
	char* var = (char*)malloc(ZC_BUFS);
	snprintf(var, ZC_BUFS,"%s", par);
	
	int dataType = (int)iniparser_getint(ini, "PROPERTY:dataType", 0);
	int r5 = (int)iniparser_getint(ini, "PROPERTY:r5", 0);
	int r4 = (int)iniparser_getint(ini, "PROPERTY:r4", 0);
	int r3 = (int)iniparser_getint(ini, "PROPERTY:r3", 0);
	int r2 = (int)iniparser_getint(ini, "PROPERTY:r2", 0);
	int r1 = (int)iniparser_getint(ini, "PROPERTY:r1", 0);
	int numOfElem = (int)iniparser_getint(ini, "PROPERTY:numOfElem", 0);
	double minValue = (double)iniparser_getdouble(ini, "PROPERTY:minValue", 0);
	double maxValue = (double)iniparser_getdouble(ini, "PROPERTY:maxValue", 0);
	double valueRange = (double)iniparser_getdouble(ini, "PROPERTY:valueRange", 0);
	double avgValue = (double)iniparser_getdouble(ini, "PROPERTY:avgValue", 0);
	double entropy = (double)iniparser_getdouble(ini, "PROPERTY:entropy", 0);
	/*double autocorr = (double)iniparser_getdouble(ini, "PROPERTY:autocorr", 0); //1-step*/
	/*TODO: Read more autocorr. coefficients (Read file) and put them in autocorr_array*/
	double* autocorr_array = NULL;
	
	/*TODO: Read fft coefficients... (Read file)*/
	complex* fftCoeff = NULL;
	
	ZC_DataProperty* property = ZC_constructDataProperty(var, dataType, r5, r4, r3, r2, r1, numOfElem, minValue, maxValue, 
	valueRange, avgValue, entropy, autocorr_array, fftCoeff);
	return property;
}
