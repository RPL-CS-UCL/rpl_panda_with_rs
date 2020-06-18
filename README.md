# rpl_panda_with_rs
RPL Panda with RealSense

Clone the repository and its dependencies:

`
git clone https://github.com/rpl-as-ucl/rpl_panda_with_rs.git
git clone https://github.com/SyrianSpock/realsense_gazebo_plugin.git
git clone https://github.com/rpl-as-ucl/franka_ros.git
git clone https://github.com/rpl-as-ucl/panda_moveit_config.git
git clone https://github.com/rpl-as-ucl/panda_simulator.git
`


To run:

`
catkin build -j 1
source devel/setup.bash
roslaunch rpl_panda_with_rs display.launch
`