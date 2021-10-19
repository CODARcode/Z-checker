/**
 *  @file   conf.c
 *  @author Sheng Di (sdi1@anl.gov or disheng222@gmail.com)
 *  @date   2015.
 *  @brief  Configuration loading functions for the EC library.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ZC_util.h"
#include "ZC_conf.h"
#include "zc.h"
#include "iniparser.h"
#include "ZC_rw.h"
#include "ZC_DataProperty.h"

ZC_DataProperty* loadProperty(const char* property_dir, const char* fileName)
{
	char tmpPathBuf[ZC_BUFS_LONG], fullPathBuf[ZC_BUFS_LONG], softLinkPath[ZC_BUFS_LONG], linkCmd[ZC_BUFS_LONG];
	
	char* propertyVarName = rmFileExtension(fileName);
	ZC_DataProperty* p = (ZC_DataProperty*)ht_get(ecPropertyTable, propertyVarName);
	if(p==NULL)
	{
		//todo: deal with full path
		sprintf(tmpPathBuf, "%s/%s", property_dir, fileName);
		//free(compResultCaseName);
		ZC_DataProperty* property = ZC_loadDataProperty(tmpPathBuf);
		ht_set(ecPropertyTable, propertyVarName, property);
		
		sprintf(softLinkPath, "dataProperties/%s", fileName);					
		if (access(softLinkPath, F_OK) != 0)
		{
			updateLinkFullPath(tmpPathBuf, fullPathBuf);
			sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
			system(linkCmd);				
		}
		
		if(autocorrFlag)
		{
			sprintf(softLinkPath, "dataProperties/%s.autocorr", propertyVarName);					
			if (access(softLinkPath, F_OK) != 0)
			{
				sprintf(tmpPathBuf, "%s/%s.autocorr", property_dir, propertyVarName);
				updateLinkFullPath(tmpPathBuf, fullPathBuf);
				sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
				system(linkCmd);
			}			
		}

		if(fftFlag)
		{
			sprintf(softLinkPath, "dataProperties/%s.fft", propertyVarName);
			if (access(softLinkPath, F_OK) != 0)
			{					
				sprintf(tmpPathBuf, "%s/%s.fft", property_dir, propertyVarName);
				updateLinkFullPath(tmpPathBuf, fullPathBuf);
				sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
				system(linkCmd);								
			}
			
			sprintf(softLinkPath, "dataProperties/%s.fft.amp", propertyVarName);
			if (access(softLinkPath, F_OK) != 0)
			{
				sprintf(tmpPathBuf, "%s/%s.fft.amp", property_dir, propertyVarName);
				updateLinkFullPath(tmpPathBuf, fullPathBuf);
				sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
				system(linkCmd);
			}			
		}
		p = property;
	}	
	free(propertyVarName);
	return p;
}

/***
 * 
 * load_ecVisDecDataTables() is developed for the compression/decompression to be launched by compressor (based on specified compression ratios)
 * 
 * */
