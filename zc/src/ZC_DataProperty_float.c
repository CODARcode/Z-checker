#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include "ZC_DataProperty.h"
#include "zc.h"
#include "iniparser.h"
#include "ZC_FFTW3_math.h"

double* ZC_compute_autocorrelation1D_float(float* data, size_t numOfElem, double avg)
{
	double *autocorr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));

	size_t i = 0;
	int delta = 0;

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
					autocorr[delta] = 1;
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
	return autocorr;
}

#ifdef HAVE_MPI

void ZC_genBasicProperties_float_online(float* data, size_t numOfElem, ZC_DataProperty* property)
{
	size_t i;
	property->dataType = ZC_FLOAT;
	property->data = data;	
	
	//property->numOfElem = numOfElem;
	double min=data[0],max=data[0],sum=0;

    for(i=0;i<numOfElem;i++)
	{
		if(min>data[i]) min = data[i];
		if(max<data[i]) max = data[i];
		sum += data[i];
	}
	
	if(globalDataLength>0)
		property->numOfElem = globalDataLength;
	else
	{
		MPI_Allreduce(&numOfElem, &globalDataLength, 1, MPI_LONG, MPI_SUM, ZC_COMM_WORLD);
		property->numOfElem = globalDataLength;
	}
	MPI_Allreduce(&min, &property->minValue, 1, MPI_DOUBLE, MPI_MIN, ZC_COMM_WORLD);
	MPI_Allreduce(&max, &property->maxValue, 1, MPI_DOUBLE, MPI_MAX, ZC_COMM_WORLD);
	MPI_Allreduce(&sum, &property->avgValue, 1, MPI_DOUBLE, MPI_SUM, ZC_COMM_WORLD);
	property->avgValue = property->avgValue/globalDataLength;
	
	//compute zeromean_variance for the following computation of SNR
	double med = property->minValue + (property->maxValue - property->minValue)/2;
	double sum_of_square = 0;
	for(i=0;i<numOfElem;i++)
		sum_of_square += (data[i] - med)*(data[i] - med);
	MPI_Allreduce(&sum_of_square, &property->zeromean_variance, 1, MPI_DOUBLE, MPI_SUM, ZC_COMM_WORLD);
	property->zeromean_variance = property->zeromean_variance/globalDataLength;
	property->valueRange = property->maxValue - property->minValue;	
}


