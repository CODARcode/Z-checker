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

#include <stdint.h>
#include <Rinternals.h>
#include <Rembedded.h>
#include "ZC_manyArgs.h"

#define ZC_R_SUCCESS 0
#define ZC_R_ERROR 1

#define ZC_R_Float 0
#define ZC_R_Double 1
#define ZC_R_Int 2

void source(const char *name);

int ZC_callR_1_1d(char* rRuncName, int vecType, size_t inLen, void* in, size_t* outLen, double** out);
int ZC_callR_2_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t* outLen, double** out);

int ZC_callR_1_2d(char* rFuncName, int vecType, size_t in_n2, size_t in_n1, void* in, size_t* outLen, double** out);

int ZC_callR_2_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t* outLen, double** out);

int ZC_callR_1_3d(char* rFuncName, int vecType, size_t in_n3, size_t in_n2, size_t in_n1, void* in, size_t* outLen, double** out);

int ZC_callR_2_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t* outLen, double** out);

int ZC_callR_3_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* int3, size_t* outLen, double** out);
int ZC_callR_4_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* in3, size_t in4Len, void* in4, size_t* outLen, double** out);
int ZC_callR_5_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* in3, size_t in4Len, void* in4, size_t in5Len, void* in5, size_t* outLen, double** out);
int ZC_callR_6_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* in3, size_t in4Len, void* in4, size_t in5Len, void* in5, size_t in6Len, void* in6, size_t* outLen, double** out);

int ZC_callR_3_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t* outLen, double** out);

int ZC_callR_4_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n2, size_t in4_n1, void* in4, 
size_t* outLen, double** out);

int ZC_callR_5_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n2, size_t in4_n1, void* in4, 
size_t in5_n2, size_t in5_n1, void* in5,
size_t* outLen, double** out);

int ZC_callR_6_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n2, size_t in4_n1, void* in4, 
size_t in5_n2, size_t in5_n1, void* in5,
size_t in6_n2, size_t in6_n1, void* in6,
size_t* outLen, double** out);

int ZC_callR_3_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t* outLen, double** out);

int ZC_callR_4_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n3, size_t in4_n2, size_t in4_n1, void* in4,
size_t* outLen, double** out);

int ZC_callR_5_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n3, size_t in4_n2, size_t in4_n1, void* in4,
size_t in5_n3, size_t in5_n2, size_t in5_n1, void* in5,
size_t* outLen, double** out);

int ZC_callR_6_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n3, size_t in4_n2, size_t in4_n1, void* in4,
size_t in5_n3, size_t in5_n2, size_t in5_n1, void* in5,
size_t in6_n3, size_t in6_n2, size_t in6_n1, void* in6,
size_t* outLen, double** out);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_CALLR_H  ----- */
