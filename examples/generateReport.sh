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

if [ -f ../../zc-patches/zc-compare.config.patch ]; then
	patch -RE -p0 < ../../zc-patches/zc-compare.config.patch
fi

if [ ! -f report/z-checker-report.pdf ]; then
	zip -r report.zip report
	echo "Notice: Your local latexmk cannot generate the report successfully, probably because of missing dependencies such as texlive. "
	echo "To solve this issue, you probably need to install texlive using root previlege."
	echo "Alternatively, you can also upload the source code (called report.zip) of the report we generated to some online latex2pdf website such as Overleaf."
	echo "Note: The total number of files is greater than 60, so you need to upgrade your Overleaf-account to Pro (\$12/month) if you want to generate a complete pdf report online by Overleaf. "
	echo "Overleaf website: https://www.overleaf.com/"
	echo "How to upload zip file to create a new project in overleaf: https://www.youtube.com/watch?v=i22sF369bqA"
	exit
else
	echo "The report is generated successfully."
	echo "Here it is: `pwd`/report/z-checker-report.pdf"
fi
