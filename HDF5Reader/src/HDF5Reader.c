#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HDF5Reader.h"
#include "HDF5Selector.h"
#include "zc.h"

#ifdef with_SZ
#include "sz.h"
#endif

#ifdef with_ZFP
#include "zfp.h"
#endif

int ZC_HDF5_ReplaceStr(char *sSrc, char *sMatchStr, char *sReplaceStr)
{
	int  StringLen;
	char caNewString[1024];

	char *FindPos = strstr(sSrc, sMatchStr);
	if( (!FindPos) || (!sMatchStr) )
		return -1;

	while( FindPos )
	{
		memset(caNewString, 0, sizeof(caNewString));
		StringLen = FindPos - sSrc;
		strncpy(caNewString, sSrc, StringLen);
		strcat(caNewString, sReplaceStr);
		strcat(caNewString, FindPos + strlen(sMatchStr));
		strcpy(sSrc, caNewString);

		FindPos = strstr(sSrc, sMatchStr);
	}

	return 0;
}

void printData(void* data, int dataType, size_t* dims)
{
	size_t i = 0;
	float* floatData;
	double* doubleData;
	switch(dataType)
	{
		case 0:
			floatData = (float*)data;
			for(i = 0;i<10;i++)
				printf("%f\n", floatData[i]);
			break;
		case 1:
			doubleData = (double*)data;
			for(i = 0;i<10;i++)
				printf("%lf\n", doubleData[i]);
			break;
	}	
}

int getDataType(hid_t tid, int *ord) 
{
	int sgn = 0, sz = 0;
	H5T_class_t t_class;
	t_class = H5Tget_class(tid);
	if(t_class < 0){ 
		puts(" Invalid datatype.\n");
		return -2;
	} else {
		if(t_class == H5T_INTEGER) {
			*ord = H5Tget_order(tid);
			sgn = H5Tget_sign(tid);
			//printf("Integer ByteOrder= ");
			/*switch (*ord) {
			case H5T_ORDER_LE:
				printf("LE");
				break;
			case H5T_ORDER_BE:
				printf("BE");
				break;
			}
			printf(" Sign= ");
			switch (sgn) {
			case H5T_SGN_NONE:
				printf("false");
				break;
			case H5T_SGN_2:
				printf("true");
				break;
			}
			printf(" Size= ");
			sz = H5Tget_size(tid);
			printf("%d", sz);
			printf("\n");*/		
		    return ZC_HDF5_INT64;
		} else if(t_class == H5T_FLOAT) {
			*ord = H5Tget_order(tid); //either H5T_ORDER_LE or H5T_ORDER_BE
			sz = H5Tget_size(tid);
			if(sz==4)
				return ZC_HDF5_FLOAT;
			else if(sz==8)
				return ZC_HDF5_DOUBLE;
		} else if(t_class == H5T_STRING) {
		      return -1;
		} else if(t_class == H5T_BITFIELD) {
		      return -1;
		} else if(t_class == H5T_OPAQUE) {
		      return -1;
		} else if(t_class == H5T_COMPOUND) {
		      return -1;
		} else if(t_class == H5T_ARRAY) {
		      return -1;
		} else if(t_class == H5T_ENUM) {
		      return -1;
		} else {
		      return -1;
		}
	}
}

int hdf5Reader(void *data, char *filename, char *dataset, int dataType)
{
	hid_t       file_id, dataset_id;  /* identifiers */

	file_id = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT); 
	dataset_id = H5Dopen2(file_id, dataset, H5P_DEFAULT);

	int status = hdf5Reader2(data, dataset_id, dataType, H5T_ORDER_LE);

	/* Close HDF5 dataset and file. */
	status = H5Dclose(dataset_id);
	status = H5Fclose(file_id);

	return status;
}

