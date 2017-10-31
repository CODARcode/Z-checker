#!/bin/bash

if [[ $# < 4 ]]; then
	echo "Usage: compareDataSet.sh [data type (-f or -d)] [varName] [original data file] [user's decompressed data file] [dimension sizes...]"
	exit
fi

datatype=$1
varName=$2
originaldataFile=$3
decompresseddataFile=$4
dim1=$5
dim2=$6
dim3=$7
dim4=$8

#run compressors
cd ../../simple-lossy-compressors
./compile-DeciIntrler.sh
./compile-HfPrecCvter.sh


if [ $datatype != "-f" ] && [ $datatype != "-d" ]; then 
  echo "Error: datatype = $datatype . "
  echo "Note: datatype can only be either -f or -d."
  exit
fi

echo ./DeciIntrler 4 ${datatype} ${originaldataFile} ${dim1} $dim2 $dim3 $dim4
./DeciIntrler 4 ${datatype} ${originaldataFile} ${dim1} $dim2 $dim3 $dim4

echo ./HfPrecCvter ${datatype}  $originaldataFile $dim1 $dim2 $dim3 $dim4
./HfPrecCvter ${datatype}  $originaldataFile $dim1 $dim2 $dim3 $dim4

cd -
#./compareDataSets ${datatype} zc.config "hf-prec-cvter" $varName "${originaldataFile}" "${originaldataFile}.hf.out" $dim1 $dim2 $dim3 $dim4
#./compareDataSets ${datatype} zc.config "dec-interp-cmprsor" $varName "${originaldataFile}" "${originaldataFile}.intrpl.out" $dim1 $dim2 $dim3 $dim4
./compareDataSets ${datatype} zc.config user-compressor $varName "${originaldataFile}" "$decompresseddataFile" $dim1 $dim2 $dim3 $dim4
