#include <stdio.h>
#include <string.h>
#include <rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    int r5=0,r4=0,r3=0,r2=0,r1=0;
    char oriFilePath[640], decFilePath[640];
    char *cfgFile;

    if(argc < 5)
    {
        printf("Usage: compareDataSets [config_file] [oriDataFilePath] [decDataFilePath] [dimension sizes...]\n");
        printf("Example: compareDataSets zc.config testfloat_8_8_128.dat testfloat_8_8_128.dat.out 8 8 128\n");
        exit(0);
    }

    cfgFile=argv[1];
    sprintf(oriFilePath, "%s", argv[2]);
    sprintf(decFilePath, "%s", argv[3]);
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

    int nbEle1, nbEle2;
    float *data1 = ZC_readFloatData(oriFilePath, &nbEle1);
    float *data2 = ZC_readFloatData(decFilePath, &nbEle2);

    if(nbEle1!=nbEle2)
    {
        printf("Error: nbEle1(%d)!=nbEle2(%d)\n",nbEle1, nbEle2);
        exit(0);
    }	

    ZC_CompareData* compareResult = ZC_compareData("testVar1", ZC_FLOAT, data1, data2, r5, r4, r3, r2, r1);
	
    ZC_printCompareResult(compareResult);
    
    ZC_writeCompareResult(compareResult, "sz", "test1", "compareResults");
	
    printf("done\n");
    return 0;
}