int hdf5Reader2(void *data, hid_t dataset_id, int dataType, int ord)
{
	int status = 0;
	
	if (dataType == ZC_HDF5_FLOAT && ord == H5T_ORDER_LE)
		status = H5Dread(dataset_id, H5T_IEEE_F32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (float*)data);
	else if(dataType == ZC_HDF5_DOUBLE && ord == H5T_ORDER_LE)
		status = H5Dread(dataset_id, H5T_IEEE_F64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (double*)data);	
	else if(dataType == ZC_HDF5_FLOAT && ord == H5T_ORDER_BE)
		status = H5Dread(dataset_id, H5T_IEEE_F32BE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (float*)data);
	else if(dataType == ZC_HDF5_DOUBLE && ord == H5T_ORDER_BE)
		status = H5Dread(dataset_id, H5T_IEEE_F64BE, H5S_ALL, H5S_ALL, H5P_DEFAULT, (double*)data);	
	return status;
}

void* getDataSet(hid_t did, int *dataType, size_t* dims)
{
	hid_t tid;
	hid_t sid;
	char ds_name[MAX_NAME];

        /*
         * Information about the group:
         *  Name and attributes
         *
         *  Other info., not shown here: number of links, object id
         */
	H5Iget_name(did, ds_name, MAX_NAME  );
	printf("Dataset Name : ");
	puts(ds_name);
	printf("\n");

	/*
	 *  process the attributes of the dataset, if any.
	 */
	//scan_attrs(did);
  
	/*    
	 * Get dataset information: dataspace, data type 
	 */
	sid = H5Dget_space(did); /* the dimensions of the dataset (not shown) */
	tid = H5Dget_type(did);
	int ord = 0;
	*dataType = getDataType(tid, &ord);
	
	hsize_t hdims[5] = {0,0,0,0,0};
	H5Sget_simple_extent_dims(sid, hdims, NULL);
	void *result_data = NULL;
	float* floatData = NULL;
	double* doubleData = NULL;

	int dimension = testHDF5_computeDimension(hdims[4], hdims[3], hdims[2], hdims[1], hdims[0]); 
	printf("dimensions=\n");
	
	if(*dataType>=0)
	{
		int i = 0;
		switch(dimension)
		{
		case 1:
			for(i=0;i<5;i++)
			{
				dims[i] = hdims[i];
				printf("%zu ", dims[i]);
			}
			break;
		case 2:
			dims[0] = hdims[1];
			dims[1] = hdims[0];
			dims[2] = 0;
			dims[3] = 0;
			dims[4] = 0;
			for(i=0;i<5;i++)
				printf("%zu ", dims[i]);
			break;
		case 3:
			dims[0] = hdims[2];
			dims[1] = hdims[1];
			dims[2] = hdims[0];
			dims[3] = 0;
			dims[4] = 0;
			for(i=0;i<5;i++)
				printf("%zu ", dims[i]);
			break;		
		}
		printf("\n");
		
		size_t nbEle = testHDF5_computeDataSize(dims[0], dims[1], dims[2], dims[3], dims[4]);		
		
		switch(*dataType)
		{
		case 0:
			floatData = (float*)malloc(sizeof(float)*nbEle);
			hdf5Reader2((void*)floatData, did, *dataType, ord);
			result_data = floatData;
			break;
		case 1:
			doubleData = (double*)malloc(sizeof(double)*nbEle);
			hdf5Reader2((void*)doubleData, did, *dataType, ord);	
			result_data = doubleData;		
			break;
		}
	}
	
	
	/*
	 * Retrieve and analyse the dataset properties
	 */
	//pid = H5Dget_create_plist(did); /* get creation property list */
	//do_plist(pid);
	//size = H5Dget_storage_size(did);
	//printf("Total space currently written in file: %d\n",(int)size);

	/*
	 * The datatype and dataspace can be used to read all or
	 * part of the data.  (Not shown in this example.)
	 */

	  /* ... read data with H5Dread, write with H5Dwrite, etc. */

	//H5Pclose(pid);

	H5Tclose(tid);
	H5Sclose(sid);
	return result_data;
}

