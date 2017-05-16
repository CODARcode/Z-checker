#!/bin/bash

dataDir="testdata"
originaldataFile="ori.dat"
errBounds="1E-1 1E-2 1E-3 1E-4 1E-5"
compressorName="candidate_compressorB"
varName="var1"
dim1=8
dim2=8
dim3=128

for errBound in $errBounds
do
	compressionCase="${compressorName}($errBound)"
	decompressDataFile="${dataDir}/${compressionCase}:${varName}.dat"
	./compareDataSets zc.config $compressionCase $varName $originaldataFile $decompressDataFile $dim1 $dim2 $dim3
done
