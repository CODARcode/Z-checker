#include "HDF5Reader.h"
#include "hdf5.h"

#define H5FILE_NAME    "testdata/testdata.h5" /* get a better example file... */

int main(void)
{
	hid_t    file;
	hid_t    grp;

	herr_t   status;
	
	file = H5Fopen(H5FILE_NAME, H5F_ACC_RDWR, H5P_DEFAULT);

	grp = H5Gopen(file,"/");
	//scan_group(grp);
	
	status = H5Fclose(file);
	return 0;
}

