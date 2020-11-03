#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ZC_util.h"
#include "ZC_gnuplot.h"
#include "zc.h"

char** genGnuplotScript_linespoints(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel)
{
	if(columns<2)
	{
		printf("Error: columns cannot be less than 2.\n");
		exit(0);
	}
	
	char** lines = (char**)malloc(24*sizeof(char*));
	char stringBuffer[ZC_BUFS_LONG];
	
	int i = 0;
	for(i=0;i<24;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "set term post eps enh \"Arial\" 22 color\n");
	sprintf(lines[1], "set output \"%s.eps\"\n", dataFileName); //the file name is without extension here
	sprintf(lines[2], "set datafile missing \"-\"\n");
	sprintf(lines[3], "set key inside top left Left reverse\n");
	sprintf(lines[4], "\n");
	sprintf(lines[5], "set auto x\n");
	sprintf(lines[6], "#set yrange [0:1]\n");
	sprintf(lines[7], "set grid y\n");
	sprintf(lines[8], "\n");
	
	sprintf(lines[9], "set style line 1 lt 1 lc rgb \"blue\" lw 5\n");
	sprintf(lines[10], "set style line 2 lt 2 lc rgb \"red\" lw 5\n");
	sprintf(lines[11], "set style line 3 lt 3 lc rgb \"black\" lw 5\n");		
	sprintf(lines[12], "set style line 4 lt 4 lc rgb \"green\" lw 5\n");
	sprintf(lines[13], "set style line 5 lt 5 lc rgb \"purple\" lw 5\n");
	sprintf(lines[14], "set style line 6 lt 6 lc rgb \"brown\" lw 5\n");
	sprintf(lines[15], "set style line 7 lt 7 lc rgb \"orange\" lw 5\n");
	sprintf(lines[16], "\n");
	
	sprintf(lines[17], "set xlabel \"%s\"\n", xlabel);	
	sprintf(lines[18], "set ylabel \"%s\"\n", ylabel);	
	sprintf(lines[19], "\n");
	
	sprintf(lines[20], "set style data linespoints\n");
	
	sprintf(lines[21], "set boxwidth 0.9\n");
	sprintf(lines[22], "set xtic rotate by -45\n");
	sprintf(lines[23], "plot '%s.%s' using 1:2 ti col ls 1", dataFileName, extension);
	
	for(i=3;i<=columns;i++)
	{
		sprintf(stringBuffer, "%s, '' u 1:%d ti col ls %d", lines[23], i, i-1);
		strcpy(lines[23], stringBuffer);
	}
	sprintf(stringBuffer, "%s\n", lines[23]);
	strcpy(lines[23], stringBuffer);
	return lines;
}


char** genGnuplotScript_linespoints_separate_datafiles(char** inputDataFileNames, int inputDataFileCount, const char* outputDataFileName, int fontSize, const char* xlabel, const char* ylabel)
{
	if(inputDataFileCount<1)
	{
		printf("Error: inputDataFileCount cannot be less than 1.\n");
		exit(0);
	}
	
	char** lines = (char**)malloc(24*sizeof(char*));
	char stringBuffer[ZC_BUFS_LONG];
	
	int i = 0;
	for(i=0;i<24;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "set term post eps enh \"Arial\" 22 color\n");
	sprintf(lines[1], "set output \"%s.eps\"\n", outputDataFileName); 
	sprintf(lines[2], "set datafile missing \"-\"\n");
	sprintf(lines[3], "set key inside top left Left reverse\n");
	sprintf(lines[4], "\n");
	sprintf(lines[5], "set auto x\n");
	sprintf(lines[6], "#set yrange [0:1]\n");
	sprintf(lines[7], "set grid y\n");
	sprintf(lines[8], "\n");
	
	sprintf(lines[9], "set style line 1 lt 1 lc rgb \"blue\" lw 5\n");
	sprintf(lines[10], "set style line 2 lt 2 lc rgb \"red\" lw 5\n");
	sprintf(lines[11], "set style line 3 lt 3 lc rgb \"black\" lw 5\n");		
	sprintf(lines[12], "set style line 4 lt 4 lc rgb \"green\" lw 5\n");
	sprintf(lines[13], "set style line 5 lt 5 lc rgb \"purple\" lw 5\n");
	sprintf(lines[14], "set style line 6 lt 6 lc rgb \"brown\" lw 5\n");
	sprintf(lines[15], "set style line 7 lt 7 lc rgb \"orange\" lw 5\n");
	sprintf(lines[16], "\n");
	
	sprintf(lines[17], "set xlabel \"%s\"\n", xlabel);	
	sprintf(lines[18], "set ylabel \"%s\"\n", ylabel);	
	sprintf(lines[19], "\n");
	
	sprintf(lines[20], "set style data linespoints\n");
	
	sprintf(lines[21], "set boxwidth 0.9\n");
	sprintf(lines[22], "set xtic rotate by -45\n");
	sprintf(stringBuffer, "plot '%s' using 1:2 ti col ls %d", inputDataFileNames[0], 1);
	strcpy(lines[23], stringBuffer);
	
	for(i=1;i<inputDataFileCount;i++)
	{
		char* inputDataFileName = inputDataFileNames[i];
		sprintf(stringBuffer, "%s, '%s' using 1:2 ti col ls %d", lines[23], inputDataFileName, i+1);
		strcpy(lines[23], stringBuffer);
	}
	
	sprintf(stringBuffer, "%s\n", lines[23]);
	strcpy(lines[23], stringBuffer);
	return lines;
}

