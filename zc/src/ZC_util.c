#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "zc.h"
#include "ZC_util.h"

//0 means false, 1 means matching.
int startsWith(const char *pre, const char *str)
{
    int lenpre = strlen(pre); 
    int lenstr = strlen(str);
    if(lenstr < lenpre)
		return 0;
	int r = strncmp(pre, str, lenpre);
	if(r==0)
		return 1;
	else
		return 0;
}

/*char *removeExtension(char* mystr) {
    char *retstr;
    char *lastdot;
    if (mystr == NULL)
         return NULL;
    if ((retstr = malloc (strlen (mystr) + 1)) == NULL)
        return NULL;
    strcpy (retstr, mystr);
    lastdot = strrchr (retstr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
    return retstr;
}*/

void removeExtension2(char* mystr) {
    char* lastdot = strrchr (mystr, '.');
    if (lastdot != NULL)
        *lastdot = '\0';
}

char* createLine(const char* str)
{
	char* line = (char*)malloc(sizeof(char)*(strlen(str)+1));
	strcpy(line, str);
	return line;
}

void ltrim(char *s)
{
	char *p;
	p = s;
	if(p==NULL)
		return;
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
	if(s==NULL)
		return;
	i = strlen(s)-1;
	while((s[i] == ' ' || s[i] == '\t' || s[i] == '\n') && i >= 0){i--;};
	s[i+1] = '\0';
}

void trim(char *s)
{
	ltrim(s);
	rtrim(s);
}

char* rtrim_C(char *s, char c)
{
	int len = strlen(s);
	char* result = (char*)malloc(len+1);
	strcpy(result, s);
	if(result[len-1]==c)
		result[len-1] = '\0';
	return result;
}


