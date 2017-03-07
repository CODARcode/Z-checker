#include <stdio.h>
#include <string.h>
#include <rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    int r5=0,r4=0,r3=0,r2=0,r1=0;
    char oriFilePath[640], outputFilePath[640];
    char *cfgFile;

    if(argc < 4)
    {
        printf("Usage: analyzeDataProperty [config_file] [srcFilePath] [dimension sizes...]\n");
        printf("Example: analyzeDataProperty zc.config testfloat_8_8_128.dat 8 8 128\n");
        exit(0);
    }

    cfgFile=argv[1];
    sprintf(oriFilePath, "%s", argv[2]);
    if(argc>=4)
        r1 = atoi(argv[3]); //8
    if(argc>=5)
        r2 = atoi(argv[4]); //8
    if(argc>=6)
        r3 = atoi(argv[5]); //128
    if(argc>=7)
        r4 = atoi(argv[6]);
    if(argc>=8)
        r5 = atoi(argv[7]);

    printf("cfgFile=%s\n", cfgFile);
    ZC_Init(cfgFile);

    int nbEle;
    float *data = ZC_readFloatData(oriFilePath, &nbEle);
	
    ZC_DataProperty* property = ZC_genProperties("test1", ZC_FLOAT, data, r5, r4, r3, r2, r1);
	
    ZC_printDataProperty(property);
	
    ZC_writeDataProperty(property, "dataProperties");
	
    printf("done\n");
    return 0;
}
