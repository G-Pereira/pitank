#ifndef P_CALIB_H
#define P_CALIB_H

#endif

#include <ros/ros.h>
#include <sensor_msgs/Image.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace cv;

class calibrator {
public:
  void read_image(const sensor_msgs::CompressedImageConstPtr& msg);
  Mat thresh(Mat image);
  Mat find_map(Mat image);
};
