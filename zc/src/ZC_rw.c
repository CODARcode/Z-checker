/**
 *  @file rw.c
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief io interface for fortrance
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */
 
#include <limits.h> /*PATH_MAX*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "ZC_util.h"
#include "ZC_rw.h"
#include "zc.h"
#ifdef HAVE_ONLINEVIS
#include "zserver.h"
#endif

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

int ZC_check_eps_file_exists(const char* fileNameWithoutExtension, const char* dir)
{
	char buffer[512];
	sprintf(buffer, "%s/%s.png.eps", dir, fileNameWithoutExtension);
	return ZC_check_file_exists(buffer);
}

int ZC_checkDirExists(const char *dir)
{
	DIR* dir_ = opendir(dir);
	if (dir_) {
		/* Directory exists. */
		closedir(dir_);
		return 1;
	} else {
		return 0;
	}	
}

void updateLinkFullPath(const char* oriPath, char* finalPath)
{
	if(oriPath[0]=='/') //full path
		sprintf(finalPath, oriPath);
	else
		sprintf(finalPath, "../%s", oriPath);
}

int ZC_checkExtension(char* str, const char* extension)
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
void ZC_getFileNames(const char* dir, const char* extension, int *fileCount, char** fileNames)
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
	closedir(dir_);
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

size_t ZC_checkFileSize(const char *srcFilePath)
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

unsigned char *ZC_readByteData(const char *srcFilePath, size_t *byteLength)
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

double *ZC_readDoubleData(const char *srcFilePath, size_t *nbEle)
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

float *ZC_readFloatData(const char *srcFilePath, size_t *nbEle)
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

double *ZC_readDoubleData_systemEndian(const char *srcFilePath, size_t *nbEle)
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

float *ZC_readFloatData_systemEndian(const char *srcFilePath, size_t *nbEle)
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

void ZC_writeByteData(unsigned char *bytes, size_t byteLength, const char *tgtFilePath)
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

void ZC_writeDoubleData(double *data, size_t nbEle, const char *tgtFilePath)
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

void ZC_writeFloatData(float *data, size_t nbEle, const char *tgtFilePath)
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

void ZC_writeData_withIndex(void *data, int dataType, size_t nbEle, const char *tgtFilePath)
{
	size_t outSize = nbEle<1000? nbEle: 1000;
	if(dataType==ZC_FLOAT)
		ZC_writeFloatData_withIndex((float*)data, outSize, tgtFilePath);
	else
		ZC_writeDoubleData_withIndex((double*)data, outSize, tgtFilePath);
}

void ZC_writeDoubleData_withIndex(double *data, size_t nbEle, const char *tgtFilePath)
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
		sprintf(s, "%zu %f\n", i, data[i]);
		fputs(s, pFile);
	}
    
    fclose(pFile);
}

void ZC_writeFloatData_withIndex(float *data, size_t nbEle, const char *tgtFilePath)
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
		sprintf(s, "%zu %f\n", i, data[i]);
		fputs(s, pFile);
	}
    
    fclose(pFile);
}

void ZC_writeData(void *data, int dataType, size_t nbEle, const char *tgtFilePath)
{
	if(dataType == ZC_FLOAT)
	{
		float* dataArray = (float *)data;
		ZC_writeFloatData(dataArray, nbEle, tgtFilePath);
	}
	else if(dataType == ZC_DOUBLE)
	{
		double* dataArray = (double *)data;
		ZC_writeDoubleData(dataArray, nbEle, tgtFilePath);
	}
	else
	{
		printf("Error: data type cannot be the types other than ZC_FLOAT or ZC_DOUBLE\n");
		exit(0);	
	}
}

void ZC_writeFloatData_gnuplotImage(float *data, size_t r2, size_t r1, const char *tgtFilePath)
{
	char s[64];
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
        exit(1);
    }
   
    size_t i = 0, j = 0;
    for(i = 0;i<r2;i++)
	{
		for(j=0;j<r1;j++)
		{
			sprintf(s, "%f\n", data[i*r1+j]);
			fputs(s, pFile);			
		}
		if(i<r2-1)
			fputs("\n",pFile);
	}
    
    fclose(pFile);	
}

