#!/bin/bash

if [ $# != 1 ]
then
	echo Usage: please specify a unique case directory name.
	echo Example: $0 case1 
	exit
fi

caseName=$1

if [ -d $caseName ]
then
	echo Error: the directory $caseName already exists.
	echo Please give a different directory name for the new case.
	exit
fi 

mkdir $caseName
#cp examples/*.c $caseName
#cp -r examples/.deps $caseName
#cp examples/Makefile* $caseName
#cp examples/zc.config $caseName
#cp examples/*.sh $caseName

#cd $caseName
#make

cp examples/*.sh $caseName
cd $caseName
ln -s ../examples/modifyZCConfig modifyZCConfig
ln -s ../examples/analyzeDataProperty analyzeDataProperty
ln -s ../examples/compareDataSets compareDataSets
ln -s ../examples/generateGNUPlot generateGNUPlot
ln -s ../examples/generateReport generateReport
ln -s ../examples/testRscript_double testRscript_double
ln -s ../examples/testRscript_float testRscript_float
ln -s ../examples/testRscript_readBinFloat testRscript_readBinFloat
ln -s ../examples/zc.config zc.config
ln -s ../examples/README README

cd -
echo A new workspace directory $caseName has been created in Z-checker.