int load_ecVisDecDataTables()
{
	char buffer[256];
	char tableFile[100]; 
	strcpy(tableFile, "compareCompressors/ecVisDecDataTables.txt");
	int lineCount = 0;
	StringLine* header = ZC_readLines(tableFile, &lineCount);
	int varCount = 0;
	
	ecVisDecDataTables = ht_create(HASHTABLE_SIZE_SMALL); //64 entries
	
	ZC_readFirstLine(tableFile, buffer);//read meta data line: var_count=4
	char* tmpStr = strtok(buffer, "=");
	tmpStr = strtok(NULL, "=");
	varCount = atoi(tmpStr);
	
	hashtable_t* varComprMap = NULL;
	CompressorCRVisElement* cmprVisE = NULL;
	
	char* varName = NULL;
	StringLine* p = header->next;
	while(p!=NULL)
	{
		strcpy(buffer, p->str);
		if(ZC_startsWith(buffer, "=====varName"))
		{
			strtok(buffer, ":");

			tmpStr = strtok(NULL, ":");
			varName = createLine(tmpStr);
			
			varComprMap = ht_create(HASHTABLE_SIZE_SMALL);
			ht_set(ecVisDecDataTables, varName, varComprMap);
		}
		else if(ZC_startsWith(buffer, "cmprName"))
		{
			strcpy(buffer, p->str);
			strtok(buffer, "=");
			tmpStr = strtok(NULL, "=");
			char* compressorName = createLine(tmpStr);
			
			cmprVisE = (CompressorCRVisElement*)malloc(sizeof(struct CompressorCRVisElement));
			memset(cmprVisE, 0, sizeof(struct CompressorCRVisElement));
			cmprVisE->varName = varName; 
			cmprVisE->compressorName = compressorName;
			cmprVisE->CRVisDataMap = ht_create(HASHTABLE_SIZE_SMALL);
			ht_set(varComprMap, compressorName, cmprVisE);			
		}
		else if(ZC_startsWith(buffer, "CR")) //data lines: such as CR=20 ErrorBound=0.00082
		{
			hashtable_t* map = cmprVisE->CRVisDataMap;
			
			ZCVisDecDataElement* visEle = (ZCVisDecDataElement*) malloc(sizeof(struct ZCVisDecDataElement));
			memset(visEle, 0, sizeof(struct ZCVisDecDataElement));			
			
			strcpy(buffer, p->str);
			char first[256], second[256];
			char* q = strtok(buffer, " ");
			strcpy(first, q);
			
			q = strtok(NULL, " ");
			strcpy(second, q);
			
			strtok(first, "=");
			char* compressionRatio = strtok(NULL, "=");
			
			strtok(second, "=");
			char* errorSetting = createLine(strtok(NULL, "="));
			
			visEle->varName = cmprVisE->varName;
			visEle->compressorName = cmprVisE->compressorName;
			//visEle->dataProperty = prop;

			visEle->errorSetting_str = errorSetting; 
			visEle->errorSetting = atof(errorSetting);
			
			sprintf(buffer, "%s(%s):%s", visEle->compressorName, visEle->errorSetting_str, visEle->varName);
			visEle->compressionCaseName = createLine(buffer);
			
			ht_set(map,compressionRatio, visEle);
		}
		p = p->next;
	}
	
	return varCount;
}

int getCompressorID(const char* compressorName)
{
	if(strcmp(compressorName,"sz_d")==0 || strcmp(compressorName, "sz")==0)
		return COMPRESSOR_SZ;
	else if(strcmp(compressorName, "zfp")==0)
		return COMPRESSOR_ZFP;
	else if(strcmp(compressorName, "fpzip")==0)
		return COMPRESSOR_FPZIP;
	else if(strcmp(compressorName, "mgard")==0)
		return COMPRESSOR_MGARD;
	else if(strcmp(compressorName, "bitgrooming")==0)
		return COMPRESSOR_BITGROOMING;
	else if(strcmp(compressorName, "digitrounding")==0)
		return COMPRESSOR_DIGITROUNDING;
		
	return -1;
} 