ZC_DataProperty* ZC_genProperties_float_online(const char* varName, float *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i = 0;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));
	
	property->varName = (char*)malloc(100);
	
	char* varN = rmFileExtension(varName);
	strcpy(property->varName, varName);
	free(varN);
	
	ZC_genBasicProperties_float_online(data, numOfElem, property);
	
	if(entropyFlag)
	{
		double entVal = 0.0;
		unsigned char index = 0;
		size_t totalLen = numOfElem*sizeof(float);
		size_t table_size = 256;
		long *table = (long*)malloc(table_size*sizeof(long));
		memset(table, 0, table_size*sizeof(long));
		long *gtable = (long*)malloc(table_size*sizeof(long));
		memset(gtable, 0, table_size*sizeof(long));
				
		unsigned char* bytes = (unsigned char*)data;	
		for(i=0;i<totalLen;i++)
		{
			index = bytes[i];
			table[index]++;
		}
		
		MPI_Reduce(table, gtable, table_size, MPI_LONG, MPI_SUM, 0, ZC_COMM_WORLD);
		
		if(myRank==0)
		{
			size_t sum = globalDataLength * sizeof(float);
			for (i = 0; i<table_size; i++)
				if (gtable[i] != 0)
				{
					double prob = (double)gtable[i]/sum;
					entVal -= prob*log(prob)/log(2);
				}
		}

		property->entropy = entVal;
		free(table);
		free(gtable);			
		
/*		double absErr = 1E-3 * property->valueRange; //TODO change fixed value to user input
		double entVal = 0.0;
		size_t table_size;
		table_size = (size_t)(property->valueRange/absErr)+1;	
		long *table = (long*)malloc(table_size*sizeof(long));
		memset(table, 0, table_size*sizeof(long));
		long *gtable = (long*)malloc(table_size*sizeof(long));
		memset(gtable, 0, table_size*sizeof(long));
		
		unsigned long index = 0;
		for (i = 0; i < numOfElem; i++)
 		{
			index = (unsigned long)((data[i]-property->minValue)/absErr);
			table[index]++;
		}
 
		MPI_Reduce(table, gtable, table_size, MPI_LONG, MPI_SUM, 0, ZC_COMM_WORLD);
 
		if(myRank==0)
		{
			for (i = 0; i<table_size; i++)
				if (gtable[i] != 0)
				{
					double prob = (double)gtable[i]/globalDataLength;
					entVal -= prob*log(prob)/log(2);
				}
		}

		property->entropy = entVal;
		free(table);
		free(gtable);*/
	}
	if(autocorrFlag)
	{
		double ccov[AUTOCORR_SIZE+1], gcov[AUTOCORR_SIZE+1];
		memset(ccov, 0, sizeof(double)*(AUTOCORR_SIZE+1));
		memset(gcov, 0, sizeof(double)*(AUTOCORR_SIZE+1));
		
		double *autocorr = (double*)malloc((AUTOCORR_SIZE+1)*sizeof(double));
		int delta;
		double var = 0, gvar = 0;
		for (i = 0; i < numOfElem; i++)
			var += (data[i] - property->avgValue)*(data[i] - property->avgValue);
		
		for(delta = 1; delta <= AUTOCORR_SIZE; delta++)
		{
			double cov = 0;
			for (i = 0; i < numOfElem-delta; i++)
				cov += (data[i] - property->avgValue)*(data[i+delta] - property->avgValue);
			ccov[delta] = cov;	
		}
		
		MPI_Reduce(&var, &gvar, 1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
		MPI_Reduce(ccov, gcov, AUTOCORR_SIZE+1, MPI_DOUBLE, MPI_SUM, 0, ZC_COMM_WORLD);
		
		if(myRank==0)
		{	
			gvar = gvar/globalDataLength;
			if (gvar == 0)
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
					autocorr[delta] = 1;
			}
			else
			{
				for (delta = 1; delta <= AUTOCORR_SIZE; delta++)
				{
					autocorr[delta] = gcov[delta]/(globalDataLength-nbProc*delta)/gvar;				
				}
			}
			autocorr[0] = 1;
			property->autocorr = autocorr;					
		}
	}
	//TODO compute FFT 
	
	
	//TODO compute Lap
	
	return property;
}
#endif

double computeLosslessEntropy_32bits(void* data, size_t nbEle)
{
	size_t i = 0;
	char vs[256];	
	
	hashtable_t* entropy_table = ht_create(nbEle);
	float* value = (float*)data;
	for(i=0;i<nbEle;i++)
	{
        eclfloat buf;
        buf.value = value[i];
        unsigned int v = buf.ivalue;
		
		sprintf(vs, "%d", v);	
		ZC_ELEMENT* qe = ht_get(entropy_table, vs);
		if(qe==NULL)
		{
			qe = (ZC_ELEMENT*) malloc(sizeof(ZC_ELEMENT));
			memset(qe, 0, sizeof(ZC_ELEMENT));
			qe->value = value[i];
			ht_set(entropy_table, vs, qe);
		}
		qe->counter ++;
	}		
	
	size_t sum = nbEle;
	size_t j = 0;
	double entVal = 0;
	for(i=0;i<entropy_table->capacity&&j<entropy_table->count;i++)
	{
		entry_t* t = entropy_table->table[i];
		while(t!=NULL)
		{
			ZC_ELEMENT* qe = (ZC_ELEMENT*)t->value;
			double prob = ((double)qe->counter)/sum;
			entVal -= prob*log(prob)/log(2);
			free(qe);
			t = t->next;
		}
	}	
	
	ht_freeTable(entropy_table);
	return entVal;
}

