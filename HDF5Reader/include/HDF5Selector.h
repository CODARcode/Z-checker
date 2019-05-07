#ifndef _HDF5Selector_H
#define _HDF5Selector_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_Number_of_VARs 256

typedef struct SelectionPolicy
{
	int nbVars;
	int dimension;
	char *var[MAX_Number_of_VARs];
	size_t minN;
	size_t r5;
	size_t r4;
	size_t r3;
	size_t r2;
	size_t r1;
	int dataType; //0: float ; 1: double
} SelectionPolicy;

extern SelectionPolicy *hdf5Selector;

size_t testHDF5_computeDataSize(size_t r1, size_t r2, size_t r3, size_t r4, size_t r5);
int testHDF5_computeDimension(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);

SelectionPolicy* testHDF5_Init(char* fields, int dim, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t minN, int dataType);
SelectionPolicy* testHDF5_Init_default();
void testHDF5_Free();
int testHDF5_check(char* varName, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, int dataType);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _HDF5Selector_H  ----- */
