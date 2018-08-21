#!/bin/bash

if [[ $# != 2 ]];then
	echo $0 [DATA DIR] [PROBLEM_SIZE]
	exit
fi
dir=$1
size=$2

filelist=`cd $dir;ls *.dat`

for file in $filelist
do
	echo gnuplot -e "filename='$dir/$file';probSize='$size'" vplot.plg
	gnuplot -e "filename='$dir/$file';probSize='$size'" vplot.plg 
done

