#include <stdio.h>
#include <stdlib.h>
#include "zc.h"

void ltrim(char *s)
{
	char *p;
	p = s;
	while(*p == ' ' || *p == '\t')
	{
		p++;
	}
	memcpy(s, p, strlen(p)+1);
//	strcpy(s,p);
}

void rtrim(char *s)
{
	int i;

	i = strlen(s)-1;
	while((s[i] == ' ' || s[i] == '\t') && i >= 0){i--;};
	s[i+1] = '\0';
}

void trim(char *s)
{
	ltrim(s);
	rtrim(s);
}

int ZC_executeCmd_GfloatVector(char* cmd, int* count, float** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicFloatArray* dfa;
	float value;
	new_DFA(&dfa, DynamicArrayInitLength);
	
	if((fp = popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		return ZC_CMDE;
	}
	while(fgets(buf, CMD_OUTPUT_BUF, fp) != NULL)
	{
		value=atof(buf);
		addDFA_Data(dfa, value);
	}
	pclose(fp);
	*count = dfa->size;
	if(dfa->size>0)
	{
		*data = (float*)malloc(dfa->size*sizeof(float));
		memcpy(*data, dfa->array, dfa->size*sizeof(float));
	}
	else
	{
		*data = NULL;
	}
	free_DFA(dfa);
	return ZC_SCES;
}

int ZC_executeCmd_GdoubleVector(char* cmd, int* count, double** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicDoubleArray* dda;
	double value;
	new_DDA(&dda, DynamicArrayInitLength);
	
	if((fp = popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		return ZC_CMDE;
	}
	while(fgets(buf, CMD_OUTPUT_BUF, fp) != NULL)
	{
		value=atof(buf);
		addDDA_Data(dda, value);
	}
	pclose(fp);
	*count = dda->size;
	if(dda->size>0)
	{
		*data = (double*)malloc(dda->size*sizeof(double));
		memcpy(*data, dda->array, dda->size*sizeof(double));
	}
	else
	{
		*data = NULL;
	}
	free_DDA(dda);
	return ZC_SCES;
}

int ZC_executeCmd_RfloatVector(char* cmd, int* count, float** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	float value;
	
	if((fp = popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		return ZC_NSCS;
	}

	char* delim = "\t ";
	char* p = NULL;
	if(fgets(buf, CMD_OUTPUT_BUF, fp)!=NULL)
	{
		DynamicFloatArray* dfa;		
		new_DFA(&dfa, DynamicArrayInitLength);		
		ltrim(buf);
		p=strtok(buf,delim);
		while(p)
		{
			p = strtok(NULL,delim);
			if(p!=NULL)
			{
				value = atof(p);
				addDFA_Data(dfa, value);	
			}
		}
		*count = dfa->size;
		*data = (float*)malloc(dfa->size*sizeof(float));
		memcpy(*data, dfa->array, dfa->size*sizeof(float));
		free_DFA(dfa);		
	}
	else
	{
		*count = 0;
		*data = NULL;
	}
	pclose(fp);
	return ZC_SCES;
}

int ZC_executeCmd_RdoubleVector(char* cmd, int* count, double** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	double value;
	
	if((fp = popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		return ZC_NSCS;
	}

	char* delim = "\t ";
	char* p = NULL;
	if(fgets(buf, CMD_OUTPUT_BUF, fp)!=NULL)
	{
		DynamicDoubleArray* dda;		
		new_DDA(&dda, DynamicArrayInitLength);		
		ltrim(buf);
		p=strtok(buf,delim);
		while(p)
		{
			p = strtok(NULL,delim);
			if(p!=NULL)
			{
				value = atof(p);
				addDDA_Data(dda, value);			
			}
		}
		*count = dda->size;
		*data = (double*)malloc(dda->size*sizeof(double));
		memcpy(*data, dda->array, dda->size*sizeof(double));
		free_DDA(dda);		
	}
	else
	{
		*count = 0;
		*data = NULL;
	}
	pclose(fp);
	return ZC_SCES;
}

int ZC_executeCmd_RfloatMatrix(char* cmd, int* m, int* n, float** data)
{
	int status = ZC_SCES;
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicFloatArray* dfa;
	float value;
	new_DFA(&dfa, DynamicArrayInitLength);
	
	if((fp = popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		free_DFA(dfa);
		return ZC_CMDE;
	}
	int i = 0, j = 0;
	char* delim = "\t ";
	char* p = NULL;
	for(i=0;fgets(buf, CMD_OUTPUT_BUF, fp) != NULL;i++)
	{
		ltrim(buf);
		p=strtok(buf,delim);

		j = 0;
		for(j=0;p;j++)
		{
			p = strtok(NULL,delim);
			if(p!=NULL)
			{
				if(i!=0)
				{
					value = atof(p);
					addDFA_Data(dfa, value);
				}
			}
		}
	}
	
	if(i==0)
	{
		*m = 0;
		*data = NULL;
		status = ZC_NSCS;
	}
	else
	{
		*m = dfa->size/i; //number of rows
		*n = i;
		*data = (float*)malloc(dfa->size*sizeof(float));
		memcpy(*data, dfa->array, dfa->size*sizeof(float));
		status = ZC_SCES;		
	}
	free_DFA(dfa);
	pclose(fp);
	return status;
}

int ZC_executeCmd_RdoubleMatrix(char* cmd, int* m, int* n, double** data)
{
	int status = ZC_SCES;
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicDoubleArray* dda;
	double value;
	new_DDA(&dda, DynamicArrayInitLength);
	
	if((fp = popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		free_DDA(dda);
		return ZC_CMDE;
	}
	int i = 0, j = 0;
	char* delim = "\t ";
	char* p = NULL;
	for(i=0;fgets(buf, CMD_OUTPUT_BUF, fp) != NULL;i++)
	{
		ltrim(buf);
		p=strtok(buf,delim);

		j = 0;
		for(j=0;p;j++)
		{
			p = strtok(NULL,delim);
			if(p!=NULL)
			{
				if(i!=0)
				{
					value = atof(p);
					addDDA_Data(dda, value);
				}
			}
		}
	}
	
	if(i==0)
	{
		*m = 0;
		*data = NULL;
		status = ZC_NSCS;
	}
	else
	{
		*m = dda->size/i; //number of rows
		*n = i;
		*data = (double*)malloc(dda->size*sizeof(double));
		memcpy(*data, dda->array, dda->size*sizeof(double));
		status = ZC_SCES;		
	}
	free_DDA(dda);
	pclose(fp);
	return status;
}