ZC_DataProperty* ZC_genProperties_float(const char* varName, float *data, size_t numOfElem, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i = 0;
	ZC_DataProperty* property = (ZC_DataProperty*)malloc(sizeof(ZC_DataProperty));
	memset(property, 0, sizeof(ZC_DataProperty));
	
	property->varName = (char*)malloc(strlen(varName)+1);
	
	char* varN = rmFileExtension(varName);
	strcpy(property->varName, varN);
	free(varN);
	
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
		double entVal = 0.0;
		unsigned char index = 0;
		size_t totalLen = numOfElem*sizeof(float);
		size_t table_size = 256;
		long *table = (long*)malloc(table_size*sizeof(long));
		memset(table, 0, table_size*sizeof(long));
				
		unsigned char* bytes = (unsigned char*)data;	
		for(i=0;i<totalLen;i++)
		{
			index = bytes[i];
			table[index]++;
		}
		
		size_t sum = numOfElem*sizeof(float);
		for (i = 0; i<table_size; i++)
			if (table[i] != 0)
			{
				double prob = (double)table[i]/sum;
				entVal -= prob*log(prob)/log(2);
			}

		property->entropy = entVal;
		free(table);
	}
	
	if(entropyFloatpointFlag)
	{
		property->entropyFloatpoint = computeLosslessEntropy_32bits(data, numOfElem);
	}

	if(autocorrFlag)
	{
		property->autocorr = ZC_compute_autocorrelation1D_float(data, numOfElem, avg);
	}
	
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);		
#ifdef HAVE_FFTW3	
	if(autocorr3DFlag)
	{		
		switch(dim)
		{
		case 1:
			property->autocorr3D = autocorr_3d_float(data, r1, 1, 1);
			break;
		case 2:
			property->autocorr3D = autocorr_3d_float(data, r1, r2, 1);
			break;
		case 3:
			property->autocorr3D = autocorr_3d_float(data, r1, r2, r3);
			break;
		case 4:
			property->autocorr3D = autocorr_3d_float(data, r1, r2, r3*r4);
			break;
		case 5:
			property->autocorr3D = autocorr_3d_float(data, r1, r2, r3*r4*r5);
			break;
		default: 
			printf("Error: wrong dimension (dim=%d)\n", dim);
			exit(0);
		}
	}
#endif	
	
	if(fftFlag)
	{
        size_t fft_size = pow(2, (int)log2(numOfElem));
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
	
	if(gradLenFlag)
	{
		float* gradLen_data = (float *)malloc(numOfElem*sizeof(float));
		memset(gradLen_data, 0, numOfElem*sizeof(float));		
		computeGradientLength_float(data, gradLen_data, r5, r4, r3, r2, r1);
		property->gradLen = gradLen_data;
	}
	
	if(sobolevNorm_s0_p2Flag)
		property->sobolevNorm_s0_p2 = calculateSobolevNorm_p2(data, ZC_FLOAT, 0, r5, r4, r3, r2, r1);	
		
	if(sobolevNorm_s1_p2Flag)
		property->sobolevNorm_s1_p2 = calculateSobolevNorm_p2(data, ZC_FLOAT, 1, r5, r4, r3, r2, r1);		
	
	if(sobolevNorm_s2_p2Flag)
		property->sobolevNorm_s2_p2 = calculateSobolevNorm_p2(data, ZC_FLOAT, 2, r5, r4, r3, r2, r1);		
	
	if(plotImageFlag)
	{
		float* data = (float*)property->data;
		float* sliceImage_ori = NULL;
		float* sliceImage_log = NULL;
		if(dim==1)
		{
			sliceImage_ori = data;
			sliceImage_log = (float*)malloc(sizeof(float)*property->numOfElem);
			for(i=0;i<property->numOfElem && i<1000;i++)
				sliceImage_log[i] = log10f(fabsf(sliceImage_ori[i]));			
		}
		else if(dim==2)
		{
			sliceImage_ori = data;
			sliceImage_log = (float*)malloc(sizeof(float)*property->numOfElem);
			for(i=0;i<property->numOfElem;i++)
				sliceImage_log[i] = log10f(fabsf(sliceImage_ori[i]));
		}
		else if(dim==3)
		{
			size_t sliceID = r3/2;
			size_t offset = sliceID*r1*r2;
			size_t nbSliceEle = r1*r2;
			sliceImage_ori = (float*)malloc(sizeof(float)*nbSliceEle);
			sliceImage_log = (float*)malloc(sizeof(float)*nbSliceEle);

			for(i=0;i<nbSliceEle;i++)
			{
				sliceImage_ori[i] = data[offset+i];
				sliceImage_log[i] = log10f(fabsf(sliceImage_ori[i]));
			}
		}
		property->sliceImage_ori = sliceImage_ori;
		property->sliceImage_log = sliceImage_log;
	}

	return property;
}


