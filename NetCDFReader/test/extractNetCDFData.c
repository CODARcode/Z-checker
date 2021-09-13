#include <stdio.h>
#include <stdlib.h>
#include "NetCDFReader.h"

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

void writeByteData(unsigned char *bytes, size_t byteLength, const char *tgtFilePath)
{
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
		exit(0);
    }
    
    fwrite(bytes, 1, byteLength, pFile); //write outSize bytes
    fclose(pFile);
}


void writeFloatData_inBytes(float *data, size_t nbEle, const char* tgtFilePath)
{
	size_t i = 0;
	eclfloat buf;
	unsigned char* bytes = (unsigned char*)malloc(nbEle*sizeof(float));
	for(i=0;i<nbEle;i++)
	{
		buf.value = data[i];
		bytes[i*4+0] = buf.byte[0];
		bytes[i*4+1] = buf.byte[1];
		bytes[i*4+2] = buf.byte[2];
		bytes[i*4+3] = buf.byte[3];					
	}

	size_t byteLength = nbEle*sizeof(float);
	writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void writeDoubleData_inBytes(double *data, size_t nbEle, const char* tgtFilePath)
{
	size_t i = 0, index = 0;
	ecldouble buf;
	unsigned char* bytes = (unsigned char*)malloc(nbEle*sizeof(double));
	for(i=0;i<nbEle;i++)
	{
		index = i*8;
		buf.value = data[i];
		bytes[index+0] = buf.byte[0];
		bytes[index+1] = buf.byte[1];
		bytes[index+2] = buf.byte[2];
		bytes[index+3] = buf.byte[3];
		bytes[index+4] = buf.byte[4];
		bytes[index+5] = buf.byte[5];
		bytes[index+6] = buf.byte[6];
		bytes[index+7] = buf.byte[7];
	}

	size_t byteLength = nbEle*sizeof(double);
	writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void usage()
{
	printf("Options:\n");
	printf("* data type:\n");
	printf("	-f: single precision (float type)\n");
	printf("	-d: double precision (double type)\n");
	printf("* input data file:\n");
	printf("	-i <NetCDF data file> : NetCDF data file\n");
	printf("	-n <dataset path> : dataset path in NetCDF file\n");
	printf("* output data file:\n");
	printf("	-o <output file path>: output file path\n");
	printf("* dimensions: \n");
	printf("	-1 <nx> : dimension for 1D data such as data[nx]\n");
	printf("	-2 <nx> <ny> : dimensions for 2D data such as data[ny][nx]\n");
	printf("	-3 <nx> <ny> <nz> : dimensions for 3D data such as data[nz][ny][nx] \n");
	printf("	-4 <nx> <ny> <nz> <np>: dimensions for 4D data such as data[np][nz][ny][nx] \n");
	printf("* examples: \n");
	printf("	extractNetCDFData -f -i testdata/testdata.nc -n data -2 6 12 -o test.dat\n");
	exit(0);
}

int main(int argc, char* argv[])
{
	int dataType;
	char* inPath = NULL;
	char *dataset = NULL;
	char* outPath = NULL;

	size_t i = 0;
	size_t nbEle;

	size_t r5 = 0;
	size_t r4 = 0;
	size_t r3 = 0;
	size_t r2 = 0; 
	size_t r1 = 0;

	if(argc==1)
	  usage();

	for(i=1;i<argc;i++)
	{
		if (argv[i][0] != '-' || argv[i][2])
		  usage();
		switch (argv[i][1])
		{
			case 'f': 
				dataType = 0;
				break;
			case 'd':
				dataType = 1;
				break;
			case 'i':
				if (++i == argc)
				  usage();
				inPath = argv[i];		
				break;
			case '1': 
				if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1)
				  usage();
				break;
			case '2':
				if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1 ||
							++i == argc || sscanf(argv[i], "%zu", &r2) != 1)
				  usage();
				break;
			case '3':
				if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1 ||
							++i == argc || sscanf(argv[i], "%zu", &r2) != 1 ||
							++i == argc || sscanf(argv[i], "%zu", &r3) != 1)
				  usage();		
				break;
			case '4':
				if (++i == argc || sscanf(argv[i], "%zu", &r1) != 1 ||
							++i == argc || sscanf(argv[i], "%zu", &r2) != 1 ||
							++i == argc || sscanf(argv[i], "%zu", &r3) != 1 ||
							++i == argc || sscanf(argv[i], "%zu", &r4) != 1)
				  usage();		
				break;
			case 'n':
				if (++i == argc)
				  usage();
				dataset = argv[i];
				break;
			case 'o':
				if (++i == argc)
				  usage();
				outPath = argv[i];
				break;
			default: 
				usage();
				break;
		}
	}

	if ((r1==0) && (r2==0) && (r3==0) && (r4==0) && (r5==0))
	{
		printf ("Error: please specify dimensions.\n");
		printf("-1 <nx> : dimension for 1D data such as data[nx]\n");
		printf("-2 <nx> <ny> : dimensions for 2D data such as data[ny][nx]\n");
		printf("-3 <nx> <ny> <nz> : dimensions for 3D data such as data[nz][ny][nx] \n");
		printf("-4 <nx> <ny> <nz> <np>: dimensions for 4D data such as data[np][nz][ny][nx] \n");
		exit(0);
	}

	if(r2==0)
	  nbEle = r1;
	else if(r3==0)
	  nbEle = r1*r2;
	else if(r4==0)
	  nbEle = r1*r2*r3;
	else if(r5==0)
	  nbEle = r1*r2*r3*r4;
	else
	  nbEle = r1*r2*r3*r4*r5;

	if (dataType == 0)
	{
		float *data = (float*)malloc(nbEle*sizeof(float));
		netcdfReader((void*)data, inPath, dataset, dataType);

		for (i = 0; i < 10; i++)
		  printf ("%f\n", ((float*)data)[i]);
		  
		 writeFloatData_inBytes(data, nbEle, outPath); 
	}
	else
	if (dataType == 1)
	{
	  double *data = (double*)malloc(nbEle*sizeof(double));
	  netcdfReader((void*)data, inPath, dataset, dataType);

	  for (i = 0; i < 10; i++)
		printf ("%lf\n", ((double*)data)[i]);
		
	  writeDoubleData_inBytes(data, nbEle, outPath); 
	}
	printf("output data file: %s\n", outPath);

	return 0;
}

