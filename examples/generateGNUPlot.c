#include <stdio.h>
#include <string.h>
#include <rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    char *zc_cfgFile, *datasetName;

    if(argc < 3)
    {
        printf("Usage: generateGNUPlot [config_file] [title_of_dataset] \n");
        printf("Example: generateGNUPlot zc.config CESM-ATM-data\n");
        exit(0);
    }

    zc_cfgFile=argv[1];
    datasetName = argv[2];

    ZC_Init(zc_cfgFile);
    
    //ZC_compareCompressionRatio();
    ZC_plotComparisonCases();
    ZC_plotRateDistortion();	
    
    ZC_plotAutoCorr_DataProperty();
    ZC_plotAutoCorr_CompressError();
    ZC_plotFFTAmplitude_OriginalData();
    ZC_plotFFTAmplitude_DecompressData();
    
    ZC_plotErrDistribtion();

    ZC_generateOverallReport(datasetName);
    printf("done\n");

    ZC_Finalize();
    return 0;
}
