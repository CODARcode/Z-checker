/**
 *  @file ZC_callR.c
 *  @author Sheng Di
 *  @date June, 2017
 *  @brief Calling R from Z-checker
 *  (C) 2017 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Rinternals.h>
#include <Rembedded.h>

#include "ZC_manyArgs.h"
#include "ZC_callR.h"

/**
 * Invokes the command source("func.R").
 */
void source(const char *name)
{
    SEXP e;

    PROTECT(e = lang2(install("source"), mkString(name)));
    R_tryEval(e, R_GlobalEnv, NULL);
    UNPROTECT(1);
}


int ZC_callR_1_1d(char* rFuncName, int vecType, size_t inLen, void* in, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg;	
    float *float_in;
    double *double_in;
    size_t i;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg = allocVector(INTSXP, inLen));
		memcpy(INTEGER(arg), (int*)in, inLen* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in = (float*)in;
		double_in = (double*)malloc(inLen*sizeof(double));
		for(i=0;i<inLen;i++)
			double_in[i] = float_in[i];
		PROTECT(arg = allocVector(REALSXP, inLen));
		memcpy(REAL(arg), double_in, inLen* sizeof(double));
		break;
	case ZC_R_Double:
		PROTECT(arg = allocVector(REALSXP, inLen));
		memcpy(REAL(arg), (double*)in, inLen* sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang2(install(rFuncName), arg));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(2);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(2);
        return ZC_R_ERROR;
    }
}

int ZC_callR_2_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2;
    float *float_in1, *float_in2;
    double *double_in1, *double_in2;
    size_t i;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocVector(INTSXP, in1Len));
		memcpy(INTEGER(arg1), (int*)in1, in1Len* sizeof(int));
		PROTECT(arg2 = allocVector(INTSXP, in2Len));
		memcpy(INTEGER(arg2), (int*)in2, in2Len* sizeof(int));
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		double_in1 = (double*)malloc(in1Len*sizeof(double));
		double_in2 = (double*)malloc(in2Len*sizeof(double));
		for(i=0;i<in1Len;i++)
			double_in1[i] = float_in1[i];
		for(i=0;i<in2Len;i++)
			double_in2[i] = float_in2[i];
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		memcpy(REAL(arg1), double_in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2Len* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		memcpy(REAL(arg1), (double*)in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2Len* sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang3(install(rFuncName), arg1, arg2));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(3);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(3);
        return ZC_R_ERROR;
    }
}

int ZC_callR_1_2d(char* rFuncName, int vecType, size_t in_n2, size_t in_n1, void* in, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg;	
    float *float_in;
    double *double_in;
    size_t i, j;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg = allocMatrix(INTSXP, in_n2, in_n1));
		memcpy(INTEGER(arg), (int*)in, in_n2*in_n1* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in = (float*)in;
		double_in = (double*)malloc(in_n2*in_n1*sizeof(double));
		for(i=0;i<in_n2;i++)
			for(j=0;j<in_n1;j++)
				double_in[i*in_n1+j] = float_in[i*in_n1+j];
		PROTECT(arg = allocMatrix(REALSXP, in_n2, in_n1));
		memcpy(REAL(arg), double_in, in_n2*in_n1* sizeof(double));
		break;
	case ZC_R_Double:
		PROTECT(arg = allocMatrix(REALSXP, in_n2, in_n1));
		memcpy(REAL(arg), (double*)in, in_n2*in_n1*sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang2(install(rFuncName), arg));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(2);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(2);
        return ZC_R_ERROR;
    }
}

int ZC_callR_2_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2;
    float *float_in1, *float_in2;
    double *double_in1, *double_in2;
    size_t i,j;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocMatrix(INTSXP, in1_n2, in1_n1));
		memcpy(INTEGER(arg1), (int*)in1, in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = allocMatrix(INTSXP, in2_n2, in2_n1));
		memcpy(INTEGER(arg2), (int*)in2, in2_n2*in2_n1* sizeof(int));
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		double_in1 = (double*)malloc(in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n2*in2_n1*sizeof(double));
		for(i=0;i<in1_n2;i++)
			for(j=0;j<in1_n1;j++)
				double_in1[i*in1_n1+j] = float_in1[i*in1_n1+j];
		for(i=0;i<in2_n2;i++)
			for(j=0;j<in2_n1;j++)
				double_in2[i*in2_n1+j] = float_in2[i*in2_n1+j];
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		memcpy(REAL(arg1), double_in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n2	*in2_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		memcpy(REAL(arg1), (double*)in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n2*in2_n1* sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang3(install(rFuncName), arg1, arg2));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(3);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(3);
        return ZC_R_ERROR;
    }
}

int ZC_callR_1_3d(char* rFuncName, int vecType, size_t in_n3, size_t in_n2, size_t in_n1, void* in, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg;	
    float *float_in;
    double *double_in;
    size_t i, j, k, n21;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg = alloc3DArray(INTSXP, in_n2, in_n1, in_n3));
		memcpy(INTEGER(arg), (int*)in, in_n3*in_n2*in_n1* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in = (float*)in;
		double_in = (double*)malloc(in_n3*in_n2*in_n1*sizeof(double));
		n21 = in_n2*in_n1;
		for(i=0;i<in_n3;i++)
			for(j=0;j<in_n2;j++)
				for(k=0;k<in_n1;k++)
					double_in[i*n21+j*in_n1+k] = float_in[i*n21+j*in_n1+k];
		PROTECT(arg = alloc3DArray(REALSXP, in_n2, in_n1, in_n3));
		memcpy(REAL(arg), double_in, in_n3*in_n2*in_n1* sizeof(double));
		break;
	case ZC_R_Double:
		PROTECT(arg = alloc3DArray(REALSXP, in_n2, in_n1, in_n3));
		memcpy(REAL(arg), (double*)in, in_n3*in_n2*in_n1*sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang2(install(rFuncName), arg));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(2);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(2);
        return ZC_R_ERROR;
    }
}