/**
 * for plotting the 1D data series
 * */
char** genGnuplotScript_linespoints2(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel)
{
	if(columns<2)
	{
		printf("Error: columns cannot be less than 2.\n");
		exit(0);
	}
	
	char** lines = (char**)malloc(24*sizeof(char*));
	char stringBuffer[ZC_BUFS_LONG];
	
	int i = 0;
	for(i=0;i<24;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "set term post eps enh \"Arial\" 22 color\n");
	sprintf(lines[1], "set output \"%s.png.eps\"\n", dataFileName); //the file name is without extension here
	sprintf(lines[2], "set datafile missing \"-\"\n");
	sprintf(lines[3], "set key inside top left Left reverse\n");
	sprintf(lines[4], "\n");
	sprintf(lines[5], "set auto x\n");
	sprintf(lines[6], "#set yrange [0:1]\n");
	sprintf(lines[7], "set grid y\n");
	sprintf(lines[8], "\n");
	
	sprintf(lines[9], "set style line 1 lt 1 lc rgb \"blue\" lw 5\n");
	sprintf(lines[10], "set style line 2 lt 2 lc rgb \"red\" lw 5\n");
	sprintf(lines[11], "set style line 3 lt 3 lc rgb \"black\" lw 5\n");		
	sprintf(lines[12], "set style line 4 lt 4 lc rgb \"green\" lw 5\n");
	sprintf(lines[13], "set style line 5 lt 5 lc rgb \"purple\" lw 5\n");
	sprintf(lines[14], "set style line 6 lt 6 lc rgb \"brown\" lw 5\n");
	sprintf(lines[15], "set style line 7 lt 7 lc rgb \"orange\" lw 5\n");
	sprintf(lines[16], "\n");
	
	sprintf(lines[17], "set xlabel \"%s\"\n", xlabel);	
	sprintf(lines[18], "set ylabel \"%s\"\n", ylabel);	
	sprintf(lines[19], "\n");
	
	sprintf(lines[20], "set style data linespoints\n");
	
	sprintf(lines[21], "set boxwidth 0.9\n");
	sprintf(lines[22], "set xtic rotate by -45\n");
	sprintf(lines[23], "plot '%s.%s' using 1:2 ti \"\" ls 1", dataFileName, extension);
	
	for(i=3;i<=columns;i++)
	{
		sprintf(stringBuffer, "%s, '' u 1:%d ti \"\" ls %d", lines[23], i, i-1);
		strcpy(lines[23], stringBuffer);
	}
	sprintf(stringBuffer, "%s\n", lines[23]);
	strcpy(lines[23], stringBuffer);
	return lines;
}

