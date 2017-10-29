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

char* gen_includegraphicsLine(char* comparisonCase, char* subDir, char* prefix);
char* gen_includegraphicsLine2(char* epsFileName, char* subDir);
StringLine* ZC_generateComparisonFigTexLines(int caseNum, char** cases, char* subDir, char* prefix, char* caption);
StringLine* ZC_generateVarStatFigTexLines(int epsFileNum, char** epsFileNames, char* subDir, char* caption, char* figLabel);
StringLine* ZC_generateSimpleTableTexLines(int rows, int columns, char** cases, char** keys, char*** cells, char* caption, char* tabLabel);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _LATEX_H  ----- */
