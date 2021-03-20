#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ZC_rw.h"
#include "ZC_util.h"
#include "ZC_latex.h"
#include "zc.h"

char* gen_includegraphicsLine(char* comparisonCase, const char* subDir, const char* prefix)
{	
	size_t i = 0;
	char* tmpLine = (char*)malloc(sizeof(char)*MAX_MSG_LENGTH);
	char stringBuffer[ZC_BUFS_LONG];
	
	if(subDir==NULL)
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.5]{figs/%s", prefix);
	else
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.5]{figs/%s/%s", subDir, prefix);	
 
	char* case_ = strtok(comparisonCase, ",");
	for(i=0;case_!=NULL;i++)
	{
		sprintf(stringBuffer, "%s_%s", tmpLine, case_); 
		strcpy(tmpLine, stringBuffer);
		case_ = strtok(NULL, ",");
	}
	strcat(tmpLine, ".eps}}");
	return tmpLine;
}

char* gen_includegraphicsLine2(const char* epsFileName, const char* subDir)
{	
	char* tmpLine = (char*)malloc(sizeof(char)*MAX_MSG_LENGTH);
	//char stringBuffer[ZC_BUFS_LONG];
	
	if(subDir==NULL)
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.3,bb = 0 0 1127 293]{figs/%s.eps}}", epsFileName);
	else
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.3,bb = 0 0 1127 293]{figs/%s/%s.eps}}", subDir, epsFileName);	

	return tmpLine;
}

char* gen_includegraphicsLine3(const char* epsFileName, const char* subDir)
{	
	char* tmpLine = (char*)malloc(sizeof(char)*MAX_MSG_LENGTH);
	//char stringBuffer[ZC_BUFS_LONG];
	
	if(subDir==NULL)
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[width=.4\\textwidth]{figs/%s.png.eps}}", epsFileName);
	else
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[width=.4\\textwidth]{figs/%s/%s.png.eps}}", subDir, epsFileName);	

	return tmpLine;
}

StringLine* ZC_generateComparisonFigTexLines(int caseNum, char** cases, const char* subDir, const char* prefix, const char* caption)
{
	char caseName[ZC_BUFS_LONG];
	char tmpLine[MAX_MSG_LENGTH];
	StringLine* header = createStringLineHeader();
	StringLine* p = header; //p always points to the tail
	char* line = createLine("\\begin{figure}[ht] \\centering\n"); p = appendOneLine(p, line);
	
	int rows = caseNum/2;
	size_t i = 0,j;
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
	char caption_[100];
	strcpy(caption_, caption);
	ZC_ReplaceStr2(caption_, "_", "\\_");
	sprintf(tmpLine, "\\caption{%s}\n", caption_);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\label{fig:%s}\n", prefix);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\end{figure}\n"); p = appendOneLine(p, line);	

	return header;
}

StringLine* ZC_generateVarStatFigTexLines(int epsFileNum, char** epsFileNames, const char* subDir, const char* caption, const char* figLabel)
{
	char caseName[ZC_BUFS_LONG];
	char tmpLine[MAX_MSG_LENGTH];
	StringLine* header = createStringLineHeader();
	StringLine* p = header; //p always points to the tail
	char* line = createLine("\\begin{figure}[ht] \\centering\n"); p = appendOneLine(p, line);
	
	size_t i = 0;
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
	
	char caption_[100];
	strcpy(caption_, caption);
	ZC_ReplaceStr2(caption_, "_", "\\_");
	sprintf(tmpLine, "\\caption{%s}\n", caption_);
	
	line = createLine(tmpLine); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\label{fig:%s}\n", figLabel);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\end{figure}\n"); p = appendOneLine(p, line);	
//	line = createLine("\\clearpage\n"); p = appendOneLine(p, line);
	
	return header;
}

StringLine* ZC_generateSliceImageTexLines(int epsFileNum, char** epsFileNames, char** subFigureTitles, const char* subDir, const char* caption, const char* figLabel)
{
	char caseName[ZC_BUFS_LONG];
	char tmpLine[MAX_MSG_LENGTH];
	StringLine* header = createStringLineHeader();
	StringLine* p = header; //p always points to the tail
	char* line = createLine("\\begin{figure}[ht] \\centering\n"); p = appendOneLine(p, line);
	
	int sucCount = 0;
	size_t i = 0;
	for(i=0;i<epsFileNum;i++)
	{
		if(!ZC_check_eps_file_exists(epsFileNames[i], subDir))
			continue;
		
		sucCount++;
		strcpy(caseName, subFigureTitles[i]);
		ZC_ReplaceStr2(caseName, "_", "\\_");
		sprintf(tmpLine, "\\subfigure[{%s}]\n", caseName);
		line = createLine(tmpLine); p = appendOneLine(p, line);
		line = createLine("{\n"); p = appendOneLine(p, line);
		line = gen_includegraphicsLine3(epsFileNames[i], subDir); p = appendOneLine(p, line);
		line = createLine("}\n"); p = appendOneLine(p, line);
	}

	if(sucCount==0)
	{
		ZC_freeLines(header);
		return NULL;
	}

	line = createLine("\\vspace{-2mm}\n"); p = appendOneLine(p, line);
	char caption_[100];
	strcpy(caption_, caption);
	ZC_ReplaceStr2(caption_, "_", "\\_");
	sprintf(tmpLine, "\\caption{%s}\n", caption_);

	line = createLine(tmpLine); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\label{fig:%s}\n", figLabel);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\end{figure}\n"); p = appendOneLine(p, line);	
//	line = createLine("\\clearpage\n"); p = appendOneLine(p, line);
	
	return header;
}

StringLine* ZC_generateSimpleTableTexLines(int rows, int columns, char** cases, char** keys, char*** cells, const char* caption, const char* tabLabel)
{
	size_t i = 0, j = 0;
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
