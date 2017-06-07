#!/bin/bash

dataDir="/home/shdi/CESM-ATM-tylor/1800x3600"
originaldataFile="CLDLOW_1_1800_3600.dat"
decompressDataFiles="CLDLOW_1_1800_3600.dat.hf.out CLDLOW_1_1800_3600.dat.intrpl.out CLDLOW_1_1800_3600.dat.sz.out"
varName="CLDLOW"
dim1=3600
dim2=1800

for decFile in $decompressDataFiles
do
	echo ./compareDataSets zc.config $decFile $varName $dataDir/$originaldataFile $dataDir/$decFile $dim1 $dim2 $dim3
	./compareDataSets zc.config $decFile $varName $dataDir/$originaldataFile $dataDir/$decFile $dim1 $dim2 $dim3
done