int ZC_executeCmd_GfloatVector(const char* cmd, int* count, float** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicFloatArray* dfa;
	float value;
	new_DFA(&dfa, DynamicArrayInitLength);
	
	if((fp = (FILE *)popen(cmd, "r")) == NULL)
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

int ZC_executeCmd_GdoubleVector(const char* cmd, int* count, double** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicDoubleArray* dda;
	double value;
	new_DDA(&dda, DynamicArrayInitLength);
	
	if((fp = (FILE *)popen(cmd, "r")) == NULL)
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

int ZC_executeCmd_RfloatVector(const char* cmd, int* count, float** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	float value;
	
	if((fp = (FILE *)popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		return ZC_NSCS;
	}

	char delim[3];
	strcpy(delim, "\t ");
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

int ZC_executeCmd_RdoubleVector(const char* cmd, int* count, double** data)
{
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	double value;
	
	if((fp = (FILE *)popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		return ZC_NSCS;
	}

	char delim[3];
	strcpy(delim, "\t ");
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

int ZC_executeCmd_RfloatMatrix(const char* cmd, int* m, int* n, float** data)
{
	int status = ZC_SCES;
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicFloatArray* dfa;
	float value;
	new_DFA(&dfa, DynamicArrayInitLength);
	
	if((fp = (FILE *)popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		free_DFA(dfa);
		return ZC_CMDE;
	}
	size_t i = 0, j = 0;
	char delim[3];
	strcpy(delim, "\t ");
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

int ZC_executeCmd_RdoubleMatrix(const char* cmd, int* m, int* n, double** data)
{
	int status = ZC_SCES;
	FILE *fp;
	char buf[CMD_OUTPUT_BUF] = {0};
	DynamicDoubleArray* dda;
	double value;
	new_DDA(&dda, DynamicArrayInitLength);
	
	if((fp = (FILE *)popen(cmd, "r")) == NULL)
	{
		perror("Fail to popen\n");
		free_DDA(dda);
		return ZC_CMDE;
	}
	size_t i = 0, j = 0;
	char delim[3];
	strcpy(delim, "\t ");
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

int ZC_ReplaceStr(char *sSrc, const char *sMatchStr, const char *sReplaceStr)
{
	int  StringLen;
	char caNewString[MAX_MSG_LENGTH];

	char *FindPos = strstr(sSrc, sMatchStr);
	if( (!FindPos) || (!sMatchStr) )
		return -1;

	while( FindPos )
	{
		memset(caNewString, 0, sizeof(caNewString));
		StringLen = FindPos - sSrc;
		strncpy(caNewString, sSrc, StringLen);
		strcat(caNewString, sReplaceStr);
		strcat(caNewString, FindPos + strlen(sMatchStr));
		strcpy(sSrc, caNewString);

		FindPos = strstr(sSrc, sMatchStr);
	}

	return 0;
}

/**
 * 
 * This function is used to deal with the case that sMatchStr overlaps with sReplaceStr.
 * For example, ZC_ReplaceStr2("test_1", "_", "\\\\_");
 * Note: the sSrc, sMatchStr, and SReplaceStr should not contain "!".
 * */
int ZC_ReplaceStr2(char *sSrc, const char *sMatchStr, const char *sReplaceStr)
{
	ZC_ReplaceStr(sSrc, sMatchStr, "!");
	ZC_ReplaceStr(sSrc, "!", sReplaceStr);	
	return 0;
}

char* strcat_new(const char *s1, const char *s2)
{
    char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
    //in real code you would check for errors in malloc here
    if (result == NULL) exit (1);

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}

void checkAndAddCmprorToList(CmprsorErrBound* compressorList, int* num, const char* compressorName, const char* errBound)
{
	int mark = 0;
	size_t i = 0, j = 0, errBoundIndex = 0;
	//check if the compressorList contains compressorName
	for(i=0;i<*num;i++)
	{
		if(strcmp(compressorList[i].compressorName, compressorName)==0)
		{
			mark = 1;
			break;
		}
	}
	if(mark==0)
	{
		i = *num;		
		strcpy(compressorList[i].compressorName, compressorName);
		compressorList[i].allErrBounds = (StringElem*)malloc(sizeof(StringElem)*ERRBOUND_MAX_LEN);
		compressorList[i].selErrBounds = (StringElem*)malloc(sizeof(StringElem)*ERRBOUND_MAX_LEN);		
		(*num)++;
	}
	//add errBound to the compressorElement if missing.
	mark = 0;
	for(j=0;j < compressorList[i].allErrBoundCount;j++)
	{
		if(strcmp(compressorList[i].allErrBounds[j]->str, errBound)==0)
		{
			mark = 1;
			break;
		}
	}
	if(mark==0)
	{
		errBoundIndex = compressorList[i].allErrBoundCount;
		compressorList[i].allErrBounds[errBoundIndex] = (StringElem)malloc(sizeof(struct StringElem_t));
		compressorList[i].allErrBounds[errBoundIndex]->str = (char*)malloc(sizeof(char)*ERRBOUND_STR_BUFSIZE);
		strcpy(compressorList[i].allErrBounds[errBoundIndex]->str, errBound);
		compressorList[i].allErrBounds[errBoundIndex]->value = atof(errBound);		
		compressorList[i].allErrBoundCount ++;		
	}
}

void checkAndAddStringToList(char** strList, int* num, const char* targetStr)
{
	size_t i = 0;
	int mark = 0;
	for(i=0;i<*num;i++)
	{
		if(strcmp(strList[i], targetStr)==0)
		{
			mark = 1;
			break;
		}
	}
	if(mark==0)
	{
		char* tmp = (char*)malloc(strlen(targetStr)+1);
		strcpy(tmp, targetStr);
		strList[*num] = tmp;
		(*num)++;
	}
}

int ZC_parseCompressionCase(char* compressionCase, char* compressorName, char* errorBound, char* varName)
{
        char* q = NULL;
        
		char delim[3], delim2[3], delim3[3];
		strcpy(delim, ":");
		strcpy(delim2, "(");
		strcpy(delim3, ")");       

        char first[256];

		char tmp[256];
		strcpy(tmp, compressionCase);
        char* p=strtok(tmp,delim);

        strcpy(first, p);
        p = strtok(NULL,delim);
        strcpy(varName,p);

        p = strtok(first, delim2);
        strcpy(compressorName, p);
        while(p)
        {
                p = strtok(NULL,delim2);
                if(p!=NULL)
                {
                        q = strtok(p, delim3);
                        strcpy(errorBound, q);
                        break;
                }
        }
        return 0;
}
