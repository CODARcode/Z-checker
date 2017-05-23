/**
 *  @file testfloat_CompDecomp.c
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief This is an example of using compression interface
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sz.h"
#include "rw.h"
#include "zc.h"

int main(int argc, char * argv[])
{
    int r5=0,r4=0,r3=0,r2=0,r1=0;
    char outDir[640], oriFilePath[640], outputFilePath[640];
    char *cfgFile, *zcFile, *solName, *varName, *errBoundMode;
    double absErrBound;
    int errboundmode;
    if(argc < 9)
    {
	printf("Test case: testfloat_CompDecomp [config_file] [zc.config] [solName] [varName] [errBoundMode] [absErrBound] [relErrBound] [srcFilePath] [dimension sizes...]\n");
	printf("Example: testfloat_CompDecomp sz.config zc.config sz(1E-6) testfloat ABS 1E-6 0 testdata/x86/testfloat_8_8_128.dat 8 8 128\n");
	exit(0);
    }
   
    cfgFile=argv[1];
    zcFile=argv[2];
    solName=argv[3];
    varName=argv[4];
    errBoundMode=argv[5];
    if(strcmp(errBoundMode, "PW_REL")==0) 
    {
	errboundmode = PW_REL;
    }
    else if(strcmp(errBoundMode, "ABS")==0)
    {
	errboundmode = ABS;
    }
    else if(strcmp(errBoundMode, "REL")==0)
    {
	errboundmode = REL;
    }
    else
    {
	echo "Error: Z-checker checking doesn't support this error bound mode: %s, but only ABS, REL, and PW_REL.\n", errBoundMode);
	exit(0); 
    }

    absErrBound=atof(arg[6]);
    sprintf(oriFilePath, "%s", argv[7]);
    if(argc>=9)
	r1 = atoi(argv[8]); //8
    if(argc>=10)
	r2 = atoi(argv[9]); //8
    if(argc>=11)
	r3 = atoi(argv[10]); //128
    if(argc>=12)
        r4 = atoi(argv[11]);
    if(argc>=13)
        r5 = atoi(argv[12]);
   
    printf("cfgFile=%s\n", cfgFile); 
    SZ_Init(cfgFile);
   
    printf("zcFile=%s\n", zcFile);
    ZC_Init(zcFile);
 
    sprintf(outputFilePath, "%s.sz", oriFilePath);
   
    int nbEle, status = SZ_SCES;
    float *data = readFloatData(oriFilePath, &nbEle, &status);
   
    int outSize; 
    ZC_DataProperty* dataProperty = ZC_startCmpr(varName, ZC_FLOAT, data, r5, r4, r3, r2, r1);
    
    unsigned char *bytes = SZ_compress_args(SZ_FLOAT, data, &outSize, errboundmode, absErrBound, absErrBound, r5, r4, r3, r2, r1);
    //unsigned char *bytes = SZ_compress(SZ_FLOAT, data, &outSize, r5, r4, r3, r2, r1);
    ZC_CompareData* compareResult = ZC_endCmpr(dataProperty, outSize);
    //writeByteData(bytes, outSize, outputFilePath, &status);
   
    ZC_startDec();
    float *decData = SZ_decompress(SZ_FLOAT, bytes, outSize, r5, r4, r3, r2, r1);
    ZC_endDec(compareResult, solName, decData);
    //ZC_endDec(compareResult, "sz(1E-7)", decData);
 
    freeDataProperty(dataProperty);
    freeCompareResult(compareResult);
    free(data);
    free(bytes);
    free(decData);
    printf("done\n");
    
    SZ_Finalize();
    ZC_Finalize();
    return 0;
}
