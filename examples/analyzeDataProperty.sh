#!/bin/bash

if [[ $# < 3 ]]; then
	echo "Usage: $0 [datatype (-f or -d)] [data directory] [dimension sizes....]"
	echo Example: $0 -f /home/shdi/CESM-testdata/1800x3600 3600 1800
	exit
fi

datatype=$1
dataDir="$2"
dim1=$3
dim2=$4
dim3=$5
dim4=$6

fileList=`cd "$dataDir";ls *.dat`

for file in $fileList
do
	echo ./analyzeDataProperty $datatype zc.config "$dataDir" "$file" $dim1 $dim2 $dim3 $dim4
	./analyzeDataProperty $datatype zc.config "$dataDir" "$file" $dim1 $dim2 $dim3 $dim4
done