int ZC_callR_2_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2;
    float *float_in1, *float_in2;
    double *double_in1, *double_in2;
    size_t i,j,k;
	size_t in1_n21 = in1_n2*in1_n1;
	size_t in2_n21 = in2_n2*in2_n1;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = alloc3DArray(INTSXP, in1_n2, in1_n1, in1_n3));
		memcpy(INTEGER(arg1), (int*)in1, in1_n3*in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = alloc3DArray(INTSXP, in2_n2, in2_n1, in2_n3));
		memcpy(INTEGER(arg2), (int*)in2, in2_n3*in2_n2*in2_n1* sizeof(int));
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		double_in1 = (double*)malloc(in1_n3*in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n3*in2_n2*in2_n1*sizeof(double));
		
		for(i=0;i<in1_n3;i++)
			for(j=0;j<in1_n2;j++)
				for(k=0;k<in1_n1;k++)
					double_in1[i*in1_n21+j*in1_n1+k] = float_in1[i*in1_n21+j*in1_n1+k];
		for(i=0;i<in2_n3;i++)
			for(j=0;j<in2_n2;j++)
				for(k=0;k<in2_n1;k++)
					double_in2[i*in2_n21+j*in2_n1+k] = float_in2[i*in2_n21+j*in2_n1+k];
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		memcpy(REAL(arg1), double_in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n3*in2_n2*in2_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		memcpy(REAL(arg1), (double*)in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n3*in2_n2*in2_n1* sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang3(install(rFuncName), arg1, arg2));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(3);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(3);
        return ZC_R_ERROR;
    }
}


int ZC_callR_3_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* in3, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3;
    float *float_in1, *float_in2, *float_in3;
    double *double_in1, *double_in2, *double_in3;
    size_t i;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocVector(INTSXP, in1Len));
		memcpy(INTEGER(arg1), (int*)in1, in1Len* sizeof(int));
		PROTECT(arg2 = allocVector(INTSXP, in2Len));
		memcpy(INTEGER(arg2), (int*)in2, in2Len* sizeof(int));
		PROTECT(arg3 = allocVector(INTSXP, in3Len));
		memcpy(INTEGER(arg3), (int*)in3, in3Len* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		double_in1 = (double*)malloc(in1Len*sizeof(double));
		double_in2 = (double*)malloc(in2Len*sizeof(double));
		double_in3 = (double*)malloc(in3Len*sizeof(double));
		for(i=0;i<in1Len;i++)
			double_in1[i] = float_in1[i];
		for(i=0;i<in2Len;i++)
			double_in2[i] = float_in2[i];
		for(i=0;i<in3Len;i++)
			double_in3[i] = float_in3[i];
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		memcpy(REAL(arg1), double_in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2Len* sizeof(double));		
		memcpy(REAL(arg3), double_in3, in3Len* sizeof(double));
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		memcpy(REAL(arg1), (double*)in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2Len* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3Len* sizeof(double));
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang4(install(rFuncName), arg1, arg2, arg3));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(4);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(4);
        return ZC_R_ERROR;
    }
}

int ZC_callR_4_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* in3, size_t in4Len, void* in4, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4;
    float *float_in1, *float_in2, *float_in3, *float_in4;
    double *double_in1, *double_in2, *double_in3, *double_in4;
    size_t i;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocVector(INTSXP, in1Len));
		memcpy(INTEGER(arg1), (int*)in1, in1Len* sizeof(int));
		PROTECT(arg2 = allocVector(INTSXP, in2Len));
		memcpy(INTEGER(arg2), (int*)in2, in2Len* sizeof(int));
		PROTECT(arg3 = allocVector(INTSXP, in3Len));
		memcpy(INTEGER(arg3), (int*)in3, in3Len* sizeof(int));	
		PROTECT(arg4 = allocVector(INTSXP, in4Len));
		memcpy(INTEGER(arg4), (int*)in4, in4Len* sizeof(int));				
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		double_in1 = (double*)malloc(in1Len*sizeof(double));
		double_in2 = (double*)malloc(in2Len*sizeof(double));
		double_in3 = (double*)malloc(in3Len*sizeof(double));
		double_in4 = (double*)malloc(in4Len*sizeof(double));
		for(i=0;i<in1Len;i++)
			double_in1[i] = float_in1[i];
		for(i=0;i<in2Len;i++)
			double_in2[i] = float_in2[i];
		for(i=0;i<in3Len;i++)
			double_in3[i] = float_in3[i];
		for(i=0;i<in4Len;i++)
			double_in4[i] = float_in4[i];			
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		PROTECT(arg4 = allocVector(REALSXP, in4Len));
		memcpy(REAL(arg1), double_in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2Len* sizeof(double));		
		memcpy(REAL(arg3), double_in3, in3Len* sizeof(double));
		memcpy(REAL(arg4), double_in4, in4Len* sizeof(double));
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		PROTECT(arg4 = allocVector(REALSXP, in4Len));
		memcpy(REAL(arg1), (double*)in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2Len* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3Len* sizeof(double));
		memcpy(REAL(arg4), (double*)in4, in4Len* sizeof(double));
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang5(install(rFuncName), arg1, arg2, arg3, arg4));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(5);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(5);
        return ZC_R_ERROR;
    }
}

