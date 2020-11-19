
# Qualcomm RB3 Auto Drive Application
<!-- TABLE OF CONTENTS -->
## Table of Contents

* [About the Project](#about-the-project)
* [Prerequisites](#prerequisites)
* [Getting Started](#getting-started)
  * [Steps to build Shared Library (SNPE)](#Steps-to-build-Shared-Library-(SNPE))
  * [Steps to build Docker Image](#Steps-to-build-Docker-Image)
  * [Steps to build Server Application for Amazon SageMaker Neo [Optional]:](#Steps-to-build-Server-Application-for-Amazon-SageMaker-Neo-[Optional]:)
  * [Steps to Build ROS Setup [Optional]](#Steps-to-Build-ROS-Setup-[Optional])
  * [Steps to Build Main Application](#Steps-to-Build-Main-Application)
* [Usage](#usage)
* [Disclaimer](#Disclaimer)
* [Contributors](#contributors)

<!-- ABOUT THE PROJECT -->
## About The Project

This project is intended to build and deploy an auto drive application onto Qualcomm Robotics development Kit (RB3) that detects obstacles and avoids it.
The source code is arranged in the following directories:
 * main_application : This directory contains code for the main application which act as a driver for capturing the image and depth data from TOF and decide on the movement of bot according to the information collected from other modules.
* snpe_shared_library : This directory contains the code for loading and inferring DLC model. 
* dlr_server : This directory contains code for loading and inferring DLR models.
* artifacts : This directory contains both DLR and DLC models for classification and Segmentation.
* bot_py : This directory contains python codes for controlling turtlebot.
* Dockerfile.rosdlr : Dockerfile for building docker image with ROS melodic and Amazon Neo DLR.

## Prerequisites

1. Install Android Platform tools (ADB, Fastboot) 

2. Install and configure the USB driver (Page 8 of [RB3 Platform Linux User Guide](https://www.thundercomm.com/app_en/product/1544580412842651))

3. Flash the RB3 firmware image on to the board (Page 10 of RB3 Platform Linux User Guide)

4. Setup the Wifi (Page 14 of RB3 Platform Linux User Guide)

5. Download and Install the [App Tool chain SDK](https://www.thundercomm.com/app_en/product/1544580412842651) which is present under Tools section of Technical Documents tab (Setup instruction can be found on page 57 of RB3 Platform Linux User Guide) and execute the following command.
```sh
source /usr/local/oecore-x86_64 environment-setup-armv7a-neon-oemllib32-linux-gnueabi
```

6. Setup the [SNPE SDK](https://developer.qualcomm.com/docs/snpe/setup.html) in the host system. You can download SNPE SDK [here](https://developer.qualcomm.com/downloads/qualcomm-neural-processing-sdk-ai-v143).

7. Turtlebot burger is assembled, operational and is connected to RB3

<!-- GETTING STARTED -->
## Getting Started

To get a local copy up and running follow these steps.
1. Clone the  project repository from the gitlab to host system.
```sh
git clone http://gitlab.globaledgesoft.com:81/qdn-cri/rb3-auto-drive.git
cd rb3-auto-drive
```

2. Create a directory named rb3-auto-drive in the home directory of RB3.
```sh
adb shell
mkdir /home/rb3-auto-drive
exit
```

3. Push the directories/files named dlr_ server, bot_py, Dockerfile.rosdlr and artifacts from project directory to newly created directory in RB3.
```sh
adb push artifacts /home/rb3-auto-drive
adb push bot_py /home/rb3-auto-drive
adb push dlr_server /home/rb3-auto-drive
adb push Dockerfile.rosdlr /home/rb3-auto-drive
```

### Steps to build Shared Library (SNPE):
This shared library can be cross compiled by following  steps:
1. From project directory change the directory to that of shared library source
```sh
cd snpe_shared_library
```
2. Run the Makefile (Note: Make sure  App Tool chain SDK for cross compilation and setup for SNPE SDK in the host system is ready)
```sh
make -f Makefile.arm-linux-gcc4.9sf 
```
3. Once the build is successful, libSNPESample.so will be created in obj/local/arm-linux-gcc4.9sf/. Copy the .so files to /usr/local/oecore-x86_64/sysroots/aarch64-oe-linux/lib.
```sh
sudo cp obj/local/arm-linux-gcc4.9sf/libSNPESample.so /usr/local/oecore-x86_64/sysroots/aarch64-oe-linux/lib
sudo cp $(SNPE_ROOT)/lib/arm-linux-gcc4.9sf/libSNPE.so /usr/local/oecore-x86_64/sysroots/aarch64-oe-linux/lib
```
4. Push the shared library .so file to RB3
```sh
adb push obj/local/arm-linux-gcc4.9sf/libSNPESample.so /lib
adb push $(SNPE_ROOT)/lib/arm-linux-gcc4.9sf/libSNPE.so /lib
```
5. A shared library (libgomp.so.1) is required which is first installed in the rb3 and then pulled from rb3 to be placed with the tool chain for cross compiling our shared library.
```sh
adb shell
apt-get update
apt-get install libgomp1
exit
```
Pull libgomp.so.1 from rb3
```sh
adb pull /usr/lib/libgomp.so.1
sudo mv libgomp.so.1 /usr/local/oecore-x86_64/sysroots/aarch64-oe-linux/usr/lib/
```

### Steps to build Docker Image:
1. Enter rb3 and cd to /home/rb3-auto-drive
```sh
adb shell
cd /home/rb3-auto-drive
```
2. Build the docker image from dockerfile
```sh
docker build --tag rosdlr -f Dockerfile.rosdlr .
```
3. Run the container
```sh 
docker run  -it -v /home/rb3-auto-drive:/rb3-auto-drive rosdlr
```
Set up ROS inside the container by following steps:

1. Change the directory to ~/catkin_ws/src
```sh
cd ~/catkin_ws/src
```
2. Clone necessary repositories
```sh
git clone https://github.com/ROBOTIS-GIT/turtlebot3_msgs.git
git clone https://github.com/ROBOTIS-GIT/turtlebot3.git
```
3. Change directory to ~/catkin_ws and use rosdep and catkin to build the package 
```sh
cd ~/catkin_ws
rosdep install --from-paths src --ignore-src -r -y
catkin_make
```
Set up the Amazon Sagemaker Neo inside the container by following steps:

1. Go to home directory
```sh
cd ~
```
2. Clone the repository
```sh
git clone --recursive https://github.com/neo-ai/neo-ai-dlr
```
3. Build Sagemaker Neo
```sh
cd neo-ai-dlr
mkdir build && cd build
cmake ..
make -j4
```
4. Copy the libdlr.so to /lib and set up python
```sh
cp ~/neo-ai-dlr/build/lib/libdlr.so /lib
cd ../python
python3 setup.py install --user
```
5. Compile the server source code natively inside the container
```sh
cd /rb3-auto-drive/dlr_server
g++ -o server image_resize.cpp server.cpp -I ~/neo-ai-dlr/include -ldlr
```
Save the docker container by following steps:
1. Come out of the docker container without stopping the container by giving the escape sequence
```
CTRL + P + Q
```
2. Get the container ID
```sh
docker ps
```
3. Save the container by docker commit command
```sh
docker commit  <container-ID> rosdlr:v2
```
4. Now stop the older container
```sh
docker stop <old-container-ID>
```
Note: You can skip next 2 instructions if the above steps are successful. Next 2 Instructions ie, Steps to build Server Application for Amazon SageMaker Neo and Steps to Build ROS Setup are for setting up 2 different docker for ROS and DLR.

### Steps to build Server Application for Amazon SageMaker Neo [Optional]:
1. You can get docker image tar file from dockerfiles branch of Git Repo
```sh
git checkout dockerfiles 
cd /Docker_zip_files
```
2. Push the tar file RB3
```sh
adb push dlrsetupV2.tar /data
```
3. Load the docker image in RB3
```sh
adb shell
docker load < /data/dlrsetupV2.tar
```
4. Check whether image is loaded or not by (you should see dlrsetup:v2)
```sh
docker images
```
If successful, remove the tar file from RB3 as it’s no longer required

5. Now run docker image with following command
```sh
docker run -it --name dlr  -v /home/rb3-auto-drive:/rb3-auto-drive  --network host dlrsetup:v2
```
6. Once bash of container is started, copy shared library of dlr to /lib
```sh
cp /neo-ai-dlr/build/lib/libdlr.so /lib
```
7. Compile the server source code natively inside the container
```sh
cd /rb3-auto-drive
g++ -o server image_resize.cpp server.cpp -I /neo-ai-dlr/include -ldlr
```
Note: Make sure you execute ./server from container before starting main application.

### Steps to Build ROS Setup [Optional]:
1. You can get docker image tar file from dockerfiles branch of Git Repo
```sh
git checkout dockerfiles 
cd /Docker_zip_files
```
2. Push the tar file RB3
```sh
adb push ttlbrosAppV11.tar /data
```
3. Load the docker image in RB3
```sh
adb shell
docker load < /data/ttlbrosAppV11.tar
```
4. Check whether image is loaded or not by (you should see ttlbros:v11)
```sh
docker images
```
If successful, remove the tar file from RB3 as it’s no longer required

5. Now run docker image with following command
```sh
docker run -it --name ttlb --device=/dev/ttyACM0 --network host ttlbros:v11
```
6. Once bash of container is started, start the ros master
```sh
roscore &
```
7. Now launch the turtlebot core
```sh
roslaunch turtlebot3_bringup turtlebot3_core.launch
```

### Steps to Build Main Application:
1. From project directory of host system cd to main_application
```sh
cd main_application
```
2. Run the makefile. (Note: Make sure  App Tool chain SDK for cross compilation is ready)
```sh
make
```
3. If build is successful, push the executable to /bin of RB3
```sh
adb push rb3_autodrive /bin
```
<!-- USAGE -->
## Usage
Before running the main application start the docker container containing the ROS Melodic setup and Sagemaker Neo and also run the server application for running DLR models.
Perform the following steps for starting the docker container and running the server application (If you are following single docker approach):
1. Access the RB3 through adb
```sh
adb shell
```
2. Start the docker container in RB3
```sh
docker run -it -v /home/rb3-auto-drive:/rb3-auto-drive --name rosdlr --device=/dev/ttyACM0 --network host rosdlr:v2
```
3. Set up the environment variables required for ROS
```sh
source ~/catkin_ws/devel/setup.bash
export ROS_HOSTNAME=localhost
```
4. Once bash of container is started, start the ros master
```sh
roscore &
```
5. Now launch the turtlebot core
```sh
roslaunch turtlebot3_bringup turtlebot3_core.launch &
```
6. Start the server application
```sh
cd /rb3-auto-drive/dlr_server
./server
```

You can run this application in 2 modes: SNPE and NEODLR. In SNPE mode, DLC models (classification and segmentation) will be run on SNPE. Whereas in NEODLR mode, DLR compatible models will be run on DLR runtime.
1. Using another terminal from host system access the RB3 through adb
```sh
adb shell
```
2. Run the shell script for setting the environment variables
bash
```sh
cd /opt/ros/indigo/
source ros-env.sh
```
3. To run the application in SNPE mode execute the following command in shell
```sh
rb3_autodrive SNPE
```
4. To run the application in NEODLR mode execute the following command in shell
```sh
rb3_autodrive NEODLR
```
Note: Before starting the application, make sure the docker container for ROS and the server program inside the dlr setup container are started.

<!-- Disclaimer -->
## Disclaimer
Thundercomm's RB3 Samples-apps-codes (listed as camera_test.tar under RB3 [Technical Documents](https://www.thundercomm.com/app_en/product/1544580412842651)) has been taken as reference for implementing the capturing images and depth data from TOF camera.

<!-- ## Contributors -->
## Contributors
* [Rakesh Sankar](s.rakesh@globaledgesoft.com)
* [Steven P](ss.pandiri@globaledgesoft.com)
* [Ashish Tiwari](t.ashish@globaledgesoft.com)
* [Venkata Sai Kiran J](sv.jinka@globaledgesoft.com)
* [Chaya H S](hs.chaya@globaledgesoft.com)
* [Arunraj A P](ap.arunraj@globaledgesoft.com)






