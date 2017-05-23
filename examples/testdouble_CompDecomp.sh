#!/bin/bash

cmdDir="/home/fti/SZ_C_version/sz-1.4.9-beta-normalsize/sz-1.4.9.2-beta/example"
#rootDir="/run/media/fti/62D7F3A471F74BE2/NCAR-data"
#rootDir="/run/media/fti/62D7F3A471F74BE2/CESM-bigdata"
rootDir="/home/fti/SZ_C_version/CESM-testdata"
absErrBound=1e-4

if [[ $# < 1 ]]; then
	echo "Usage: $0 [errBoundMode]"
	echo "Example: $0 PW_REL/ABS/REL"
	exit
fi

errBoundMode=$1

targetDir="$rootDir"
fileList=`cd "$targetDir";ls *.dat2`
cd "$cmdDir"
dimSizes="1800 3600" 
for file in $fileList
do
	echo processing $file
        echo testdouble_CompDecomp sz.config zc.config "sz($absErrBound)" "$file" $errBoundMode $absErrBound "$targetDir/$file" $dimSizes
        testdouble_CompDecomp sz.config zc.config "sz($absErrBound)" "$file" $errBoundMode $absErrBound "$targetDir/$file" $dimSizes
done

echo "complete"

