#include <stdio.h>
#include <string.h>
#include <ZC_rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    char *zc_cfgFile, *datasetName;

    if(argc < 3)
    {
        printf("Usage: generateReport [config_file] [title_of_dataset] \n");
        printf("Example: generateReport zc.config CESM-ATM-data\n");
        exit(0);
    }

    zc_cfgFile=argv[1];
    datasetName = argv[2];

    ZC_Init(zc_cfgFile);
    
    ZC_generateOverallReport(datasetName);
    printf("done\n");

    ZC_Finalize();
    return 0;
}
