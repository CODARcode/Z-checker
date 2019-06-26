#include <stdio.h>
#include <string.h>
#include <mpi.h> 
#include "ZC_rw.h"
#include "zc.h"

#define WORKTAG 50

int main(int argc, char * argv[])
{	
	size_t r5=0,r4=0,r3=0,r2=0,r1=0;
	char oriFilePath[640], decFilePath[640];
	char *cfgFile, *compressionCase, *varName;
	
	if(argc < 8)
	{
		printf("Usage: compareDataSets [dataType -f or -d] [config_file] [compressionCase] [varName] [oriDataFilePath] [decDataFilePath] [dimension sizes...]\n");
		printf("Example: compareDataSets -f zc.config SZ 8_8_128 testfloat_8_8_128.dat testfloat_8_8_128.dat.out 8 8 128\n");
		exit(0);
	}

	cfgFile=argv[2];
	compressionCase=argv[3];
	varName=argv[4];
	sprintf(oriFilePath, "%s", argv[5]);
	sprintf(decFilePath, "%s", argv[6]);
	if(argc>=8)
		r1 = atoi(argv[7]); //8
	if(argc>=9)
		r2 = atoi(argv[8]); //8
	if(argc>=10)
		r3 = atoi(argv[9]); //128
	if(argc>=11)
		r4 = atoi(argv[10]);
	if(argc>=12)
		r5 = atoi(argv[11]);

	printf("cfgFile=%s\n", cfgFile);
	printf("oriFilePath=%s\n", oriFilePath);
	printf("decFilePath=%s\n", decFilePath);
	
	MPI_Init(&argc,&argv);
		
	int numprocs, myrank;
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);/* get the rank */ 
	
	if(myrank==0)
		printf("[ZC] Reading ZC configuration file (%s) ...\n", cfgFile); 	
	ZC_Init(cfgFile);

	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	size_t offset = 0;
	switch(dim)
	{
	case 1:
		r1 = r1/numprocs;
		offset = r1*myrank;
		break;
	case 2:
		r2 = r2/numprocs;
		offset = r1*r2*myrank;
		break;
	case 3:
		r3 = r3/numprocs;
		offset = r1*r2*r3*myrank;
		break;
	case 4:
		r4 = r4/numprocs;	
		offset = r1*r2*r3*r4*myrank;
		break;
	case 5:
		r5 = r5/numprocs;
		offset = r1*r2*r3*r4*r5*myrank;	
		break;
	}
	
	long localLength = ZC_computeDataLength(r5, r4, r3, r2, r1);
	long globalLength = ZC_computeDataLength_online(r5,r4,r3,r2,r1);

	if(myrank==0)
	    printf("localLength = %ld, globalDataLength = %ld\n", localLength, globalLength);

	MPI_Status status;
	size_t nbEle1, nbEle2, i = 0;
	float *data1 = NULL, *data2 = NULL;
	MPI_Request req1[2], req2[2];
	if(myrank==0)
	{
		data1 = ZC_readFloatData(oriFilePath, &nbEle1);
		data2 = ZC_readFloatData(decFilePath, &nbEle2);

	if(nbEle1!=nbEle2)
	{
		printf("Error: nbEle1(%zu)!=nbEle2(%zu)\n",nbEle1, nbEle2);
		exit(0);
	}	
	for(i=1;i<numprocs;i++)
	{
		MPI_Isend(data1+offset, localLength, MPI_FLOAT, i, WORKTAG, MPI_COMM_WORLD, &req1[0]);
		MPI_Isend(data2+offset, localLength, MPI_FLOAT, i, WORKTAG, MPI_COMM_WORLD, &req1[1]);					
	}
	}
	else
	{
		data1 = (float*)malloc(sizeof(float)*localLength);
		data2 = (float*)malloc(sizeof(float)*localLength);
		MPI_Irecv(data1, localLength, MPI_FLOAT, 0, WORKTAG, MPI_COMM_WORLD, &req2[0]);
		MPI_Irecv(data2, localLength, MPI_FLOAT, 0, WORKTAG, MPI_COMM_WORLD, &req2[1]);			
	}

	if (myrank == 0)
	{
		MPI_Waitall(2,req1,&status);
	}
	else 
	{
		MPI_Waitall(2,req2,&status);
	}

	ZC_CompareData* compareResult;
	
	double startTime = MPI_Wtime();
	if (argv[1][1] == 'f')
	{
		compareResult = ZC_compareData(varName, ZC_FLOAT, data1, data2, r5, r4, r3, r2, r1);
	}
	else if (argv[1][1] == 'd')
	{
		compareResult = ZC_compareData(varName, ZC_DOUBLE, data1, data2, r5, r4, r3, r2, r1);
	}
	else
	{
		printf ("Wrong data type.\n");
		printf ("Please use -f or -d to specify single or double data type.\n");
		exit(0);
	}
	double endTime = MPI_Wtime();
	if(myRank==0)
		printf("execution time = %f\n", endTime - startTime);

	if(myrank==0)
	{
		ZC_printCompressionResult(compareResult);
		//ZC_writeCompressionResult(compareResult, compressionCase, varName, "compressionResults");		
	}

	freeDataProperty(compareResult->property);
	freeCompareResult(compareResult);	
	free(data1);
	free(data2);
	MPI_Finalize();		
	return 0;
}

