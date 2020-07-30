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

char** genGnuplotScript_linespoints(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel);
char** genGnuplotScript_linespoints_separate_datafiles(char** inputDataFileNames, int inputDataFileCount, const char* outputDataFileName, int fontSize, const char* xlabel, const char* ylabel);
char** genGnuplotScript_linespoints2(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel);
char** genGnuplotScript_histogram(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel, long maxYValue);
char** genGnuplotScript_lines(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel);
char** genGnuplotScript_fillsteps(const char* dataFileName, const char* extension, int fontSize, int columns, const char* xlabel, const char* ylabel);
char** genGnuplotScript_sliceImage(const char* dataFileName, size_t r2, size_t r1);
void plotSliceImageData(const char* keyName, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, void* slice_data_ori, void* slice_data_log, const char* tgtWorkspaceDir);
void ZC_executeCompDecomp_vis(const char* solName, const char* varName, const char* cmdTemplate, int dataType, const char* inputDataPath, const char* errorBound, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_Gnuplot_H  ----- */
