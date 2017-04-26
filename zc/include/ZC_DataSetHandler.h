/**
 *  @file ZC_DataSetHandler.h
 *  @author Sheng Di
 *  @date May, 2017
 *  @brief Handler of HDF5 
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_DataSetHandler_H
#define _ZC_DataSetHandler_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZC_DataSet
{
	int id;
	char* varName;
	int dataType; /*ZC_DOUBLE or ZC_FLOAT or ZC_INT32 or ZC_INT16 or ZC_INT8*/
	int r5;
	int r4;
	int r3;
	int r2;
	int r1;
	
	void *data;
	
	int numOfElem;
} ZC_DataSet;

//extern ZC_DataSet* zc_datasets;
//extern int zc_dataset_count;

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_DataSetHandler_H  ----- */
