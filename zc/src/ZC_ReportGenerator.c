#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
	if(reportTemplateFile==NULL)
	{
		printf("Error: reportTemplateFile==NULL\n");
		printf("You need to set the template file path in zc.config.\n");
		exit(0);
	}
	
	ZC_generateCompressionRateReport();
	ZC_generateDecompressionRateReport();
	ZC_generatePSNRReport();
	ZC_generateErrDistributionReport();
	ZC_generateErrAutoCorrReport();
	ZC_generateRateDistortionReport();
	ZC_generateCompressionFactorReport();
	ZC_generateSpectrumDistortionReport();
	
	//Perform generating pdf file based on latex command
	char genReportCmd[ZC_BUFS_LONG];
	sprintf(genReportCmd, "if [ -d report ];then rm -rf report;fi; cp -r \"%s\" report;cd report;make", reportTemplateFile);
	printf("%s\n", genReportCmd);
	system(genReportCmd);
}

