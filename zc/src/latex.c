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
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.5]{figs/%s", prefix);
	else
		sprintf(tmpLine, "\\raisebox{-1cm}{\\includegraphics[scale=.5]{figs/%s/%s", subDir, prefix);	
 
	char* cmprsor = strtok(comparisonCase, ",");
	for(i=0;cmprsor!=NULL;i++)
	{
		sprintf(stringBuffer, "%s_%s", tmpLine, cmprsor); 
		strcpy(tmpLine, stringBuffer);
		cmprsor = strtok(NULL, ",");
	}
	strcat(tmpLine, ".eps}}");
	return tmpLine;
}

StringLine* ZC_generateFigureTexLines(int caseNum, char** cases, char* subDir, char* prefix, char* caption)
{
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
			sprintf(tmpLine, "\\subfigure[{%s}]\n", cases[i*2+j]);
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
		sprintf(tmpLine, "\\subfigure[{%s}]\n", cases[caseNum-1]);
		line = createLine(tmpLine); p = appendOneLine(p, line);
		line = createLine("{\n"); p = appendOneLine(p, line);
		line = gen_includegraphicsLine(cases[caseNum-1], subDir, prefix); p = appendOneLine(p, line);
		line = createLine("}\n"); p = appendOneLine(p, line);
		line = createLine("\\hspace{-7mm}\n"); p = appendOneLine(p, line);		
		line = createLine("\n"); p = appendOneLine(p, line);				
	}

	line = createLine("\\vspace{-2mm}\n"); p = appendOneLine(p, line);
	sprintf(tmpLine, "\\caption{%s}", caption);
	line = createLine(tmpLine); p = appendOneLine(p, line);
	line = createLine("\\label{fig:cmprRatio}\n"); p = appendOneLine(p, line);
	line = createLine("\\end{figure}\n"); p = appendOneLine(p, line);	

	return header;
}
