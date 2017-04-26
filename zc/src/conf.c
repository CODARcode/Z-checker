/**
 *  @file   conf.c
 *  @author Sheng Di (sdi1@anl.gov or disheng222@gmail.com)
 *  @date   2015.
 *  @brief  Configuration loading functions for the EC library.
 *  (C) 2015 by Mathematics and Computer Science (MCS), Argonne National Laboratory.
 *      See COPYRIGHT in top-level directory.
 */

#include "string.h"
#include "zc.h"
#include "iniparser.h"
#include "rw.h"

 
/*-------------------------------------------------------------------------*/
int ZC_ReadConf() {
    // Check access to EC configuration file and load dictionary
    int x = 1;
    char sol_name[256];
    char *modeBuf;
    char *errBoundMode;
    char *endianTypeString;
    char *checkingStatusString;
    dictionary *ini;
    char *par;
    printf("[ZC] Reading ZC configuration file (%s) ...\n", zc_cfgFile);
    if (access(zc_cfgFile, F_OK) != 0)
    {
        printf("[ZC] Configuration file NOT accessible.\n");
        return 1;
    }
    ini = iniparser_load(zc_cfgFile);
    if (ini == NULL)
    {
        printf("[ZC] Iniparser failed to parse the conf. file.\n");
        return 1;
    }

	endianTypeString = iniparser_getstring(ini, "ENV:dataEndianType", NULL);
	if(strcmp(endianTypeString, "LITTLE_ENDIAN_DATA")==0)
		dataEndianType = LITTLE_ENDIAN_DATA;
	else if(strcmp(endianTypeString, "BIG_ENDIAN_DATA")==0)
		dataEndianType = BIG_ENDIAN_DATA;
	else
	{
		printf("Error: Wrong dataEndianType: please set it correctly in ec.config.\n");
		exit(0);
	}

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

	char *y = (char*)&x;
	
	if(*y==1)
		sysEndianType = LITTLE_ENDIAN_SYSTEM;
	else //=0
		sysEndianType = BIG_ENDIAN_SYSTEM;

	// Reading/setting detection parameters
	//ZC_workspaceDir = iniparser_getstring(ini, "PARAMETER:workspaceDir", NULL);
	//if(opendir(ZC_workspaceDir)==NULL)
	//	mkdir(ZC_workspaceDir,0775);
	
	minValueFlag= (int)iniparser_getint(ini, "DATA:minValue", 0);
	maxValueFlag= (int)iniparser_getint(ini, "DATA:maxValue", 0);
	valueRangeFlag= (int)iniparser_getint(ini, "DATA:valueRange", 0);
	avgValueFlag= (int)iniparser_getint(ini, "DATA:avgValue", 0);
	entropyFlag= (int)iniparser_getint(ini, "DATA:entropy", 0);
	autocorrFlag= (int)iniparser_getint(ini, "DATA:autocorr", 0);
	fftFlag= (int)iniparser_getint(ini, "DATA:fft", 0);
	lapFlag= (int)iniparser_getint(ini, "DATA:lap", 0);
	
	compressTimeFlag = (int)iniparser_getint(ini, "COMPARE:compressTime", 0);
	decompressTimeFlag = (int)iniparser_getint(ini, "COMPARE:decompressTime", 0);
	compressSizeFlag = (int)iniparser_getint(ini, "COMPARE:compressSize", 0);
	
	minAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:minAbsErr", 0);
	avgAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:avgAbsErr", 0);
	maxAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:maxAbsErr", 0);
	autoCorrAbsErrFlag = (int)iniparser_getint(ini, "COMPARE:autoCorrAbsErr", 0);
	absErrPDFFlag = (int)iniparser_getint(ini, "COMPARE:absErrPDF", 0);
	
	minRelErrFlag = (int)iniparser_getint(ini, "COMPARE:minRelErr", 0);
	avgRelErrFlag = (int)iniparser_getint(ini, "COMPARE:avgRelErr", 0);
	maxRelErrFlag = (int)iniparser_getint(ini, "COMPARE:maxRelErr", 0);

	rmseFlag = (int)iniparser_getint(ini, "COMPARE:rmse", 0);
	nrmseFlag = (int)iniparser_getint(ini, "COMPARE:nrmse", 0);
	snrFlag = (int)iniparser_getint(ini, "COMPARE:snr", 0);
	psnrFlag = (int)iniparser_getint(ini, "COMPARE:psnr", 0);
	pearsonCorrFlag = (int)iniparser_getint(ini, "COMPARE:pearsonCorr", 0);
			
	//if(plotAutoCorrFlag || plotEntropyFlag || plotAbsErrPDFFlag || checkCompressorsFlag)
	if(checkingStatus==COMPARE_COMPRESSOR)
	{		
		plotAbsErrPDFFlag = (int)iniparser_getint(ini, "PLOT:plotAbsErrPDF", 0);
		plotAutoCorrFlag = 	(int)iniparser_getint(ini, "PLOT:plotAutoCorr", 0);
		plotFFTAmpFlag = (int)iniparser_getint(ini, "PLOT:plotFFTAmp", 0);
		plotEntropyFlag = (int)iniparser_getint(ini, "PLOT:plotEntropy", 0);
		
		checkCompressorsFlag = (int)iniparser_getint(ini, "PLOT:plotCompressionResults", 0);
				
		//TODO: check if the "dataProperties" directory exists in the current directory.
		//TODO: If not, create one dir named dataProperties, and softlinks will be created under it later.		
		if(access("dataProperties", F_OK)!=0)
		{
			system("mkdir dataProperties");
		}
		if(access("compareResults", F_OK)!=0)
		{
			system("mkdir compareResults");
		}
		char fullPathBuf[ZC_BUFS_LONG], softLinkPath[ZC_BUFS], linkCmd[ZC_BUFS_LONG];
		char* propertyExtension = iniparser_getstring(ini, "PLOT:propertyExtension", NULL);		
		char* compressorsString = iniparser_getstring(ini, "PLOT:compressors", NULL);
		char* cmpResultFileExtension = iniparser_getstring(ini, "PLOT:cmpResultFileExtension", NULL);
		//update compressors_count, compressors and compressors_dir
		char *p;
		char *compressorsBuf[20]; //to compare at most 20 compressors
		
		int i = 0, j;
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
			cmprsorDir = strtok(NULL, ":");
			compressors_dir[i] = (char*)malloc(strlen(cmprsorDir)+1);
			sprintf(compressors_dir[i], "%s", cmprsorDir);
			properties_dir[i] = (char*)malloc(strlen(cmprsorDir)+20);
			sprintf(properties_dir[i], "%s/dataProperties", compressors_dir[i]);
			compareData_dir[i] = (char*)malloc(strlen(cmprsorDir)+20);
			sprintf(compareData_dir[i], "%s/compareResults", compressors_dir[i]);
		}

		//load property data first
		ecPropertyTable = ht_create( HASHTABLE_SIZE );
		for(i=0;i<compressors_count;i++)
		{
			char* compressor = compressors[i];
			printf("Loading property data for %s...\n", compressor);																																													
			char* property_dir = properties_dir[i];
			int count;
			char** fileNames = ZC_getFileNames(property_dir, propertyExtension, &count);
			//TODO: update the data structure  ZC_CompareData
			for(j=0;j<count;j++)
			{
				char* propertyVarName = rmFileExtension(fileNames[j]);
				ZC_DataProperty* p = (ZC_DataProperty*)ht_get(ecPropertyTable, propertyVarName);
				if(p==NULL)
				{
					//todo: deal with full path
					sprintf(fullPathBuf, "%s/%s", property_dir, fileNames[j]);
					//free(compResultCaseName);
					ZC_DataProperty* property = ZC_loadDataProperty(fullPathBuf);
					ht_set(ecPropertyTable, propertyVarName, property);
					
					sprintf(softLinkPath, "dataProperties/%s", fileNames[j]);					
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);				
					}

					sprintf(softLinkPath, "dataProperties/%s.autocorr", propertyVarName);					
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(fullPathBuf, "%s/%s.autocorr", property_dir, propertyVarName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);
					}

					sprintf(softLinkPath, "dataProperties/%s.fft", propertyVarName);
					if (access(softLinkPath, F_OK) != 0)
					{					
						sprintf(fullPathBuf, "%s/%s.fft", property_dir, propertyVarName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);								
					}
					
					sprintf(softLinkPath, "dataProperties/%s.fft.amp", propertyVarName);
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(fullPathBuf, "%s/%s.fft.amp", property_dir, propertyVarName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);
					}												
				}
				free(propertyVarName);
			}
			
			for(j=0;j<1000;j++)
				free(fileNames[j]);
			free(fileNames);
		}
		
		char* buf = iniparser_getstring(ini, "PLOT:comparisonCases", NULL);
		comparisonCases = (char*)malloc(strlen(buf)+1);
		sprintf(comparisonCases, "%s", buf);
		
		cmpResultFileExtension = iniparser_getstring(ini, "PLOT:cmpResultFileExtension", NULL);	
		
		//and then load comparison results....
		ecCompareDataTable = ht_create(HASHTABLE_SIZE);
	
		/*ht_set( hashtable, "key1", compare);*/
		
		for(i=0;i<compressors_count;i++)
		{
			char* compressor = compressors[i];
			printf("Reading compression results for %s\n", compressor);																																													
			char* compare_dir = compareData_dir[i];
			int count;
			char** fileNames = ZC_getFileNames(compare_dir, cmpResultFileExtension, &count);
			//TODO: update the data structure  ZC_CompareData
			for(j=0;j<count;j++)
			{
				char* compResultCaseName = rmFileExtension(fileNames[j]);
				ZC_CompareData* p = (ZC_CompareData*)ht_get(ecCompareDataTable, compResultCaseName);
				if(p==NULL)
				{				
					//todo: deal with full path
					sprintf(fullPathBuf, "%s/%s", compare_dir, fileNames[j]);				
					//free(compResultCaseName);
					ZC_CompareData* compare = ZC_loadCompareResult(fullPathBuf);
					ht_set(ecCompareDataTable, compResultCaseName, compare);
					
					sprintf(softLinkPath, "compareResults/%s.dis", compResultCaseName);
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(fullPathBuf, "%s/%s.dis", compare_dir, compResultCaseName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);
					}
					
					sprintf(softLinkPath, "compareResults/%s.autocorr", compResultCaseName);
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(fullPathBuf, "%s/%s.autocorr", compare_dir, compResultCaseName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);
					}
		
					sprintf(softLinkPath, "compareResults/%s.fft", compResultCaseName);
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(fullPathBuf, "%s/%s.fft", compare_dir, compResultCaseName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);
					}			
					
					sprintf(softLinkPath, "compareResults/%s.fft.amp", compResultCaseName);
					if (access(softLinkPath, F_OK) != 0)
					{
						sprintf(fullPathBuf, "%s/%s.fft.amp", compare_dir, compResultCaseName);
						sprintf(linkCmd, "ln -s \"%s\" \"%s\"", fullPathBuf, softLinkPath);  
						system(linkCmd);
					}											
				}
				free(compResultCaseName);
			}
			for(j=0;j<1000;j++)
				free(fileNames[j]);
			free(fileNames);			
		}
	}																																																																																																													
	
	ZC_versionNumber[0] = ZC_VER_MAJOR; //0
	ZC_versionNumber[1] = ZC_VER_MINOR; //5
	ZC_versionNumber[2] = ZC_VER_REVISION; //15
    
    iniparser_freedict(ini);
    return 1;
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
    if (res == 0)
    {
        printf("[ZC] ERROR: Impossible to read configuration.\n");
        return 0;
    }
    return 1;
}
