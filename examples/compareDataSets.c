#include <stdio.h>
#include <string.h>
#include <ZC_rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    int r5=0,r4=0,r3=0,r2=0,r1=0;
    char oriFilePath[640], decFilePath[640];
    char *cfgFile, *compressionCase, *varName;

    if(argc < 8)
    {
        printf("Usage: compareDataSets [config_file] [compressionCase] [oriDataFilePath] [decDataFilePath] [dimension sizes...]\n");
        printf("Example: compareDataSets zc.config SZ testfloat_8_8_128.dat testfloat_8_8_128.dat.out 8 8 128\n");
        exit(0);
    }

    cfgFile=argv[1];
    compressionCase=argv[2];
    varName=argv[3];
    sprintf(oriFilePath, "%s", argv[4]);
    sprintf(decFilePath, "%s", argv[5]);
    if(argc>=7)
        r1 = atoi(argv[6]); //8
    if(argc>=8)
        r2 = atoi(argv[7]); //8
    if(argc>=9)
        r3 = atoi(argv[8]); //128
    if(argc>=10)
        r4 = atoi(argv[9]);
    if(argc>=11)
        r5 = atoi(argv[10]);

    printf("cfgFile=%s\n", cfgFile);
    printf("oriFilePath=%s\n", oriFilePath);
    printf("decFilePath=%s\n", decFilePath);
    ZC_Init(cfgFile);

    int nbEle1, nbEle2;
    float *data1 = ZC_readFloatData(oriFilePath, &nbEle1);
    float *data2 = ZC_readFloatData(decFilePath, &nbEle2);

    if(nbEle1!=nbEle2)
    {
        printf("Error: nbEle1(%d)!=nbEle2(%d)\n",nbEle1, nbEle2);
        exit(0);
    }	

    ZC_CompareData* compareResult = ZC_compareData(varName, ZC_FLOAT, data1, data2, r5, r4, r3, r2, r1);
	
    ZC_printCompressionResult(compareResult);
    
    ZC_writeCompressionResult(compareResult, compressionCase, varName, "compressionResults");
	
    printf("done\n");
    return 0;
}

