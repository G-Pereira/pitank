cmake_minimum_required(VERSION 2.8.3)
project(devices)

find_package(catkin REQUIRED COMPONENTS roscpp)

catkin_package()

include_directories(include ${catkin_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_node src/devices_node.cpp src/deviceManager.cpp src/deviceManager.h)

target_link_libraries(${PROJECT_NAME}_node ${catkin_LIBRARIES} usb-1.0)
