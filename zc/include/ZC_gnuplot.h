/**
 *  @file ZC_gnuplot.h
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief Header file for the ZC_gnuplot.c.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_Gnuplot_H
#define _ZC_Gnuplot_H

#include <stdio.h>

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

#ifdef __cplusplus
extern "C" {
#endif

char** genGnuplotScript_linespoints(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);
char** genGnuplotScript_linespoints2(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);
char** genGnuplotScript_histogram(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel, long maxYValue);
char** genGnuplotScript_lines(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);
char** genGnuplotScript_fillsteps(char* dataFileName, char* extension, int fontSize, int columns, char* xlabel, char* ylabel);
char** genGnuplotScript_sliceImage(char* dataFileName, size_t r2, size_t r1);
void plotSliceImageData(char* keyName, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, void* slice_data_ori, void* slice_data_log, char* tgtWorkspaceDir);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_Gnuplot_H  ----- */
