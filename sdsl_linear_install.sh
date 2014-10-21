#!/bin/bash
#
# Script for installing SDSL linear
# Written by Andras Majdan
echo "Building SDSL linear.."
./sdsl_linear/install.sh ${PWD}"/sdsl_linear_build" 1>/dev/null 2>&1 
if [ $? != 0 ]; then
	printf "ERROR: Failed to build SDSL linear"
	printf "Running it again to see the output.."
	./sdsl_linear/install.sh ${PWD}"/sdsl_linear_build"
	exit 1
fi
