#!/bin/bash

#originaldataFile="/home/shdi/CESM-ATM-tylor/1800x3600/CLDLOW_1_1800_3600.dat"

if [[ $# < 4 ]]; then
	echo "Usage: compareDataSet.sh [varName] [original data file] [user's decompressed data file] [dimension sizes...]"
	exit
fi

varName=$1
originaldataFile=$2
decompresseddataFile=$3
dim1=$4
dim2=$5
dim3=$6
dim4=$7

#run compressors
cd ../../simple-lossy-compressors
./compile-DeciIntrler.sh
./compile-HfPrecCvter.sh
./DeciIntrler 5 -f ${originaldataFile} ${dim1} $dim2 $dim3 $dim4
./HfPrecCvter -f $originaldataFile $dim1 $dim2 $dim3 $dim4

cd ../SZ/example
./testfloat_compress sz.config $originaldataFile $dim1 $dim2 $dim3 $dim4
./testfloat_decompress sz.config "${originaldataFile}.sz" $dim1 $dim2 $dim3 $dim4

#generate results

cd ../../Z-checker/examples/
./compareDataSets zc.config "hf-prec-cvter" $varName "${originaldataFile}" "${originaldataFile}.hf.out" $dim1 $dim2 $dim3 $dim4

./compareDataSets zc.config "dec-interp-cmprsor" $varName "${originaldataFile}" "${originaldataFile}.intrpl.out" $dim1 $dim2 $dim3 $dim4

./compareDataSets zc.config sz $varName "${originaldataFile}" "${originaldataFile}.sz.out" $dim1 $dim2 $dim3 $dim4

./compareDataSets zc.config user-compressor $varName "${originaldataFile}" "$decompresseddataFile" $dim1 $dim2 $dim3 $dim4
