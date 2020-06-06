#!/bin/bash

if [ ! -d "download" ]
then
    mkdir download
fi

if [ ! -e "download/LucenePlusPlus" ]
then
    cd download
    git clone https://github.com/luceneplusplus/LucenePlusPlus.git
    cd ..
fi

if [ ! -d "lib" ]
then
    mkdir lib
fi

dpkg -s gcc &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y gcc
fi

dpkg -s g++ &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y g++
fi

dpkg -s libboost-date-time-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-date-time-dev
fi

dpkg -s libboost-filesystem-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-filesystem-dev
fi

dpkg -s libboost-regex-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-regex-dev
fi

dpkg -s libboost-thread-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-thread-dev
fi

dpkg -s libboost-iostreams-dev &> /dev/null
if [ $? -eq 1 ]
then
    sudo apt install -y libboost-iostreams-dev
fi

if [ ! -e "lib/lucene-7.1.0/build/core/lucene-core-7.1.0-SNAPSHOT.jar" ]
then
    cd download/LucenePlusPlus
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX=../../../lib/luceneplusplus-3.0.7 ..
    make
    make install
    cd ../..
fi
