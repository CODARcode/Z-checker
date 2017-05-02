#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ZC_ReportGenerator.h"
#include "zc.h"

void ZC_generateCompressionRateReport()
{
	
}

void ZC_generateDecompressionRateReport()
{
	
}

void ZC_generatePSNRReport()
{
	
}

void ZC_generateErrDistributionReport()
{
	
}

void ZC_generateErrAutoCorrReport()
{
	
}

void ZC_generateRateDistortionReport()
{
	
}

void ZC_generateCompressionFactorReport()
{
	
}

void ZC_generateSpectrumDistortionReport()
{
	
}

void ZC_generateOverallReport()
{
	ZC_generateCompressionRateReport();
	ZC_generateDecompressionRateReport();
	ZC_generatePSNRReport();
	ZC_generateErrDistributionReport();
	ZC_generateErrAutoCorrReport();
	ZC_generateRateDistortionReport();
	ZC_generateCompressionFactorReport();
	ZC_generateSpectrumDistortionReport();
	
	//Perform generating pdf file based on latex command
	//system();
}