int ZC_callR_5_1d(char* rFuncName, int vecType, size_t in1Len, void* in1, size_t in2Len, void* in2, size_t in3Len, void* in3, size_t in4Len, void* in4, size_t in5Len, void* in5, size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4, arg5;
    float *float_in1, *float_in2, *float_in3, *float_in4, *float_in5;
    double *double_in1, *double_in2, *double_in3, *double_in4, *double_in5;
    size_t i;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocVector(INTSXP, in1Len));
		memcpy(INTEGER(arg1), (int*)in1, in1Len* sizeof(int));
		PROTECT(arg2 = allocVector(INTSXP, in2Len));
		memcpy(INTEGER(arg2), (int*)in2, in2Len* sizeof(int));
		PROTECT(arg3 = allocVector(INTSXP, in3Len));
		memcpy(INTEGER(arg3), (int*)in3, in3Len* sizeof(int));	
		PROTECT(arg4 = allocVector(INTSXP, in4Len));
		memcpy(INTEGER(arg4), (int*)in4, in4Len* sizeof(int));
		PROTECT(arg5 = allocVector(INTSXP, in5Len));
		memcpy(INTEGER(arg5), (int*)in5, in5Len* sizeof(int));
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		float_in5 = (float*)in5;
		double_in1 = (double*)malloc(in1Len*sizeof(double));
		double_in2 = (double*)malloc(in2Len*sizeof(double));
		double_in3 = (double*)malloc(in3Len*sizeof(double));
		double_in4 = (double*)malloc(in4Len*sizeof(double));
		double_in5 = (double*)malloc(in5Len*sizeof(double));
		for(i=0;i<in1Len;i++)
			double_in1[i] = float_in1[i];
		for(i=0;i<in2Len;i++)
			double_in2[i] = float_in2[i];
		for(i=0;i<in3Len;i++)
			double_in3[i] = float_in3[i];
		for(i=0;i<in4Len;i++)
			double_in4[i] = float_in4[i];
		for(i=0;i<in5Len;i++)
			double_in5[i] = float_in5[i];
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		PROTECT(arg4 = allocVector(REALSXP, in4Len));
		PROTECT(arg5 = allocVector(REALSXP, in5Len));
		memcpy(REAL(arg1), double_in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2Len* sizeof(double));		
		memcpy(REAL(arg3), double_in3, in3Len* sizeof(double));
		memcpy(REAL(arg4), double_in4, in4Len* sizeof(double));
		memcpy(REAL(arg5), double_in5, in5Len* sizeof(double));
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		PROTECT(arg4 = allocVector(REALSXP, in4Len));
		PROTECT(arg5 = allocVector(REALSXP, in5Len));
		memcpy(REAL(arg1), (double*)in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2Len* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3Len* sizeof(double));
		memcpy(REAL(arg4), (double*)in4, in4Len* sizeof(double));
		memcpy(REAL(arg5), (double*)in5, in5Len* sizeof(double));		
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang6(install(rFuncName), arg1, arg2, arg3, arg4, arg5));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(6);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(6);
        return ZC_R_ERROR;
    }
}


int ZC_callR_6_1d(char* rFuncName, int vecType, 
size_t in1Len, void* in1, 
size_t in2Len, void* in2, 
size_t in3Len, void* in3, 
size_t in4Len, void* in4, 
size_t in5Len, void* in5, 
size_t in6Len, void* in6, 
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4, arg5, arg6;
    float *float_in1, *float_in2, *float_in3, *float_in4, *float_in5, *float_in6;
    double *double_in1, *double_in2, *double_in3, *double_in4, *double_in5, *double_in6;
    size_t i;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocVector(INTSXP, in1Len));
		memcpy(INTEGER(arg1), (int*)in1, in1Len* sizeof(int));
		PROTECT(arg2 = allocVector(INTSXP, in2Len));
		memcpy(INTEGER(arg2), (int*)in2, in2Len* sizeof(int));
		PROTECT(arg3 = allocVector(INTSXP, in3Len));
		memcpy(INTEGER(arg3), (int*)in3, in3Len* sizeof(int));	
		PROTECT(arg4 = allocVector(INTSXP, in4Len));
		memcpy(INTEGER(arg4), (int*)in4, in4Len* sizeof(int));
		PROTECT(arg5 = allocVector(INTSXP, in5Len));
		memcpy(INTEGER(arg5), (int*)in5, in5Len* sizeof(int));
		PROTECT(arg6 = allocVector(INTSXP, in6Len));
		memcpy(INTEGER(arg6), (int*)in6, in6Len* sizeof(int));
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		float_in5 = (float*)in5;
		float_in6 = (float*)in6;
		double_in1 = (double*)malloc(in1Len*sizeof(double));
		double_in2 = (double*)malloc(in2Len*sizeof(double));
		double_in3 = (double*)malloc(in3Len*sizeof(double));
		double_in4 = (double*)malloc(in4Len*sizeof(double));
		double_in5 = (double*)malloc(in5Len*sizeof(double));
		double_in6 = (double*)malloc(in6Len*sizeof(double));		
		for(i=0;i<in1Len;i++)
			double_in1[i] = float_in1[i];
		for(i=0;i<in2Len;i++)
			double_in2[i] = float_in2[i];
		for(i=0;i<in3Len;i++)
			double_in3[i] = float_in3[i];
		for(i=0;i<in4Len;i++)
			double_in4[i] = float_in4[i];
		for(i=0;i<in5Len;i++)
			double_in5[i] = float_in5[i];
		for(i=0;i<in6Len;i++)
			double_in6[i] = float_in6[i];			
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		PROTECT(arg4 = allocVector(REALSXP, in4Len));
		PROTECT(arg5 = allocVector(REALSXP, in5Len));
		PROTECT(arg6 = allocVector(REALSXP, in6Len));		
		memcpy(REAL(arg1), double_in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2Len* sizeof(double));		
		memcpy(REAL(arg3), double_in3, in3Len* sizeof(double));
		memcpy(REAL(arg4), double_in4, in4Len* sizeof(double));
		memcpy(REAL(arg5), double_in5, in5Len* sizeof(double));
		memcpy(REAL(arg6), double_in6, in6Len* sizeof(double));				
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocVector(REALSXP, in1Len));
		PROTECT(arg2 = allocVector(REALSXP, in2Len));
		PROTECT(arg3 = allocVector(REALSXP, in3Len));
		PROTECT(arg4 = allocVector(REALSXP, in4Len));
		PROTECT(arg5 = allocVector(REALSXP, in5Len));
		PROTECT(arg6 = allocVector(REALSXP, in6Len));		
		memcpy(REAL(arg1), (double*)in1, in1Len* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2Len* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3Len* sizeof(double));
		memcpy(REAL(arg4), (double*)in4, in4Len* sizeof(double));
		memcpy(REAL(arg5), (double*)in5, in5Len* sizeof(double));		
		memcpy(REAL(arg6), (double*)in6, in6Len* sizeof(double));				
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang7(install(rFuncName), arg1, arg2, arg3, arg4, arg5, arg6));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(7);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(7);
        return ZC_R_ERROR;
    }
}

