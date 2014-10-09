#!/bin/bash
#
# Script for installing SDSL
# Written by Andras Majdan
echo "Building SDSL.."
./sdsl/install.sh ${PWD}"/sdsl_build" 1>/dev/null 2>&1 
if [ $? != 0 ]; then
	printf "ERROR: Failed to build SDSL"
	printf "Running it again to see the output.."
	./sdsl/install.sh ${PWD}"/sdsl_build"
	exit 1
fi
