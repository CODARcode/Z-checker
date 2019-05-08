#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include "zc.h"
#include "ZC_util.h"
#include <math.h>
#ifdef HAVE_R
#include "ZC_callR.h"
#endif

#define RO_LITTLE_ENDIAN 0
#define RO_BIG_ENDIAN 1

#define RO_CR 0
#define RO_ERR 1
#define RO_FULL 2

#define DELIM ":"
#define DELIM2 "x"
#define DELIM3 "="

#define MAX_NB_OF_PRECISIONS 100

typedef struct ComprItem
{
	char precStr[20];
	double precision;
	double cmprTime; //in seconds
	double decoTime; //in seconds
	char cmprDataFile[256]; 
	char decoDataFile[256];
	
	void* cmprData;
	void* decoData;
	struct ComprItem* next;
} ComprItem;

typedef struct VarItem
{
	int varID;
	char varName[64];
	int dataType; //ZC_FLOAT or ZC_DOUBLE
	int endianType;
	int dimSize;
	long dim1; //low dimension
	long dim2;
	long dim3;
	long dim4;
	long dim5; //high dimension
	char dataFilePath[256];
	
	int nbCmprCases;
	ComprItem* comprHeader;
	
	struct VarItem* next; 
} VarItem;


/**
 * get the value of string (format: key=value). 
 * If value is "", then return 0; otherwise, return 1;
 * */
int getValueStr(char* str, char* result)
{
	char *buf;
	strtok(str, DELIM3);
	buf = strtok(NULL, DELIM3);
	if(buf==NULL)
		return 0;
	trim(buf);
	strcpy(result, buf);
	return 1;
}

void usage()
{
	printf("Usage: runOfflineCase <options>\n");
	printf("Options:\n");
	printf("* input information:\n");
	printf("    -N <compressor name>: the name of the compressor\n");
	printf("    -C <information file> : the file containing the data information\n");
	printf("* analysis options:\n");
	printf("    -A : perform the full analysis of the compression results\n");
	printf("    -a <metric> : perform quick analysis for specific metric\n");
	printf("        *metric options: (including all variables)\n");
	printf("          cr  : compression ratio (min, avg, max)\n");
	printf("          err : compression error (min, avg, max)\n");
	printf("          full: complete information listed as above\n");
	printf("* metadata options:\n");
	printf("    -n : print number of variables\n");
	printf("    -m : print the names of variables\n");
	printf("    -l : list complete information of all variables\n");
	printf("    -p : print all the precisions used in the analysis\n"); 
	printf("* Examples:\n");
	printf("    runOfflineCase -C varCmpr.inf -l\n");
	printf("    runOfflineCase -C varCmpr.inf -m\n");
	printf("    runOfflineCase -C varCmpr.inf -A\n");
	printf("    runOfflineCase -C varCmpr.inf -a err\n");
	printf("    runOfflineCase -C varCmpr.inf -a cr\n");
}

VarItem* createVarItemHeader()
{
	VarItem* header = (VarItem*)malloc(sizeof(VarItem));
	memset(header, 0, sizeof(VarItem));
	return header;
}

void freeComprItems(ComprItem* header)
{
	ComprItem* p = header;
	ComprItem* q = header;
	while(p!=NULL)
	{
		q = p->next;
		if(p->cmprData!=NULL)
			free(p->cmprData);
		if(p->decoData!=NULL)
			free(p->decoData);
		free(p);
		p = q;		
	}
}

void freeVarItems(VarItem* header)
{
	VarItem* p = header;
	VarItem* q = header;
	while(p!=NULL)
	{
		q = p->next;
		if(p->comprHeader!=NULL)
			freeComprItems(p->comprHeader);
		free(p);
		p = q;
	}
}

