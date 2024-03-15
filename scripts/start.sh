robot_ip=192.168.31.120
curl http://${robot_ip}:8000/robot/start
roslaunch rockrobo_ros_bridge navigation.launch
