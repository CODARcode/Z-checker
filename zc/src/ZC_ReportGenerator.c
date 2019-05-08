#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ZC_util.h"
#include "ZC_latex.h"
#include "ZC_ReportGenerator.h"
#include "ZC_rw.h"
#include "zc.h"

StringLine* ZC_generatePropertyAnalysisTable(char** varCases, int varCaseCount)
{
	size_t numOfElem = 0, dataType = 0;
	float size_inMB = 0;
	char* minValue, *avgValue, *maxValue, *valueRange, *entropy;	
	//char rowTexLine[ZC_BUFS_LONG];
	
	dictionary *ini;
	size_t i = 0, j = 0;
	char varPropFile[ZC_BUFS];
	int rows = varCaseCount, columns = 7;
	char** keys = (char**)malloc(sizeof(char*)*columns);
	for(i=0;i<columns;i++)
		keys[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	strcpy(keys[0], "dimensions");
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
	
	char dimensions[50];
	size_t r5 = 0, r4 = 0, r3 = 0, r2 = 0, r1 = 0;
	memset(dimensions, 0, 50);
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
		
		
		r5 = iniparser_getlint(ini, "PROPERTY:r5", 0);
		r4 = iniparser_getlint(ini, "PROPERTY:r4", 0);
		r3 = iniparser_getlint(ini, "PROPERTY:r3", 0);
		r2 = iniparser_getlint(ini, "PROPERTY:r2", 0);
		r1 = iniparser_getlint(ini, "PROPERTY:r1", 0);
		
		ZC_constructDimString(r5, r4, r3, r2, r1, dimensions);
		
		minValue = iniparser_getstring(ini, "PROPERTY:minValue", "0");
		avgValue = iniparser_getstring(ini, "PROPERTY:avgValue", "0");
		maxValue = iniparser_getstring(ini, "PROPERTY:maxValue", "0");
		valueRange = iniparser_getstring(ini, "PROPERTY:valueRange", "0");
		entropy = iniparser_getstring(ini, "PROPERTY:entropy", "0");
		
		//sprintf(rowTexLine, "\\hline %d & %f & %s & %s & %s & %s & %s \\\\", numOfElem, size_inMB, minValue, avgValue, maxValue, valueRange, entropy);
		strcpy(cells[i][0], dimensions);
		sprintf(cells[i][1], "%f", size_inMB);
		strcpy(cells[i][2], minValue);
		strcpy(cells[i][3], avgValue);
		strcpy(cells[i][4], maxValue);
		strcpy(cells[i][5], valueRange);
		strcpy(cells[i][6], entropy);
		
		iniparser_freedict(ini);	
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
	for(i=0;i<columns;i++)
		free(keys[i]);
	free(keys);
	
	return header;
}

void ZC_extractCompressorAndErrorBounds(char** compressionCaseFiles, int caseCount)
{
	size_t i = 0;
    //char buff[ZC_BUFS];
	char *cmpCaseFile, *cmpCase, *varCase, *varName, *compressor, *errBound, *tmpErrBoundString;
	for(i=0;i<caseCount;i++)
	{
		cmpCaseFile = compressionCaseFiles[i];
        cmpCase = strtok(cmpCaseFile,":"); //there should be only one ":"
        varCase = strtok(NULL,":");
       
        compressor = strtok(cmpCase, "(");
        tmpErrBoundString = strtok(NULL,"(");
        errBound = rtrim_C(tmpErrBoundString, ')');
        //strlen(varCase);
        
        //remove the first "." in the varCase (e.g., FLDSC_1_1800_3600.dat.dis --> FLDSC_1_1800_3600)
        /*j = strcspn(varCase,".");
        varCase[j]='\0';
        varName = varCase;*/
        removeExtension2(varCase);
        varName = varCase;
        
		//checkAndAddStringToList(allCompressors, &allCompressorCount, compressor);//e.g., compressor=sz
		//checkAndAddStringToList(allErrorBounds, &allErrorBoundCount, errBound); //e.g., errBound=1E-3
		
		checkAndAddCmprorToList(allCompressors, &allCompressorCount, compressor, errBound);
		checkAndAddStringToList(allVarCases, &allVarCaseCount, varName); //e.g., varName=FLDSC_1_1800_3600
	}
}


void ZC_constructSortedSelectedErrorBounds(CmprsorErrBound* compressor)
{
	StringElem* allErrBounds = compressor->allErrBounds;
	int allErrBoundCount = compressor->allErrBoundCount;
	StringElem* selectedErrBounds = compressor->selErrBounds;
	
	size_t errorBoundInterval = 0, i;
	size_t errBoundCount = 0;
	StringElem sortedErrorBounds[ZC_BUFS];
	for(i=0;i<ERRBOUND_MAX_LEN;i++)
		sortedErrorBounds[i] = (StringElem)malloc(sizeof(struct StringElem_t));
	//select error bounds based on errorBoundCases setting
	if(allErrBoundCount%numOfErrorBoundCases==0)
		errorBoundInterval = allErrBoundCount/numOfErrorBoundCases;
	else
		errorBoundInterval = allErrBoundCount/numOfErrorBoundCases+1;

	for(i=0;i<allErrBoundCount;i++)
	{
		sortedErrorBounds[i]->str = allErrBounds[i]->str;
		sortedErrorBounds[i]->value = atof(allErrBounds[i]->str);
	}
	
	//sort error bounds
	ZC_quick_sort2(sortedErrorBounds,0,allErrBoundCount-1);	
	
	for(i=0;i<allErrBoundCount;i++)
	{
		if(i%errorBoundInterval==0)
		{
			selectedErrBounds[errBoundCount] = (StringElem)malloc(sizeof(struct StringElem_t));
			selectedErrBounds[errBoundCount]->str = sortedErrorBounds[i]->str;
			selectedErrBounds[errBoundCount]->value = atof(sortedErrorBounds[i]->str);
			errBoundCount++;
		}
	}
	
	for(i=0;i<ERRBOUND_MAX_LEN;i++)
		free(sortedErrorBounds[i]);
	
	compressor->selErrBoundCount = errBoundCount;
	//no need to sort varCases, because it's fine as long as a particular order of 
	//presenting all varCases is chosen and it's consistent.
}

void ZC_constructSortedSelectedErrorBounds4CmprsEelments(CmprsorErrBound *allCompressors, int allCompressorCount)
{
	size_t i = 0;
	for(i=0;i < allCompressorCount;i++)
	{
		ZC_constructSortedSelectedErrorBounds(allCompressors+i);
		//printf("allCompressors[%d].compressorName=%s\n", i, allCompressors[i].compressorName);
	}
}

StringLine* ZC_generateDataPropertyAnalysisFigures(char** caseNames, int caseNameCount)
{	
	StringLine* resultHeader = NULL, *figHeader = NULL, *figHeader2 = NULL;
	char caption[ZC_BUFS], figLabel[ZC_BUFS];
	size_t i = 0;
	char** caseFiles = (char**)malloc(sizeof(char*)*caseNameCount);
	for(i=0;i<caseNameCount;i++)
		caseFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	
	if(autocorrFlag)
	{
		resultHeader = createOneStringLine("The auto-correlation results with different lags (1 through 100) are presented in Figure \ref{fig:dp-autocorr}.\n");
		for(i=0;i<caseNameCount;i++)
			sprintf(caseFiles[i], "%s-autocorr", caseNames[i]);
		strcpy(caption, "Auto-correlation of the data");
		strcpy(figLabel, "dp-autocorr");
		figHeader = ZC_generateVarStatFigTexLines(caseNameCount, caseFiles, "dataProperties", caption, figLabel);		
		ZC_appendLines(resultHeader, figHeader);
	}
	
	if(fftFlag)
	{
		StringLine* fftHeader = createOneStringLine("The amplitude values of the spectrum analysis (based on FFT) are presented in Figure \ref{fig:dp-fft-amp}. Since the amplitude value of the first spectrum coefficient is often much larger than others, we present its value on the left-corner of each figure instead of showing its bar in the figure.\n");
		for(i=0;i<caseNameCount;i++)
			sprintf(caseFiles[i], "%s-fft-amp", caseNames[i]);
		strcpy(caption, "Amplitude of FFT Coefficients of the data");
		strcpy(figLabel, "dp-fft-amp");
		figHeader2 = ZC_generateVarStatFigTexLines(caseNameCount, caseFiles, "dataProperties", caption, figLabel);			
		ZC_appendLines(fftHeader, figHeader2);
		ZC_appendLines(resultHeader, fftHeader);
	}
	
	for(i=0;i<caseNameCount;i++)
		free(caseFiles[i]);
	free(caseFiles);
	return resultHeader;
}

StringLine* ZC_generateDataSliceImages(char** caseNames, int caseNameCount)
{
	StringLine* resultHeader = NULL, *figHeader = NULL, *figHeader2 = NULL;
	char caption[ZC_BUFS], figLabel[ZC_BUFS];
	size_t i = 0;
	char** caseFiles = (char**)malloc(sizeof(char*)*caseNameCount);
	for(i=0;i<caseNameCount;i++)
		caseFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	
	if(plotImageFlag)
	{
		resultHeader = createOneStringLine("The slice images\\footnote{If the dataset is of 3D {z,y,x}, the image shown here is the middle slice, i.e., {i=z/2,j={1..y},k={1..x}}; if the dataset is of 2D {y,x}, the image shownhere is the whole image, i.e., {i={1..y},j={1..x}}.} of the datasets are presented in Figure \\ref{fig:dp-slice-ori} and Figure \\ref{fig:dp-slice-log} in original data domain and log domain, respectively.\n");
		
		for(i=0;i<caseNameCount;i++)
			sprintf(caseFiles[i], "%s-oriimg", caseNames[i]);
		strcpy(caption, "Slice Image (original domain)");
		strcpy(figLabel, "dp-slice-ori");
		figHeader = ZC_generateSliceImageTexLines(caseNameCount, caseFiles, "dataProperties", caption, figLabel);		
		ZC_appendLines(resultHeader, figHeader);
	
		for(i=0;i<caseNameCount;i++)
			sprintf(caseFiles[i], "%s-logimg", caseNames[i]);
		strcpy(caption, "Slice Image (log domain)");
		strcpy(figLabel, "dp-slice-log");
		figHeader2 = ZC_generateSliceImageTexLines(caseNameCount, caseFiles, "dataProperties", caption, figLabel);			
		ZC_appendLines(resultHeader, figHeader2);
	}	
	
	for(i=0;i<caseNameCount;i++)
		free(caseFiles[i]);
	free(caseFiles);
	return resultHeader;	
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
	ZC_replaceLines(texLines, "ZCVARNUM", varCountString);
	ZC_ReplaceStr2(varListString, "_", "\\_");
	ZC_replaceLines(texLines, "ZCVARLIST", varListString);
	
	StringLine* tabLines = ZC_generatePropertyAnalysisTable(caseFiles, n);
	ZC_insertLines("%create property table\n", texLines, tabLines);
	
	StringLine* figLines =  ZC_generateDataPropertyAnalysisFigures(caseFiles, n);
	ZC_insertLines("%plot data properties\n", texLines, figLines);
	
	StringLine* imgLines = ZC_generateDataSliceImages(caseFiles, n);
	ZC_insertLines("%plot slice images\n", texLines, imgLines);	
	
	ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generateCompressionRateFigure()
{
	char* cases[ZC_BUFS];
	size_t i, n = getComparisonCases(cases);
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
	ZC_insertLines("%plot compression rate\n", texLines, figLines);
	ZC_writeLines(texLines, compressionRateTexFile);
	ZC_freeLines(texLines);
}

StringLine* ZC_generateDecompressionRateFigure()
{
	char* cases[ZC_BUFS];
	size_t i, n = getComparisonCases(cases);
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
	ZC_insertLines("%plot decompression rate\n", texLines, figLines);
	ZC_writeLines(texLines, decompressionRateTexFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generatePSNRFigure()
{
	char* cases[ZC_BUFS];
	size_t i, n = getComparisonCases(cases);
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
	ZC_insertLines("%plot psnr\n", texLines, figLines);
	ZC_writeLines(texLines, psnrTexFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generateStaticAnalysisFigures(char* metricType, 
CmprsorErrBound *allCompressors, int allCompressorCount)
{
	size_t i, j, k, n = 0;// n: numOfEpsFiles
	char* selectedEpsFiles[ZC_BUFS];
	for(i=0;i<ZC_BUFS;i++)
		selectedEpsFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	char* selectedErrorBound, *varCase, *compressorName;
	CmprsorErrBound *compressor;
	
	StringLine* totalHeader = createStringLineHeader();
	StringLine* figHeader;
	char caption[ZC_BUFS], figLabel[ZC_BUFS];
	
	//construct the selected cases (eps files) based on compressors
	for(i=0;i<allCompressorCount;i++)
	{		
		compressor = allCompressors+i;
		compressorName = compressor->compressorName;
		//construct figure tex lines
		for(j=0;j<compressor->selErrBoundCount;j++)
		{
			selectedErrorBound = compressor->selErrBounds[j]->str;
			for(k=0;k<allVarCaseCount;k++)
			{
				varCase = allVarCases[k];
				
				//construct varFiles
				sprintf(selectedEpsFiles[n++], "%s(%s):%s-%s", compressorName, selectedErrorBound, varCase, metricType);
			}
			//generate figure tex lines	
			//TODO
			sprintf(caption, "%s: compressor=%s, err bound=%s", metricType, compressorName, selectedErrorBound);
			sprintf(figLabel, "%s-%s-%s", metricType, compressorName, selectedErrorBound);
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
	size_t i; 
	int varCount;
	char* varFiles[ZC_BUFS];
	char* fileName;
	//char rateDisFile[ZC_BUFS];
	
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
	ZC_insertLines("%plot rate distortion\n", texLines, figLines);
	ZC_writeLines(texLines, rateDisTexFile);
	ZC_freeLines(texLines);	
}

StringLine* ZC_generateRateCorrelationFigure()
{
	size_t i; 
	int varCount;
	char* varFiles[ZC_BUFS];
	char* fileName;
	//char rateDisFile[ZC_BUFS];

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

void ZC_generateResultTexFile()
{
	int lineCount;
	char* line = NULL;
	char resultTexFile[ZC_BUFS];
	sprintf(resultTexFile, "report/tex/results.tex");
	printf("Processing %s\n", resultTexFile);
	StringLine* texLines = ZC_readLines(resultTexFile, &lineCount);
	
	StringLine* header = createStringLineHeader();
	StringLine* p = header; //p always points to the tail
	if(compressSizeFlag)
	{	
		line = createLine("\\input{tex/resultsTex/compressionFactor}\n"); 
		p = appendOneLine(p, line);
	}
	if(psnrFlag || snrFlag)
	{	
		line = createLine("\\input{tex/resultsTex/psnr}\n"); 
		p = appendOneLine(p, line);
	}
	if(compressSizeFlag && psnrFlag)
	{
		line = createLine("\\input{tex/resultsTex/rateDistortion}\n"); 
		p = appendOneLine(p, line);
	}
	if(compressTimeFlag)
	{
		line = createLine("\\input{tex/resultsTex/compressionRate}\n"); 
		p = appendOneLine(p, line);
	}
	if(decompressTimeFlag)
	{
		line = createLine("\\input{tex/resultsTex/decompressionRate}\n"); 
		p = appendOneLine(p, line);
	}
	if(avgAbsErrFlag && plotAbsErrPDFFlag)
	{
		line = createLine("\\input{tex/resultsTex/errDistribution}\n"); 
		p = appendOneLine(p, line);
	}
	if(errAutoCorrFlag && plotErrAutoCorrFlag)
	{
		line = createLine("\\input{tex/resultsTex/errAutoCorr}\n"); 
		p = appendOneLine(p, line);
	}
	
	ZC_appendLines(texLines, header);
	ZC_writeLines(texLines, resultTexFile);
	ZC_freeLines(texLines);	
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
	ZC_insertLines("%plot rate correlation\n", texLines, figLines);
	ZC_writeLines(texLines, rateDisTexFile);
	ZC_freeLines(texLines);
}

StringLine* ZC_generateCompressionFactorFigure()
{
	char* cases[ZC_BUFS];
	size_t i, n = getComparisonCases(cases);
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
	ZC_insertLines("%plot compression factor\n", texLines, figLines);
	ZC_writeLines(texLines, compressionFactorTexFile);
	ZC_freeLines(texLines);
}

void ZC_generateErrDistributionReport(CmprsorErrBound *allCompressors, int allCompressorCount)
{
	int lineCount;
	char texFile[ZC_BUFS];
	sprintf(texFile, "report/tex/resultsTex/errDistribution.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", texFile);
	
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	
	if(absErrPDFFlag)
	{
		StringLine* figLines =  ZC_generateStaticAnalysisFigures("dis", allCompressors, allCompressorCount);
		ZC_insertLines("%plot error distribution\n", texLines, figLines);		
	}

	if(pwrErrPDFFlag)
	{
		StringLine* figLines2 = ZC_generateStaticAnalysisFigures("pds", allCompressors, allCompressorCount);
		ZC_insertLines("%plot point-wise relative error distribution\n", texLines, figLines2);		
	}
	if(absErrPDFFlag || pwrErrPDFFlag)
		ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);
}

void ZC_generateErrAutoCorrReport(CmprsorErrBound *allCompressors, int allCompressorCount)
{
	int lineCount;
	char texFile[ZC_BUFS];
	sprintf(texFile, "report/tex/resultsTex/errAutoCorr.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", texFile);
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	StringLine* figLines =  ZC_generateStaticAnalysisFigures("autocorr", allCompressors, allCompressorCount);
	ZC_insertLines("%plot error auto correlation\n", texLines, figLines);
	
	ZC_writeLines(texLines, texFile);
	ZC_freeLines(texLines);	
}

void ZC_generateSpectrumDistortionReport(CmprsorErrBound *allCompressors, int allCompressorCount)
{
	int lineCount;
	char texFile[ZC_BUFS];
	sprintf(texFile, "report/tex/resultsTex/specDistortion.tex");
	//printf("%s\n", reportTemplateDir);
	printf("Processing %s\n", texFile);
	StringLine* texLines = ZC_readLines(texFile, &lineCount);
	StringLine* figLines =  ZC_generateStaticAnalysisFigures("fft-amp", allCompressors, allCompressorCount);
	ZC_insertLines("%plot spectrum distortion\n", texLines, figLines);
	
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
	ZC_insertLines("%title_of_report\n", texLines, titleLine);
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
	
	if(compressSizeFlag)
		ZC_generateCompressionFactorReport();	
	if(compressTimeFlag)
		ZC_generateCompressionRateReport();
	if(decompressTimeFlag)
		ZC_generateDecompressionRateReport();
	if(psnrFlag || snrFlag)
		ZC_generatePSNRReport();
	if(psnrFlag && compressSizeFlag)
		ZC_generateRateDistortionReport();
	if(compressSizeFlag)
		ZC_generateRateCorrelationReport();

	int i, n = 0;
	char* caseFiles[ZC_BUFS_LONG];
	for(i=0;i<ZC_BUFS_LONG;i++)
		caseFiles[i] = (char*)malloc(sizeof(char)*ZC_BUFS);
	ZC_getFileNames("compressionResults", "dis", &n, caseFiles);
	ZC_extractCompressorAndErrorBounds(caseFiles, n);
	for(i=0;i<ZC_BUFS_LONG;i++)
		free(caseFiles[i]);
	
	ZC_constructSortedSelectedErrorBounds4CmprsEelments(allCompressors, allCompressorCount);	
	
	if(absErrPDFFlag && plotAbsErrPDFFlag)
		ZC_generateErrDistributionReport(allCompressors, allCompressorCount);
	if(errAutoCorrFlag && plotErrAutoCorrFlag)
		ZC_generateErrAutoCorrReport(allCompressors, allCompressorCount);	
	if(plotFFTAmpFlag)
		ZC_generateSpectrumDistortionReport(allCompressors, allCompressorCount);
	
	ZC_generateResultTexFile();	
	
	ZC_updateZCRootTexFile(dataSetName);
	
	//sprintf(cmd, "cd report;make clean;make");
	//printf("%s\n", cmd);
	//system(cmd);
}

