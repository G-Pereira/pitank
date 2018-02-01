#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv/cv.h>
#include <cv_bridge/cv_bridge.h>
#include <image_geometry/pinhole_camera_model.h>
#include <tf/transform_listener.h>
#include <boost/foreach.hpp>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

class frame_processor {

  ros::NodeHandle nh_;
  image_transport::ImageTransport it_;
  image_transport::CameraSubscriber sub_;
  image_transport::Publisher pub_;
  tf::TransformListener tf_listener_;
  image_geometry::PinholeCameraModel cam_model_;
  std::vector<std::string> frame_ids_;

public:
  frame_processor(const std::vector<std::string>& frame_ids) : it_(nh_), frame_ids_(frame_ids) {
    sensor_msgs::CameraInfoConstPtr info_msg;
   // sub_ = it_.subscribeCamera("image_raw/compressed", 1, boost::bind(&frame_processor::imageCb, this, _1, info_msg), ros::VoidPtr(), image_transport::TransportHints::TransportHints("compressed"));
    pub_ = it_.advertise("image_out", 1);
  }

  void imageCb(const sensor_msgs::CompressedImageConstPtr& image_msg, const sensor_msgs::CameraInfoConstPtr& info_msg) {
    cv_bridge::CvImagePtr image;

    try {
      image = cv_bridge::toCvCopy(image_msg, sensor_msgs::image_encodings::BGR8);
      cv::imwrite("frame.jpg", image->image);
    }
    catch (cv_bridge::Exception& ex) {
      ROS_ERROR("Error capturing the frame");
      return;
    }

    cam_model_.fromCameraInfo(info_msg);

    pub_.publish(image->toImageMsg());
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "image_processing");
  std::vector<std::string> frame_ids(argv + 1, argv + argc);
  frame_processor drawer(frame_ids);
  ros::spin();
}

