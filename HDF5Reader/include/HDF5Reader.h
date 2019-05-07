#include "hdf5.h"
#include "HDF5Selector.h"

#ifndef _HDF5Reader_H
#define _HDF5Reader_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NAME 1024

#define ZC_HDF5_FLOAT 0
#define ZC_HDF5_DOUBLE 1
#define ZC_HDF5_INT8 2
#define ZC_HDF5_INT16 3
#define ZC_HDF5_INT32 4
#define ZC_HDF5_INT64 5
#define ZC_HDF5_UINT8 6
#define ZC_HDF5_UINT16 7
#define ZC_HDF5_UINT32 8
#define ZC_HDF5_UINT64 9

void printData(void* data, int dataType, size_t* dims);
int getDataType(hid_t tid, int *ord);
int hdf5Reader(void*, char*, char*, int);
int hdf5Reader2(void *data, hid_t dataset_id, int dataType, int ord);
void* getDataSet(hid_t did, int *dataType, size_t* dims);
void scan_group_SZ(hid_t gid, int errBoundMode, int nbErrBounds, char **errBounds);
void scan_group_property_analysis(hid_t gid);

size_t ZFP_computeDataLength(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
unsigned char* zfp_compression_float(float* data, int mode, double tolerance, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outSize);
unsigned char* zfp_compression_double(double* data, int mode, double tolerance, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outSize);
float* zfp_decompression_float(unsigned char* bytes, size_t outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
double* zfp_decompression_double(unsigned char* bytes, size_t outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
unsigned char* zfp_compression(void* data, int dataType, int mode, double tolerance, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outSize);
void* zfp_decompression(int dataType, unsigned char* bytes, size_t outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1);
void scan_group_ZFP(hid_t gid, int errBoundMode, int nbErrBounds, char **errBounds);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _HDF5Reader_H  ----- */
