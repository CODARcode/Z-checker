/**
 *  @file ZC_HDF5Handler.h
 *  @author Sheng Di
 *  @date May, 2017
 *  @brief Handler of HDF5 
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#ifndef _ZC_HDF5Handler_H
#define _ZC_HDF5Handler_H

#include "hdf5.h"

#ifdef __cplusplus
extern "C" {
#endif

ZC_DataSet* read();

#ifdef __cplusplus
}
#endif

#endif /* ----- #ifndef _ZC_HDF5Handler_H  ----- */
