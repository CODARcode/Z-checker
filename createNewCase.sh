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
cp examples/*.c $caseName
cp -r examples/.deps $caseName
cp examples/Makefile* $caseName
cp examples/zc.config $caseName
cp examples/*.sh $caseName

cd $caseName
make
