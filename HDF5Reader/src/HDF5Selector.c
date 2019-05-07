#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HDF5Selector.h"
#include "zc.h"

SelectionPolicy *hdf5Selector;

size_t testHDF5_computeDataSize(size_t r1, size_t r2, size_t r3, size_t r4, size_t r5)
{
	size_t nbEle = 0;
	if(r2==0)
		nbEle = r1;
	else if(r3==0)
		nbEle = r1*r2;
	else if(r4==0)
		nbEle = r1*r2*r3;
	else if(r5==0)
		nbEle = r1*r2*r3*r4;
	else
		nbEle = r1*r2*r3*r4*r5;
	return nbEle;
}

int testHDF5_computeDimension(size_t r5, size_t r4, size_t r3, size_t r2, size_t r1)
{
        int dimension;
        if(r1==0)
        {
                dimension = 0;
        }
        else if(r2==0)
        {
                dimension = 1;
        }
        else if(r3==0)
        {
                dimension = 2;
        }
        else if(r4==0)
        {
                dimension = 3;
        }
        else if(r5==0)
        {
                dimension = 4;
        }
        else
        {
                dimension = 5;
        }
        return dimension;
}


SelectionPolicy* testHDF5_Init(char* fields, int dim, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, size_t minN, int dataType)
{
	int nbVars = 0, i = 0; 
	hdf5Selector = (SelectionPolicy*)malloc(sizeof(struct SelectionPolicy));
	memset(hdf5Selector, 0, sizeof(struct SelectionPolicy));
	
	char* pch = strtok (fields,",");
	for (i = 0;pch != NULL;i++)
	{
		hdf5Selector->var[i] = (char*)malloc(strlen(pch)+1);
		strcpy(hdf5Selector->var[i], pch);
		pch = strtok (NULL, ",");
	}
	
	hdf5Selector->nbVars = i; //-1 or 0 represents no such constraint
	hdf5Selector->dimension = dim;
	
	//if r5,r4,r3,r2,r1 are all 0, then no such constraint
	hdf5Selector->r5 = r5; 
	hdf5Selector->r4 = r4; 
	hdf5Selector->r3 = r3; 
	hdf5Selector->r2 = r2; 
	hdf5Selector->r1 = r1; 
	
	hdf5Selector->minN = minN; //0 represents no such constraint
	hdf5Selector->dataType = dataType; //-1 represents no such constraint
	
	return hdf5Selector;
}

SelectionPolicy* testHDF5_Init_default()
{
	return testHDF5_Init(NULL, -1, 0, 0, 0, 0, 0, 0, -1);
}

void testHDF5_Free()
{
	SelectionPolicy* p = hdf5Selector;
	int i = 0;
	for(i=0;i<p->nbVars;i++)
		free(p->var[i]);
	free(p);
}

int testHDF5_check(char* varName, size_t r5, size_t r4, size_t r3, size_t r2, size_t r1, int dataType)
{
	if(hdf5Selector->dataType>=0)
	{
		if(hdf5Selector->dataType != dataType)
			return 0; //0 represents false
	}
	
	if(hdf5Selector->r5==0&&hdf5Selector->r4==0&&hdf5Selector->r3==0&&hdf5Selector->r2==0&&hdf5Selector->r1==0)
	{
		//no such constraint
	}
	else
	{
		if(hdf5Selector->r5!=r5 || hdf5Selector->r4!=r4 || hdf5Selector->r3!=r3 || hdf5Selector->r2!=r2 || hdf5Selector->r1!=r1)
			return 0;		
	}
	
	int dimension = testHDF5_computeDimension(r5, r4, r3, r2, r1);
	
	if(hdf5Selector->dimension>0)
	{
		switch(dimension)
		{
		case 1:
			if(hdf5Selector->dimension!=1 && hdf5Selector->dimension!=12 && hdf5Selector->dimension !=13 && hdf5Selector->dimension!=123)
				return 0;
			break;
		case 2:
			if(hdf5Selector->dimension!=2 && hdf5Selector->dimension!=12 && hdf5Selector->dimension !=23 && hdf5Selector->dimension!=123)
				return 0;
			break;
		case 3:
			if(hdf5Selector->dimension!=3 && hdf5Selector->dimension!=13 && hdf5Selector->dimension !=23 && hdf5Selector->dimension!=123)
				return 0;
			break;
		}	
	}
	
	size_t nbEle = testHDF5_computeDataSize(r1, r2, r3, r4, r5);
	if(nbEle < hdf5Selector->minN)
		return 0;
	
	int i = 0;
	int flag = 0;
	if(hdf5Selector->nbVars > 0)
	{
		for(i=0;i<hdf5Selector->nbVars;i++)
		{
			if(strcmp(varName, hdf5Selector->var[i])==0)
			{
				flag = 1;
				break;
			}
		}
	}
	else
		flag = 1;
	return flag;
}
