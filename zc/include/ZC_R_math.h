/**
 *  @file ZC_R_math.h
 *  @author Sheng Di
 *  @date Jan, 2018
 *  @brief Header file for ZC_R_math.c.
 *  (C) 2018 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_R_MATH_H
#define _ZC_R_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

void ZC_R_process(char* funName, int dim, int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outLen, double** out);
double KS_test(int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
double* SSIM(int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
double* SSIM3(int dataType, void* oriData, void* decData, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_R_MATH_H  ----- */
