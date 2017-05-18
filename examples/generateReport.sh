#!/bin/bash

if [ $# != 1 ]
then
	echo Usage: $0 [dataSetName]
	echo Example: $0 CESM-ATM-Tylor-Data
	exit
fi 

dataSetName=$1
if [ -f ../../zc-patches/zc-compare.config.patch ]; then
	patch -p0 < ../../zc-patches/zc-compare.config.patch
fi

GNUPLOT_PATH=`which gnuplot`
if [ ! -x $GNUPLOT_PATH ]; then
	echo "Error: Please install GNUPLOT first and make sure the command 'gnuplot' works."
	exit
fi
LATEXMK_PATH=`which latexmk`
if [ ! -x $LATEXMK_PATH ]; then
	echo "Error: Please install latexmk first and make sure the command 'latexmk' works."
	exit
fi

echo ./generateGNUPlot zc.config
./generateGNUPlot zc.config

mkdir compareCompressors
mv *.eps compareCompressors/

mkdir compareCompressors/data
mv *_*.txt compareCompressors/data

mkdir compareCompressors/gnuplot_scripts
mv *.p compareCompressors/gnuplot_scripts

echo ./generateReport zc.config $dataSetName
./generateReport zc.config $dataSetName

cd report
if [ -f z-checker-report.pdf ]; then
	make clean
fi
make
cd ..

if [ ! -f ../../zc-patches/zc-compare.config.patch ]; then
	patch -RE -p0 < ../../zc-patches/zc-compare.config.patch
fi

if [ ! -f report/z-checker-report.pdf ]; then
	echo "Noice: Your local latexmk cannot generate the report successfully."
	echo "In this situation, you can upload the source code (called report.zip) of the report we generated to Overleaf instead, for generating the pdf report online."
	echo "Overleaf: https://www.overleaf.com/"
	exit
fi
