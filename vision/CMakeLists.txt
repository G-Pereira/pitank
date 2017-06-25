cmake_minimum_required(VERSION 2.8.3)
project(vision)

find_package(catkin REQUIRED COMPONENTS
        roscpp
        cv_bridge
        image_transport
        )

find_package(OpenCV REQUIRED)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES vision
#  CATKIN_DEPENDS roscpp
#  DEPENDS system_lib
)

include_directories(${OpenCV_INCLUDE_DIRS} ${catkin_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}_node src/vision_node.cpp)

target_link_libraries(${PROJECT_NAME}_node ${OpenCV_LIBRARIES} ${catkin_LIBRARIES})