char** genGnuplotScript_histogram(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel, long maxYValue)
{
	if(columns<2)
	{
		printf("Error: columns cannot be less than 2.\n");
		exit(0);
	}
	
	char** lines = (char**)malloc(18*sizeof(char*));
	char stringBuffer[ZC_BUFS_LONG];
	
	int i = 0;
	for(i=0;i<18;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "set term post eps enh \"Arial\" 22 color\n");
	sprintf(lines[1], "set output \"%s.eps\"\n", dataFileName); //the file name is without extension here
	sprintf(lines[2], "set datafile missing \"-\"\n");
	sprintf(lines[3], "set key inside top left Left reverse\n");
	sprintf(lines[4], "\n");
	sprintf(lines[5], "set auto x\n");
	if(maxYValue<=0)
		sprintf(lines[6], "#set yrange [0:4.5]\n");
	else
		sprintf(lines[6], "set yrange [0:%ld]\n", maxYValue);
	sprintf(lines[7], "set grid y\n");
	sprintf(lines[8], "\n");
	sprintf(lines[9], "set xlabel \"%s\"\n", xlabel);	
	sprintf(lines[10], "set ylabel \"%s\"\n", ylabel);	
	sprintf(lines[11], "\n");
	sprintf(lines[12], "set style histogram gap 5\n");
	sprintf(lines[13], "set style data histogram\n");
	
	sprintf(lines[14], "set style fill pattern border -1\n");
	sprintf(lines[15], "set boxwidth 0.9\n");
	sprintf(lines[16], "set xtic rotate by -45\n");
	sprintf(lines[17], "plot '%s.%s' using 2:xtic(1) ti col", dataFileName, extension);
	
	for(i=3;i<=columns;i++)
	{	
		sprintf(stringBuffer, "%s, '' u %d ti col", lines[17], i);
		strcpy(lines[17], stringBuffer);
	}
	
	sprintf(stringBuffer, "%s\n", lines[17]);
	strcpy(lines[17], stringBuffer);
	return lines;
}

char** genGnuplotScript_lines(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel)
{
	if(columns<2)
	{
		printf("Error: columns cannot be less than 2.\n");
		exit(0);
	}
	
	char** lines = (char**)malloc(24*sizeof(char*));
	char stringBuffer[ZC_BUFS_LONG];
	
	int i = 0;
	for(i=0;i<24;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "set term post eps enh \"Arial\" 22 color\n");
	sprintf(lines[1], "set output \"%s.eps\"\n", dataFileName); //the file name is without extension here
	sprintf(lines[2], "set datafile missing \"-\"\n");
	sprintf(lines[3], "set key inside top left Left reverse\n");
	sprintf(lines[4], "\n");
	sprintf(lines[5], "set auto x\n");
	sprintf(lines[6], "#set yrange [0:1]\n");
	sprintf(lines[7], "set grid y\n");
	sprintf(lines[8], "\n");
	
	sprintf(lines[9], "set style line 1 lt 1 lc rgb \"blue\" lw 5\n");
	sprintf(lines[10], "set style line 2 lt 2 lc rgb \"red\" lw 5\n");
	sprintf(lines[11], "set style line 3 lt 3 lc rgb \"black\" lw 5\n");		
	sprintf(lines[12], "set style line 4 lt 4 lc rgb \"green\" lw 5\n");
	sprintf(lines[13], "set style line 5 lt 5 lc rgb \"purple\" lw 5\n");
	sprintf(lines[14], "set style line 6 lt 6 lc rgb \"brown\" lw 5\n");
	sprintf(lines[15], "set style line 7 lt 7 lc rgb \"orange\" lw 5\n");
	sprintf(lines[16], "\n");
	
	sprintf(lines[17], "set xlabel \"%s\"\n", xlabel);	
	sprintf(lines[18], "set ylabel \"%s\"\n", ylabel);	
	sprintf(lines[19], "\n");
	
	sprintf(lines[20], "set style data lines\n");
	
	sprintf(lines[21], "set boxwidth 0.9\n");
	sprintf(lines[22], "set xtic rotate by -45\n");
	sprintf(lines[23], "plot '%s.%s' using 1:2 ti col ls 1", dataFileName, extension);
	
	for(i=3;i<=columns;i++)
	{
		sprintf(stringBuffer, "%s, '' u 1:%d ti col ls %d", lines[23], i, i-1);
		strcpy(lines[23], stringBuffer);
	}
	strcat(lines[23], "\n");
	return lines;
}