void scan_group_property_analysis(hid_t gid) 
{
	int i;
	ssize_t len;
	hsize_t nobj;
	herr_t err;
	int otype;
	hid_t grpid, typeid, dsid;
	char group_name[MAX_NAME];
	char memb_name[MAX_NAME];
	char fullname[MAX_NAME] = {0};
	len = H5Iget_name(gid, group_name, MAX_NAME);
	ZC_HDF5_ReplaceStr(group_name, "/", "");
	printf("Group Name: %s\n",group_name);

	err = H5Gget_num_objs(gid, &nobj);	

	int selection = 0;
	size_t outSize = 0;
	for (i = 0; i < nobj; i++) {
		printf("  Member: %d ",i);fflush(stdout);
		len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, (size_t)MAX_NAME );
		otype =  H5Gget_objtype_by_idx(gid, (size_t)i );
		switch(otype) {
			case H5G_GROUP:
				printf(" GROUP:\n");
				grpid = H5Gopen(gid,memb_name);
				scan_group_property_analysis(grpid);
				H5Gclose(grpid);
				break;
			case H5G_DATASET:
				printf(" DATASET:\n");
				dsid = H5Dopen(gid,memb_name);
				sprintf(fullname, "%s_%s", group_name, memb_name);
				printf("fullname=%s\n", fullname);
				//printf("memb_name=%s\n", memb_name);
				size_t dims[5];
				int dataType = 0;
				void* data = getDataSet(dsid, &dataType, dims);
				H5Dclose(dsid);
				
				selection = testHDF5_check(memb_name, dims[4], dims[3], dims[2], dims[1], dims[0], dataType);
				
				if(selection)
				{
					printData(data, dataType, dims);
					
					ZC_DataProperty* property = ZC_genProperties(fullname, dataType, data, dims[4], dims[3], dims[2], dims[1], dims[0]);
					ZC_printDataProperty(property);
					ZC_writeDataProperty(property, "dataProperties");					
				}
				if(data!=NULL)
					free(data);
				break;
			default:
				printf(" unknown?\n");
				break;
		}		
	}		
}

#ifdef with_SZ
void scan_group_SZ(hid_t gid, int errBoundMode, int nbErrBounds, char **errBounds) 
{
	int i;
	ssize_t len;
	hsize_t nobj;
	herr_t err;
	int otype;
	hid_t grpid, typeid, dsid;
	char group_name[MAX_NAME];
	char memb_name[MAX_NAME];
	static char fullname[MAX_NAME] = {0};
	len = H5Iget_name(gid, group_name, MAX_NAME);
	ZC_HDF5_ReplaceStr(group_name, "/", "");
	printf("Group Name: %s\n",group_name);

	err = H5Gget_num_objs(gid, &nobj);	

	int selection = 0;
	size_t outSize = 0;
	for (i = 0; i < nobj; i++) {
		printf("  Member: %d ",i);fflush(stdout);
		len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, (size_t)MAX_NAME );
		otype =  H5Gget_objtype_by_idx(gid, (size_t)i );
		switch(otype) {
			case H5G_GROUP:
				printf(" GROUP:\n");
				grpid = H5Gopen(gid,memb_name);
				scan_group_SZ(grpid, errBoundMode, nbErrBounds, errBounds);
				H5Gclose(grpid);
				break;
			case H5G_DATASET:
				printf(" DATASET:\n");
				dsid = H5Dopen(gid,memb_name);
				sprintf(fullname, "%s_%s", group_name, memb_name);
				printf("fullname=%s\n", fullname);
				//printf("memb_name=%s\n", memb_name);
				size_t dims[5];
				int dataType = 0;
				void* data = getDataSet(dsid, &dataType, dims);
				H5Dclose(dsid);
				
				selection = testHDF5_check(memb_name, dims[4], dims[3], dims[2], dims[1], dims[0], dataType);
				if(selection)
				{
					printData(data, dataType, dims);
					//TODO call SZ compression and decompression 
					ZC_DataProperty* dataProperty = NULL;
					ZC_CompareData* compareResult = NULL;
					char solName[256];
					unsigned char* bytes = NULL;
					int j = 0;
					for(j=0;j<nbErrBounds;j++)
					{
						//compression
						//SZ_Init(NULL);
						double errBound = atof(errBounds[j]);
						sprintf(solName, "sz_d(%s)", errBounds[j]);
						printf("%s: field=%s, dimension: (%zu, %zu, %zu, %zu, %zu)\n", solName, fullname, dims[4], dims[3], dims[2], dims[1], dims[0]);
						dataProperty = ZC_startCmpr(fullname, dataType, data, dims[4], dims[3], dims[2], dims[1], dims[0]);
						bytes = SZ_compress_args(dataType, data, &outSize, errBoundMode, errBound, errBound, errBound, dims[4], dims[3], dims[2], dims[1], dims[0]);
						compareResult = ZC_endCmpr(dataProperty, solName, outSize);
						//decompression
						ZC_startDec();
						void* decData = SZ_decompress(dataType, bytes, outSize,  dims[4], dims[3], dims[2], dims[1], dims[0]);
						ZC_endDec(compareResult, decData);
						freeDataProperty(dataProperty);
						freeCompareResult(compareResult);
						free(bytes);
						free(decData);
						//SZ_Finalize();
					}					
				}
				free(data);
				break;
			default:
				printf(" unknown?\n");
				break;
		}		
	}		
}
#endif

