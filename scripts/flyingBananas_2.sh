#!/bin/bash
export LD_LIBRARY_PATH="lib/luceneplusplus-3.0.7/lib"
export LC_ALL="en_US.UTF-8"

path1=/home/cc/MetaData/SplitFiles10K
path2=/home/cc/MetaData/SplitFiles

datentime=$(date +'%Y-%m-%d-%H:%M')
numThreads=4
maxBufferedDocs=100
RAMBufferedSizeMB=1024


log="iteration$datentime.log"
#------------------------------Start Parameter Sweep ------------------------------#

make


if [ $? -eq 0 ]
then
	python scripts/FileGenerator.py $path1 &>> $log
	for ((i=1;i<=11;i++));
	do 
		for ((x=1;x<=5;x++));
		do 
			sync
			sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
			 ./bin/XSearchData.exe "$(pwd)/FileList.txt" $numThreads $maxBufferedDocs $RAMBufferedSizeMB >> ~/IWB/Results/squattingLion_Metadata.csv 
		done
		numThreads=$((numThreads+2))
	done

	sleep 10
	numThreads=8	
	python scripts/FileGenerator.py $path2 &>> $log
	
	for ((i=1;i<=11;i++));
	do 
		for ((x=1;x<=5;x++));
		do 
			sync
			sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
			 ./bin/XSearchData.exe "$(pwd)/FileList.txt" $numThreads $maxBufferedDocs $RAMBufferedSizeMB >> ~/IWB/Results/squattingLion_Metadata.csv 
		done
		numThreads=$((numThreads+2))
	done
fi
