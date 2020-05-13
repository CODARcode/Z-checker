/**
 *  @file ZC_DataSetHandler.c
 *  @author Sheng Di
 *  @date May, 2017
 *  @brief ZC Data Set Handler for handling the translation of particular data formats such as HDF5
 *  (C) 2016 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdlib.h> 
#include <stdio.h>
#include "ZC_DataSetHandler.h"
#include "ZC_CompareData.h"
#include "zc.h"
	
ZC_CompareData* ZC_constructOneVariable(char* varName, char* endian, char* type, char* dim, char* filePath)
{
	int dataType = ZC_FLOAT; //or ZC_DOUBLE
	
	ZC_CompareData* cData = ZC_constructCompareResult(varName, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	ZC_DataProperty* property = cData->property;
	char* token;
	char delim[2] = {'x', '\0'};
	size_t dimSize[5] = {0,0,0,0,0};
	int i = 0;
	if(property==NULL)
	{
		token = strtok(dim, delim);	
		if(sizeof(size_t)==4)
			dimSize[0] = atoi(token);
		else
			dimSize[0] = atol(token);
			
		for(i = 1;i<5;i++)
		{
			token = strtok(NULL, delim);
			if(token==NULL)
				break;
			if(sizeof(size_t)==4)
				dimSize[i] = atoi(token);
			else
				dimSize[i] = atol(token);
				
		}
		int dimension = i;
		size_t r5 = 0, r4 = 0, r3 = 0, r2 = 0, r1 = 0;
		switch(dimension)
		{
		case 1:
			r1 = dimSize[0];
			break;
		case 2:
			r2 = dimSize[0];
			r1 = dimSize[1];
			break;
		case 3:
			r3 = dimSize[0];
			r2 = dimSize[1];
			r1 = dimSize[2];
			break;
		case 4:
			r4 = dimSize[0];
			r3 = dimSize[1];
			r2 = dimSize[2];
			r1 = dimSize[3];
			break;
		case 5:
			r5 = dimSize[0];
			r4 = dimSize[1];
			r3 = dimSize[2];
			r2 = dimSize[3];
			r1 = dimSize[4];			
			break;
		}
		
		void* oriData;
		
		if(strcmp(type,"FLOAT")==0)
			dataType = ZC_FLOAT;
		else if(strcmp(type, "DOUBLE")==0)
			dataType = ZC_DOUBLE;
		else
		{
			printf("Error: wrong type setting, which should be either FLOAT or DOUBLE\n");
			exit(0);
		}
		
		if(strcmp(endian, "LITTLE_ENDIAN")==0)
			dataEndianType = LITTLE_ENDIAN_DATA;
		else if(strcmp(endian, "BIG_ENDIAN")==0)
			dataEndianType = BIG_ENDIAN_DATA;
		else
		{
			printf("Error: wrong endian_type setting, which should be either LITTLE_ENDIAN or BIG_ENDIAN\n");
			exit(0);
		}
		
		size_t nbEle;
		if(dataType == ZC_FLOAT)
			oriData = ZC_readFloatData(filePath, &nbEle);
		else
			oriData = ZC_readDoubleData(filePath, &nbEle);
		
		property = ZC_genProperties(varName, dataType, oriData, r5, r4, r3, r2, r1);
		ht_set(ecPropertyTable, varName, property);
	}
	
	cData->property = property;
	return cData;
}