#ifdef with_ZFP
unsigned char* zfp_compression_float(float* data, int mode, double tolerance, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outSize)
{
	double realTolerance = tolerance;
	if(mode==1)
	{
		size_t i = 0;
		size_t nbEle = testHDF5_computeDataSize(r1, r2, r3, r4, r5);
		double min = data[0], max = min;
		for(i=0;i<nbEle;i++)
		{
			if(min>data[i]) min = data[i];
			if(max<data[i]) max = data[i];
		}
		realTolerance = tolerance*(max-min);
	}
	zfp_exec_policy exec = zfp_exec_serial;
	unsigned char* buffer = NULL; //to store compressed data
	zfp_field* field = NULL;
	zfp_stream* zfp = NULL;
	bitstream* stream = NULL;
	zfp_type type = zfp_type_float;
	size_t typesize = zfp_type_size(type);
	
	uint dims = 5;
	if(r5==0) dims--;
	if(r4==0) dims--;
	if(r3==0) dims--;
	if(r2==0) dims--;
	uint threads = 0;
	uint chunk_size = 0;
	zfp = zfp_stream_open(NULL);
	field = zfp_field_alloc();
	
	zfp_field_set_pointer(field, data);
	
	zfp_field_set_type(field, type);	
	switch (dims) {
		case 1:
			zfp_field_set_size_1d(field, r1);
			break;
		case 2:
			zfp_field_set_size_2d(field, r1, r2);
			break;
		case 3:
			zfp_field_set_size_3d(field, r1, r2, r3);
			break;
	}	
	
	zfp_stream_set_accuracy(zfp, realTolerance);		
	
    /* allocate buffer for compressed data */
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    if (!bufsize) {
      fprintf(stderr, "invalid compression parameters\n");
      exit(0);
    }
    buffer = (unsigned char*)malloc(bufsize+sizeof(double));
    doubleToBytes(buffer, realTolerance);
    
    if (!buffer) {
      fprintf(stderr, "cannot allocate memory\n");
      exit(0);
    }

	zfp_stream_set_execution(zfp, exec);

    /* associate compressed bit stream with memory buffer */
    stream = stream_open(buffer+sizeof(double), bufsize);
    if (!stream) {
      fprintf(stderr, "cannot open compressed stream\n");
      exit(0);
    }
    zfp_stream_set_bit_stream(zfp, stream);
	zfp_stream_rewind(zfp);

    /*if (!zfp_write_header(zfp, field, ZFP_HEADER_FULL)) {
      fprintf(stderr, "cannot write header\n");
      exit(0);
    }*/
	
	size_t zfpsize = zfp_compress(zfp, field);	
    if (zfpsize == 0) {
      fprintf(stderr, "compression failed\n");
      exit(0);
    }

	zfp_field_free(field);
	zfp_stream_close(zfp);
	stream_close(stream);
    
    *outSize = zfpsize;
    return buffer;
}

