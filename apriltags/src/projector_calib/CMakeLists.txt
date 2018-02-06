cmake_minimum_required(VERSION 2.8.3)
project(projector_calib)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  roscpp
  std_msgs
)

find_package(OpenCV REQUIRED)

catkin_package()

include_directories(
    ${catkin_INCLUDE_DIRS}
    ${OpenCV_INCLUDE_DIRS}
)

add_executable(p_calib src/p_calib.cpp src/p_calib_node.cpp src/projector_calib/p_calib.h)

target_link_libraries(p_calib
    ${catkin_LIBRARIES}
    ${OpenCV_LIBRARIES}
)


