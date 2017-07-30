/**
 *  @file ZC_quicksort.h
 *  @author Sheng Di
 *  @date April, 2015
 *  @brief Header file for the ZC_quicksort.c.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_Quicksort_H
#define _ZC_Quicksort_H

#include <stdio.h>
#include "zc.h"

#ifdef _WIN32
#define PATH_SEPARATOR ';'
#else
#define PATH_SEPARATOR ':'
#endif

#ifdef __cplusplus
extern "C" {
#endif

int ZC_divide(RateDistElem* list,int start,int end);
void ZC_quick_sort(RateDistElem* list,int start,int end);
int ZC_divide2(StringElem* list,int start,int end);
void ZC_quick_sort2(StringElem* list,int start,int end);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_Quicksort_H  ----- */