void ZC_writeDoubleData_gnuplotImage(double *data, size_t r2, size_t r1, const char *tgtFilePath)
{
	char s[64];
	FILE *pFile = fopen(tgtFilePath, "wb");
    if (pFile == NULL)
    {
        printf("Failed to open input file. 3\n");
        exit(1);
    }
   
    size_t i = 0, j = 0;
    for(i = 0;i<r2;i++)
	{
		for(j=0;j<r1;j++)
		{
			sprintf(s, "%f\n", data[i*r1+j]);
			fputs(s, pFile);			
		}
		if(i<r2-1)
			fputs("\n",pFile);
	}
    
    fclose(pFile);	
}

void ZC_writeData_genuplotImage(void *data, int dataType, size_t r2, size_t r1, const char *tgtFilePath)
{
	if(dataType == ZC_FLOAT)
	{
		float* dataArray = (float *)data;
		ZC_writeFloatData_gnuplotImage(dataArray, r2, r1, tgtFilePath);
	}
	else if(dataType == ZC_DOUBLE)
	{
		double* dataArray = (double *)data;
		ZC_writeDoubleData_gnuplotImage(dataArray, r2, r1, tgtFilePath);
	}
	else
	{
		printf("Error: data type cannot be the types other than ZC_FLOAT or ZC_DOUBLE\n");
		exit(0);	
	}
}

/**
 * 
 * @return the real number of elements
 * */
