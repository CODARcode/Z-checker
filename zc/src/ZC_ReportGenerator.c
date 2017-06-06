#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "latex.h"
#include "ZC_ReportGenerator.h"
#include "ZC_util.h"
#include "zc.h"
#include "ZC_rw.h"

StringLine* ZC_generatePropertyAnalysisTable(char** varCases, int varCaseCount)
{
	int numOfElem = 0, dataType = 0;
	float size_inMB = 0;
	char* minValue, *avgValue, *maxValue, *valueRange, *entropy;	
	//char rowTexLine[ZC_BUFS_LONG];
	
	dictionary *ini;
	int i = 0, j = 0;
	char varPropFile[ZC_BUFS];
	int rows = varCaseCount, columns = 7;
	char** keys = (char**)malloc(sizeof(char*)*columns);
	for(i=0;i<columns;i++)
		keys[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	strcpy(keys[0], "numOfElem");
	strcpy(keys[1], "size(MB)");
	strcpy(keys[2], "minValue");
	strcpy(keys[3], "avgValue");
	strcpy(keys[4], "maxValue");
	strcpy(keys[5], "valueRange");
	strcpy(keys[6], "entropy");
	
	char*** cells = (char***)malloc(sizeof(char**)*varCaseCount);
	for(i=0;i<varCaseCount;i++)
	{
		cells[i] = (char**)malloc(sizeof(char*)*columns);
		for(j=0;j<columns;j++)
			cells[i][j] = (char*)malloc(sizeof(char)*ZC_BUFS);
	}
	
	for(i=0;i<varCaseCount;i++)
	{
		sprintf(varPropFile, "dataProperties/%s.prop", varCases[i]);
		ini = iniparser_load(varPropFile);
		if (ini == NULL)
		{
			printf("[ZC] Iniparser failed to parse the dataProperties/%s file.\n", varCases[i]);
			exit(0);
		}
		numOfElem = iniparser_getint(ini, "PROPERTY:numOfElem", 0);
		if(dataType == ZC_FLOAT)
			size_inMB = ((float)numOfElem)*4/1024/1024;
		else if(dataType == ZC_DOUBLE)
			size_inMB = ((float)numOfElem)*8/1024/1024;
		else
		{
			printf("Error: support only float or double but doesn't support other types\n");
			exit(0);
		}
		minValue = iniparser_getstring(ini, "PROPERTY:minValue", "0");
		avgValue = iniparser_getstring(ini, "PROPERTY:avgValue", "0");
		maxValue = iniparser_getstring(ini, "PROPERTY:maxValue", "0");
		valueRange = iniparser_getstring(ini, "PROPERTY:valueRange", "0");
		entropy = iniparser_getstring(ini, "PROPERTY:entropy", "0");
		
		//sprintf(rowTexLine, "\\hline %d & %f & %s & %s & %s & %s & %s \\\\", numOfElem, size_inMB, minValue, avgValue, maxValue, valueRange, entropy);
		sprintf(cells[i][0], "%d", numOfElem);
		sprintf(cells[i][1], "%f", size_inMB);
		strcpy(cells[i][2], minValue);
		strcpy(cells[i][3], avgValue);
		strcpy(cells[i][4], maxValue);
		strcpy(cells[i][5], valueRange);
		strcpy(cells[i][6], entropy);
	}
	
	StringLine* header = ZC_generateSimpleTableTexLines(rows, columns, 
	varCases, keys, cells, "Properties of Original Data", "tab:propAnalysis");
	
	for(i=0;i<varCaseCount;i++)
	{
		for(j=0;j<columns;j++)
			free(cells[i][j]);
		free(cells[i]);
	}
	free(cells);
	for(i=0;i<varCaseCount;i++)
		free(keys[i]);
	free(keys);
	
	iniparser_freedict(ini);	
	return header;
}

void ZC_extractCompressorAndErrorBounds(char** compressionCaseFiles, int caseCount)
{
	int i = 0, j = 0, mark = 0, len = 0;
    //char buff[ZC_BUFS];
	int compressorNum = 0, errorBoundNum = 0;
	char *cmpCaseFile, *cmpCase, *varCase, *varName, *compressor, *errBound, *tmpErrBoundString;
	for(i=0;i<caseCount;i++)
	{
		cmpCaseFile = compressionCaseFiles[i];
        cmpCase = strtok(cmpCaseFile,":"); //there should be only one ":"
        varCase = strtok(NULL,":");
       
        compressor = strtok(cmpCase, "(");
        tmpErrBoundString = strtok(NULL,"(");
        errBound = rtrim_C(tmpErrBoundString, ')');
        len = strlen(varCase);
        varCase[len-8]='\0';
        varName = varCase;
        
		checkAndAddStringToList(allCompressors, &allCompressorCount, compressor);//e.g., compressor=sz
		checkAndAddStringToList(allErrorBounds, &allErrorBoundCount, errBound); //e.g., errBound=1E-3
		checkAndAddStringToList(allVarCases, &allVarCaseCount, varName); //e.g., varName=FLDSC_1_1800_3600
	}
}

void ZC_constructSortedSelectedErrorBounds(StringElem* selectedErrorBounds, int *selectedErrorBoundCount)
{
	int errorBoundInterval = 0, i, j;
	int errBoundCount = 0;
	StringElem sortedErrorBounds[ZC_BUFS];
	for(i=0;i<ZC_BUFS;i++)
		sortedErrorBounds[i] = (StringElem)malloc(sizeof(struct StringElem_t));
	//select error bounds based on errorBoundCases setting
	if(allErrorBoundCount%numOfErrorBoundCases==0)
		errorBoundInterval = allErrorBoundCount/numOfErrorBoundCases;
	else
		errorBoundInterval = allErrorBoundCount/numOfErrorBoundCases+1;

	for(i=0;i<allErrorBoundCount;i++)
	{
		sortedErrorBounds[i]->str = allErrorBounds[i];
		sortedErrorBounds[i]->value = atof(allErrorBounds[i]);
	}
	
	//sort error bounds
	ZC_quick_sort2(sortedErrorBounds,0,allErrorBoundCount-1);	
	
	for(i=0;i<allErrorBoundCount;i++)
	{
		if(i%errorBoundInterval==0)
		{
			selectedErrorBounds[errBoundCount]->str = sortedErrorBounds[i]->str;
			selectedErrorBounds[errBoundCount]->value = atof(sortedErrorBounds[i]->str);
			errBoundCount++;
		}
	}
	
	for(i=0;i<ZC_BUFS;i++)
		free(sortedErrorBounds[i]);
	
	*selectedErrorBoundCount = errBoundCount;
	//no need to sort varCases, because it's fine as long as a particular order of 
	//presenting all varCases is chosen and it's consistent.
}

StringLine* ZC_generateDataPropertyAnalysisFigures(char** caseNames, int caseNameCount)
{	
	StringLine* figHeader = NULL, *figHeader2 = NULL;
	char caption[ZC_BUFS], figLabel[ZC_BUFS];
	int i, n = 0;
	char** caseFiles = (char**)malloc(sizeof(char*)*caseNameCount);
	for(i=0;i<caseNameCount;i++)
		caseFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	
	if(autocorrFlag)
	{
		for(i=0;i<caseNameCount;i++)
			sprintf(caseFiles[i], "%s-autocorr", caseNames[i]);
		strcpy(caption, "Auto-correlation of the data");
		strcpy(figLabel, "dp-autocorr");
		figHeader = ZC_generateVarStatFigTexLines(caseNameCount, caseFiles, "dataProperties", caption, figLabel);		
	}		
		
	if(fftFlag)
	{
		for(i=0;i<caseNameCount;i++)
			sprintf(caseFiles[i], "%s-fft-amp", caseNames[i]);
		strcpy(caption, "Amplitude of FFT Coefficients of the data");
		strcpy(figLabel, "dp-fft-amp");
		figHeader2 = ZC_generateVarStatFigTexLines(caseNameCount, caseFiles, "dataProperties", caption, figLabel);			
	}	
	
	ZC_appendLines(figHeader, figHeader2);
	for(i=0;i<caseNameCount;i++)
		free(caseFiles[i]);
	free(caseFiles);
	return figHeader;
}

void ZC_generateDataPropertyAnalysisReport()
{
	int lineCount;
	char texFile[ZC_BUFS], varCountString[ZC_BUFS], varListString[ZC_BUFS_SUPER_LONG], buf[ZC_BUFS_SUPER_LONG];
	memset(varListString, 0, ZC_BUFS_SUPER_LONG);
	sprintf(texFile, "report/tex/property.tex");
	printf("Processing %s\n", texFile);
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	
	int i, n = 0, len = 0;
	char* caseFiles[ZC_BUFS_LONG];
	for(i=0;i<ZC_BUFS_LONG;i++)
		caseFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	ZC_getFileNames("dataProperties", "prop", &n, caseFiles);
	
	len = strlen(caseFiles[0]);
	len = len - 5;
	caseFiles[0][len] = '\0';	
	strcpy(varListString, caseFiles[0]);
		
	for(i=1;i<n;i++)
	{
		len = strlen(caseFiles[i]);
		len = len - 5; //remove the extension prop
		caseFiles[i][len] = '\0';
		sprintf(buf, "%s, %s", varListString, caseFiles[i]);
		strcpy(varListString, buf);
	}
	
	sprintf(varCountString, "%d", n);
	ZC_replaceLines(texLines, "ZC_VARNUM", varCountString);
	ZC_ReplaceStr2(varListString, "_", "\\_");
	ZC_replaceLines(texLines, "ZC_VARLIST", varListString);
	
	StringLine* tabLines = ZC_generatePropertyAnalysisTable(caseFiles, n);
	int lineNumInsted = ZC_insertLines("%create property table\n", texLines, tabLines);
	
	StringLine* figLines =  ZC_generateDataPropertyAnalysisFigures(caseFiles, n);
	lineNumInsted = ZC_insertLines("%plot data properties\n", texLines, figLines);
	
	ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generateCompressionRateFigure()
{
	char* cases[ZC_BUFS];
	int i, n = getComparisonCases(cases);
	StringLine* header = ZC_generateComparisonFigTexLines(n, cases, "compareCompressors", "crate", "Compression Rate (in B/s)");

	for(i=0;i<n;i++)
		free(cases[i]);
	return header;	
}

void ZC_generateCompressionRateReport()
{
	int lineCount;
	char compressionRateTexFile[ZC_BUFS];
	sprintf(compressionRateTexFile, "report/tex/resultsTex/compressionRate.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", compressionRateTexFile);
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
	StringLine* header = ZC_generateComparisonFigTexLines(n, cases, "compareCompressors", "drate", "Decompression Rate (in B/s)");
	for(i=0;i<n;i++)
		free(cases[i]);
		
	return header;	
}

void ZC_generateDecompressionRateReport()
{	
	int lineCount;
	char decompressionRateTexFile[ZC_BUFS];
	sprintf(decompressionRateTexFile, "report/tex/resultsTex/decompressionRate.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", decompressionRateTexFile);
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
	StringLine* header = ZC_generateComparisonFigTexLines(n, cases, "compareCompressors", "psnr", "Peak Signal-to-Noise Ratio (PSNR)");
	for(i=0;i<n;i++)
		free(cases[i]);
		
	return header;	
}

void ZC_generatePSNRReport()
{
	int lineCount;
	char psnrTexFile[ZC_BUFS];
	sprintf(psnrTexFile, "report/tex/resultsTex/psnr.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", psnrTexFile);
	StringLine* texLines = ZC_readLines(psnrTexFile, &lineCount);
	StringLine* figLines =  ZC_generatePSNRFigure();
	int lineNumInsted = ZC_insertLines("%plot psnr\n", texLines, figLines);
	ZC_writeLines(texLines, psnrTexFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generateStaticAnalysisFigures(char* metricType, 
StringElem* selectedErrorBounds, int selectedErrorBoundCount)
{
	int i, j, k, n = 0;// n: numOfEpsFiles
	char* selectedEpsFiles[ZC_BUFS];
	for(i=0;i<ZC_BUFS;i++)
		selectedEpsFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	char* selectedErrorBound, *varCase, *compressor;
	
	StringLine* totalHeader = createStringLineHeader();
	StringLine* figHeader;
	char caption[ZC_BUFS], figLabel[ZC_BUFS];
	
	//construct the selected cases (eps files) based on compressors
	for(i=0;i<allCompressorCount;i++)
	{		
		compressor = allCompressors[i];
		//construct figure tex lines
		for(j=0;j<selectedErrorBoundCount;j++)
		{
			selectedErrorBound = selectedErrorBounds[j]->str;
			for(k=0;k<allVarCaseCount;k++)
			{
				varCase = allVarCases[k];
				
				//construct varFiles
				sprintf(selectedEpsFiles[n++], "%s(%s):%s.dat-%s", compressor, selectedErrorBound, varCase, metricType);
			}
			//generate figure tex lines	
			//TODO
			sprintf(caption, "%s: compressor=%s, err bound=%s", metricType, compressor, selectedErrorBound);
			sprintf(figLabel, "%s-%s-%s", metricType, compressor, selectedErrorBound);
			figHeader = ZC_generateVarStatFigTexLines(n, selectedEpsFiles, "compressionResults", caption, figLabel);		
			ZC_appendLines(totalHeader, figHeader);
			
			n = 0;
		}
	}

	for(j=0;j<ZC_BUFS;j++)
		free(selectedEpsFiles[j]);
		
	return totalHeader;	
}

StringLine* ZC_generateRateDistortionFigure()
{
	int i, varCount;
	char* varFiles[ZC_BUFS];
	char* fileName;
	char rateDisFile[ZC_BUFS];
	
	for(i=0;i<ZC_BUFS;i++)
		varFiles[i] = (char*)malloc(sizeof(char)*500);
	ZC_getFileNames("dataProperties", "prop", &varCount, varFiles);
	for(i = 0;i<varCount;i++)
	{
		fileName = rmFileExtension(varFiles[i]);
		strcpy(varFiles[i], fileName);
		free(fileName);
	}
	StringLine* header = ZC_generateComparisonFigTexLines(varCount, varFiles, 
	"compareCompressors", "rate-distortion_psnr", "Rate Distortion (PSNR vs. Bit-rate)");
	
	for(i=0;i<varCount;i++)
		free(varFiles[i]);
	
	return header;
}

void ZC_generateRateDistortionReport()
{
	int lineCount;
	char rateDisTexFile[ZC_BUFS];
	sprintf(rateDisTexFile, "report/tex/resultsTex/rateDistortion.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", rateDisTexFile);
	StringLine* texLines = ZC_readLines(rateDisTexFile, &lineCount);
	StringLine* figLines =  ZC_generateRateDistortionFigure();
	int lineNumInsted = ZC_insertLines("%plot rate distortion\n", texLines, figLines);
	ZC_writeLines(texLines, rateDisTexFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generateRateCorrelationFigure()
{
	int i, varCount;
	char* varFiles[ZC_BUFS];
	char* fileName;
	char rateDisFile[ZC_BUFS];

	for(i=0;i<ZC_BUFS;i++)
		varFiles[i] = (char*)malloc(sizeof(char)*500);
	ZC_getFileNames("dataProperties", "prop", &varCount, varFiles);
	for(i = 0;i<varCount;i++)
	{
		fileName = rmFileExtension(varFiles[i]);
		strcpy(varFiles[i], fileName);
		free(fileName);
	}
	StringLine* header = ZC_generateComparisonFigTexLines(varCount, varFiles,
	"compareCompressors", "rate-corr", "Rate Correlation (Correlation vs. Bit-rate)");

	for(i=0;i<varCount;i++)
		free(varFiles[i]);

	return header;
}

void ZC_generateRateCorrelationReport()
{
	int lineCount;
	char rateDisTexFile[ZC_BUFS];
	sprintf(rateDisTexFile, "report/tex/resultsTex/rateCorrelation.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", rateDisTexFile);
	StringLine* texLines = ZC_readLines(rateDisTexFile, &lineCount);
	StringLine* figLines =  ZC_generateRateCorrelationFigure();
	int lineNumInsted = ZC_insertLines("%plot rate correlation\n", texLines, figLines);
	ZC_writeLines(texLines, rateDisTexFile);
	ZC_freeLines(texLines);
}

StringLine* ZC_generateCompressionFactorFigure()
{
	char* cases[ZC_BUFS];
	int i, n = getComparisonCases(cases);
	StringLine* header = ZC_generateComparisonFigTexLines(n, cases, "compareCompressors", "cratio", "Compression Factor (i.e., Compression Ratio)");
	for(i=0;i<n;i++)
		free(cases[i]);
		
	return header;
}

void ZC_generateCompressionFactorReport()
{
	int lineCount;
	char compressionFactorTexFile[ZC_BUFS];
	sprintf(compressionFactorTexFile, "report/tex/resultsTex/compressionFactor.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", compressionFactorTexFile);
	StringLine* texLines = ZC_readLines(compressionFactorTexFile, &lineCount);
	StringLine* figLines =  ZC_generateCompressionFactorFigure();
	int lineNumInsted = ZC_insertLines("%plot compression factor\n", texLines, figLines);
	ZC_writeLines(texLines, compressionFactorTexFile);
	ZC_freeLines(texLines);
}

void ZC_generateErrDistributionReport(StringElem* selectedErrorBounds, int selectedErrorBoundCount)
{
	int lineCount;
	char texFile[ZC_BUFS];
	sprintf(texFile, "report/tex/resultsTex/errDistribution.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", texFile);
	
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	
	if(absErrPDFFlag)
	{
		StringLine* figLines =  ZC_generateStaticAnalysisFigures("dis", selectedErrorBounds, selectedErrorBoundCount);
		int lineNumInsted = ZC_insertLines("%plot error distribution\n", texLines, figLines);		
	}

	if(pwrErrPDFFlag)
	{
		StringLine* figLines2 = ZC_generateStaticAnalysisFigures("pds", selectedErrorBounds, selectedErrorBoundCount);
		int lineNumInsted = ZC_insertLines("%plot point-wise relative error distribution\n", texLines, figLines2);		
	}
	if(absErrPDFFlag || pwrErrPDFFlag)
		ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);
}

void ZC_generateErrAutoCorrReport(StringElem* selectedErrorBounds, int selectedErrorBoundCount)
{
	int lineCount;
	char texFile[ZC_BUFS];
	sprintf(texFile, "report/tex/resultsTex/errAutoCorr.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", texFile);
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	StringLine* figLines =  ZC_generateStaticAnalysisFigures("autocorr", selectedErrorBounds, selectedErrorBoundCount);
	int lineNumInsted = ZC_insertLines("%plot error auto correlation\n", texLines, figLines);
	
	ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);	
}

void ZC_generateSpectrumDistortionReport(StringElem* selectedErrorBounds, int selectedErrorBoundCount)
{
	int lineCount;
	char texFile[ZC_BUFS];
	sprintf(texFile, "report/tex/resultsTex/specDistortion.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", texFile);
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	StringLine* figLines =  ZC_generateStaticAnalysisFigures("fft-amp", selectedErrorBounds, selectedErrorBoundCount);
	int lineNumInsted = ZC_insertLines("%plot spectrum distortion\n", texLines, figLines);
	
	ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);		
}

void ZC_updateZCRootTexFile(char* dataSetName)
{
	int lineCount;
	char rootTexFile[ZC_BUFS], dsName[ZC_BUFS];
	char* titleLineString = (char*)malloc(sizeof(char)*ZC_BUFS);
	sprintf(rootTexFile, "report/z-checker-report.tex");	
	StringLine* texLines = ZC_readLines(rootTexFile, &lineCount);
	strcpy(dsName, dataSetName);
	ZC_ReplaceStr2(dsName, "_", "\\_");
	sprintf(titleLineString, "\\title{Compression Assessment Report for %s}", dsName);
	StringLine* titleLine = createOneStringLine(titleLineString);
	int lineNumInsted = ZC_insertLines("%title_of_report\n", texLines, titleLine);
	ZC_writeLines(texLines, rootTexFile);
	ZC_freeLines(texLines);
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
	
	sprintf(cmd, "ln -s \"../../compareCompressors\" report/figs/compareCompressors;ln -s \"../../compressionResults\" report/figs/compressionResults;ln -s \"../../dataProperties\" report/figs/dataProperties");  
	system(cmd);	
	
	ZC_generateDataPropertyAnalysisReport();
	
	ZC_generateCompressionFactorReport();	
	ZC_generateCompressionRateReport();
	ZC_generateDecompressionRateReport();
	ZC_generatePSNRReport();
	ZC_generateRateDistortionReport();
	ZC_generateRateCorrelationReport();

	int i, n = 0, selectedErrorBoundCount;
	char* caseFiles[ZC_BUFS_LONG];
	for(i=0;i<ZC_BUFS_LONG;i++)
		caseFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	ZC_getFileNames("compressionResults", "dis", &n, caseFiles);
	ZC_extractCompressorAndErrorBounds(caseFiles, n);
	for(i=0;i<ZC_BUFS;i++)
		free(caseFiles[i]);
	
	StringElem selectedErrorBounds[ZC_BUFS];
	for(i=0;i<ZC_BUFS;i++)
		selectedErrorBounds[i] = (StringElem)malloc(sizeof(struct StringElem_t));
	ZC_constructSortedSelectedErrorBounds(selectedErrorBounds, &selectedErrorBoundCount);
	
	ZC_generateErrDistributionReport(selectedErrorBounds, selectedErrorBoundCount);
	ZC_generateErrAutoCorrReport(selectedErrorBounds, selectedErrorBoundCount);	
	ZC_generateSpectrumDistortionReport(selectedErrorBounds, selectedErrorBoundCount);
	
	ZC_updateZCRootTexFile(dataSetName);
	//sprintf(cmd, "cd report;make clean;make");
	//printf("%s\n", cmd);
	//system(cmd);
}