float* zfp_decompression_float(unsigned char* bytes, size_t outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{	
	zfp_exec_policy exec = zfp_exec_serial;
	unsigned char* buffer = NULL; //to store compressed data
	zfp_field* field = NULL;
	zfp_stream* zfp = NULL;
	bitstream* stream = NULL;
	zfp_type type = zfp_type_float;
	size_t typesize = zfp_type_size(type);;
	uint dims = 5;
	if(r5==0) dims--;
	if(r4==0) dims--;
	if(r3==0) dims--;
	if(r2==0) dims--;
	size_t nbEle = 0;
	switch(dims)
	{
	case 1: nbEle = r1;break;
	case 2: nbEle = r1*r2;break;
	case 3: nbEle = r1*r2*r3;break;
	case 4: nbEle = r1*r2*r3*r4;break;	
	}

	field = zfp_field_alloc();
	zfp_field_set_type(field, type);	
	switch (dims) {
		case 1:
			zfp_field_set_size_1d(field, r1);
		break;
		case 2:
			zfp_field_set_size_2d(field, r1, r2);
		break;
		case 3:
			zfp_field_set_size_3d(field, r1, r2, r3);
		break;
	}	
	zfp = zfp_stream_open(NULL);
    
    double tolerance = bytesToDouble(bytes);
    //printf("tolerance = %f\n", tolerance);
    zfp_stream_set_accuracy(zfp, tolerance);
    
    /* allocate buffer for compressed data */
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    if (!bufsize) {
      fprintf(stderr, "invalid compression parameters\n");
      exit(0);
    }
    /* associate compressed bit stream with memory buffer */
    stream = stream_open(bytes+sizeof(double), bufsize);
    if (!stream) {
      fprintf(stderr, "cannot open compressed stream\n");
      exit(0);
    }
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);
   
    //zfp_read_header(zfp, field, ZFP_HEADER_FULL);

    zfp_stream_set_execution(zfp, exec);		

    size_t rawsize = typesize * nbEle;
    float* fo = malloc(rawsize);
    if (!fo) {
      fprintf(stderr, "cannot allocate memory\n");
      exit(0);
    }
    zfp_field_set_pointer(field, fo);
    
    //compression
	zfp_decompress(zfp, field);
	
	zfp_field_free(field);
	zfp_stream_close(zfp);
	stream_close(stream);	
	
    return fo;
}