void computeGradientLength_float(float* data, float*gradMag, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i, j, k, index;
	double gradx, grady, gradz;
	
	if(r2==0)
	{
		gradMag[0] = data[1] - data[0];
		gradMag[r1-1] = data[r1-1]-data[r1-2];
		for(i=1;i<r1-1;i++)
			gradMag[i] = (data[i+1]-data[i-1])/2;
	}
	else if(r3==0)
	{
		//process four corners
		gradx = data[1]-data[0];
		grady = data[r1]-data[0];
		gradMag[0] = sqrt(gradx*gradx+grady*grady);
		index = r1-1;
		gradx = data[index]-data[index-1];
		grady = data[index+r1]-data[index];
		gradMag[index] = sqrt(gradx*gradx+grady*grady);		
		index = (r2-1)*r1;
		gradx = data[index+1]-data[index];
		grady = data[index]-data[index-r1];
		gradMag[index] = sqrt(gradx*gradx+grady*grady);	
		index = (r2-1)*r1 + r1 - 1;			
		gradx = data[index]-data[index-1];
		grady = data[index]-data[index-r1];
		gradMag[index] = sqrt(gradx*gradx+grady*grady);								
		
		//process four edges
		for(i=1;i<r1-1;i++)
		{
			index = i;
			gradx = (data[index+1]-data[index-1])/2;
			grady = data[index+r1]-data[index];
			gradMag[index] = sqrt(gradx*gradx+grady*grady);			
		}
		for(i=1;i<r1-1;i++)
		{
			index = (r2-1)*r1 + i;
			gradx = (data[index+1]-data[index-1])/2;
			grady = data[index]-data[index-r1];
			gradMag[index] = sqrt(gradx*gradx+grady*grady);			
		}
		
		for(i=1;i<r2-1;i++)
		{
			index = i*r1;
			gradx = (data[index+1] - data[index]);
			grady = (data[index+r1]-data[index-r1])/2;
			gradMag[index] = sqrt(gradx*gradx+grady*grady);						
		}
		for(i=1;i<r2-1;i++)
		{
			index = i*r1+r1-1;
			gradx = (data[index] - data[index-1]);
			grady = (data[index+r1]-data[index-r1])/2;
			gradMag[index] = sqrt(gradx*gradx+grady*grady);						
		}		
		
		//process all interior points
		for(i=1;i<r2-1;i++)
			for(j=1;j<r1-1;j++)
			{
				index = i*r1+j;
				gradx = (data[index+1] - data[index-1])/2;
				grady = (data[index+r1] - data[index-r1])/2;
				gradMag[index] = sqrt(gradx*gradx+grady*grady);
			}
		
	}else if(r4==0) //3D
	{
		size_t r2r1 = r2*r1;
		//process all 8 corners
		gradx = data[1]-data[0];
		grady = data[r1]-data[0];
		gradz = data[r2r1]-data[0];		
		gradMag[0] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
		
		index = r1-1;
		gradx = data[index]-data[index-1];
		grady = data[index+r1]-data[index];		
		gradz = data[index+r2r1]-data[index];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
		
		index = (r2-1)*r1;
		gradx = data[index+1]-data[index];
		grady = data[index]-data[index-r1];		
		gradz = data[index+r2r1]-data[index];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);

		index = (r2-1)*r1 + r1 - 1;
		gradx = data[index]-data[index-1];
		grady = data[index]-data[index-r1];		
		gradz = data[index+r2r1]-data[index];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
		
		index = (r3-1)*r2r1;
		gradx = data[index+1]-data[index];
		grady = data[index+r1]-data[index];
		gradz = data[index]-data[index-r2r1];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
		
		index = (r3-1)*r2r1+r1-1;
		gradx = data[index]-data[index-1];
		grady = data[index+r1]-data[index];		
		gradz = data[index]-data[index-r2r1];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
		
		index = (r3-1)*r2r1 + (r2-1)*r1;
		gradx = data[index+1]-data[index];
		grady = data[index]-data[index-r1];		
		gradz = data[index]-data[index-r2r1];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);

		index = (r3-1)*r2r1 + (r2-1)*r1 + r1 - 1;
		gradx = data[index]-data[index-1];
		grady = data[index]-data[index-r1];		
		gradz = data[index]-data[index-r2r1];		
		gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);		
				
		//process all 8 edges
		for(i=1;i<r1-1;i++)
		{
			index = i;
			gradx = (data[index+1]-data[index-1])/2;
			grady = data[index+r1]-data[index];
			gradz = data[index+r2r1] - data[index];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);			
		}

		for(i=1;i<r1-1;i++)
		{
			index = (r2-1)*r1 + i;
			gradx = (data[index+1]-data[index-1])/2;
			grady = data[index]-data[index-r1];
			gradz = data[index+r2r1] - data[index];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);			
		}
		
		for(i=1;i<r2-1;i++)
		{
			index = i*r1;
			gradx = (data[index+1] - data[index]);
			grady = (data[index+r1]-data[index-r1])/2;
			gradz = data[index+r2r1] - data[index];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);						
		}
		for(i=1;i<r2-1;i++)
		{
			index = i*r1+r1-1;
			gradx = (data[index] - data[index-1]);
			grady = (data[index+r1]-data[index-r1])/2;
			gradz = data[index+r2r1] - data[index];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);						
		}
		
		for(i=1;i<r1-1;i++)
		{
			index = (r3-1)*r2r1 + i;
			gradx = (data[index+1]-data[index-1])/2;
			grady = data[index+r1]-data[index];
			gradz = data[index] - data[index-r2r1];
			gradMag[i] = sqrt(gradx*gradx+grady*grady+gradz*gradz);			
		}

		for(i=1;i<r1-1;i++)
		{
			index = (r3-1)*r2r1 + (r2-1)*r1 + i;
			gradx = (data[index+1]-data[index-1])/2;
			grady = data[index]-data[index-r1];
			gradz = data[index] - data[index-r2r1];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);			
		}
		
		for(i=1;i<r2-1;i++)
		{
			index = (r3-1)*r2r1+i*r1;
			gradx = (data[index+1] - data[index]);
			grady = (data[index+r1]-data[index-r1])/2;
			gradz = data[index] - data[index-r2r1];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);						
		}
		
		for(i=1;i<r2-1;i++)
		{
			index = (r3-1)*r2r1+i*r1+r1-1;
			gradx = (data[index] - data[index-1]);
			grady = (data[index+r1]-data[index-r1])/2;
			gradz = data[index] - data[index-r2r1];
			gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);						
		}
		
		//process all 6 sides
		for(i=1;i<r2-1;i++)
			for(j=1;j<r1-1;j++)
			{
				index = i*r1+j;
				gradx = (data[index+1] - data[index-1])/2;
				grady = (data[index+r1] - data[index-r1])/2;
				gradz = data[index+r2r1] - data[index];
				gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
			}		

		for(i=1;i<r2-1;i++)
			for(j=1;j<r1-1;j++)
			{
				index = (r3-1)*r2r1 + i*r1 + j;
				gradx = (data[index+1] - data[index-1])/2;
				grady = (data[index+r1] - data[index-r1])/2;
				gradz = data[index] - data[index-r2r1];
				gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
			}				
		
		for(i=1;i<r3-1;i++)
			for(k=1;k<r1-1;k++)
			{
				index = i*r2r1 + k; //j is always 0
				gradx = (data[index+1] - data[index-1])/2;
				grady = data[index+r1] - data[index];
				gradz = (data[index+r2r1] - data[index-r2r1])/2;
				gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
			}
			
		for(i=1;i<r3-1;i++)
			for(k=1;k<r1-1;k++)
			{
				j = r2-1;
				index = i*r2r1 + j*r1 + k;
				gradx = (data[index+1] - data[index-1])/2;
				grady = data[index] - data[index-r1];
				gradz = (data[index+r2r1] - data[index-r2r1])/2;
				gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
			}

		for(i=1;i<r3-1;i++)
			for(j=1;j<r2-1;j++)
			{
				index = i*r2r1 + j*r1;
				gradx = data[index+1] - data[index];
				grady = (data[index+r1] - data[index-r1])/2;
				gradz = (data[index+r2r1] - data[index-r2r1])/2;
				gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
			}

		for(i=1;i<r3-1;i++)
			for(j=1;j<r2-1;j++)
			{
				k = r1-1;
				index = i*r2r1 + j*r1 + k; 
				gradx = data[index] - data[index-1];
				grady = (data[index+r1] - data[index-r1])/2;
				gradz = (data[index+r2r1] - data[index-r2r1])/2;
				gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
			}
		
		//process interior points
		for(i=1;i<r3-1;i++)
			for(j=1;j<r2-1;j++)
				for(k=1;k<r1-1;k++)
				{
					size_t index = i*r2r1+j*r1+k;
					gradx = (data[index+1] - data[index-1])/2;
					grady = (data[index+r1] - data[index-r1])/2;
					gradz = (data[index+r2r1] - data[index-r2r1])/2;
					gradMag[index] = sqrt(gradx*gradx+grady*grady+gradz*gradz);
				}		
	}
	
}

