#include <stdio.h>
#include "zc.h"
#include "ZC_rw.h"

int main(int argc, char* argv[])
{
	if(argc<3)
	{
		printf("Usage: delCompressorZCConfig [zc.config] [compressorName]\n");
		printf("Example: delCompressorZCConfig zc.config SZauto\n");
		exit(0);
	}

	char* configFile = argv[1];
	char* compressor = argv[2];
	int lineCount;
	
	StringLine* header = ZC_readLines(configFile, &lineCount);
	delCompressorZCConfig(header, compressor);
	ZC_writeLines(header, configFile);
	ZC_freeLines(header);
	return 0;
}
