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
int ZC_check_eps_file_exists(const char* fileNameWithoutExtension, const char* dir);
int ZC_checkDirExists(const char *dir);
void updateLinkFullPath(const char* oriPath, char* finalPath);
int ZC_checkExtension(char* str, const char* extension);
void ZC_getFileNames(const char* dir, const char* extension, int *fileCount, char** fileNames);

void ZC_symTransform_8bytes(unsigned char data[8]);
void ZC_symTransform_2bytes(unsigned char data[2]);
void ZC_symTransform_4bytes(unsigned char data[4]);

size_t ZC_checkFileSize(const char *srcFilePath);
unsigned char *ZC_readByteData(const char *srcFilePath, size_t *byteLength);
double *ZC_readDoubleData_systemEndian(const char *srcFilePath, size_t *nbEle);
float *ZC_readFloatData_systemEndian(const char *srcFilePath, size_t *nbEle);
double *ZC_readDoubleData(const char *srcFilePath, size_t *nbEle);

float *ZC_readFloatData(const char *srcFilePath, size_t *nbEle);
void ZC_writeByteData(unsigned char *bytes, size_t byteLength, const char *tgtFilePath);
void ZC_writeDoubleData(double *data, size_t nbEle, const char *tgtFilePath);
void ZC_writeFloatData(float *data, size_t nbEle, const char *tgtFilePath);
void ZC_writeDoubleData_withIndex(double *data, size_t nbEle, const char *tgtFilePath);
void ZC_writeFloatData_withIndex(float *data, size_t nbEle, const char *tgtFilePath);
void ZC_writeData_withIndex(void *data, int dataType, size_t nbEle, const char *tgtFilePath);
void ZC_writeData(void *data, int dataType, size_t nbEle, const char *tgtFilePath);

void ZC_writeFloatData_gnuplotImage(float *data, size_t r2, size_t r1, const char *tgtFilePath);
void ZC_writeDoubleData_gnuplotImage(double *data, size_t r2, size_t r1, const char *tgtFilePath);
void ZC_writeData_genuplotImage(void *data, int dataType, size_t r2, size_t r1, const char *tgtFilePath);

int ZC_writeStrings(int string_size, char **string, const char *tgtFilePath);

StringLine* createStringLineHeader();
StringLine* createOneStringLine(char* lineContent);
StringLine* generateOneStringLine(char* lineContent);
StringLine* appendOneLine(StringLine* tail, char* str);
void ZC_readFirstLine(const char* filePath, char* line);
StringLine* ZC_readLines(const char* filePath, int *lineCount);
size_t ZC_writeLines(StringLine* lineHeader, const char *tgtFilePath);
void ZC_replaceLines(StringLine* originalLines, const char* matchKeyword, const char* replaceString);
int ZC_startsWith(char* str, const char* key);
int ZC_startsWithLines(StringLine* line, const char* key);
size_t ZC_insertLines(const char* keyAnnotationLine, StringLine* globalLineHeader, StringLine* toAddLineHeader);
StringLine* ZC_appendLines(StringLine* globalLineHeader, StringLine* toAddLineHeader);

void ZC_removeLines(StringLine* preLine, StringLine* endingLineToRmve);
void ZC_freeLines(StringLine* header);
void ZC_freeCharArrayLines(char** lines, size_t lineNum);

char* rmFileExtension(const char* fullFileName);
char* getFileName(const char* fullFilePath);
char* getFileNameWithoutExtension(const char* fullFilePath);

void ZC_writeFloatData_inBytes(float *data, size_t nbEle, const char* tgtFilePath);
void ZC_writeDoubleData_inBytes(double *data, size_t nbEle, const char* tgtFilePath);

void ZC_writeShortData_inBytes(short *states, size_t stateLength, char *tgtFilePath);
void ZC_writeUShortData_inBytes(unsigned short *states, size_t stateLength, char *tgtFilePath);
void ZC_writeIntData_inBytes(int *states, size_t stateLength, char *tgtFilePath);
void ZC_writeUIntData_inBytes(unsigned int *states, size_t stateLength, char *tgtFilePath);
void ZC_writeLongData_inBytes(long *states, size_t stateLength, char *tgtFilePath);
void ZC_writeULongData_inBytes(unsigned long *states, size_t stateLength, char *tgtFilePath);

void ZC_convertShortArrayToBytes(short* states, size_t stateLength, unsigned char* bytes);
void ZC_convertUShortArrayToBytes(unsigned short* states, size_t stateLength, unsigned char* bytes);
void ZC_convertIntArrayToBytes(int* states, size_t stateLength, unsigned char* bytes);
void ZC_convertUIntArrayToBytes(unsigned int* states, size_t stateLength, unsigned char* bytes);
void ZC_convertLongArrayToBytes(long* states, size_t stateLength, unsigned char* bytes);
void ZC_convertULongArrayToBytes(unsigned long* states, size_t stateLength, unsigned char* bytes);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_IO_H  ----- */