int ZC_callR_3_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3;
    float *float_in1, *float_in2, *float_in3;
    double *double_in1, *double_in2, *double_in3;
    size_t i,j;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocMatrix(INTSXP, in1_n2, in1_n1));
		memcpy(INTEGER(arg1), (int*)in1, in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = allocMatrix(INTSXP, in2_n2, in2_n1));
		memcpy(INTEGER(arg2), (int*)in2, in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = allocMatrix(INTSXP, in3_n2, in3_n1));
		memcpy(INTEGER(arg3), (int*)in3, in3_n2*in3_n1* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		double_in1 = (double*)malloc(in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n2*in3_n1*sizeof(double));
		for(i=0;i<in1_n2;i++)
			for(j=0;j<in1_n1;j++)
				double_in1[i*in1_n1+j] = float_in1[i*in1_n1+j];
		for(i=0;i<in2_n2;i++)
			for(j=0;j<in2_n1;j++)
				double_in2[i*in2_n1+j] = float_in2[i*in2_n1+j];
		for(i=0;i<in3_n2;i++)
			for(j=0;j<in3_n1;j++)
				double_in3[i*in3_n1+j] = float_in3[i*in3_n1+j];				
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		memcpy(REAL(arg1), double_in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n2*in3_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		memcpy(REAL(arg1), (double*)in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n2*in2_n1* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3_n2*in3_n1* sizeof(double));
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang4(install(rFuncName), arg1, arg2, arg3));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(4);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(4);
        return ZC_R_ERROR;
    }
}

int ZC_callR_4_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n2, size_t in4_n1, void* in4, 
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4;
    float *float_in1, *float_in2, *float_in3, *float_in4;
    double *double_in1, *double_in2, *double_in3, *double_in4;
    size_t i,j;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocMatrix(INTSXP, in1_n2, in1_n1));
		memcpy(INTEGER(arg1), (int*)in1, in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = allocMatrix(INTSXP, in2_n2, in2_n1));
		memcpy(INTEGER(arg2), (int*)in2, in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = allocMatrix(INTSXP, in3_n2, in3_n1));
		memcpy(INTEGER(arg3), (int*)in3, in3_n2*in3_n1* sizeof(int));
		PROTECT(arg4 = allocMatrix(INTSXP, in4_n2, in4_n1));
		memcpy(INTEGER(arg4), (int*)in4, in4_n2*in4_n1* sizeof(int));				
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		double_in1 = (double*)malloc(in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n2*in3_n1*sizeof(double));
		double_in4 = (double*)malloc(in4_n2*in4_n1*sizeof(double));		
		for(i=0;i<in1_n2;i++)
			for(j=0;j<in1_n1;j++)
				double_in1[i*in1_n1+j] = float_in1[i*in1_n1+j];
		for(i=0;i<in2_n2;i++)
			for(j=0;j<in2_n1;j++)
				double_in2[i*in2_n1+j] = float_in2[i*in2_n1+j];
		for(i=0;i<in3_n2;i++)
			for(j=0;j<in3_n1;j++)
				double_in3[i*in3_n1+j] = float_in3[i*in3_n1+j];				
		for(i=0;i<in4_n2;i++)
			for(j=0;j<in4_n1;j++)
				double_in4[i*in4_n1+j] = float_in4[i*in4_n1+j];
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		PROTECT(arg4 = allocMatrix(REALSXP, in4_n2, in4_n1));
		memcpy(REAL(arg1), double_in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n2*in3_n1* sizeof(double));		
		memcpy(REAL(arg4), double_in4, in4_n2*in4_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		PROTECT(arg4 = allocMatrix(REALSXP, in4_n2, in4_n1));		
		memcpy(REAL(arg1), (double*)in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n2*in2_n1* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3_n2*in3_n1* sizeof(double));
		memcpy(REAL(arg4), (double*)in4, in4_n2*in4_n1* sizeof(double));		
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang5(install(rFuncName), arg1, arg2, arg3, arg4));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(5);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(5);
        return ZC_R_ERROR;
    }
}

int ZC_callR_5_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n2, size_t in4_n1, void* in4, 
size_t in5_n2, size_t in5_n1, void* in5,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4, arg5;
    float *float_in1, *float_in2, *float_in3, *float_in4, *float_in5;
    double *double_in1, *double_in2, *double_in3, *double_in4, *double_in5;
    size_t i,j;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocMatrix(INTSXP, in1_n2, in1_n1));
		memcpy(INTEGER(arg1), (int*)in1, in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = allocMatrix(INTSXP, in2_n2, in2_n1));
		memcpy(INTEGER(arg2), (int*)in2, in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = allocMatrix(INTSXP, in3_n2, in3_n1));
		memcpy(INTEGER(arg3), (int*)in3, in3_n2*in3_n1* sizeof(int));
		PROTECT(arg4 = allocMatrix(INTSXP, in4_n2, in4_n1));
		memcpy(INTEGER(arg4), (int*)in4, in4_n2*in4_n1* sizeof(int));		
		PROTECT(arg5 = allocMatrix(INTSXP, in5_n2, in5_n1));
		memcpy(INTEGER(arg5), (int*)in5, in5_n2*in5_n1* sizeof(int));
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		float_in5 = (float*)in5;
		double_in1 = (double*)malloc(in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n2*in3_n1*sizeof(double));
		double_in4 = (double*)malloc(in4_n2*in4_n1*sizeof(double));		
		double_in5 = (double*)malloc(in5_n2*in5_n1*sizeof(double));				
		for(i=0;i<in1_n2;i++)
			for(j=0;j<in1_n1;j++)
				double_in1[i*in1_n1+j] = float_in1[i*in1_n1+j];
		for(i=0;i<in2_n2;i++)
			for(j=0;j<in2_n1;j++)
				double_in2[i*in2_n1+j] = float_in2[i*in2_n1+j];
		for(i=0;i<in3_n2;i++)
			for(j=0;j<in3_n1;j++)
				double_in3[i*in3_n1+j] = float_in3[i*in3_n1+j];				
		for(i=0;i<in4_n2;i++)
			for(j=0;j<in4_n1;j++)
				double_in4[i*in4_n1+j] = float_in4[i*in4_n1+j];
		for(i=0;i<in5_n2;i++)
			for(j=0;j<in5_n1;j++)
				double_in5[i*in5_n1+j] = float_in5[i*in5_n1+j];				
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		PROTECT(arg4 = allocMatrix(REALSXP, in4_n2, in4_n1));
		PROTECT(arg5 = allocMatrix(REALSXP, in5_n2, in5_n1));		
		memcpy(REAL(arg1), double_in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n2*in3_n1* sizeof(double));		
		memcpy(REAL(arg4), double_in4, in4_n2*in4_n1* sizeof(double));
		memcpy(REAL(arg5), double_in5, in5_n2*in5_n1* sizeof(double));				
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		PROTECT(arg4 = allocMatrix(REALSXP, in4_n2, in4_n1));	
		PROTECT(arg5 = allocMatrix(REALSXP, in5_n2, in5_n1));				
		memcpy(REAL(arg1), (double*)in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n2*in2_n1* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3_n2*in3_n1* sizeof(double));
		memcpy(REAL(arg4), (double*)in4, in4_n2*in4_n1* sizeof(double));		
		memcpy(REAL(arg5), (double*)in5, in5_n2*in5_n1* sizeof(double));		
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang6(install(rFuncName), arg1, arg2, arg3, arg4, arg5));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(6);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(6);
        return ZC_R_ERROR;
    }
}

