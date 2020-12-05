/**
 *  @file ZC_LibpressioOpt.h
 *  @author Sheng Di
 *  @date Dec., 2020
 *  @brief Header file of ZC_LibpressioOpt.c
 *  (C) 2020 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include "libpressio.h"

#ifndef _ZC_LIBPRESSIO_OPT_H
#define _ZC_LIBPRESSIO_OPT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZC_evaluation_result {
  double metric;
  double error_bound;
} ZC_evaluation_result;

struct pressio_data* ZC_constructLibPressioData(void* data, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
ZC_evaluation_result* search_cmpr_setting_with_CR(int compressor, void* data, int dataType, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, 
const float target_cr, const float lower_bound, const float upper_bound);
ZC_evaluation_result* search_error_setting_and_compress(const float target_cr, const char* compressor_id, struct pressio_data* pressioData, struct pressio_options* compressor_options, const char* input, const double lower_bound, const double upper_bound, const char* metric, double global_rel_tolerance);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_LIBPRESIO_OPT_H  ----- */

