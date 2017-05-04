#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "latex.h"
#include "ZC_ReportGenerator.h"
#include "zc.h"
#include "ZC_rw.h"

StringLine* ZC_generateCompressionRateFigure()
{
	char* cases[ZC_BUFS];
	int i, n = getComparisonCases(cases);
	StringLine* header = ZC_generateFigureTexLines(n, cases, "compareCompressors", "crate", "Compression Rate (in B/s)");

	for(i=0;i<n;i++)
		free(cases[i]);
	return header;	
}

void ZC_generateCompressionRateReport()
{
	int lineCount;
	char compressionRateTexFile[ZC_BUFS];
	sprintf(compressionRateTexFile, "report/tex/resultsTex/compressionRate.tex");
	printf("%s\n", reportTemplateDir);
	printf("%s\n", compressionRateTexFile);
	StringLine* texLines = ZC_readLines(compressionRateTexFile, &lineCount);
	StringLine* figLines =  ZC_generateCompressionRateFigure();
	int lineNumInsted = ZC_insertLines("%plot compression rate\n", texLines, figLines);
	ZC_writeLines(texLines, compressionRateTexFile);
	ZC_freeLines(texLines);
}

StringLine* ZC_generateDecompressionRateFigure()
{
	char* cases[ZC_BUFS];
	int i, n = getComparisonCases(cases);
	StringLine* header = ZC_generateFigureTexLines(n, cases, "compareCompressors", "drate", "Decompression Rate (in B/s)");
	for(i=0;i<n;i++)
		free(cases[i]);
		
	return header;	
}

void ZC_generateDecompressionRateReport()
{	
	int lineCount;
	char decompressionRateTexFile[ZC_BUFS];
	sprintf(decompressionRateTexFile, "report/tex/resultsTex/decompressionRate.tex");
	printf("%s\n", reportTemplateDir);
	printf("%s\n", decompressionRateTexFile);
	StringLine* texLines = ZC_readLines(decompressionRateTexFile, &lineCount);
	StringLine* figLines =  ZC_generateDecompressionRateFigure();
	int lineNumInsted = ZC_insertLines("%plot decompression rate\n", texLines, figLines);
	ZC_writeLines(texLines, decompressionRateTexFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generatePSNRFigure()
{
	char* cases[ZC_BUFS];
	int i, n = getComparisonCases(cases);
	StringLine* header = ZC_generateFigureTexLines(n, cases, "compareCompressors", "psnr", "Peak Signal-to-Noise Ratio (PSNR)");
	for(i=0;i<n;i++)
		free(cases[i]);
		
	return header;	
}

void ZC_generatePSNRReport()
{
	int lineCount;
	char psnrTexFile[ZC_BUFS];
	sprintf(psnrTexFile, "report/tex/resultsTex/psnr.tex");
	printf("%s\n", reportTemplateDir);
	printf("%s\n", psnrTexFile);
	StringLine* texLines = ZC_readLines(psnrTexFile, &lineCount);
	StringLine* figLines =  ZC_generatePSNRFigure();
	int lineNumInsted = ZC_insertLines("%plot psnr\n", texLines, figLines);
	ZC_writeLines(texLines, psnrTexFile);
	ZC_freeLines(texLines);	
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

StringLine* ZC_generateCompressionFactorFigure()
{
	char* cases[ZC_BUFS];
	int i, n = getComparisonCases(cases);
	StringLine* header = ZC_generateFigureTexLines(n, cases, "compareCompressors", "cratio", "Compression Factor (i.e., Compression Ratio)");
	for(i=0;i<n;i++)
		free(cases[i]);
		
	return header;
}

void ZC_generateCompressionFactorReport()
{
	int lineCount;
	char compressionFactorTexFile[ZC_BUFS];
	sprintf(compressionFactorTexFile, "report/tex/resultsTex/compressionFactor.tex");
	printf("%s\n", reportTemplateDir);
	printf("%s\n", compressionFactorTexFile);
	StringLine* texLines = ZC_readLines(compressionFactorTexFile, &lineCount);
	StringLine* figLines =  ZC_generateCompressionFactorFigure();
	int lineNumInsted = ZC_insertLines("%plot compression factor\n", texLines, figLines);
	ZC_writeLines(texLines, compressionFactorTexFile);
	ZC_freeLines(texLines);
}

void ZC_generateSpectrumDistortionReport()
{
	
}

void ZC_updateZCRootTexFile(char* dataSetName)
{
	int lineCount;
	char rootTexFile[ZC_BUFS], titleLineString[ZC_BUFS];
	sprintf(rootTexFile, "report/z-checker-report.tex");	
	StringLine* texLines = ZC_readLines(rootTexFile, &lineCount);
	sprintf(titleLineString, "\\title{Compression Assessment Report for %s}", dataSetName);
	StringLine* titleLine = createOneStringLine(titleLineString);
	int lineNumInsted = ZC_insertLines("%title_of_report\n", texLines, titleLine);
	ZC_writeLines(texLines, rootTexFile);
	ZC_freeLines(texLines);;
}

void ZC_generateOverallReport(char* dataSetName)
{
	if(reportTemplateDir==NULL)
	{
		printf("Error: reportTemplateDir==NULL\n");
		printf("You need to set the template dir path in zc.config.\n");
		exit(0);
	}
	
	//Perform generating pdf file based on latex command
	char cmd[ZC_BUFS_LONG];
	sprintf(cmd, "if [ -d report ];then rm -rf report;fi; cp -r \"%s\" report", reportTemplateDir);
	printf("%s\n", cmd);
	system(cmd);	
	
	sprintf(cmd, "ln -s \"../../compareCompressors\" report/figs/compareCompressors");  
	system(cmd);	
	
	ZC_generateCompressionRateReport();
	ZC_generateDecompressionRateReport();
	ZC_generatePSNRReport();
	ZC_generateErrDistributionReport();
	ZC_generateErrAutoCorrReport();
	ZC_generateRateDistortionReport();		
	ZC_generateSpectrumDistortionReport();
	ZC_generateCompressionFactorReport();
	ZC_updateZCRootTexFile(dataSetName);
	sprintf(cmd, "cd report;make");
	printf("%s\n", cmd);
	system(cmd);
}

