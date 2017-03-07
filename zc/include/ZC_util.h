#ifndef _ZC_UTIL_H
#define _ZC_UTIL_H

#include "zc.h"

#ifdef __cplusplus
extern "C" {
#endif

void ltrim(char *s);
void rtrim(char *s);
void trim(char *s);

int ZC_executeCmd_GfloatVector(char* cmd, int* count, float** data);
int ZC_executeCmd_GdoubleVector(char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatVector(char* cmd, int* count, float** data);
int ZC_executeCmd_RdoubleVector(char* cmd, int* count, double** data);
int ZC_executeCmd_RfloatMatrix(char* cmd, int* m, int* n, float** data);
int ZC_executeCmd_RdoubleMatrix(char* cmd, int* m, int* n, double** data);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_UTIL_H  ----- */
