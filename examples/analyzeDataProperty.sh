#!/bin/bash

if [[ $# < 2 ]]; then
	echo "Usage - option 1: $0 [datatype (-f or -d)] [data directory] [extension] [dimension sizes....]"
	echo "      - option 2: $0 [datatype (-f or -d)] [varListFile]"
	echo Example: $0 -f /home/shdi/CESM-testdata/1800x3600 3600 1800
	exit
fi

datatype=$1
if [ -d $2 ]; then
	option=1
else
	option=0
fi

if [[ $option == 1 ]]; then
	dataDir="$2"
	extension=$3
	dim1=$4
	dim2=$5
	dim3=$6
	dim4=$7
else
	varListFile=$2
fi

if [[ $option == 1 ]]; then
	fileList=`cd "$dataDir";ls *.${extension}`

	for file in $fileList
	do
		echo ./analyzeDataProperty $file $datatype zc.config "$dataDir/$file" $dim1 $dim2 $dim3 $dim4
		./analyzeDataProperty $file $datatype zc.config "$dataDir/$file" $dim1 $dim2 $dim3 $dim4
	done
else
	nbVars=`./queryVarList -n -i $varListFile`
	for (( i = 0; i < nbVars; i++)); do
		varName=`./queryVarList -m -I $i -i $varListFile`
		file=`./queryVarList -f -I $i -i $varListFile`
		dims=`./queryVarList -d -I $i -i $varListFile`
		./analyzeDataProperty $varName $datatype zc.config $file $dims
        done	
fi
