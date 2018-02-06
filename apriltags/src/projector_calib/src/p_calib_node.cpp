#include "projector_calib/p_calib.h"
#include <ros/ros.h>

void calibrator::read_image(const sensor_msgs::CompressedImageConstPtr& msg) {
  cv::Mat image;

  try {
    image = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::MONO8)->image; // ROS image ---> OPENCV grayscale image
    imwrite("../img.png", image);
    find_map(thresh(image));
  }
  catch(cv_bridge::Exception e) {
    ROS_ERROR("Cv Bridge excpetion: %s.", e.what());
  }
}


int main(int argc, char **argv) {
  calibrator calib;
  ros::init(argc, argv, "p_calib");
  ros::NodeHandle nh;

  ros::Subscriber sub = nh.subscribe("image_raw/compressed", 1, &calibrator::read_image, &calib);
  ros::spin();

  return 0;
}