VarItem* appendOneLine_varItem(VarItem* tail, char* str, int id)
{
	VarItem* newItem = (VarItem*)malloc(sizeof(VarItem));
	memset(newItem, 0, sizeof(VarItem));

	char* token, *token2;
	token = strtok(str, DELIM);
	newItem->varID = id;
	strcpy(newItem->varName, token);
	
	token = strtok(NULL, DELIM);
	if(strcmp(token, "LITTLE_ENDIAN")==0)
		newItem->endianType = RO_LITTLE_ENDIAN;
	else if(strcmp(token, "BIG_ENDIAN")==0)
		newItem->endianType = RO_BIG_ENDIAN;	
	
	token = strtok(NULL, DELIM);
	if(strcmp(token, "FLOAT")==0)
		newItem->dataType = ZC_FLOAT;
	else if(strcmp(token, "DOUBLE")==0)
		newItem->dataType = ZC_DOUBLE;	

	token = strtok(NULL, DELIM);
	char tmpStr[64], tmpStr2[256];
	strcpy(tmpStr, token);

	token = strtok(NULL, DELIM);
	strcpy(tmpStr2, token);

	int n = 0;
	long d[5] = {0,0,0,0,0};	
	token2 = strtok(tmpStr, DELIM2);
	d[0] = atol(token2);
	n = 1;
	do{
		token2 = strtok(NULL, DELIM2);
		if(token2==NULL)
			break;
		d[n] = atol(token2);	
		n++;
	} while(1);

	switch(n)
	{
	case 1:
		newItem->dimSize = 1;
		newItem->dim1 = d[0];
		break;
	case 2:
		newItem->dimSize = 2;
		newItem->dim2 = d[0];
		newItem->dim1 = d[1];
		break; 
	case 3:
		newItem->dimSize = 3;
		newItem->dim3 = d[0];
		newItem->dim2 = d[1];
		newItem->dim1 = d[2];
		break; 
	case 4:
		newItem->dimSize = 4;
		newItem->dim4 = d[0];
		newItem->dim3 = d[1];
		newItem->dim2 = d[2];
		newItem->dim1 = d[3];
		break; 
	case 5:
		newItem->dimSize = 5;
		newItem->dim5 = d[0];
		newItem->dim4 = d[1];
		newItem->dim3 = d[2];
		newItem->dim2 = d[3];
		newItem->dim1 = d[4];
		break;
	}
	
	trim(tmpStr2);
	strcpy(newItem->dataFilePath, tmpStr2);
	
	tail->next = newItem;
	return newItem;
}

VarItem* ZC_readInfo(char* filePath, int *varCount)
{
	char buf[MAX_MSG_LENGTH], valueBuf[MAX_MSG_LENGTH];
	int status = 0;
	memset(buf, 0, MAX_MSG_LENGTH);
	int i = 0;

	FILE *fp = fopen(filePath, "r");
	if(fp == NULL)
	{
		printf("failed to open the file %s\n", filePath);
		exit(0);
	}

	VarItem *header = createVarItemHeader();
	VarItem *tail = header; //the last element
	ComprItem *comprPreItem = NULL, *newCItem = NULL;
	while(!feof(fp))
	{
		fgets(buf, MAX_MSG_LENGTH, fp); // already including \n
		if(buf[0]=='#' || strcmp(buf, "")==0 || strcmp(buf, "\n")==0)
			continue;		
		if(startsWith("ori_data", buf))		
		{
			status = getValueStr(buf, valueBuf);
			if(status==0)
			{
				printf("Error: the information of one ori_data item is NULL\n");\
				exit(0);
			}
			tail = appendOneLine_varItem(tail, valueBuf, i);			
			tail->comprHeader = (ComprItem*)malloc(sizeof(ComprItem));
			memset(tail->comprHeader, 0, sizeof(ComprItem));
			comprPreItem = tail->comprHeader;
			i++;
		}
		else
		{
			status = getValueStr(buf, valueBuf);
			if(status==0)
				continue;
			if(startsWith("prec", buf))
			{
				newCItem = (ComprItem*)malloc(sizeof(ComprItem));
				memset(newCItem, 0, sizeof(ComprItem));
				comprPreItem->next = newCItem;
				strcpy(newCItem->precStr, valueBuf);
				newCItem->precision = atof(valueBuf);
				comprPreItem = newCItem;
			}
			else if(startsWith("cpr_data", buf))
				strcpy(newCItem->cmprDataFile, valueBuf);
			else if(startsWith("dec_data", buf))
				strcpy(newCItem->decoDataFile, valueBuf);
			else if(startsWith("cpr_time", buf))
				newCItem->cmprTime = atof(valueBuf);
			else if(startsWith("dec_time", buf))
				newCItem->decoTime = atof(valueBuf);
			else
			{
				printf("Error: unrecoganized key in the line: %s\n", buf);
				exit(0);
			}
		}
	}
	*varCount = i;

	fclose(fp);	
	return header;
}

