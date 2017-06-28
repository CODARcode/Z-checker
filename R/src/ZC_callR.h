/**
 *  @file ZC_callR.h
 *  @author Sheng Di
 *  @date June, 2017
 *  @brief Calling R from Z-checker
 *  (C) 2017 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_CALLR_H
#define _ZC_CALLR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <Rinternals.h>
#include <Rembedded.h>


#define ZC_R_SUCCESS 0
#define ZC_R_ERROR 1

#define ZC_R_Int 0
#define ZC_R_Float 1
#define ZC_R_Double 2

void source(const char *name);

int ZC_callR_1_1d(char* rRuncName, int vecType, int inLen, void* in, int* outLen, double** out);
int ZC_callR_2_1d(char* rFuncName, int vecType, int in1Len, void* in1, int in2Len, void* in2, int* outLen, double** out);

int ZC_callR_1_2d(char* rFuncName, int vecType, int in_n2, int in_n1, void* in, int* outLen, double** out);

int ZC_callR_2_2d(char* rFuncName, int vecType, 
int in1_n2, int in1_n1, void* in1, 
int in2_n2, int in2_n1, void* in2, 
int* outLen, double** out);

int ZC_callR_1_3d(char* rFuncName, int vecType, int in_n3, int in_n2, int in_n1, void* in, int* outLen, double** out);

int ZC_callR_2_3d(char* rFuncName, int vecType, 
int in1_n3, int in1_n2, int in1_n1, void* in1, 
int in2_n3, int in2_n2, int in2_n1, void* in2, 
int* outLen, double** out);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_CALLR_H  ----- */