unsigned char* zfp_compression_double(double* data, int mode, double tolerance, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outSize)
{
	double realTolerance = tolerance;
	if(mode==1)
	{
		size_t i = 0;
		size_t nbEle = testHDF5_computeDataSize(r1, r2, r3, r4, r5);
		double min = data[0], max = min;
		for(i=0;i<nbEle;i++)
		{
			if(min>data[i]) min = data[i];
			if(max<data[i]) max = data[i];
		}
		realTolerance = tolerance*(max-min);
	
	}
	zfp_exec_policy exec = zfp_exec_serial;
	unsigned char* buffer = NULL; //to store compressed data
	zfp_field* field = NULL;
	zfp_stream* zfp = NULL;
	bitstream* stream = NULL;
	zfp_type type = zfp_type_double;
	size_t typesize = zfp_type_size(type);
	
	uint dims = 5;
	if(r5==0) dims--;
	if(r4==0) dims--;
	if(r3==0) dims--;
	if(r2==0) dims--;
	uint threads = 0;
	uint chunk_size = 0;
	zfp = zfp_stream_open(NULL);
	field = zfp_field_alloc();
	
	zfp_field_set_pointer(field, data);
	
	zfp_field_set_type(field, type);	
	switch (dims) {
		case 1:
			zfp_field_set_size_1d(field, r1);
			break;
		case 2:
			zfp_field_set_size_2d(field, r1, r2);
			break;
		case 3:
			zfp_field_set_size_3d(field, r1, r2, r3);
			break;
	}	
	
	zfp_stream_set_accuracy(zfp, realTolerance);		
	
    /* allocate buffer for compressed data */
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    if (!bufsize) {
      fprintf(stderr, "invalid compression parameters\n");
      exit(0);
    }
    buffer = (unsigned char*)malloc(bufsize+sizeof(double));
    doubleToBytes(buffer, realTolerance);
    
    if (!buffer) {
      fprintf(stderr, "cannot allocate memory\n");
      exit(0);
    }

	zfp_stream_set_execution(zfp, exec);

    /* associate compressed bit stream with memory buffer */
    stream = stream_open(buffer+sizeof(double), bufsize);
    if (!stream) {
      fprintf(stderr, "cannot open compressed stream\n");
      exit(0);
    }
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);

    size_t zfpsize = zfp_compress(zfp, field);	
    
    if (zfpsize == 0) {
      fprintf(stderr, "compression failed\n");
      exit(0);
    }

	zfp_field_free(field);
	zfp_stream_close(zfp);
	stream_close(stream);
    
    *outSize = zfpsize;
    return buffer;
}

