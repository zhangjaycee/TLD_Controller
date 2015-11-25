#!/bin/bash


cd bin
#sudo ./run_tld -p ../parameters.yml -tl -d 1 
sudo ./run_tld -p ../parameters.yml -tl | grep data
