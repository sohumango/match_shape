#!/bin/bash

if [[ -d build ]]; then
	rm -rf build
fi


if [[ ! -d build ]]; then
	mkdir build
fi

debug="build/src/Debug"
mkdir -p $debug
if [[ -d $debug ]]; then
	cp -r res $debug
fi


cd build
if [[ $# -eq 0 ]]; then
	cmake .. -G "Xcode"
	st
fi

if [[ $# -eq 1 ]]; then
	if [[ "$1" == "b" ]]; then	
		xcodebuild
	else	
		cmake ..
		make
	fi
fi