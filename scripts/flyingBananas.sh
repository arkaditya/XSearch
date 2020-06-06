#!/bin/bash

#------------------------------Variables To Perform Sweep On-----------------------#
export LD_LIBRARY_PATH="lib/luceneplusplus-3.0.7/lib"
export LC_ALL="en_US.UTF-8"

path1=/home/cc/a_WikiDataFiles/kB1
path2=/home/cc/a_WikiDataFiles/kB2

datentime=$(date +'%Y-%m-%d-%H:%M')
numThreads=4

log="iteration$datentime.log"
#------------------------------Start Parameter Sweep ------------------------------#

make


if [ $? -eq 0 ]
then
	python scripts/FileGenerator.py $path1 &>> $log
	for ((i=1;i<=12;i++));
	do 
		for ((x=1;x<=5;x++));
		do 
			sync
			sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
			 ./bin/XSearchData.exe "$(pwd)/FileList.txt" $numThreads >> ~/IWB/Results/squattingLion.csv
		done
		echo -n "" > $log
		numThreads=$((numThreads+2))
	done
	
	sleep 10	
	numThreads=4
	python scripts/FileGenerator.py $path2 &>> $log
	for ((i=1;i<=12;i++));
	do 
		for ((x=1;x<=5;x++));
		do 
			sync
			sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
			 ./bin/XSearchData.exe "$(pwd)/FileList.txt" $numThreads >> ~/IWB/Results/squattingLion.csv
		done
		echo -n "" > $log
		numThreads=$((numThreads+2))
	done
fi