int ZC_callR_6_2d(char* rFuncName, int vecType, 
size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n2, size_t in4_n1, void* in4, 
size_t in5_n2, size_t in5_n1, void* in5,
size_t in6_n2, size_t in6_n1, void* in6,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4, arg5, arg6;
    float *float_in1, *float_in2, *float_in3, *float_in4, *float_in5, *float_in6;
    double *double_in1, *double_in2, *double_in3, *double_in4, *double_in5, *double_in6;
    size_t i,j;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = allocMatrix(INTSXP, in1_n2, in1_n1));
		memcpy(INTEGER(arg1), (int*)in1, in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = allocMatrix(INTSXP, in2_n2, in2_n1));
		memcpy(INTEGER(arg2), (int*)in2, in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = allocMatrix(INTSXP, in3_n2, in3_n1));
		memcpy(INTEGER(arg3), (int*)in3, in3_n2*in3_n1* sizeof(int));
		PROTECT(arg4 = allocMatrix(INTSXP, in4_n2, in4_n1));
		memcpy(INTEGER(arg4), (int*)in4, in4_n2*in4_n1* sizeof(int));		
		PROTECT(arg5 = allocMatrix(INTSXP, in5_n2, in5_n1));
		memcpy(INTEGER(arg5), (int*)in5, in5_n2*in5_n1* sizeof(int));
		PROTECT(arg6 = allocMatrix(INTSXP, in6_n2, in6_n1));
		memcpy(INTEGER(arg6), (int*)in6, in6_n2*in6_n1* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		float_in5 = (float*)in5;
		float_in6 = (float*)in6;		
		double_in1 = (double*)malloc(in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n2*in3_n1*sizeof(double));
		double_in4 = (double*)malloc(in4_n2*in4_n1*sizeof(double));		
		double_in5 = (double*)malloc(in5_n2*in5_n1*sizeof(double));	
		double_in6 = (double*)malloc(in6_n2*in6_n1*sizeof(double));			
		for(i=0;i<in1_n2;i++)
			for(j=0;j<in1_n1;j++)
				double_in1[i*in1_n1+j] = float_in1[i*in1_n1+j];
		for(i=0;i<in2_n2;i++)
			for(j=0;j<in2_n1;j++)
				double_in2[i*in2_n1+j] = float_in2[i*in2_n1+j];
		for(i=0;i<in3_n2;i++)
			for(j=0;j<in3_n1;j++)
				double_in3[i*in3_n1+j] = float_in3[i*in3_n1+j];				
		for(i=0;i<in4_n2;i++)
			for(j=0;j<in4_n1;j++)
				double_in4[i*in4_n1+j] = float_in4[i*in4_n1+j];
		for(i=0;i<in5_n2;i++)
			for(j=0;j<in5_n1;j++)
				double_in5[i*in5_n1+j] = float_in5[i*in5_n1+j];				
		for(i=0;i<in6_n2;i++)
			for(j=0;j<in6_n1;j++)
				double_in6[i*in6_n1+j] = float_in6[i*in6_n1+j];
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		PROTECT(arg4 = allocMatrix(REALSXP, in4_n2, in4_n1));
		PROTECT(arg5 = allocMatrix(REALSXP, in5_n2, in5_n1));
		PROTECT(arg6 = allocMatrix(REALSXP, in6_n2, in6_n1));				
		memcpy(REAL(arg1), double_in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n2*in3_n1* sizeof(double));		
		memcpy(REAL(arg4), double_in4, in4_n2*in4_n1* sizeof(double));
		memcpy(REAL(arg5), double_in5, in5_n2*in5_n1* sizeof(double));
		memcpy(REAL(arg6), double_in6, in6_n2*in6_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = allocMatrix(REALSXP, in1_n2, in1_n1));
		PROTECT(arg2 = allocMatrix(REALSXP, in2_n2, in2_n1));
		PROTECT(arg3 = allocMatrix(REALSXP, in3_n2, in3_n1));
		PROTECT(arg4 = allocMatrix(REALSXP, in4_n2, in4_n1));	
		PROTECT(arg5 = allocMatrix(REALSXP, in5_n2, in5_n1));	
		PROTECT(arg6 = allocMatrix(REALSXP, in6_n2, in6_n1));						
		memcpy(REAL(arg1), (double*)in1, in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n2*in2_n1* sizeof(double));	
		memcpy(REAL(arg3), (double*)in3, in3_n2*in3_n1* sizeof(double));
		memcpy(REAL(arg4), (double*)in4, in4_n2*in4_n1* sizeof(double));		
		memcpy(REAL(arg5), (double*)in5, in5_n2*in5_n1* sizeof(double));		
		memcpy(REAL(arg6), (double*)in6, in6_n2*in6_n1* sizeof(double));				
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang7(install(rFuncName), arg1, arg2, arg3, arg4, arg5, arg6));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(7);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(7);
        return ZC_R_ERROR;
    }
}