char** genGnuplotScript_fillsteps(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel)
{
	if(columns<2)
	{
		printf("Error: columns cannot be less than 2.\n");
		exit(0);
	}
	
	char** lines = (char**)malloc(19*sizeof(char*));
	char stringBuffer[ZC_BUFS_LONG];
	
	int i = 0;
	for(i=0;i<19;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "set term post eps enh \"Arial\" 22 color\n");
	sprintf(lines[1], "set output \"%s.%s.eps\"\n", dataFileName, extension); //the file name is without extension here
	sprintf(lines[2], "set datafile missing \"-\"\n");
	sprintf(lines[3], "set key inside top left Left reverse\n");
	sprintf(lines[4], "\n");
	sprintf(lines[5], "set auto x\n");
	sprintf(lines[6], "#set xtic 30\n");
	sprintf(lines[7], "#set xrange [0:100]\n");
	sprintf(lines[8], "#set yrange [-50000:50000]\n");
	sprintf(lines[9], "set grid y\n");
	sprintf(lines[10], "\n");
	sprintf(lines[11], "set xlabel \"%s\"\n", xlabel);	
	sprintf(lines[12], "set ylabel \"%s\"\n", ylabel);	
	sprintf(lines[13], "\n");
	
	sprintf(lines[14], "set style fill solid border -1\n");
	sprintf(lines[15], "set style data fillsteps\n");
	sprintf(lines[16], "set size 3,1\n");
	sprintf(lines[17], "set xtic rotate by -45\n");
	sprintf(lines[18], "plot '%s.%s' using 1:2 ti col ls 1", dataFileName, extension);
	
	for(i=3;i<=columns;i++)
	{
		sprintf(stringBuffer, "%s, '' u 1:%d ti col ls %d", lines[18], i, i-1);
		strcpy(lines[18], stringBuffer);
	}
	strcat(lines[18], "\n");
	return lines;
}

char** genGnuplotScript_sliceImage(const char* dataFileName, size_t r2, size_t r1)
{
	char** lines = (char**)malloc(10*sizeof(char*));
	
	int i = 0;
	for(i=0;i<10;i++)
	{
		lines[i] = (char*)malloc(ZC_BUFS_LONG);
		memset(lines[i], 0, ZC_BUFS_LONG);
	}
	sprintf(lines[0], "#!/usr/bin/gnuplot\n");
	sprintf(lines[1], "set term png size 900, 900 enhanced font \"Arial,16\"\n");
	sprintf(lines[2], "set pm3d map\n");
	sprintf(lines[3], "#set cbrange [-4:4]\n");
	sprintf(lines[4], "set output \"%s.png\"\n", dataFileName);
	sprintf(lines[5], "set size square\n");
	sprintf(lines[6], "set xrange [0:%zu]\n", r1); 
	sprintf(lines[7], "set yrange [%zu:0]\n", r2);
	sprintf(lines[8], "set palette rgbformulae 33,13,10\n");
	sprintf(lines[9], "splot \"%s\"", dataFileName);
	
	return lines;
}

