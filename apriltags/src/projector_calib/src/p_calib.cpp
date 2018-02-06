#include "projector_calib/p_calib.h"

Mat calibrator::thresh(Mat image) {
  Mat thresh_img;
  int thresh = 100, max_val = 255, option = 1; // option = 1 -> THRESH_BINARY_INV
  threshold(image, thresh_img, thresh, max_val, option);

  try {
    imwrite("../thresh.png", thresh_img);
  }
  catch (cv_bridge::Exception ex) {
    ROS_ERROR("Could not write image. Exception: %s", ex.what());
  }

  return thresh_img;
}

Mat calibrator::find_map(Mat image) {
  Mat floodfill_img = image.clone();
  Size s = image.size();
  int filled_vals = 255;

  floodFill(floodfill_img, Point(round(s.width/2), round(s.height/2)), filled_vals);

  try {
    imwrite("../filled.png", floodfill_img);
  }
  catch (cv_bridge::Exception ex) {
    ROS_ERROR("Could not write image. Exception: %s", ex.what());
  }

  return floodfill_img;
}
