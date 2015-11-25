#!/bin/bash
cd bin
if [ $# -eq 0 ];then
    sudo ./run_tld -p  ../parameters.yml -tl -v0| grep data
else
    sudo ./run_tld -p  ../parameters.yml -tl -v1| grep data
fi
#cd bin
#sudo ./run_tld -p ../parameters.yml -tl -d 1 
