#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "zc.h"

char** genGnuplotScript_linespoints(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel)
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
		lines[i] = (char*)malloc(250);
		memset(lines[i], 0, 250);
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

char** genGnuplotScript_histogram(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel, long maxYValue)
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
		lines[i] = (char*)malloc(250);
		memset(lines[i], 0, 250);
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

char** genGnuplotScript_lines(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel)
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
		lines[i] = (char*)malloc(250);
		memset(lines[i], 0, 250);
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


char** genGnuplotScript_fillsteps(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel)
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
		lines[i] = (char*)malloc(250);
		memset(lines[i], 0, 250);
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
