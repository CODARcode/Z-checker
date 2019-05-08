/**
 *  @file testHDF5_CompDecomp.c
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
#include "sz.h"
#include "zc.h"
#include "HDF5Reader.h"

#define hdf5_ABS 0
#define hdf5_REL 1
#define hdf5_PWR 10

int errBoundMode = hdf5_ABS;

void usage()
{
	printf("Usage: testHDF5_CompDecomp <options>\n");
	printf("Options:\n");
	printf("* input HDF5 file:\n");
	printf("	-i <hdf5 file>: specify the input hdf5 file\n");
	printf("* configuration files:\n");
	printf("	-e <err config file>: specify the error bound configuration\n");
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
	printf("* error bounds: specifying the error bounds\n");
	printf("	-R: using value range based error bound\n");
	printf("	-A: using absolute error bound\n");
	printf("	-P: using point-wise relative error bound\n");
	printf("* output Workspace\n");
	printf("	-o <workspace dir>\n");
	printf("* examples:\n");
	printf("	testHDF5_CompDecomp -i testdata/testdata.h5 -d 2 -c ~/Development/Z-checker/z-checker-0.2/examples/zc.config -e ~/Development/z-checker-installer/z-checker-installer/errBounds.cfg\n");
}

int main(int argc, char * argv[])
{
	int i = 0, type = -1;
	size_t r5=0,r4=0,r3=0,r2=0,r1=0,n=0;
	int dim = 0;
	char* fields = NULL;	
	char* inPath = NULL, *confPath = NULL, *errPath = NULL, *outDirPath = NULL;
	char* szErrBoundKey = ":sz_d_ERR_BOUNDS";
	char* zfpErrBoundKey = ":zfp_ERR_BOUNDS";
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
		case 'e':
			if (++i == argc)
				usage();
			errPath = argv[i];
			break;
		case 'o':
			if (++i == argc)
				usage();
			outDirPath = argv[i];
			break;
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
		case 'A':
			errBoundMode = hdf5_ABS;
			break;
		case 'R':
			errBoundMode = hdf5_REL;
			break;
		case 'P':
			errBoundMode = hdf5_PWR;
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
	
	if(errPath == NULL)
	{
		printf("Error: missing err configuration file\n");
		exit(0);
	}

	//get error bounds from errBound.cfg

	dictionary *ini = iniparser_load(errPath);
	if (ini == NULL)
	{
		printf("[ZC] Iniparser failed to parse the conf. file.\n");
		return ZC_NSCS;
	}
	char* errBoundString_sz = iniparser_getstring(ini, szErrBoundKey, NULL);
	char* errBoundString_zfp = iniparser_getstring(ini, zfpErrBoundKey, NULL);

	int nbErrBounds_sz = 0, nbErrBounds_zfp = 0;
	char **errBounds_sz = (char**)malloc(sizeof(char*)*20);
	char **errBounds_zfp = (char**)malloc(sizeof(char*)*20);
	for(i=0;i<20;i++)
	{
		errBounds_sz[i] = (char*)malloc(100);
		errBounds_zfp[i] = (char*)malloc(100);		
	}

	printf("errBoundString for SZ = %s\n", errBoundString_sz);
	printf("errBoundString for ZFP = %s\n", errBoundString_zfp);

	char* pch = strtok (errBoundString_sz," ");
	for (i = 0;pch != NULL;i++)
	{
		strcpy(errBounds_sz[i], pch);
		pch = strtok (NULL, " ");
	}
	nbErrBounds_sz = i;

	pch = strtok (errBoundString_zfp," ");
	for (i = 0;pch != NULL;i++)
	{
		strcpy(errBounds_zfp[i], pch);
		pch = strtok (NULL, " ");
	}
	nbErrBounds_zfp = i;

	SZ_Init(NULL);
	ZC_Init(confPath);
	testHDF5_Init(fields, dim, r5, r4, r3, r2, r1, n, type);
	executionMode = ZC_OFFLINE;

	hid_t    file;
	hid_t    grp;

	herr_t   status;

//scan the hdf5 file to do the compression, decompression, and z-checker assessment
	file = H5Fopen(inPath, H5F_ACC_RDWR, H5P_DEFAULT);

	grp = H5Gopen(file,"/");
	//perform data property analysis
	scan_group_property_analysis(grp);
	
	//deal with SZ
	scan_group_SZ(grp, errBoundMode, nbErrBounds_sz, errBounds_sz);
	
	//deal with ZFP
	scan_group_ZFP(grp, errBoundMode, nbErrBounds_zfp, errBounds_zfp);

	status = H5Fclose(file);
	for(i=0;i<20;i++)
	{
		free(errBounds_sz[i]);
		free(errBounds_zfp[i]);
	}
	free(errBounds_sz);
	free(errBounds_zfp);
	ZC_Finalize();
	SZ_Finalize();
	testHDF5_Free();
	iniparser_freedict(ini);
} 
