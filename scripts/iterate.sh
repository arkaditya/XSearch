#!/bin/bash

function clear_cache {
	sync
	sudo sh -c 'echo 3 > /proc/sys/vm/drop_caches'
}

export LD_LIBRARY_PATH="lib/luceneplusplus-3.0.7/lib"
export LC_ALL="en_US.UTF-8"

datentime=$(date +'%Y-%m-%d-%H:%M')
path_hdd=$1
path_ssd="/home/cc/data/wikiData/kB2"
terms="/home/cc/XSearch/xsearch/ir-libraries/luceneplus/scripts/terms.txt"
metaterms="metaterms.txt"
log="iteration$datentime.log"

echo -n "" > $log

for file in $path_hdd
do

    clear_cache
    ./bin/XSearchData.exe $path_hdd$file &>> $log
done

: '
for i in {1..10}
do
    file="dataset$(($i * 200))MB.txt"

    clear_cache
    ./bin/XSearchData.exe $path_ssd$file $path_ssd$terms &>> $log
done

for i in {1..10}
do
    file="metaset$(($i * 50))kME.txt"

    clear_cache
    ./bin/XSearchMeta.exe $path_hdd$file $path_hdd$metaterms &>> $log
done

for i in {1..10}
do
    file="metaset$(($i * 50))kME.txt"

    clear_cache
    ./bin/XSearchMeta.exe $path_ssd$file $path_ssd$metaterms &>> $log
done
'