int isInPlotDecCompressors(const char* compressorName)
{
	int compressorID = getCompressorID(compressorName);
	int i = 0;
	for(i=0;i<nbPlotCompressors;i++)
	{
		if(plotCompressors[i]==compressorID)
			return 1;
	}
	return 0;
}
 
 
/*-------------------------------------------------------------------------*/
int ZC_ReadConf() {
    // Check access to EC configuration file and load dictionary
    int x = 1;
    char *endianTypeString;
    char *checkingStatusString;
    char *executionModeString;
    char *visModeString;
    dictionary *ini;

    if (access(zc_cfgFile, F_OK) != 0)
    {
        printf("[ZC] Configuration file NOT accessible.\n");
        return ZC_NSCS;
    }
    ini = iniparser_load(zc_cfgFile);
    if (ini == NULL)
    {
        printf("[ZC] Iniparser failed to parse the conf. file.\n");
        return ZC_NSCS;
    }

#ifdef HAVE_R
	char* rscriptPath_ = iniparser_getstring(ini, "ENV:RscriptPath", NULL);	
	if(rscriptPath_ != NULL)
		strcpy(rscriptPath, rscriptPath_);
#endif

	endianTypeString = iniparser_getstring(ini, "ENV:dataEndianType", NULL);
	if(strcmp(endianTypeString, "LITTLE_ENDIAN_DATA")==0)
		dataEndianType = LITTLE_ENDIAN_DATA;
	else if(strcmp(endianTypeString, "BIG_ENDIAN_DATA")==0)
		dataEndianType = BIG_ENDIAN_DATA;
	else
	{
		printf("Error: Wrong dataEndianType: please set it correctly in zc.config.\n");
		exit(0);
	}
	if(checkingStatus==-1) //this means that the checkingStatus has been set externally, e.g., in analyzeDataProperty.c
	{
		checkingStatusString = iniparser_getstring(ini, "ENV:checkingStatus", NULL);
		if(strcmp(checkingStatusString, "PROBE_COMPRESSOR")==0 || strcmp(checkingStatusString, "probe_compressor")==0)
			checkingStatus = PROBE_COMPRESSOR;
		else if(strcmp(checkingStatusString, "ANALYZE_DATA")==0 || strcmp(checkingStatusString, "analyze_data")==0)
			checkingStatus = ANALYZE_DATA;
		else if(strcmp(checkingStatusString, "COMPARE_COMPRESSOR")==0 || strcmp(checkingStatusString, "compare_compressor")==0)
			checkingStatus = COMPARE_COMPRESSOR;
		else
		{
			checkingStatus = -1;
			printf("Error: Wrong checking status in the configuration setting. \n");
			printf("Example: checkingStatus = PROBE_COMPRESSOR, ANALYZE_DATA, COMPARE_COMPRESSOR\n");
			exit(0);
		}
	}	

	executionModeString = iniparser_getstring(ini, "ENV:executionMode", NULL);
	if(strcmp(executionModeString, "OFFLINE")==0)
		executionMode = ZC_OFFLINE;
	else
		executionMode = ZC_ONLINE;
	
	
	char OFFLINE_STR[100];
	strcpy(OFFLINE_STR, "OFFLINE");	
	visModeString = iniparser_getstring(ini, "ENV:visMode", OFFLINE_STR);
	if(strcmp(visModeString, OFFLINE_STR)==0)
		visMode = 0;
	else
		visMode = 1;

	char *y = (char*)&x;
	
	if(*y==1)
		sysEndianType = LITTLE_ENDIAN_SYSTEM;
	else //=0
		sysEndianType = BIG_ENDIAN_SYSTEM;

	// Reading/setting detection parameters
	//ZC_workspaceDir = iniparser_getstring(ini, "PARAMETER:workspaceDir", NULL);
	//if(opendir(ZC_workspaceDir)==NULL)
	//	mkdir(ZC_workspaceDir,0775);
	
	minValueFlag= (int)iniparser_getint(ini, "DATA:minValue", 1);
	maxValueFlag= (int)iniparser_getint(ini, "DATA:maxValue", 1);
	valueRangeFlag= (int)iniparser_getint(ini, "DATA:valueRange", 1);
	avgValueFlag= (int)iniparser_getint(ini, "DATA:avgValue", 1);
	entropyFlag= (int)iniparser_getint(ini, "DATA:entropy", 1);
	entropyFloatpointFlag = (int)iniparser_getint(ini, "DATA:entropyFloatpoint", 0);
	autocorrFlag= (int)iniparser_getint(ini, "DATA:autocorr", 0);
	autocorr3DFlag = (int)iniparser_getint(ini, "DATA:autocorr3D", 0);
	fftFlag= (int)iniparser_getint(ini, "DATA:fft", 0);
	lapFlag= (int)iniparser_getint(ini, "DATA:lap", 0);
	
	compressTimeFlag = (int)iniparser_getint(ini, "COMPARE:compressTime", 1);
	decompressTimeFlag = (int)iniparser_getint(ini, "COMPARE:decompressTime", 1);
	compressSizeFlag = (int)iniparser_getint(ini, "COMPARE:compressSize", 1);
	
	minAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:minAbsErr", 1);
	avgAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:avgAbsErr", 1);
	maxAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:maxAbsErr", 1);
	errAutoCorrFlag = (int)iniparser_getint(ini, "COMPARE:errAutoCorr", 0);
	errAutoCorr3DFlag = (int)iniparser_getint(ini, "COMPARE:errAutoCorr3D", 0);
	absErrPDFFlag = (int)iniparser_getint(ini, "COMPARE:absErrPDF", 0);
	pwrErrPDFFlag = (int)iniparser_getint(ini, "COMPARE:pwrErrPDF", 0);
	
	minRelErrFlag = (int)iniparser_getint(ini, "COMPARE:minRelErr", 1);
	avgRelErrFlag = (int)iniparser_getint(ini, "COMPARE:avgRelErr", 1);
	maxRelErrFlag = (int)iniparser_getint(ini, "COMPARE:maxRelErr", 1);

	rmseFlag = (int)iniparser_getint(ini, "COMPARE:rmse", 1);
	nrmseFlag = (int)iniparser_getint(ini, "COMPARE:nrmse", 1);
	snrFlag = (int)iniparser_getint(ini, "COMPARE:snr", 1);
	psnrFlag = (int)iniparser_getint(ini, "COMPARE:psnr", 1);

	valErrCorrFlag = (int)iniparser_getint(ini, "COMPARE:valErrCorr", 0);

	pearsonCorrFlag = (int)iniparser_getint(ini, "COMPARE:pearsonCorr", 1);
	
	KS_testFlag = (int)iniparser_getint(ini, "COMPARE:KS_test", 0);
	SSIMFlag = (int)iniparser_getint(ini, "COMPARE:ssim", 0);
	SSIMIMAGE2DFlag = (int)iniparser_getint(ini, "COMPARE:ssimImage2D", 0);
	
	derivativeOrder1_sep_maxDiffFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder1_sep_maxDiff", 0);
	derivativeOrder1_sep_psnrFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder1_sep_psnr", 0);
	derivativeOrder1_sep_ssimFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder1_sep_ssim", 0);	
	
	derivativeOrder1_psnrFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder1_psnr", 0);
	derivativeOrder2_psnrFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder2_psnr", 0);
	derivativeOrder1_ssimFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder1_ssim", 0);
	derivativeOrder2_ssimFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder2_ssim", 0);	

	derivativeOrder1_sobolevFlag = (int)iniparser_getint(ini, "COMPARE:derivativeOrder1_sobolev", 0);	

	plotImageFlag = (int)iniparser_getint(ini, "PLOT:plotSliceImage", 0);
	
	memset(plotCRs_str, 0, MAX_VIS_DEC_CRS);
	memset(plotCompressors, 0, sizeof(int)*MAX_NB_CMPR_CASES);
	
	plotDecImageFlag = (int)iniparser_getint(ini, "PLOT:plotDecSliceImage", 0);
	if(plotDecImageFlag==1 && checkingStatus == COMPARE_COMPRESSOR)
	{
		int i = 0;
		char* p = NULL;

		char buffer[100];		
		char* sliceMode = iniparser_getstring(ini, "PLOT:plotDecSliceMode", "DECVIS_ERROR_LINEAR_APPROX");
		
		if(strcmp(sliceMode, "DECVIS_ERROR_SELECT_NEAREST")==0)
			plotDecSliceMode = DECVIS_ERROR_SELECT_NEAREST;
		else if(strcmp(sliceMode, "DECVIS_ERROR_LINEAR_APPROX")==0)
			plotDecSliceMode = DECVIS_ERROR_LINEAR_APPROX;
		else if(strcmp(sliceMode, "DECVIS_ERROR_LIBPRESSIO_OPT")==0)
			plotDecSliceMode = DECVIS_ERROR_LIBPRESSIO_OPT;
		else
		{
			plotDecSliceMode = DECVIS_ERROR_LINEAR_APPROX;
		}
		
		char* CmprString = iniparser_getstring(ini, "PLOT:plotDecCompressors", NULL);
		
		p = strtok(CmprString, " \r\n");
		strcpy(buffer, p);
		
		plotCompressors[0] = getCompressorID(buffer);
		i++;
		
		while(p)
		{	
			p = strtok(NULL, " \r\n");
			if(p!=NULL)
			{
				strcpy(buffer, p);
				plotCompressors[i] = getCompressorID(buffer);
				i++;
			}
		}
		
		nbPlotCompressors = i;

		printf("There are %d compressors specified by users for plotting: %s\n", nbPlotCompressors, CmprString);			
		
		i = 0;
		p = NULL;
		char* CRString = iniparser_getstring(ini, "PLOT:plotDecSliceCR", NULL);
		
		plotCRs_str[i] = (char*)malloc(MAX_VIS_DEC_CRS);//support the length of the string for the error bound is at most MAX_VIS_DEC_CRS chars
		p = strtok(CRString, " \r\n");
		strcpy(plotCRs_str[i], p);
		i++;
		
		while(p)
		{	
			p = strtok(NULL, " \r\n");
			if(p!=NULL)
			{
				plotCRs_str[i] = (char*)malloc(MAX_NB_CMPR_CASES);
				strcpy(plotCRs_str[i++], p);				
			}
		}
		
		nbPlotCRs = i;

		printf("There are %d compression ratios specified by users for plotting: ", nbPlotCRs);			
		for(i=0;i<nbPlotCRs;i++)
		{
			plotCRs[i] = atof(plotCRs_str[i]);
			printf("%s ", plotCRs_str[i]);
		}
		printf("\n");
	}	

	ecPropertyTable = ht_create( HASHTABLE_SIZE );			
	ecCompareDataTable = ht_create(HASHTABLE_SIZE);
	
	//if(plotAutoCorrFlag || plotEntropyFlag || plotAbsErrPDFFlag || checkCompressorsFlag)
	if(checkingStatus==COMPARE_COMPRESSOR)
	{	
		plotAutoCorrFlag = 	(int)iniparser_getint(ini, "PLOT:plotAutoCorr", 0);	
		if(autocorrFlag == 0) plotAutoCorrFlag = 0;
		plotAbsErrPDFFlag = (int)iniparser_getint(ini, "PLOT:plotAbsErrPDF", 0);
		if(errAutoCorrFlag == 0) plotErrAutoCorrFlag = 0;
		plotErrAutoCorrFlag = (int)iniparser_getint(ini, "PLOT:plotErrAutoCorr", 0);
		plotFFTAmpFlag = (int)iniparser_getint(ini, "PLOT:plotFFTAmp", 0);
		if(fftFlag == 0) plotFFTAmpFlag = 0;
		plotEntropyFlag = (int)iniparser_getint(ini, "PLOT:plotEntropy", 0);
		if(entropyFlag == 0) plotEntropyFlag = 0;
		
		checkCompressorsFlag = (int)iniparser_getint(ini, "PLOT:plotCompressionResults", 0);
				
		//TODO: check if the "dataProperties" directory exists in the current directory.
		//TODO: If not, create one dir named dataProperties, and softlinks will be created under it later.		
		if(access("dataProperties", F_OK)!=0)
		{
			system("mkdir -p dataProperties");
		}
		if(access("compressionResults", F_OK)!=0)
		{
			system("mkdir -p compressionResults");
		}
		char tmpPathBuf[ZC_BUFS_LONG], fullPathBuf[ZC_BUFS_LONG], softLinkPath[ZC_BUFS], linkCmd[ZC_BUFS_LONG];
		char* propertyExtension = iniparser_getstring(ini, "PLOT:propertyExtension", NULL);		
		char* compressorsString = iniparser_getstring(ini, "PLOT:compressors", NULL);
		char* cmpResultFileExtension = iniparser_getstring(ini, "PLOT:cmpResultFileExtension", NULL);
		//update compressors_count, compressors and compressors_dir
		char *p;
		char *compressorsBuf[20]; //to compare at most 20 compressors
		
		int i = 0, j = 0, k = 0;
		compressorsBuf[i++] = strtok(compressorsString, " \r\n");
		p = strtok(NULL, " \r\n");
		while(p)
		{	
			compressorsBuf[i++] = p;
			p = strtok(NULL, " \r\n");
		}
		compressors_count = i;
		
		char* cmprsorBuf;
		char* cmprsorDir;
		for(i=0;i<compressors_count;i++)
		{
			cmprsorBuf = strtok(compressorsBuf[i], ":");
			compressors[i] = (char*)malloc(strlen(cmprsorBuf)+1);
			sprintf(compressors[i], "%s", cmprsorBuf);
			compressorIDs[i] = getCompressorID(compressors[i]);
			
			cmprsorDir = strtok(NULL, ":");
			compressors_dir[i] = (char*)malloc(strlen(cmprsorDir)+1);
			sprintf(compressors_dir[i], "%s", cmprsorDir);
			properties_dir[i] = (char*)malloc(strlen(cmprsorDir)+20);
			sprintf(properties_dir[i], "%s/dataProperties", compressors_dir[i]);
			compareData_dir[i] = (char*)malloc(strlen(cmprsorDir)+20);
			sprintf(compareData_dir[i], "%s/compressionResults", compressors_dir[i]);
		}

		//load property data first
		char* property_dir = NULL;
				
		char** fileNames = (char**)malloc(ZC_BUFS_LONG*sizeof(char*)); //at most 1000 files, each of which has 500 chars.
		for(i=0;i<ZC_BUFS_LONG;i++)
			fileNames[i] = (char*)malloc(500);		
		//load property info from current directory (in Z-checker)
		
		ecVisDecDataTables = ht_create(HASHTABLE_SIZE_SMALL); //64 entries						
		int count;
		ZC_getFileNames("dataProperties", propertyExtension, &count, fileNames);
		for(j=0;j<count;j++)
		{
			ZC_DataProperty* prop = loadProperty("dataProperties", fileNames[j]);

			if(plotDecImageFlag)
			{	
				hashtable_t *varComprMap = (hashtable_t*)ht_get(ecVisDecDataTables, prop->varName);
				if(varComprMap==NULL)  //double-gurantee no duplicated entries
				{
					nbVars++; //record number of variables
					
					varComprMap = ht_create(HASHTABLE_SIZE_SMALL);
					ht_set(ecVisDecDataTables, prop->varName, varComprMap);
								
					//traverse all the CRs required by users
					for(k=0;k<nbPlotCompressors;k++)
					{
						int compressorID = plotCompressors[k];
						
						char* compressorName = NULL;
						int w = 0;
						for(w=0;w<compressors_count;w++)
						{
							if(compressorIDs[w]==compressorID)
							{
								compressorName = compressors[w]; //compressorName will be used to query in the varComprMap to get the entry.
								break;
							}	
						}
						
						CompressorCRVisElement* cmprVisE = (CompressorCRVisElement*)malloc(sizeof(struct CompressorCRVisElement));
						memset(cmprVisE, 0, sizeof(struct CompressorCRVisElement));
						cmprVisE->varName = prop->varName;
						cmprVisE->compressorName = compressorName;
						cmprVisE->CRVisDataMap = ht_create(HASHTABLE_SIZE_SMALL);
						ht_set(varComprMap, compressorName, cmprVisE);
						
						for(i=0;i<nbPlotCRs;i++)
						{
							ZCVisDecDataElement* visEle = (ZCVisDecDataElement*) malloc(sizeof(struct ZCVisDecDataElement));
							memset(visEle, 0, sizeof(struct ZCVisDecDataElement));
							visEle->varName = prop->varName;
							visEle->dataProperty = prop;
							visEle->compressorName = compressorName;
							visEle->compressorID = compressorID;					
							ht_set(cmprVisE->CRVisDataMap, plotCRs_str[i], visEle);
						}
					}					
				}
			}
		}
		
		//load property info from compressor-directories
		for(i=0;i<compressors_count;i++)
		{
			char* compressor = compressors[i];
			printf("Loading property data for %s...\n", compressor);																																													
			property_dir = properties_dir[i];
			ZC_getFileNames(property_dir, propertyExtension, &count, fileNames);
			//TODO: update the data structure  ZC_CompareData
			for(j=0;j<count;j++)
				loadProperty(property_dir, fileNames[j]);
		}
			
		int propVarCount = ht_getElemCount(ecPropertyTable);		
		if(propVarCount==0)
		{
			printf("Error: No property values are detected.\nPlease run analyzeDataProperty.sh (i.e., analyzeDataProperty.c) to generate the data perperties first.\n");
			exit(0);
		}

		//and then load comparison results....
				
		char* buf = iniparser_getstring(ini, "PLOT:comparisonCases", NULL);
		comparisonCases = (char*)malloc(strlen(buf)+1);
		sprintf(comparisonCases, "%s", buf);

		cmpResultFileExtension = iniparser_getstring(ini, "PLOT:cmpResultFileExtension", NULL);	
		
		for(i=0;i<compressors_count;i++)
		{
			char* compressor = compressors[i];
			printf("Reading compression results for %s\n", compressor);																																													
			char* compare_dir = compareData_dir[i];
			int count;
			ZC_getFileNames(compare_dir, cmpResultFileExtension, &count, fileNames);
			printf("There are %d files in %s\n", count, compare_dir);
			//TODO: update the data structure  ZC_CompareData
			for(j=0;j<count;j++)
			{
				char* compResultCaseName = rmFileExtension(fileNames[j]);
				
				ZC_CompareData* p = (ZC_CompareData*)ht_get(ecCompareDataTable, compResultCaseName);
				if(p==NULL)
				{				
					//todo: deal with full path
					sprintf(tmpPathBuf, "%s/%s", compare_dir, fileNames[j]);				
					//free(compResultCaseName);
					
					ZC_CompareData* compare = ZC_loadCompressionResult(tmpPathBuf);
					p = compare;
					compare->solution = (char*)malloc(strlen(compResultCaseName)+1);
					strcpy(compare->solution, compResultCaseName);
					
					ht_set(ecCompareDataTable, compResultCaseName, compare);
					//TODO: extract the error bound from the compResultCaseName, e.g., zfp(1E-2):CLDHGH_1_1800_3600
					if(plotDecImageFlag)
					{
						char compressorName[64];
						char errorBound[64];
						char varName[128];
						ZC_parseCompressionCase(compResultCaseName, compressorName, errorBound, varName);
						
						int isin = isInPlotDecCompressors(compressorName);
						
						if(isin)
						{
							hashtable_t* varCmprMap = (hashtable_t*)ht_get(ecVisDecDataTables, varName);
							CompressorCRVisElement* cmprVisE = (CompressorCRVisElement*)ht_get(varCmprMap, compressorName);
							compare->errorBound = atof(errorBound);						
							cmprVisE->compressionResults[cmprVisE->resultCount++] = compare;							
						}

					}

					
					sprintf(softLinkPath, "compressionResults/%s", fileNames[j]);					
					if (access(softLinkPath, F_OK) != 0)
					{
						updateLinkFullPath(tmpPathBuf, fullPathBuf);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);				
					}					
					
					if(absErrPDFFlag)
					{
						sprintf(softLinkPath, "compressionResults/%s.dis", compResultCaseName);
						if (access(softLinkPath, F_OK) != 0)
						{
							sprintf(tmpPathBuf, "%s/%s.dis", compare_dir, compResultCaseName);
							updateLinkFullPath(tmpPathBuf, fullPathBuf);
							sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
							system(linkCmd);
						}						
					}
					
					if(pwrErrPDFFlag)
					{
						sprintf(softLinkPath, "compressionResults/%s.pds", compResultCaseName);
						if (access(softLinkPath, F_OK) != 0)
						{
							sprintf(tmpPathBuf, "%s/%s.pds", compare_dir, compResultCaseName);
							updateLinkFullPath(tmpPathBuf, fullPathBuf);
							sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
							system(linkCmd);
						}						
					}					
					
					if(errAutoCorrFlag)
					{
						sprintf(softLinkPath, "compressionResults/%s.autocorr", compResultCaseName);
						if (access(softLinkPath, F_OK) != 0)
						{
							sprintf(tmpPathBuf, "%s/%s.autocorr", compare_dir, compResultCaseName);
							updateLinkFullPath(tmpPathBuf, fullPathBuf);
							sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
							system(linkCmd);
						}
					}
		
					if(fftFlag)
					{
						sprintf(softLinkPath, "compressionResults/%s.fft", compResultCaseName);
						if (access(softLinkPath, F_OK) != 0)
						{
							sprintf(tmpPathBuf, "%s/%s.fft", compare_dir, compResultCaseName);
							updateLinkFullPath(tmpPathBuf, fullPathBuf);
							sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
							system(linkCmd);
						}
						
						sprintf(softLinkPath, "compressionResults/%s.fft.amp", compResultCaseName);
						if (access(softLinkPath, F_OK) != 0)
						{
							sprintf(tmpPathBuf, "%s/%s.fft.amp", compare_dir, compResultCaseName);
							updateLinkFullPath(tmpPathBuf, fullPathBuf);
							sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
							system(linkCmd);
						}											
					}									
				}
				
				free(compResultCaseName);
			}	
		}
		for(j=0;j<ZC_BUFS_LONG;j++)
			free(fileNames[j]);
		free(fileNames);					
	
		generateReportFlag = (int)iniparser_getint(ini, "REPORT:generateReport", 0);
		if(generateReportFlag==1)
		{
			char *reportTemplateDirString = iniparser_getstring(ini, "REPORT:reportTemplateDir", NULL);
			reportTemplateDir = (char*)malloc(sizeof(char)*(strlen(reportTemplateDirString)+1));
			sprintf(reportTemplateDir, "%s", reportTemplateDirString);
			numOfErrorBoundCases = (int)iniparser_getint(ini, "REPORT:numOfErrorBoundCases", 1);
		}
		else
			reportTemplateDir = NULL;
	}
	
	ZC_versionNumber[0] = ZC_VER_MAJOR; //0
	ZC_versionNumber[1] = ZC_VER_MINOR; //5
	ZC_versionNumber[2] = ZC_VER_REVISION; //15
    
    iniparser_freedict(ini);
    return ZC_SCES;
}

