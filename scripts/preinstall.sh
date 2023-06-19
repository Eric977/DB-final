#!/bin/bash
mkdir ../compressions
cd ../compressions

# Install lz4
echo -e "\n----------Installing lz4----------\n"
git clone https://github.com/lz4/lz4.git
cd lz4
make
make install
cd ..
echo -e "\n----------Finished installing lz4----------\n"

# Install snappy
echo -e "\n----------Installing snappy----------\n"
git clone https://github.com/google/snappy.git
cd snappy
git submodule update --init
mkdir build
cd build && cmake ../ && make
cd ../..
echo -e "\n----------Finished installing snappy----------\n"