int main(int argc, char* argv[])
{
	int printNbVars = 0;
	int printVarNames = 0;
	int listAllVarInfo = 0;
	int printPrecisions = 0;
	int fullAnalysis = 0;

	char* inPath = NULL;
	char* compressorName = "user_compressor";
	int analysisType = -1;
	char* buf;
	
	if(argc==1)
	{
		usage();
		return 0;
	}

	int i = 0;
	for(i=1;i<argc;i++)
	{
		if (argv[i][0] != '-' || argv[i][2])
			usage();
		switch (argv[i][1])
		{
		case 'N':
			if (++i == argc)
				usage();
			compressorName = argv[i];					
			break;
		case 'n': 
			printNbVars = 1;
			break;
		case 'm': 
			printVarNames = 1;
			break;
		case 'l':
			listAllVarInfo = 1;
			break;
		case 'p':
			printPrecisions = 1;
			break;
		case 'A': 
			fullAnalysis = 1;
			break;
		case 'C':
			if (++i == argc)
				usage();
			inPath = argv[i];		
			break;
		case 'a':
			if (++i == argc)
				usage();
			buf = argv[i];
			if(strcmp(buf, "cr")==0)
				analysisType = RO_CR;
			else if(strcmp(buf, "err")==0)
				analysisType = RO_ERR;
			else if(strcmp(buf, "full")==0)
				analysisType = RO_FULL;
			else 
			{
				printf("Err: unrecoganized analysis_type. Please set -a option to only cr, err, or full.");
				usage();
				exit(0);
			}
			break;
		default: 
			usage();
			break;
		}
	}

	if(inPath == NULL)
	{
		printf("Error: you need to specify either a raw binary data file or a compressed data file as input\n");
		usage();
		exit(0);
	}
	
	ZC_Init_NULL();

	//get Rscript path if needed
#ifdef HAVE_R
	char R_buf[MAX_MSG_LENGTH];
	ZC_readFirstLine(inPath, R_buf);
	int rPathSetting = startsWith("#RscriptPath", R_buf);
	if(rPathSetting)
	{
		char *RsPath = strchr(R_buf, '=');
		int len = strlen(RsPath);
		RsPath[len-1]=0;
		//rscriptPath = (char*)malloc(MAX_MSG_LENGTH);
		char *tRsPath = RsPath+1;
		trim(tRsPath);
		strcpy(rscriptPath, tRsPath);
	}
	// Intialize the R environment.
	int r_argc = 2;
	char *r_argv[] = { "R", "--silent" };
	Rf_initEmbeddedR(r_argc, r_argv);
	source(rscriptPath);	
#endif	
	
	//read info file
	int varCount = 0;
	VarItem* varItemHeader = ZC_readInfo(inPath, &varCount);
	VarItem* p = varItemHeader->next;
	
	if(printNbVars)
	{
		printf("Nubmer of variables = %d\n", varCount);
	}
		
	if(printVarNames)
	{
		p = varItemHeader->next;
		printf("var names: %s", p->varName);
		p = p->next;
		while(p!=NULL)
		{
			printf(" ");
			printf("%s", p->varName);
			p = p->next;
		}
		printf("\n");
	}

	if(printPrecisions)
	{
		int i = 0, n = 0;
		double precList[MAX_NB_OF_PRECISIONS], precision;
		memset(precList, 0, sizeof(double)*MAX_NB_OF_PRECISIONS);
		ComprItem* q = NULL;
		while(p!=NULL)
		{
			VarItem* varItem = p;
			
			q = varItem->comprHeader->next;
			while(q!=NULL)
			{
				precision = q->precision;
				for(i=0;i<MAX_NB_OF_PRECISIONS;i++)
				{
					if(precList[i] == precision)
						break;
					
					if(precList[i]==0)
					{
						precList[i] = precision;
						n++;
						break;
					}
				}
				if(i == MAX_NB_OF_PRECISIONS)
				{
					printf("Err: too many precisions. Please change the MAX_NB_OF_PRECISIONS in the code. \n");
					exit(0);
				}				
				q = q->next;
			}
			p = p->next;
		}
		
		printf("Precisions: ");
		for(i=0;i<n;i++)
			printf("%f ", precList[i]);
		printf("\n");
	}
	
	if(listAllVarInfo)
	{
		int i = 0;
		VarItem* q = varItemHeader->next;
		while(q!=NULL)
		{
			VarItem* varItem = q;
			
			char *data_type = varItem->dataType==ZC_FLOAT?"FLOAT":"DOUBLE";
			char *endian_type = varItem->endianType==RO_LITTLE_ENDIAN?"LITTLE_ENDIAN":"BIG_ENDIAN";
			printf("%d\t- name: %s ; dataType: %s ; endianType: %s ; dimSize: %d\n", i, varItem->varName, data_type, endian_type, varItem->dimSize);
			printf("\t  dim sizes: %ld %ld %ld %ld %ld\n", varItem->dim1, varItem->dim2, varItem->dim3, varItem->dim4, varItem->dim5);
			q = q->next;
			i++;
		}			
	}
	
	int datatype = 0;
	void *oriData_v = NULL, *decData_v = NULL;
	size_t len, oriDataSize, cmprSize = 0;
	ComprItem* q  = NULL;
	i = 0;
	size_t j = 0;
	double minErr = 0, maxErr = 0, avgErr = 0, err = 0;	
	switch(analysisType)
	{
	case RO_CR: 
		p = varItemHeader->next;
		while(p!=NULL)
		{	
			if(p->dataType==ZC_FLOAT)
				datatype = 4;
			else if(p->dataType==ZC_DOUBLE)
				datatype = 8;
			
			len = ZC_computeDataLength(p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);
			oriDataSize = len*datatype;
			
			printf("[%d]\t varName: %s\n", i, p->varName);
			q = p->comprHeader;
			while(q->next!=NULL)
			{
				ComprItem* ci = q->next;
				double prec = ci->precision;
				int cmprFileExists = ZC_check_file_exists(ci->cmprDataFile);
				if(cmprFileExists)
				{
					cmprSize = ZC_checkFileSize(ci->cmprDataFile);
					double cr = (oriDataSize*1.0/cmprSize);
					printf("\t\t Precision=%f, CR = %f, bitrate = %f\n", prec, cr, datatype*8.0/cr);
				}
				else
					printf("\t\t Precision=%f, ERROR: missing compressed file (%s)\n", prec, ci->cmprDataFile);				
				q = q->next;
			}
			p = p->next;
			i++;
		}
		break;
	case RO_ERR:
		p = varItemHeader->next;
		size_t nbEle = 0;
		while(p!=NULL)
		{	
			len = ZC_computeDataLength(p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);
			oriDataSize = len*datatype;
			
			if(p->dataType==ZC_FLOAT)
				oriData_v = ZC_readFloatData(p->dataFilePath, &nbEle);
			else if(p->dataType == ZC_DOUBLE)
				oriData_v = ZC_readDoubleData(p->dataFilePath, &nbEle);
				
			if(nbEle!=len)
			{
				free(oriData_v);
				printf("Error: inconsistent number of elements between the info file and original data file. \n");
				printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
				exit(0);
			}
			
			printf("[%d]\t varName: %s\n", i, p->varName);
			q = p->comprHeader;
			while(q->next!=NULL)
			{
				ComprItem* ci = q->next;
				double prec = ci->precision;
				int decoFileExists = ZC_check_file_exists(ci->decoDataFile);
				if(decoFileExists)
				{
					if(p->dataType==ZC_FLOAT)
					{
						float* decData = ZC_readFloatData(ci->decoDataFile, &nbEle);
						if(nbEle!=len)
						{
							free(oriData_v);
							free(decData);
							printf("Error: inconsistent number of elements between the info file and decompressed data file. \n");
							printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
							exit(0);
						}					
						float* oriData = (float*)oriData_v;
						err = fabs(oriData[0] - decData[0]);
						minErr = maxErr = avgErr = err;
						for(j=1;j<nbEle;j++)
						{
							err = fabs(oriData[j] - decData[j]);
							if(err<minErr)
								minErr = err;
							if(err>maxErr)
								maxErr = err;
							avgErr += err;
						}
						avgErr = avgErr/nbEle;						
					}
					else if(p->dataType == ZC_DOUBLE)
					{
						double* decData = ZC_readDoubleData(ci->decoDataFile, &nbEle);	
						if(nbEle!=len)
						{
							free(oriData_v);
							free(decData);
							printf("Error: inconsistent number of elements between the info file and decompressed data file. \n");
							printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
							exit(0);
						}							
						double* oriData = (double*)oriData_v;
						err = fabs(oriData[0] - decData[0]);
						minErr = maxErr = avgErr = err;
						for(j=1;j<nbEle;j++)
						{
							err = fabs(oriData[j] - decData[j]);
							if(err<minErr)
								minErr = err;
							if(err>maxErr)
								maxErr = err;
							avgErr += err;
						}
						avgErr = avgErr/nbEle;								
					}
				
					printf("\t\t Precision=%f, minErr = %f, avgErr = %f, maxErr = %f\n", prec, minErr, avgErr, maxErr);
				}
				else
					printf("\t\t Precision=%f, ERROR: missing decompressed file (%s)\n", prec, ci->cmprDataFile);				
				q = q->next;
			}
			p = p->next;
			i++;
		}
		break;
	case RO_FULL:
		p = varItemHeader->next;
		while(p!=NULL)
		{	
			len = ZC_computeDataLength(p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);
			oriDataSize = len*datatype;
			
			if(p->dataType==ZC_FLOAT)
				oriData_v = ZC_readFloatData(p->dataFilePath, &nbEle);
			else if(p->dataType == ZC_DOUBLE)
				oriData_v = ZC_readDoubleData(p->dataFilePath, &nbEle);
			if(nbEle!=len)
			{
				free(oriData_v);
				printf("Error: inconsistent number of elements between the info file and original data file. \n");
				printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
				exit(0);
			}
			
			printf("[%d]\t varName: %s\n", i, p->varName);
			q = p->comprHeader;
			double psnr = 0, nrmse = 0, acEff = 0;
			while(q->next!=NULL)
			{
				ComprItem* ci = q->next;
				double prec = ci->precision;
				int decoFileExists = ZC_check_file_exists(ci->decoDataFile);
				if(decoFileExists)
				{
					if(p->dataType==ZC_FLOAT)
					{
						float* decData = ZC_readFloatData(ci->decoDataFile, &nbEle);
						if(nbEle!=len)
						{
							free(oriData_v);
							free(decData);
							printf("Error: inconsistent number of elements between the info file and decompressed data file. \n");
							printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
							exit(0);
						}
					
						float* oriData = (float*)oriData_v;
						double Max = 0, Min = 0;
						Max = oriData[0];
						Min = oriData[0];
						maxErr = fabs(decData[0] - oriData[0]);
						double sum1 = 0, sum2 = 0;
						for (j = 0; j < nbEle; j++)
						{
							sum1 += oriData[j];
							sum2 += decData[j];
						}
						double mean1 = sum1/nbEle;
						double mean2 = sum2/nbEle;

						double sum3 = 0, sum4 = 0;
						double sum = 0, prodSum = 0;

						for (j = 0; j < nbEle; j++)
						{
							if (Max < oriData[j]) Max = oriData[j];
							if (Min > oriData[j]) Min = oriData[j];
							
							double err = fabs(decData[j] - oriData[j]);

							if(maxErr < err)
								maxErr = err;
							if(minErr > err)
								minErr = err;
							avgErr += err;
							prodSum += (oriData[j]-mean1)*(decData[j]-mean2);
							sum3 += (oriData[j] - mean1)*(oriData[j]-mean1);
							sum4 += (decData[j] - mean2)*(decData[j]-mean2);
							sum += err*err;	
						}
						double std1 = sqrt(sum3/nbEle);
						double std2 = sqrt(sum4/nbEle);
						double ee = prodSum/nbEle;
						acEff = ee/std1/std2;

						double mse = sum/nbEle;
						double range = Max - Min;
						psnr = 20*log10(range)-10*log10(mse);
						nrmse = sqrt(mse)/range;	
						avgErr = avgErr/nbEle;					
					}
					else if(p->dataType == ZC_DOUBLE)
					{
						double* decData = ZC_readDoubleData(ci->decoDataFile, &nbEle);	
						if(nbEle!=len)
						{
							free(oriData_v);
							free(decData);
							printf("Error: inconsistent number of elements between the info file and decompressed data file. \n");
							printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
							exit(0);
						}
					
						double* oriData = (double*)oriData_v;
						double Max = 0, Min = 0;
						Max = oriData[0];
						Min = oriData[0];
						maxErr = fabs(decData[0] - oriData[0]);
						double sum1 = 0, sum2 = 0;
						for (j = 0; j < nbEle; j++)
						{
							sum1 += oriData[j];
							sum2 += decData[j];
						}
						double mean1 = sum1/nbEle;
						double mean2 = sum2/nbEle;

						double sum3 = 0, sum4 = 0;
						double sum = 0, prodSum = 0;

						for (j = 0; j < nbEle; j++)
						{
							if (Max < oriData[j]) Max = oriData[j];
							if (Min > oriData[j]) Min = oriData[j];
							
							double err = fabs(decData[j] - oriData[j]);

							if(maxErr < err)
								maxErr = err;
							if(minErr > err)
								minErr = err;
							avgErr += err;
							prodSum += (oriData[j]-mean1)*(decData[j]-mean2);
							sum3 += (oriData[j] - mean1)*(oriData[j]-mean1);
							sum4 += (decData[j] - mean2)*(decData[j]-mean2);
							sum += err*err;	
						}
						double std1 = sqrt(sum3/nbEle);
						double std2 = sqrt(sum4/nbEle);
						double ee = prodSum/nbEle;
						acEff = ee/std1/std2;

						double mse = sum/nbEle;
						double range = Max - Min;
						psnr = 20*log10(range)-10*log10(mse);
						nrmse = sqrt(mse)/range;	
						avgErr = avgErr/nbEle;					
					}
					
					printf("\t\t Precision=%f, minErr = %f, avgErr = %f, maxErr = %f, psnr = %f, nrmse = %f, corr_in_between = %f\n", prec, minErr, avgErr, maxErr, psnr, nrmse, acEff);
				}
				else
					printf("\t\t Precision=%f, ERROR: missing decompressed file (%s)\n", prec, ci->cmprDataFile);				
				q = q->next;
				free(decData_v);
			}
			p = p->next;
			free(oriData_v);
			i++;
		}	
		break;
	}
	
	char cmdBuf[1024];
	memset(cmdBuf, 0, 1024);
	
	ZC_DataProperty* property = NULL;
	if(fullAnalysis)
	{
		sprintf(cmdBuf, "mkdir -p dataProperties");
		system(cmdBuf);
		
		p = varItemHeader->next;
		void *oriData_v;
		size_t nbEle = 0;
		char compressionCase[128];
		while(p!=NULL)
		{	
			if(p->dataType==ZC_FLOAT)
				datatype = 4;
			else if(p->dataType==ZC_DOUBLE)
				datatype = 8;
			
			len = ZC_computeDataLength(p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);
			oriDataSize = len*datatype;
			
			if(p->dataType==ZC_FLOAT)
				oriData_v = ZC_readFloatData(p->dataFilePath, &nbEle);
			else if(p->dataType == ZC_DOUBLE)
				oriData_v = ZC_readDoubleData(p->dataFilePath, &nbEle);
			else
			{
				printf("Error: unrecoganized data type: %d\n", p->dataType);
				exit(0);
			}
			if(nbEle!=len)
			{
				free(oriData_v);
				printf("Error: inconsistent number of elements between the info file and original data file. \n");
				printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
				exit(0);
			}
			
			printf("[%d]\t data analysis for varName: %s\n", i, p->varName);
			
			if(p->dataType==ZC_FLOAT)
				property = ZC_genProperties(p->varName, ZC_FLOAT, (float*)oriData_v, p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);
			else if(p->dataType == ZC_DOUBLE)
				property = ZC_genProperties(p->varName, ZC_DOUBLE, (double*)oriData_v, p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);			
			
			ZC_printDataProperty(property);

			ZC_writeDataProperty(property, "dataProperties");
			
			ZC_CompareData* compareResult = NULL;
			//processing compression results
			q = p->comprHeader;
			while(q->next!=NULL)
			{
				ComprItem* ci = q->next;
				double prec = ci->precision;
				printf("- data distortion analysis: %s with precision = %f ....\n", p->varName, prec);
				int decoFileExists = ZC_check_file_exists(ci->decoDataFile);
				if(decoFileExists)
				{
					if(p->dataType == ZC_FLOAT)
					{
						float* decData = ZC_readFloatData(ci->decoDataFile, &nbEle);
						if(nbEle!=len)
						{
							free(oriData_v);
							free(decData);
							printf("Error: inconsistent number of elements between the info file and decompressed data file. \n");
							printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
							exit(0);
						}
						
						compareResult = ZC_compareData(p->varName, ZC_FLOAT, (float*)oriData_v, decData, p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);	
					}
					else if(p->dataType == ZC_DOUBLE)
					{
						double* decData = ZC_readDoubleData(ci->decoDataFile, &nbEle);	
						if(nbEle!=len)
						{
							free(oriData_v);
							free(decData);
							printf("Error: inconsistent number of elements between the info file and decompressed data file. \n");
							printf("data size %ld x %ld x %ld x %ld x %ld doesn't match file size (%zu elements).\n", p->dim5, p->dim4, p->dim3, p->dim2, p->dim1, nbEle); 
							exit(0);
						}
						
						compareResult = ZC_compareData(p->varName, ZC_DOUBLE, (double*)oriData_v, decData, p->dim5, p->dim4, p->dim3, p->dim2, p->dim1);			
					}
					else
					{
						printf("Error: wrong data type\n");
						exit(0);
					}

					compareResult->compressSize = ZC_checkFileSize(ci->cmprDataFile);
					compareResult->compressRatio = (oriDataSize*1.0/compareResult->compressSize);
					compareResult->rate = datatype*8.0/compareResult->compressRatio;
					compareResult->compressTime = ci->cmprTime;
					compareResult->decompressTime = ci->decoTime;
					if(compareResult->compressTime!=0)
						compareResult->compressRate = oriDataSize*1.0/compareResult->compressTime;
					else
						compareResult->compressRate = 0;
					if(compareResult->decompressTime!=0)
						compareResult->decompressRate = oriDataSize*1.0/compareResult->decompressTime;					
					else
						compareResult->decompressRate = 0;
					ZC_printCompressionResult(compareResult);

					sprintf(compressionCase, "%s(%s)", compressorName, ci->precStr);
					ZC_writeCompressionResult(compareResult, compressionCase, p->varName, "compressionResults");

				}
				else
					printf("\t\t Precision=%f, ERROR: missing decompressed file (%s)\n", prec, ci->cmprDataFile);				
				q = q->next;
			}
			p = p->next;
			i++;
		}				
	}
	
	ZC_Finalize();
}
