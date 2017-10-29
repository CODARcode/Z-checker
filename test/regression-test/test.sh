#!/bin/bash

if [[ $# < 2 ]] ; then
	echo "Usage: test.sh [config_file] [SZ root package]"
	echo "Example: test.sh zc.config /home/sdi/Development/Z-checker/z-checker-0.1"
	exit
fi

java -cp lib/ZC_RegressionTest.jar test.CheckConfiguration $1 $2
