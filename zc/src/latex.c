#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zc.h"

char* gen_includegraphicsLine(char* comparisonCase, char* subDir, char* prefix)
{	
	int i = 0;
	char* tmpLine = (char*)malloc(sizeof(char)*MAX_MSG_LENGTH);
	char stringBuffer[ZC_BUFS_LONG];
	
	if(subDir==NULL)
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.5]{figs/{%s", prefix);
	else
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.5]{figs/%s/{%s", subDir, prefix);	
 
	char* case_ = strtok(comparisonCase, ",");
	for(i=0;case_!=NULL;i++)
	{
		sprintf(stringBuffer, "%s_%s", tmpLine, case_); 
		strcpy(tmpLine, stringBuffer);
		case_ = strtok(NULL, ",");
	}
	strcat(tmpLine, "}.eps}}");
	return tmpLine;
}

char* gen_includegraphicsLine2(char* epsFileName, char* subDir)
{	
	int i = 0;
	char* tmpLine = (char*)malloc(sizeof(char)*MAX_MSG_LENGTH);
	char stringBuffer[ZC_BUFS_LONG];
	
	if(subDir==NULL)
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.3,bb = 54 51 1127 293]{figs/{%s}.eps}}", epsFileName);
	else
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.3,bb = 54 51 1127 293]{figs/%s/{%s}.eps}}", subDir, epsFileName);	

	return tmpLine;
}

StringLine* ZC_generateComparisonFigTexLines(int caseNum, char** cases, char* subDir, char* prefix, char* caption)
{
	char caseName[ZC_BUFS_LONG];
	char tmpLine[MAX_MSG_LENGTH];
	StringLine* header = createStringLineHeader();
	StringLine* p = header; //p always points to the tail
	char* line = createLine("\\begin{figure}[ht] \\centering\n"); p = appendOneLine(p, line);
	
	int rows = caseNum/2;
	int i = 0,j;
	for(i=0;i<rows;i++)
	{
		line = createLine("\\hspace{-7mm}\n"); 
		p = appendOneLine(p, line);
		for(j=0;j<2;j++)
		{
			strcpy(caseName, cases[i*2+j]);
			ZC_ReplaceStr2(caseName, "_", "\\_");
			sprintf(tmpLine, "\\subfigure[{%s}]\n", caseName);
			line = createLine(tmpLine); p = appendOneLine(p, line);
			line = createLine("{\n"); p = appendOneLine(p, line);
			line = gen_includegraphicsLine(cases[i*2+j], subDir, prefix); p = appendOneLine(p, line);
			line = createLine("}\n"); p = appendOneLine(p, line);
			line = createLine("\\hspace{-7mm}\n"); p = appendOneLine(p, line);
		}
		line = createLine("\n"); p = appendOneLine(p, line);		
	}
	if(caseNum%2==1)
	{
		strcpy(caseName, cases[caseNum-1]);
		ZC_ReplaceStr2(caseName, "_", "\\_");
		sprintf(tmpLine, "\\subfigure[{%s}]\n", caseName);
		line = createLine(tmpLine); p = appendOneLine(p, line);
		line = createLine("{\n"); p = appendOneLine(p, line);
		line = gen_includegraphicsLine(cases[caseNum-1], subDir, prefix); p = appendOneLine(p, line);
		line = createLine("}\n"); p = appendOneLine(p, line);
		line = createLine("\\hspace{-7mm}\n"); p = appendOneLine(p, line);		
		line = createLine("\n"); p = appendOneLine(p, line);				
	}

	line = createLine("\\vspace{-2mm}\n"); p = appendOneLine(p, line);
	ZC_ReplaceStr2(caption, "_", "\\_");
	sprintf(tmpLine, "\\caption{%s}\n", caption);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\label{fig:%s}\n", prefix);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\end{figure}\n"); p = appendOneLine(p, line);	

	return header;
}

StringLine* ZC_generateVarStatFigTexLines(int epsFileNum, char** epsFileNames, char* subDir, char* caption, char* figLabel)
{
	char caseName[ZC_BUFS_LONG];
	char tmpLine[MAX_MSG_LENGTH];
	StringLine* header = createStringLineHeader();
	StringLine* p = header; //p always points to the tail
	char* line = createLine("\\begin{figure}[ht] \\centering\n"); p = appendOneLine(p, line);
	
	int i = 0;
	for(i=0;i<epsFileNum;i++)
	{
		strcpy(caseName, epsFileNames[i]);
		ZC_ReplaceStr2(caseName, "_", "\\_");
		sprintf(tmpLine, "\\subfigure[{%s}]\n", caseName);
		line = createLine(tmpLine); p = appendOneLine(p, line);
		line = createLine("{\n"); p = appendOneLine(p, line);
		line = gen_includegraphicsLine2(epsFileNames[i], subDir); p = appendOneLine(p, line);
		line = createLine("}\n"); p = appendOneLine(p, line);
	}

	line = createLine("\\vspace{-2mm}\n"); p = appendOneLine(p, line);
	ZC_ReplaceStr2(caption, "_", "\\_");
	sprintf(tmpLine, "\\caption{%s}\n", caption);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\label{fig:%s}\n", figLabel);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\end{figure}\n"); p = appendOneLine(p, line);	
	line = createLine("\\clearpage\n"); p = appendOneLine(p, line);
	
	return header;
}

StringLine* ZC_generateSimpleTableTexLines(int rows, int columns, char** cases, char** keys, char*** cells, char* caption, char* tabLabel)
{
	int i = 0, j = 0;
	char tmpLine[MAX_MSG_LENGTH], buf[MAX_MSG_LENGTH], caseName[MAX_MSG_LENGTH];	
	StringLine* header = createStringLineHeader();
	StringLine* p = header;
	char* line = createLine("\\begin{table*}[ht]\n"); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\caption{%s} \\centering\n", caption);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\scriptsize\n"); p = appendOneLine(p, line);
	strcpy(tmpLine, "\\begin{tabular}{|c");
	for(i=0;i<columns;i++)
		strcat(tmpLine, "|c");
	strcat(tmpLine, "|}\n");
	line = createLine(tmpLine); p = appendOneLine(p, line);
	
	sprintf(tmpLine, "\\hline & \\textbf{%s}", keys[0]);	
	for(i=1;i<columns;i++)
	{
		sprintf(buf, "%s & \\textbf{%s}", tmpLine, keys[i]);
		strcpy(tmpLine, buf);
	}
	strcat(tmpLine, "\\\\ \n");
	line = createLine(tmpLine); p = appendOneLine(p, line);
	
	for(i=0;i<rows;i++)
	{
		strcpy(caseName, cases[i]);
		ZC_ReplaceStr2(caseName, "_", "\\_");
		sprintf(tmpLine, "\\hline %s & %s", caseName, cells[i][0]);
		for(j=1;j<columns;j++)
		{
			sprintf(buf, "%s & %s", tmpLine, cells[i][j]);
			strcpy(tmpLine, buf);
		}
		strcat(tmpLine, " \\\\\n");
		line = createLine(tmpLine); 
		p = appendOneLine(p, line);
	}
	
	line = createLine("\\hline\n"); p = appendOneLine(p, line);
	line = createLine("\\end{tabular}\n"); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\label{%s}\n", tabLabel); 
	line = createLine(tmpLine);p = appendOneLine(p, line);
	line = createLine("\\end{table*}\n"); p = appendOneLine(p, line);
	
	return header;
}
