#include <stdio.h>
#include "zc.h"
#include "ZC_rw.h"

int main(int argc, char* argv[])
{
	if(argc<4)
	{
		printf("Usage: modifyZCConfig [zc.config] [targetAttr] [value]\n");
		printf("Example: modifyZCConfig zc.config compressors \"sz:../../SZ/example zfp:../../zfp/utils\"\n");
		exit(0);
	}

	char* configFile = argv[1];
	char* targetAttr = argv[2];
	char value[ZC_BUFS_LONG];
	strcpy(value, argv[3]);
	int lineCount;
	
	StringLine* header = ZC_readLines(configFile, &lineCount);	
	modifyZCConfig(header, targetAttr, value);
	ZC_writeLines(header, configFile);
	ZC_freeLines(header);
	return 0;
}
