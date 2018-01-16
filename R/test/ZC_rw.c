/**+
 *  @file rw.c
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief io interface for fortrance
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include "ZC_rw.h"
#include "ZC_callR.h"

int dataEndianType;
int sysEndianType;

int ZC_ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr)
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
int ZC_ReplaceStr2(char *sSrc, char *sMatchStr, char *sReplaceStr)
{
	ZC_ReplaceStr(sSrc, sMatchStr, "!");
	ZC_ReplaceStr(sSrc, "!", sReplaceStr);	
	return 0;
}

int ZC_check_file_exists(const char *fname)
{
    FILE *file;
    if ((file = fopen(fname, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}

void updateLinkFullPath(char* oriPath, char* finalPath)
{
	if(oriPath[0]=='/') //full path
		finalPath = oriPath;
	else
		sprintf(finalPath, "../%s", oriPath);
}

int ZC_checkExtension(char* str, char* extension)
{
	char* l = strrchr(str, '.');
	if(l==NULL)
		return 0;
	if(strcmp(l+1,extension)==0)
		return 1;
	else
		return 0;
}

/**
 * 
 * Note: char** fileNames should be allocated beforehand before calling this function.
 * */
void ZC_getFileNames(char* dir, char* extension, int *fileCount, char** fileNames)
{
	size_t i;
	
	if(access(dir,0)!=0)
	{
		*fileCount = 0;
		return;
	}
		
	DIR *dir_;
	struct dirent *ptr;
	
	dir_ = opendir(dir);
	i = 0;
	while((ptr=readdir(dir_))!=NULL)
	{
		if(i==1000)
		{
			printf("Error: File Count >=1000, but we can only read the first 1000 files.\n");
			break;
		}
		if(ZC_checkExtension(ptr->d_name, extension))
		{
			memcpy(fileNames[i], ptr->d_name, strlen(ptr->d_name)+1);
			i++;
		}
	}
	*fileCount = i;
}


void ZC_symTransform_8bytes(unsigned char data[8])
{
	unsigned char tmp = data[0];
	data[0] = data[7];
	data[7] = tmp;

	tmp = data[1];
	data[1] = data[6];
	data[6] = tmp;

	tmp = data[2];
	data[2] = data[5];
	data[5] = tmp;

	tmp = data[3];
	data[3] = data[4];
	data[4] = tmp;
}

void ZC_symTransform_2bytes(unsigned char data[2])
{
	unsigned char tmp = data[0];
	data[0] = data[1];
	data[1] = tmp;
}

void ZC_symTransform_4bytes(unsigned char data[4])
{
	unsigned char tmp = data[0];
	data[0] = data[3];
	data[3] = tmp;

	tmp = data[1];
	data[1] = data[2];
	data[2] = tmp;
}

size_t ZC_checkFileSize(char *srcFilePath)
{
	size_t filesize;
	FILE *pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
	{
		printf("Failed to open input file. 1\n");
		exit(1);
	}
	fseek(pFile, 0, SEEK_END);
    filesize = ftell(pFile);
    fclose(pFile);
    return filesize;
}

unsigned char *ZC_readByteData(char *srcFilePath, size_t *byteLength)
{
	FILE *pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 1\n");
        exit(1);
    }
	fseek(pFile, 0, SEEK_END);
    *byteLength = ftell(pFile);
    fclose(pFile);
    
    unsigned char *byteBuf = ( unsigned char *)malloc((*byteLength)*sizeof(unsigned char)); //sizeof(char)==1
    
    pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 2\n");
        exit(1);
    }
    fread(byteBuf, 1, *byteLength, pFile);
    fclose(pFile);
    return byteBuf;
}

double *ZC_readDoubleData(char *srcFilePath, size_t *nbEle)
{
	if(dataEndianType==sysEndianType)
	{
		double *daBuf = ZC_readDoubleData_systemEndian(srcFilePath, nbEle);
		return daBuf;
	}
	else
	{
		size_t i,j;
		
		size_t byteLength;
		unsigned char* bytes = ZC_readByteData(srcFilePath, &byteLength);
		double *daBuf = (double *)malloc(byteLength);
		*nbEle = byteLength/8;
		
		ecldouble buf;
		for(i = 0;i<*nbEle;i++)
		{
			j = i*8;
			memcpy(buf.byte, bytes+j, 8);
			ZC_symTransform_8bytes(buf.byte);
			daBuf[i] = buf.value;
		}
		free(bytes);
		return daBuf;
	}
}

