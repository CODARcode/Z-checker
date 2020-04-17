/**
 *  @file io.h
 *  @author Sheng Di
 *  @date April, 2017
 *  @brief Header file for generating latex text.
 *  (C) 2017 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _LATEX_H
#define _LATEX_H

#include "ZC_rw.h"
#include "zc.h"

#ifdef __cplusplus
extern "C" {
#endif

char* gen_includegraphicsLine(char* comparisonCase, const char* subDir, const char* prefix);
char* gen_includegraphicsLine2(const char* epsFileName, const char* subDir);
char* gen_includegraphicsLine3(const char* epsFileName, const char* subDir);
StringLine* ZC_generateComparisonFigTexLines(int caseNum, char** cases, const char* subDir, const char* prefix, const char* caption);
StringLine* ZC_generateVarStatFigTexLines(int epsFileNum, char** epsFileNames, const char* subDir, const char* caption, const char* figLabel);
StringLine* ZC_generateSliceImageTexLines(int epsFileNum, char** epsFileNames, char** subFigureTitles, const char* subDir, const char* caption, const char* figLabel);
StringLine* ZC_generateSimpleTableTexLines(int rows, int columns, char** cases, char** keys, char*** cells, const char* caption, const char* tabLabel);


#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _LATEX_H  ----- */
