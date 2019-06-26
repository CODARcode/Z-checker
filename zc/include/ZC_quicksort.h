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

size_t ZC_divide(RateDistElem* list,size_t start,size_t end);
size_t ZC_divide2(StringElem* list,size_t start,size_t end);
size_t ZC_divide3(RateDistElem* list,size_t start,size_t end);
size_t ZC_divide_vis_CR(ZC_CompareData** list,size_t start,size_t end);

void ZC_quick_sort(RateDistElem* list,size_t start,size_t end);
void ZC_quick_sort2(StringElem* list,size_t start,size_t end);
void ZC_quick_sort_vis_CR(ZC_CompareData** list,size_t start,size_t end);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_Quicksort_H  ----- */