float *ZC_readFloatData(char *srcFilePath, size_t *nbEle)
{
	if(dataEndianType==sysEndianType)
	{
		float *daBuf = ZC_readFloatData_systemEndian(srcFilePath, nbEle);
		return daBuf;
	}
	else
	{
		size_t i,j;
		
		size_t byteLength;
		unsigned char* bytes = ZC_readByteData(srcFilePath, &byteLength);
		float *daBuf = (float *)malloc(byteLength);
		*nbEle = byteLength/4;
		
		eclfloat buf;
		for(i = 0;i<*nbEle;i++)
		{
			j = i*4;
			memcpy(buf.byte, bytes+j, 4);
			ZC_symTransform_4bytes(buf.byte);
			daBuf[i] = buf.value;
		}
		free(bytes);
		return daBuf;
	}
}

double *ZC_readDoubleData_systemEndian(char *srcFilePath, size_t *nbEle)
{
	size_t inSize;
	FILE *pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 1\n");
        exit(1);
    }
	fseek(pFile, 0, SEEK_END);
    inSize = ftell(pFile);
    *nbEle = inSize/8; //only support double in this version
    fclose(pFile);
    
    double *daBuf = (double *)malloc(inSize);
    
    pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 2\n");
        exit(1);
    }
    fread(daBuf, 8, *nbEle, pFile);
    fclose(pFile);
    return daBuf;
}

float *ZC_readFloatData_systemEndian(char *srcFilePath, size_t *nbEle)
{
	size_t inSize;
	FILE *pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 1\n");
        exit(1);
    }
	fseek(pFile, 0, SEEK_END);
    inSize = ftell(pFile);
    *nbEle = inSize/4; 
    fclose(pFile);
    
    if(inSize<=0)
    {
		printf("Error: input file is wrong!\n");
		exit(0);
	}
    
    float *daBuf = (float *)malloc(inSize);
    
    pFile = fopen(srcFilePath, "rb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 2\n");
        exit(1);
    }
    fread(daBuf, 4, *nbEle, pFile);
    fclose(pFile);
    return daBuf;
}

void ZC_writeByteData(unsigned char *bytes, size_t byteLength, char *tgtFilePath)
{
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
		exit(0);
    }
    
    fwrite(bytes, 1, byteLength, pFile); //write outSize bytes
    fclose(pFile);
}

void ZC_writeDoubleData(double *data, size_t nbEle, char *tgtFilePath)
{
	size_t i = 0;
	char s[64];
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
        exit(1);
    }
    
    for(i = 0;i<nbEle;i++)
	{
		//sprintf(s,"%.20G\n",data[i]);
		sprintf(s, "%f", data[i]);
		fputs(s, pFile);
	}
    
    fclose(pFile);
}

void ZC_writeFloatData(float *data, size_t nbEle, char *tgtFilePath)
{
	size_t i = 0;
	char s[64];
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
        exit(1);
    }
   
    for(i = 0;i<nbEle;i++)
	{
		//printf("i=%d\n",i);
		//printf("data[i]=%f\n",data[i]);
		sprintf(s, "%f", data[i]);
		fputs(s, pFile);
	}
    
    fclose(pFile);
}

void ZC_writeData(void *data, int dataType, size_t nbEle, char *tgtFilePath)
{
	if(dataType == ZC_R_Float)
	{
		float* dataArray = (float *)data;
		ZC_writeFloatData(dataArray, nbEle, tgtFilePath);
	}
	else if(dataType == ZC_R_Double)
	{
		double* dataArray = (double *)data;
		ZC_writeDoubleData(dataArray, nbEle, tgtFilePath);
	}
	else
	{
		printf("Error: data type cannot be the types other than ZC_R_Float or ZC_R_Double\n");
		exit(0);	
	}
}

/**
 * 
 * @return the real number of elements
 * */
int ZC_writeStrings(int string_size, char **string, char *tgtFilePath)
{
	size_t i = 0;
	char s[ZC_BUFS];
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
        exit(1);
    }
   
    for(i = 0;i<string_size;i++)
	{
		if(string[i]==0)
			break;
		fputs(string[i], pFile);
	}
    
    fclose(pFile);
    return i;	
}

StringLine* createStringLineHeader()
{
	StringLine* header = (StringLine*)malloc(sizeof(StringLine));
	header->str = NULL;
	header->next = NULL;
	return header;
}

StringLine* createOneStringLine(char* lineContent)
{
	StringLine* header = createStringLineHeader();
	appendOneLine(header, lineContent);
	return header;
}

StringLine* appendOneLine(StringLine* tail, char* str)
{
	StringLine* newLine = (StringLine*)malloc(sizeof(StringLine));
	newLine->str = str;
	tail->next = newLine;
	newLine->next = NULL;
	return newLine;
}

