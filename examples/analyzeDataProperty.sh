#!/bin/bash

if [[ $# < 2 ]]; then
	echo Usage: $0 [data directory] [dimension sizes....] 
	echo Example: $0 /home/shdi/CESM-testdata/1800x3600 3600 1800
	exit
fi

dataDir="$1"
dim1=$2
dim2=$3
dim3=$4
dim4=$5

fileList=`cd "$dataDir";ls *.dat`

for file in $fileList
do
	echo ./analyzeDataProperty zc.config "$dataDir" "$file" $dim1 $dim2 $dim3 $dim4
	./analyzeDataProperty zc.config "$dataDir" "$file" $dim1 $dim2 $dim3 $dim4
done
