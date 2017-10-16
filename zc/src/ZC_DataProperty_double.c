#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include "ZC_DataProperty.h"
#include "zc.h"
#include "iniparser.h"

ZC_DataProperty* ZC_genProperties_double(char* varName, double *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i = 0;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));

	property->varName = varName;
	property->dataType = ZC_DOUBLE;
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
		size_t table_size;
		table_size = (size_t)(valueRange/absErr)+1;	
		size_t *table = (size_t*)malloc(table_size*sizeof(size_t));
		memset(table, 0, table_size*sizeof(size_t));
		
		unsigned long index = 0;
		for (i = 0; i < numOfElem; i++)
 		{
			index = (unsigned long)((data[i]-min)/absErr);
			table[index]++;
		}
		for (i = 0; i<table_size; i++)
			if (table[i] != 0)
			{
				double prob = (double)table[i]/numOfElem;
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