StringLine* ZC_readLines(char* filePath, int *lineCount)
{
	char* buf;
	//char buf[500] = {0};
	int len = 0;

	FILE *fp = fopen(filePath, "r");
	if(fp == NULL)
	{
		printf("failed to open the file %s\n", filePath);
		exit(0);
	}

	StringLine *header = createStringLineHeader();
	StringLine *tail = header; //the last element
	
	size_t i = 0;
	while(!feof(fp))
	{
		buf = (char*)malloc(MAX_MSG_LENGTH);
		memset(buf, 0, MAX_MSG_LENGTH);
		fgets(buf, MAX_MSG_LENGTH, fp); // already including \n
		tail = appendOneLine(tail, buf);
		i++;
		//lines[i++] = buf;
		//printf("%s", szTest);
	}
	*lineCount = i;

	fclose(fp);	
	return header;
}

/**
 * 
 * @return the final number of lines
 * */
size_t ZC_writeLines(StringLine* lineHeader, char *tgtFilePath)
{
	size_t i = 0;
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
        exit(1);
    }
   
    StringLine* p = lineHeader;
    for(i = 0;p->next!=NULL;i++)
	{
		p = p->next;
		fputs(p->str, pFile);
	}
    
    fclose(pFile);
    return i;	
}

void ZC_replaceLines(StringLine* originalLines, char* matchKeyword, char* replaceString)
{
	StringLine* p = originalLines;
	while(p->next!=NULL)
	{
		ZC_ReplaceStr2(p->next->str, matchKeyword, replaceString);
		p = p->next;
	}
}

size_t ZC_insertLines(char* keyAnnotationLine, StringLine* globalLineHeader, StringLine* toAddLineHeader)
{
	if(toAddLineHeader==NULL)
	{
		printf("Error: toAddLineHeader cannot be NULL\n");
		exit(0);
	}
	
	if(toAddLineHeader->next==NULL)
		return 0;
	
	StringLine* p = globalLineHeader->next, *q;
	size_t count = 0;
	while(p!=NULL)
	{
		if(strcmp(keyAnnotationLine, p->str)==0)
		{
			q = p->next;
			p->next = toAddLineHeader->next;
			free(toAddLineHeader);
			
			while(p->next!=NULL)
			{
				p = p->next;
				count++;
			}
			p->next = q;
			break;
		}
		else
			p = p->next;
	}
	return count;
}

void ZC_appendLines(StringLine* globalLineHeader, StringLine* toAddLineHeader)
{
	if(toAddLineHeader==NULL)
	{
		printf("Error: toAddLineHeader cannot be NULL\n");
		exit(0);
	}
	
	if(toAddLineHeader->next==NULL)
		return;
	
	StringLine* p = globalLineHeader, *q;
	int count = 0;
	while(p->next!=NULL)
		p = p->next;
	
	p->next = toAddLineHeader->next;	
	free(toAddLineHeader);
}

void ZC_freeLines(StringLine* header)
{
	StringLine *p = header, *q;
	while(p!=NULL)
	{
		if(p->str!=NULL)
		{
			free(p->str);
		}
		q = p->next;
		free(p);
		p = q;
	}
}

void ZC_freeCharArrayLines(char** lines, size_t lineNum)
{
	size_t i = 0;
	for(;i<lineNum;i++)
		free(lines[i]);
	free(lines);
}

char* rmFileExtension(char* fullFileName)
{
	char* s = (char*)malloc(ZC_BUFS);
	sprintf(s, "%s", fullFileName);
	int i = strcspn(s,".");
	if(i==strlen(s)) //if s doesn't contain '.'
		return s;
	char* b = strrchr(s, '.');
	b[0] = '\0';
	return s;
}

void ZC_writeFloatData_inBytes(float *data, size_t nbEle, char* tgtFilePath)
{
	size_t i = 0;
	eclfloat buf;
	unsigned char* bytes = (unsigned char*)malloc(nbEle*sizeof(float));
	for(i=0;i<nbEle;i++)
	{
		buf.value = data[i];
		bytes[i*4+0] = buf.byte[0];
		bytes[i*4+1] = buf.byte[1];
		bytes[i*4+2] = buf.byte[2];
		bytes[i*4+3] = buf.byte[3];					
	}

	size_t byteLength = nbEle*sizeof(float);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_writeDoubleData_inBytes(double *data, size_t nbEle, char* tgtFilePath)
{
	size_t i = 0, index = 0;
	ecldouble buf;
	unsigned char* bytes = (unsigned char*)malloc(nbEle*sizeof(double));
	for(i=0;i<nbEle;i++)
	{
		index = i*8;
		buf.value = data[i];
		bytes[index+0] = buf.byte[0];
		bytes[index+1] = buf.byte[1];
		bytes[index+2] = buf.byte[2];
		bytes[index+3] = buf.byte[3];
		bytes[index+4] = buf.byte[4];
		bytes[index+5] = buf.byte[5];
		bytes[index+6] = buf.byte[6];
		bytes[index+7] = buf.byte[7];
	}

	size_t byteLength = nbEle*sizeof(double);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}
