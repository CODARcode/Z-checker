#!/bin/bash

examplesDir=`pwd`
echo examplesDir = $examplesDir
cd ../R/test
RscriptDir=`pwd`
echo RscriptDir = $RscriptDir
RscriptPath="$RscriptDir/data_analysis_script.R"

#Rscript file path = $RscriptPath
echo set RscriptPath in zc.config and varCmpr.inf to be $RscriptPath
cd $examplesDir
./modifyZCConfig ./zc.config RscriptPath $RscriptPath
./modifyZCConfig ./varCmpr.inf \#RscriptPath $RscriptPath
