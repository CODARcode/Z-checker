#ifndef _ZC_UTIL_H
#define _ZC_UTIL_H

#include "zc.h"

#ifdef __cplusplus
extern "C" {
#endif

void removeExtension2(char* mystr);

int startsWith(const char *pre, const char *str);
char* createLine(char* str);

void ltrim(char *s);
void rtrim(char *s);
void trim(char *s);
char* rtrim_C(char *s, char c);

int ZC_ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr);
int ZC_ReplaceStr2(char *sSrc, char *sMatchStr, char *sReplaceStr);

char* strcat_new(char *s1, char *s2);
void checkAndAddCmprorToList(CmprsorErrBound* compressorList, int* num, char* compressorName, char* errBound);
void checkAndAddStringToList(char** strList, int* num, char* targetStr);

int ZC_parseCompressionCase(char* compressionCase, char* compressorName, char* errorBound, char* varName);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_UTIL_H  ----- */