//the sum of square / nbEle
double calculateSobolevNorm_s0_p2_float(float *data, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	double result = 0;
	size_t i;
	size_t nbEle = ZC_computeDataLength(r5, r4, r3, r2, r1);
	double sum = 0;
	for(i=0;i<nbEle;i++)
		sum += data[i]*data[i];
	result = sqrt(sum/nbEle);
	return result;
}

//sqrt((||f(0)||^2+||f(1)||^2)/nbEle), where f(i) means i-th derivative
double calculateSobolevNorm_s1_p2_float(float *data, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i, j, k;
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	//size_t nbEle = computeDataLength(r5, r4, r3, r2, r1);
	double sum = 0;
	size_t counter =  0;
	if(dim==1)
	{
		for(i=1;i<r1-1;i++)
		{
			sum += data[i]*data[i];
			sum += (data[i+1]-data[i-1])*(data[i+1]-data[i-1])/4;
			counter ++;
		}
		return sqrt(sum/counter);
	}
	else if(dim==2)
	{
		for(i=1;i<r2-1;i++)
		{	
			for(j=1;j<r1-1;j++)
			{
				size_t index = i*r1+j;
				sum += data[index]*data[index];
				sum += (data[index+1]-data[index-1])*(data[index+1]-data[index-1])/4;
				sum += (data[index+r1]-data[index-r1])*(data[index+r1]-data[index-r1])/4;
				counter ++;
			}
		}				
		
		return sqrt(sum/counter);
	}
	else if(dim==3)
	{
		size_t r2r1 = r2*r1;
		for(i=1;i<r3-1;i++)
		{	
			for(j=1;j<r2-1;j++)
			{
				for(k=1;k<r1-1;k++)
				{
					size_t index = i*r2r1+j*r1+k;
					sum += data[index]*data[index];
					sum += (data[index+1]-data[index-1])*(data[index+1]-data[index-1])/4;
					sum += (data[index+r1]-data[index-r1])*(data[index+r1]-data[index-r1])/4;
					sum += (data[index+r2r1]-data[index-r2r1])*(data[index+r2r1]-data[index-r2r1])/4;		
					counter ++;			
				}
			}
		}			
		
		return sqrt(sum/counter);
	}
	
	return -1; //error or does not support dim>3
}

 //sqrt((||f(0)||^2+||f(1)||^2+||f(2)||^2)/nbEle), where f(i) means i-th derivative (including mixed partial if possible)
