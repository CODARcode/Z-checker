#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "ZC_callR.h"

int main(int argc, char *argv[])
{
	// Intialize the R environment.
	int r_argc = 2;
	char *r_argv[] = { "R", "--silent" };
	Rf_initEmbeddedR(r_argc, r_argv);

	int arg[6];
	size_t i,k=1;
	for(i=0;i<6;i++)
		arg[i]=k++;

	// Invoke a function in R
	source("func.R");
	size_t outLen;
	double *out;
	printf("ZC_callR_1_1d (int vector)----\n");
	ZC_callR_1_1d("add1", ZC_R_Int, 6, arg, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	free(out);

	printf("ZC_callR_1_1d (float vector)----\n");
	float arg2[6];
	k = 1;
	for(i=0;i<6;i++)
		arg2[i]=k++;

	ZC_callR_1_1d("add1", ZC_R_Float, 6, arg2, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	free(out);

	printf("ZC_callR_1_1d (double vector)----\n");
	double arg3[6];
	k = 1;
	for(i=0;i<6;i++)
		arg3[i]=k++;

	ZC_callR_1_1d("add1", ZC_R_Double, 6, arg3, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	free(out);
	printf("ZC_callR_2_1d (float vector)----\n");

	k = 1;
	float arg4[6];
	for(i=0;i<6;i++)
		arg4[i]=k++;

	ZC_callR_2_1d("add1", ZC_R_Float, 6, arg2, 6, arg4, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	printf("ZC_callR_3_1d (float vector)----\n");	
	k = 1;
	float arg5[6];
	for(i=0;i<6;i++)
		arg5[i]=k++;
	
	ZC_callR_3_1d("add1", ZC_R_Float, 6, arg2, 6, arg4, 6, arg5, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	k = 1;
	float arg6[6];
	for(i=0;i<6;i++)
		arg6[i]=k++;
		
	k = 1;
	float arg7[6];
	for(i=0;i<6;i++)
		arg7[i]=k++;
		
	k = 1;
	float arg8[6];
	for(i=0;i<6;i++)
		arg8[i]=k++;				

	ZC_callR_6_1d("add1", ZC_R_Float, 6, arg2, 6, arg4, 6, arg5, 6, arg6, 6, arg7, 6, arg8, &outLen, &out);
	//ZC_callR_5_1d("add1", ZC_R_Float, 6, arg2, 6, arg4, 6, arg5, 6, arg6, 6, arg7, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	// Release R environment
	Rf_endEmbeddedR(0);
	return(0);
}
