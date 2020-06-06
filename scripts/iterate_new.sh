#!/bin/bash

function clear_cache {
	sync
	sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
}

export LD_LIBRARY_PATH="lib/luceneplusplus-3.0.7/lib"
export LC_ALL="en_US.UTF-8"

datentime=$(date +'%Y-%m-%d-%H:%M')
path_hdd=$1
data="$HOME/data/wikiData/"
num_threads=$2
terms="/home/cc/XSearch/xsearch/ir-libraries/luceneplus/scripts/terms.txt"
metaterms="metaterms.txt"
log="iteration$datentime.log"

echo -n "" > $log

python scripts/FileGenerator.py $1 &>> $log

echo  "$(pwd)/FileList.txt"

clear_cache
./bin/XSearchData.exe "$(pwd)/FileList.txt" $num_threads &>> $log

