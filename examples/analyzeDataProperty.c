#include <stdio.h>
#include <string.h>
#include "ZC_rw.h"
#include "zc.h"

int main(int argc, char * argv[])
{	
    size_t r5=0,r4=0,r3=0,r2=0,r1=0;
    char oriFilePath[640];
    char *datatype, *cfgFile, *varName;

    if(argc < 4)
    {
        printf("Usage: analyzeDataProperty [varName] [datatype (-f or -d)] [config_file] [filePath] [dimension sizes...]\n");
        printf("Example: analyzeDataProperty var1 -f zc.config testdata/x86/testfloat_8_8_128.dat 8 8 128\n");
        exit(0);
    }

    varName = argv[1];
    datatype=argv[2];
    cfgFile=argv[3];
    sprintf(oriFilePath, "%s", argv[4]); 

    if(argc>=6)
        r1 = atoi(argv[5]); //8
    if(argc>=7)
        r2 = atoi(argv[6]); //8
    if(argc>=8)
        r3 = atoi(argv[7]); //128
    if(argc>=9)
        r4 = atoi(argv[8]);
    if(argc>=10)
        r5 = atoi(argv[9]);

    if(executionMode==ZC_ONLINE)
    {
		printf("Error: you are running analyzeProperty, so please set the executionMode to OFFLINE in zc.config\n");
		printf("Usage: for online executionMode with MPI support, you need to run analyzeProperty_online instead.\n");
		exit(0);
    }

    //printf("myrank=%d, cfgFile=%s\n", myrank, cfgFile);
    printf("[ZC] Reading ZC configuration file (%s) ...\n", cfgFile);     
    checkingStatus = ANALYZE_DATA;
    ZC_Init(cfgFile);
    executionMode = ZC_OFFLINE;
	
    size_t nbEle;
    ZC_DataProperty* property = NULL;
    if(strcmp(datatype, "-f")==0)
    {
		float *data = ZC_readFloatData(oriFilePath, &nbEle);
		property = ZC_genProperties(varName, ZC_FLOAT, data, r5, r4, r3, r2, r1);
    }
    else if(strcmp(datatype, "-d")==0)
    {
		double *data = ZC_readDoubleData(oriFilePath, &nbEle);
		property = ZC_genProperties(varName, ZC_DOUBLE, data, r5, r4, r3, r2, r1);
    }
    
    property->filePath = oriFilePath;
	
    ZC_printDataProperty(property);
	
    ZC_writeDataProperty(property, "dataProperties");
	
    printf("done\n");
    return 0;
}
