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
#include "ZC_rw.h"

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

int ZC_check_file_exists(const char *fname);
int ZC_check_eps_file_exists(char* fileNameWithoutExtension, char* dir);
int ZC_checkDirExists(const char *dir);
void updateLinkFullPath(char* oriPath, char* finalPath);
int ZC_checkExtension(char* str, char* extension);
void ZC_getFileNames(char* dir, char* extension, int *fileCount, char** fileNames);

void ZC_symTransform_8bytes(unsigned char data[8]);
void ZC_symTransform_2bytes(unsigned char data[2]);
void ZC_symTransform_4bytes(unsigned char data[4]);

size_t ZC_checkFileSize(char *srcFilePath);
unsigned char *ZC_readByteData(char *srcFilePath, size_t *byteLength);
double *ZC_readDoubleData_systemEndian(char *srcFilePath, size_t *nbEle);
float *ZC_readFloatData_systemEndian(char *srcFilePath, size_t *nbEle);
double *ZC_readDoubleData(char *srcFilePath, size_t *nbEle);
float *ZC_readFloatData(char *srcFilePath, size_t *nbEle);
void ZC_writeByteData(unsigned char *bytes, size_t byteLength, char *tgtFilePath);
void ZC_writeDoubleData(double *data, size_t nbEle, char *tgtFilePath);
void ZC_writeFloatData(float *data, size_t nbEle, char *tgtFilePath);
void ZC_writeDoubleData_withIndex(double *data, size_t nbEle, char *tgtFilePath);
void ZC_writeFloatData_withIndex(float *data, size_t nbEle, char *tgtFilePath);
void ZC_writeData_withIndex(void *data, int dataType, size_t nbEle, char *tgtFilePath);
void ZC_writeData(void *data, int dataType, size_t nbEle, char *tgtFilePath);

void ZC_writeFloatData_gnuplotImage(float *data, size_t r2, size_t r1, char *tgtFilePath);
void ZC_writeDoubleData_gnuplotImage(double *data, size_t r2, size_t r1, char *tgtFilePath);
void ZC_writeData_genuplotImage(void *data, int dataType, size_t r2, size_t r1, char *tgtFilePath);

int ZC_writeStrings(int string_size, char **string, char *tgtFilePath);

StringLine* createStringLineHeader();
StringLine* createOneStringLine(char* lineContent);
StringLine* generateOneStringLine(char* lineContent);
StringLine* appendOneLine(StringLine* tail, char* str);
void ZC_readFirstLine(char* filePath, char* line);
StringLine* ZC_readLines(char* filePath, int *lineCount);
size_t ZC_writeLines(StringLine* lineHeader, char *tgtFilePath);
void ZC_replaceLines(StringLine* originalLines, char* matchKeyword, char* replaceString);
int ZC_startsWith(char* str, char* key);
int ZC_startsWithLines(StringLine* line, char* key);
size_t ZC_insertLines(char* keyAnnotationLine, StringLine* globalLineHeader, StringLine* toAddLineHeader);
StringLine* ZC_appendLines(StringLine* globalLineHeader, StringLine* toAddLineHeader);

void ZC_removeLines(StringLine* preLine, StringLine* endingLineToRmve);
void ZC_freeLines(StringLine* header);
void ZC_freeCharArrayLines(char** lines, size_t lineNum);

char* rmFileExtension(char* fullFileName);
char* getFileName(char* fullFilePath);
char* getFileNameWithoutExtension(char* fullFilePath);

void ZC_writeFloatData_inBytes(float *data, size_t nbEle, char* tgtFilePath);
void ZC_writeDoubleData_inBytes(double *data, size_t nbEle, char* tgtFilePath);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_IO_H  ----- */