double calculateSobolevNorm_s2_p2_float(float *data, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	size_t i, j, k;
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	//size_t nbEle = computeDataLength(r5, r4, r3, r2, r1);
	size_t counter = 0;
	double sum = 0;
	if(dim==1)
	{
		for(i=1;i<r1-1;i++)
		{
			float x1_dev = (data[i+1]-data[i-1])/2; //x1_dev1 means along x, 1st-order partial
			float x2_dev = (data[i+1]-data[i])-(data[i]-data[i-1]); //second order partial
		
			sum += data[i]*data[i];
			sum += x1_dev*x1_dev;
			sum += x2_dev*x2_dev;
			counter ++;
		}
		
		return sqrt(sum/counter);
	}	
	else if(dim==2)
	{
		for(i=1;i<r2-1;i++)
			for(j=1;j<r1-1;j++)
			{
				size_t index = i*r1+j;
				float x1_dev = (data[index+1]-data[index-1])/2;
				float x2_dev = (data[index+1]-data[index])-(data[index]-data[index-1]);
				float y1_dev = (data[index+r1]-data[index-r1])/2;
				float y2_dev = (data[index+r1]-data[index])-(data[index]-data[index-r1]);
				float xy_dev = (data[index-r1-1]+data[index+r1+1]-data[index-r1+1]-data[index+r1-1])/4;
				
				sum += data[index]*data[index];
				sum += x1_dev*x1_dev;
				sum += x2_dev*x2_dev;
				sum += y1_dev*y1_dev;
				sum += y2_dev*y2_dev;
				sum += xy_dev*xy_dev;
				counter ++;
			}
			
		return sqrt(sum/counter);
	}
	else if(dim==3)
	{
		size_t r2r1 = r2*r1;
		for(i=1;i<r3-1;i++)
			for(j=1;j<r2-1;j++)
				for(k=1;k<r1-1;k++)
				{
					size_t index = i*r2r1+j*r1+k;
					float x1_dev = (data[index+1]-data[index-1])/2;
					float x2_dev = (data[index+1]-data[index])-(data[index]-data[index-1]);
					float y1_dev = (data[index+r1]-data[index-r1])/2;
					float y2_dev = (data[index+r1]-data[index])-(data[index]-data[index-r1]);
					float z1_dev = (data[index+r2r1]-data[index-r2r1])/2;
					float z2_dev = (data[index+r2r1]-data[index])-(data[index]-data[index-r2r1]);										
					float xy_dev = (data[index-r1-1]+data[index+r1+1] - data[index-r1+1] - data[index+r1-1])/4;
					float yz_dev = (data[index-r2r1-r1]+data[index+r2r1+r1] - data[index-r2r1+r1] - data[index+r2r1-r1])/4;
					float xz_dev = (data[index-r2r1-1]+data[index+r2r1+1]-data[index-r2r1+1]-data[index+r2r1-1])/4;
					sum += data[index]*data[index];
					sum += x1_dev*x1_dev;
					sum += x2_dev*x2_dev;
					sum += y1_dev*y1_dev;
					sum += y2_dev*y2_dev;
					sum += z1_dev*z1_dev;
					sum += z2_dev*z2_dev;
					sum += xy_dev*xy_dev;
					sum += yz_dev*yz_dev;
					sum += xz_dev*xz_dev;
					counter ++;
				}
				
		return sqrt(sum/counter);
	}
	
	return -1;
}
