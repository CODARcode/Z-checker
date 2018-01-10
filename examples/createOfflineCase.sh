#!/bin/bash

if [[ $# < 1 ]]; then
	echo "Usage: createOfflineCase.sh [case name]"
	exit
fi
 
caseName=$1

if [ ! -d "$caseName" ]; then
	mkdir $caseName
else
	echo The use case "$caseName" already exits. 
	echo Please remove it first.
	exit
fi

rootPath=`pwd`

cd $caseName
ln -s "$rootPath/runOfflineCase" runOfflineCase
ln -s "$rootPath/generateGNUPlot" generateGNUPlot
cp ../zc.config .
cp ../varCmpr.inf .

echo "#readme.txt" > readme.txt
echo "- Use createOfflineCase.sh to create a use-case directory, which will contain the executables." >> readme.txt
echo "- Quick Start: " >> readme.txt
echo "- An example using the CESM ATM climate data is described below." >> readme.txt
echo ".1. Download the testing data from here: http://www.mcs.anl.gov/~shdi/download/runOfflineCase_testdata.tar.gz" >> readme.txt
echo "    (Tip: the testing data package contains two original data files, and their corresponding SZ-compressed files and decompressed files based on different error bounds)" >> readme.txt
echo ".2. tar -xzvf runOfflineCase_testdata.tar.gz" >> readme.txt
echo ".3. ./createOfflineCase.sh testcase1" >> readme.txt
echo ".4. cd testcase1" >> readme.txt
echo ".5. ./runOfflineCase -C varCmpr.inf -A -N sz" >> readme.txt
echo "    (Tip: More options can be shown by executing ./runOfflineCase without any input options)" >> readme.txt
echo ".6. Edit zc.config as follows:" >> readme.txt
echo "    checkingStatus = COMPARE_COMPRESSOR" >> readme.txt
echo "    compressors = sz:[the absolute path of the directory of the test case] (i.e., echo `pwd`)" >> readme.txt
echo "              (tip: sz in the above line refers to the compressor's name. You can replace it by your own name)" >> readme.txt
echo "    comparisonCases = sz(1E-3)" >> readme.txt 
echo ".7. ./generateGNUPlot zc.config" >> readme.txt
echo "Then, you can find rate-distortion eps files generated in the current directory." >> readme.txt
echo "More information can be found in the doc/userguide.pdf" >> readme.txt
