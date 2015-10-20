#!/bin/bash

cd build
rm -rf *
cmake ../src
make
cd ../bin
