#ifndef _ZC_UTIL_H
#define _ZC_UTIL_H

#include "zc.h"

#ifdef __cplusplus
extern "C" {
#endif

char* createLine(char* str);

void ltrim(char *s);
void rtrim(char *s);
void trim(char *s);
char* rtrim_C(char *s, char c);

int ZC_executeCmd_GfloatVector(char* cmd, int* count, float** data);
int ZC_executeCmd_GdoubleVector(char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatVector(char* cmd, int* count, float** data);
int ZC_executeCmd_RdoubleVector(char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatMatrix(char* cmd, int* m, int* n, float** data);
int ZC_executeCmd_RdoubleMatrix(char* cmd, int* m, int* n, double** data);

int ZC_ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr);
int ZC_ReplaceStr2(char *sSrc, char *sMatchStr, char *sReplaceStr);

char* strcat_new(char *s1, char *s2);
void checkAndAddStringToList(char** strList, int* num, char* targetStr);
#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_UTIL_H  ----- */
