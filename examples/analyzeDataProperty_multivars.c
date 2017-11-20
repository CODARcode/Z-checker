#include <stdio.h>
#include <string.h>
#include "zc.h"

int main(int argc, char * argv[])
{	
    size_t r5=0,r4=0,r3=0,r2=0,r1=0;
    char dataFile[640], outputFilePath[640], oriFilePath[640];
	char multivarsConfFile[256];

    if(argc < 3)
    {
        printf("Usage: analyzeDataProperty_multivars [zc_configFile] [multi_variable_data_file]\n");
        printf("Example: analyzeDataProperty_multivars zc.config multivars.txt\n");
        exit(0);
    }

	printf("argc=%d\n", argc);
	ZC_Init(argv[1]);
    sprintf(multivarsConfFile, "%s", argv[2]);

	int nbVars, status;
	ZC_CompareData** zcd = loadMultiVars(multivarsConfFile, &nbVars, &status);
	
	if(status==ZC_NSCS)
	{
		printf("Failed Status of loadMultiVars()\n");
		exit(0);
	}
	
	int i = 0;
	ZC_DataProperty* property = NULL;
	for(i=0;i<nbVars;i++)
	{
		property = zcd[i]->property;
		printf("var %d: %s, datatype=%d, r5=%zu, r4=%zu, r3=%zu, r2=%zu, r1=%zu\n", i, property->varName, property->dataType, property->r5, property->r4, property->r3, property->r2, property->r1);
		if(property->dataType==ZC_FLOAT)
		{
			float* data = (float*)property->data;
			printf("\t data=%f %f %f %f %f ....\n", data[0], data[1], data[2], data[3], data[4]);
			free(property->data);
			property->data = NULL;			
		}
		else
		{
			double* data = (double*)property->data;
			printf("\t data=%f %f %f %f %f ....\n", data[0], data[1], data[2], data[3], data[4]);	
			free(property->data);
			property->data = NULL;		
		}
	}
	
	for(i=0;i<nbVars;i++)
		freeCompareResult(zcd[i]);	

	free(zcd);
	
	ZC_Finalize();
    printf("done\n");
    
    return 0;
}
