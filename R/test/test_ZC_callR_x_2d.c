#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#include "ZC_callR.h"

int main(int argc, char *argv[])
{
    // Intialize the R environment.
    int r_argc = 2;
    char *r_argv[] = { "R", "--silent" };
    Rf_initEmbeddedR(r_argc, r_argv);

    int arg[2][3];
    size_t i,j,k=1;
    for(i=0;i<2;i++)
		for(j=0;j<3;j++)
			arg[i][j]=k++;

    // Invoke a function in R
    source("func.R");
    size_t outLen;
    double *out;
    printf("ZC_callR_1_2d (int vector)----\n");
    ZC_callR_1_2d("add1", ZC_R_Int, 2, 3, arg, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	free(out);

	printf("ZC_callR_1_2d (float vector)----\n");
    float arg2[2][3];
    k=1;
    for(i=0;i<2;i++)
		for(j=0;j<3;j++)
			arg2[i][j]=k++;

    ZC_callR_1_2d("add1", ZC_R_Float, 2, 3, arg2, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	free(out);

	printf("ZC_callR_1_2d (double vector)----\n");
    double arg3[2][3];
    k=1;
    for(i=0;i<2;i++)
		for(j=0;j<3;j++)
			arg3[i][j]=k++;

    ZC_callR_1_2d("add1", ZC_R_Double, 2, 3, arg3, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

	free(out);
	printf("ZC_callR_2_2d (float vector)----\n");

	float arg4[2][3];
	k=1;
    for(i=0;i<2;i++)
		for(j=0;j<3;j++)
			arg4[i][j]=k++;

	ZC_callR_2_2d("add1", ZC_R_Float, 2, 3, arg2, 2, 3, arg4, &outLen, &out);
	for(i=0;i<outLen;i++)
		printf("%d %f\n", i, out[i]);

    // Release R environment
    Rf_endEmbeddedR(0);
    return(0);
}