/*-------------------------------------------------------------------------*/
/**
    @brief      It reads and tests the configuration given.
    @return     integer         1 if successfull.

    This function reads the configuration file. Then test that the
    configuration parameters are correct (including directories).

 **/
/*-------------------------------------------------------------------------*/
int ZC_LoadConf() {
    int res = ZC_ReadConf();
    if (res == ZC_NSCS)
    {
        printf("[ZC] ERROR: Impossible to read configuration.\n");
        return 0;
    }
    return res;
}

int modifyZCConfig(StringLine* confLinesHeader, const char* targetAttribute, const char* newStringValue)
{
	char* attr; 
	char tmp[ZC_BUFS_LONG];
	StringLine* p = confLinesHeader;
	while(p->next!=NULL)
	{
		strcpy(tmp, p->next->str);
		attr = strtok(tmp,"=");
		if(attr==NULL)
		{
			p=p->next;
			continue;
		}
		trim(attr);
		if(strcmp(attr, targetAttribute)==0)
		{
			sprintf(p->next->str, "%s=%s\n", targetAttribute, newStringValue);
			return ZC_SCES;
		}
		p=p->next;
	}
	return ZC_NSCS;
}

int delCompressorZCConfig(StringLine* confLinesHeader, const char* compressor)
{
	char* delim = " ";
	char* delim2 = "=";
	char* delim3 = ":";
	
	char* zname;
	char* p = NULL;
	char p2[1024], p3[1024];
	int counter =  0;
	int i = 0;
	StringLine* curLine = NULL;
	StringLine* preLine = confLinesHeader;
	while(preLine->next!=NULL) //go to the modifyZCConfig line
	{
		curLine = preLine->next;
		if(ZC_startsWithLines(curLine, "compressors")==1)
		{
				break;
		}
		preLine = preLine->next;
	}
	StringLine* modifyLine = curLine;	
	
	strcpy(p2, modifyLine->str);
	
	strtok(modifyLine->str, delim2); //split by "="
	char* pp = strtok(NULL, delim2);
	
	char cmprs[20][100]; //at most 20 compressors
	p = strtok(pp, delim);
	for(counter = 0;p!=NULL;counter++)
	{
		strcpy(cmprs[counter],p);
		p = strtok(NULL, delim);
	}

	char tmp[1024], p2_[1024];
	memset(p2_, 0, 1024);
	for(i=0;i<counter;i++)
	{
		p = cmprs[i];
		strcpy(p3, p);
		zname = strtok(p3, delim3); //sz_f : ../../SZ...
		if(strcmp(zname, compressor)!=0)
		{
			sprintf(tmp, "%s %s", p2_, p);
			strcpy(p2_, tmp);
		}
	}
	trim(p2_);
	printf("compressors=%s\n", p2_);
	modifyZCConfig(confLinesHeader, "compressors", p2_);
	
	return ZC_SCES;
}
