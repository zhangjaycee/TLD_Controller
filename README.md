#version2_landing(实现自动降落)
#### new README by Jaycee qq 605829557 email yxzjc@126.com

###文件结构
src中:
sender.cpp是我后来加的用来生成控制字符串和发送这个字符串;
TLD.cpp和tld_run.cpp做过控制四旋翼的功能的一些修改,主要是增加了一些代码;
其他文件基本没动,是原TLD例程里的.

###编译和运行
我参照原例程文档写了自动编译和运行的脚本
编译脚本:
在本文件夹下运行./compile.sh
运行脚本:
请在本文件夹下运行./run.sh
(不加参数默认用/dev/video0摄像头,加人以参数用/dev/cideo1摄像头)


### 以下为原TLD算法例程的编译和运行命令样例:
This will be a C++ implementations of the OpenTLD (aka Predator)
----------------------------------------------------------------------------
This is a work in progress, as of right now the code is functional but pretty slow.
----------------------------------------------------------------------------
Installation Notes
=====================================
git clone git@github.com:alantrrs/OpenTLD.git
cd OpenTLD
mkdir build
cd build
cmake ../src/
make
cd ../bin/
%To run from camera
./run_tld -p ../parameters.yml -tl
%To run from file
./run_tld -p ../parameters.yml -s ../datasets/06_car/car.mpg -tl
%To init bounding box from file
./run_tld -p ../parameters.yml -s ../datasets/06_car/car.mpg -b ../datasets/06_car/init.txt -tl
%To train only in the firs frame (no tracking, no learning)
./run_tld -p ../parameters.yml -s ../datasets/06_car/car.mpg -b ../datasets/06_car/init.txt 
%To test the final detector (Repeat the video, first time learns, second time detects)
./run_tld -p ../parameters.yml -s ../datasets/06_car/car.mpg -b ../datasets/06_car/init.txt -tl -r

=====================================
Evaluation
=====================================
The output of the program is a file called bounding_boxes.txt which contains all the detections made through the video. This file should be compared with the ground truth file to evaluate the performance of the algorithm. This is done using a python script:
python ../datasets/evaluate_vis.py ../datasets/06_car/car.mpg bounding_boxes.txt ../datasets/06_car/gt.txt

====================================
Thanks
====================================
To Zdenek Kalal for realeasing his awesome algorithm

