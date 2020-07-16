#!/bin/bash

echo "Installing packages for Robocare. You might have to enter your password for admin install access"

sudo rosdep init
rosdep update
rosdep install --from-paths src --ignore-src --rosdistro melodic --skip-keys="opencv2 opencv2-nonfree pal_laser_filters speed_limit_node sensor_to_cloud hokuyo_node libdw-dev python-graphitesend-pip python-statsd pal_filters pal_vo_server pal_usb_utils pal_pcl pal_pcl_points_throttle_and_filter pal_karto pal_local_joint_control camera_calibration_files pal_startup_msgs pal-orbbec-openni2 dummy_actuators_manager pal_local_planner gravity_compensation_controller current_limit_controller dynamic_footprint dynamixel_cpp tf_lookup opencv3"

echo "Sourcing workspace and building catkin"

source /opt/ros/melodic/setup.bash
catkin build -DCATKIN_ENABLE_TESTING=0


echo "Building complete"

echo "Moving .dae files to gazebo models repo"
mv aruco_cube_5cm ~/.gazebo/models/
mv aruco_cube_8cm ~/.gazebo/models/

sudo apt install nodejs
python -m pip install tornado
sudo apt-get install python-bson
sudo apt install libssl1.0-dev
sudo apt install nodejs-dev
sudo apt install node-gyp
sudo apt install npm
npm install -g bespoken-tools
sudo apt-get install ros-melodic-rosbridge-server