int ZC_callR_3_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3;
    float *float_in1, *float_in2, *float_in3;
    double *double_in1, *double_in2, *double_in3;
    size_t i,j,k;
	size_t in1_n21 = in1_n2*in1_n1;
	size_t in2_n21 = in2_n2*in2_n1;
	size_t in3_n21 = in3_n2*in3_n1;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = alloc3DArray(INTSXP, in1_n2, in1_n1, in1_n3));
		memcpy(INTEGER(arg1), (int*)in1, in1_n3*in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = alloc3DArray(INTSXP, in2_n2, in2_n1, in2_n3));
		memcpy(INTEGER(arg2), (int*)in2, in2_n3*in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = alloc3DArray(INTSXP, in3_n2, in3_n1, in3_n3));
		memcpy(INTEGER(arg3), (int*)in3, in3_n3*in3_n2*in3_n1* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		double_in1 = (double*)malloc(in1_n3*in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n3*in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n3*in3_n2*in3_n1*sizeof(double));
		
		for(i=0;i<in1_n3;i++)
			for(j=0;j<in1_n2;j++)
				for(k=0;k<in1_n1;k++)
					double_in1[i*in1_n21+j*in1_n1+k] = float_in1[i*in1_n21+j*in1_n1+k];
		for(i=0;i<in2_n3;i++)
			for(j=0;j<in2_n2;j++)
				for(k=0;k<in2_n1;k++)
					double_in2[i*in2_n21+j*in2_n1+k] = float_in2[i*in2_n21+j*in2_n1+k];
		for(i=0;i<in3_n3;i++)
			for(j=0;j<in3_n2;j++)
				for(k=0;k<in3_n1;k++)
					double_in3[i*in3_n21+j*in3_n1+k] = float_in3[i*in3_n21+j*in3_n1+k];					
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));		
		memcpy(REAL(arg1), double_in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n3*in3_n2*in3_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));
		memcpy(REAL(arg1), (double*)in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), (double*)in3, in3_n3*in3_n2*in3_n1* sizeof(double));			
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang4(install(rFuncName), arg1, arg2, arg3));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(4);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(4);
        return ZC_R_ERROR;
    }
}

int ZC_callR_4_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n3, size_t in4_n2, size_t in4_n1, void* in4,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4;
    float *float_in1, *float_in2, *float_in3, *float_in4;
    double *double_in1, *double_in2, *double_in3, *double_in4;
    size_t i,j,k;
	size_t in1_n21 = in1_n2*in1_n1;
	size_t in2_n21 = in2_n2*in2_n1;
	size_t in3_n21 = in3_n2*in3_n1;
	size_t in4_n21 = in4_n2*in4_n1;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = alloc3DArray(INTSXP, in1_n2, in1_n1, in1_n3));
		memcpy(INTEGER(arg1), (int*)in1, in1_n3*in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = alloc3DArray(INTSXP, in2_n2, in2_n1, in2_n3));
		memcpy(INTEGER(arg2), (int*)in2, in2_n3*in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = alloc3DArray(INTSXP, in3_n2, in3_n1, in3_n3));
		memcpy(INTEGER(arg3), (int*)in3, in3_n3*in3_n2*in3_n1* sizeof(int));
		PROTECT(arg4 = alloc3DArray(INTSXP, in4_n2, in4_n1, in4_n3));
		memcpy(INTEGER(arg4), (int*)in4, in4_n3*in4_n2*in4_n1* sizeof(int));				
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		double_in1 = (double*)malloc(in1_n3*in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n3*in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n3*in3_n2*in3_n1*sizeof(double));
		double_in4 = (double*)malloc(in4_n3*in4_n2*in4_n1*sizeof(double));
		
		for(i=0;i<in1_n3;i++)
			for(j=0;j<in1_n2;j++)
				for(k=0;k<in1_n1;k++)
					double_in1[i*in1_n21+j*in1_n1+k] = float_in1[i*in1_n21+j*in1_n1+k];
		for(i=0;i<in2_n3;i++)
			for(j=0;j<in2_n2;j++)
				for(k=0;k<in2_n1;k++)
					double_in2[i*in2_n21+j*in2_n1+k] = float_in2[i*in2_n21+j*in2_n1+k];
		for(i=0;i<in3_n3;i++)
			for(j=0;j<in3_n2;j++)
				for(k=0;k<in3_n1;k++)
		for(i=0;i<in4_n3;i++)
			for(j=0;j<in4_n2;j++)
				for(k=0;k<in4_n1;k++)				
					double_in4[i*in4_n21+j*in4_n1+k] = float_in4[i*in4_n21+j*in4_n1+k];					
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));	
		PROTECT(arg4 = alloc3DArray(REALSXP, in4_n2, in4_n1, in4_n3));	
		memcpy(REAL(arg1), double_in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n3*in3_n2*in3_n1* sizeof(double));	
		memcpy(REAL(arg4), double_in4, in4_n3*in4_n2*in4_n1* sizeof(double));			
		break;
	case ZC_R_Double:
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));
		PROTECT(arg4 = alloc3DArray(REALSXP, in4_n2, in4_n1, in4_n3));		
		memcpy(REAL(arg1), (double*)in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), (double*)in3, in3_n3*in3_n2*in3_n1* sizeof(double));			
		memcpy(REAL(arg4), (double*)in4, in4_n3*in4_n2*in4_n1* sizeof(double));			
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang5(install(rFuncName), arg1, arg2, arg3, arg4));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(5);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(5);
        return ZC_R_ERROR;
    }
}

