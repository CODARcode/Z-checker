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

#include "ZC_CompareData.h"

#ifdef __cplusplus
extern "C" {
#endif

ZC_CompareData* ZC_constructOneVariable(char* varName, char* endian, char* type, char* dim, char* filePath);

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_DataSetHandler_H  ----- */
