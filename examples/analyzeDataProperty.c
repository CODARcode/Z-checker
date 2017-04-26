#include <stdio.h>
#include <string.h>
#include <rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    int r5=0,r4=0,r3=0,r2=0,r1=0;
    char dataFile[640], outputFilePath[640], oriFilePath[640];
    char *cfgFile, *dataDir;

    if(argc < 5)
    {
        printf("Usage: analyzeDataProperty [config_file] [srcDataDir] [dataFileName] [dimension sizes...]\n");
        printf("Example: analyzeDataProperty zc.config testdata/x86 testfloat_8_8_128.dat 8 8 128\n");
        exit(0);
    }

    cfgFile=argv[1];
    dataDir=argv[2];
    sprintf(dataFile, "%s", argv[3]);
    sprintf(oriFilePath, "%s/%s", dataDir, dataFile);
    if(argc>=5)
        r1 = atoi(argv[4]); //8
    if(argc>=6)
        r2 = atoi(argv[5]); //8
    if(argc>=7)
        r3 = atoi(argv[6]); //128
    if(argc>=8)
        r4 = atoi(argv[7]);
    if(argc>=9)
        r5 = atoi(argv[8]);

    printf("cfgFile=%s\n", cfgFile);
    ZC_Init(cfgFile);

    int nbEle;
    float *data = ZC_readFloatData(oriFilePath, &nbEle);

    ZC_DataProperty* property = ZC_genProperties(dataFile, ZC_FLOAT, data, r5, r4, r3, r2, r1);
	
    ZC_printDataProperty(property);
	
    ZC_writeDataProperty(property, "dataProperties");
	
    printf("done\n");
    return 0;
}
