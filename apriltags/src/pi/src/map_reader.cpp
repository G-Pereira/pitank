#include "ros/ros.h"
#include "apriltags_ros/AprilTagDetection.h"
#include "apriltags_ros/AprilTagDetectionArray.h"
#include "geometry_msgs/PoseArray.h"
#include "geometry_msgs/Pose.h"
#include "image_geometry/pinhole_camera_model.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int image_show(const char *name){
  Mat image = imread(name, CV_LOAD_IMAGE_COLOR);

  if(!image.data ) {
      cout <<  "Could not open or find the image" << std::endl;
      return -1;
  }

  namedWindow("Display window", WINDOW_AUTOSIZE);
  imshow("Display window", image );

  waitKey();
  return 0;
}

void image_as_matrix(const char *name){
  Mat image = imread(name, CV_LOAD_IMAGE_COLOR);

  for(int i = 0; i < image.rows; i++) {
    for(int j = 0; j < image.cols; j++) {
      if(image.at<uchar>(i,j) == 0)
        printf("(%d,%d)", i, j);
    }
    printf("\n");
  }
}

void cam_to_pixel(const apriltags_ros::AprilTagDetection& point) {
  cv::Point3d p_threeD(point.pose.pose.position.x, point.pose.pose.position.y, point.pose.pose.position.z);

  image_geometry::PinholeCameraModel model_;
  cv::Point2d p_twoD = model_.project3dToPixel(p_threeD);

  ROS_INFO("x [%f]  ;  y [%f]\n", p_twoD.x, p_twoD.y);
}

void posesProcessing(const apriltags_ros::AprilTagDetectionArray& msg){

  apriltags_ros::AprilTagDetection poses;

  for(int i = 0; i < msg.detections.size(); i++){
    poses = msg.detections[i];
    ROS_INFO("x [%f]  ;  y [%f]  ;  z  [%f]\n", poses.pose.pose.position.x, poses.pose.pose.position.y, poses.pose.pose.position.z);
   // cam_to_pixel(poses);
  }

}


int main(int argc, char **argv){
  ros::init(argc, argv, "map_reader");
  ros::NodeHandle n;

  std::string map;
  cout << "Enter the name of the map" << endl;
  cin >> map;
  image_as_matrix(map.c_str());


  ros::Subscriber sub = n.subscribe("tag_detections", 1, posesProcessing);

  ros::spin();

  return 0;
}

