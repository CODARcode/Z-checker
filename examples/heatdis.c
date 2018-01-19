/**
 *  @file   heatdis.c
 *  @author Sheng Di
 *  @date   Jan, 2018
 *  @brief  Heat distribution code to test online Z-checker.
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>
#include "sz.h"
#include "zc.h"

#define PRECISION   0.0001
#define ITER_TIMES  1300
#define ITER_OUT    500
#define WORKTAG     50
#define REDUCE      5


//initializing the simulation data
void initData(int nbLines, int M, int rank, double *h)
{
	int i, j;
	for (i = 0; i < nbLines; i++) {
		for (j = 0; j < M; j++) {
			h[(i*M)+j] = 0;
		}
	}
	if (rank == 0) {
		for (j = (M*0.1); j < (M*0.9); j++) {
			h[j] = 100;
		}
	}
}

//performing the simulation
double doWork(int numprocs, int rank, int M, int nbLines, double *g, double *h)
{
	int i,j;
	MPI_Request req1[2], req2[2];
	MPI_Status status1[2], status2[2];
	double localerror;
	localerror = 0;
	for(i = 0; i < nbLines; i++) {
		for(j = 0; j < M; j++) {
			h[(i*M)+j] = g[(i*M)+j];
		}
	}
	if (rank > 0) {
		MPI_Isend(g+M, M, MPI_DOUBLE, rank-1, WORKTAG, MPI_COMM_WORLD, &req1[0]);
		MPI_Irecv(h,   M, MPI_DOUBLE, rank-1, WORKTAG, MPI_COMM_WORLD, &req1[1]);
	}
	if (rank < numprocs-1) {
		MPI_Isend(g+((nbLines-2)*M), M, MPI_DOUBLE, rank+1, WORKTAG, MPI_COMM_WORLD, &req2[0]);
		MPI_Irecv(h+((nbLines-1)*M), M, MPI_DOUBLE, rank+1, WORKTAG, MPI_COMM_WORLD, &req2[1]);
	}
	if (rank > 0) {
		MPI_Waitall(2,req1,status1);
	}
	if (rank < numprocs-1) {
		MPI_Waitall(2,req2,status2);
	}
	for(i = 1; i < (nbLines-1); i++) {
		for(j = 0; j < M; j++) {
			g[(i*M)+j] = 0.25*(h[((i-1)*M)+j]+h[((i+1)*M)+j]+h[(i*M)+j-1]+h[(i*M)+j+1]);
			if(localerror < fabs(g[(i*M)+j] - h[(i*M)+j])) {
				localerror = fabs(g[(i*M)+j] - h[(i*M)+j]);
			}
		}
	}
	if (rank == (numprocs-1)) {
		for(j = 0; j < M; j++) {
			g[((nbLines-1)*M)+j] = g[((nbLines-2)*M)+j];
		}
	}
	return localerror;
}


int main(int argc, char *argv[])
{
	int rank, nbProcs, nbLines, i, M;
	double wtime, *h, *g, localerror, globalerror = 1;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nbProcs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		
	if(argc<6)
	{
		if(rank==0)
		{
			printf("Usage: mpirun -np [# ranks] heatdis [problem size] [sz_cfg_file] [zc_cfg_file] [var_name] [compressor_name]\n");
			printf("Example: mpirun -np 8 ./heatdis 100 sz.config zc.config temperature sz\n");			
		}

		MPI_Finalize();
		exit(0);
	}
	
	M = atoi(argv[1]);
	char* szCfgFile = argv[2]; //sz configuration file
	char* zcCfgFile = argv[3]; //zc configuration file
	char* varName = argv[4]; //the name of the variable (such as temperature)
	char* compressorName = argv[5]; //the name of the compressor, such as sz

    if(rank==0)
		printf("Reading SZ cfg file (%s) and ZC cfg file (%s) ...\n", szCfgFile, zcCfgFile);

	SZ_Init(szCfgFile); //initialization of sz
	ZC_Init(zcCfgFile); //initialization of zc
	
	nbLines = (M / nbProcs)+3;
	h = (double *) malloc(sizeof(double) * M * nbLines);
	g = (double *) malloc(sizeof(double) * M * nbLines);
	initData(nbLines, M, rank, g);
	
	if (rank == 0) {
		printf("Local data size is %d x %d.\n", M, nbLines);
		printf("Target precision : %f \n", PRECISION);
		printf("Maximum number of iterations : %d \n", ITER_TIMES);
	}

	unsigned char* cmprBytes = NULL; //used to store compressed data
	double* decData = NULL;
	size_t cmprSize = 0;
	char propName[128], cmprCaseName[128];
	memset(propName, 0, 128);
	memset(cmprCaseName, 0, 128);
	
	for (i = 0; i < ITER_TIMES; i++) {
		localerror = doWork(nbProcs, rank, M, nbLines, g, h);
		
		if(i%50==0) //control the compression frequency over time steps
		{
			sprintf(propName, "%s_%04d", varName, i); //make a name for the current target data property (variable_name)
			ZC_DataProperty* dataProperty = ZC_startCmpr(propName, ZC_DOUBLE, g, 0, 0, 0, nbLines, M); //start compression
			cmprBytes = SZ_compress(SZ_DOUBLE, g, &cmprSize, 0, 0, 0, nbLines, M);
			//cmprBytes = SZ_compress_args(SZ_DOUBLE, g, &cmprSize, REL, 1E-3, 1E-3, 1E-3, 0, 0, 0, 0, nbLines, M);
			ZC_CompareData* compareResult =ZC_endCmpr(dataProperty, cmprSize); //end compression
			
			sprintf(cmprCaseName, "%s_%s_%04d(1E-3)", varName, compressorName, i); //name the compression case
			ZC_startDec(); //start decompression
			decData = SZ_decompress(SZ_DOUBLE, cmprBytes, cmprSize, 0, 0, 0, nbLines, M);	
			ZC_endDec(compareResult, cmprCaseName, decData); //end decompression			

			if(rank==0)
			{
				ZC_writeCompressionResult(compareResult, compressorName, propName, "compressionResults");					
				ZC_writeDataProperty(dataProperty, "dataProperties");
			}
			freeDataProperty(dataProperty); //free data property generated at current time step
			freeCompareResult(compareResult); //free compression assessment results generated at current time step
			free(cmprBytes);
			free(decData);
		}
		
		if (((i%ITER_OUT) == 0) && (rank == 0)) {
			printf("Step : %d, error = %f\n", i, globalerror);
		}
		if ((i%REDUCE) == 0) {
			MPI_Allreduce(&localerror, &globalerror, 1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD);
		}
		if(globalerror < PRECISION) {
			break;
		}
	}

	free(h);
	free(g);

	MPI_Finalize();
	SZ_Finalize(); //free the memory for SZ
	ZC_Finalize(); //free the memory for ZC
	return 0;
}
