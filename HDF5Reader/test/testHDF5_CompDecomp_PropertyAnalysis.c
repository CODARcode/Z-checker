/**
 *  @file testHDF5_CompDecomp_PropertyAnalysis.c
 *  @author Sheng Di
 *  @date April, 2019
 *  @brief This is an example of using compression interface
 *  (C) 2019 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hdf5.h>
#include "zc.h"
#include "HDF5Reader.h"

int errBoundMode = 0;

void usage()
{
	printf("Usage: testHDF5_CompDecomp_PropertyAnalysis <options>\n");
	printf("Options:\n");
	printf("* input HDF5 file:\n");
	printf("	-i <hdf5 file>: specify the input hdf5 file\n");
	printf("* configuration files:\n");
	printf("	-c <zc config file>: specify the ZC configuration file\n");
	printf("* field filter: selecting the fields in terms of specific info.\n");
	printf("	-d <dimsensions> : 1(1D), 2(2D), 3(3D), 12(1D+2D), 13(1D+3D), 23(2D+3D), ...\n");
	printf("	-f <fields> : field1,field2,.... (separated by comma)\n");
	printf("	-1 <nx> : only 1D fields with <nx> dimension will be selected\n");
	printf("	-2 <nx> <ny> : only 2D fields with <nx> <ny> will be selected\n");
	printf("	-3 <nx> <ny> <nz> : dimensions for 3D data such as data[nz][ny][nx] \n");
	printf("	-4 <nx> <ny> <nz> <np>: dimensions for 4D data such as data[np][nz][ny][nx] \n");
	printf(" 	-n <number of elements> : only the fields with >= <number of elements>\n");
	printf("	-t <data type> : only the field with the specific data type. 0:float; 1:double\n");
	printf("* output Workspace\n");
	printf("	-o <workspace dir>\n");
	printf("* examples:\n");
	printf("	testHDF5_CompDecomp_PropertyAnalysis -i testdata/testdata.h5 -d 2 -c ~/Development/Z-checker/z-checker-0.2/examples/zc.config\n");
}

int main(int argc, char * argv[])
{
	int i = 0;
	size_t r5=0,r4=0,r3=0,r2=0,r1=0,n=0;
	int dim = 0, type = 0;
	char* fields = NULL;	
	char* inPath = NULL, *confPath = NULL, *outDirPath = NULL;
	
	if(argc==1)
		usage();

	for(i=1;i<argc;i++)
	{
		if (argv[i][0] != '-' || argv[i][2])
			usage();
		switch (argv[i][1])
		{
		case 'h':
			usage();
			exit(0);
		case 'i':
			if (++i == argc)
				usage();
			inPath = argv[i];		
			break;
		case 'c':
			if (++i == argc)
				usage();
			confPath = argv[i];
			break;
		case 'o':
			if (++i == argc)
				usage();
			outDirPath = argv[i];
		case 'd':
			if (++i == argc || sscanf(argv[i], "%d", &dim) != 1)
				usage();
			break;
		case 'f':
			if (++i == argc)
				usage();
			fields = argv[i];
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
			if (++i == argc || sscanf(argv[i], "%zu", &n) != 1)
				usage();
			break;
		case 't':
			if (++i == argc || sscanf(argv[i], "%d", &type) != 1)
				usage();
			break;				
		default: 
			usage();
			break;
		}
	}

	if((inPath==NULL) & (outDirPath == NULL))
	{
		printf("Error: you need to specify either a an HDF5 file and an output dir for working space.\n");
		exit(0);
	}

	if(confPath == NULL)
	{
		printf("Error: missing zc configuration file\n");
		exit(0);
	}
	
	ZC_Init(confPath);
	testHDF5_Init(fields, dim, r5, r4, r3, r2, r1, n, type);	
	executionMode = ZC_OFFLINE;

	hid_t    file;
	hid_t    grp;

	herr_t   status;

	//scan the hdf5 file to do the z-checker data property analysis 
	file = H5Fopen(inPath, H5F_ACC_RDWR, H5P_DEFAULT);

	grp = H5Gopen(file,"/");
	scan_group_property_analysis(grp);

	status = H5Fclose(file);

	ZC_Finalize();
	testHDF5_Free();
} 