int ZC_callR_5_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n3, size_t in4_n2, size_t in4_n1, void* in4,
size_t in5_n3, size_t in5_n2, size_t in5_n1, void* in5,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4, arg5;
    float *float_in1, *float_in2, *float_in3, *float_in4, *float_in5;
    double *double_in1, *double_in2, *double_in3, *double_in4, *double_in5;
    size_t i,j,k;
	size_t in1_n21 = in1_n2*in1_n1;
	size_t in2_n21 = in2_n2*in2_n1;
	size_t in3_n21 = in3_n2*in3_n1;
	size_t in4_n21 = in4_n2*in4_n1;
	size_t in5_n21 = in5_n2*in5_n1;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = alloc3DArray(INTSXP, in1_n2, in1_n1, in1_n3));
		memcpy(INTEGER(arg1), (int*)in1, in1_n3*in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = alloc3DArray(INTSXP, in2_n2, in2_n1, in2_n3));
		memcpy(INTEGER(arg2), (int*)in2, in2_n3*in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = alloc3DArray(INTSXP, in3_n2, in3_n1, in3_n3));
		memcpy(INTEGER(arg3), (int*)in3, in3_n3*in3_n2*in3_n1* sizeof(int));
		PROTECT(arg4 = alloc3DArray(INTSXP, in4_n2, in4_n1, in4_n3));
		memcpy(INTEGER(arg4), (int*)in4, in4_n3*in4_n2*in4_n1* sizeof(int));
		PROTECT(arg5 = alloc3DArray(INTSXP, in5_n2, in5_n1, in5_n3));
		memcpy(INTEGER(arg5), (int*)in5, in5_n3*in5_n2*in5_n1* sizeof(int));		
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		float_in5 = (float*)in5;		
		double_in1 = (double*)malloc(in1_n3*in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n3*in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n3*in3_n2*in3_n1*sizeof(double));
		double_in4 = (double*)malloc(in4_n3*in4_n2*in4_n1*sizeof(double));
		double_in5 = (double*)malloc(in5_n3*in5_n2*in5_n1*sizeof(double));		
		
		for(i=0;i<in1_n3;i++)
			for(j=0;j<in1_n2;j++)
				for(k=0;k<in1_n1;k++)
					double_in1[i*in1_n21+j*in1_n1+k] = float_in1[i*in1_n21+j*in1_n1+k];
		for(i=0;i<in2_n3;i++)
			for(j=0;j<in2_n2;j++)
				for(k=0;k<in2_n1;k++)
					double_in2[i*in2_n21+j*in2_n1+k] = float_in2[i*in2_n21+j*in2_n1+k];
		for(i=0;i<in3_n3;i++)
			for(j=0;j<in3_n2;j++)
				for(k=0;k<in3_n1;k++)
		for(i=0;i<in4_n3;i++)
			for(j=0;j<in4_n2;j++)
				for(k=0;k<in4_n1;k++)				
					double_in4[i*in4_n21+j*in4_n1+k] = float_in4[i*in4_n21+j*in4_n1+k];
		for(i=0;i<in5_n3;i++)
			for(j=0;j<in5_n2;j++)
				for(k=0;k<in5_n1;k++)				
					double_in5[i*in5_n21+j*in5_n1+k] = float_in5[i*in5_n21+j*in5_n1+k];					
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));	
		PROTECT(arg4 = alloc3DArray(REALSXP, in4_n2, in4_n1, in4_n3));
		PROTECT(arg5 = alloc3DArray(REALSXP, in5_n2, in5_n1, in5_n3));			
		memcpy(REAL(arg1), double_in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n3*in3_n2*in3_n1* sizeof(double));	
		memcpy(REAL(arg4), double_in4, in4_n3*in4_n2*in4_n1* sizeof(double));
		memcpy(REAL(arg5), double_in5, in5_n3*in5_n2*in5_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));
		PROTECT(arg4 = alloc3DArray(REALSXP, in4_n2, in4_n1, in4_n3));	
		PROTECT(arg5 = alloc3DArray(REALSXP, in5_n2, in5_n1, in5_n3));				
		memcpy(REAL(arg1), (double*)in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), (double*)in3, in3_n3*in3_n2*in3_n1* sizeof(double));			
		memcpy(REAL(arg4), (double*)in4, in4_n3*in4_n2*in4_n1* sizeof(double));
		memcpy(REAL(arg5), (double*)in5, in5_n3*in5_n2*in5_n1* sizeof(double));	
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang6(install(rFuncName), arg1, arg2, arg3, arg4, arg5));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(6);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(6);
        return ZC_R_ERROR;
    }
}