double* zfp_decompression_double(unsigned char* bytes, size_t outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{	
	zfp_exec_policy exec = zfp_exec_serial;
	unsigned char* buffer = NULL; //to store compressed data
	zfp_field* field = NULL;
	zfp_stream* zfp = NULL;
	bitstream* stream = NULL;
	zfp_type type = zfp_type_double;
	size_t typesize = zfp_type_size(type);;
	uint dims = 5;
	if(r5==0) dims--;
	if(r4==0) dims--;
	if(r3==0) dims--;
	if(r2==0) dims--;
	size_t nbEle = 0;
	switch(dims)
	{
	case 1: nbEle = r1;break;
	case 2: nbEle = r1*r2;break;
	case 3: nbEle = r1*r2*r3;break;
	case 4: nbEle = r1*r2*r3*r4;break;	
	}

	field = zfp_field_alloc();
	zfp_field_set_type(field, type);	
	switch (dims) {
		case 1:
			zfp_field_set_size_1d(field, r1);
		break;
		case 2:
			zfp_field_set_size_2d(field, r1, r2);
		break;
		case 3:
			zfp_field_set_size_3d(field, r1, r2, r3);
		break;
	}	
	zfp = zfp_stream_open(NULL);
    
    double tolerance = bytesToDouble(bytes);
    //printf("tolerance = %f\n", tolerance);
    zfp_stream_set_accuracy(zfp, tolerance);
    
    /* allocate buffer for compressed data */
    size_t bufsize = zfp_stream_maximum_size(zfp, field);
    if (!bufsize) {
      fprintf(stderr, "invalid compression parameters\n");
      exit(0);
    }
    /* associate compressed bit stream with memory buffer */
    stream = stream_open(bytes+sizeof(double), bufsize);
    if (!stream) {
      fprintf(stderr, "cannot open compressed stream\n");
      exit(0);
    }
    zfp_stream_set_bit_stream(zfp, stream);
    zfp_stream_rewind(zfp);
    
    //zfp_read_header(zfp, field, ZFP_HEADER_FULL);

    zfp_stream_set_execution(zfp, exec);		

    size_t rawsize = typesize * nbEle;
    double* fo = malloc(rawsize);
    if (!fo) {
      fprintf(stderr, "cannot allocate memory\n");
      exit(0);
    }
    zfp_field_set_pointer(field, fo);
    
    //compression
	zfp_decompress(zfp, field);
	
	zfp_field_free(field);
	zfp_stream_close(zfp);
	stream_close(stream);	
	
    return fo;
}

unsigned char* zfp_compression(void* data, int dataType, int mode, double tolerance, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t* outSize)
{
	if(dataType ==0)
	{
		return zfp_compression_float((float*)data, mode, tolerance, r5, r4, r3, r2, r1, outSize);
	}
	else if(dataType == 1)
	{
		return zfp_compression_double((double*)data, mode, tolerance, r5, r4, r3, r2, r1, outSize);
	}
}

void* zfp_decompression(int dataType, unsigned char* bytes, size_t outSize, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
	if(dataType==0)
	{
		return zfp_decompression_float(bytes, outSize, r5, r4, r3, r2, r1);	
	}
	else if(dataType==1)
	{
		return zfp_decompression_double(bytes, outSize, r5, r4, r3, r2, r1);
	}
}

void scan_group_ZFP(hid_t gid, int errBoundMode, int nbErrBounds, char **errBounds) 
{	
	int i;
	ssize_t len;
	hsize_t nobj;
	herr_t err;
	int otype;
	hid_t grpid, typeid, dsid;
	char group_name[MAX_NAME];
	char memb_name[MAX_NAME];
	static char fullname[MAX_NAME] = {0};
	len = H5Iget_name(gid, group_name, MAX_NAME);
	ZC_HDF5_ReplaceStr(group_name, "/", "");
	printf("Group Name: %s\n",group_name);

	err = H5Gget_num_objs(gid, &nobj);	

	int selection = 0;
	size_t outSize = 0;
	for (i = 0; i < nobj; i++) {
		printf("  Member: %d ",i);fflush(stdout);
		len = H5Gget_objname_by_idx(gid, (hsize_t)i, memb_name, (size_t)MAX_NAME );
		otype =  H5Gget_objtype_by_idx(gid, (size_t)i );
		switch(otype) {
			case H5G_GROUP:
				printf(" GROUP:\n");
				grpid = H5Gopen(gid,memb_name);
				scan_group_ZFP(grpid, errBoundMode, nbErrBounds, errBounds);
				H5Gclose(grpid);
				break;
			case H5G_DATASET:
				printf(" DATASET:\n");
				dsid = H5Dopen(gid,memb_name);
				sprintf(fullname, "%s_%s", group_name, memb_name);
				printf("fullname=%s\n", fullname);
				//printf("memb_name=%s\n", memb_name);
				size_t dims[5];
				int dataType = 0;
				void* data = getDataSet(dsid, &dataType, dims);
				H5Dclose(dsid);
				
				selection = testHDF5_check(memb_name, dims[4], dims[3], dims[2], dims[1], dims[0], dataType);
				if(selection)
				{
					printData(data, dataType, dims);				
				
					//TODO call ZFP compression and decompression 
					ZC_DataProperty* dataProperty = NULL;
					ZC_CompareData* compareResult = NULL;
					char solName[256];
					unsigned char* bytes = NULL;
					int j = 0;
					for(j=0;j<nbErrBounds;j++)
					{
						//compression
						double errBound = atof(errBounds[j]);
						sprintf(solName, "zfp(%s)", errBounds[j]);
						printf("%s: field=%s, dimension: (%zu, %zu, %zu, %zu, %zu)\n", solName, fullname, dims[4], dims[3], dims[2], dims[1], dims[0]);
						dataProperty = ZC_startCmpr(fullname, dataType, data, dims[4], dims[3], dims[2], dims[1], dims[0]);
						
						bytes = zfp_compression(data, dataType, errBoundMode, errBound, dims[4], dims[3], dims[2], dims[1], dims[0], &outSize);
						
						compareResult = ZC_endCmpr(dataProperty, solName, outSize);
						//decompression
						ZC_startDec();
						void* decData = zfp_decompression(dataType, bytes, outSize, dims[4], dims[3], dims[2], dims[1], dims[0]);
						ZC_endDec(compareResult, decData);
						freeDataProperty(dataProperty);
						freeCompareResult(compareResult);
						free(bytes);
						free(decData);
					}
				}
				free(data);
				break;
			default:
				printf(" unknown?\n");
				break;
		}		
	}		
}

#endif
