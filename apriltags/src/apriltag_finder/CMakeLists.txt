cmake_minimum_required(VERSION 2.8.3)
project(apriltag_finder)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  rospy
  std_msgs
  uvc_camera
  rosbag
  apriltags_ros
  image_geometry
)

find_package(OpenCV REQUIRED)

catkin_package()

include_directories(
    ${catkin_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(poses src/poses.cpp src/poses_node.cpp src/apriltag_finder/poses.h)

target_link_libraries(poses
    ${catkin_LIBRARIES}
    ${OpenCV_LIBRARIES}
)