int ZC_writeStrings(int string_size, char **string, const char *tgtFilePath)
{
	size_t i = 0;
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
    
#ifdef HAVE_ONLINEVIS
	if(visMode) //ZC_writeStrings() is always called by rank 0 actually, so no need to add myRank==0
	{
		char actualpath[PATH_MAX+1] = {0};
        	realpath(tgtFilePath, actualpath);
		char* key = getFileName(actualpath);
		//printf("writing pair: %s, %s\n", key, actualpath);
		// zserver_commit_file(key, actualpath);		
		free(key);
	}
#endif
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

StringLine* generateOneStringLine(char* lineContent)
{
	StringLine* line = (StringLine*)malloc(sizeof(StringLine));
	line->str = lineContent;
	line->next = NULL;
	return line;
}

StringLine* appendOneLine(StringLine* tail, char* str)
{
	StringLine* newLine = (StringLine*)malloc(sizeof(StringLine));
	newLine->str = str;
	tail->next = newLine;
	newLine->next = NULL;
	return newLine;
}

void ZC_readFirstLine(const char* filePath, char* line)
{
	char buf[MAX_MSG_LENGTH] = {0};
	memset(buf, 0, MAX_MSG_LENGTH);

	FILE *fp = fopen(filePath, "r");
	if(fp == NULL)
	{
		printf("failed to open the file %s\n", filePath);
		exit(0);
	}
	
	//get the first line
	fgets(buf, MAX_MSG_LENGTH, fp); // already including \n
	strcpy(line, buf);

	fclose(fp);	
}

StringLine* ZC_readLines(const char* filePath, int *lineCount)
{
	char* buf;
	//char buf[500] = {0};

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
size_t ZC_writeLines(StringLine* lineHeader, const char *tgtFilePath)
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

void ZC_replaceLines(StringLine* originalLines, const char* matchKeyword, const char* replaceString)
{
	StringLine* p = originalLines;
	while(p->next!=NULL)
	{
		ZC_ReplaceStr2(p->next->str, matchKeyword, replaceString);
		p = p->next;
	}
}

int ZC_startsWith(char* str, const char* key)
{
    int lenkey = strlen(key), lenstr = strlen(str);
    return lenstr < lenkey ? 0 : strncmp(key, str, lenkey) == 0;
}

int ZC_startsWithLines(StringLine* line, const char* key)
{
	char* str = line->str;
	return ZC_startsWith(str, key);
}

size_t ZC_insertLines(const char* keyAnnotationLine, StringLine* globalLineHeader, StringLine* toAddLineHeader)
{
	if(toAddLineHeader==NULL)
	{
		printf("toAddLineHeader is NULL in %s\n", keyAnnotationLine);
		return 0;
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

StringLine* ZC_appendOneLine(StringLine* globalLineHeader, StringLine* stringLine)
{
	if(stringLine==NULL)
	{
		return NULL;
	}
	
	if(stringLine->next==NULL)
		return NULL;
	
	StringLine* p = globalLineHeader;
	while(p->next!=NULL)
		p = p->next;
	
	p->next = stringLine;
	
	while(p->next!=NULL)
		p = p->next;
	
	return p;	
}

/**
 * 
 * return: the tail element
 * */
StringLine* ZC_appendLines(StringLine* globalLineHeader, StringLine* toAddLineHeader)
{
	if(toAddLineHeader==NULL)
	{
		return NULL;
		//printf("Error: toAddLineHeader cannot be NULL\n");
		//exit(0);
	}
	
	if(toAddLineHeader->next==NULL)
		return NULL;
	
	StringLine* p = globalLineHeader;
	while(p->next!=NULL)
		p = p->next;
	
	p->next = toAddLineHeader->next;
	
	while(p->next!=NULL)
		p = p->next;
		
	free(toAddLineHeader);
	return p;
}

void ZC_removeLines(StringLine* preLine, StringLine* endingLineToRmve)
{
	StringLine* nextValidLine  = endingLineToRmve->next;
	endingLineToRmve->next = NULL;
	StringLine* removedLines = preLine->next;
	preLine->next = nextValidLine;
	ZC_freeLines(removedLines);
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

char* rmFileExtension(const char* fullFileName)
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

char* getFileName(const char* fullFilePath)
{
	char buf[256] = {0};
	char* s = (char*)malloc(256);
	sprintf(buf, "%s", fullFilePath);
	int i = strcspn(buf,"/");
	if(i==strlen(buf))
	{
		strcpy(s, buf);
		return s;
	}
	char* b = strrchr(buf, '/');
	strcpy(s, b+1);
	return s;
}

char* getFileNameWithoutExtension(const char* fullFilePath)
{
	char* fileName = getFileName(fullFilePath);
	char* result = rmFileExtension(fileName);
	free(fileName);
	return result;
}

void ZC_writeFloatData_inBytes(float *data, size_t nbEle, const char* tgtFilePath)
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

void ZC_writeDoubleData_inBytes(double *data, size_t nbEle, const char* tgtFilePath)
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

void ZC_writeShortData_inBytes(short *states, size_t stateLength, char *tgtFilePath)
{
	size_t byteLength = stateLength*2;
	unsigned char* bytes = (unsigned char*)malloc(byteLength*sizeof(char));
	ZC_convertShortArrayToBytes(states, stateLength, bytes);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_writeUShortData_inBytes(unsigned short *states, size_t stateLength, char *tgtFilePath)
{
	size_t byteLength = stateLength*2;
	unsigned char* bytes = (unsigned char*)malloc(byteLength*sizeof(char));
	ZC_convertUShortArrayToBytes(states, stateLength, bytes);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_writeIntData_inBytes(int *states, size_t stateLength, char *tgtFilePath)
{
	size_t byteLength = stateLength*4;
	unsigned char* bytes = (unsigned char*)malloc(byteLength*sizeof(char));
	ZC_convertIntArrayToBytes(states, stateLength, bytes);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_writeUIntData_inBytes(unsigned int *states, size_t stateLength, char *tgtFilePath)
{
	size_t byteLength = stateLength*4;
	unsigned char* bytes = (unsigned char*)malloc(byteLength*sizeof(char));
	ZC_convertUIntArrayToBytes(states, stateLength, bytes);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_writeLongData_inBytes(long *states, size_t stateLength, char *tgtFilePath)
{
	size_t byteLength = stateLength*8;
	unsigned char* bytes = (unsigned char*)malloc(byteLength*sizeof(char));
	ZC_convertLongArrayToBytes(states, stateLength, bytes);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_writeULongData_inBytes(unsigned long *states, size_t stateLength, char *tgtFilePath)
{
	size_t byteLength = stateLength*8;
	unsigned char* bytes = (unsigned char*)malloc(byteLength*sizeof(char));
	ZC_convertULongArrayToBytes(states, stateLength, bytes);
	ZC_writeByteData(bytes, byteLength, tgtFilePath);
	free(bytes);
}

void ZC_convertShortArrayToBytes(short* states, size_t stateLength, unsigned char* bytes)
{
	eclint16 ls;
	size_t i;
	if(sysEndianType==dataEndianType)
	{
		for(i=0;i<stateLength;i++)
		{
			ls.svalue = states[i];
			bytes[i*2] = ls.byte[0];
			bytes[i*2+1] = ls.byte[1];
		}		
	}
	else
	{
		for(i=0;i<stateLength;i++)
		{
			ls.svalue = states[i];
			bytes[i*2] = ls.byte[1];
			bytes[i*2+1] = ls.byte[0];
		}			
	}
}

void ZC_convertUShortArrayToBytes(unsigned short* states, size_t stateLength, unsigned char* bytes)
{
	eclint16 ls;
	size_t i;
	if(sysEndianType==dataEndianType)
	{
		for(i=0;i<stateLength;i++)
		{
			ls.usvalue = states[i];
			bytes[i*2] = ls.byte[0];
			bytes[i*2+1] = ls.byte[1];
		}		
	}
	else
	{
		for(i=0;i<stateLength;i++)
		{
			ls.usvalue = states[i];
			bytes[i*2] = ls.byte[1];
			bytes[i*2+1] = ls.byte[0];
		}			
	}
}

void ZC_convertIntArrayToBytes(int* states, size_t stateLength, unsigned char* bytes)
{
	eclint32 ls;
	size_t index = 0;
	size_t i;
	if(sysEndianType==dataEndianType)
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 2; //==i*4
			ls.ivalue = states[i];
			bytes[index] = ls.byte[0];
			bytes[index+1] = ls.byte[1];
			bytes[index+2] = ls.byte[2];
			bytes[index+3] = ls.byte[3];
		}		
	}
	else
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 2; //==i*4
			ls.ivalue = states[i];
			bytes[index] = ls.byte[3];
			bytes[index+1] = ls.byte[2];
			bytes[index+2] = ls.byte[1];
			bytes[index+3] = ls.byte[0];
		}			
	}
}

void ZC_convertUIntArrayToBytes(unsigned int* states, size_t stateLength, unsigned char* bytes)
{
	eclint32 ls;
	size_t index = 0;
	size_t i;
	if(sysEndianType==dataEndianType)
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 2; //==i*4
			ls.uivalue = states[i];
			bytes[index] = ls.byte[0];
			bytes[index+1] = ls.byte[1];
			bytes[index+2] = ls.byte[2];
			bytes[index+3] = ls.byte[3];
		}		
	}
	else
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 2; //==i*4
			ls.uivalue = states[i];
			bytes[index] = ls.byte[3];
			bytes[index+1] = ls.byte[2];
			bytes[index+2] = ls.byte[1];
			bytes[index+3] = ls.byte[0];
		}			
	}
}

void ZC_convertLongArrayToBytes(long* states, size_t stateLength, unsigned char* bytes)
{
	eclint64 ls;
	size_t index = 0;
	size_t i;
	if(sysEndianType==dataEndianType)
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 3; //==i*8
			ls.lvalue = states[i];
			bytes[index] = ls.byte[0];
			bytes[index+1] = ls.byte[1];
			bytes[index+2] = ls.byte[2];
			bytes[index+3] = ls.byte[3];
			bytes[index+4] = ls.byte[4];
			bytes[index+5] = ls.byte[5];
			bytes[index+6] = ls.byte[6];
			bytes[index+7] = ls.byte[7];	
		}		
	}
	else
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 3; //==i*8
			ls.lvalue = states[i];
			bytes[index] = ls.byte[7];
			bytes[index+1] = ls.byte[6];
			bytes[index+2] = ls.byte[5];
			bytes[index+3] = ls.byte[4];
			bytes[index+4] = ls.byte[3];
			bytes[index+5] = ls.byte[2];
			bytes[index+6] = ls.byte[1];
			bytes[index+7] = ls.byte[0];	
		}			
	}
}

void ZC_convertULongArrayToBytes(unsigned long* states, size_t stateLength, unsigned char* bytes)
{
	eclint64 ls;
	size_t index = 0;
	size_t i;
	if(sysEndianType==dataEndianType)
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 3; //==i*8
			ls.ulvalue = states[i];
			bytes[index] = ls.byte[0];
			bytes[index+1] = ls.byte[1];
			bytes[index+2] = ls.byte[2];
			bytes[index+3] = ls.byte[3];
			bytes[index+4] = ls.byte[4];
			bytes[index+5] = ls.byte[5];
			bytes[index+6] = ls.byte[6];
			bytes[index+7] = ls.byte[7];			
		}		
	}
	else
	{
		for(i=0;i<stateLength;i++)
		{
			index = i << 3; //==i*8
			ls.ulvalue = states[i];
			bytes[index] = ls.byte[7];
			bytes[index+1] = ls.byte[6];
			bytes[index+2] = ls.byte[5];
			bytes[index+3] = ls.byte[4];
			bytes[index+4] = ls.byte[3];
			bytes[index+5] = ls.byte[2];
			bytes[index+6] = ls.byte[1];
			bytes[index+7] = ls.byte[0];	
		}			
	}
}
