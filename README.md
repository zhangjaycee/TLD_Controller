#version8_pid3 分层降落状态机规范化
## 状态
#### 具体思路见分支内pdf v8.pdf,四个状态:

状态1: 初次调整

状态2: 下降(可能会被再次调整(状态3)打断)

状态3: 再次调整

状态4: 直接下降(无人机离地面很近时触发)

#### 转移: ①--->(②<-->③)--->④

#version7_pid2 实现了无人机边下降便调整
但是效果不好


#version6_pid-addI(加入pid的i参数)

pid的参数在sender.cpp中define了，可调后重新编译

#version5_pid-builtin(加入pid的p和d参数)

pid的参数在sender.cpp中define了，可调后重新编译


#version4_adjust(实现降落时的调整)
## 状态
状态1： flag_landing == 0 && flag_adjust == 0 : 最开始的搜寻状态，还未下降

状态2： flag_landing == 1 && flag_adjust == 0 : 目标在中央后，开始下降

状态3： flag_landing == 1 && flag_adjust == 1 : 下降过程中，暂停下降，进行调整状态

## 下降过程中调整的初步实现思路

### 假定：
目标框边长、飞机与目标的距离 是成正比变化的，那么可以设定一个可调整的区间和偏离目标进行调整的条件进行调整。

### 比如：
假定开始下落时，飞机距离地面10米，可以当满足以下三个条件时进行调整：

1. 目标框中心偏出镜头中心1.0倍的当前目标框边长

2. 飞机低于10米（目标框边长/初始边长 > 1.0）

3. 飞机高于4米 （木边框边长/初始边长 < 2.5）

## 控制指令的指定：

状态1：(第一次降落前的调整)

[ :RC 00(高低) xx(前后) xx(左右) 00(不降落) xx(校验) / ] 

状态2：(降落)

[ :RC 00(高低) 00(前后) 00(左右) 0A(降落) xx(校验) / ] 

状态3：(第一次将落后的调整，指令和状态1一样)

[ :RC 00(高低) xx(前后) xx(左右) 00(不降落) xx(校验) / ] 

## 目标丢失的处理

状态1:悬停

[ :RC 00(高低) 00(前后) 00(左右) 00(不降落) xx(校验) / ] 

状态2:继续降落

[ :RC 00(高低) 00(前后) 00(左右) 0A(不降落) xx(校验) / ] 

状态2:悬停

[ :RC 00(高低) 00(前后) 00(左右) 00(不降落) xx(校验) / ] 


#version2_landing(实现自动降落)
#### new README by Jaycee qq 605829557 email yxzjc@126.com

###文件结构
src中:

sender.cpp是我后来加的用来生成控制字符串和发送这个字符串;

TLD.cpp和tld_run.cpp做过控制四旋翼的功能的一些修改,主要是增加了一些代码;
其他文件基本没动,是原TLD例程里的.

###编译和运行
我参照原例程文档写了自动编译和运行的脚本

####编译脚本:
在本文件夹下运行./compile.sh

####运行脚本:
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

