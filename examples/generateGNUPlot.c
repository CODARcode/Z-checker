#include <stdio.h>
#include <string.h>
#include <ZC_rw.h>
#include <zc.h>

int main(int argc, char * argv[])
{	
    char *zc_cfgFile;

    if(argc < 2)
    {
        printf("Usage: generateGNUPlot [config_file]\n");
        printf("Example: generateGNUPlot zc.config\n");
        exit(0);
    }

    zc_cfgFile=argv[1];

    int lineCount;

	//update zc.config to be the COMPARE_COMPRESSOR checkingStatus.
    StringLine* header = ZC_readLines(zc_cfgFile, &lineCount);
    modifyZCConfig(header, "checkingStatus", "COMPARE_COMPRESSOR");
    ZC_writeLines(header, zc_cfgFile);

    checkingStatus = COMPARE_COMPRESSOR;
    ZC_Init(zc_cfgFile);
    if(checkingStatus!=COMPARE_COMPRESSOR)
    {
		printf("Error: you are running generateGNUPlot, which requires the checkingStatus to be COMPARE_COMPRESSOR.\n");
		exit(0);
    }
    
    //ZC_compareCompressionRatio();
    ZC_plotComparisonCases();
    ZC_plotRateDistortion();	
    
    if(autocorrFlag)
		ZC_plotAutoCorr_DataProperty();
    if(errAutoCorrFlag)
		ZC_plotAutoCorr_CompressError();
    if(fftFlag)
		ZC_plotFFTAmplitude_OriginalData();	
    if(fftFlag)
		ZC_plotFFTAmplitude_DecompressData();
    if(absErrPDFFlag)
		ZC_plotErrDistribtion();
    if(plotImageFlag)
		ZC_plotSliceImage();
	
    ZC_plotOverallRateDistortion_all_compressors();	
		
    ZC_Finalize();
    return 0;
}
