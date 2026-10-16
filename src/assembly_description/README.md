# assembly_description (ROS 2 Jazzy + Gazebo Harmonic, Ubuntu 24.04)

## Install dependencies
sudo apt install ros-jazzy-ros-gz ros-jazzy-xacro ros-jazzy-robot-state-publisher \
                 ros-jazzy-joint-state-publisher-gui ros-jazzy-rviz2

## Build
cd ~/ros2_ws/src && unzip assembly_description.zip
cd ~/ros2_ws && colcon build --symlink-install && source install/setup.bash

## 1. Check the URDF (should print 8 links, base_footprint as root)
xacro src/assembly_description/urdf/assembly.urdf.xacro > /tmp/assembly.urdf
check_urdf /tmp/assembly.urdf

## 2. RViz only (wheels should touch the grid, lidar sits on top, sliders spin the wheels)
ros2 launch assembly_description display.launch.py

## 3. Gazebo Harmonic + RViz
ros2 launch assembly_description sim.launch.py
# drive:  ros2 run teleop_twist_keyboard teleop_twist_keyboard
# topics: /cmd_vel /odom /scan /imu /joint_states /tf

## Edit these numbers in urdf/assembly.urdf.xacro (top of file) after measuring the real robot
wheel_radius, wheel_x (half wheelbase), wheel_y (half track), laser_x/y/z, imu_x/y/z, masses.
Frame convention: x = forward (lidar side), y = left, z = up. Dump box is at the rear (-x).
