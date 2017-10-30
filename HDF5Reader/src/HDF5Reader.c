#include <stdio.h>
#include <stdlib.h>
#include "HDF5Reader.h"

int hdf5Reader(void *data, char *filename, char *dataset, int dataType)
{
	hid_t       file_id, dataset_id;  /* identifiers */
	herr_t      status;

	file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT); 
	dataset_id = H5Dopen2(file_id, dataset, H5P_DEFAULT);

	if (dataType == 0)
	  status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (float*)data);
	else
	  status = H5Dread(dataset_id, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (double*)data);

	if (status < 0)
	{
		printf("Error: %s%s file cannot be read!\n", filename, dataset);
		exit(0);
	}

	/* Close HDF5 dataset and file. */
	status = H5Dclose(dataset_id);
	status = H5Fclose(file_id);

	return 0;
}

