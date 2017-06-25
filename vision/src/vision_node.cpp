#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

int main(int argc, char** argv)
{
    ros::init(argc, argv, "pitank_vision");
    ros::NodeHandle nh;
    image_transport::ImageTransport it(nh);

    image_transport::Publisher pubLeft = it.advertise("/stereo/left/image_raw", 1);
    image_transport::Publisher pubRight = it.advertise("/stereo/right/image_raw", 1);

    cv::Mat firstCameraFrame;
    cv::Mat secondCameraFrame;
    cv::VideoCapture firstCamera;
    cv::VideoCapture sencondCamera;
    firstCamera.open(1);
    sencondCamera.open(2);

    if (!firstCamera.isOpened()) {
        fprintf(stderr, "ERROR! Unable to open camera ONE\n");
        return -1;
    }
    if (!sencondCamera.isOpened()) {
        fprintf(stderr, "ERROR! Unable to open camera TWO\n");
        return -1;
    }

    sensor_msgs::ImagePtr msgLeft;
    sensor_msgs::ImagePtr msgRight;

    ros::Rate loop_rate(50);

    while (ros::ok()) {
        firstCamera.read(firstCameraFrame);
        sencondCamera.read(secondCameraFrame);
        if(!firstCameraFrame.empty() && !secondCameraFrame.empty()) {
            cv::imshow("left", firstCameraFrame);
            cv::imshow("right", secondCameraFrame);
            msgLeft = cv_bridge::CvImage(std_msgs::Header(), "bgr8", firstCameraFrame).toImageMsg();
            msgRight = cv_bridge::CvImage(std_msgs::Header(), "bgr8", secondCameraFrame).toImageMsg();
            pubLeft.publish(msgLeft);
            pubRight.publish(msgRight);
            cv::waitKey(1);
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}