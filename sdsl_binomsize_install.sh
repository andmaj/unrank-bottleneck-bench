#!/bin/bash
#
# Script for installing SDSLv binomsize
# Written by Andras Majdan
echo "Building SDSL binomsize.."
./sdsl_binomsize/install.sh ${PWD}"/sdsl_binomsize_build" 1>/dev/null 2>&1 
if [ $? != 0 ]; then
	printf "ERROR: Failed to build SDSL binomsize"
	printf "Running it again to see the output.."
	./sdsl_binomsize/install.sh ${PWD}"/sdsl_binomsize_build"
	exit 1
fi