void plotSliceImageData(const char* keyName, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, void* slice_data_ori, void* slice_data_log, const char* tgtWorkspaceDir)
{
	int i = 0;
	char imagPlotScriptFile[ZC_BUFS], oriImageFile[ZC_BUFS], logImageFile[ZC_BUFS];
	char tgtFilePath[ZC_BUFS];
	memset(oriImageFile, 0, ZC_BUFS);
	memset(logImageFile, 0, ZC_BUFS);

	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	if(dim == 1)
	{
		memset(tgtFilePath, 0, ZC_BUFS);	
		sprintf(oriImageFile, "%s.oriimg", keyName);
		sprintf(tgtFilePath, "%s/%s.tt", tgtWorkspaceDir, oriImageFile);			
		
		ZC_writeData_withIndex(slice_data_ori, dataType, r1, tgtFilePath); //print only first 1000 (or less if nbEle<1000) data points
		
		char** scriptLines = genGnuplotScript_linespoints2(oriImageFile, "tt", GNUPLOT_FONT, 2, "Index", "Data Values");
		sprintf(imagPlotScriptFile, "%s/%s-oriimg.p", tgtWorkspaceDir, keyName);
		ZC_writeStrings(24, scriptLines, imagPlotScriptFile);
		for(i=0;i<24;i++)
			free(scriptLines[i]);
		free(scriptLines);
		
		memset(tgtFilePath, 0, ZC_BUFS);		
		sprintf(logImageFile, "%s.logimg", keyName);
		sprintf(tgtFilePath, "%s/%s.tt", tgtWorkspaceDir, logImageFile);						
		
		ZC_writeData_withIndex(slice_data_log, dataType, r1, tgtFilePath);
		
		scriptLines = genGnuplotScript_linespoints2(logImageFile, "tt", GNUPLOT_FONT, 2, "Index", "log10(Data Values)");
		sprintf(imagPlotScriptFile, "%s/%s-logimg.p", tgtWorkspaceDir, keyName);
		ZC_writeStrings(24, scriptLines, imagPlotScriptFile);
		for(i=0;i<24;i++)
			free(scriptLines[i]);
		free(scriptLines);	
	}
	else if(dim > 1)
	{
		memset(tgtFilePath, 0, ZC_BUFS);		
		sprintf(oriImageFile, "%s.oriimg", keyName);
		sprintf(tgtFilePath, "%s/%s", tgtWorkspaceDir, oriImageFile);
		ZC_writeData_genuplotImage(slice_data_ori, dataType, r2, r1, tgtFilePath);

		memset(tgtFilePath, 0, ZC_BUFS);
		sprintf(logImageFile, "%s.logimg", keyName);
		sprintf(tgtFilePath, "%s/%s", tgtWorkspaceDir, logImageFile);
		ZC_writeData_genuplotImage(slice_data_log, dataType, r2, r1, tgtFilePath);			
		
		sprintf(imagPlotScriptFile, "%s/%s-oriimg.p", tgtWorkspaceDir, keyName);
		char** sliceImageStrs = genGnuplotScript_sliceImage(oriImageFile, r2, r1);
		ZC_writeStrings(10, sliceImageStrs, imagPlotScriptFile);
		for(i=0;i<10;i++)
			free(sliceImageStrs[i]);
		free(sliceImageStrs);
		
		sprintf(imagPlotScriptFile, "%s/%s-logimg.p", tgtWorkspaceDir, keyName);
		sliceImageStrs = genGnuplotScript_sliceImage(logImageFile, r2, r1);
		ZC_writeStrings(10, sliceImageStrs, imagPlotScriptFile);
		for(i=0;i<10;i++)
			free(sliceImageStrs[i]);
		free(sliceImageStrs);			
	}	
}

void ZC_executeCompDecomp_vis(const char* solName, const char* varName, const char* cmdTemplate, int dataType, const char* inputDataPath, const char* errorBound, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	char buffer[256];
	char cmdLine[512]; 
	strcpy(cmdLine, cmdTemplate);
	if(dataType==ZC_FLOAT)
		ZC_ReplaceStr(cmdLine, "DATATYPE", "-f");
	else
		ZC_ReplaceStr(cmdLine, "DATATYPE", "-d");
	
	int dim = ZC_computeDimension(r5, r4, r3, r2, r1);
	switch(dim)
	{
	case 1:
		sprintf(buffer, "%zu", r1);
		ZC_ReplaceStr(cmdLine, "R1", buffer);
		break;
	case 2:
		sprintf(buffer, "%zu %zu", r1, r2);
		ZC_ReplaceStr(cmdLine, "R1 R2", buffer);
		break;
	case 3:
		sprintf(buffer, "%zu %zu %zu", r1, r2, r3);
		ZC_ReplaceStr(cmdLine, "R1 R2 R3", buffer);
		break;
	}
	
	ZC_ReplaceStr(cmdLine, "SOLUTION", solName);
	ZC_ReplaceStr(cmdLine, "VARNAME", varName);
	ZC_ReplaceStr(cmdLine, "INPUTDATAPATH", inputDataPath);
	ZC_ReplaceStr(cmdLine, "ERRORBOUND", errorBound);
	
	printf("execute compression/decompression: %s\n", cmdLine);
	system(cmdLine);
}
