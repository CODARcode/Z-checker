/**
 *  @file io.h
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief Header file for the whole io interface.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_IO_H
#define _ZC_IO_H

#include <stdio.h>

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StringLine
{
	char* str;
	struct StringLine* next; 
} StringLine;

void updateLinkFullPath(char* oriPath, char* finalPath);
int ZC_checkExtension(char* str, char* extension);
void ZC_getFileNames(char* dir, char* extension, int *fileCount, char** fileNames);

void ZC_symTransform_8bytes(unsigned char data[8]);
void ZC_symTransform_2bytes(unsigned char data[2]);
void ZC_symTransform_4bytes(unsigned char data[4]);

int ZC_checkFileSize(char *srcFilePath);
unsigned char *ZC_readByteData(char *srcFilePath, int *byteLength);
double *ZC_readDoubleData_systemEndian(char *srcFilePath, int *nbEle);
float *ZC_readFloatData_systemEndian(char *srcFilePath, int *nbEle);
double *ZC_readDoubleData(char *srcFilePath, int *nbEle);
float *ZC_readFloatData(char *srcFilePath, int *nbEle);
void ZC_writeByteData(unsigned char *bytes, int byteLength, char *tgtFilePath);
void ZC_writeDoubleData(double *data, int nbEle, char *tgtFilePath);
void ZC_writeFloatData(float *data, int nbEle, char *tgtFilePath);
void ZC_writeData(void *data, int dataType, int nbEle, char *tgtFilePath);

int ZC_writeStrings(int string_size, char **string, char *tgtFilePath);

StringLine* createStringLineHeader();
StringLine* createOneStringLine(char* lineContent);
StringLine* appendOneLine(StringLine* tail, char* str);
StringLine* ZC_readLines(char* filePath, int *lineCount);
int ZC_writeLines(StringLine* lineHeader, char *tgtFilePath);
void ZC_replaceLines(StringLine* originalLines, char* matchKeyword, char* replaceString);
int ZC_insertLines(char* keyAnnotationLine, StringLine* globalLineHeader, StringLine* toAddLineHeader);
void ZC_appendLines(StringLine* globalLineHeader, StringLine* toAddLineHeader);

void ZC_freeLines(StringLine* header);
void ZC_freeCharArrayLines(char** lines, int lineNum);

char* rmFileExtension(char* fullFileName);

void ZC_writeFloatData_inBytes(float *data, int nbEle, char* tgtFilePath);
void ZC_writeDoubleData_inBytes(double *data, int nbEle, char* tgtFilePath);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_IO_H  ----- */
