#!/bin/bash

patch -p0 < ../../zc-patches/zc-compare.config.patch
./generateGNUPlot zc.config "CESM-ATM-Tylor-Data"

mkdir compareCompressors
mv *.eps compareCompressors/

mkdir compareCompressors/data
mv *_*.txt compareCompressors/data

mkdir compareCompressors/gnuplot_scripts
mv *.p compareCompressors/gnuplot_scripts

patch -r 