int ZC_callR_6_3d(char* rFuncName, int vecType, 
size_t in1_n3, size_t in1_n2, size_t in1_n1, void* in1, 
size_t in2_n3, size_t in2_n2, size_t in2_n1, void* in2, 
size_t in3_n3, size_t in3_n2, size_t in3_n1, void* in3,
size_t in4_n3, size_t in4_n2, size_t in4_n1, void* in4,
size_t in5_n3, size_t in5_n2, size_t in5_n1, void* in5,
size_t in6_n3, size_t in6_n2, size_t in6_n1, void* in6,
size_t* outLen, double** out)
{
	// Allocate an R vector and copy the C array into it.
    SEXP arg1, arg2, arg3, arg4, arg5, arg6;
    float *float_in1, *float_in2, *float_in3, *float_in4, *float_in5, *float_in6;
    double *double_in1, *double_in2, *double_in3, *double_in4, *double_in5, *double_in6;
    size_t i,j,k;
	size_t in1_n21 = in1_n2*in1_n1;
	size_t in2_n21 = in2_n2*in2_n1;
	size_t in3_n21 = in3_n2*in3_n1;
	size_t in4_n21 = in4_n2*in4_n1;
	size_t in5_n21 = in5_n2*in5_n1;
	size_t in6_n21 = in6_n2*in6_n1;
	switch(vecType)
	{
	case ZC_R_Int:
		PROTECT(arg1 = alloc3DArray(INTSXP, in1_n2, in1_n1, in1_n3));
		memcpy(INTEGER(arg1), (int*)in1, in1_n3*in1_n2*in1_n1* sizeof(int));
		PROTECT(arg2 = alloc3DArray(INTSXP, in2_n2, in2_n1, in2_n3));
		memcpy(INTEGER(arg2), (int*)in2, in2_n3*in2_n2*in2_n1* sizeof(int));
		PROTECT(arg3 = alloc3DArray(INTSXP, in3_n2, in3_n1, in3_n3));
		memcpy(INTEGER(arg3), (int*)in3, in3_n3*in3_n2*in3_n1* sizeof(int));
		PROTECT(arg4 = alloc3DArray(INTSXP, in4_n2, in4_n1, in4_n3));
		memcpy(INTEGER(arg4), (int*)in4, in4_n3*in4_n2*in4_n1* sizeof(int));
		PROTECT(arg5 = alloc3DArray(INTSXP, in5_n2, in5_n1, in5_n3));
		memcpy(INTEGER(arg5), (int*)in5, in5_n3*in5_n2*in5_n1* sizeof(int));		
		PROTECT(arg6 = alloc3DArray(INTSXP, in6_n2, in6_n1, in6_n3));
		memcpy(INTEGER(arg6), (int*)in6, in6_n3*in6_n2*in6_n1* sizeof(int));			
		break;
	case ZC_R_Float:
		float_in1 = (float*)in1;
		float_in2 = (float*)in2;
		float_in3 = (float*)in3;
		float_in4 = (float*)in4;
		float_in5 = (float*)in5;		
		float_in6 = (float*)in6;
		double_in1 = (double*)malloc(in1_n3*in1_n2*in1_n1*sizeof(double));
		double_in2 = (double*)malloc(in2_n3*in2_n2*in2_n1*sizeof(double));
		double_in3 = (double*)malloc(in3_n3*in3_n2*in3_n1*sizeof(double));
		double_in4 = (double*)malloc(in4_n3*in4_n2*in4_n1*sizeof(double));
		double_in5 = (double*)malloc(in5_n3*in5_n2*in5_n1*sizeof(double));		
		double_in6 = (double*)malloc(in6_n3*in6_n2*in6_n1*sizeof(double));				
		
		for(i=0;i<in1_n3;i++)
			for(j=0;j<in1_n2;j++)
				for(k=0;k<in1_n1;k++)
					double_in1[i*in1_n21+j*in1_n1+k] = float_in1[i*in1_n21+j*in1_n1+k];
		for(i=0;i<in2_n3;i++)
			for(j=0;j<in2_n2;j++)
				for(k=0;k<in2_n1;k++)
					double_in2[i*in2_n21+j*in2_n1+k] = float_in2[i*in2_n21+j*in2_n1+k];
		for(i=0;i<in3_n3;i++)
			for(j=0;j<in3_n2;j++)
				for(k=0;k<in3_n1;k++)
		for(i=0;i<in4_n3;i++)
			for(j=0;j<in4_n2;j++)
				for(k=0;k<in4_n1;k++)				
					double_in4[i*in4_n21+j*in4_n1+k] = float_in4[i*in4_n21+j*in4_n1+k];
		for(i=0;i<in5_n3;i++)
			for(j=0;j<in5_n2;j++)
				for(k=0;k<in5_n1;k++)
					double_in5[i*in5_n21+j*in5_n1+k] = float_in5[i*in5_n21+j*in5_n1+k];					
		for(i=0;i<in6_n3;i++)
			for(j=0;j<in6_n2;j++)
				for(k=0;k<in6_n1;k++)
					double_in6[i*in6_n21+j*in6_n1+k] = float_in6[i*in6_n21+j*in6_n1+k];					
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));	
		PROTECT(arg4 = alloc3DArray(REALSXP, in4_n2, in4_n1, in4_n3));
		PROTECT(arg5 = alloc3DArray(REALSXP, in5_n2, in5_n1, in5_n3));
		PROTECT(arg6 = alloc3DArray(REALSXP, in6_n2, in6_n1, in6_n3));					
		memcpy(REAL(arg1), double_in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), double_in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), double_in3, in3_n3*in3_n2*in3_n1* sizeof(double));	
		memcpy(REAL(arg4), double_in4, in4_n3*in4_n2*in4_n1* sizeof(double));
		memcpy(REAL(arg5), double_in5, in5_n3*in5_n2*in5_n1* sizeof(double));		
		memcpy(REAL(arg6), double_in6, in6_n3*in6_n2*in6_n1* sizeof(double));		
		break;
	case ZC_R_Double:
		PROTECT(arg1 = alloc3DArray(REALSXP, in1_n2, in1_n1, in1_n3));
		PROTECT(arg2 = alloc3DArray(REALSXP, in2_n2, in2_n1, in2_n3));
		PROTECT(arg3 = alloc3DArray(REALSXP, in3_n2, in3_n1, in3_n3));
		PROTECT(arg4 = alloc3DArray(REALSXP, in4_n2, in4_n1, in4_n3));	
		PROTECT(arg5 = alloc3DArray(REALSXP, in5_n2, in5_n1, in5_n3));				
		PROTECT(arg6 = alloc3DArray(REALSXP, in6_n2, in6_n1, in6_n3));				
		memcpy(REAL(arg1), (double*)in1, in1_n3*in1_n2*in1_n1* sizeof(double));
		memcpy(REAL(arg2), (double*)in2, in2_n3*in2_n2*in2_n1* sizeof(double));
		memcpy(REAL(arg3), (double*)in3, in3_n3*in3_n2*in3_n1* sizeof(double));			
		memcpy(REAL(arg4), (double*)in4, in4_n3*in4_n2*in4_n1* sizeof(double));
		memcpy(REAL(arg5), (double*)in5, in5_n3*in5_n2*in5_n1* sizeof(double));
		memcpy(REAL(arg6), (double*)in6, in6_n3*in6_n2*in6_n1* sizeof(double));			
		break;
	default: 
		printf("Error: vecType (==%d) should only be ZC_R_Int(0), ZC_R_Float(1), or ZC_R_Double(2).\n", vecType);
		return ZC_R_ERROR;
	}

    // Setup a call to the R function
    SEXP r_call;
    PROTECT(r_call = lang7(install(rFuncName), arg1, arg2, arg3, arg4, arg5, arg6));

    // Execute the function
    int errorOccurred;
    SEXP ret = R_tryEval(r_call, R_GlobalEnv, &errorOccurred);
    
    if (!errorOccurred)
    {
        double *val = REAL(ret);
        *outLen = LENGTH(ret);
        *out = (double*)malloc(*outLen*sizeof(double));
        memcpy(*out, val, *outLen*sizeof(double));
        UNPROTECT(7);
        return ZC_R_SUCCESS;
    }
    else
    {
        printf("Error occurred calling R\n");
        UNPROTECT(7);
        return ZC_R_ERROR;
    }
}
